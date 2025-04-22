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

    void FollowProsess();

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw(Camera* camera);

    /// <summary>
    /// 制御点の登録
    /// </summary>
    void RegisterControlPoints();


public:

    Vector3 translate_ = { 0,0,0 };
    Vector3 scale_ = { 1,1,1 };
    Vector3 rotate_ = { 0,0,0 };
    Matrix4x4 matView_ = {};

public:


    void SetTarget(const WorldTransform& target) { target_ = &target; }
    float GetFov() const { return (fov_ >= 110.0f) ? fov_ : fov_; }

private:

    Vector3 EvaluateSpline(float t);

private:

    float fov_ = 0.90f;
    std::unique_ptr <JsonManager> jsonManager_;

    std::vector<std::unique_ptr<Object3d>> obj_;
    std::vector<std::unique_ptr<WorldTransform>> wt_;
	std::vector<Vector3> controlPoints_;


    // 追従処理関連
    Vector3 rotation_;
    const WorldTransform* target_;
    Vector3 offset_ = { 0.0f, 6.0f, -40.0f };


	float t_ = 0.0f;
    float speed_ = 0.01f;
};

