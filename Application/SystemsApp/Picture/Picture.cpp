#include "Picture.h"
#include <filesystem>
#include <DirectXMath.h>
#include <wincodec.h>
#include <../Core/WinApp/WinApp.h>
#include "Matrix4x4.h"
#include "Vector4.h"

Picture::Picture() = default;

void Picture::Initialize() {
    // DirectX関連インスタンスの取得
    dxCommon_ = DirectXCommon::GetInstance();
    device_ = dxCommon_->GetDevice();
    commandQueue_ = dxCommon_->GetCommandQueue();
    
    // スクリーンショット用コマンドリスト作成
    device_->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(screenshotCommandAllocator_.GetAddressOf())
    );
    
    device_->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        screenshotCommandAllocator_.Get(),
        nullptr,
        IID_PPV_ARGS(screenshotCommandList_.GetAddressOf())
    );
    
    screenshotCommandList_->Close();
    
    // カメラビュー用のレンダーターゲットとデプスバッファを作成
    {
        // レンダーターゲット作成
        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        
        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        resourceDesc.Width = WinApp::kClientWidth;
        resourceDesc.Height = WinApp::kClientHeight;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        
        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        clearValue.Color[0] = 0.0f;
        clearValue.Color[1] = 0.0f;
        clearValue.Color[2] = 0.0f;
        clearValue.Color[3] = 1.0f;
        
        device_->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            &clearValue,
            IID_PPV_ARGS(cameraRenderTarget_.GetAddressOf())
        );
        
        // RTV用ディスクリプタヒープ作成
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = 1;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        device_->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(cameraRtvHeap_.GetAddressOf()));
        
        // RTV作成
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        
        device_->CreateRenderTargetView(
            cameraRenderTarget_.Get(),
            &rtvDesc,
            cameraRtvHeap_->GetCPUDescriptorHandleForHeapStart()
        );
        
        // 深度バッファ作成
        D3D12_RESOURCE_DESC depthDesc = {};
        depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        depthDesc.Width = WinApp::kClientWidth;
        depthDesc.Height = WinApp::kClientHeight;
        depthDesc.DepthOrArraySize = 1;
        depthDesc.MipLevels = 1;
        depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        
        D3D12_CLEAR_VALUE depthClearValue = {};
        depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;
        depthClearValue.DepthStencil.Depth = 1.0f;
        
        device_->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &depthDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthClearValue,
            IID_PPV_ARGS(cameraDepthBuffer_.GetAddressOf())
        );
        
        // DSV用ディスクリプタヒープ作成
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        device_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(cameraDsvHeap_.GetAddressOf()));
        
        // DSV作成
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        
        device_->CreateDepthStencilView(
            cameraDepthBuffer_.Get(),
            &dsvDesc,
            cameraDsvHeap_->GetCPUDescriptorHandleForHeapStart()
        );
    }
    
    // 保存ディレクトリ確認・作成
    std::wstring fullSavePath = filePath_ + L"/" + savePath_;
    EnsureDirectoryExists(fullSavePath);
}

void Picture::EnsureDirectoryExists(const std::wstring& path) {
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
    }
}

void Picture::Update() {
    if (shouldTakeScreenshot_) {
        if (useCameraView_ && camera_) {
            TakeCameraViewScreenshot();
        } else {
            TakeScreenshot();
        }
        shouldTakeScreenshot_ = false;
    }
}

void Picture::TakeScreenshot() {
    // 最大枚数チェック
    if (pictureNumber_ >= MAX_SCREENSHOTS) {
        pictureNumber_ = 0;
    }
    
    // スワップチェインリソースを取得
    auto swapChainResources = dxCommon_->GetOffScreenResource();
    UINT backBufferIndex = dxCommon_->GetCurrentBackBufferIndex();
    
    // DirectXTex を使って画像をキャプチャ（バリア遷移も含めて内部で処理）
    DirectX::ScratchImage image;
    HRESULT hr = DirectX::CaptureTexture(
        commandQueue_.Get(),
        swapChainResources.Get(),
        false,
        image,
        D3D12_RESOURCE_STATE_PRESENT,    // 現在の状態
        D3D12_RESOURCE_STATE_PRESENT     // キャプチャ後に戻す状態
    );
    
    if (FAILED(hr)) {
        return;
    }
    
    // イメージデータの確認
    const DirectX::Image* images = image.GetImages();
    size_t imageCount = image.GetImageCount();
    if (!images || imageCount == 0) {
        return;
    }
    
    // PNG に保存
    std::wstringstream ss;
    ss << filePath_ << L"/"
       << savePath_ << L"/"
       << filePrefix_ << L"_"
       << std::setw(4) << std::setfill(L'0') << pictureNumber_
       << png_ext_;
    std::wstring fullPath = ss.str();
    
    hr = DirectX::SaveToWICFile(
        images,
        imageCount,
        DirectX::WIC_FLAGS_FORCE_SRGB,
        GUID_ContainerFormatPng,
        fullPath.c_str()
    );
    
    // 成功したら通し番号をインクリメント
    if (SUCCEEDED(hr)) {
        pictureNumber_++;
    }
}

