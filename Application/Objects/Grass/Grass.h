#pragma once
#include "BaseObject/BaseObject.h"
class Grass :
    public BaseObject
{
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


	void SetPos(Vector3 pos) { worldTransform_.translation_ = pos; }

	Vector3 GetPos() { return worldTransform_.translation_; }

};

