#pragma once

// Engine
#include "Systems/Input/Input.h"
#include "Object3D/Object3d.h"
#include "WorldTransform/WorldTransform.h"
#include "Systems/Camera/Camera.h"


// Application
#include "Player/Player.h"
#include "EnemyManager/EnemyManager.h"
#include "Grass/GrassManager.h"
#include "CheckPoint.h"


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
	void InitCheckPoint();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void NotDebugCameraUpdate();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	void DrawAnimation();
	void DrawCollision();

	// チェックポイントにたどり着いたときの処理
	TransitionType ReachCheckPoint();

private:
	bool StageSelector(const char* label = "StageSelector");


public:
	void ReloadObject();

public:
	void SetPlayer(Player* player) { player_ = player; }
	void SetEnemyManager(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }
	void SetGrassManager(GrassManager* grassManager) { grassManager_ = grassManager; }

	/// <summary>
	/// チェックポイントの座標を取得
	/// </summary>
	float GetCheckPoint();
	Vector3 GetCheckPointPos();

	bool IsClear() { return isClear_; }

	void TransitionEnd() { transitionType_ = TransitionType::kNone; }

	int GetCheckPointID();

private:
	int currentStageNum_ = 0;

	CheckPoint checkPoint_;	// チェックポイントクラス

	//std::unique_ptr<JsonManager> jsonManager_;
	
	Camera* camera_ = nullptr;

	Player* player_ = nullptr;
	EnemyManager* enemyManager_ = nullptr;
	GrassManager* grassManager_ = nullptr;

	int checkPointElements_ = 0;

	int currentCheckPoint_ = 0;						// チェックポイント番号

	bool isClear_ = false;

	TransitionType transitionType_ = TransitionType::kNone;
};

