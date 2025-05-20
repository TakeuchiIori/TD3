#pragma once

// C++
#include <wrl.h>
#include <d3d12.h>
#include <unordered_map>

#include "Matrix4x4.h"


// Math
#include "Vector2.h"
#include "Vector4.h"

/// <summary>
/// オフスクリーン描画タイプ
/// </summary>


class DirectXCommon;
class OffScreen
{
public:

	enum class OffScreenEffectType {
		Copy,
		GaussSmoothing,
		DepthOutline,
		Sepia,
		Grayscale,
		Vignette,
		RadialBlur
	};

	/// <summary>
	/// シングルトンインスタンス取得
	/// </summary>
	static OffScreen* GetInstance() {
		static OffScreen instance;
		return &instance;
	}

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 逆行列のセット
	/// </summary>
	/// <param name="projectionMatrix"></param>
	void SetProjection(Matrix4x4 projectionMatrix) { projectionInverse_ = projectionMatrix; }

	/// <summary>
	/// エフェクトの種類を設定する
	/// </summary>
	/// <param name="type">エフェクトの種類</param>
	void SetEffectType(OffScreenEffectType type) {effectType_ = type;}

	/// <summary>
	/// 現在のエフェクトタイプを取得
	/// </summary>
	OffScreenEffectType GetEffectType() const { return effectType_; }



private:
	OffScreen() = default;
	~OffScreen() = default;
	OffScreen(const OffScreen&) = delete;
	OffScreen& operator=(const OffScreen&) = delete;

	void CreateBoxFilterResource();
	void CreateGaussFilterResource();
	void CreateMaterialResource();
	void CreateRadialBlurResource();

private:

	/*=================================================================

							 　リソース管理

	=================================================================*/

	struct KernelForGPU {
		int kernelSize;
	};
	struct GaussKernelForGPU {
		int kernelSize;
		float sigma;
	};
	struct Material {
		Matrix4x4 Inverse;
		int kernelSize;
		int padding[3];
		Vector4 outlineColor;
	};
	struct RadialBlurForGPU {
		Vector2 direction;
		Vector2 center;
		float width;
		int sampleCount;
		bool isRadial;
		float padding[1];
	};

	DirectXCommon* dxCommon_ = nullptr;

	struct OffScreenPipeline {
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	};
	std::unordered_map<OffScreenEffectType, OffScreenPipeline> pipelineMap_;


	// ぼかし用
	Microsoft::WRL::ComPtr<ID3D12Resource> boxResource_;
	KernelForGPU* boxData_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> gaussResource_;
	GaussKernelForGPU* gaussData_ = nullptr;

	// デプスアウトライン用
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Material* materialData_ = nullptr;
	Matrix4x4 projectionInverse_;

	// ラジアルブラー用
	Microsoft::WRL::ComPtr<ID3D12Resource> radialBlurResource_;
	RadialBlurForGPU* radialBlurData_ = nullptr;

	OffScreenEffectType effectType_ = OffScreenEffectType::Copy;


public:

	/*=================================================================

						 　	実際にゲームで扱うもの

	=================================================================*/	

	struct RadialBlurPrams
	{
		Vector2 direction;
		Vector2 center;
		float width;
		int sampleCount;
		bool isRadial;
	};


	/// <summary>
	/// ブラーの更新
	/// </summary>
	/// <param name="deltaTime"></param>
	void UpdateBlur(float deltaTime);


	// --- 追加メソッド ---
	/// <summary>
	/// ブラー演出を開始する（最初強→最後弱）
	/// </summary>
	void StartBlurMotion(RadialBlurPrams radialBlurPrams);


private:

	RadialBlurPrams radialBlurPrams_;
	bool isBlurMotion_ = false;
	float blurTime_ = 0.0f;
	float blurDuration_ = 1.0f;		// ブラー時間（秒）
	float initialWidth_ = 0.01f;	// ブラー初期幅
	int initialSampleCount_ = 10;


};