void Picture::TakeCameraViewScreenshot() {
    if (!camera_) return;
    
    // 最大枚数チェック
    if (pictureNumber_ >= MAX_SCREENSHOTS) {
        pictureNumber_ = 0;
    }
    
    // コマンドアロケータとコマンドリストをリセット
    screenshotCommandAllocator_->Reset();
    screenshotCommandList_->Reset(screenshotCommandAllocator_.Get(), nullptr);
    
    // カメラの視点設定を取得
    Matrix4x4 viewMatrix = camera_->GetViewMatrix();
    Matrix4x4 projMatrix = camera_->GetProjectionMatrix();
    Matrix4x4 viewProjMatrix = viewMatrix * projMatrix;
    
    // フラスタム更新
    DirectX::XMMATRIX dxProjMatrix = ConvertToXMMATRIX(projMatrix);
    DirectX::BoundingFrustum::CreateFromMatrix(cameraFrustum_, dxProjMatrix);

    // カメラの位置と姿勢から視錐台を変換
    Matrix4x4 invViewMatrix = Inverse(viewMatrix);
    DirectX::XMMATRIX dxInvViewMatrix = ConvertToXMMATRIX(invViewMatrix);
    cameraFrustum_.Transform(cameraFrustum_, dxInvViewMatrix);
    
    // メインのレンダーターゲットからカメラビュー用レンダーターゲットへコピー
    // ※実際のゲームエンジンでは、ここでカメラの視点からシーンを再レンダリングすることも可能
    
    auto swapChainResources = dxCommon_->GetOffScreenResource();
    UINT backBufferIndex = dxCommon_->GetCurrentBackBufferIndex();
    
    // リソースバリア設定
    D3D12_RESOURCE_BARRIER barriers[2] = {};
    
    // バックバッファを読み取り用に
    barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barriers[0].Transition.pResource = swapChainResources.Get();
    barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
    barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
    
    // カメラレンダーターゲットをコピー先用に
    barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barriers[1].Transition.pResource = cameraRenderTarget_.Get();
    barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
    
    screenshotCommandList_->ResourceBarrier(2, barriers);
    
    // コピー実行
    screenshotCommandList_->CopyResource(cameraRenderTarget_.Get(), swapChainResources.Get());
    
    // リソースバリアを元に戻す
    barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
    barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
    
    screenshotCommandList_->ResourceBarrier(2, barriers);
    
    // コマンド終了
    screenshotCommandList_->Close();
    
    // コマンド実行
    ID3D12CommandList* ppCommandLists[] = { screenshotCommandList_.Get() };
    commandQueue_->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    
    // GPUの処理完了を待機
    dxCommon_->WaitForGpu();
    
    // DirectXTex を使ってカメラビューをキャプチャ
    DirectX::ScratchImage image;
    HRESULT hr = DirectX::CaptureTexture(
        commandQueue_.Get(),
        cameraRenderTarget_.Get(),
        false,
        image,
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );
    
    if (FAILED(hr)) {
        return;
    }
    
    // イメージデータの確認
    const DirectX::Image* images = image.GetImages();
    size_t imageCount = image.GetImageCount();
    if (!images || imageCount == 0) {
        return;
    }
    
    // PNG に保存
    std::wstringstream ss;
    ss << filePath_ << L"/"
       << savePath_ << L"/"
       << L"camera_view_" << filePrefix_ << L"_"
       << std::setw(4) << std::setfill(L'0') << pictureNumber_
       << png_ext_;
    std::wstring fullPath = ss.str();
    
    hr = DirectX::SaveToWICFile(
        images,
        imageCount,
        DirectX::WIC_FLAGS_FORCE_SRGB,
        GUID_ContainerFormatPng,
        fullPath.c_str()
    );
    
    // 成功したら通し番号をインクリメント
    if (SUCCEEDED(hr)) {
        pictureNumber_++;
    }
}

bool Picture::IsObjectVisible(const DirectX::XMFLOAT4X4& objectWorldMatrix, const DirectX::BoundingBox& bounds) {
    if (!camera_) return false;
    
    // オブジェクトのバウンディングボックスをワールド空間に変換
    DirectX::BoundingBox worldBounds = bounds;
    DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4(&objectWorldMatrix);
    worldBounds.Transform(worldBounds, worldMatrix);
    
    // カメラのフラスタムとの判定
    return cameraFrustum_.Intersects(worldBounds);
}