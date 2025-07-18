#pragma once
#include "BaseObject/BaseObject.h"

// Engine

// Collision
#include "Collision/AABB/AABBCollider.h"
#include "Loaders/Json/JsonManager.h"
#include "Systems/Audio/Audio.h"

class Grass;

class Branch :
    public BaseObject
{
public:
	~Branch();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Camera* camera) override;
	void InitCollision();
	void InitJson();
	void SetParentGrass(Grass* grass) { parentGrass_ = grass; }

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
	void BrokenUpdate();

public:
	void SetPlayerBoost(bool boost) { isPlayerBoost_ = boost; }

	void SetRight();
	void SetLeft();

	void SetGrassWorldTransform(WorldTransform* grassWorldTransform) { grassWorldTransform_ = grassWorldTransform; }

	void SetPos(Vector3 pos) { 
		worldTransform_.translation_ = pos;
		collisionWT_.translation_ = pos;
	}
public:
	Vector3 GetCenterPosition() const
	{
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
	void OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir);

private:
	WorldTransform* grassWorldTransform_ = nullptr;

	WorldTransform collisionWT_;

	std::shared_ptr<AABBCollider> aabbCollider_;

	//std::unique_ptr<JsonManager> jsonManager_;
	//std::unique_ptr<JsonManager> jsonCollider_;

	float rightLimit_ = 34.0f;
	float leftLimit_ = 0.0f;

	bool isPlayerBoost_ = false;

	Grass* parentGrass_ = nullptr;

	bool isBroken_ = false;
	float fallTimer_ = 0.0f;
	const float kFallDuration_ = 1.5f;
	Vector3 fallVelocity_ = { 0.0f, -0.1f, 0.0f };
	float rotationVelocityZ_ = 0.01f;
	float rotateDir = 0;
	Vector3 fallScale_ = {};



	Audio::SoundData soundDataBranch_ = {};
	IXAudio2SourceVoice* sourceVoiceBranch_ = nullptr;
};

