#pragma once
#include "Systems/UI/UIBase.h"
#include "Loaders/Json/JsonManager.h"

class Player;
class Camera;
class ClearScreen
{
public:

	enum class YodareState {
		Hidden,
		Appearing,
		Visible,
		Disappearing
	};

	YodareState yodareState_ = YodareState::Hidden;
	float yodareTimer_ = 0.0f;
	const float yodareEaseTime_ = 0.3f;



	ClearScreen() = default;
	~ClearScreen() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	void InitJson();

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
	std::unique_ptr<UIBase> background_[2];
	uint32_t numBGs_ = 2;
};


