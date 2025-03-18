#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <DirectXTex.h>
#include <DirectXCollision.h>
#include "../Core/DX/DirectXCommon.h"
#include "Systems/Camera/Camera.h"

/// <summary>
/// カメラシステムと連携したスクリーンショット機能
/// </summary>
class Picture {
public:
    // 最大保存枚数
    static const uint32_t MAX_SCREENSHOTS = 9999;

    /// <summary>
    /// コンストラクタ
    /// </summary>
    Picture();

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize();

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    /// <summary>
    /// カメラのセット
    /// </summary>
    /// <param name="camera">カメラポインタ</param>
    void SetCamera(Camera* camera) { camera_ = camera; }

    /// <summary>
    /// スクリーンショットの撮影トリガー
    /// </summary>
    /// <param name="takeCameraView">trueの場合カメラ視点、falseの場合通常のスクリーンショット</param>
    void TriggerScreenshot(bool takeCameraView = false) {
        shouldTakeScreenshot_ = true;
        useCameraView_ = takeCameraView;
    }

    /// <summary>
    /// 保存先パスの設定
    /// </summary>
    /// <param name="path">保存先パス</param>
    void SetSavePath(const std::wstring& path) { savePath_ = path; }

    /// <summary>
    /// ファイル名のプレフィックスを設定
    /// </summary>
    /// <param name="prefix">ファイル名のプレフィックス</param>
    void SetFilePrefix(const std::wstring& prefix) { filePrefix_ = prefix; }

private:

    // 保存先ディレクトリの確認・作成
    void EnsureDirectoryExists(const std::wstring& path);

    // カメラビューのスクリーンショットを撮影
    void TakeCameraViewScreenshot();

    // オブジェクト可視性チェック用
    bool IsObjectVisible(const DirectX::XMFLOAT4X4& objectWorldMatrix, const DirectX::BoundingBox& bounds);

private:
    // DirectX関連
    DirectXCommon* dxCommon_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> screenshotCommandAllocator_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> screenshotCommandList_;

    // カメラ参照
    Camera* camera_ = nullptr;

    // カメラビュー用リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> cameraRenderTarget_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cameraRtvHeap_;
    Microsoft::WRL::ComPtr<ID3D12Resource> cameraDepthBuffer_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cameraDsvHeap_;

    // ファイルパス関連
    std::wstring filePath_ = L".";  // 基本ディレクトリ
    std::wstring savePath_ = L"Resources/Photos/";  // 保存ディレクトリ
    std::wstring filePrefix_ = L"";  // ファイル名プレフィックス
    std::wstring png_ext_ = L".png";  // 拡張子

    // スクリーンショット管理
    uint32_t pictureNumber_ = 0;  // 通し番号
    bool shouldTakeScreenshot_ = false;  // スクリーンショット撮影フラグ
    bool useCameraView_ = false;  // カメラビュー使用フラグ

    // カメラフラスタム更新用
    DirectX::BoundingFrustum cameraFrustum_;
};