#include "Stage.h"

void Stage::Initialize(Camera* camera)
{
	camera_ = camera;
	// TODO: それぞれのステージごとの初期化を呼び出す
	checkPoint_;
	player_;
	enemyManager_;
	grassManager_;
	checkPointNum_;
}

void Stage::InitJson()
{
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
