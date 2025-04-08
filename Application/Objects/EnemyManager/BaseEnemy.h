#pragma once
// Engine
#include "../BaseObject/BaseObject.h"
#include "Systems/MapChip/MapChipCollision.h"
#include "Loaders/Json/JsonManager.h"

// Collision
#include "Collision/Sphere/SphereCollider.h"
#include "Collision/OBB/OBBCollider.h"
#include "Collision/AABB/AABBCollider.h"
#include "Collision/Core/ColliderFactory.h"
#include "Collision/Core/CollisionDirection.h"

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

	bool isAlive_ = true;
	//std::shared_ptr<OBBCollider> obbCollider_;
	std::shared_ptr<AABBCollider> aabbCollider_;
	//std::shared_ptr<SphereCollider> sphereCollider_;


	std::unique_ptr<JsonManager> jsonManager_;
	std::unique_ptr<JsonManager> jsonCollider_;

};

