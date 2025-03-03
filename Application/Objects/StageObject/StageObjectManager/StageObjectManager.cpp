#include "StageObjectManager.h"

void StageObjectManager::Initialize(Camera* camera)
{
	camera_ = camera;
}

void StageObjectManager::ResetObjList(std::string& stageName)
{
	for (auto&& obj : stageObjects_) {
		jsonManager_[stageName]->ChildResset(stageName, obj->GetName());
		obj.reset();
	}
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
	if (obj->GetId() + 1 > totalObjNums_[stageName]) {
		totalObjNums_[stageName] = obj->GetId() + 1;
	}

	stageObjects_.push_back(std::move(obj));
}

void StageObjectManager::InitStageJson(std::string& stageName)
{
	ResetObjList(stageName);

	// 既に登録済みならリセット
	if (jsonManager_.find(stageName) != jsonManager_.end()) {
		jsonManager_.erase(stageName);
	}

	jsonManager_.insert({ stageName, std::make_unique<JsonManager>(stageName, "Resources/JSON") });


	// totalObjNums_ を初期化
	jsonManager_[stageName]->Register(stageName + "totalObjNum", &totalObjNums_[stageName]);

	for (int i = 0; i < totalObjNums_[stageName]; ++i) {
		AddObject(stageName);
	}
}

void StageObjectManager::Draw()
{
	for (auto&& obj : stageObjects_) {
		obj->Draw();
	}
}
