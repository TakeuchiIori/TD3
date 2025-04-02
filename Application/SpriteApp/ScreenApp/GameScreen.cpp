#include "GameScreen.h"

void GameScreen::Initialize()
{
	// ゲーム背景
	background_ = std::make_unique<UIBase>("GameScreen");
	background_->Initialize("Resources/JSON/UI/GameScreen.json");

}

void GameScreen::Update()
{
	background_->Update();
}

void GameScreen::Draw()
{
	background_->Draw();
}
