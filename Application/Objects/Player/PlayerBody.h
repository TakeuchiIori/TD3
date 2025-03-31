#pragma once
// Application
#include "BaseObject/BaseObject.h"

enum ExtendDirection
{
	Up,
	Left,
	Right,
	Down,
};

class PlayerBody :
    public BaseObject
{
public:
	PlayerBody() : id_(count_) { ++count_; }
	~PlayerBody() override { --count_; }

public:
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


	void UpExtend();

	void LeftExtend();

	void RightExtend();

	void DownExtend();

private:
	void ExtendUpdate();


public:
	void SetPos(const Vector3 pos) { worldTransform_.translation_ = pos; }

	Vector3 GetPos() { return worldTransform_.translation_; }

	void SetStartPos(const Vector3 pos) { startPos_ = pos; }

	void SetEndPos(const Vector3& pos) { endPos_ = pos; }

private:
	Vector3 verticalGrowthScale_ = { 2.0f,0.0f,2.0f };
	Vector3 horizontalGrowthScale_ = { 0.0f,2.0f,2.0f };

	Vector3 startPos_ = {};
	Vector3 endPos_ = {};

	ExtendDirection extendDirection_ = ExtendDirection::Up;




	static int count_;    // 現在のインスタンス数
	int id_;              // 各インスタンスのID
};

