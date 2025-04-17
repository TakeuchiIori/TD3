#pragma once

// C++
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <string>
#include <dxcapi.h>
#include <vector>
#include <unordered_map>


class DirectXCommon;
class PipelineManager
{
public: 
	// ブレンドモード構造体
	enum BlendMode {
		// 通常のブレンド
		kBlendModeNormal,
		// 加算
		kBlendModeAdd,
		// 減算
		kBlendModeSubtract,
		// 乗算
		kBlendModeMultiply,
		// スクリーン
		kBlendModeScreen,

		// 利用してはいけない
		kCount0fBlendMode,
	};
	// コンストラクタとデストラクタ
	 PipelineManager() = default;
	~ PipelineManager() = default;

	static  PipelineManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	D3D12_BLEND_DESC GetBlendDesc(BlendMode _mode);
	ID3D12RootSignature* GetRootSignature(const std::string& key);
	ID3D12PipelineState* GetPipeLineStateObject(const std::string& key);

private:

	//==================================================//
	/*				各パイプラインの作成					*/
	//==================================================//


	/// <summary>
	/// スプライト用パイプライン
	/// </summary>
	void CreatePSO_Sprite();

	/// <summary>
	/// オブジェクト用のパイプライン
	/// </summary>
	void CreatePSO_Object();
	void CreatePSO_ObjectInstance();

	/// <summary>
	/// ライン用のパイプライン
	/// </summary>
	void CreatePSO_Animation();

	/// <summary>
	/// ライン用のパイプライン
	/// </summary>
	void CreatePSO_Line();

	/// <summary>
	/// パーティクル用のパイプライン
	/// </summary>
	void CreatePSO_Particle();

	ID3D12PipelineState* GetBlendModePSO(BlendMode blendMode);

	/*=================================================================

							オフスクリーン関連

	=================================================================*/
	/// <summary>
	/// オフスクリーン用のパイプライン
	/// </summary>
	//void CreatePSO_OffScreen();

	void CreatePSO_BaseOffScreen(
		const std::wstring& pixelShaderPath = L"",
		const std::string& pipelineKey = ""
	);
	void CreatePSO_Smoothing(
		const std::wstring& pixelShaderPath = L"",
		const std::string& pipelineKey = ""
	);
	void CreatePSO_Edge(
		const std::wstring& pixelShaderPath = L"",
		const std::string& pipelineKey = ""
	);
	void CreatePSO_DepthOutLine(
		const std::wstring& pixelShaderPath = L"",
		const std::string& pipelineKey = ""
	);


public:

	/// <summary>
	/// パーティクル用のInputLayoutを取得
	/// </summary>
	const D3D12_INPUT_LAYOUT_DESC& GetParticleInputLayoutDesc() const {
		return particleInputLayoutDesc_;
	}

	///// <summary>
	///// パーティクル用のVS Shaderを取得
	///// </summary>
	//IDxcBlob* GetParticleVertexShaderBlob() const {
	//	return particleVertexShaderBlob_.Get();
	//}

	///// <summary>
	///// パーティクル用のPS Shaderを取得
	///// </summary>
	//IDxcBlob* GetParticlePixelShaderBlob() const {
	//	return particlePixelShaderBlob_.Get();
	//}

	///// <summary>
	///// パーティクル用のRasterizer設定を取得
	///// </summary>
	//const D3D12_RASTERIZER_DESC& GetParticleRasterizerDesc() const {
	//	return particleRasterrizerDesc_;
	//}

	///// <summary>
	///// パーティクル用のDepthStencil設定を取得
	///// </summary>
	//const D3D12_DEPTH_STENCIL_DESC& GetParticleDepthStencilDesc() const {
	//	return particleDepthStencilDesc_;
	//}


private:
	PipelineManager(const  PipelineManager&) = delete;
	PipelineManager& operator=(const  PipelineManager&) = delete;
	PipelineManager(PipelineManager&&) = delete;
	PipelineManager& operator=(PipelineManager&&) = delete;
	DirectXCommon* dxCommon_ = nullptr;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> pipelineStates_;
	std::unordered_map<BlendMode, Microsoft::WRL::ComPtr<ID3D12PipelineState>> blendModePipelineStates_;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12RootSignature>> rootSignatures_;
	BlendMode blendMode_{};


	// パーティクルで使用
	D3D12_RASTERIZER_DESC particleRasterrizerDesc_{};
	D3D12_DEPTH_STENCIL_DESC particleDepthStencilDesc_{};
	D3D12_INPUT_LAYOUT_DESC particleInputLayoutDesc_{};
	Microsoft::WRL::ComPtr<IDxcBlob> particleVertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> particlePixelShaderBlob_;

};

