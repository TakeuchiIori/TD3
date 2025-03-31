#include "PlayerBody.h"

void PlayerBody::Initialize(Camera* camera)
{
	camera_ = camera;

	// トランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = defaultScale_;

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitCube.obj");
	obj_->SetMaterialColor({ 0.7f,1.0f,0.7f,1.0f });
}

void PlayerBody::Update()
{
	worldTransform_.UpdateMatrix();
}

void PlayerBody::Draw()
{
	obj_->Draw(camera_, worldTransform_);
}

void PlayerBody::UpGrowth()
{
	worldTransform_.anchorPoint_ = { 0.5f,0.0f,0.5f };
}

void PlayerBody::LeftGrowth()
{
	worldTransform_.anchorPoint_ = { 0.0f,0.5f,0.5f };
}

void PlayerBody::RightGrowth()
{
	worldTransform_.anchorPoint_ = { 1.0f,0.5f,0.5f };
}

void PlayerBody::DownGrowth()
{
	worldTransform_.anchorPoint_ = { 0.5f,1.0f,0.5f };
}
