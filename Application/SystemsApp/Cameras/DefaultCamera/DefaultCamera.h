#pragma once

// Math
#include <Vector3.h>
#include <Matrix4x4.h>

#include "Loaders/Json/JsonManager.h"

class DefaultCamera
{
public:
    void Initialize();
	void InitJson();
    void Update();
    float GetFov() const { return (fov_ >= 110.0f) ? fov_ : fov_; }


    Vector3 translate_ = { 15,0,-80 };
    Vector3 scale_ = { 1,1,1 };
    Vector3 rotate_ = { 0,0,0 };
    Matrix4x4 matView_ = {};

private:
    float fov_ = 0.45f;
	std::unique_ptr<JsonManager> jsonManager_;
};

