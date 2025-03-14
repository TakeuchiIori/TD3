#include "StageObjectManager.h"

void StageObjectManager::Initialize(Camera* camera)
{
	camera_ = camera;
}

void StageObjectManager::ResetObjList(std::string& stageName)
{
	for (auto&& obj : stageObjects_) {
		jsonManager_[stageName]->ChildReset(stageName, obj->GetName());
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
	ObjectModelSetting(stageName);
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

void StageObjectManager::ObjectModelSetting(std::string& stageName)
{
#ifdef _DEBUG
	ImGui::Begin("ModelSetting");

	ImGui::Text("CurrentStage : %s", stageName.c_str());

	std::vector<const char*> models;
	for (const auto& model : modelList_) {
		models.push_back(model);
	}

	static std::unordered_map<std::string, int> comboCurrentModelMap;

	for (auto&& obj : stageObjects_) {

		std::string objName = obj->GetName();

		// 各オブジェクトごとに選択状態を保持する
		if (comboCurrentModelMap.find(objName) == comboCurrentModelMap.end()) {
			comboCurrentModelMap[objName] = 0;
		}

		// CollapsingHeader に一意のIDを付与
		std::string headerLabel = objName + "##" + std::to_string(obj->GetId());

		//ImGui::SetNextTreeNodeOpen(true, ImGuiSetCond_Once);
		if (ImGui::TreeNode(headerLabel.c_str()))
		{
			ImGui::Combo(obj->GetName().c_str(), &comboCurrentModelMap[objName], models.data(), modelList_.size());
			std::string a = obj->GetName().c_str();
			a += " : Apply";
			if (ImGui::Button(a.c_str()))
			{
				obj->SetModel(models[comboCurrentModelMap[objName]]);
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();
#endif // _DEBUG
}
