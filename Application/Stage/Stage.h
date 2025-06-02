#pragma once

// Engine
#include "Systems/Input/Input.h"
#include "Object3D/Object3d.h"
#include "WorldTransform/WorldTransform.h"
#include "Systems/Camera/Camera.h"

// C++
#include <vector>
#include <memory>
#include <random>

// Application
#include "Player/Player.h"
#include "EnemyManager/EnemyManager.h"
#include "Grass/GrassManager.h"
#include "CheckPoint.h"
#include "../SpriteApp/Background.h"
#include "../Objects/Balloon/Balloon.h"
#include "Cloud/Cloud.h"

class Stage
{
public:
	enum TransitionType
	{
		kNone,
		kCheckPoint,
		kStage,
		kClear,
	};
public:
	// デフォルトコンストラクタ
	Stage() = default;
	~Stage() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Camera* camera);
	void InitJson();
	void SetStageBackgroundColor();
	void InitCheckPoint();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void NotDebugCameraUpdate();

	/// <summary>
	/// 描画
	/// </summary>
	void DrawBackground();
	void Draw();
	void DrawAnimation();
	void DrawCollision();
	void DrawSprite();

	// チェックポイントにたどり着いたときの処理
	TransitionType ReachCheckPoint();

private:
	bool StageSelector(const char* label = "StageSelector");

	/// <summary>
	/// ランダムに雲を生成
	/// </summary>
	void GenerateRandomClouds();

public:
	void ReloadObject();

public:
	void SetPlayer(Player* player) { player_ = player; }
	void SetEnemyManager(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }
	void SetGrassManager(GrassManager* grassManager) { grassManager_ = grassManager; }
	void SetBackground(Background* background) { background_ = background; }
	void SetBalloon(Balloon* balloon) { balloon_ = balloon; }

	/// <summary>
	/// チェックポイントの座標を取得
	/// </summary>
	float GetCheckPoint();
	Vector3 GetCheckPointPos();

	int GetCurrentStageNum() const { return currentStageNum_; }
	bool IsClear() { return isClear_; }

	void TransitionEnd() { transitionType_ = TransitionType::kNone; }

	int GetCheckPointID();

private:
	int currentStageNum_ = 0;

	CheckPoint checkPoint_;	// チェックポイントクラス

	//std::unique_ptr<JsonManager> jsonManager_;

	Camera* camera_ = nullptr;
	Background* background_ = nullptr;
	Player* player_ = nullptr;
	EnemyManager* enemyManager_ = nullptr;
	GrassManager* grassManager_ = grassManager_;
	Balloon* balloon_ = nullptr;

	// 雲管理
	std::vector<std::unique_ptr<Cloud>> clouds_;
	std::mt19937 randomEngine_;  // ランダム生成器

	int checkPointElements_ = 0;

	int currentCheckPoint_ = 0;						// チェックポイント番号

	bool isClear_ = false;

	TransitionType transitionType_ = TransitionType::kNone;
};