#include "TitleScreen.h"

void TitleScreen::Initialize()
{
	///////////////////////////////////////////////////////////////////////////
	// 
	// 背景の初期化
	// 
	///////////////////////////////////////////////////////////////////////////
	background_[0] = std::make_unique<UIBase>("TitleScreen");
	background_[0]->Initialize("Resources/JSON/UI/TitleScreen.json");
	background_[1] = std::make_unique<UIBase>("TitleScreen_2");
	background_[1]->Initialize("Resources/JSON/UI/TitleScreen_2.json");
	background_[2] = std::make_unique<UIBase>("TitleScreen_3");
	background_[2]->Initialize("Resources/JSON/UI/TitleScreen_3.json");
}

void TitleScreen::Update()
{
	for (uint32_t i = 0; i < numBGs_; i++)
	{
		background_[i]->Update();
		background_[i]->ImGUi();
	}

}

void TitleScreen::Draw()
{
	for (uint32_t i = 0; i < numBGs_; i++)
	{
		background_[i]->Draw();
	}

}
