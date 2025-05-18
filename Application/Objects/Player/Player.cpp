#include "Player.h"

#include <cmath>
#include <algorithm>

#include "Collision/Core/CollisionManager.h"
#include "../Generators/OffScreen/OffScreen.h"
#include "Systems/GameTime/GameTIme.h"

#include "../Application/SystemsApp/AppAudio/AudioVolumeManager.h"

#ifdef _DEBUG
#include "imgui.h"
#include "string"
#endif // _DEBUG

bool::Player::isHit = false;

Player::~Player()
{
	//aabbCollider_->~AABBCollider();
	obbCollider_->~OBBCollider();
	nextAabbCollider_->~AABBCollider();
	Audio::GetInstance()->PauseAudio(sourceVoiceGrow);
	Audio::GetInstance()->PauseAudio(sourceVoiceBoost);
	Audio::GetInstance()->PauseAudio(sourceVoiceDamage);
	Audio::GetInstance()->PauseAudio(sourceVoiceEat);
	Audio::GetInstance()->PauseAudio(sourceVoiceYodare);
}

void Player::Initialize(Camera* camera)
{
	input_ = Input::GetInstance();

	BaseObject::camera_ = camera;

	// トランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = { 2.0f,6.0f,0.0f };
	worldTransform_.scale_ = { 0.99f,0.99f,0.99f };
	modelWT_.Initialize();
	modelWT_.parent_ = &worldTransform_;
	modelWT_.rotation_.y = std::numbers::pi_v<float>;
	//worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	nextWorldTransform_.Initialize();
	nextWorldTransform_.translation_ = worldTransform_.translation_;
	nextWorldTransform_.scale_ = worldTransform_.scale_;

	worldTransform_.UpdateMatrix();
	modelWT_.UpdateMatrix();
	nextWorldTransform_.UpdateMatrix();

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("kirin.gltf",true);
	obj_->SetMaterialColor(defaultColorV4_);
	//obj_->SetLoopAnimation(true);  無限ループ再生

	for (size_t i = 0; i < kMaxHP_; ++i)
	{
		std::unique_ptr<PlayerHaert> haert = std::make_unique<PlayerHaert>();
		haert->Initialize(camera_);
		haerts_.push_back(std::move(haert));
	}
	

	/*SphereCollider::SetCamera(BaseObject::camera_);
	SphereCollider::Initialize();*/

	InitCollision();
	InitJson();
	//colliderRct_.height = 2.0f;
	//colliderRct_.width = 2.0f;

	soundDataGrow = Audio::GetInstance()->LoadAudio(L"Resources/Audio/Grow.mp3");
	soundDataBoost = Audio::GetInstance()->LoadAudio(L"Resources/Audio/acceleration.mp3");

	soundDataDamage = Audio::GetInstance()->LoadAudio(L"Resources/Audio/damage.mp3");
	soundDataEat = Audio::GetInstance()->LoadAudio(L"Resources/Audio/eat.mp3");
	soundDataYodare = Audio::GetInstance()->LoadAudio(L"Resources/Audio/yodare.mp3");
	// 音量の設定（0.0f ～ 1.0f）
	//Audio::GetInstance()->SetVolume(sourceVoice, 0.5f);


	emitter_ = std::make_unique<ParticleEmitter>("YodareParticle",worldTransform_.translation_,3);
	emitter_->Initialize("Yodare");
}

void Player::InitCollision()
{
	/*aabbCollider_ = ColliderFactory::Create<AABBCollider>(
		this,
		&worldTransform_,
		camera_,
		static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)
	);*/

	obbCollider_ = ColliderFactory::Create<OBBCollider>(
		this,
		&worldTransform_,
		camera_,
		static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)
	);

	nextAabbCollider_ = ColliderFactory::Create<AABBCollider>(
		this,
		&nextWorldTransform_,
		camera_,
		static_cast<uint32_t>(CollisionTypeIdDef::kNextFramePlayer)
	);
}

