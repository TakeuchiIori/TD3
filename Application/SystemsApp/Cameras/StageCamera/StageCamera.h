#pragma once
#include <Vector3.h>
#include <WorldTransform/WorldTransform.h>
#include "Loaders/Json/JsonManager.h"

class StageCamera
{
public:

    void Initialize();
    void Update();

    void UpdateInput();
    void FollowProsess();

    void InitJson();
    void JsonImGui();

    Vector3 translate_ = { 0,0,0 };
    Vector3 scale_ = { 1,1,1 };
    Vector3 rotate_ = { 0,0,0 };

    Matrix4x4 matView_ = {};

    void SetTarget(const WorldTransform& target) { target_ = &target; }

    void SetStageName(std::string& stageName) { stageName_ = stageName; }

private:

    std::unique_ptr <JsonManager> jsonManager_;
    Vector3 rotation_;
    // 追従対象
    const WorldTransform* target_;
    Vector3 offset_ = { 0.0f, 6.0f, -40.0f };

    std::string stageName_;
};

