#include "Ground.h"

void Ground::Initialize(Camera* camera)
{
	camera_ = camera;
	// Object3dの生成
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitFloor.obj");
	obj_->SetMaterialUVTransform(MakeScaleMatrix({ 100,100,0 }));

	worldTransform_.Initialize();
	worldTransform_.scale_ = { 100,1,100 };
}

void Ground::Update()
{
	worldTransform_.UpdateMatrix();
}

void Ground::Draw()
{
	obj_->Draw(camera_,worldTransform_);
}
