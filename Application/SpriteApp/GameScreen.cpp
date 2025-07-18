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
	background_[0]->isDrawImGui_ = true;
	background_[1] = std::make_unique<UIBase>("GameScreen_2");
	background_[1]->Initialize("Resources/JSON/UI/GameScreen_2.json");
	background_[1]->isDrawImGui_ = true;

	///////////////////////////////////////////////////////////////////////////
	// 
	// UIの初期化
	// 
	///////////////////////////////////////////////////////////////////////////

	//// キーボード用UI
	//option_[0] = std::make_unique<UIBase>("KeyBoard_0");
	//option_[0]->Initialize("Resources/JSON/UI/KeyBoard_0.json");
	//option_[1] = std::make_unique<UIBase>("KeyBoard_1");
	//option_[1]->Initialize("Resources/JSON/UI/KeyBoard_1.json");
	//option_[2] = std::make_unique<UIBase>("KeyBoard_2");
	//option_[2]->Initialize("Resources/JSON/UI/KeyBoard_2.json");

	//// コントローラUI
	//option_[3] = std::make_unique<UIBase>("Controller_3");
	//option_[3]->Initialize("Resources/JSON/UI/Controller_3.json");
	//option_[4] = std::make_unique<UIBase>("Controller_4");
	//option_[4]->Initialize("Resources/JSON/UI/Controller_4.json");
	//option_[5] = std::make_unique<UIBase>("Controller_5");
	//option_[5]->Initialize("Resources/JSON/UI/Controller_5.json");

	//// ImGuiの描画やめる
	//for (uint32_t i = 0; i < numOptions_; i++)
	//{
	//	option_[i]->isDrawImGui_ = true;
	//}

	// CurrentMap
	uiMap_ = std::make_unique<Sprite>();
	uiMap_->Initialize("Resources/Textures/In_Game/map.png");
	uiMap_->SetPosition(startMapPos_);
	uiMap_->SetSize(uiMap_->GetTextureSize() * offsetMapScale_);

	uiMapCurrent_ = std::make_unique<Sprite>();
	uiMapCurrent_->Initialize("Resources/Textures/In_Game/here.png");
	uiMapCurrent_->SetAnchorPoint({0.5f,0.5f});
	Vector2 size = uiMapCurrent_->GetTextureSize();;
	uiMapCurrent_->SetSize({ size.x * offsetMapCurrentScale_.x, size.y * offsetMapCurrentScale_.y });
	uiMapCurrent_->SetPosition(offsetMapCurrentPos_);

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
	boost_[0]->SetUVScale({ 1.0f, 1.0f });
	boost_[1] = std::make_unique<Sprite>();
	boost_[1]->Initialize("Resources/Textures/In_Game/dashGauge_fill.png");
	boost_[1]->SetAnchorPoint({ 1.0f, 0.0f });
	boost_[1]->SetSize({ 100.0f, 100.0f });

	for (int i = 0; i < numHeart_; i++)
	{
		heart_[i] = std::make_unique<Sprite>();
		heart_[i]->Initialize("Resources/Textures/In_Game/hp.png");
		heart_[i]->SetSize({ 70.0f, 70.0f });
		heart_[i]->SetPosition(offsetH_ + Vector3{ 70,0,0 } *float(i));
		heart_[i]->Update();
	}
	
	///////////////////////////////////////////////////////////////////////////
	// 
	// 制限時間の初期化
	// 
	///////////////////////////////////////////////////////////////////////////

	baseLimit_ = std::make_unique<UIBase>("BaseLimit");
	baseLimit_->Initialize("Resources/JSON/UI/BaseLimit.json");
	sizeBaseLimit_ = baseLimit_->GetScale();
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
		timeSprites_[i]->SetSize({ timeSpritesSize_ });
	}

	for (int i = 0; i < 5; ++i) {
		plusTimeSprites_[i] = std::make_unique<Sprite>();
		// 初期は全部0.pngで初期化しておく（あとで切り替え）
		plusTimeSprites_[i]->Initialize("Resources/Textures/Each_Number/0.png");
		plusTimeSprites_[i]->SetAnchorPoint({ 0.5f, 0.5f });
		plusTimeSprites_[i]->SetSize({ 20.0f, 30.0f });
		plusTimeSprites_[i]->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	}
	plusTimeSprites_[0]->ChangeTexture(plusTexturePath_);
	plusTimeSprites_[2]->ChangeTexture(dotTexturePath_);
	


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


	///////////////////////////////////////////////////////////////////////////
	// 
	// よだれのスプライト初期化
	// 
	///////////////////////////////////////////////////////////////////////////

	uiYodare_ = std::make_unique<Sprite>();
	uiYodare_->Initialize("Resources/Textures/Option/operation_yodare.png");
	uiYodare_->SetSize({ 60.0f, 60.0f });
	uiYodare_->SetAnchorPoint({ 0.5f, 0.5f });

	uiYodareop_ = std::make_unique<Sprite>();
	uiYodareop_->Initialize("Resources/Textures/Option/yodareUI.png");
	uiYodareop_->SetSize({ 120.0f, 120.0f });

	uiReturn_ = std::make_unique<Sprite>();
	uiReturn_->Initialize("Resources/Textures/In_Game/returnQuickly.png");
	uiReturn_->SetSize({ 400.0f, 160.0f });
	uiReturn_->SetAnchorPoint({ 0.5f, 0.5f });


	InitJson();

	uiMenuOpen_ = std::make_unique<Sprite>();
	uiMenuOpen_->Initialize("Resources/Textures/Menu/menuUI.png");
	uiMenuOpen_->SetPosition(offsetMenuOpen_);
}

