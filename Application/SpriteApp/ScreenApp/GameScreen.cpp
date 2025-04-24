#include "GameScreen.h"
#include "Systems/Input/Input.h"
#include "./Player/Player.h"
#include "Systems/Camera/Camera.h"
#include "../Core/WinApp/WinApp.h"

// Math 
#include "MathFunc.h"
#include "Matrix4x4.h"
#include <Systems/GameTime/GameTIme.h>
#include "Easing.h"

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

	// ImGuiの描画やめる
	for (uint32_t i = 0; i < numOptions_; i++)
	{
		option_[i]->isDrawImGui_ = false;
	}

	///////////////////////////////////////////////////////////////////////////
	// 
	// 草の初期化
	// 
	///////////////////////////////////////////////////////////////////////////
	grass_[0] = std::make_unique<UIBase>("Grass_0");
	grass_[0]->Initialize("Resources/JSON/UI/Grass_0.json");
	grass_[1] = std::make_unique<UIBase>("Grass_1");
	grass_[1]->Initialize("Resources/JSON/UI/Grass_1.json");

	// ImGuiの描画やめる
	for (uint32_t i = 0; i < 2; i++) {
		grass_[i]->isDrawImGui_ = false;
	}

	///////////////////////////////////////////////////////////////////////////
	// 
	// ブーストの初期化
	// 
	///////////////////////////////////////////////////////////////////////////
	boost_[0] = std::make_unique<Sprite>();
	boost_[0]->Initialize("Resources/Textures/In_Game/dashGauge_base.png");
	boost_[0]->SetAnchorPoint({ 1.0f, 0.0f });
	boost_[0]->SetSize({ 100.0f, 100.0f });
	boost_[1] = std::make_unique<Sprite>();
	boost_[1]->Initialize("Resources/Textures/In_Game/dashGauge_fill.png");
	boost_[1]->SetAnchorPoint({ 1.0f, 0.0f });
	boost_[1]->SetSize({ 100.0f, 100.0f });
	
	///////////////////////////////////////////////////////////////////////////
	// 
	// 制限時間の初期化
	// 
	///////////////////////////////////////////////////////////////////////////

	baseLimit_ = std::make_unique<UIBase>("BaseLimit");
	baseLimit_->Initialize("Resources/JSON/UI/BaseLimit.json");
	for (int i = 0; i < 10; ++i) {
		digitTexturePaths_[i] = "Resources/Textures/Each_Number/" + std::to_string(i) + ".png";
	}
	// コロンスプライトの初期化
	colonSprite_ = std::make_unique<Sprite>();
	colonSprite_->Initialize("Resources/Textures/Each_Number/a.png");  // "a.png" = コロン
	colonSprite_->SetAnchorPoint({ 0.0f, 0.0f });

	// 表示用スプライト5個分を生成（00:00）
	for (int i = 0; i < 5; ++i) {
		timeSprites_[i] = std::make_unique<Sprite>();
		// 初期は全部0.pngで初期化しておく（あとで切り替え）
		timeSprites_[i]->Initialize("Resources/Textures/Each_Number/0.png");
		timeSprites_[i]->SetAnchorPoint({ 0.5f, 0.5f });
		timeSprites_[i]->SetSize({ 60.0f, 80.0f });
	}


	///////////////////////////////////////////////////////////////////////////
	// 
	// 進んだ距離のスプライト初期化
	// 
	///////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < 10; ++i) {
		disPaths_[i] = "Resources/Textures/Each_Number/distance_" + std::to_string(i) + ".png";
	}
	mSprite_ = std::make_unique<Sprite>();
	mSprite_->Initialize("Resources/Textures/Each_Number/m.png");
	mSprite_->SetAnchorPoint({ 0.0f, 0.0f });

	// 表示用スプライト5個分を生成（00:00）
	for (int i = 0; i < 4; ++i) {
		ditSprites_[i] = std::make_unique<Sprite>();
		ditSprites_[i]->Initialize("Resources/Textures/Each_Number/distance_0.png");
		ditSprites_[i]->SetAnchorPoint({ 0.5f, 0.5f });
		ditSprites_[i]->SetSize({ 30.0f, 40.0f });
		ditSprites_[i]->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	}


	uiYodare_ = std::make_unique<Sprite>();
	uiYodare_->Initialize("Resources/Textures/Option/operation_yodare.png");
	uiYodare_->SetSize({ 60.0f, 60.0f });
	uiYodare_->SetAnchorPoint({ 0.5f, 0.5f });

	uiYodareop_ = std::make_unique<Sprite>();
	uiYodareop_->Initialize("Resources/Textures/Option/yodareUI.png");
	uiYodareop_->SetSize({ 120.0f, 120.0f });

	InitJson();

}

