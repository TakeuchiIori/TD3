#pragma once
#include "BaseObject/BaseObject.h"

// Engine
#include "Systems/Input/Input.h"
#include "Collision/Sphere/SphereCollider.h"
#include "Collision/AABB/AABBCollider.h"
#include "Loaders/Json/JsonManager.h"

// Collision
#include "Collision/Sphere/SphereCollider.h"
#include "Collision/OBB/OBBCollider.h"
#include "Collision/AABB/AABBCollider.h"
#include "Collision/Core/ColliderFactory.h"
#include "Collision/Core/CollisionDirection.h"


// Application
#include "Player/Player.h"
#include "Branch.h"

#include "Particle/ParticleEmitter.h"

enum class BehaviorGrass
{
	Root,
	Eaten,
	Growth,
	Repop,
	Delete,
};

class Grass :
	public BaseObject
{
public:
	Grass() : id_(count_) { ++count_; }
	~Grass() override;

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

	void Repop() 
	{ 
		if(behavior_ != BehaviorGrass::Growth)
		{
			behaviortRquest_ = BehaviorGrass::Repop;
		}
		else
		{
			growthWait_ = false;
		}
	}


public:
	Vector3 GetCenterPosition() const { return worldTransform_.translation_; }
	//virtual Vector3 GetEulerRotation() override { return{}; }
	const WorldTransform& GetWorldTransform() { return worldTransform_; }

	// 衝突イベント（共通で受け取る）
	void OnEnterCollision(BaseCollider* self, BaseCollider* other);
	void OnCollision(BaseCollider* self, BaseCollider* other);
	void OnExitCollision(BaseCollider* self, BaseCollider* other);
	void OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir);

private:
#ifdef _DEBUG
	/// <summary>
	/// デバッグ用表示
	/// </summary>
	void DebugGrass();
#endif // _DEBUG


private: // ふるまい

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
	void BehaviorEatenInit();
	/// <summary>
	/// 通常状態更新
	/// </summary>
	void BehaviorEatenUpdate();


	/// <summary>
	/// 成長状態初期化
	/// </summary>
	void BehaviorGrowthInit();
	/// <summary>
	/// 成長状態更新
	/// </summary>
	void BehaviorGrowthUpdate();


	/// <summary>
	/// リポップ状態初期化
	/// </summary>
	void BehaviorRepopInit();
	/// <summary>
	/// リポップ状態更新
	/// </summary>
	void BehaviorRepopUpdate();


	/// <summary>
	/// 削除状態初期化
	/// </summary>
	void BehaviorDeleteInit();
	/// <summary>
	/// 削除状態更新
	/// </summary>
	void BehaviorDeleteUpdate();
	

public: // getter & setter
	void SetPos(Vector3 pos);

	Vector3 GetPos() { return worldTransform_.translation_; }

	bool IsLarge() { return isLarge_; }

	void SetMadeByPlayer(bool isMadeByPlayer) { isMadeByPlayer_ = isMadeByPlayer; }

	bool IsMadeByPlayer() { return isMadeByPlayer_; }

	void SetPlayer(Player* player) { player_ = player; }

	bool IsDelete() { return behavior_ == BehaviorGrass::Delete; }

private:
	Player* player_ = nullptr;
	Input* input_ = nullptr;

	//std::shared_ptr<OBBCollider> obbCollider_;
	std::shared_ptr<AABBCollider> aabbCollider_;
	std::shared_ptr<AABBCollider> aabbGrowthCollider_;
	//std::shared_ptr<SphereCollider> sphereCollider_;

	WorldTransform growthAreaWT_;

	//std::unique_ptr<JsonManager> jsonManager_;
	//std::unique_ptr<JsonManager> jsonCollider_;

	// 枝
	std::unique_ptr<Branch> branch_;

	const float deltaTime_ = 1.0f / 60.0f; // 仮対応

	Vector3 defaultScale_ = { 1.0f,1.0f,1.0f };
	Vector3 growthScale_ = { 1.4f,1.4f,1.4f };

	float kGrowthTime_ = 0.5f;
	float growthTimer_ = 0.0f;
	bool growthWait_ = false;

	bool isLarge_ = false;

	bool isMadeByPlayer_ = false;

	int enter = 0;

	float  centerX_ = 17.0f;


	// 振る舞い
	BehaviorGrass behavior_ = BehaviorGrass::Root;
	// 次の振る舞いリクエスト
	std::optional<BehaviorGrass> behaviortRquest_ = std::nullopt;

	static int count_;    // 現在のインスタンス数
	int id_;              // 各インスタンスのID


	// パーティクル
	std::unique_ptr<ParticleEmitter> particleEmitter_;
};

