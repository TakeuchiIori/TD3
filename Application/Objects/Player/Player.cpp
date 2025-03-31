#include "Player.h"

#ifdef _DEBUG
#include "imgui.h"
#include "string"
#endif // _DEBUG

void Player::Initialize(Camera* camera)
{
	input_ = Input::GetInstance();

	BaseObject::camera_ = camera;

	// トランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = { 2.0f,0.0f,6.0f };
	worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;


	worldTransform_.UpdateMatrix();

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitCube.obj");
	obj_->SetMaterialColor({ 0.3f,0.3f,1.0f,1.0f });
	

	/*SphereCollider::SetCamera(BaseObject::camera_);
	SphereCollider::Initialize();*/

	AABBCollider::SetCamera(BaseObject::camera_);
	AABBCollider::Initialize();

	SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayer));

	InitJson();
	//colliderRct_.height = 2.0f;
	//colliderRct_.width = 2.0f;
}

void Player::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("playerObj", "Resources/JSON/");

	jsonCollider_ = std::make_unique<JsonManager>("playerCollider", "Resources/JSON/");
	//SphereCollider::InitJson(jsonCollider_.get());
	AABBCollider::InitJson(jsonCollider_.get());
}

void Player::Update()
{
	beforebehavior_ = behavior_;

	// 各行動の初期化
	BehaviorInitialize();

	// 各行動の更新
	BehaviorUpdate();

	ExtendBody();

	PopGrass();

	TimerManager();

	UpdateMatrices();
	//SphereCollider::Update();
	AABBCollider::Update();
	
#ifdef _DEBUG
	DebugPlayer();
#endif // _DEBUG

}

void Player::Draw()
{
	obj_->Draw(BaseObject::camera_, worldTransform_);
	for (const auto& body : playerBodys_) {
		body->Draw();
	}
}

void Player::DrawCollision()
{
	//SphereCollider::Draw();
	AABBCollider::Draw();
}

//void Player::OnCollision()
//{
//	if (false) // 草を食べたら
//	{
//		if (MaxGrassGauge_ > grassGauge_)
//		{
//			grassGauge_++;
//			extendTimer_ = (std::min)(kTimeLimit_, extendTimer_ + grassTime_);
//		}
//	}
//}

void Player::MapChipOnCollision(const CollisionInfo& info)
{// 衝突したブロックの種類に応じた処理
	switch (info.blockType) {
	case MapChipType::kBlock:
		// 通常ブロックの処理
		break;

		// 将来的に追加される特殊ブロックの処理
		// case MapChipType::kDamageBlock:
		//     TakeDamage(10);
		//     break;
		// case MapChipType::kJumpBlock:
		//     velocity_.y = 10.0f;  // ジャンプさせる
		//     break;

	default:
		break;
	}

	// 衝突方向に応じた処理
	if (info.direction == 4) {  // 下方向の衝突 = 着地
		//isGrounded_ = true;
	}
}

void Player::OnCollision(Collider* other)
{
}

void Player::EnterCollision(Collider* other)
{
	if (behavior_ != BehaviorPlayer::Return)
	{
		if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kGrass)) // 草を食べたら
		{
			if (MaxGrass_ > grassGauge_ && createGrassTimer_ <= 0)
			{
				if (dynamic_cast<AABBCollider*>(other)->GetWorldTransform().scale_.x != /*GetRadius()*/0)
				{
					extendTimer_ = (std::min)(kTimeLimit_, extendTimer_ + grassTime_);
				}
				else
				{
					extendTimer_ = (std::min)(kTimeLimit_, extendTimer_ + largeGrassTime_);
				}
				grassGauge_++;
			}
			if (MaxGrass_ <= grassGauge_)
			{
				grassGauge_ = 0;
				createGrassTimer_ = kCreateGrassTime_;
				isCreateGrass_ = true;
			}
		}
	}
}

void Player::ExitCollision(Collider* other)
{
}

void Player::UpdateMatrices()
{
	worldTransform_.UpdateMatrix();
	for (const auto& body : playerBodys_) {
		body->Update();
	}
}

