#pragma once

// Engine
#include "MapChipField.h"
#include "MapChipCollision.h"
#include "Systems/Camera/Camera.h"
#include "Object3D/Object3d.h"
#include "WorldTransform/WorldTransform.h"
#include "Loaders/Json/JsonManager.h"

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
	
	void InitJson();

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

	void SetMapChipField(MapChipField* mpField) { mpField_ = mpField; }
	void SetWorldTransform(std::vector<std::vector<WorldTransform*>> wt) { wt_ = wt; }
	MapChipField* GetMapChipField() { return mpField_; }

private:
	/*=======================================================================
	
								  ポインタなど

	========================================================================*/
	Camera* camera_ = nullptr;
	MapChipField* mpField_ = nullptr;
	std::vector<std::vector<WorldTransform*>> wt_;
	std::vector<std::vector<std::unique_ptr<Object3d>>> objects_;
	std::unique_ptr<JsonManager> jsonManager_;
	std::vector<std::vector<std::unique_ptr<Object3d>>> floors_;



	Vector3 color_[2]{};
	float alpha_[2];
};

