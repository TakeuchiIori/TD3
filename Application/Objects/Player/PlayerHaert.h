#pragma once
// Application
#include "BaseObject/BaseObject.h"
class PlayerHaert :
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


	void SetPos(const Vector3 pos) { worldTransform_.translation_ = pos; }

	void SetRotaZ(float z) { worldTransform_.rotation_.z = z; }
};

