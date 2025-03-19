#pragma once

// Engine
#include "Systems/Input/Input.h"
#include "PlayerMapCollision.h"


// Application
#include "BaseObject/BaseObject.h"
#include "PlayerBody.h"

class Player 
	: BaseObject
{
public:
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

private:
	/// 全行列の転送
	void UpdateMatrices();

	// 移動
	void Move();


public: // getter&setter
	/// WorldTransformの取得
	WorldTransform& GetWorldTransform() { return worldTransform_; }

	/// WorldTransformの取得
	WorldTransform& GetBodyTransform() { return bodyTransform_; }

	// 一人称視点にした場合横を向いているので操作を切り替えるため
	void SetFPSMode(bool isFPS) { isFPSMode_ = isFPS; }

	void SetMapInfo(MapChipField* mapChipField) { 
		mapCollision_.SetMap(mapChipField);
		mapCollision_.Init(colliderRct_, worldTransform_.translation_);
	}

private:
	Input* input_ = nullptr;

	// 体のトランスフォーム
	WorldTransform bodyTransform_;
	// 体のオフセット
	Vector3 bodyOffset_ = { 0.0f,0.5f,0.0f };

	// 移動
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };			// 加速度
	Vector3 moveDirection_ = { 0.0f,0.0f,0.0f };	// 動く向き
	float speed_ = 0.3f;							// 動く速度
	bool isFPSMode_ = false;

	PlayerMapCollision mapCollision_;

	MapChipCollision::ColliderRect colliderRct_;

	MapChipCollision::CollisionFlag collisionFlag_ = MapChipCollision::CollisionFlag::None;

	std::list <std::unique_ptr<PlayerBody>> playerBodys_;
};

