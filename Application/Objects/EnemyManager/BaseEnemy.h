#pragma once
// Engine
#include "../BaseObject/BaseObject.h"
#include "Systems/MapChip/MapChipCollision.h"
#include "Loaders/Json/JsonManager.h"
#include "Systems/Audio/Audio.h"
#include "Sprite/Sprite.h"

#include "../Application/SystemsApp//AppAudio/AudioVolumeManager.h"

// Collision
#include "Collision/Sphere/SphereCollider.h"
#include "Collision/OBB/OBBCollider.h"
#include "Collision/AABB/AABBCollider.h"
#include "Collision/Core/ColliderFactory.h"
#include "Collision/Core/CollisionDirection.h"

class Player;

class BaseEnemy : public BaseObject
{
public:

	virtual ~BaseEnemy() {};

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="camera"></param>
	void Initialize(Camera* camera) override = 0;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override = 0;

	/// <summary>
	/// 更新
	/// </summary>
	void Draw() override = 0;

	/// <summary>
	///	判定の描画
	/// </summary>
	virtual void DrawCollision() = 0;

	void DrawSprite();

	/// <summary>
	///	判定の描画
	/// </summary>
	virtual void Reset(Vector3& pos) = 0;

	bool IsDead() const { return !isAlive_; }

	virtual Vector3 GetTranslate() const = 0;
	virtual void SetTranslate(const Vector3& pos) = 0;

	virtual float GetMoveSpeed() const = 0;
	virtual void SetMoveSpeed(float speed) = 0;

	virtual float GetFallSpeed() const { return 0.0f; }
	virtual void SetFallSpeed(float /*speed*/) {}

	virtual const char* GetTypeName() const = 0;

	// 攻撃を食らったら次まで気絶
	bool IsStop() const 
	{ 
		if(isTakeAttack_ || isFaint_)
		{
			return true;
		}
		return false;

	}

	void FaintUpdate(Player* player);

	void ApplyKnockback(const Vector3& direction, float power) 
	{
		knockbackVelocity_ = direction * power;
		knockbackTimer_ = kKnockbackDuration_;
	}

	void KnockBack();

	void SetIsStop(bool is) { isStop_ = is; }

protected:
	// 攻撃を受けた時
	void TakeAttack()
	{ 
		sourceVoiceFly = Audio::GetInstance()->SoundPlayAudio(soundDataFly);
		AudioVolumeManager::GetInstance()->SetSourceToSubmix(sourceVoiceFly, kSE);
		obj_->SetMaterialColor(Vector3{ 0.3f,0.3f,0.3f });
		obbCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kNone));
		if(isTakeAttack_)
		{
			isTakeAttack_ = false;
			isFaint_ = true;
		}
		else
		{
			isTakeAttack_ = true;
		}
	}

	void kirisuteUpdate();

	void IconInit();
	void IconUpdate();

	void SoundInit();

public:

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="self"></param>
	/// <param name="other"></param>
	virtual void OnEnterCollision(BaseCollider* self, BaseCollider* other) = 0;
	virtual void OnCollision(BaseCollider* self, BaseCollider* other) = 0;
	virtual void OnExitCollision(BaseCollider* self, BaseCollider* other) = 0;
	virtual void OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir) = 0;
	virtual void MapChipOnCollision(const CollisionInfo& info) = 0;



protected:
	Player* player_ = nullptr;

	bool isTakeAttack_ = false;
	bool isFaint_ = false;

	bool isStop_ = false;

	bool isAlive_ = true;
	std::shared_ptr<OBBCollider> obbCollider_;
	std::shared_ptr<AABBCollider> aabbCollider_;
	//std::shared_ptr<SphereCollider> sphereCollider_;


	std::unique_ptr<JsonManager> jsonManager_;
	std::unique_ptr<JsonManager> jsonCollider_;

	Vector3 knockbackVelocity_ = {};
	float knockbackTimer_ = 0.0f;
	const float kKnockbackDuration_ = 0.5f;

	WorldTransform kirisutegomennWT_;
	bool isSpinning_ = false;
	float angularVelocityY_ = 0;

	std::string dashIconPath_ = "Resources/Textures/In_Game/dashIcon.png";
	std::unique_ptr<Sprite> dashIconSprite_;
	bool iconVisible_ = true;
	Vector3 offsetPos_ = { 25,-40,0 };
	float offsetScale_ = 0.15f;


	// サウンド
	Audio::SoundData soundDataFly = {};
	IXAudio2SourceVoice* sourceVoiceFly = nullptr;
};

