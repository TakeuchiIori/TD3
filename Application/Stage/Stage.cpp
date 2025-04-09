#include "Stage.h"
#include <string>
#include "StageEditor.h"

void Stage::Initialize(Camera* camera)
{
	camera_ = camera;
	// TODO: それぞれのステージごとの初期化を呼び出す
	checkPoint_.Initialize(camera_);
	//enemyManager_;
	currentCheckPoint_;

	InitJson();
	InitCheckPoint();
}

void Stage::InitJson()
{
}

void Stage::InitCheckPoint()
{
}

void Stage::Update()
{
	player_->Update();
	//enemyManager_->Update();
	grassManager_->Update();
	//ReachCheckPoint();
}

void Stage::Draw()
{
	player_->Draw();
	//enemyManager_->Draw();
	grassManager_->Draw();
}

void Stage::DrawCollision()
{
	player_->DrawCollision();
	grassManager_->DrawCollision();
}

void Stage::ReachCheckPoint()
{
	/*if (player_->GetCenterPosition().y >= *checkPoint_.GetCheckPointHight())
	{
		currentCheckPoint_++;
		Vector3 pos = player_->GetCenterPosition();
		player_->SetPos({ pos.x, 2.0f, pos.z });
	}*/
}

void Stage::ReloadObject()
{
	const std::vector<PlacedObject>* obj = StageEditor::Instance()->GetObjects(currentStageNum_, currentCheckPoint_);

	if (obj)
	{
		grassManager_->ClearGrass();
		for (const auto& o : *obj) 
		{
			grassManager_->PopGrass(o.position);
		}
	}

	const float* hight = StageEditor::Instance()->GetCheckHight(currentStageNum_, currentCheckPoint_);

	if (hight)
	{
		checkPoint_.SetCheckPointHight(*hight);
	}
}
