#pragma once
#include "Systems/UI/UIBase.h"
#include "Loaders/Json/JsonManager.h"
#include "Sprite/Sprite.h"

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

	void UpdateKirin();

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

	// スプライト関連
	std::unique_ptr<Sprite> sprite_;
	std::unique_ptr<Sprite> spriteA_;
	std::unique_ptr<JsonManager> jsonManager_;
	float spriteSlideTimer_ = 0.0f;
	const float spriteSlideTime_ = 1.0f;
	bool isSliding_ = false;
	float spriteStartX_ = 1280.0f;
	float spriteEndX_ = 640.0f;
	Vector3 offset_ = { 0.0f, 0.0f, 0.0f };

	// sin波による浮遊効果
	float sinTimer_ = 0.0f;
	const float floatSpeed_ = 3.0f;    // 浮遊速度
	const float floatAmplitude_ = 10.0f; // 浮遊幅
};