#include "Player.h"

void Player::Initialize(Camera* camera)
{
	input_ = Input::GetInstance();

	camera_ = camera;

	// トランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	bodyTransform_.Initialize();
	bodyTransform_.parent_ = &worldTransform_;
	bodyTransform_.translation_ += bodyOffset_;

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitSphere.obj");
	obj_->SetMaterialColor({ 0.3f,0.3f,1.0f,1.0f });
}

void Player::Update()
{
	Move();
	UpdateMatrices();
}

void Player::Draw()
{
	obj_->Draw(camera_, bodyTransform_);
}

void Player::UpdateMatrices()
{
	worldTransform_.UpdateMatrix();
	bodyTransform_.UpdateMatrix();
}

void Player::Move()
{
	velocity_ = { 0.0f,0.0f,0.0f };
	moveDirection_ = { 0.0f,0.0f,0.0f };

	if (input_->PushKey(DIK_W))
	{
		moveDirection_.z++;
	}

	if (input_->PushKey(DIK_S)) 
	{
		moveDirection_.z--;
	}

	if (input_->PushKey(DIK_A)) 
	{
		moveDirection_.x--;
	}

	if (input_->PushKey(DIK_D))
	{
		moveDirection_.x++;
	}
	
	moveDirection_ = Normalize(moveDirection_);

	if(isFPSMode_) // カメラをプレイヤー視点にしたとき
	{
		moveDirection_ = TransformNormal(moveDirection_, MakeRotateMatrixY(worldTransform_.rotation_.y));
	}

	velocity_ += moveDirection_ * speed_;

	worldTransform_.translation_ += velocity_;

}