void Player::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("Obj", "Resources/JSON/");
	jsonManager_->SetCategory("Objects");
	jsonManager_->SetSubCategory("Player");
	jsonManager_->Register("位置", &worldTransform_.translation_);
	jsonManager_->Register("通常時の移動速度",&defaultSpeed_);
	jsonManager_->Register("ブースト時の速度", &boostSpeed_);
	jsonManager_->Register("帰還時の速度", &returnSpeed_);
	jsonManager_->Register("伸びられるタイマーの限界値", &kTimeLimit_);

	jsonManager_->Register("ブーストの最大効果時間", &kBoostTime_);
	jsonManager_->Register("ブーストのクールタイム", &kBoostCT_);
	jsonManager_->Register("草が詰まるまでの時間", &kCreateGrassTime_);
	jsonManager_->Register("無敵時間", &kInvincibleTime_);

	jsonManager_->Register("HPの最大値", &kMaxHP_);
	jsonManager_->Register("草ゲージの最大値", &kMaxGrassGauge_);

	jsonManager_->Register("通常の草の回復量(sec)", &grassTime_);
	jsonManager_->Register("大きい草の回復量(sec)", &largeGrassTime_);

	jsonManager_->Register("方向転換できるまでの距離", &moveInterval_);

	jsonCollider_ = std::make_unique<JsonManager>("playerCollider", "Resources/JSON/");
	//aabbCollider_->InitJson(jsonCollider_.get());
}

void Player::Update()
{
	canSpitting_ = false;
	beforebehavior_ = behavior_;

	// Bボタンで一回だけ「食べるアニメーション」再生
	if (!isEating_ && input_->IsPadTriggered(0, GamePadButton::B)) {
		obj_->ChangeModel("kirin_yodare.gltf", true);
		isEating_ = true;
	} else if (isEating_ && obj_->GetModel()->IsAnimationPlayFinished()) {
		obj_->ChangeModel("kirin.gltf", true);
		isEating_ = false;
	}

	// 各行動の初期化
	BehaviorInitialize();

	// 各行動の更新
	BehaviorUpdate();

	// 草ゲージの更新
	GrassGaugeUpdate();

	ExtendBody();

	IsPopGrass();

	DamageProcessBodys();

	TimerManager();

	HeartPos();

	HeadDir();

	UpdateMatrices();
	
	//aabbCollider_->Update();
	obj_->UpdateAnimation();
	obbCollider_->Update();
	nextAabbCollider_->Update();
	
#ifdef _DEBUG
	DebugPlayer();
#endif // _DEBUG
}

void Player::Draw()
{
	

	for (const auto& body : playerBodys_) 
	{
		body->Draw();
	}

	for (const auto& body : stuckGrassList_) 
	{
		body->Draw();
	}

	for (size_t i = 0; i < drawCount_; ++i)
	{
		haerts_[i]->Draw();
	}
}

void Player::DrawAnimation()
{
	obj_->Draw(camera_, modelWT_);
}

void Player::DrawCollision()
{
	//aabbCollider_->Draw();
	obbCollider_->Draw();
	nextAabbCollider_->Draw();
	for (const auto& body : playerBodys_) {
		body->DrawCollision();
	}
	for (const auto& body : stuckGrassList_) {
		body->DrawCollision();
	}
}

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

void Player::Reset()
{
	extendTimer_ = 0;
	boostCoolTimer_ = 0;
	boostTimer_ = 0;
	createGrassTimer_ = 0;
	invincibleTimer_ = 0;
	playerBodys_.clear();
	moveHistory_.clear();
	stuckGrassList_.clear();
	behaviortRquest_ = BehaviorPlayer::Root;
}


#pragma region // 判定


