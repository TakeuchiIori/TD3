#include "TitleScreen.h"
#include "Systems/Input/Input.h"
#include "Systems/GameTime/GameTime.h"
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


	title_ = std::make_unique<Sprite>();
	title_->Initialize("Resources/Textures/BackGround/title.png");
	title_->SetAnchorPoint({ 0.5f, 0.5f });


	ctrl_ = std::make_unique<Sprite>();
	ctrl_->Initialize("Resources/Textures/BackGround/controller.png");
	ctrl_->SetAnchorPoint({ 0.5f, 0.5f });

	InitJson();
}

void TitleScreen::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("TitleScene", "Resources/JSON/UI/");
	jsonManager_->SetCategory("UI");
	jsonManager_->Register("OffsetTitle", &offset[0]);
	jsonManager_->Register("OffsetCtrl", &offsetCtrl_);

}

void TitleScreen::Update()
{
	for (uint32_t i = 0; i < numBGs_; i++)
	{
		background_[i]->Update();
		if (i != 2) {
			//background_[i]->ImGUi();
		}
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
	ctrl_->SetPosition(offsetCtrl_);
	ctrl_->Update();

	Vector2 stickInput = Input::GetInstance()->GetLeftStickInput(0);
	if (!isFadingTitle_ && (std::abs(stickInput.x) > 0.1f || std::abs(stickInput.y) > 0.1f)) {
		isFadingTitle_ = true;
	}
	if (Input::GetInstance()->IsPadPressed(0, GamePadButton::A) || Input::GetInstance()->IsAnyKeyPressed()) {
		isFadingTitle_ = true;
	}		

	if (isFadingTitle_) {
		titleAlpha_ -= 0.02f; // 徐々に透明に（速度は調整OK）
		titleAlpha_ = std::clamp(titleAlpha_, 0.0f, 1.0f);

		title_->SetColor({ 1.0f, 1.0f, 1.0f, titleAlpha_ }); // RGBA
	}


	// Update内の最後に追加
	titleAnimTimer_ += 0.016f; // deltaTime相当、適宜調整

	float bobbing = std::sin(titleAnimTimer_ * 2.0f) * 5.0f; // 揺れの強さ調整
	Vector3 pos = offset[0];
	pos.y += bobbing;
	


	title_->SetPosition(pos);
	title_->Update();


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
			//option_[i]->Draw();
		}
	} else

	{
		for (uint32_t i = 0; i < keyBoardOption_; i++)
		{
			//option_[i]->Draw();
		}
	}

	ctrl_->Draw();

	title_->Draw();
}
