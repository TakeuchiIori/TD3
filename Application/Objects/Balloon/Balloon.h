#pragma once
#include "BaseObject/BaseObject.h"

// Engine
#include "Systems/Input/Input.h"
#include "Collision/Sphere/SphereCollider.h"
#include "Collision/AABB/AABBCollider.h"
#include "Loaders/Json/JsonManager.h"
#include "Easing.h"

// Collision
#include "Collision/Sphere/SphereCollider.h"
#include "Collision/OBB/OBBCollider.h"
#include "Collision/AABB/AABBCollider.h"
#include "Collision/Core/ColliderFactory.h"
#include "Collision/Core/CollisionDirection.h"


#include <random>

enum class BehaviorBalloon
{
	kROOT,
	kUP,
	kSTOP,
};

class Balloon :
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

private:
	/// <summary>
	/// ふるまい全体の初期化
	/// </summary>
	void BehaviorInitialize();
	/// <summary>
	/// ふるまい全体の更新
	/// </summary>
	void BehaviorUpdate();


	/// <summary>
	/// 通常状態初期化
	/// </summary>
	void BehaviorRootInit();
	/// <summary>
	/// 通常状態更新
	/// </summary>
	void BehaviorRootUpdate();


	/// <summary>
	/// 通常状態初期化
	/// </summary>
	void BehaviorUPInit();
	/// <summary>
	/// 通常状態更新
	/// </summary>
	void BehaviorUPUpdate();


	/// <summary>
	/// 成長状態初期化
	/// </summary>
	void BehaviorSTOPInit();
	/// <summary>
	/// 成長状態更新
	/// </summary>
	void BehaviorSTOPUpdate();

private:
	void ColliderOffset();


public:
	Vector3 GetCenterPosition() const { return worldTransform_.translation_; }
	const WorldTransform& GetWorldTransform() { return worldTransform_; }

	// 衝突イベント（共通で受け取る）
	void OnEnterCollision(BaseCollider* self, BaseCollider* other);
	void OnCollision(BaseCollider* self, BaseCollider* other);
	void OnExitCollision(BaseCollider* self, BaseCollider* other);
	void OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir);

	void BehaviorTransition();

	void TransitionRoot()
	{ 
		if(behavior_ == BehaviorBalloon::kUP)
		{
			behaviortRquest_ = BehaviorBalloon::kROOT;
		}
	}

public: // getter & setter
	void SetPos(Vector3 pos) { worldTransform_.translation_ = pos; }

	int GetEnableMapNum() { return enableMapNum_; }

private:
	Input* input_ = nullptr;

	std::unique_ptr<Object3d> timerObj_;

	std::unique_ptr<JsonManager> jsonManager_;

	std::shared_ptr<AABBCollider> aabbCollider_;
	std::shared_ptr<AABBCollider> aabbStopCollider_;

	WorldTransform colliderWT_;
	WorldTransform stopAreaWT_;

	Vector3 balloonScale_ = { 2.5f,2.5f,2.5f };
	Vector3 stopAreaScale_ = { 5.0f,5.0f,5.0f };

	bool isVisible_ = false;


	// スポーンはX3.5～30.5
	float minX_ = 3.5f;
	float maxX_ = 30.5f;

	float spawnX_ = 0.0f;

	float defaultY_ = 0.0f;

	float upY_ = 0.1f;

	float collisionOffsetY_ = 3.9f;

	float scaleOffsetY_ = 1.7f;

	int enableMapNum_ = 0;

	std::random_device seedGene_;


	// 振る舞い
	BehaviorBalloon behavior_ = BehaviorBalloon::kROOT;
	// 次の振る舞いリクエスト
	std::optional<BehaviorBalloon> behaviortRquest_ = std::nullopt;
};

