#pragma once

// C++
#include <memory>


// Engine
#include "Systems/Camera/Camera.h"


// App
#include "StageObject/BaseStageObject.h"

class BaseStage
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(Camera* camera) { camera_ = camera; }

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
	Camera* camera_;

	std::list<std::unique_ptr<BaseStageObject>> stageObjects_;
};

