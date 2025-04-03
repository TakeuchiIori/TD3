#include "GameScreen.h"
#include "Systems/Input/Input.h"
#include "./Player/Player.h"
#include "Systems/Camera/Camera.h"
#include "../Core/WinApp/WinApp.h"
// Math 
#include "MathFunc.h"
#include "Matrix4x4.h"

void GameScreen::Initialize()
{
	///////////////////////////////////////////////////////////////////////////
	// 
	// 背景の初期化
	// 
	///////////////////////////////////////////////////////////////////////////
	background_[0] = std::make_unique<UIBase>("GameScreen");
	background_[0]->Initialize("Resources/JSON/UI/GameScreen.json");
	background_[1] = std::make_unique<UIBase>("GameScreen_2");
	background_[1]->Initialize("Resources/JSON/UI/GameScreen_2.json");


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

	grass_[0] = std::make_unique<UIBase>("Grass_0");
	grass_[0]->Initialize("Resources/JSON/UI/Grass_0.json");
	grass_[1] = std::make_unique<UIBase>("Grass_1");
	grass_[1]->Initialize("Resources/JSON/UI/Grass_1.json");

	//grass_[0]->SetCamera(camera_);
	//grass_[1]->SetCamera(camera_);


	baseLimit_ = std::make_unique<UIBase>("BaseLimit");
	baseLimit_->Initialize("Resources/JSON/UI/BaseLimit.json");

	// GameScreen::Initialize 内

	for (int i = 0; i < 4; i++) {
		limitNum_[i] = new UIBase("LimitNum_" + std::to_string(i));
		limitNum_[i]->Initialize("Resources/JSON/UI/LimitNum_" + std::to_string(i) + ".json");
	}
}

void GameScreen::Update()
{
	///////////////////////////////////////////////////////////////////////////
	// 
	// 背景の更新処理
	// 
	///////////////////////////////////////////////////////////////////////////

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

	///////////////////////////////////////////////////////////////////////////
	// 
	// 草のUIの更新処理
	// 
	///////////////////////////////////////////////////////////////////////////
	for (UINT32 i = 0; i < numGrass_; i++)
	{
		Vector3 playerPos = player_->GetWorldTransform().translation_;
		Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1);
		Matrix4x4 matViewProjectionViewport = Multiply(camera_->GetViewMatrix(), Multiply(camera_->GetProjectionMatrix(), matViewport));
		playerPos = Transform(playerPos, matViewProjectionViewport);
		playerPos += offset_;
		grass_[i]->SetPosition(playerPos);

		if (i == 1) {
			float ratio = static_cast<float>(player_->GetGrassGauge());
			grass_[1]->SetVerticalGaugeRatio(ratio);
		}

		grass_[i]->Update();
	}


	baseLimit_->Update();

	for (uint32_t i = 0; i < 4; i++)
	{
		limitNum_[i]->Update();
	}

}

void GameScreen::Draw()
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

	for (uint32_t i = 0; i < numGrass_; i++)
	{
		grass_[i]->Draw();
	}


	baseLimit_->Draw();

	for (uint32_t i = 0; i < 4; i++)
	{
		limitNum_[i]->Draw();
	}

}