void GameScreen::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("UI", "Resources/JSON/UI");
	jsonManager_->SetCategory("UI");
	jsonManager_->Register("プレイヤー追従してるヨダレの位置", &offsetYodare_);
	jsonManager_->Register("草の位置", &offset_);
	jsonManager_->Register("ヨダレの操作アイコンの位置", &offsetYodareop_);
	jsonManager_->Register("ハートの位置", &offsetH_);
	jsonManager_->Register("ブーストの操作アイコンの位置", &offsetB_);
	jsonManager_->Register("マップの位置", &offsetMapPos_);

	jsonManager_->Register("メニュー開くボタンの座標", &offsetMenuOpen_);

	jsonManager_->SetTreePrefix("BoostUI");
	jsonManager_->Register("UIの位置", &boost_[1]->uvScale_);
	jsonManager_->Register("UIのUVスケール", &boost_[1]->uvScale_);
	jsonManager_->Register("UIのUVオフセット", &boost_[1]->uvTranslate_);

	jsonManager_->Register("位置", &boost_[1]->position_);
	jsonManager_->Register("サイズ", &boost_[1]->size_);


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
		//background_[i]->ImGUi();
		background_[i]->Update();
	}

	///////////////////////////////////////////////////////////////////////////
	// 
	// UIの更新処理
	// 
	///////////////////////////////////////////////////////////////////////////
	/*for (uint32_t i = 0; i < numOptions_; i++)
	{
		option_[i]->Update();
	}*/

	UpdateMapView();

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

			boost_[1]->SetPosition(position);

			// UV設定（比率に応じて縦方向に縮小 & 下から満ちる）
			boost_[1]->SetSize({ baseSize.x, baseSize.y * ratio });
			boost_[1]->SetUVScale({ 1.0f, ratio });
			boost_[1]->SetUVTranslate({ 0.0f, 1.0f - ratio });

			if (player_->CanBoost()) {
				if (ratio >= 0.9f) {
					
				}
			}
		}

		boost_[i]->Update();
	}

	for (int i = 0; i < numHeart_; i++)
	{
		heart_[i]->SetPosition(offsetH_ + Vector3{ 70,0,0 } *float(i));
		heart_[i]->Update();
	}

	///////////////////////////////////////////////////////////////////////////
	// 
	// 制限時間の更新処理
	// 
	///////////////////////////////////////////////////////////////////////////
	baseLimit_->Update();
	baseLimit_->ImGUi();
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
		Vector2 baseScale = { 130.0f,100.0f };
		Vector2 scale = baseScale * (1.0f + pulse);
		uiYodare_->SetSize({ scale.x, scale.y });
	}

	if (yodareState_ != YodareState::Hidden) {
		Vector3 playerPos = player_->GetWorldTransform().translation_;
		Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1);
		Matrix4x4 matViewProjectionViewport = Multiply(camera_->GetViewMatrix(), Multiply(camera_->GetProjectionMatrix(), matViewport));
		playerPos = Transform(playerPos, matViewProjectionViewport);
		if(player_->GetGrassXDir() < 0)
		{
			playerPos.x += offsetYodare_.x;
			playerPos.y += offsetYodare_.y;
		}
		else
		{
			playerPos.x -= offsetYodare_.x;
			playerPos.y += offsetYodare_.y;
		}
		uiYodare_->SetPosition(playerPos);
		uiYodare_->Update();
	}
	static float alp = 1.0f;
	static float dirA = -1.0f;
	if (player_->behavior_ == BehaviorPlayer::Return)
	{
		Vector3 playerPos = { 640, 100,0 };
		if (dirA < 0)
		{
			if (alp < 0.2f)
			{
				dirA *= -1.0f;
			}
		}
		else
		{
			if (alp > 1.0f)
			{
				dirA *= -1.0f;
			}
		}


		alp += GameTime::GetDeltaTime() * dirA;
		

		uiReturn_->SetPosition(playerPos);
		uiReturn_->SetAlpha(alp);
		uiReturn_->Update();
	}
	else
	{
		alp = 1.0f;
		dirA = -1.0f;
	}

	uiYodareop_->SetPosition(offsetYodareop_);
	uiYodareop_->Update();

