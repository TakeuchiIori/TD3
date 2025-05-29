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

void BaseEnemy::IconInit()
{
	dashIconSprite_ = std::make_unique<Sprite>();
	dashIconSprite_->Initialize(dashIconPath_);
	dashIconSprite_->SetSize(dashIconSprite_->GetTextureSize() * offsetScale_);
}

void BaseEnemy::IconUpdate()
{
	if (player_->CanBoost())
	{
		iconVisible_ = true;
	}
	else
	{
		iconVisible_ = false;
	}
	if (iconVisible_)
	{
		Vector3 pos = worldTransform_.translation_;
		Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1);
		Matrix4x4 matViewProjectionViewport = Multiply(camera_->GetViewMatrix(), Multiply(camera_->GetProjectionMatrix(), matViewport));
		pos = Transform(pos, matViewProjectionViewport);
		pos += offsetPos_;
		dashIconSprite_->SetPosition(pos);
		dashIconSprite_->Update();
	}
}

void BaseEnemy::SoundInit()
{
	soundDataFly = Audio::GetInstance()->LoadAudio(L"Resources/Audio/fly.mp3");
}

void BaseEnemy::DrawSprite()
{
	if (iconVisible_ && !isFaint_ && !isTakeAttack_)
	{
		dashIconSprite_->Draw();
	}
}
