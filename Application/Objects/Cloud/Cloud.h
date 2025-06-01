#pragma once

// Engine
#include "Systems/Camera/Camera.h"
#include "Object3D/Object3d.h"
#include "WorldTransform/WorldTransform.h"
#include "Loaders/Json/JsonManager.h"

// C++
#include <memory>

// Math
#include "MathFunc.h"

class Cloud
{
public:
    Cloud();
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

    Vector3 GetTranslate() { return cloudPosition_; }

private:
    Camera* camera_ = nullptr;
    std::unique_ptr<JsonManager> jsonManager_ = nullptr;

    // 雲
    std::unique_ptr<Object3d> cloud_;
    WorldTransform* cloudWorldTransform_;

    int cloudId_;               // 雲のID（Json名前重複防止用）
    inline static int nextCloudId_ = 0;

    float moveSpeed_;           // 雲の移動速度
    float resetDistance_;       // リセット距離
    Vector3 cloudPosition_;     // 雲の位置
    Vector3 cloudScale_;        // 雲のスケール
};