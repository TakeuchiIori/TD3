#include "Stage.h"

void Stage::Initialize(Camera* camera)
{
	(void)camera;
	// TODO: それぞれのステージごとの初期化を呼び出す
	checkPoint_;
	player_;
	enemyManager_;
	grassManager_;
	currentCheckPoint_;
}

void Stage::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>(stageName_, "Resources/JSON/");
	jsonManager_->SetCategory("Stages");
	/*jsonManager_->Register("草の取得数", &grassGauge_);
	jsonManager_->Register("草の最大数", &MaxGrass_);*/
}

void Stage::Update()
{
	player_->Update();
	enemyManager_->Update();
	grassManager_->Update();
}

void Stage::Draw()
{
	player_->Draw();
	enemyManager_->Draw();
	grassManager_->Draw();
}

void Stage::DrawCollision()
{
	player_->DrawCollision();
	grassManager_->DrawCollision();
}

void Stage::ReachCheckPoint()
{
}
