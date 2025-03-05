#pragma once

// Engine
#include "Object3D/Object3d.h"
#include "WorldTransform/WorldTransform.h"
#include "Systems/Camera/Camera.h"

class BaseObject
{
public:
	virtual ~BaseObject() {};
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(Camera* camera) = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

protected:
	// ポインタ
	std::unique_ptr<Object3d> obj_;
	Camera* camera_;
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
};

