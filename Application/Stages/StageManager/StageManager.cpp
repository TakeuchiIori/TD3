#include "StageManager.h"
#include "imgui.h"

void StageManager::Initialize(Camera* camera)
{
	camera_ = camera;
	InitJson();
	currentStage_ = stageVector_[0];
	stageObjManager_.Initialize(camera_);
	stageObjManager_.InitStageJson(currentStage_);
	stageObjManager_.SetModelName("unitCube.obj");
	stageObjManager_.SetModelName("unitSphere.obj");
	stageObjManager_.SetModelName("player.obj");

}

void StageManager::Update()
{
	SelectStage();
	stageObjManager_.Update(currentStage_);
}

void StageManager::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("StageManager", "Resources/JSON");
	jsonManager_->Register("Total Stage Number", &totalStageNum_);
	for (int i = 0; i < totalStageNum_; ++i) {
		stageVector_.push_back("Stage" + std::to_string(i));
	}
}

void StageManager::SelectStage()
{
#ifdef _DEBUG
	ImGui::Begin("stage");
	if (ImGui::Button("Add Stage")) {
		stageVector_.push_back("Stage" + std::to_string(totalStageNum_));
		totalStageNum_++;
		jsonManager_->Save();
	}
	ImGui::SameLine();
	if (ImGui::Button("Sub Stage")) {
		totalStageNum_--;
		stageVector_.pop_back();
		jsonManager_->Save();
	}
	static int comboCurrentStage = 0;
	std::vector<const char*> stages;
	for (const auto& stage : stageVector_) {
		stages.push_back(stage.c_str());
	}
	static std::string beforeStage = currentStage_;
	if (ImGui::Combo("StageSelector", &comboCurrentStage, stages.data(), totalStageNum_)) {
		currentStage_ = stageVector_[comboCurrentStage];
		if (currentStage_ == beforeStage)
		{
		}
		else
		{
			stageObjManager_.InitStageJson(currentStage_);
		}
	}
	ImGui::Text("CurrentStage : %s", currentStage_.c_str());
	ImGui::End();
#endif // _DEBUG
}

void StageManager::Draw()
{
	stageObjManager_.Draw();
}
