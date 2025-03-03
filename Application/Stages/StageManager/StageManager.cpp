#include "StageManager.h"
#include "imgui.h"

void StageManager::Initialize(Camera* camera)
{
	camera_ = camera;
	InitJson();
	currentStage_ = stageVector_[0];
	objManager_.Initialize(camera_);
	objManager_.InitStageJson(currentStage_);
}

void StageManager::Update()
{
	SelectStage();
	objManager_.Update(currentStage_);
}

void StageManager::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("StageManager", "Resources/JSON");
	jsonManager_->Register("Total Stage Number", &totalStageNum_);
	for (int i = 0; i < totalStageNum_; ++i) {
		stageVector_.push_back("Stage" + std::to_string(i + 1));
	}
}

void StageManager::SelectStage()
{
#ifdef _DEBUG
	ImGui::Begin("stage");
	if (ImGui::Button("Add Stage")) {
		totalStageNum_++;
		stageVector_.push_back("Stage" + std::to_string(totalStageNum_));
	}
	ImGui::SameLine();
	if (ImGui::Button("Sub Stage")) {
		totalStageNum_--;
		stageVector_.pop_back();
	}
	static int comboCurrentStage = 0;
	std::vector<const char*> stages;
	for (const auto& stage : stageVector_) {
		stages.push_back(stage.c_str());
	}
	static std::string beforeStage = currentStage_;
	if (ImGui::Combo("StageSelect", &comboCurrentStage, stages.data(), totalStageNum_)) {
		currentStage_ = stageVector_[comboCurrentStage];
		if (currentStage_ == beforeStage)
		{
		}
		else
		{
			objManager_.InitStageJson(currentStage_);
		}
	}
	ImGui::Text("CurrentStage : %s", currentStage_.c_str());
	ImGui::End();
#endif // _DEBUG
}

void StageManager::Draw()
{
	objManager_.Draw();
}