#ifdef _DEBUG
	uiMenuOpen_->SetPosition(offsetMenuOpen_);
#endif // _DEBUG

	uiMenuOpen_->Update();


	Updatedistance();
}

void GameScreen::Draw()
{

	for (uint32_t i = 0; i < numBGs_; i++)
	{
		background_[i]->Draw();
	}


	/*if (Input::GetInstance()->IsControllerConnected())
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
	}*/

	uiMap_->Draw();
	uiMapCurrent_->Draw();

	uiMenuOpen_->Draw();

	/*for (uint32_t i = 0; i < numGrass_; i++)
	{
		grass_[i]->Draw();
	}*/


	boost_[1]->Draw();
	boost_[0]->Draw();

	for (int i = 0; i < player_->GetHP(); i++)
	{
		heart_[i]->Draw();
	}


	baseLimit_->Draw();

	for (const auto& sprite : timeSprites_) {
		sprite->Draw();
	}

	for (const auto& sprite : ditSprites_) {
		sprite->Draw();
	}

	if (visibleTimer_ > 0)
	{
		for (const auto& sprite : plusTimeSprites_) {
			sprite->Draw();
		}
	}

	if (yodareState_ != YodareState::Hidden) {
		uiYodare_->Draw();
	}
	if (player_->behavior_ == BehaviorPlayer::Return)
	{
		uiReturn_->Draw();
	}

	uiYodareop_->Draw();

}