void Player::Move()
{

	velocity_ = { 0.0f,0.0f,0.0f };

	if (input_->TriggerKey(DIK_W) && moveDirection_ != Vector3{ 0,1,0 })
	{
		moveDirection_ = { 0,1,0 };
		moveHistory_.push_back(worldTransform_.translation_);

		ExtendBody();
		std::unique_ptr<PlayerBody> body = std::make_unique<PlayerBody>();
		body->Initialize(BaseObject::camera_);
		body->SetStartPos(GetCenterPosition());
		body->SetPos(GetCenterPosition());
		body->UpExtend();
		playerBodys_.push_back(std::move(body));
	}
	else if (input_->TriggerKey(DIK_S) && moveDirection_ != Vector3{ 0,-1,0 })
	{
		moveDirection_ = { 0,-1,0 };
		moveHistory_.push_back(worldTransform_.translation_);

		ExtendBody();
		std::unique_ptr<PlayerBody> body = std::make_unique<PlayerBody>();
		body->Initialize(BaseObject::camera_);
		body->SetStartPos(GetCenterPosition());
		body->SetPos(GetCenterPosition());
		body->DownExtend();
		playerBodys_.push_back(std::move(body));
	}
	else if (input_->TriggerKey(DIK_A) && moveDirection_ != Vector3{ -1,0,0 })
	{
		moveDirection_ = { -1,0,0 };
		moveHistory_.push_back(worldTransform_.translation_);

		ExtendBody();
		std::unique_ptr<PlayerBody> body = std::make_unique<PlayerBody>();
		body->Initialize(BaseObject::camera_);
		body->SetStartPos(GetCenterPosition());
		body->SetPos(GetCenterPosition());
		body->LeftExtend();
		playerBodys_.push_back(std::move(body));
	}
	else if (input_->TriggerKey(DIK_D) && moveDirection_ != Vector3{ 1,0,0 })
	{
		moveDirection_ = { 1,0,0 };
		moveHistory_.push_back(worldTransform_.translation_);

		ExtendBody();
		std::unique_ptr<PlayerBody> body = std::make_unique<PlayerBody>();
		body->Initialize(BaseObject::camera_);
		body->SetStartPos(GetCenterPosition());
		body->SetPos(GetCenterPosition());
		body->RightExtend();
		playerBodys_.push_back(std::move(body));
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

	Vector3 newPos = worldTransform_.translation_ + velocity_;

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

	mpCollision_.DetectAndResolveCollision(
		colliderRect_,  // 衝突判定用矩形
		newPos,    // 更新される位置（衝突解決後）
		velocity_,      // 更新される速度
		MapChipCollision::CollisionFlag::All,  // すべての方向をチェック
		[this](const CollisionInfo& info) {
			// 衝突時の処理（例：特殊ブロック対応）
			MapChipOnCollision(info);
		}
	);

	worldTransform_.translation_ = newPos;


	ExtendBody();

}

void Player::EntryMove()
{
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_SPACE))
	{
		behaviortRquest_ = BehaviorPlayer::Moving;
		moveDirection_ = { 0,1,0 };
		extendTimer_ = kTimeLimit_;
		moveHistory_.push_back(worldTransform_.translation_);
	}
#endif // _DEBUG
}

void Player::EntryBoost()
{
	if(0 >= boostCoolTimer_)
	{
#ifdef _DEBUG
		if (input_->TriggerKey(DIK_B))
		{
			behaviortRquest_ = BehaviorPlayer::Boost;
		}
#endif // _DEBUG
	}
}

void Player::EntryReturn()
{
	behaviortRquest_ = BehaviorPlayer::Return;
}

void Player::TimerManager()
{
	if (0 < extendTimer_) 
	{
		extendTimer_ -= deltaTime_;
	}
	if (0 < boostCoolTimer_)
	{
		boostCoolTimer_ -= deltaTime_;
	}
	if (0 < createGrassTimer_)
	{
		createGrassTimer_ -= deltaTime_;
	}
}

bool Player::PopGrass()
{
	if (0 >= createGrassTimer_ && isCreateGrass_)
	{
		isCreateGrass_ = false;
		return true;
	}
	return false;
}

void Player::ExtendBody()
{
	if (beforebehavior_ == BehaviorPlayer::Root && behavior_ == BehaviorPlayer::Moving)
	{
		std::unique_ptr<PlayerBody> body = std::make_unique<PlayerBody>();
		body->Initialize(BaseObject::camera_);
		body->SetStartPos(GetCenterPosition());
		body->SetPos(GetCenterPosition());
		body->UpExtend();
		playerBodys_.push_back(std::move(body));
	}









	if (playerBodys_.size() > 0)
	{
		playerBodys_.back()->SetEndPos(GetCenterPosition());
	}
}

