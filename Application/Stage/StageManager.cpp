#include "StageManager.h"

void StageManager::Initialize(Camera* camera)
{
	input_ = Input::GetInstance();

	camera_ = camera;
	//std::unique_ptr<Stage> s = std::make_unique<Stage>();
	//s->Initialize(camera_);

	player_ = std::make_unique<Player>(mpInfo_->GetMapChipField());
	player_->Initialize(camera_);

	// 草
	grassManager_ = std::make_unique<GrassManager>();
	grassManager_->SetPlayer(player_.get());
	grassManager_->Initialize(camera_);

	// 敵
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->SetPlayer(player_.get());
	enemyManager_->Initialize(camera_, mpInfo_->GetMapChipField());

	stageList_.push_back(std::make_unique<Stage>());
	stageList_[0]->SetPlayer(player_.get());
	stageList_[0]->SetGrassManager(grassManager_.get());
	stageList_[0]->SetEnemyManager(enemyManager_.get());
	stageList_[0]->Initialize(camera_);


	ReloadObject();
}

void StageManager::InitJson()
{
}

void StageManager::Update()
{
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_V))
	{
		ReloadObject();
	}
#endif // _DEBUG

	
	stageList_[currentStageNum_]->Update();

	CameraScroll();

	if (player_->EndReturn())
	{
		grassManager_->Repop();
	}
}

void StageManager::NotDebugCameraUpdate()
{
	stageList_[currentStageNum_]->NotDebugCameraUpdate();
}

void StageManager::Draw()
{
	stageList_[currentStageNum_]->Draw();
}

void StageManager::DrawCollision()
{
	stageList_[currentStageNum_]->DrawCollision();
}

void StageManager::CameraScroll()
{
	if (player_->GetCenterPosition().y < cameraScrollStart_)
	{
		followCamera_->SetOffsetY(cameraScrollStart_ + offsetY_ - player_->GetCenterPosition().y);
	}
	else if (cameraScrollEnd_ + player_->GetCenterPosition().y >= stageList_[currentStageNum_]->GetCheckPoint())
	{
		float offset = (cameraScrollEnd_ - offsetY_ + player_->GetCenterPosition().y) - stageList_[currentStageNum_]->GetCheckPoint();
		followCamera_->SetOffsetY(-offset);
#ifdef _DEBUG
		/*ImGui::Begin("Scroll");
		ImGui::Text("%.2f", offset);
		ImGui::End();*/
#endif // _DEBUG
	}
}

void StageManager::ReloadObject()
{
	stageList_[currentStageNum_]->ReloadObject();
}

bool StageManager::CheckPointTransition()
{
	Stage::TransitionType type = stageList_[currentStageNum_]->ReachCheckPoint();
	isTransition_ = false;
	switch (type)
	{
	case Stage::TransitionType::kNone:
		break;
	case Stage::TransitionType::kCheckPoint:
	case Stage::TransitionType::kStage:
		if (transitionTimer_ < kTransitionTime_)
		{
			float prevTimer = transitionTimer_; // 前回の値を保持
			float halfTransTime = kTransitionTime_ / 2.0f; // ちょうど真ん中

			transitionTimer_ += deltaTime_;

			if (prevTimer < halfTransTime && transitionTimer_ >= halfTransTime)
			{
				stageList_[currentStageNum_]->InitCheckPoint();
				// 2フレーム分更新処理を入れないと描画がおかしくなる
				Update();
				NotDebugCameraUpdate();
				Update();
				NotDebugCameraUpdate();
			}
		}
		else
		{
			transitionTimer_ = 0;
			stageList_[currentStageNum_]->TransitionEnd();
		}


		isTransition_ = true;
		break;
	case Stage::TransitionType::kClear:
		break;
	}
	return isTransition_;
}
