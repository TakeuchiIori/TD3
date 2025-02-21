#include "BaseStageObject.h"

void BaseStageObject::Initialize(Camera* camera)
{
	camera_ = camera;

	// トランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = { 0.0f,worldTransform_.scale_.y / 2.0f,0.0f };

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitCube.obj");

	InitJson();
}

void BaseStageObject::Update()
{
	worldTransform_.UpdateMatrix();
}

void BaseStageObject::Draw()
{
	obj_->Draw(camera_, worldTransform_);
}

void BaseStageObject::InitJson()
{
	if (name_.size() > 0)
	{
		jsonManager_ = std::make_unique<JsonManager>(name_, "Resources/JSON");
		jsonManager_->Register("Scale", &worldTransform_.scale_);
		jsonManager_->Register("Translate", &worldTransform_.translation_);
		jsonManager_->Register("Rotate", &worldTransform_.rotation_);
	}
}