void Player::OnEnterCollision(BaseCollider* self, BaseCollider* other)
{
	if (behavior_ != BehaviorPlayer::Return && self->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer))
	{
		if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kGrass)) // 草を食べたら
		{
			// オーディオの再生
			sourceVoiceEat = Audio::GetInstance()->SoundPlayAudio(soundDataEat, false);
			AudioVolumeManager::GetInstance()->SetSourceToSubmix(sourceVoiceEat, kSE);
			if (kMaxGrassGauge_ > grassGauge_ && createGrassTimer_ <= 0)
			{
				if (dynamic_cast<AABBCollider*>(other)->GetWorldTransform().scale_.x <= /*GetRadius()*/1.1f)
				{
					extendTimer_ = (std::min)(kTimeLimit_, extendTimer_ + grassTime_);
				} 
				else
				{
					extendTimer_ = (std::min)(kTimeLimit_, extendTimer_ + largeGrassTime_);
				}
				grassGauge_++;
				if (kMaxGrassGauge_ <= grassGauge_)
				{
					createGrassTimer_ = kCreateGrassTime_;
					isCreateGrass_ = true;
				}
			}
			else
			{
				if (dynamic_cast<AABBCollider*>(other)->GetWorldTransform().scale_.x <= /*GetRadius()*/1.1f)
				{
					extendTimer_ = (std::min)(kTimeLimit_, extendTimer_ + (grassTime_ / 2.0f));
				}
				else
				{
					extendTimer_ = (std::min)(kTimeLimit_, extendTimer_ + (largeGrassTime_ / 2.0f));
				}
			}
		}

		if (behavior_ != BehaviorPlayer::Boost)
		{
			if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kBranch))
			{
				TakeDamage();
			}
		}
	}
}

void Player::OnCollision(BaseCollider* self, BaseCollider* other)
{
	if (behavior_ == BehaviorPlayer::Moving || behavior_ == BehaviorPlayer::Boost)
	{
		if (self->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer))
		{
			if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kGrowthArea)) // 草の成長エリア
			{
				canSpitting_ = true;
				if (input_->TriggerKey(DIK_Q) || input_->IsPadTriggered(0, GamePadButton::B))
				{
						// 唾を吐く
						sourceVoiceYodare = Audio::GetInstance()->SoundPlayAudio(soundDataYodare, false);
						AudioVolumeManager::GetInstance()->SetSourceToSubmix(sourceVoiceYodare, kSE);
						//emitter_->EmitFromTo(worldTransform_.translation_, other->GetWorldTransform().translation_);
						// オーディオの再生
						sourceVoiceGrow = Audio::GetInstance()->SoundPlayAudio(soundDataGrow, false);
						AudioVolumeManager::GetInstance()->SetSourceToSubmix(sourceVoiceGrow, kSE);
				}
			}
		}
	}
}

void Player::OnExitCollision(BaseCollider* self, BaseCollider* other)
{
	if (self->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer))
	{
		if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kGrowthArea)) // 草の成長エリア
		{
			canSpitting_ = false;
		}
		if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy))
		{
			isHit = false;
		}
	}
}



////////////////////////////////////////////////////////////
//
//
// 				こんな感じに使うよっていう例置いとく
//
// 
///////////////////////////////////////////////////////////
void Player::OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir)
{
	if(self->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer))
	{
		// 敵からダメージ受ける
		if (behavior_ == BehaviorPlayer::Moving)
		{
			if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy))
			{
				HitDirection hitDir = Collision::GetSelfLocalHitDirection(self, other);
				HitDirection otherDir = Collision::GetSelfLocalHitDirection(other, self);
				if (otherDir != HitDirection::None && !isHit)
				{
					isHit = true;
					if (otherDir == HitDirection::Back)
					{
						TakeDamage();
					}
					else
					{
						Eliminate();
					}
				}
			}
		}
	}

	if (self->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kNextFramePlayer))
	{
		if (behavior_ == BehaviorPlayer::Moving || behavior_ == BehaviorPlayer::Boost)
		{
			if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerBody))
			{
				HitDirection hitDir = Collision::GetSelfLocalHitDirection(self, other);
				if (hitDir == HitDirection::Top)
				{
					isCollisionBody = true;
					TakeDamage();
				}
			}

		}
	}
}

#pragma endregion


void Player::UpdateMatrices()
{
	worldTransform_.UpdateMatrix();
	modelWT_.UpdateMatrix();
	nextWorldTransform_.UpdateMatrix();
	for (const auto& body : playerBodys_) 
	{
		body->Update();
	}
	for (const auto& body : stuckGrassList_) 
	{
		body->Update();
	}
	for (const auto& haert : haerts_)
	{
		haert->Update();
	}
}

