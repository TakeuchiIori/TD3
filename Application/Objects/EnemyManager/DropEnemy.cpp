#include "DropEnemy.h"
#include "../Player/Player.h"
#include "Easing.h"

#include "Collision/Core/CollisionManager.h"

bool::DropEnemy::isHit = false;

DropEnemy::~DropEnemy()
{
	obbCollider_->~OBBCollider();
}

void DropEnemy::Initialize(Camera* camera)
{
	camera_ = camera;

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("bard.obj");
	obj_->SetMaterialColor({ 1.0f,1.0f,1.0f,1.0f });

	worldTransform_.Initialize();

	InitCollision();
	//InitJson();
}

void DropEnemy::InitCollision()
{
	obbCollider_ = ColliderFactory::Create<OBBCollider>(
		this,
		&worldTransform_,
		camera_,
		static_cast<uint32_t>(CollisionTypeIdDef::kEnemy)
	);
}

void DropEnemy::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("DropEnemy", "Resources/JSON/EnemyManager/DropEnemy");
	jsonManager_->SetCategory("Enemy");

	jsonCollider_ = std::make_unique<JsonManager>("DropEnemyCollider", "Resources/JSON/Colliders");
	jsonCollider_->SetCategory("Colliders");
}

void DropEnemy::Update()
{
	FaintUpdate(player_);
	if (!isAlive_) {
		obbCollider_->~OBBCollider();
		return;
	}

	if (!IsStop()) // 攻撃を食らったら次まで動かない
	{
		Move();


		Vector3 newPos = worldTransform_.translation_ + velocity_;
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
	}
	worldTransform_.UpdateMatrix();
	obbCollider_->Update();
}

void DropEnemy::Draw()
{
	if (!IsStop()) // 攻撃を食らったら次まで描画しない
	{
		obj_->Draw(camera_, worldTransform_);
	}
}

void DropEnemy::DrawCollision()
{
	obbCollider_->Draw();
}

void DropEnemy::OnEnterCollision(BaseCollider* self, BaseCollider* other) {

	// プレイヤーの期間中は跳ね返るだけでダメージ無し
	if ((other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer) ||
		other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerBody)))
	{
		isInversion_ = isInversion_ ? false : true;
	}
}

void DropEnemy::OnCollision(BaseCollider* self, BaseCollider* other) {

	if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer) ||
		other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerBody))
	{
	}
}

void DropEnemy::OnExitCollision(BaseCollider* self, BaseCollider* other) {


}
void DropEnemy::OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir)
{
	if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer))
	{
		if (player_->behavior_ == BehaviorPlayer::Boost)
		{
			isHit = true;
			TakeAttack();
		}
		HitDirection selfDir = Collision::GetSelfLocalHitDirection(self, other);
		HitDirection otherDir = Collision::GetSelfLocalHitDirection(other, self);

		if (player_->behavior_ == BehaviorPlayer::Moving)
		{
			if (selfDir != HitDirection::None && !isHit)
			{
				isHit = true;
				if (selfDir != HitDirection::Back)
				{
					isTakeAttack_ = true;
					TakeAttack();
				}
			}
		}
	}
}

void DropEnemy::MapChipOnCollision(const CollisionInfo& info) {
	switch (info.blockType) {
	case MapChipType::kBlock:
		
		
		break;
	default:
		break;
	}

	// 衝突方向に応じた処理
	if (info.direction == 3) {  // 下の面
		isInversion_ = false;
	}
	if (info.direction == 4) {  // 上の面
		isInversion_ = true;
	}
}

void DropEnemy::Move()
{
	// ターゲット回転角度（X軸を使って上下向きに傾ける）
	float targetRotationX = isInversion_
		? DirectX::XMConvertToRadians(90.0f)  // 上に向かうとき
		: DirectX::XMConvertToRadians(-90.0f);  // 下に向かうとき

	// 補間して自然な回転にする
	float t = 0.7f;
	t = Easing::easeInExpo(t);
	worldTransform_.rotation_.x = Lerp(worldTransform_.rotation_.x, targetRotationX, t);

	// 落下処理
	constexpr float gravity = -0.16f;
	constexpr float terminalVelocity = -0.05f;

	velocity_.y += gravity;

	// 上下の向きに応じて速度制限
	if (velocity_.y < terminalVelocity) {
		velocity_.y = isInversion_ ? -terminalVelocity : terminalVelocity;
	}
}

void DropEnemy::Reset(Vector3& pos)
{
	velocity_ = { 0.0f, 0.0f, 0.0f };
	moveDirection_ = { 0.0f, 0.0f, 0.0f };
	speed_ = defaultSpeed_;
	isMove_ = false;

	worldTransform_.translation_ = pos;
	worldTransform_.UpdateMatrix();
}

Vector3 DropEnemy::GetCenterPosition() const
{
	return {
		worldTransform_.matWorld_.m[3][0],
		worldTransform_.matWorld_.m[3][1],
		worldTransform_.matWorld_.m[3][2]
	};
}

void DropEnemy::SetTranslate(const Vector3& pos)
{
	worldTransform_.translation_ = pos;
}
