#pragma once
#include "BaseObject/BaseObject.h"
#include "Systems/GameTime/GameTime.h"
#include "Loaders/Json/JsonManager.h"
#include <string>

class ClearPlayer : public BaseObject
{
public:
	ClearPlayer() = default;
	~ClearPlayer() override = default;
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="camera"></param>
	void Initialize(Camera* camera) override;
	void InitJson();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;
	void UpdateMatrix();

	/// <summary>
	/// イベントを開始する
	/// </summary>
	void StartEvent();

	/// <summary>
	/// イベントを更新する
	/// </summary>
	void UpdateEvent();

	/// <summary>
	///  描画処理
	/// </summary>
	void Draw() override;
	void DrawAnimation();

	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	Vector3 GetCenterPosition() const {
		return
		{
			worldTransform_.matWorld_.m[3][0],
			worldTransform_.matWorld_.m[3][1],
			worldTransform_.matWorld_.m[3][2]
		};
	}

	// イベント状態のゲッター
	bool IsEventStarted() const { return isEventStarted_; }
	bool IsEventActive() const { return isEventActive_; }
	float GetEventTimer() const { return eventTimer_; }

private:
	// worldTransform = 57;
	std::unique_ptr<JsonManager> jsonManager_;
	std::unique_ptr<Object3d> neck_;
	WorldTransform neckTransform_;

	float up_ = 0.1f;
	const Vector4 defaultColorV4_ = { 0.90625f,0.87109f,0.125f,1.0f };

	/*=================================================================
							イベント管理
	=================================================================*/
	bool isEventStarted_ = false;     // イベントが開始されたかどうか
	bool isEventActive_ = false;      // イベントが進行中かどうか
	float eventTimer_ = 0.0f;         // イベント用タイマー
	const float eventTriggerHeight_ = 55.0f; // イベント発動高度

	// イベント中の演出用変数
	bool isEventCameraActive_ = false; // イベント用カメラ演出
	Vector3 originalHeadColor_;        // 元の頭の色
	float colorChangeTimer_ = 0.0f;    // 色変更用タイマー

	// アニメーション向き切り替え用
	bool wasAnimationFinished_ = false; // 前フレームでアニメーションが終了していたか
	bool isRotationToggled_ = false;    // 向きが切り替わっているかの状態
	bool animationJustFinished_ = false; // アニメーション終了を1度だけ処理するためのフラグ

	// アニメーションファイル管理
	const std::string animation1_ = "eat_1.gltf";  // 0度向きのアニメーション
	const std::string animation2_ = "eat_2.gltf";  // 3.2度向きのアニメーション
	// 必要に応じて追加のアニメーション
	// const std::string animation3_ = "eat_3.gltf";
	// const std::string animation4_ = "eat_4.gltf";
};