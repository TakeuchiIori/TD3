#pragma once

#include "Object3d/object3d.h"
#include "WorldTransform/WorldTransform.h"
#include "Systems/Camera/Camera.h"
#include "Loaders/Json/JsonManager.h"


class Planet
{
public:
	void Initialize(Camera* camera);
	void Update();
	void Draw();
private:

	void InitJson();
	Camera* camera_ = nullptr;
	std::unique_ptr<Object3d> planet_;
	WorldTransform wt_;

	std::unique_ptr<JsonManager> jsonManager_;
};

