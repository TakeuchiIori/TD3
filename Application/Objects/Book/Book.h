#pragma once

// Engine
#include "Systems/MapChip/MapChipInfo.h"
#include "Collision/Core/BaseCollider.h"
#include "Collision/Core/CollisionManager.h"
#include "Systems/Input/Input.h"
#include "Systems/Audio/Audio.h"
#include "WorldTransform/WorldTransform.h"
#include "Systems/GameTime/GameTime.h"
#include "Loaders/Json/JsonManager.h"
#include "Systems/UI/UIBase.h"

// app
#include "BaseObject/BaseObject.h"


// Math



// C++

class Camera;
class Book: public BaseObject
{
public:

	~Book() override;
	Book(MapChipField* mapChipField)
		: velocity_(0, 0, 0),
		mpCollision_(mapChipField) {
		// プレイヤーの衝突判定用矩形を設定
		colliderRect_ = { 2.0f, 2.0f, 0.0f, 0.0f };
		worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };
	}

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="camera"></param>
	void Initialize(Camera* camera) override;
	void InitCollision();
	void InitJson();


	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;
	void UpdateSprite();
	void UpdateMatrix();


	/// <summary>
	///  描画処理
	/// </summary>
	void Draw() override;
	void DrawSprite();
	void DrawCollision();



	/// <summary>
	/// マップチップの判定処理
	/// </summary>
	/// <param name="info"></param>
	void MapChipOnCollision(const CollisionInfo& info);
	void Reset();

	std::function<void()> OnBookTrigger_ = nullptr;

private:

	/// <summary>
	/// 移動関数
	/// </summary>
	void Move();











public:
	Vector3 GetCenterPosition() const {
		return
		{
			worldTransform_.matWorld_.m[3][0],
			worldTransform_.matWorld_.m[3][1],
			worldTransform_.matWorld_.m[3][2]
		};
	}

	// 衝突イベント（共通で受け取る）
	void OnEnterCollision(BaseCollider* self, BaseCollider* other);
	void OnCollision(BaseCollider* self, BaseCollider* other);
	void OnExitCollision(BaseCollider* self, BaseCollider* other);
	void OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir);



private:

	// ポインタ
	Input* input_ = nullptr;

	std::unique_ptr<JsonManager> jsonManager_;
	std::unique_ptr<JsonManager> jsonCollider_;

	// コライダー
	std::shared_ptr<OBBCollider> obbCollider_;


	// UI関連
	std::unique_ptr<Sprite> uiBook_;
	bool isDrawUI_ = false;
	Vector3 offset_ = {-50.0f,-100.0,0.0f};


	// マップチップ
	MapChipCollision mpCollision_;
	MapChipCollision::ColliderRect colliderRect_;
	Vector3 velocity_ = {};

	// ワールドトランスフォーム
	WorldTransform worldTransform_;

};

