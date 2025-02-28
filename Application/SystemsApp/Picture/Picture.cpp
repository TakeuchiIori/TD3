#include "Picture.h"
#include <filesystem>

Picture::Picture() = default;

void Picture::Initialize()
{
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

    // 保存ディレクトリ確認
    std::wstring fullSavePath = filePath_ + L"/" + savePath_;
    EnsureDirectoryExists(fullSavePath);
}

void Picture::EnsureDirectoryExists(const std::wstring& path)
{
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
    }
}

void Picture::Update()
{
    // 写真を撮る処理を追加
    TakePicture();
}

void Picture::TakePicture()
{
    // 最大枚数チェック
    if (pictureNumber_ >= MAX_SCREENSHOTS) {
        pictureNumber_ = 0;
    }

    // スワップチェインリソースを取得
    auto swapChainResources = dxCommon_->GetSwapChainResources();
    UINT backBufferIndex = dxCommon_->GetCurrentBackBufferIndex();

    // DirectXTex を使って画像をキャプチャ（バリア遷移も含めて内部で処理）
    DirectX::ScratchImage image;
    HRESULT hr = DirectX::CaptureTexture(
        commandQueue_.Get(),
        swapChainResources[backBufferIndex].Get(),
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