#include "Grass.h"

void Grass::Initialize(Camera* camera)
{
	camera_ = camera;

	// トランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = { 2.0f,2.0f,2.0f };

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitCube.obj");
	obj_->SetMaterialColor({ 0.7f,1.0f,0.7f,1.0f });
}

void Grass::Update()
{
	worldTransform_.UpdateMatrix();
}

void Grass::Draw()
{
	obj_->Draw(camera_, worldTransform_);
}

void Grass::Growth()
{
}
