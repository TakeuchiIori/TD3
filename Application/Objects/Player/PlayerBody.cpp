#include "PlayerBody.h"

void PlayerBody::Initialize(Camera* camera)
{
	camera_ = camera;

	// トランスフォームの初期化
	worldTransform_.Initialize();

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitCube.obj");
	obj_->SetMaterialColor({ 1.0f,1.0f,0.3f,1.0f });
}

void PlayerBody::Update()
{
	worldTransform_.UpdateMatrix();
	ExtendUpdate();
}

void PlayerBody::Draw()
{
	obj_->Draw(camera_, worldTransform_);
}

void PlayerBody::UpExtend()
{
	worldTransform_.anchorPoint_ = { 0.0f,1.0f,0.0f };
	worldTransform_.scale_ = verticalGrowthScale_;
	worldTransform_.translation_.y = -1.0f;
	extendDirection_ = ExtendDirection::Up;
}

void PlayerBody::LeftExtend()
{
	worldTransform_.anchorPoint_ = { 1.0f,0.0f,0.0f };
	worldTransform_.scale_ = horizontalGrowthScale_;
	worldTransform_.translation_.x = +1.0f;
	extendDirection_ = ExtendDirection::Left;
}

void PlayerBody::RightExtend()
{
	worldTransform_.anchorPoint_ = { -1.0f,0.0f,0.0f };
	worldTransform_.scale_ = horizontalGrowthScale_;
	worldTransform_.translation_.x = -1.0f; 
	extendDirection_ = ExtendDirection::Right;
}

void PlayerBody::DownExtend()
{
	worldTransform_.anchorPoint_ = { 0.0f,1.0f,0.0f };
	worldTransform_.scale_ = verticalGrowthScale_;
	worldTransform_.translation_.y = +1.0f;
	extendDirection_ = ExtendDirection::Down;
}

void PlayerBody::ExtendUpdate()
{
	float length = Length(endPos_ - startPos_);
	switch (extendDirection_)
	{
	default:
	case ExtendDirection::Up:
		worldTransform_.scale_ = verticalGrowthScale_ + (worldTransform_.anchorPoint_ * length);

		break;

	case ExtendDirection::Left:
		worldTransform_.scale_ = horizontalGrowthScale_ + (worldTransform_.anchorPoint_ * length);

		break;

	case ExtendDirection::Right:
		worldTransform_.scale_ = horizontalGrowthScale_ + (worldTransform_.anchorPoint_ * length);

		break;

	case ExtendDirection::Down:
		worldTransform_.scale_ = verticalGrowthScale_ + (worldTransform_.anchorPoint_ * length);

		break;
	}
}
