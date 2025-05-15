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
	Falling,
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
	void UpdateLeaf();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	void DrawCollision();

	void Repop() 
	{
		obj_->SetMaterialColor(defaultColor_);
		if(behavior_ != BehaviorGrass::Growth)
		{
			if(behavior_ == BehaviorGrass::Eaten)
			{
				isLarge_ = false;
				behaviortRquest_ = BehaviorGrass::Repop;
			}
			if (!isLarge_)
			{
				aabbGrowthCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kGrowthArea));
			}
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


	/// <summary>
	/// 落下状態初期化
	/// </summary>
	void BehaviorFallingInit();
	/// <summary>
	/// 落下状態更新
	/// </summary>
	void BehaviorFallingUpdate();


	
	void DropLeaves(int count);
	Vector3 LerpGrass(const Vector3& start, const Vector3& end, float t, Easing::Function easingFunc);

public: // getter & setter
	void SetPos(Vector3 pos);

	

	Vector3 GetPos() { return worldTransform_.translation_; }

	bool IsLarge() { return isLarge_; }

	void SetMadeByPlayer(bool isMadeByPlayer) { isMadeByPlayer_ = isMadeByPlayer; }

	bool IsMadeByPlayer() { return isMadeByPlayer_; }

	void SetPlayer(Player* player) { player_ = player; }

	bool IsDelete() { return behavior_ == BehaviorGrass::Delete; }

	void StartFalling() { behaviortRquest_ = BehaviorGrass::Falling; }

	void SetBehaviorrequest(BehaviorGrass behavior){behaviortRquest_ = behavior;}

private:
	Player* player_ = nullptr;
	Input* input_ = nullptr;

	//std::shared_ptr<OBBCollider> obbCollider_;
	std::shared_ptr<AABBCollider> aabbCollider_;
	std::shared_ptr<AABBCollider> aabbGrowthCollider_;
	//std::shared_ptr<SphereCollider> sphereCollider_;

	WorldTransform growthAreaWT_;

	//std::unique_ptr<JsonManager> jsonManager_;
	std::unique_ptr<JsonManager> jsonCollider_;

	// 枝
	std::unique_ptr<Branch> branch_;

	Vector3 defaultColor_ = { 1.0f,1.0f,1.0f };
	Vector3 growthColor_ = { 0.3f,0.3f,1.0f };

	const float deltaTime_ = 1.0f / 60.0f; // 仮対応

	Vector3 defaultScale_ = { 2.0f,2.0f,2.0f };
	Vector3 growthScale_ = { 2.8f,2.8f,2.8f };
	float growthAreaScaleF_ = 4.5f;

	// 食べる処理
	float eatenTimer_ = 0.0f;
	const float kEatenTime_ = 0.2f;
	Vector3 eatenStartScale_{};

	float kGrowthTime_ = 0.5f;
	float growthTimer_ = 0.0f;
	bool growthWait_ = false;

	bool isLarge_ = false;

	bool isMadeByPlayer_ = false;

	int enter = 0;

	float  centerX_ = 16.0f;

	float kRepopTime_ = 0.5f;
	float repopTimer_ = 0.0f;
	bool repopWait_ = false;

	float kFallTime_ = 1.5f; // 1.5秒後に削除
	float fallTimer_ = 0.0f;
	Vector3 fallVelocity_ = {}; // 落下速度

	// 振る舞い
	BehaviorGrass behavior_ = BehaviorGrass::Root;
	// 次の振る舞いリクエスト
	std::optional<BehaviorGrass> behaviortRquest_ = std::nullopt;

	static int count_;    // 現在のインスタンス数
	int id_;              // 各インスタンスのID


	// パーティクル
	std::unique_ptr<ParticleEmitter> particleEmitter_;


	// 葉っぱの落下モデル管理
	//std::vector<std::unique_ptr<Object3d>> fallingLeaves_;
	std::vector<std::unique_ptr<WorldTransform>> fallingLeafTransforms_;
	std::vector<Vector3> fallingLeafVelocities_;
	float fallingLeafLifetime_ = 1.5f; // 落下時間

	struct FallingLeaf {
		std::unique_ptr<Object3d> obj;
		std::unique_ptr<WorldTransform> wt;
		Vector3 velocity;
		Vector3 angularVelocity;
		float swingPhase = 0.0f;
		float floatPhase = 0.0f;
		float lifetime = 1.5f;
		float alpha = 1.0f;
	};
	std::vector<FallingLeaf> fallingLeaves_;



};

