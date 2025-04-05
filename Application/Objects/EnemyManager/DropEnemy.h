#pragma once
#include "BaseEnemy.h"

class Player;
class DropEnemy : public BaseEnemy
{
public:

	DropEnemy(MapChipField* mapChipField)
		: velocity_(0, 0, 0),
		mpCollision_(mapChipField) {
		colliderRect_ = { 2.0f, 2.0f, 0.0f, 0.0f };
		worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };
	}

	~DropEnemy();

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

	/// <summary>
	/// コライダー描画
	/// </summary>
	void DrawCollision() override;

	/// <summary>
	/// 衝突判定
	/// </summary>
	void OnEnterCollision(BaseCollider* self, BaseCollider* other) override;
	void OnCollision(BaseCollider* self, BaseCollider* other) override;
	void OnExitCollision(BaseCollider* self, BaseCollider* other) override;
	void OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir) override;
	void MapChipOnCollision(const CollisionInfo& info) override;

	/// <summary>
	/// 状態リセット
	/// </summary>
	void Reset(Vector3& pos) override;

	/// <summary>
	/// 現在位置を取得
	/// </summary>
	Vector3 GetCenterPosition() const;

	/// <summary>
	/// 位置設定
	/// </summary>
	void SetTranslate(Vector3 pos);

	/// <summary>
	/// プレイヤーのセット
	/// </summary>
	/// <param name="player"></param>
	void SetPlayer(Player* player) { player_ = player; }


private:

	/// <summary>
	/// 移動処理（重力による落下）
	/// </summary>
	void Move();

	/// <summary>
	/// コライダー初期化
	/// </summary>
	void InitCollision();

	/// <summary>
	/// JSON読み込み初期化
	/// </summary>
	void InitJson();

private:

	/*=======================================================

							移動関連

	==========================================================*/
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };
	Vector3 moveDirection_ = { 0.0f,0.0f,0.0f };
	float defaultSpeed_ = 0.05f;
	float speed_ = defaultSpeed_;
	bool isMove_ = false;
	bool isInversion_ = false;

	/*=======================================================

							プレイヤー

	==========================================================*/
	Player* player_ = nullptr;
	// マップチップとの当たり判定
	MapChipCollision mpCollision_;
	MapChipCollision::ColliderRect colliderRect_;
};