void Player::Move()
{
	velocity_ = { 0.0f,0.0f,0.0f };
	beforeDirection_ = moveDirection_;

	if (input_->IsControllerConnected())
	{
	}
	stick = input_->GetLeftStickInput(0);
	if (std::abs(stick.x) < threshold && std::abs(stick.y) < threshold) {
		stick = {};
	}

	if(Length(moveHistory_.back() - worldTransform_.translation_) >= moveInterval_ || isCollisionBody) ChangeDir();
	

	moveDirection_ = Normalize(moveDirection_);

	if(beforeDirection_ == moveDirection_)
	{
		velocity_ += moveDirection_ * speed_;
	}


	Vector3 newPos = worldTransform_.translation_;
	newPos = worldTransform_.translation_ + velocity_;

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

	if (isCollisionBody && beforeDirection_ == moveDirection_)
	{
		newPos = worldTransform_.translation_;
		velocity_ = { 0,0,0 };
		isCollisionBody = false;
	}

	worldTransform_.translation_ = newPos;
	nextWorldTransform_.translation_ = newPos + velocity_;

	ExtendBody();

}

void Player::ChangeDir()
{
	if ((input_->TriggerKey(DIK_W) || input_->TriggerKey(DIK_UP)) &&
		moveDirection_ != Vector3{ 0,1,0 } &&
		moveDirection_ != Vector3{ 0,-1,0 })
	{
		UpBody();
	}
	else if ((input_->TriggerKey(DIK_S) || input_->TriggerKey(DIK_DOWN)) &&
		moveDirection_ != Vector3{ 0,-1,0 } &&
		moveDirection_ != Vector3{ 0,1,0 })
	{
		DownBody();
	}
	else if ((input_->TriggerKey(DIK_A) || input_->TriggerKey(DIK_LEFT)) &&
		moveDirection_ != Vector3{ -1,0,0 } &&
		moveDirection_ != Vector3{ 1,0,0 })
	{
		LeftBody();
	}
	else if ((input_->TriggerKey(DIK_D) || input_->TriggerKey(DIK_RIGHT)) &&
		moveDirection_ != Vector3{ 1,0,0 } &&
		moveDirection_ != Vector3{ -1,0,0 })
	{
		RightBody();
	}
	else  if (std::abs(stick.x) > std::abs(stick.y) && (stick.x != 0 || stick.y != 0))
	{
		if (stick.x > 0 &&
			moveDirection_ != Vector3{ 1,0,0 } &&
			moveDirection_ != Vector3{ -1,0,0 })
		{
			RightBody();
		}
		else if (moveDirection_ != Vector3{ -1,0,0 } &&
			moveDirection_ != Vector3{ 1,0,0 })
		{
			LeftBody();
		}
	}
	else if (stick.x != 0 || stick.y != 0)
	{
		if (stick.y > 0 &&
			moveDirection_ != Vector3{ 0,1,0 } &&
			moveDirection_ != Vector3{ 0,-1,0 })
		{
			UpBody();
		}
		else if (moveDirection_ != Vector3{ 0,-1,0 } &&
			moveDirection_ != Vector3{ 0,1,0 })
		{
			DownBody();
		}
	}
}

#pragma region // 体が伸びる向き決定
void Player::UpBody()
{
	moveDirection_ = { 0,1,0 };
	moveHistory_.push_back(worldTransform_.translation_);
	// 体の出現
	ExtendBody();
	std::unique_ptr<PlayerBody> body = std::make_unique<PlayerBody>();
	body->Initialize(BaseObject::camera_);
	body->SetStartPos(GetCenterPosition());
	body->SetPos(GetCenterPosition());
	body->UpExtend();
	playerBodys_.push_back(std::move(body));
}

void Player::DownBody()
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

void Player::LeftBody()
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

void Player::RightBody()
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
#pragma endregion

void Player::EntryMove()
{

	if (input_->IsControllerConnected())
	{
	}
	stick = input_->GetLeftStickInput(0);
	if (std::abs(stick.x) < threshold && std::abs(stick.y) < threshold) {
		stick = {};
	}

	if ((input_->TriggerKey(DIK_W) || input_->TriggerKey(DIK_UP)))
	{
		behaviortRquest_ = BehaviorPlayer::Moving;
		moveDirection_ = { 0,1,0 };
		extendTimer_ = kTimeLimit_;
		moveHistory_.push_back(worldTransform_.translation_);
	}
	else if (std::abs(stick.x) < std::abs(stick.y) && (stick.x != 0 || stick.y != 0))
	{
		if (stick.y > 0)
		{
			behaviortRquest_ = BehaviorPlayer::Moving;
			moveDirection_ = { 0,1,0 };
			extendTimer_ = kTimeLimit_;
			moveHistory_.push_back(worldTransform_.translation_);
		}
	}
#ifdef _DEBUG
#endif // _DEBUG
}

