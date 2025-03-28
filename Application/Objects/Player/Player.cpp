#include "Player.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

void Player::Initialize(Camera* camera)
{
	input_ = Input::GetInstance();

	camera_ = camera;

	// トランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = { 2.0f,-1.0f,6.0f };
	worldTransform_.scale_ = { 2.0f,2.0f,2.0f };
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	bodyTransform_.Initialize();
	bodyTransform_.parent_ = &worldTransform_;
	bodyTransform_.translation_ += bodyOffset_;

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitCube.obj");
	obj_->SetMaterialColor({ 0.3f,0.3f,1.0f,1.0f });

	colliderRct_.height = 2.0f;
	colliderRct_.width = 2.0f;
}

void Player::Update()
{

	Move();
	TimerManager();
	UpdateMatrices();
}

void Player::Draw()
{
	obj_->Draw(camera_, bodyTransform_);
	for (const auto& body : playerBodys_) {
		body->Draw();
	}
}

void Player::OnCollision()
{
	if (false) // 草を食べたら
	{
		if (MaxGrassGauge_ > grassGauge_)
		{
			grassGauge_++;
			extendTimer_ = (std::min)(kTimeLimit_, extendTimer_ + grassTime_);
		}
	}
}

void Player::UpdateMatrices()
{
	worldTransform_.UpdateMatrix();
	bodyTransform_.UpdateMatrix();
	for (const auto& body : playerBodys_) {
		body->Update();
	}
}

void Player::Move()
{
	Boost();

	velocity_ = { 0.0f,0.0f,0.0f };

	if (input_->PushKey(DIK_W))
	{
		moveDirection_ = { 0,1,0 };
		//moveDirection_.z++;
		collisionFlag_ = MapChipCollision::CollisionFlag::Top;
	}
	else if (input_->PushKey(DIK_S))
	{
		moveDirection_ = { 0,-1,0 };
		collisionFlag_ = MapChipCollision::CollisionFlag::Bottom;
	}
	else if (input_->PushKey(DIK_A))
	{
		moveDirection_ = { -1,0,0 };
		collisionFlag_ = MapChipCollision::CollisionFlag::Right;
	}
	else if (input_->PushKey(DIK_D))
	{
		moveDirection_ = { 1,0,0 };
		collisionFlag_ = MapChipCollision::CollisionFlag::Left;
	}
	
	moveDirection_ = Normalize(moveDirection_);

	if(isFPSMode_) // カメラをプレイヤー視点にしたとき
	{
		moveDirection_ = TransformNormal(moveDirection_, MakeRotateMatrixY(worldTransform_.rotation_.y));
	}

	velocity_ += moveDirection_ * speed_;

	//mapCollision_.DetectAndResolveCollision(colliderRct_, worldTransform_.translation_, velocity_, collisionFlag_);


	

	/*if (mapCollision_.GetIsPopBody()) {
		std::unique_ptr<PlayerBody> body = std::make_unique<PlayerBody>();
		body->Initialize(camera_);
		body->SetPos(mapCollision_.GetPopPos());
		playerBodys_.push_back(move(body));
	}*/

	worldTransform_.translation_ += velocity_;

	/*if (mapCollision_.GetIsCollision())
	{
		moveDirection_ = { 0,0,0 };
		if (mapCollision_.GetIsCollisionBody()) {
			for (size_t i = playerBodys_.size(); 2 < playerBodys_.size();) {
				mapCollision_.ElasePos(playerBodys_.begin()->get()->GetPos());
				playerBodys_.pop_front();
			}
		}
	}*/

}

void Player::Boost()
{
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_B))
	{
		boostTimer_ = kBoostTime_;
	}
#endif // _DEBUG

	if (0 < boostTimer_)
	{
		speed_ = defaultSpeed_ + boostSpeed_;
	}
	else
	{
		speed_ = defaultSpeed_;
	}
}

void Player::TimerManager()
{
	if (0 < extendTimer_) 
	{
		extendTimer_ -= deltaTime_;
	}
	if (0 < boostTimer_) 
	{
		boostTimer_ -= deltaTime_;
	}
}
