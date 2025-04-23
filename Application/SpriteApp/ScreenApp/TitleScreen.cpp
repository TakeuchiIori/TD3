#include "TitleScreen.h"
#include "Systems/Input/Input.h"

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


	///////////////////////////////////////////////////////////////////////////
	// 
	// UIの初期化
	// 
	///////////////////////////////////////////////////////////////////////////

// キーボード用UI
	option_[0] = std::make_unique<UIBase>("KeyBoard_0");
	option_[0]->Initialize("Resources/JSON/UI/KeyBoard_0.json");
	option_[1] = std::make_unique<UIBase>("KeyBoard_1");
	option_[1]->Initialize("Resources/JSON/UI/KeyBoard_1.json");
	option_[2] = std::make_unique<UIBase>("KeyBoard_2");
	option_[2]->Initialize("Resources/JSON/UI/KeyBoard_2.json");

	// コントローラUI
	option_[3] = std::make_unique<UIBase>("Controller_3");
	option_[3]->Initialize("Resources/JSON/UI/Controller_3.json");
	option_[4] = std::make_unique<UIBase>("Controller_4");
	option_[4]->Initialize("Resources/JSON/UI/Controller_4.json");
	option_[5] = std::make_unique<UIBase>("Controller_5");
	option_[5]->Initialize("Resources/JSON/UI/Controller_5.json");


}

void TitleScreen::Update()
{
	for (uint32_t i = 0; i < numBGs_; i++)
	{
		background_[i]->Update();
	}


	///////////////////////////////////////////////////////////////////////////
	// 
	// UIの更新処理
	// 
	///////////////////////////////////////////////////////////////////////////
	for (uint32_t i = 0; i < numOptions_; i++)
	{
		option_[i]->Update();
	}

}

void TitleScreen::Draw()
{
	for (uint32_t i = 0; i < numBGs_; i++)
	{
		background_[i]->Draw();
	}

	if (Input::GetInstance()->IsControllerConnected())
	{
		for (uint32_t i = 3; i < controllerOption_; i++)
		{
			option_[i]->Draw();
		}
	} else

	{
		for (uint32_t i = 0; i < keyBoardOption_; i++)
		{
			option_[i]->Draw();
		}
	}
}
