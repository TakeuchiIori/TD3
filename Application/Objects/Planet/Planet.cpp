#include "Planet.h"

void Planet::Initialize(Camera* camera)
{
	camera_ = camera;

	planet_ = std::make_unique<Object3d>();
	planet_->Initialize();
	planet_->SetModel("planet.obj");
	wt_.Initialize();

	InitJson();
}
void Planet::InitJson() {
	jsonManager_ = std::make_unique<JsonManager>("Planet","Resources/JSON/");
	jsonManager_->SetCategory("Planet");
	jsonManager_->Register("位置", &wt_.translation_);
	jsonManager_->Register("回転", &wt_.rotation_);
	jsonManager_->Register("拡大", &wt_.scale_);
}
void Planet::Update()
{

	wt_.UpdateMatrix();
}

void Planet::Draw()
{
	planet_->Draw(camera_, wt_);
}