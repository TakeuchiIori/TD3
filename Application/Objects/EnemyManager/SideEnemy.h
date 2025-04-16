#pragma once
#include "BaseEnemy.h"

class Player;
class SideEnemy : public BaseEnemy
{
public:

	SideEnemy(MapChipField* mapChipField)
		: velocity_(0, 0, 0),
		mpCollision_(mapChipField) {
		colliderRect_ = { 2.0f, 2.0f, 0.0f, 0.0f };
		worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };
	}

	~SideEnemy();

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
	/// 現在位置取得
	/// </summary>
	Vector3 GetCenterPosition() const;

	/// <summary>
	/// 位置設定
	/// </summary>
	void SetTranslate(const Vector3& pos) override;
	Vector3 GetTranslate() const override { return worldTransform_.translation_; }

	/// <summary>
	/// プレイヤーのセット
	/// </summary>
	/// <param name="player"></param>
	void SetPlayer(Player* player) { player_ = player; }

	void SetMoveSpeed(float speed) { speed_ = speed; }
	float GetMoveSpeed() const override { return speed_; }
	const char* GetTypeName() const override{
		return "Side";
	}


private:

	void Move();
	void InitCollision();
	void InitJson();

private:
	/*=======================================================
	
						  移動関連
	
	==========================================================*/
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };
	float moveRange_ = 5.0f;
	float speed_ = 0.05f;
	float defaultSpeed_ = 0.05f;
	bool moveRight_ = true;

	/*=======================================================

							プレイヤー

	==========================================================*/
	Player* player_ = nullptr;
	MapChipCollision mpCollision_;
	MapChipCollision::ColliderRect colliderRect_;
	Vector3 startPos_;




	static bool isHit;
};
