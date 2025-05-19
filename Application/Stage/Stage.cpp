#include "Stage.h"
#include <string>
#include "StageEditor.h"

void Stage::Initialize(Camera* camera)
{
	camera_ = camera;
	// TODO: それぞれのステージごとの初期化を呼び出す
	checkPoint_.Initialize(camera_);

	enemyManager_->ClearAll();
	std::string stageLabel = "Stage" + std::to_string(currentStageNum_);
	std::string checkpointLabel = "Checkpoint" + std::to_string(currentCheckPoint_);
	enemyManager_->SetCurrentCheckPoint(stageLabel + checkpointLabel);

	InitJson();
	InitCheckPoint();
}

void Stage::InitJson()
{
}

void Stage::InitCheckPoint()
{
	enemyManager_->ClearAll();
	std::string stageLabel = "Stage" + std::to_string(currentStageNum_);
	std::string checkpointLabel = "Checkpoint" + std::to_string(currentCheckPoint_);
	enemyManager_->SetCurrentCheckPoint(stageLabel + checkpointLabel);

	Vector3 pos = player_->GetCenterPosition();
	player_->Reset();
	pos.x = StageEditor::Instance()->GetInitX(currentStageNum_, currentCheckPoint_);
	player_->SetTimeLimit(StageEditor::Instance()->GetTimeLimit(currentStageNum_, currentCheckPoint_));
	player_->SetPos({ pos.x, 2.0f, pos.z });
	ReloadObject();
}

void Stage::Update()
{
	enemyManager_->Update();
	grassManager_->Update();

	checkPoint_.DebugUpdate();
#ifdef _DEBUG
	//checkPoint_.DebugUpdate();
	if (StageSelector())
	{
		InitCheckPoint();
	}
#endif // _DEBUG

}

void Stage::NotDebugCameraUpdate()
{
	player_->Update();
	grassManager_->hakuGrass(player_->IsPopGrass(), player_->GetCenterPosition());
}

void Stage::Draw()
{
	player_->Draw();
	enemyManager_->Draw();
	grassManager_->Draw();

	checkPoint_.DebugDraw();
#ifdef _DEBUG
	//checkPoint_.DebugDraw();
#endif // _DEBUG
}

void Stage::DrawAnimation()
{
	player_->DrawAnimation();
}

void Stage::DrawCollision()
{
	player_->DrawCollision();
	grassManager_->DrawCollision();
	enemyManager_->DrawCollisions();
}

Stage::TransitionType Stage::ReachCheckPoint()
{
	if (player_->GetCenterPosition().y >= *checkPoint_.GetCheckPointHight() && transitionType_ == TransitionType::kNone)
	{
		if (currentCheckPoint_ < StageEditor::Instance()->GetMaxCheckPointNumber(currentStageNum_))
		{
			currentCheckPoint_++;
			transitionType_ = TransitionType::kCheckPoint;
		}
		else if (currentStageNum_ < StageEditor::Instance()->GetMaxStageNumber())
		{
			currentStageNum_++;
			transitionType_ = TransitionType::kStage;
		}
		else 
		{
			isClear_ = true;
			transitionType_ = TransitionType::kClear;
		}
	}
	return transitionType_;
}

bool Stage::StageSelector(const char* label)
{
#ifdef _DEBUG
	bool changed = false;

	ImGui::Begin(label);
	// ステージ選択ドロップダウン
	int maxStage = StageEditor::Instance()->GetMaxStageNumber();
	std::string stageLabel = "Stage " + std::to_string(currentStageNum_);
	if (ImGui::BeginCombo("Stage", stageLabel.c_str())) {
		for (int i = 0; i <= maxStage; ++i) {
			bool isSelected = (currentStageNum_ == i);
			std::string itemLabel = "Stage " + std::to_string(i);
			if (ImGui::Selectable(itemLabel.c_str(), isSelected)) {
				currentStageNum_ = i;
				currentCheckPoint_ = 0; // ステージ変えたらチェックポイントもリセット
				changed = true;
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	// チェックポイント選択ドロップダウン
	int maxCheckpointPerStage = StageEditor::Instance()->GetMaxCheckPointNumber(currentStageNum_);
	std::string checkpointLabel = "Checkpoint " + std::to_string(currentCheckPoint_);
	if (ImGui::BeginCombo("Checkpoint", checkpointLabel.c_str())) {
		for (int i = 0; i <= maxCheckpointPerStage; ++i) {
			bool isSelected = (currentCheckPoint_ == i);
			std::string itemLabel = "Checkpoint " + std::to_string(i);
			if (ImGui::Selectable(itemLabel.c_str(), isSelected)) {
				currentCheckPoint_ = i;
				changed = true;
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::End();

	return changed;
#endif // _DEBUG

}

void Stage::ReloadObject()
{
	const std::vector<PlacedObject>* obj = StageEditor::Instance()->GetObjects(currentStageNum_, currentCheckPoint_);

	if (obj)
	{
		grassManager_->ClearGrass();
		for (const auto& o : *obj) 
		{
			grassManager_->PopGrass(o.position);
		}
	}

	const float* hight = StageEditor::Instance()->GetCheckHight(currentStageNum_, currentCheckPoint_);

	if (hight)
	{
		checkPoint_.SetCheckPointHight(*hight);
	}
}

float Stage::GetCheckPoint()
{
	return *checkPoint_.GetCheckPointHight();
}

Vector3 Stage::GetCheckPointPos()
{
	return checkPoint_.GetCheckPointPos();
}

int Stage::GetCheckPointID()
{
	int id = 0;
	int prevStageNum = currentStageNum_ - 1;
	if (prevStageNum >= 0)
	{
		return id = StageEditor::Instance()->GetMaxCheckPointNumber(prevStageNum) + currentCheckPoint_;
	}
	return id = currentCheckPoint_;
}
