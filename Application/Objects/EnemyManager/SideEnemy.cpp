#include "SideEnemy.h"
#include "../Player/Player.h"
#include "Easing.h"


// DX
#include <DirectXMath.h>

#include "Collision/Core/CollisionManager.h"

bool::SideEnemy::isHit = false;

SideEnemy::~SideEnemy()
{
	obbCollider_->~OBBCollider();
}

void SideEnemy::Initialize(Camera* camera)
{
	camera_ = camera;

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("bard.obj");
	obj_->SetMaterialColor({ 1.0f,1.0f,1.0f,1.0f });

	worldTransform_.Initialize();
	startPos_ = worldTransform_.translation_;

	InitCollision();
	//InitJson();
}

void SideEnemy::InitCollision()
{
	obbCollider_ = ColliderFactory::Create<OBBCollider>(
		this,
		&worldTransform_,
		camera_,
		static_cast<uint32_t>(CollisionTypeIdDef::kEnemy)
	);
}

void SideEnemy::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("SideEnemy", "Resources/JSON/EnemyManager/SideEnemy");
	jsonManager_->SetCategory("Enemy");

	jsonCollider_ = std::make_unique<JsonManager>("SideEnemyCollider", "Resources/JSON/Colliders");
	jsonCollider_->SetCategory("Colliders");
}

void SideEnemy::KnockBackDir()
{
	// 吹っ飛び方向の計算（プレイヤー中心 - 敵中心 → 正規化）
	Vector3 direction = Normalize(GetCenterPosition() - player_->GetCenterPosition());
	ApplyKnockback(direction, 0.6f); // ← 距離調整可能
}

void SideEnemy::Update()
{
	FaintUpdate(player_);
	if (!isAlive_) {
		//aabbCollider_->~AABBCollider();
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

	KnockBack();
	worldTransform_.UpdateMatrix();
	//aabbCollider_->Update();
	obbCollider_->Update();
}

void SideEnemy::Draw()
{
	if (!isFaint_) // 攻撃を食らったら次まで描画しない
	{
		obj_->Draw(camera_, worldTransform_);
	}
}

void SideEnemy::DrawCollision()
{
	//aabbCollider_->Draw();
	obbCollider_->Draw();
}

void SideEnemy::OnEnterCollision(BaseCollider* self, BaseCollider* other) 
{

	if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer) ||
		other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerBody))
	{
		moveRight_ = moveRight_ ? false : true;
	}
}

void SideEnemy::OnCollision(BaseCollider* self, BaseCollider* other) 
{

}

void SideEnemy::OnExitCollision(BaseCollider* self, BaseCollider* other) 
{

	if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer))
	{
		isHit = false;
	}
}

void SideEnemy::OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir)
{
	if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer))
	{
		if (player_->behavior_ == BehaviorPlayer::Boost && !isHit)
		{
			isHit = true;
			TakeAttack();
			KnockBackDir();
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

void SideEnemy::MapChipOnCollision(const CollisionInfo& info) {
	switch (info.blockType) {
	case MapChipType::kBlock:

		
		break;
	default:
		break;
	}

	// 衝突方向に応じた処理
	if (info.direction == 1) {  // 下方向の衝突 = 着地
		moveRight_ = false;
	}
	if (info.direction == 2) {  // 下方向の衝突 = 着地
		moveRight_ = true;
	}
}


/// <summary>
///  移動処理（回転補間による滑らかな方向転換）
/// </summary>
void SideEnemy::Move() {
	// ターゲット回転角度（Y軸）
	float targetRotationY = moveRight_
		? DirectX::XMConvertToRadians(270.0f)
		: DirectX::XMConvertToRadians(90.0f);

	float t = 0.7f;
	t = Easing::easeInExpo(t);
	worldTransform_.rotation_.y = Lerp(worldTransform_.rotation_.y, targetRotationY, t);
	// 移動速度の設定（方向に応じて）
	velocity_.x = moveRight_ ? speed_ : -speed_;

	// 進行方向を使って前方チェック座標を計算
	Vector3 checkPos = worldTransform_.translation_;
	checkPos.x += (moveRight_ ? 1.0f : -1.0f);
}


void SideEnemy::Reset(Vector3& pos)
{
	velocity_ = { 0.0f, 0.0f, 0.0f };
	worldTransform_.translation_ = pos;
	startPos_ = pos;
	moveRight_ = true;
	worldTransform_.UpdateMatrix();
}

Vector3 SideEnemy::GetCenterPosition() const
{
	return {
		worldTransform_.matWorld_.m[3][0],
		worldTransform_.matWorld_.m[3][1],
		worldTransform_.matWorld_.m[3][2]
	};
}

void SideEnemy::SetTranslate(const Vector3& pos)
{
	worldTransform_.translation_ = pos;
	startPos_ = pos;
}

