#include "SideEnemy.h"
#include "../Player/Player.h"

// DX
#include <DirectXMath.h>

SideEnemy::~SideEnemy()
{
	aabbCollider_->~AABBCollider();
}

void SideEnemy::Initialize(Camera* camera)
{
	camera_ = camera;

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("needle_Body.obj");
	obj_->SetMaterialColor({ 1.0f,1.0f,1.0f,1.0f });

	worldTransform_.Initialize();
	startPos_ = worldTransform_.translation_;

	InitCollision();
	//InitJson();
}

void SideEnemy::InitCollision()
{
	aabbCollider_ = ColliderFactory::Create<AABBCollider>(
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

void SideEnemy::Update()
{
	if (!isAlive_) {
		aabbCollider_->~AABBCollider();
		return;
	}


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
	worldTransform_.UpdateMatrix();
	aabbCollider_->Update();
}

void SideEnemy::Draw()
{
	obj_->Draw(camera_, worldTransform_);
}

void SideEnemy::DrawCollision()
{
	aabbCollider_->Draw();
}

void SideEnemy::OnEnterCollision(BaseCollider* self, BaseCollider* other) {

	//if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer) ||
	//	other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerBody))
	//{
	//	isAlive_ = false;
	//}
}

void SideEnemy::OnCollision(BaseCollider* self, BaseCollider* other) {

	// プレイヤーの期間中は跳ね返るだけでダメージ無し
	if ((other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer) || 
		other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerBody)) && 
		player_->behavior_ == BehaviorPlayer::Return)
	{
		moveRight_ = moveRight_ ? false : true;
	}
}

void SideEnemy::OnExitCollision(BaseCollider* self, BaseCollider* other) {

}

void SideEnemy::OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir)
{
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


void SideEnemy::Move()
{

	// 左右移動
	if (moveRight_) {
		velocity_.x = speed_;
		worldTransform_.rotation_.y = 0.0f; // 右向き
	} else {
		velocity_.x = -speed_;
		worldTransform_.rotation_.y = DirectX::XMConvertToRadians(180.0f); // 左向き（180度回転）
	}

	// 現在のワールド座標を使って、前方のマップを調べる
	Vector3 checkPos = worldTransform_.translation_;
	checkPos.x += (moveRight_ ? 1.0f : -1.0f); // 進行方向に1.0fずらす

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

