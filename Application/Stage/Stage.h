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
	std::unique_ptr<CheckPoint> checkPoint_;

	Player* player_ = nullptr;
	EnemyManager* enemyManager_ = nullptr;
	GrassManager* grassManager_ = nullptr;
};

