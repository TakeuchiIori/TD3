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

	///////////////////////////////////////////////////////////////////////////
	// 
	// 草の初期化
	// 
	///////////////////////////////////////////////////////////////////////////
	grass_[0] = std::make_unique<UIBase>("Grass_0");
	grass_[0]->Initialize("Resources/JSON/UI/Grass_0.json");
	grass_[1] = std::make_unique<UIBase>("Grass_1");
	grass_[1]->Initialize("Resources/JSON/UI/Grass_1.json");
	
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
			float gauge = static_cast<float>(player_->GetGrassGauge());
			float maxGauge = static_cast<float>(player_->GetMaxGrassGauge());
			float ratio = std::clamp(gauge / maxGauge, 0.0f, 100.0f);  // 0.0〜1.0

			Vector4 bottomColor{};
			Vector4 topColor{};

			if (ratio < 0.5f) {
				// 緑 → 黄
				float t = ratio / 0.5f; // 0〜1に再マッピング
				bottomColor = { 0.0f, 1.0f, 0.0f, 1.0f }; // 緑
				topColor = {
					1.0f * t,             // 赤が増える
					1.0f,                 // 緑そのまま
					0.0f, 1.0f
				}; // 緑→黄
			} else {
				// 黄 → 赤
				float t = (ratio - 0.5f) / 0.5f;
				bottomColor = { 1.0f, 1.0f, 0.0f, 1.0f }; // 黄
				topColor = {
					1.0f,             // 赤
					1.0f - t,         // 緑が減る
					0.0f, 1.0f
				}; // 黄→赤
			}

			// スプライトに色設定
		//	grass_[1]->SetGradientColor(bottomColor, topColor);
		//	grass_[1]->SetGradientFillRatio(ratio); // ← 追加！

			
			//grass_[1]->SetScale({ 1.0f, ratio });
			// UVスケールとUVトランスレートを設定
			//grass_[1]->SetUVScale({ 1.0f, ratio });
			//grass_[1]->SetUVTranslate({ 0.0f,1.0f - ratio });

			Vector2 baseSize = { 51.5f, 50.0f }; // 草画像の本来のサイズ

			// 補正用オフセット計算（比率1.0なら0、0.5なら半分ズレる）
			float yOffset = baseSize.y * (1.0f - ratio);
			Vector3 position = playerPos + offsetGrass_;
			position.y += yOffset;

			grass_[1]->SetPosition(position);
			// UV設定（比率に応じて縦方向に縮小 & 下から満ちる）
			grass_[1]->SetUVScale({ 1.0f, ratio });
			grass_[1]->SetUVTranslate({ 0.0f, 1.0f - ratio });

			// アンカーを下に固定（下から伸びてくるイメージ）
			//grass_[1]->SetAnchorPoint({ 0.5f, 1.0f });


			//grass_[1]->SetScale(baseSize);
		}

#ifdef _DEBUG



		//ImGui::Begin("Grass");
		//ImGui::DragFloat3("offsetGrass_", &offsetGrass_.x, 0.1f);
		//ImGui::End();
#endif // _DEBUG
		grass_[i]->Update();

	}

	///////////////////////////////////////////////////////////////////////////
	// 
	// 制限時間の更新処理
	// 
	///////////////////////////////////////////////////////////////////////////
	baseLimit_->Update();
	UpdateLimit();


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

	for (const auto& sprite : timeSprites_) {
		sprite->Draw();
	}



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

