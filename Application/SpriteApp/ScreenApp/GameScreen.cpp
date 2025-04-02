#include "GameScreen.h"

void GameScreen::Initialize()
{
	// ゲーム背景
	background_[0] = std::make_unique<UIBase>("GameScreen");
	background_[0]->Initialize("Resources/JSON/UI/GameScreen.json");

	background_[1] = std::make_unique<UIBase>("GameScreen_2");
	background_[1]->Initialize("Resources/JSON/UI/GameScreen_2.json");

}

void GameScreen::Update()
{
	background_[0]->Update();
	background_[1]->Update();
}

void GameScreen::Draw()
{
	background_[0]->Draw();
	background_[1]->Draw();
}
