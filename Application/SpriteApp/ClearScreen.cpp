#include "ClearScreen.h"

void ClearScreen::Initialize()
{
	///////////////////////////////////////////////////////////////////////////
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
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("Resources/Textures/Option/clear_krin.png");
	spriteA_ = std::make_unique<Sprite>();
	spriteA_->Initialize("Resources/Textures/Option/title_go.png");
	spriteA_->SetAnchorPoint(Vector2(0.5, 0.5));
	// スプライトスライド初期化
	spriteSlideTimer_ = 0.0f;
	isSliding_ = true;
	// sin波のタイマー初期化
	sinTimer_ = 0.0f;



    spriteA_->position_ = targetPosition_;
    // スプライトスライド初期化
    spriteSlideTimer_ = 0.0f;
    isSliding_ = true;
    // sin波のタイマー初期化
    sinTimer_ = 0.0f;

	InitJson();

	originalSize_ = spriteA_->size_;

}

void ClearScreen::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("ClearUI", "Resources/JSON/UI");
	jsonManager_->SetCategory("ClearUI");
	jsonManager_->Register("スタート", &spriteStartX_);
	jsonManager_->Register("ゴール", &spriteEndX_);
	jsonManager_->Register("サイズ", &sprite_->size_);
	jsonManager_->Register("Y", &sprite_->position_.y);
	jsonManager_->Register("Aサイズ", &spriteA_->size_);
	jsonManager_->Register("offset", &offset_);
	jsonManager_->Register("位置", &spriteA_->position_);

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

void ClearScreen::UpdateKirin()
{
    // スプライトスライド処理
    if (isSliding_) {
        spriteSlideTimer_ += 1.0f / 60.0f; // 60FPS想定
        if (spriteSlideTimer_ >= spriteSlideTime_) {
            spriteSlideTimer_ = spriteSlideTime_;
            isSliding_ = false;
            // スライド完了時にスプライトAの出現開始
            isAppearing_ = true;
            appearTimer_ = 0.0f;
        }
        // イージング計算（滑らかな動き）
        float t = spriteSlideTimer_ / spriteSlideTime_;
        float easedT = 1.0f - (1.0f - t) * (1.0f - t); // イーズアウト
        sprite_->position_.x = spriteStartX_ + (spriteEndX_ - spriteStartX_) * easedT;
    }

    // スプライトAの出現アニメーション
    if (isAppearing_) {
        appearTimer_ += 1.0f / 60.0f;

        if (appearTimer_ < appearDuration_) {
            // 弾性的なイージング（ぽよよん効果）
            float t = appearTimer_ / appearDuration_;
            float elasticOut = 1.0f - pow(2.0f, -10.0f * t) * cos(t * 3.14159f * 3.0f);

            // サイズをぽよよんと大きくする
            Vector2 currentScale = originalSize_ * elasticOut;
            spriteA_->size_ = currentScale;

            // 位置も少し上から落ちてくるような動き（オプション）
            float yOffset = (1.0f - elasticOut) * -20.0f;
            spriteA_->position_ = targetPosition_ + Vector3(0.0f, yOffset, 0.0f);
        } else {
            // アニメーション終了
            isAppearing_ = false;
            spriteA_->position_ = targetPosition_;
        }
    }

    // 出現完了後のみsin波でサイズ変化
    if (!isSliding_ && !isAppearing_) {
        sinTimer_ += 1.0f / 60.0f;

        float scaleWave = sin(sinTimer_ * floatSpeed_) * 0.05f; // 5%の変化
        spriteA_->size_ = originalSize_ * (1.0f + scaleWave);
    }

    sprite_->Update();
    spriteA_->Update();
}

void ClearScreen::Draw()
{
    for (uint32_t i = 0; i < numBGs_; i++)
    {
        background_[i]->Draw();
    }
    sprite_->Draw();

    // スプライトAはスライド完了後のみ描画
    if (!isSliding_) {
        spriteA_->Draw();
    }
}