void Player::EntryBoost()
{
	if(0 >= boostCoolTimer_)
	{
		if (input_->TriggerKey(DIK_E) || input_->IsPadTriggered(0, GamePadButton::A))
		{
			behaviortRquest_ = BehaviorPlayer::Boost;
		}
#ifdef _DEBUG
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
	if (0 < boostTimer_)
	{
		boostTimer_ -= deltaTime_;
	}
	if (0 < createGrassTimer_)
	{
		createGrassTimer_ -= deltaTime_;
	}
	if (0 < invincibleTimer_)
	{
		static int time = 0;
		if (time > 3)
		{
			isRed_ = false;
			time = 0;
		}

		if (isRed_)
		{
			changeColor_ = { 1,0,0 };
		}
		else
		{
			if (time == 0)
			{
				if(changeColor_ == defaultColorV3_)
				{
					changeColor_ = { 1,1,1 };
				}
				else
				{
					changeColor_ = defaultColorV3_;
				}
			}
		}

		time++;
		invincibleTimer_ -= deltaTime_;

		if (0 >= invincibleTimer_)
		{
			time = 0;
			changeColor_ = defaultColorV3_;
		}

		obj_->SetMaterialColor(changeColor_);

		for (const auto& body : playerBodys_) 
		{
			body->SetColor(changeColor_);
		}
	}
}

void Player::TimerZero()
{
	extendTimer_ = 0;
	boostCoolTimer_ = 0;
	boostTimer_ = 0;
	createGrassTimer_ = 0;
	invincibleTimer_ = 0;
}

bool Player::IsPopGrass()
{
	if (0 >= createGrassTimer_ && isCreateGrass_)
	{
		grassGauge_ = 0;
		std::unique_ptr<StuckGrass> stuck = std::make_unique<StuckGrass>();
		stuck->Initialize(camera_);
		stuck->SetPlayer(this);
		stuck->SetPos(worldTransform_.translation_);
		stuckGrassList_.push_back(std::move(stuck));
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

	if (playerBodys_.size() > 0)
	{
		playerBodys_.back()->SetEndPos(GetCenterPosition());
		if (playerBodys_.back()->GetLength() <= 0)
		{
			playerBodys_.pop_back();
		}
	}
}

void Player::TakeDamage()
{
	if(behavior_ != BehaviorPlayer::Return)
	{
		if (HP_ > 0 && invincibleTimer_ <= 0)
		{
			HP_--;
			isRed_ = true;
			camera_->Shake(0.3f, { -0.5f,-0.5f }, { 0.5f,0.5f });

			// オーディオの再生
			sourceVoiceDamage = Audio::GetInstance()->SoundPlayAudio(soundDataDamage, false);
			AudioVolumeManager::GetInstance()->SetSourceToSubmix(sourceVoiceDamage, kSE);
			if (HP_ <= 0)
			{
				extendTimer_ = 0;
			}
			else
			{
				invincibleTimer_ = kInvincibleTime_;
			}
		}
	}
}

void Player::DamageProcessBodys()
{
	for (auto&& body : playerBodys_)
	{
		if (0 < boostTimer_)
		{
			body->SetIsInvincible(true);
		}
		else
		{
			body->SetIsInvincible(false);
		}

		if (body->IsTakeDamage())
		{
			TakeDamage();
		}
	}

}

void Player::GrassGaugeUpdate()
{
	if (grassGauge_ < kMaxGrassGauge_)
	{
		UIGauge_ = std::clamp(static_cast<float>(grassGauge_) / static_cast<float>(kMaxGrassGauge_), 0.0f, 1.0f);
	}
	else
	{
		UIGauge_ = std::clamp(1.0f * (createGrassTimer_ / kCreateGrassTime_), 0.0f, 1.0f);
	}
}

void Player::Eliminate()
{
	extendTimer_ = (std::min)(kTimeLimit_, extendTimer_ + grassTime_);
}

void Player::HeartPos()
{
	drawCount_ = 0;
	if (HP_ > 0)
	{
		std::vector<PointWithDirection> result;
		const float length = 1.4f;
		float targetDistance = length;
		float accumulated = 0.0f;
		std::list<Vector3> v = moveHistory_;
		v.push_back(worldTransform_.translation_);
		auto it = v.rbegin();
		if (it == v.rend()) return;

		Vector3 prev = *it;
		++it;

		while (it != v.rend() && result.size() < HP_) {
			Vector3 curr = *it;
			float segLen = Length(prev - curr);

			if (accumulated + segLen >= targetDistance) {
				float remain = targetDistance - accumulated;
				float t = remain / segLen;

				// 補間して位置を算出
				Vector3 position = prev + (curr - prev) * t;
				position.z -= 1.0f;

				// 進行方向（XY平面）からラジアン角を計算
				Vector3 dir = Normalize(curr - prev); // 方向ベクトル（単位ベクトル）
				float angle = std::atan2(dir.y, dir.x);  // XY平面での角度

				result.push_back({ position, angle });

				targetDistance += length;
			}
			else {
				accumulated += segLen;
				prev = curr;
				++it;
			}
		}
		drawCount_ = result.size();
		for (size_t i = 0; i < result.size(); ++i)
		{
			haerts_[i]->SetPos(result[i].position);
			haerts_[i]->SetRotaZ(result[i].radian + (std::numbers::pi_v<float> / 2.0f));
		}
	}

	// resultに3つの配置場所が入っている（足りなければ少ない場合もある）
}

#ifdef _DEBUG
void Player::DebugPlayer()
{
	int a = static_cast<int>(moveHistory_.size());
	ImGui::Begin("DebugPlayer");
	ImGui::Text("Start : SPACE  |  Boost : E or Pad:A  |  Return : N");
	ImGui::Text("TimeLimit  : %.2f", extendTimer_);
	ImGui::Text("BoostTimer : %.2f", boostTimer_);
	ImGui::Text("BoostCT    : %.2f", boostCoolTimer_);
	ImGui::Text("HistorySize: %d", a);
	ImGui::Text("createGrassTimer_: %.2f", createGrassTimer_);
	ImGui::DragFloat3("Transration", &worldTransform_.translation_.x);
	int b = grassGauge_;
	ImGui::Text("grassGauge_: %d", b);
	ImGui::Text("isCollisionBody: %d", isCollisionBody);
	int c = HP_;
	ImGui::Text("HP : %d", c);
	ImGui::Text("Inv : %.2f", invincibleTimer_);
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
	grassGauge_ = 0;
	playerBodys_.clear();
	isCollisionBody = false;
	HP_ = kMaxHP_;
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
	speed_ = boostSpeed_;
	boostTimer_ = kBoostTime_;
	invincibleTimer_ = kBoostTime_; // ブースト中無敵に
	sourceVoiceBoost = Audio::GetInstance()->SoundPlayAudio(soundDataBoost, false);
	OffScreen::RadialBlurPrams blurPrams;
	blurPrams.center = { 0.0f,0.0f };
	blurPrams.direction = { 0.0f,-1.0f };
	blurPrams.isRadial = false;
	blurPrams.sampleCount = 5;
	blurPrams.width = 0.01f;

	OffScreen::GetInstance()->StartBlurMotion(blurPrams);
	AudioVolumeManager::GetInstance()->SetSourceToSubmix(sourceVoiceBoost, kSE);
}

void Player::BehaviorBoostUpdate()
{
	Move();

	OffScreen::GetInstance()->UpdateBlur(GameTime::GetDeltaTime());

	if (0 >= boostTimer_)
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
	speed_ = returnSpeed_;
	moveDirection_ = { 0,0,0 };
	isCollisionBody = false;
	TimerZero();
	OffScreen::GetInstance()->SetEffectType(OffScreen::OffScreenEffectType::Copy);
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

		stuckGrassList_.remove_if([](const std::unique_ptr<StuckGrass>& s)
			{
				return s->IsDelete();
			});
	}
	else
	{
		stuckGrassList_.clear();
		behaviortRquest_ = BehaviorPlayer::Root;
	}
	nextWorldTransform_.translation_ = worldTransform_.translation_;
	ShrinkBody();
}
