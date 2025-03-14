#include "Cube.h"

void Cube::Initialize(Camera* camera)
{
	camera_ = camera;

	// トランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = { 5,5,5 };
	worldTransform_.translation_ = {4,2.5f,0};

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitCube.obj");
}

void Cube::Update()
{
	worldTransform_.UpdateMatrix();
}

void Cube::Draw()
{
	obj_->Draw(camera_, worldTransform_);
}
