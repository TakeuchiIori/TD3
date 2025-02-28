#pragma once
#include <d3d12.h>
#include <wincodec.h>
#include <DirectXTex.h>
#include <wrl.h>
#include <sstream>
#include <iomanip>
#include "../Core/DX/DirectXCommon.h"

class Picture
{
public:
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
    /// 写真撮ります
    /// </summary>
    void TakePicture();

private:
    /// <summary>
    /// ディレクトリ確認と作成
    /// </summary>
    void EnsureDirectoryExists(const std::wstring& path);

    /// <summary>
    /// ポインタ関連
    /// </summary>
    DirectXCommon* dxCommon_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;
    //Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

    /// <summary>
    /// スクリーンショット用コマンドリスト
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> screenshotCommandAllocator_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> screenshotCommandList_;

    /// <summary>
    /// 写真管理
    /// </summary>
    int pictureNumber_ = 0;
    static constexpr int MAX_SCREENSHOTS = 9999;

    /// <summary>
    /// パス設定
    /// </summary>
    std::wstring filePath_ = L"Resources/GameAlbum";
    std::wstring savePath_ = L"images";
    std::wstring png_ext_ = L".png";
};