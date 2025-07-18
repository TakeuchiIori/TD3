#pragma once
#include <Vector3.h>
#include <WorldTransform/WorldTransform.h>
#include "Loaders/Json/JsonManager.h"

class FollowCamera
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

    void SetOffsetY(float Y) { offset_.y = Y; }

    float GetOffsetY() { return offset_.y; }


    void SetFollowEnabled(bool enabled) { isFollowEnabled_ = enabled; }

    bool IsFollowEnabled() const { return isFollowEnabled_; }

    void SetZoom(bool is, Vector3 pos)
    {
        isZoom_ = is;
        ZoomPos_ = pos;
    }

private:
   // float fov_ = 0.45f;
    std::unique_ptr <JsonManager> jsonManager_;
    void ImGui();
    Vector3 rotation_;
    float kDeadZoneL_ = 100.0f;
    // 追従対象
    const WorldTransform* target_;
    Vector3 offset_ = { 0.0f, 6.0f, -40.0f };

    // 追従の有効/無効フラグ
    bool isFollowEnabled_ = true;

    bool isZoom_ = false;
    Vector3 ZoomPos_ = {};
    float zoomOffsetZ_ = -30;
};

