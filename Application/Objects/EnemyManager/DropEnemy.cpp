#include "DropEnemy.h"

DropEnemy::~DropEnemy()
{
	aabbCollider_->~AABBCollider();
}

void DropEnemy::Initialize(Camera* camera)
{
	camera_ = camera;

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("cube.obj");
	obj_->SetMaterialColor({ 1.0f,1.0f,1.0f,1.0f });

	worldTransform_.Initialize();

	InitCollision();
	//InitJson();
}

void DropEnemy::InitCollision()
{
	aabbCollider_ = ColliderFactory::Create<AABBCollider>(
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

void DropEnemy::Draw()
{
	obj_->Draw(camera_, worldTransform_);
}

void DropEnemy::DrawCollision()
{
	aabbCollider_->Draw();
}

void DropEnemy::OnEnterCollision(BaseCollider* self, BaseCollider* other) {

	if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer) ||
		other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kNextFramePlayer))
	{
		isAlive_ = false;
	}
}

void DropEnemy::OnCollision(BaseCollider* self, BaseCollider* other) {

	if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer) ||
		other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kNextFramePlayer))
	{
		isAlive_ = false;
	}
}

void DropEnemy::OnExitCollision(BaseCollider* self, BaseCollider* other) {


}
void DropEnemy::MapChipOnCollision(const CollisionInfo& info) {
	switch (info.blockType) {
	case MapChipType::kBlock:
		isAlive_ = false;
		
		break;
	default:
		break;
	}

	// 衝突方向に応じた処理
	if (info.direction == 4) {  // 下方向の衝突 = 着地
		//isGrounded_ = true;
	}
}

void DropEnemy::Move()
{
	constexpr float gravity = -0.16f;
	constexpr float terminalVelocity = -0.3f;

	velocity_.y += gravity;
	if (velocity_.y < terminalVelocity) {
		velocity_.y = terminalVelocity;
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

void DropEnemy::SetTranslate(Vector3 pos)
{
	worldTransform_.translation_ = pos;
}
