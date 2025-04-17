#pragma once

#include "Object3D/Object3d.h"
#include "WorldTransform/WorldTransform.h"
#include "Loaders/Json/JsonManager.h"
#include "Systems/Camera/Camera.h"

class Giraffe
{
public:

	void Initialize();
	void InitJson();

	void Update();

	void Draw();

	void SetCamera(Camera* camera) { camera_ = camera; }

private:
	Camera* camera_ = nullptr;
	std::unique_ptr<Object3d> head;
	std::unique_ptr<Object3d> neck;
	std::unique_ptr<Object3d> body;

	WorldTransform headTransform;
	WorldTransform neckTransform;
	WorldTransform bodyTransform;

	std::unique_ptr<JsonManager> headJson_;
	std::unique_ptr<JsonManager> neckJson_;
	std::unique_ptr<JsonManager> bodyJson_;
};

