#pragma once
#include "Systems/UI/UIBase.h"

#include "Stage.h"
#include "Systems/MapChip/MapChipInfo.h"
#include "../SystemsApp/Cameras/FollowCamera/FollowCamera.h"

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

	void DrawTransition();

	bool CheckPointTransition();

private:
	void CameraScroll();

	void ReloadObject();

public:
	Player* GetPlayer() { return player_.get(); }

	void SetMapChipInfo(MapChipInfo* mapChipInfo) { mpInfo_ = mapChipInfo; }

	/// <summary>
	/// 現在のチェックポイントの座標を取得 Vector3
	/// </summary>
	Vector3 GetCheckPointPos() const { return stageList_[currentStageNum_]->GetCheckPointPos(); }

	/// <summary>
	/// 現在のチェックポイントの座標を取得 float
	/// </summary>
	float GetCheckPoint() const { return stageList_[currentStageNum_]->GetCheckPoint(); }

	bool IsClear() { return stageList_[currentStageNum_]->IsClear() || input_->TriggerKey(DIK_L); }
	void SetFollowCamera(FollowCamera* camera) { followCamera_ = camera; }


private:
	Input* input_ = nullptr;

	Camera* camera_ = nullptr;
	FollowCamera* followCamera_ = nullptr;

	std::vector<std::unique_ptr<Stage>> stageList_;
	MapChipInfo* mpInfo_ = nullptr;

	int currentStageNum_ = 0;


	std::unique_ptr<Player> player_;
	std::unique_ptr<EnemyManager> enemyManager_;
	std::unique_ptr<GrassManager> grassManager_;

	float cameraScrollStart_ = 16.0f;
	float cameraScrollEnd_ = 22.0f;
	float offsetY_ = 2.0f;

	bool isTransition_ = false; // ポイント遷移中か

	float deltaTime_ = 1.0f / 60.0f;

	float kTransitionTime_ = 2.0f;
	float transitionTimer_ = 0.0f;

	std::unique_ptr<Sprite> transSprite_;
	Vector3 transSpritePos_{};
	float startY_ = 720.0f + 512.0f;
	float endY_ = -512.0f;
};