void GameScreen::UpdateLimit()
{
	// 時間をfloatで取得（例：9.83）
	float time = player_->GetTimeLimit();

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
	if(!player_->PauseUpdate())
	{
		for (int i = 0; i < 5; ++i)
		{
			timeSprites_[i]->SetPosition({ basePos.x + spacing * i, basePos.y, 0.0f });
			timeSprites_[i]->Update();
		}
	}
	else
	{
		if (timeUpTimer_ <= kTimeUpTime_)
		{
			timeUpTimer_ += GameTime::GetDeltaTime();

			// 拡大率（最大1.5倍、1秒かけて拡大）
			float scale = 1.0f + 0.3f * std::min(timeUpTimer_ / kTimeUpTime_, 1.0f);

			// 揺れ角度（-10～+10度を往復）
			float angle = std::sin(timeUpTimer_ * 20.0f) * 10.0f;

			// 中央位置（コロンの位置）を基準に揺らす
			Vector2 timeUpOrigin = { basePos.x + spacing * 2, basePos.y };

			for (int i = 0; i < 5; ++i)
			{
				Vector2 localOffset = {
					basePos.x + spacing * i - timeUpOrigin.x,
					basePos.y - timeUpOrigin.y
				};

				// 回転（2D変換）
				float rad = angle * (std::numbers::pi_v<float> / 180.0f);
				Vector2 rotated = {
					std::cos(rad) * localOffset.x - std::sin(rad) * localOffset.y,
					std::sin(rad) * localOffset.x + std::cos(rad) * localOffset.y
				};

				Vector2 newPos = {
					timeUpOrigin.x + rotated.x * scale,
					timeUpOrigin.y + rotated.y * scale - (scale - 0.5f) * 20.0f
				};

				timeSprites_[i]->SetPosition({ newPos.x, newPos.y, 0.0f });
				timeSprites_[i]->SetSize(timeSpritesSize_ * scale);
				timeSprites_[i]->SetRotation({ 0,0,rad }); // 回転揺れ
				timeSprites_[i]->Update();
				baseLimit_->SetScale(sizeBaseLimit_ * scale);
			}
		}
		else
		{
			for (int i = 0; i < 5; ++i)
			{
				timeSprites_[i]->SetRotation({ 0,0,0 });
				timeSprites_[i]->SetSize(timeSpritesSize_);
				timeSprites_[i]->Update();
			}
			baseLimit_->SetScale(sizeBaseLimit_);
			timeUpTimer_ = 0;
			player_->ResumeUpdate();
		}
	}

	if (player_->IsAddTime())
	{
		Vector3 playerPos = player_->GetWorldTransform().translation_;
		Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1);
		Matrix4x4 matViewProjectionViewport = Multiply(camera_->GetViewMatrix(), Multiply(camera_->GetProjectionMatrix(), matViewport));
		playerPos = Transform(playerPos, matViewProjectionViewport);

		time = player_->GetAddtime();

		seconds = static_cast<int>(time);          // 整数部（9）
		fraction = static_cast<int>(time * 100) % 100; // 小数部2桁（83）

		// 桁ごとに数字を分解
		secOnes = seconds % 10;
		fracTens = fraction / 10;
		fracOnes = fraction % 10;

		// テクスチャを変更
		plusTimeSprites_[1]->ChangeTexture(disPaths_[secOnes]);
		plusTimeSprites_[3]->ChangeTexture(disPaths_[fracTens]);
		plusTimeSprites_[4]->ChangeTexture(disPaths_[fracOnes]);
		visibleTimer_ = kVisibleTime_;

		basePos = { 580.0f, 630.0f };
		spacing = 15.0f;
		for (int i = 0; i < 5; ++i) 
		{
			if(i < 3)
			{
				plusTimeSprites_[i]->SetPosition({ playerPos.x + spacing * i - 80, playerPos.y - 8, 0.0f });
			}
			else
			{
				plusTimeSprites_[i]->SetPosition({ playerPos.x + spacing * (i - 1) -75, playerPos.y - 8, 0.0f });
			}
			plusTimeSprites_[i]->SetAlpha(1.0f);
			plusTimeSprites_[i]->Update();
		}
	}
	if (visibleTimer_ > 0)
	{
		visibleTimer_ -= deltaTime_;
		if (visibleTimer_ >= kVisibleTime_ / 2.0f)
		{
			for (int i = 0; i < 5; ++i)
			{
				Vector3 pos = plusTimeSprites_[i]->GetPosition();
				pos.y -= 30.0f * deltaTime_;
				plusTimeSprites_[i]->SetPosition(pos);
				plusTimeSprites_[i]->Update();
			}
		}
		else 
		{
			for (int i = 0; i < 5; ++i)
			{
				float alpha = visibleTimer_ / (kVisibleTime_ / 2.0f);
				plusTimeSprites_[i]->SetAlpha(alpha);
				plusTimeSprites_[i]->Update();
			}
		}
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
	if(player_->GetMoveDirection().y < 0)
	{
		for (int i = 0; i < 4; ++i) {
			ditSprites_[i]->SetPosition({ playerPos.x - 30 + spacing * i, playerPos.y + 50, 0.0f });
			ditSprites_[i]->Update();
		}
	}
	else
	{
		for (int i = 0; i < 4; ++i) {
			ditSprites_[i]->SetPosition({ playerPos.x - 30 + spacing * i, playerPos.y - 50, 0.0f });
			ditSprites_[i]->Update();
		}
	}

}

