#pragma once
#include "StageObject/BaseStageObject/BaseStageObject.h"

class BaseFloorObject :
    public BaseStageObject
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


	void InitJson() override;

private:
	WorldTransform clampAreaTransform_;
};