void GameScreen::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("YodareUI", "Resources/JSON/UI");
	jsonManager_->SetCategory("UI");
	jsonManager_->Register("OffsetYodare", &offsetYodare_);
	jsonManager_->Register("OffsetGrass", &offsetGrass_);
	jsonManager_->Register("Offset", &offset_);
	jsonManager_->Register("offsetYodareop_", &offsetYodareop_);
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
		grass_[0]->SetPosition(playerPos);
		//grass_[1]->SetPosition(playerPos + offsetGrass_);

		if (i == 1) {
			float ratio = player_->GetUIGrassGauge();


			Vector2 baseSize = { 55.0f, 55.0f }; // 草画像の本来のサイズ

			// 補正用オフセット計算（比率1.0なら0、0.5なら半分ズレる）
			float yOffset = baseSize.y * (1.0f - ratio);
			Vector3 position = playerPos + offsetGrass_;
			//position.y += yOffset;

			grass_[1]->SetPosition(position);
			// UV設定（比率に応じて縦方向に縮小 & 下から満ちる）
			grass_[1]->SetScale({ baseSize.x, baseSize.y * ratio });
			grass_[1]->SetUVScale({ 1.0f, ratio });
			grass_[1]->SetUVTranslate({ 0.0f, 1.0f - ratio });
		}
		grass_[i]->Update();

	}
	for (uint32_t i = 0; i < 2; i++) {
		grass_[i]->ImGUi();
	}
	///////////////////////////////////////////////////////////////////////////
	// 
	// ブーストのUIの更新処理
	// 
	///////////////////////////////////////////////////////////////////////////
	for (UINT32 i = 0; i < numBoost_; i++)
	{
		Vector3 playerPos = offsetB_;
		boost_[0]->SetPosition(playerPos);
		//grass_[1]->SetPosition(playerPos + offsetGrass_);

		if (i == 1) {
			float ratio = player_->GetUIBoostGauge();


			Vector2 baseSize = { 100.0f, 100.0f }; // ブースト画像の本来のサイズ

			// 補正用オフセット計算（比率1.0なら0、0.5なら半分ズレる）
			playerPos.y += baseSize.y * (1.0f - ratio);
			Vector3 position = playerPos;
			//position.y += yOffset;

			boost_[1]->SetPosition(position);
			// UV設定（比率に応じて縦方向に縮小 & 下から満ちる）
			boost_[1]->SetSize({ baseSize.x, baseSize.y * ratio });
			boost_[1]->SetUVScale({ 1.0f, ratio });
			boost_[1]->SetUVTranslate({ 0.0f, 1.0f - ratio });
		}
		boost_[i]->Update();

	}

	///////////////////////////////////////////////////////////////////////////
	// 
	// 制限時間の更新処理
	// 
	///////////////////////////////////////////////////////////////////////////
	baseLimit_->Update();
	UpdateLimit();

	///////////////////////////////////////////////////////////////////////////
	// 
	// ヨダレの更新処理
	// 
	///////////////////////////////////////////////////////////////////////////


	bool canSpit = player_->CanSpitting();
	float deltaTime_ = GameTime::GetDeltaTime();

	// 状態遷移判定
	if (canSpit && yodareState_ == YodareState::Hidden) {
		yodareState_ = YodareState::Appearing;
		yodareTimer_ = 0.0f;
	} else if (!canSpit && yodareState_ == YodareState::Visible) {
		yodareState_ = YodareState::Disappearing;
		yodareTimer_ = 0.0f;
	}

	// イージング処理
	if (yodareState_ == YodareState::Appearing) {
		yodareTimer_ += deltaTime_;
		float t = std::min(yodareTimer_ / yodareEaseTime_, 1.0f);
		t = Easing::easeOutQuad(t);
		uiYodare_->SetSize({ t * 125.0f, t * 100.0f });

		if (yodareTimer_ >= yodareEaseTime_) {
			yodareState_ = YodareState::Visible;
		}
	} else if (yodareState_ == YodareState::Disappearing) {
		yodareTimer_ += deltaTime_;
		float t = 1.0f - std::min(yodareTimer_ / yodareEaseTime_, 1.0f);
		t = Easing::easeInQuad(t);
		uiYodare_->SetSize({ t * 125.0f, t * 100.0f });

		if (yodareTimer_ >= yodareEaseTime_) {
			yodareState_ = YodareState::Hidden;
		}
	} else if (yodareState_ == YodareState::Visible) {
		// スケールを周期的に変化させて目立たせる
		float pulse = std::sin(static_cast<float>(GameTime::GetTotalTime()) * 5.0f) * 0.1f; // 鼓動みたいな動き
		Vector2 baseScale = { 125.0f,100.0f };
		Vector2 scale = baseScale * (1.0f + pulse);
		uiYodare_->SetSize({ scale.x, scale.y });
	}

	if (yodareState_ != YodareState::Hidden) {
		Vector3 playerPos = player_->GetWorldTransform().translation_;
		Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1);
		Matrix4x4 matViewProjectionViewport = Multiply(camera_->GetViewMatrix(), Multiply(camera_->GetProjectionMatrix(), matViewport));
		playerPos = Transform(playerPos, matViewProjectionViewport);
		playerPos += offsetYodare_;
		uiYodare_->SetPosition(playerPos);
		uiYodare_->Update();
	}

	uiYodareop_->SetPosition(offsetYodareop_);
	uiYodareop_->Update();




	Updatedistance();
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


	boost_[1]->Draw();
	boost_[0]->Draw();


	baseLimit_->Draw();

	for (const auto& sprite : timeSprites_) {
		sprite->Draw();
	}

	for (const auto& sprite : ditSprites_) {
		sprite->Draw();
	}

	if (yodareState_ != YodareState::Hidden) {
		uiYodare_->Draw();
	}

	uiYodareop_->Draw();

}