void GameScreen::UpdateMapView()
{
	Vector2 size = uiMapCurrent_->GetTextureSize();
#ifdef _DEBUG
	ImGui::Begin("MapView");
	ImGui::DragFloat2("pos", &offsetMapCurrentPos_.x, 0.1f);
	ImGui::DragFloat2("scale", &offsetMapCurrentScale_.x, 0.1f);
	ImGui::End();
#endif // _DEBUG
	uiMap_->SetPosition(offsetMapPos_);
	uiMapCurrent_->SetSize({ size.x * offsetMapCurrentScale_.x, size.y * offsetMapCurrentScale_.y });
	uiMapCurrent_->SetPosition(offsetMapCurrentPos_);
	if (currentMapNum_ == 0)
	{
		uiMapCurrent_->SetSize({ size.x * 1.5f, size.y * 1.1f });
		uiMapCurrent_->SetPosition(Vector3(173.0f, 622.3f, 0.0f) - startMapPos_ + offsetMapPos_);
	}
	if (currentMapNum_ == 1)
	{
		uiMapCurrent_->SetSize({ size.x * 1.4f, size.y * 1.3f });
		uiMapCurrent_->SetPosition(Vector3(173.0f, 564.2f, 0.0f) - startMapPos_ + offsetMapPos_);
	}
	if (currentMapNum_ == 2)
	{
		uiMapCurrent_->SetSize({ size.x * 1.4f, size.y * 1.7f });
		uiMapCurrent_->SetPosition(Vector3(173.0f, 495.3f, 0.0f) - startMapPos_ + offsetMapPos_);
	}
	if (currentMapNum_ == 3)
	{
		uiMapCurrent_->SetSize({ size.x * 1.4f, size.y * 1.7f });
		uiMapCurrent_->SetPosition(Vector3(173.0f, 416.0f, 0.0f) - startMapPos_ + offsetMapPos_);
	}
	if (currentMapNum_ == 4)
	{
		uiMapCurrent_->SetSize({ size.x * 1.4f, size.y * 1.8f });
		uiMapCurrent_->SetPosition(Vector3(173.0f, 335.7f, 0.0f) - startMapPos_ + offsetMapPos_);
	}
	if (currentMapNum_ == 5)
	{
		uiMapCurrent_->SetSize({ size.x * 1.4f, size.y * 1.4f });
		uiMapCurrent_->SetPosition(Vector3(173.0f, 257.6f, 0.0f) - startMapPos_ + offsetMapPos_);
	}
	if (currentMapNum_ == 6)
	{
		uiMapCurrent_->SetSize({ size.x * 1.4f, size.y * 1.6f });
		uiMapCurrent_->SetPosition(Vector3(173.0f, 184.1f, 0.0f) - startMapPos_ + offsetMapPos_);
	}
	uiMap_->Update();
	uiMapCurrent_->Update();
}
