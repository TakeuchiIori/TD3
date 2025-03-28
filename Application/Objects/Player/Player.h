#pragma once

// Engine
#include "Systems/Input/Input.h"
#include "PlayerMapCollision.h"
#include "Systems/MapChip/MapChipCollision.h"

// Application
#include "BaseObject/BaseObject.h"
#include "PlayerBody.h"

enum BehaviorPlayer
{
	Move,
	Return,
	Boost
};

class Player 
	: BaseObject
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

	void OnCollision();
	void MapChipOnCollision(const CollisionInfo& info);
private:
	/// 全行列の転送
	void UpdateMatrices();

	// 移動
	void Move();

	void Boost();

	void TimerManager();

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
	float defaultSpeed_ = 0.3f;
	float speed_ = defaultSpeed_;							// 動く速度
	bool isFPSMode_ = false;

	bool isMove_ = false;

	float boostSpeed_ = 0.4f;

	// 移動履歴
	std::list<Vector3> moveHistory_;


	// ゲージ
	int32_t MaxGrassGauge_ = 6;
	int32_t grassGauge_ = 0;

	// 時間制限 : 単位(sec)
	float kTimeLimit_ = 10.0f;			// タイマーの限界値
	float extendTimer_ = kTimeLimit_;	// 伸びられる残り時間
	float grassTime_ = 6.0f;			// 草を食べて追加される時間

	float kBoostTime_ = 1.5f;
	float boostTimer_ = 0;

	const float deltaTime_ = 1.0f / 60.0f; // 仮対応


	//PlayerMapCollision mapCollision_;

	//MapChipCollision::ColliderRect colliderRct_;

	//MapChipCollision::CollisionFlag collisionFlag_ = MapChipCollision::CollisionFlag::None;

	std::list <std::unique_ptr<PlayerBody>> playerBodys_;
};

