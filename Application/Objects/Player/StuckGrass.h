#pragma once
#include "BaseObject/BaseObject.h"


// Engine
#include "Collision/AABB/AABBCollider.h"
#include <Collision/Core/BaseCollider.h>
#include "Collision/Core/CollisionDirection.h"
#include <MathFunc.h>
#include <Vector3.h>

class Player;

class StuckGrass :
    public BaseObject
{
public:
	StuckGrass() : id_(count_) { ++count_; }
	~StuckGrass() override;

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

public:
	void SetPos(const Vector3 pos) { worldTransform_.translation_ = pos; }

	Vector3 GetPos() { return worldTransform_.translation_; }

	void SetPlayer(Player* player) { player_ = player; }

	bool IsDelete() { return isDelete_; }

private:
	Player* player_ = nullptr;

	bool isDelete_ = false;

	std::shared_ptr<AABBCollider> aabbCollider_;

	std::unique_ptr<JsonManager> jsonManager_;
	std::unique_ptr<JsonManager> jsonCollider_;

	float timer_ = 0;
	float kScaleTime_ = 0.5f;

	static int count_;    // 現在のインスタンス数
	int id_;              // 各インスタンスのID

};

