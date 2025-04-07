#pragma once

#include "Stage.h"

class StageManager
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Camera* camera);
	void InitCollision();
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
	std::vector<std::unique_ptr<Stage>> stages_;
};