void GameScreen::UpdateLimit()
{
	// 時間をfloatで取得（例：9.83）
	float time = player_->GetTimeLimit();
	if (time > 10.0f) time = 10.0f;  // 最大10秒

	int seconds = static_cast<int>(time);          // 整数部（9）
	int fraction = static_cast<int>(time * 100) % 100; // 小数部2桁（83）

	// 桁ごとに数字を分解
	int secTens = seconds / 10;
	int secOnes = seconds % 10;
	int fracTens = fraction / 10;
	int fracOnes = fraction % 10;

	// テクスチャを変更
	timeSprites_[0]->ChangeTexture(digitTexturePaths_[secTens]);
	timeSprites_[1]->ChangeTexture(digitTexturePaths_[secOnes]);
	timeSprites_[2]->ChangeTexture(colonTexturePath_);
	timeSprites_[3]->ChangeTexture(digitTexturePaths_[fracTens]);
	timeSprites_[4]->ChangeTexture(digitTexturePaths_[fracOnes]);

	// 配置と更新
	Vector2 basePos = { 560.0f, 680.0f };
	float spacing = 40.0f;
	for (int i = 0; i < 5; ++i) {
		timeSprites_[i]->SetPosition({ basePos.x + spacing * i, basePos.y, 0.0f });
		timeSprites_[i]->Update();
	}

}

void GameScreen::Updatedistance()
{

	Vector3 playerPos = player_->GetWorldTransform().translation_;
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1);
	Matrix4x4 matViewProjectionViewport = Multiply(camera_->GetViewMatrix(), Multiply(camera_->GetProjectionMatrix(), matViewport));
	playerPos = Transform(playerPos, matViewProjectionViewport);

	// 時間をfloatで取得（例：9.83）
	float dit = std::max<float>(0, checkPointPos_ - (player_->GetCenterPosition().y));
	//if (dit > 10.0f) dit = 10.0f;  // 最大10秒

// 整数部と小数部に分ける
	int seconds = static_cast<int>(dit);                  // 整数部（例：123）
	int fraction = static_cast<int>(dit * 100) % 100;     // 小数部2桁（例：45）

	// 100の位まで分解（最大999まで想定）
	int secHundreds = seconds / 100;                      // 百の位
	int secTens = (seconds / 10) % 10;                    // 十の位
	int secOnes = seconds % 10;                           // 一の位

	// テクスチャを変更
	ditSprites_[0]->ChangeTexture(disPaths_[secHundreds]);
	ditSprites_[1]->ChangeTexture(disPaths_[secTens]);
	ditSprites_[2]->ChangeTexture(disPaths_[secOnes]);
	ditSprites_[3]->ChangeTexture(mTexturePath_);
	// 配置と更新
	float spacing = 20.0f;
	for (int i = 0; i < 4; ++i) {
		ditSprites_[i]->SetPosition({ playerPos.x - 10 + spacing * i, playerPos.y - 80, 0.0f });
		ditSprites_[i]->Update();
	}

}
