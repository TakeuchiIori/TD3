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

	// 背景
	background_ = std::make_unique<Background>();
	background_->Initialzie();
	background_->SetColor({ 0.53f, 0.81f, 0.92f, 1.0f });

	// 気球
	balloon_ = std::make_unique<Balloon>();
	balloon_->Initialize(camera_);

	stageList_.push_back(std::make_unique<Stage>());
	stageList_[0]->SetPlayer(player_.get());
	stageList_[0]->SetGrassManager(grassManager_.get());
	stageList_[0]->SetEnemyManager(enemyManager_.get());
	stageList_[0]->SetBackground(background_.get());
	stageList_[0]->SetBalloon(balloon_.get());
	stageList_[0]->Initialize(camera_);


	ReloadObject();


	// コロンスプライトの初期化
	transSprite_ = std::make_unique<Sprite>();
	transSprite_->Initialize("Resources/Textures/In_Game/checkpointTrans.png");
	transSprite_->SetAnchorPoint({ 0.5f, 0.5f });
	transSpritePos_ = { 660,startY_,0 };
	transSprite_->SetPosition(transSpritePos_);
	transSprite_->SetSize(Vector2{ 616, 1024 });


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
	ImGui::Begin("transp");
	ImGui::DragFloat3("pos", &transSpritePos_.x, 0.1f);
	ImGui::End();
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

void StageManager::DrawBackground()
{
	stageList_[currentStageNum_]->DrawBackground();
}

void StageManager::Draw()
{
	stageList_[currentStageNum_]->Draw();
}

void StageManager::DrawAnimation()
{
	stageList_[currentStageNum_]->DrawAnimation();
}

void StageManager::DrawCollision()
{
	stageList_[currentStageNum_]->DrawCollision();
}

void StageManager::DrawSprite()
{
	stageList_[currentStageNum_]->DrawSprite();
}

void StageManager::DrawTransition()
{
	transSprite_->Draw();
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
	}

	followCamera_->SetZoom(player_->IsZeroHP(), player_->GetCenterPosition());
}

void StageManager::ReloadObject()
{
	stageList_[currentStageNum_]->ReloadObject();
}

bool StageManager::CheckPointTransition()
{
	Stage::TransitionType type = stageList_[currentStageNum_]->ReachCheckPoint();
	isTransition_ = false;
	transSprite_->Update();
	transSprite_->SetPosition(transSpritePos_);
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
			float t = transitionTimer_ / kTransitionTime_;
			transSpritePos_.y = Lerp(startY_, endY_, t);

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
