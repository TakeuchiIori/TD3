#pragma once
#include "BaseObject/BaseObject.h"

// Engine
#include "Systems/Input/Input.h"
#include "Collision/Sphere/SphereCollider.h"
#include "Loaders/Json/JsonManager.h"


#include "Player/Player.h"

enum class BehaviorGrass
{
	Root,
	Growth,
	Repop,
	Delete,
};

class Grass :
	public BaseObject , public SphereCollider
{
public:
	Grass() : id_(count_) { ++count_; }
	~Grass() override { --count_; }

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


public:
	Vector3 GetCenterPosition() const override { return worldTransform_.translation_; }
	virtual Vector3 GetEulerRotation() override { return{}; }
	Vector3 GetScale() const override { return worldTransform_.scale_ / 2.0f; }
	Matrix4x4 GetWorldMatrix() const override { return worldTransform_.matWorld_; }
	void OnCollision([[maybe_unused]] Collider* other) override;
	void EnterCollision([[maybe_unused]] Collider* other) override;
	void ExitCollision([[maybe_unused]] Collider* other) override;


private:
#ifdef _DEBUG
	/// <summary>
	/// デバッグ用表示
	/// </summary>
	void DebugGrass();
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
	/// 通常状態初期化
	/// </summary>
	void BehaviorRootInit();
	/// <summary>
	/// 通常状態更新
	/// </summary>
	void BehaviorRootUpdate();


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
	void SetPos(Vector3 pos) { worldTransform_.translation_ = pos; }

	Vector3 GetPos() { return worldTransform_.translation_; }

	bool IsLarge() { return isLarge_; }

	void SetMadeByPlayer(bool isMadeByPlayer) { isMadeByPlayer_ = isMadeByPlayer; }

	bool IsMadeByPlayer() { return isMadeByPlayer_; }

	void SetPlayer(Player* player) { player_ = player; }

	bool IsDelete() { return behavior_ == BehaviorGrass::Delete; }

private:
	Player* player_ = nullptr;

	Input* input_ = nullptr;

	std::unique_ptr<JsonManager> jsonManager_;
	std::unique_ptr<JsonManager> jsonCollider_;

	const float deltaTime_ = 1.0f / 60.0f; // 仮対応

	Vector3 defaultScale_ = { 2.0f,2.0f,2.0f };
	Vector3 growthScale_ = { 2.8f,2.8f,2.8f };

	float kGrowthTime_ = 1.0f;
	float growthTimer_ = 0.0f;

	bool isLarge_ = false;

	bool isMadeByPlayer_ = false;


	// 振る舞い
	BehaviorGrass behavior_ = BehaviorGrass::Root;
	// 次の振る舞いリクエスト
	std::optional<BehaviorGrass> behaviortRquest_ = std::nullopt;

	static int count_;    // 現在のインスタンス数
	int id_;              // 各インスタンスのID
};

