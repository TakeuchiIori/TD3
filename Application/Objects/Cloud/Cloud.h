#pragma once

// Engine
#include "Systems/Camera/Camera.h"
#include "Object3D/Object3d.h"
#include "WorldTransform/WorldTransform.h"
#include "Loaders/Json/JsonManager.h"


// C++
#include <vector>


// Math
#include "MathFunc.h"

class Cloud
{

public:

	~Cloud();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(Camera* camera);
	void InitJson();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();



private:





public:





private:
	Camera* camera_ = nullptr;
	std::unique_ptr<JsonManager> jsonManager_ = nullptr;
	std::vector<std::unique_ptr<Object3d>> obj_;
	std::vector<WorldTransform*> wt_;


	float moveSpeed_;           // 雲の移動速度
	float resetDistance_;       // リセット距離
	Vector3 cloud1Position_;    // 雲1の初期位置
	Vector3 cloud1Scale_;       // 雲1のスケール
	Vector3 cloud2Position_;    // 雲2の初期位置
	Vector3 cloud2Scale_;       // 雲2のスケール
};

