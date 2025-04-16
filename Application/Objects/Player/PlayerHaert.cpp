#include "PlayerHaert.h"

void PlayerHaert::Initialize(Camera* camera)
{
	camera_ = camera;

	// トランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = { 0.3f,0.3f,0.3f };

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitCube.obj");
	obj_->SetMaterialColor({ 0.90625f,0.125f,0.125f,1.0f });
}

void PlayerHaert::Update()
{
	worldTransform_.UpdateMatrix();
}

void PlayerHaert::Draw()
{
	obj_->Draw(BaseObject::camera_, worldTransform_);
}
