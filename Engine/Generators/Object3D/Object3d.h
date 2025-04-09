#pragma once

// C++
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>

// Engine
#include "Systems/Camera/Camera.h"
#include "Loaders/Model/Model.h"
#include "../Graphics/Culling/OcclusionCullingManager.h"
#include "Loaders/Model/Material/MaterialColor.h"


// Math
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"


class Line;
class WorldTransform;
class Object3dCommon;
class Model;
// 3Dオブジェクト
class Object3d
{
public: // メンバ関数

	Object3d();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// アニメーションの更新
	/// </summary>
	void UpdateAnimation();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(Camera* camera, WorldTransform& worldTransform);

	/// <summary>
	/// スケルトン描画
	/// </summary>
	void DrawBone(Line& line);

	/// <summary>
	/// モデルのセット
	/// </summary>
	void SetModel(const std::string& filePath, bool isAnimation = false);

	/// <summary>
	/// アニメーションを切り替える
	/// </summary>
	void SetChangeAnimation(const std::string& filePath);


private:

	/// <summary>
	/// マテリアルリソース作成
	/// </summary>
	void CreateCameraResource();

public: // アクセッサ
	Model* GetModel() { return model_; }
	void SetMaterialColor(const Vector4& color) { materialColor_->SetColor(color); }
	void SetMaterialColor(const Vector3& color) { materialColor_->SetColor(color); }
	void SetAlpha(float alpha) { materialColor_->SetAlpha(alpha); }

private:

	struct CameraForGPU {
		Vector3 worldPosition;
	};

	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;
	CameraForGPU* cameraData_ = nullptr;

	// 外部からのポインタ
	Object3dCommon* object3dCommon_ = nullptr;
	Model* model_ = nullptr;


	uint32_t queryIndex_ = 0;

	std::unique_ptr<MaterialColor> materialColor_;

	// デフォルトのモデルパス
	static const std::string defaultModelPath_;

};

