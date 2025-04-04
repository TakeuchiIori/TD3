#pragma once
#include "MathFunc.h"

// Engine
#include "Object3D/Object3d.h"
#include "WorldTransform/WorldTransform.h"
#include "Systems/Camera/Camera.h"

// チェックポイント用クラス
class CheckPoint
{
public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize([[maybe_unused]]Camera* camera);

	void InitJson();

private: // getter & setter
	float GetCheckPointHight() { return checkPointHight_; }
	void SetCheckPointHight(float checkPointHight) { checkPointHight_ = checkPointHight; }


public: // デバッグ用メンバ関数
#ifdef _DEBUG
	/// <summary>
	/// デバッグ時用の初期化
	/// </summary>
	void DebugInitialize(Camera* camera);

	/// <summary>
	/// デバッグ時用の更新
	/// </summary>
	void DebugUpdate();

	/// <summary>
	/// デバッグ時用の描画
	/// </summary>
	void DebugDraw();
#endif // _DEBUG



private:
	float checkPointHight_ = 0;		// チェックポイントの高さ



	// デバッグ時用
#ifdef _DEBUG
	// ポインタ
	std::unique_ptr<Object3d> obj_;
	Camera* camera_;
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
#endif // _DEBUG

};

