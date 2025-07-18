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


	/// <summary>
	/// ブロック生成
	/// </summary>
	void GenerateBlocks();
private:



public:

	void SetMapChip(std::string path);
	void SetMapChipField(MapChipField* mpField) { mpField_ = mpField; }
	void SetWorldTransform(std::vector<std::vector<WorldTransform*>> wt) { wt_ = wt; }
	MapChipField* GetMapChipField() { return mpField_; }

private:
	/*=======================================================================
	
								  ポインタなど

	========================================================================*/
	Camera* camera_ = nullptr;
	MapChipField* mpField_ = nullptr;
	std::unique_ptr<JsonManager> jsonManager_;

	std::vector<std::vector<WorldTransform*>> wt_;
	
	std::vector<std::vector<std::unique_ptr<Object3d>>> objects_;
	std::vector<std::vector<std::unique_ptr<Object3d>>> floors_;
	std::vector<std::vector<std::unique_ptr<Object3d>>> ceilings_;
	std::vector<std::vector<std::unique_ptr<Object3d>>> soils_;


	Vector3 color_[3]{};
	float alpha_[3];
};

