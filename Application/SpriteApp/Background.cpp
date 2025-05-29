#include "Background.h"

void Background::Initialzie()
{
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("Resources/images/white.png");


	InitJson();
}

void Background::Update()
{
	sprite_->Update();
}

void Background::Draw()
{
	sprite_->Draw();
}

void Background::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("Background", "Resources/Json/");
	jsonManager_->SetCategory("Background");
	jsonManager_->Register("位置", &sprite_->position_);
	jsonManager_->Register("大きさ", &sprite_->size_);
	jsonManager_->Register("回転", &sprite_->rotation_);
}
