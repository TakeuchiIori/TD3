#pragma once
#include "Systems/UI/UIBase.h"
#include "Loaders/Json/JsonManager.h"

class Player;
class Camera;
class CheckPoint;
class GameScreen
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



	GameScreen() = default;
	~GameScreen() = default;

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
	

	/// <summary>
	/// 制限時間の更新
	/// </summary>
	void UpdateLimit();

	/// <summary>
	/// 進んだ距離の更新
	/// </summary>
	void Updatedistance();


	/// <summary>
	/// マップ表示の更新
	/// </summary>
	void UpdateMapView();

public:

	void SetPlayer(Player* player) { player_ = player; }
	void SetCamera(Camera* camera) { camera_ = camera; }
	void SetCheckPoint(float checkPoint) { checkPointPos_ = checkPoint; }
	void SetCheckPointPos(Vector3 checkPointv) { checkPointPosv_ = checkPointv; }
	void SetCurrentMap(int num) { currentMapNum_ = num; }

private:

	Player* player_ = nullptr;
	Camera* camera_ = nullptr;
	float checkPointPos_ = 0.0f;
	Vector3 checkPointPosv_{};

	std::unique_ptr<UIBase> background_[2];
	uint32_t numBGs_ = 2;

	std::unique_ptr<UIBase> option_[6];
	uint32_t numOptions_ = 6;
	uint32_t keyBoardOption_ = 3;
	uint32_t controllerOption_ = 4;

	std::unique_ptr<UIBase> grass_[2];
	uint32_t numGrass_ = 2;
	Vector3 offset_ = Vector3(20.0f, -40.0f, 0.0f);
	Vector3 offsetGrass_ = Vector3(0.0f, 55.0f, 0.0f);

	std::unique_ptr<UIBase> baseLimit_;



	std::unique_ptr<Sprite> boost_[2];
	uint32_t numBoost_ = 2;
	Vector3 offsetB_ = Vector3(1100.0f, 200.0f, 0.0f);

	std::unique_ptr<Sprite> heart_[3];
	uint32_t numHeart_ = 3;
	Vector3 offsetH_ = Vector3(1000.0f, 310.0f, 0.0f);


	// 制限時間関連
	std::unique_ptr<Sprite> colonSprite_;
	std::array<std::unique_ptr<Sprite>, 5> timeSprites_;
	std::array<std::string, 10> digitTexturePaths_;
	std::string colonTexturePath_ = "Resources/Textures/Each_Number/a.png";

	// 進んだ距離
	std::unique_ptr<Sprite> mSprite_;
	std::array<std::unique_ptr<Sprite>, 4> ditSprites_;
	std::array<std::string, 10> disPaths_;
	std::string mTexturePath_ = "Resources/Textures/Each_Number/m.png";


	std::unique_ptr<Sprite> uiYodare_;
	Vector3 offsetYodare_ = Vector3(0.0f, 0.0f, 0.0f);

	std::unique_ptr<Sprite> uiYodareop_;
	Vector3 offsetYodareop_ = Vector3(0.0f, 0.0f, 0.0f);


	std::unique_ptr<Sprite> uiMap_;
	Vector3 offsetMapPos_ = Vector3(20.0f, 80.0f, 0.0f);
	float offsetMapScale_ = 1.5f;

	std::unique_ptr<Sprite> uiMapCurrent_;
	Vector3 offsetMapCurrentPos_ = Vector3(20.0f, 80.0f, 0.0f);
	Vector3 offsetMapCurrentScale_ = Vector3(offsetMapScale_, 1.0f, 1.0f);
	int currentMapNum_ = -1;


	std::unique_ptr<JsonManager> jsonManager_;
};

