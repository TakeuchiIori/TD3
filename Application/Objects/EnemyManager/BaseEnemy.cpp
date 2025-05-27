#include "BaseEnemy.h"

#include "Player/Player.h"

#include "Systems/GameTime/GameTIme.h"

void BaseEnemy::FaintUpdate(Player* player)
{
	if (isFaint_) {
		if (player->behavior_ == BehaviorPlayer::Return &&
			player->beforebehavior_ != player->behavior_)
		{
			obj_->SetMaterialColor(Vector3{ 1.0f,1.0f,1.0f });
			obbCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kEnemy));
			isFaint_ = false;
		}
	}
	if (isTakeAttack_)
	{
		if (player->behavior_ == BehaviorPlayer::Return &&
			player->beforebehavior_ != player->behavior_)
		{

			// obbCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kEnemy));
			isTakeAttack_ = false;
			isFaint_ = true;
		}
	}
}

void BaseEnemy::KnockBack()
{
	// === ノックバック中の処理 ===
	if (knockbackTimer_ > 0.0f && isTakeAttack_) {
		kirisutegomennWT_.translation_ += knockbackVelocity_;
		knockbackVelocity_ *= 0.9f; // 減衰
		knockbackTimer_ -= 1.0f / 60.0f;
		//obbCollider_->Update();
		if (isSpinning_) {
			kirisutegomennWT_.rotation_.y += angularVelocityY_ * GameTime::GetDeltaTime();
		}
		return;
	}
	else
	{
		isSpinning_ = false;
	}
}

void BaseEnemy::kirisuteUpdate()
{
	if (!isFaint_ && !isTakeAttack_)
	{
		kirisutegomennWT_.scale_ = worldTransform_.scale_;
		kirisutegomennWT_.rotation_ = worldTransform_.rotation_;
		kirisutegomennWT_.translation_ = worldTransform_.translation_;
	}
	else
	{

	}
	kirisutegomennWT_.UpdateMatrix();
}
