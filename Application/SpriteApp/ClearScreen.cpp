#include "ClearScreen.h"
void ClearScreen::Initialize()
{	///////////////////////////////////////////////////////////////////////////
	// 
	// 背景の初期化
	// 
	///////////////////////////////////////////////////////////////////////////
	background_[0] = std::make_unique<UIBase>("GameScreen");
	background_[0]->Initialize("Resources/JSON/UI/ClearScreen.json");
	background_[0]->isDrawImGui_ = true;
	background_[1] = std::make_unique<UIBase>("GameScreen_2");
	background_[1]->Initialize("Resources/JSON/UI/ClearScreen_2.json");
	background_[1]->isDrawImGui_ = true;
}

void ClearScreen::InitJson()
{
}

void ClearScreen::Update()
{
	///////////////////////////////////////////////////////////////////////////
	// 
	// 背景の更新処理
	// 
	///////////////////////////////////////////////////////////////////////////

	for (uint32_t i = 0; i < numBGs_; i++)
	{
		background_[i]->ImGUi();
		background_[i]->Update();
	}
}

void ClearScreen::Draw()
{
	for (uint32_t i = 0; i < numBGs_; i++)
	{
		background_[i]->Draw();
	}
}