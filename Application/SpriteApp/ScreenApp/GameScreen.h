#pragma once

#include "Systems/UI/UIBase.h"
class GameScreen
{
public:
	GameScreen() = default;
	~GameScreen() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	

private:

	std::unique_ptr<UIBase> background_[2];
	uint32_t numBGs_ = 2;

	std::unique_ptr<UIBase> option_[6];
	uint32_t numOptions_ = 6;
	uint32_t keyBoardOption_ = 3;
	uint32_t controllerOption_ = 4;
};