void Player::ShrinkBody()
{
	if(playerBodys_.back()->GetLength() <= 0)
	{
		playerBodys_.pop_back();
	}

	if (playerBodys_.size() > 0)
	{
		playerBodys_.back()->SetEndPos(GetCenterPosition());
	}
}

#ifdef _DEBUG
void Player::DebugPlayer()
{
	int a = static_cast<int>(moveHistory_.size());
	ImGui::Begin("DebugPlayer");
	ImGui::Text("Start : SPACE  |  Boost : B  |  Return : N");
	ImGui::Text("TimeLimit  : %.2f", extendTimer_);
	ImGui::Text("BoostTimer : %.2f", boostTimer_);
	ImGui::Text("BoostCT    : %.2f", boostCoolTimer_);
	ImGui::Text("HistorySize: %d", a);
	ImGui::Text("createGrassTimer_: %.2f", createGrassTimer_);
	int b = grassGauge_;
	ImGui::Text("grassGauge_: %d", b);
	ImGui::DragFloat3("pos", &worldTransform_.translation_.x);
	ImGui::End();

	if (input_->TriggerKey(DIK_N))
	{
		EntryReturn();
	}
}
#endif // _DEBUG

void Player::BehaviorInitialize()
{
	if (behaviortRquest_)
	{
		// 振る舞いを変更する
		behavior_ = behaviortRquest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_)
		{
		case BehaviorPlayer::Root:
		default:
			BehaviorRootInit();
			break;
		case BehaviorPlayer::Moving:
			BehaviorMovingInit();
			break;
		case BehaviorPlayer::Boost:
			BehaviorBoostInit();
			break;
		case BehaviorPlayer::Return:
			BehaviorReturnInit();
			break;
		}
		// 振る舞いリクエストをリセット
		behaviortRquest_ = std::nullopt;
	}
}

void Player::BehaviorUpdate()
{
	switch (behavior_)
	{
	case BehaviorPlayer::Root:
	default:
		BehaviorRootUpdate();
		break;
	case BehaviorPlayer::Moving:
		BehaviorMovingUpdate();
		break;
	case BehaviorPlayer::Boost:
		BehaviorBoostUpdate();
		break;
	case BehaviorPlayer::Return:
		BehaviorReturnUpdate();
		break;
	}

}

void Player::BehaviorRootInit()
{
	speed_ = 0;
}

void Player::BehaviorRootUpdate()
{
	EntryMove();
}

void Player::BehaviorMovingInit()
{
	speed_ = defaultSpeed_;
}

void Player::BehaviorMovingUpdate()
{
	Move();

	EntryBoost();

	if (0 >= extendTimer_)
	{
		EntryReturn();
	}
}

void Player::BehaviorBoostInit()
{
	speed_ = defaultSpeed_ + boostSpeed_;
	boostTimer_ = kBoostTime_;
}

void Player::BehaviorBoostUpdate()
{
	Move();

	if (0 < boostTimer_)
	{
		boostTimer_ -= deltaTime_;
	}
	else
	{
		behaviortRquest_ = BehaviorPlayer::Moving;
		boostCoolTimer_ = kBoostCT_;
	}

	if (0 >= extendTimer_)
	{
		EntryReturn();
	}
}

void Player::BehaviorReturnInit()
{
	speed_ = defaultSpeed_ + boostSpeed_;
	moveDirection_ = { 0,0,0 };
}

void Player::BehaviorReturnUpdate()
{
	if (moveHistory_.size() > 0)
	{
		if (Length(worldTransform_.translation_ - moveHistory_.back()) > speed_)
		{
			Vector3 direction = Normalize(moveHistory_.back() - worldTransform_.translation_);
			worldTransform_.translation_ += speed_ * direction;
		}
		else
		{
			worldTransform_.translation_ = moveHistory_.back();
			moveHistory_.pop_back();
		}
	}
	else
	{
		behaviortRquest_ = BehaviorPlayer::Root;
	}
	ShrinkBody();
}
