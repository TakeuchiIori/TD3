#pragma once
#include "Systems/UI/UIBase.h"

class Player;
class Camera;
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
	

	/// <summary>
	/// ゲージ比率を指定して上から下に減る表示を行う
	/// </summary>
	//void SetVerticalGaugeRatio(float ratio);


public:

	void SetPlayer(Player* player) { player_ = player; }
	void SetCamera(Camera* camera) { camera_ = camera; }

private:

	Player* player_ = nullptr;
	Camera* camera_ = nullptr;
	std::unique_ptr<UIBase> background_[2];
	uint32_t numBGs_ = 2;

	std::unique_ptr<UIBase> option_[6];
	uint32_t numOptions_ = 6;
	uint32_t keyBoardOption_ = 3;
	uint32_t controllerOption_ = 4;

	std::unique_ptr<UIBase> grass_[2];
	uint32_t numGrass_ = 2;
	Vector3 offset_ = Vector3(20.0f, -40.0f, 0.0f);

	std::unique_ptr<UIBase> baseLimit_;

	// 数字スプライト (0~9)
	std::array<std::unique_ptr<Sprite>, 10> numberSprites_;

	// コロンスプライト
	std::unique_ptr<Sprite> colonSprite_;

	// 表示専用のスプライト（5文字 = MM:SS）
	std::array<std::unique_ptr<Sprite>, 5> timeSprites_;

	std::array<std::string, 10> digitTexturePaths_;

	// コロン用テクスチャパス
	std::string colonTexturePath_ = "Resources/Textures/Each_Number/a.png";

};

