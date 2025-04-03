#pragma once

// Engine
#include "Systems/Input/Input.h"
#include "PlayerMapCollision.h"
#include "Systems/MapChip/MapChipCollision.h"
#include "Collision/Sphere/SphereCollider.h"
#include "Collision/AABB/AABBCollider.h"
#include "Loaders/Json/JsonManager.h"

// Collision
#include "Collision/Sphere/SphereCollider.h"
#include "Collision/OBB/OBBCollider.h"
#include "Collision/AABB/AABBCollider.h"
#include "Collision/Core/ColliderFactory.h"


// Application
#include "BaseObject/BaseObject.h"
#include "PlayerBody.h"
#include "StuckGrass.h"

enum class BehaviorPlayer
{
	Root,
	Moving,
	Boost,
	Return,
};

class Player 
	: public BaseObject
{
public:
	Player(MapChipField* mapChipField)
		: velocity_(0, 0, 0),
		mpCollision_(mapChipField) {
		// プレイヤーの衝突判定用矩形を設定
		colliderRect_ = { 2.0f, 2.0f, 0.0f, 0.0f };
		worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };
	}

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

	void MapChipOnCollision(const CollisionInfo& info);


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




private:
	/// 全行列の転送
	void UpdateMatrices();

	// 移動
	void Move();

	void UpBody();
	void DownBody();
	void LeftBody();
	void RightBody();

	// 移動へ移行
	void EntryMove();

	// ブーストへ移行
	void EntryBoost();

	// 帰還へ移行
	void EntryReturn();


	void TimerManager();


	void ExtendBody();

	void ShrinkBody();

	void TakeDamage();


#ifdef _DEBUG
	// デバッグ用 (ImGuiとか)
	void DebugPlayer();
#endif // _DEBUG

private: // プレイヤーのふるまい

	/// <summary>
	/// ふるまい全体の初期化
	/// </summary>
	void BehaviorInitialize();
	/// <summary>
	/// ふるまい全体の更新
	/// </summary>
	void BehaviorUpdate();


	/// <summary>
	/// 停止状態初期化
	/// </summary>
	void BehaviorRootInit();
	/// <summary>
	/// 停止状態更新
	/// </summary>
	void BehaviorRootUpdate();


	/// <summary>
	/// 移動状態初期化
	/// </summary>
	void BehaviorMovingInit();
	/// <summary>
	/// 移動状態更新
	/// </summary>
	void BehaviorMovingUpdate();


	/// <summary>
	/// 加速状態初期化
	/// </summary>
	void BehaviorBoostInit();
	/// <summary>
	/// 加速状態更新
	/// </summary>
	void BehaviorBoostUpdate();


	/// <summary>
	/// 帰還状態初期化
	/// </summary>
	void BehaviorReturnInit();
	/// <summary>
	/// 帰還状態更新
	/// </summary>
	void BehaviorReturnUpdate();



public: // getter&setter

	// 一人称視点にした場合横を向いているので操作を切り替えるため
	void SetFPSMode(bool isFPS) { isFPSMode_ = isFPS; }

	void SetMapInfo(MapChipField* mapChipField) { 
		//mapCollision_.SetMap(mapChipField);
		//mapCollision_.Init(colliderRct_, worldTransform_.translation_);
	}

	bool IsBoost() { return behavior_ == BehaviorPlayer::Boost; }

	bool EndReturn()
	{
		return beforebehavior_ == BehaviorPlayer::Return &&
			behavior_ == BehaviorPlayer::Root;
	}

	bool IsPopGrass();

	int32_t GetGrassGauge() { return grassGauge_; }

	int32_t GetMaxGrassGauge() { return MaxGrass_; }

	float GetMaxGrassTime() { return kCreateGrassTime_; }
	float GetGrassTimer() { return createGrassTimer_; }

	float GetTimeLimit() { return extendTimer_; }

	int32_t GetMaxHP() { return kMaxHP_; }	// 最大HPの取得
	int32_t GetHP() { return HP_; }			// 現在のHPの取得

	bool CanSpitting() { return canSpitting_; }

private:
	Input* input_ = nullptr;

	std::unique_ptr<JsonManager> jsonManager_;
	std::unique_ptr<JsonManager> jsonCollider_;

	//std::shared_ptr<OBBCollider> obbCollider_;
	std::shared_ptr<AABBCollider> aabbCollider_;
	std::shared_ptr<AABBCollider> nextAabbCollider_;
	WorldTransform nextWorldTransform_;
	//std::shared_ptr<SphereCollider> sphereCollider_;
	
	MapChipCollision mpCollision_;
	MapChipCollision::ColliderRect colliderRect_;

	bool isCollisionBody = false;


	// 移動
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };			// 加速度
	Vector3 moveDirection_ = { 0.0f,0.0f,0.0f };	// 動く向き
	Vector3 beforeDirection_ = { 0.0f,0.0f,0.0f };	// 動く向き
	float defaultSpeed_ = 0.1f;
	float speed_ = defaultSpeed_;							// 動く速度
	bool isFPSMode_ = false;

	bool isMove_ = false;

	float boostSpeed_ = 0.2f;

	// 移動履歴
	std::list<Vector3> moveHistory_;


	// ゲージ
	int32_t MaxGrass_ = 4;				// 暫定対応
	int32_t grassGauge_ = 0;

	// 時間制限 : 単位(sec)
	float kTimeLimit_ = 10.0f;			// タイマーの限界値
	float extendTimer_ = 0;				// 伸びられる残り時間
	float grassTime_ = 3.0f / 2.0f;			// 草を食べて追加される時間
	float largeGrassTime_ = 6.0f / 2.0f;		// 大きい草

	float kBoostTime_ = 1.5f;			// ブーストの最大効果時間
	float boostTimer_ = 0;				// 現在のブーストの残り時間

	float kBoostCT_ = 5.0f;				// ブーストのクールタイム
	float boostCoolTimer_ = 0;			// 現在のクールタイムトの残り時間


	float kCreateGrassTime_ = 3.0f;		// 草が詰まるまでの時間
	float createGrassTimer_ = 0.0f;
	bool isCreateGrass_ = false;

	bool canSpitting_ = false;			// 唾を吐けるか


	const float deltaTime_ = 1.0f / 60.0f; // 仮対応

	// ヒットポイント
	int32_t kMaxHP_ = 3;
	int32_t HP_ = kMaxHP_;

	float kInvincibleTime_ = 2.0f;
	float invincibleTimer_ = 0.0f;


	//PlayerMapCollision mapCollision_;

	//MapChipCollision::ColliderRect colliderRct_;

	//MapChipCollision::CollisionFlag collisionFlag_ = MapChipCollision::CollisionFlag::None;

	std::list <std::unique_ptr<PlayerBody>> playerBodys_;

	std::list<std::unique_ptr<StuckGrass>> stuckGrassList_;

	// コントローラー用
	Vector2 stick = {};
	float threshold = 0.5f;

public:
	// 振る舞い
	BehaviorPlayer behavior_ = BehaviorPlayer::Root;
	BehaviorPlayer beforebehavior_ = behavior_;
	// 次の振る舞いリクエスト
	std::optional<BehaviorPlayer> behaviortRquest_ = std::nullopt;
};

