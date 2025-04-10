#pragma once

// Application
#include "BaseObject/BaseObject.h"


// Engine
#include "Collision/AABB/AABBCollider.h"
#include "Collision/Core/CollisionDirection.h"
#include "Loaders/Json/JsonManager.h"
#include <memory>
#include <WorldTransform/WorldTransform.h>
#include <Collision/Core/BaseCollider.h>
#include <Systems/Camera/Camera.h>
#include <MathFunc.h>
#include <Vector3.h>

enum ExtendDirection
{
	Up,
	Left,
	Right,
	Down,
};

class PlayerBody :
    public BaseObject
{
public:
	PlayerBody() : id_(count_) { ++count_; }
	~PlayerBody() override;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Camera* camera) override;

	void InitCollision();
	void InitJson();

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	void DrawCollision();


	void UpExtend();

	void LeftExtend();

	void RightExtend();

	void DownExtend();


public:
	Vector3 GetCenterPosition() const {
		return
		{
			worldTransform_.matWorld_.m[3][0],
			worldTransform_.matWorld_.m[3][1],
			worldTransform_.matWorld_.m[3][2]
		};
	}
	//virtual Vector3 GetEulerRotation() override { return{}; }
	const WorldTransform& GetWorldTransform() { return worldTransform_; }

	// 衝突イベント（共通で受け取る）
	void OnEnterCollision(BaseCollider* self, BaseCollider* other);
	void OnCollision(BaseCollider* self, BaseCollider* other);
	void OnExitCollision(BaseCollider* self, BaseCollider* other);
	void OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir);

private:
	void ExtendUpdate();


public:
	void SetPos(const Vector3 pos) { worldTransform_.translation_ = pos; }

	Vector3 GetPos() { return worldTransform_.translation_; }

	void SetStartPos(const Vector3 pos) { startPos_ = pos; }

	void SetEndPos(const Vector3& pos) { endPos_ = pos; }

	float GetLength() { return Length(endPos_ - startPos_); }

	void SetIsInvincible(bool isInvincible = false) { isPlayerInvincible_ = isInvincible; }

	bool IsTakeDamage() 
	{
		if (isTakeDamage_)
		{
			isTakeDamage_ = false;
			return true;
		}
		return false;
	}

private:
	Vector3 verticalGrowthScale_ = { 1.0f,0.0f,1.0f };
	Vector3 horizontalGrowthScale_ = { 0.0f,1.0f,1.0f };

	Vector3 startPos_ = {};
	Vector3 endPos_ = {};

	ExtendDirection extendDirection_ = ExtendDirection::Up;


	std::shared_ptr<AABBCollider> aabbCollider_;

	//std::unique_ptr<JsonManager> jsonManager_;
	//std::unique_ptr<JsonManager> jsonCollider_;

	bool isPlayerInvincible_ = false;

	bool isTakeDamage_ = false;

	static int count_;    // 現在のインスタンス数
	int id_;              // 各インスタンスのID
};

