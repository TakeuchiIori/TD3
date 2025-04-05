#pragma once
#include "BaseObject/BaseObject.h"

// Engine

// Collision
#include "Collision/AABB/AABBCollider.h"
#include "Loaders/Json/JsonManager.h"


class Branch :
    public BaseObject
{
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

	void SetRight();
	void SetLeft();

	void SetGrassWorldTransform(WorldTransform* grassWorldTransform) { grassWorldTransform_ = grassWorldTransform; }

public:
	Vector3 GetCenterPosition() const
	{
		return Vector3
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

private:
	WorldTransform* grassWorldTransform_ = nullptr;

	std::shared_ptr<AABBCollider> aabbCollider_;
};

