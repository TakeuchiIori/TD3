#include "DefaultCamera.h"

void DefaultCamera::Initialize()
{
	InitJson();
}

void DefaultCamera::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("DefaultCamera", "Resources/JSON/Cameras");
	jsonManager_->SetCategory("Cameras");
	jsonManager_->SetSubCategory("DefaultCamera");
	jsonManager_->Register("Translate", &translate_);
	jsonManager_->Register("Rotate", &rotate_);

}

void DefaultCamera::Update()
{
	matView_ = Inverse(MakeAffineMatrix(scale_, rotate_, translate_));
}
