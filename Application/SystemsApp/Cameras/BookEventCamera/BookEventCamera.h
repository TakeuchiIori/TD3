#pragma once
#include <Vector3.h>
#include <Matrix4x4.h>
#include "MathFunc.h"
#include <WorldTransform/WorldTransform.h>
#include "Loaders/Json/JsonManager.h"
#include "Object3D/Object3d.h"

class BookEventCamera
{
public:
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize();
    void InitJson();

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw(Camera* camera);

	/// <summary>
	/// 制御点の生成
	/// </summary>
	void GenerateControlPoints(const Vector3& endPos,int num);

    /// <summary>
    /// 制御点追加
    /// </summary>
    /// <param name="point"></param>
    void AddControlPoint(const Vector3& point);

    /// <summary>
    /// 制御点の登録
    /// </summary>
    void RegisterControlPoints();



    Vector3 translate_ = { 0,0,0 };
    Vector3 scale_ = { 1,1,1 };
    Vector3 rotate_ = { 0,0,0 };

    Matrix4x4 matView_ = {};

    void SetTarget(const WorldTransform& target) { target_ = &target; }
    float GetFov() const { return (fov_ >= 110.0f) ? fov_ : fov_; }
    void ImGui();

private:

    Vector3 EvaluateSpline(float t);

private:

    float fov_ = 0.90f;
    std::unique_ptr <JsonManager> jsonManager_;

    std::vector<std::unique_ptr<Object3d>> obj_;
    std::vector<std::unique_ptr<WorldTransform>> wt_;
	std::vector<Vector3> controlPoints_;


    Vector3 rotation_;
    float kDeadZoneL_ = 100.0f;
    // 追従対象
    const WorldTransform* target_;
    Vector3 offset_ = { 0.0f, 6.0f, -40.0f };

	float t_ = 0.0f;
    float speed_ = 0.01f;
};

