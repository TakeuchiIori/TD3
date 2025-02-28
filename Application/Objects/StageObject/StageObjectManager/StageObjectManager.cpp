#include "StageObjectManager.h"

void StageObjectManager::Initialize(Camera* camera)
{
	camera_ = camera;
}

void StageObjectManager::ResetObjList()
{
	stageObjects_.clear();
}

void StageObjectManager::Update(std::string& stageName)
{
#ifdef _DEBUG
	ImGui::Begin("stage");
	if (ImGui::Button("Add Obj")) {
		AddObject(stageName);
	}
	ImGui::End();
#endif // _DEBUG
	for (auto&& obj : stageObjects_) {
		obj->Update();
	}

}

void StageObjectManager::AddObject(std::string& stageName)
{
	std::unique_ptr<BaseStageObject> obj = std::make_unique<BaseStageObject>();
	obj->SetStageName(stageName);
	std::string objName = "object";
	obj->SetName(objName);
	obj->Initialize(camera_);
	if (obj->GetId() + 1 > totalObjNum_) {
		totalObjNum_ = obj->GetId() + 1;
	}

	stageObjects_.push_back(std::move(obj));
}

void StageObjectManager::InitStageJson(std::string& stageName)
{
	ResetObjList();
	jsonManager_ = std::make_unique<JsonManager>(stageName, "Resources/JSON");
	jsonManager_->Register("totalObjNum", &totalObjNum_);
	for (int i = 0; i < totalObjNum_; ++i) {
		AddObject(stageName);
	}
}

void StageObjectManager::Draw()
{
	for (auto&& obj : stageObjects_) {
		obj->Draw();
	}
}
