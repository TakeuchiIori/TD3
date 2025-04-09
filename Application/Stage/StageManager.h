#pragma once

#include "Stage.h"
#include "Systems/MapChip/MapChipInfo.h"

class StageManager
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

	void NotDebugCameraUpdate();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void DrawCollision();

private:
	void ReloadObject();

public:
	Player* GetPlayer() { return player_.get(); }

	void SetMapChipInfo(MapChipInfo* mapChipInfo) { mpInfo_ = mapChipInfo; }

private:
	Input* input_ = nullptr;

	Camera* camera_ = nullptr;

	std::vector<std::unique_ptr<Stage>> stageList_;
	MapChipInfo* mpInfo_ = nullptr;

	int currentStageNum_ = 0;


	std::unique_ptr<Player> player_;
	//std::unique_ptr<EnemyManager> enemyManager_;
	std::unique_ptr<GrassManager> grassManager_;
};

