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
#include "Sprite/Sprite.h"

// app
#include "BaseObject/BaseObject.h"


// Math



// C++

class Camera;
class TitlePlayer : public BaseObject
{
public:

	~TitlePlayer() override;
	TitlePlayer(MapChipField* mapChipField)
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
	void UpdateMatrix();
	void UpdateSprite();


	/// <summary>
	///  描画処理
	/// </summary>
	void Draw() override;
	void DrawCollision();
	void DrawSprite();


	/// <summary>
	/// マップチップの判定処理
	/// </summary>
	/// <param name="info"></param>
	void MapChipOnCollision(const CollisionInfo& info);
	void MapChipOnCollision();
	void Reset();

private:

	/// <summary>
	/// 移動関数
	/// </summary>
	void Move();

	/// <summary>
	/// モデルパーティクルの更新
	/// </summary>
	void UpdateParticle();








public:
	Vector3 GetCenterPosition() const {
		return
		{
			worldTransform_.matWorld_.m[3][0],
			worldTransform_.matWorld_.m[3][1],
			worldTransform_.matWorld_.m[3][2]
		};
	}

	WorldTransform& GetWorldTransform() {
		return worldTransform_;
	}

	// 衝突イベント（共通で受け取る）
	void OnEnterCollision(BaseCollider* self, BaseCollider* other);
	void OnCollision(BaseCollider* self, BaseCollider* other);
	void OnExitCollision(BaseCollider* self, BaseCollider* other);
	void OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir);

	void GenerateCeilingBreakParticle(const Vector3& position);

	void SetIsFinishedReadBook(bool isFinishedReadBook) { isFinishedReadBook_ = isFinishedReadBook; }
	void SetShowUI(bool showUI) { showUI_ = showUI; }
	void SetMapChipInfo(MapChipInfo* info) { mpInfo_ = info; }

	void Shake();
private:

	// ポインタ
	Input* input_ = nullptr;
	MapChipInfo* mpInfo_ = nullptr;
	std::unique_ptr<Object3d> neck_;
	std::unique_ptr<Object3d> body_;

	std::unique_ptr<Sprite> uiA_;
	Vector3 offsetUI_ = {};


	struct ExplosionParticle {
		std::unique_ptr<Object3d> obj;
		std::unique_ptr<WorldTransform> wt;
		Vector3 velocity;
		float lifetime;
		float switchTime; // 噴火から放射に切り替えるタイミング
		bool hasSwitched = false;
		Vector3 rotationVelocity;
	};

	std::vector<ExplosionParticle> breakParticles_;


	std::unique_ptr<JsonManager> jsonManager_;
	std::unique_ptr<JsonManager> jsonCollider_;

	// コライダー
	std::shared_ptr<OBBCollider> obbCollider_;


	// マップチップ
	MapChipCollision mpCollision_;
	MapChipCollision::ColliderRect colliderRect_;


	// ワールドトランスフォーム
	WorldTransform rootTransform_; // 全体を動かすための親
	WorldTransform worldTransform_;
	WorldTransform neckTransform_;
	WorldTransform bodyTransform_;

	// 移動
	float deltaTime_ = 0.0f;
	Vector3 velocity_ = {};
	Vector3 moveDirection_ = {};
	float defaultSpeed_ = 3.0f;
	bool isFinishedReadBook_ = false;
	bool showUI_ = false;
	bool isScaling_ = false;
	float targetRotationY_;
	float UpPower_ = 0.0f;
	bool isShake = false;
	float up_ = 0.0f;
	float time_ = 0.0f;

	const Vector4 defaultColorV4_ = { 0.90625f,0.87109f,0.125f,1.0f };
};

Vector3 MakeExplosionVelocity(float minSpeed, float maxSpeed);