#pragma once
#include "BaseObject/BaseObject.h"

class BaseFloorObject :
    public BaseObject
{
public:
	enum Shapes {
		kRect,
		kCircle,
	};
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


	void InitJson();

public:
	void SetShapes(const Shapes& shapes) { shapes_ = shapes; }

private:
	WorldTransform clampAreaTransform_;
	Shapes shapes_;
};

