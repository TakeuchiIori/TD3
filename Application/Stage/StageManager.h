#pragma once

#include "Stage.h"

class StageManager
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Camera* camera);
	void InitJson();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void DrawCollision();





private:
	Camera* camera_ = nullptr;

	std::list<Stage> stageList_;
};

