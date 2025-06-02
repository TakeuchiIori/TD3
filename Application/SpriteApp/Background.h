#pragma once
#include "Sprite/Sprite.h"
#include "../../../Engine/Utility/Loaders/Json/JsonManager.h"

class Background
{
public:

	void Initialzie();
	void Update();
	void Draw();

public:
	void SetColor(const Vector4& color) { sprite_->SetColor(color); }

private:

	void InitJson();


private:

	std::unique_ptr<Sprite> sprite_;
	std::unique_ptr<JsonManager> jsonManager_;
};

