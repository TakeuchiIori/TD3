#pragma once

#include "Systems/UI/UIBase.h"

class Camera;
class TitleScreen
{
public:
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


public:
	void SetCamera(Camera* camera) { camera_ = camera; }
private:

	Camera* camera_ = nullptr;

	std::unique_ptr<UIBase> background_[3];
	uint32_t numBGs_ = 3;

	std::unique_ptr<UIBase> option_[6];
	uint32_t numOptions_ = 6;
	uint32_t keyBoardOption_ = 3;
	uint32_t controllerOption_ = 4;


};

