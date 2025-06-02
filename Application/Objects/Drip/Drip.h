#pragma once
#include "../BaseObject/BaseObject.h"
#include <Loaders/Json/JsonManager.h>
#include "Vector3.h"

class Drip : public BaseObject
{
public:
	Drip();
	~Drip() override;
	void Initialize(Camera* camera) override;
	void Update() override;
	void Draw() override;

	/// <summary>
	/// 雫を飛ばす
	/// </summary>
	void Shoot(const Vector3& targetPos);

	/// <summary>
	/// アクティブ状態を取得
	/// </summary>
	bool IsActive() const { return isActive_; }

private:
	void InitJson();

private:
	std::unique_ptr<JsonManager> jsonManager_;

	// 雫の状態
	bool isActive_ = false;

	// 移動関連
	Vector3 velocity_;           // 速度
	Vector3 startPos_;          // 開始位置
	float gravity_ = 0.5f;      // 重力
	float speed_ = 30.0f;       // 初速

	// フェードアウト
	float alpha_ = 1.0f;        // 透明度
	float fadeSpeed_ = 0.02f;   // フェード速度

	// 回転
	float rotationAngle_ = 0.0f; // Z軸回転角度


	int jsonId_ = 0;
	inline static int nextJsonId_ = 0;
};