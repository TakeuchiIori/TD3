#include "BaseStageObject.h"

int BaseStageObject::count = 0;
int BaseStageObject::next_id = 0;

void BaseStageObject::Initialize(Camera* camera)
{
	camera_ = camera;

	name_ = name_ + std::to_string(id);

	// トランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = { 0.0f,worldTransform_.scale_.y / 2.0f,0.0f };

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitCube.obj");

	InitJson();
	worldTransform_.UpdateMatrix();
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
	if (stageName_.size() > 0)
	{
		jsonManager_ = std::make_unique<JsonManager>(stageName_, "Resources/JSON");
		jsonManager_->ChildRegister(stageName_, name_, "ID", &id);
		jsonManager_->ChildRegister(stageName_, name_, "Scale", &worldTransform_.scale_);
		jsonManager_->ChildRegister(stageName_, name_, "Translate", &worldTransform_.translation_);
		jsonManager_->ChildRegister(stageName_, name_, "Rotate", &worldTransform_.rotation_);
	}
}
