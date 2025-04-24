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
#include "Easing.h"

// app
#include "BaseObject/BaseObject.h"


// Math



// C++

class Camera;
class Player;
class Book: public BaseObject
{
public:

	enum class UIReadScaleState {
		None,
		Growing,
		Shrinking
	};
	UIReadScaleState uiReadScaleState_ = UIReadScaleState::None;



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
	void InitializeSprite();
	void InitCollision();
	void InitJson();
	void InitEvent();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;
	void UpdateSprite();
	void UpdateMatrix();
	void UpdateReadBook();

	/// <summary>
	///  描画処理
	/// </summary>
	void Draw() override;
	void DrawSprite();
	void DrawCollision();

	/// <summary>
	/// 本を読むイベント
	/// </summary>
	void ReadEvent();

	/// <summary>
	/// マップチップの判定処理
	/// </summary>
	/// <param name="info"></param>
	void MapChipOnCollision(const CollisionInfo& info);

	std::function<void()> OnBookTrigger_ = nullptr;
	std::function<void()> OffBookTrigger_ = nullptr;

private:

	/// <summary>
	/// UIの更新処理
	/// </summary>
	void UpdateUI();








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


	void SetPlayer(Player* player) { player_ = player; }
	void SetIsDrawUI(bool isDrawUI) { isDrawUI_ = isDrawUI; }
	void SetIsDrawReadUI(bool isDrawReadUI) { isDrawReadUI_ = isDrawReadUI; }
	void SetIsCloseIconVisible(bool isCloseIconVisible) { isCloseIconVisible_ = isCloseIconVisible; }

private:

	// ポインタ
	Input* input_ = nullptr;
	Player* player_ = nullptr;

	std::unique_ptr<JsonManager> jsonManager_;
	std::unique_ptr<JsonManager> jsonCollider_;

	// コライダー
	std::shared_ptr<OBBCollider> obbCollider_;


	// UI関連
	std::unique_ptr<Sprite> uiBook_[2];
	bool isDrawUI_ = false;
	Vector3 offset_ = {-50.0f,-100.0f,0.0f};
	Vector3 offsetUI_ = {};

	bool hasReadBookShown_ = false;
	float readBookDelayTimer_ = 0.0f;
	bool isCloseIconVisible_ = false;





	// 本を読むUI
	std::unique_ptr<Sprite> uiReadBook_[2];
	bool isDrawReadUI_ = false;
	bool isDrawBack_ = false;
	bool isScaling_ = false;
	Vector3 offsetReadUI_{};
	Vector2 uiSizeReadBase_ = { 1000,600 };

	float uiReadScaleT_ = 0.0f;
	float uiReadScaleTarget_ = 0.0f;
	float uiReadScaleCurrent_ = 0.0f;
	float readScaleDuration_ = 1.0f;


	// UI補完
	Vector2 uiSizeBase_ = { 150.0f, 100.0f };
	float uiScaleT_ = 0.0f;
	float uiScaleTarget_ = 0.0f;
	float uiScaleCurrent_ = 0.0f;
	Easing::Function easeFunctionGrow_ = Easing::Function::EaseOutQuad;
	Easing::Function easeFunctionShrink_ = Easing::Function::EaseInQuad;


	// マップチップ
	MapChipCollision mpCollision_;
	MapChipCollision::ColliderRect colliderRect_;
	Vector3 velocity_ = {};

	// ワールドトランスフォーム
	WorldTransform worldTransform_;

};

