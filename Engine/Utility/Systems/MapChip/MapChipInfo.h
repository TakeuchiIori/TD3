#pragma once

// Engine
#include "MapChipField.h"
#include "MapChipCollision.h"
#include "Systems/Camera/Camera.h"
#include "Object3D/Object3d.h"
#include "WorldTransform/WorldTransform.h"

// C++ 
#include <vector>

// Math

class MapChipInfo
{
public:

	~MapChipInfo();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="camera"></param>
	void SetCamera(Camera* camera) { camera_ = camera; }


private:

	/// <summary>
	/// ブロック生成
	/// </summary>
	void GenerateBlocks();


public:

	void SetMapChipField(MapChipField* mpField) { mpField_ = std::unique_ptr<MapChipField>(mpField);; }
	void SetWorldTransform(std::vector<std::vector<WorldTransform*>> wt) { wt_ = wt; }

	MapChipField* GetMapChipField() { return mpField_.get(); }

private:
	/*=======================================================================

								  ポインタなど

	========================================================================*/
	Camera* camera_ = nullptr;
	std::vector<std::vector<WorldTransform*>> wt_;
	std::unique_ptr<MapChipField> mpField_ ;
	//MapChipCollision* mpCollision_ = nullptr;
	std::unique_ptr<Object3d> obj_;
};

