#pragma once

// C++
#include <memory>


// Engine
#include "Systems/Camera/Camera.h"


// App
#include "../SystemsApp/Cameras/StageCamera/StageCamera.h"
#include "StageObject/BaseStageObject/BaseStageObject.h"

class BaseStage
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

public:
	void SetStageName(std::string stageName) { stageName_ = stageName; }

protected:
	// ポインタ
	Camera* camera_;

	std::list<std::unique_ptr<BaseStageObject>> stageObjects_;

	std::string stageName_;

	StageCamera stageCamera_;
};

