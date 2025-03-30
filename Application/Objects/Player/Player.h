#pragma once

// Engine
#include "Systems/Input/Input.h"
#include "PlayerMapCollision.h"
#include "Systems/MapChip/MapChipCollision.h"
#include "Collision/Sphere/SphereCollider.h"

// Application
#include "BaseObject/BaseObject.h"
#include "PlayerBody.h"

enum class BehaviorPlayer
{
	Root,
	Moving,
	Boost,
	Return,
};

class Player 
	: public BaseObject, public SphereCollider
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

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	void MapChipOnCollision(const CollisionInfo& info);


public:
	Vector3 GetCenterPosition() const override { return worldTransform_.translation_; }
	virtual Vector3 GetEulerRotation() override { return{}; }
	Matrix4x4 GetWorldMatrix() const override { return worldTransform_.matWorld_; }
	void OnCollision([[maybe_unused]] Collider* other) override;
	void EnterCollision([[maybe_unused]] Collider* other) override;
	void ExitCollision([[maybe_unused]] Collider* other) override;



private:
	/// 全行列の転送
	void UpdateMatrices();

	// 移動
	void Move();

	// 移動へ移行
	void EntryMove();

	// ブーストへ移行
	void EntryBoost();

	// 帰還へ移行
	void EntryReturn();


	void TimerManager();

	bool PopGrass();


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
	/// WorldTransformの取得
	WorldTransform& GetWorldTransform() { return worldTransform_; }

	/// WorldTransformの取得
	WorldTransform& GetBodyTransform() { return bodyTransform_; }

	// 一人称視点にした場合横を向いているので操作を切り替えるため
	void SetFPSMode(bool isFPS) { isFPSMode_ = isFPS; }

	void SetMapInfo(MapChipField* mapChipField) { 
		//mapCollision_.SetMap(mapChipField);
		//mapCollision_.Init(colliderRct_, worldTransform_.translation_);
	}

	bool IsBoost() { return behavior_ == BehaviorPlayer::Boost; }

private:
	Input* input_ = nullptr;
	
	MapChipCollision mpCollision_;
	MapChipCollision::ColliderRect colliderRect_;

	// 体のトランスフォーム
	WorldTransform bodyTransform_;
	// 体のオフセット
	Vector3 bodyOffset_ = { 0.0f,0.5f,0.0f };

	// 移動
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };			// 加速度
	Vector3 moveDirection_ = { 0.0f,0.0f,0.0f };	// 動く向き
	float defaultSpeed_ = 0.15f;
	float speed_ = defaultSpeed_;							// 動く速度
	bool isFPSMode_ = false;

	bool isMove_ = false;

	float boostSpeed_ = 0.2f;

	// 移動履歴
	std::list<Vector3> moveHistory_;


	// ゲージ
	int32_t MaxGrass_ = 2;
	int32_t grassGauge_ = 0;

	// 時間制限 : 単位(sec)
	float kTimeLimit_ = 10.0f;			// タイマーの限界値
	float extendTimer_ = 0;				// 伸びられる残り時間
	float grassTime_ = 3.0f;			// 草を食べて追加される時間
	float largeGrassTime_ = 6.0f;		// 大きい草

	float kBoostTime_ = 1.5f;			// ブーストの最大効果時間
	float boostTimer_ = 0;				// 現在のブーストの残り時間

	float kBoostCT_ = 5.0f;				// ブーストのクールタイム
	float boostCoolTimer_ = 0;			// 現在のクールタイムトの残り時間


	bool kCreateGrassTime_ = 2.0f;
	bool createGrassTimer_ = 0.0f;
	bool isCreateGrass_ = false;


	const float deltaTime_ = 1.0f / 60.0f; // 仮対応

	// ヒットポイント
	int32_t HP_ = 3;


	//PlayerMapCollision mapCollision_;

	//MapChipCollision::ColliderRect colliderRct_;

	//MapChipCollision::CollisionFlag collisionFlag_ = MapChipCollision::CollisionFlag::None;

	std::list <std::unique_ptr<PlayerBody>> playerBodys_;

public:
	// 振る舞い
	BehaviorPlayer behavior_ = BehaviorPlayer::Root;
	// 次の振る舞いリクエスト
	std::optional<BehaviorPlayer> behaviortRquest_ = std::nullopt;
};

