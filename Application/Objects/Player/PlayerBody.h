#pragma once

// Application
#include "BaseObject/BaseObject.h"


// Engine
#include "Collision/AABB/AABBCollider.h"
#include "Loaders/Json/JsonManager.h"

enum ExtendDirection
{
	Up,
	Left,
	Right,
	Down,
};

class PlayerBody :
    public BaseObject, public AABBCollider
{
public:
	PlayerBody() : id_(count_) { ++count_; }
	~PlayerBody() override { --count_; }

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Camera* camera) override;

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


	void UpExtend();

	void LeftExtend();

	void RightExtend();

	void DownExtend();


public:
	Vector3 GetCenterPosition() const override {
		return
		{
			worldTransform_.matWorld_.m[3][0],
			worldTransform_.matWorld_.m[3][1],
			worldTransform_.matWorld_.m[3][2]
		};
	}
	virtual Vector3 GetEulerRotation() override { return{}; }
	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	void OnCollision([[maybe_unused]] Collider* other) override;
	void EnterCollision([[maybe_unused]] Collider* other) override;
	void ExitCollision([[maybe_unused]] Collider* other) override;


private:
	void ExtendUpdate();


public:
	void SetPos(const Vector3 pos) { worldTransform_.translation_ = pos; }

	Vector3 GetPos() { return worldTransform_.translation_; }

	void SetStartPos(const Vector3 pos) { startPos_ = pos; }

	void SetEndPos(const Vector3& pos) { endPos_ = pos; }

	float GetLength() { return Length(endPos_ - startPos_); }

private:
	Vector3 verticalGrowthScale_ = { 1.0f,0.0f,1.0f };
	Vector3 horizontalGrowthScale_ = { 0.0f,1.0f,1.0f };

	Vector3 startPos_ = {};
	Vector3 endPos_ = {};

	ExtendDirection extendDirection_ = ExtendDirection::Up;


	std::unique_ptr<JsonManager> jsonManager_;
	std::unique_ptr<JsonManager> jsonCollider_;


	static int count_;    // 現在のインスタンス数
	int id_;              // 各インスタンスのID
};

