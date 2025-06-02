#include "Stage.h"
#include <string>
#include "StageEditor.h"

void Stage::Initialize(Camera* camera)
{
	camera_ = camera;

	// ランダムエンジンの初期化
	std::random_device rd;
	randomEngine_.seed(rd());

	// TODO: それぞれのステージごとの初期化を呼び出す
	checkPoint_.Initialize(camera_);

	enemyManager_->ClearAll();
	std::string stageLabel = "Stage" + std::to_string(currentStageNum_);
	std::string checkpointLabel = "Checkpoint" + std::to_string(currentCheckPoint_);
	enemyManager_->SetCurrentCheckPoint(stageLabel + checkpointLabel);

	InitJson();
	InitCheckPoint();

	// 雲をランダム生成
	GenerateRandomClouds();
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
	ReloadObject();

	// チェックポイント変更時も雲を再生成
	GenerateRandomClouds();
}

void Stage::GenerateRandomClouds()
{
	// 既存の雲をクリア
	clouds_.clear();
	// ゴールの高さを取得
	float goalHeight = GetCheckPoint();
	// 雲の個数をランダムに決定
	std::uniform_int_distribution<int> cloudCountDist(1, 5);
	int cloudCount = cloudCountDist(randomEngine_);
	// X軸の範囲
	std::uniform_real_distribution<float> xPosDist(0.0f, 50.0f);
	// Y軸の範囲
	std::uniform_real_distribution<float> yPosDist(0.0f, goalHeight);
	// Z軸の範囲
	std::uniform_real_distribution<float> zPosDist(40.0f, 40.0f);
	const float minDistanceX = 20.0f;
	const float minDistanceY = 15.0f;
	const float minDistanceZ = 3.0f; 
	// 配置済み雲の位置を保存
	std::vector<Vector3> placedPositions;
	for (int i = 0; i < cloudCount; ++i) {
		Vector3 candidatePos;
		bool validPosition = false;
		int attempts = 0;
		const int maxAttempts = 100; // 無限ループ防止
		// 重複しない位置を見つけるまで試行
		while (!validPosition && attempts < maxAttempts) {
			candidatePos = {
				xPosDist(randomEngine_),
				yPosDist(randomEngine_),
				zPosDist(randomEngine_)
			};
			validPosition = true;
			// 既存の雲との距離をチェック
			for (const auto& existingPos : placedPositions) {
				float deltaX = std::abs(candidatePos.x - existingPos.x);
				float deltaY = std::abs(candidatePos.y - existingPos.y);
				float deltaZ = std::abs(candidatePos.z - existingPos.z);

				// 各軸で最小距離をチェック（いずれかの軸で十分離れていればOK）
				// つまり、すべての軸で最小距離内にある場合のみ無効
				if (deltaX < minDistanceX && deltaY < minDistanceY && deltaZ < minDistanceZ) {
					validPosition = false;
					break;
				}
			}
			attempts++;
		}
		// 有効な位置が見つかったら雲を作成
		if (validPosition) {
			auto cloud = std::make_unique<Cloud>();
			cloud->Initialize(camera_);
			cloud->SetTranslate(candidatePos);
			// 位置を記録
			placedPositions.push_back(candidatePos);
			clouds_.emplace_back(std::move(cloud));
		}
	}
}

void Stage::Update()
{
	enemyManager_->SetIsStop(player_->IsReturn());
	enemyManager_->Update();
	grassManager_->Update();
	if (player_->StartReturn() && GetCheckPointID() >= balloon_->GetEnableMapNum())
	{
		balloon_->BehaviorTransition();
	}
	if (player_->EndReturn())
	{
		balloon_->TransitionRoot();
	}
	balloon_->Update();

	// 雲の更新
	for (auto& cloud : clouds_) {
		cloud->Update();
		cloud->UpdateScale();
	}

	checkPoint_.DebugUpdate();
#ifdef _DEBUG
	//checkPoint_.DebugUpdate();
	if (StageSelector())
	{
		InitCheckPoint();
	}
#endif // _DEBUG

	background_->Update();
}

void Stage::NotDebugCameraUpdate()
{
	player_->Update();
	grassManager_->hakuGrass(player_->IsPopGrass(), player_->GetCenterPosition());
}

void Stage::DrawBackground()
{
	background_->Draw();
}

void Stage::Draw()
{
	player_->Draw();
	enemyManager_->Draw();
	grassManager_->Draw();
	balloon_->Draw();

	// 雲の描画
	for (auto& cloud : clouds_) {
		cloud->Draw();
	}

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
	balloon_->DrawCollision();
}

void Stage::DrawSprite()
{
	player_->DrawSprite();
	enemyManager_->DrawSprite();
	balloon_->DrawSprite();
}

Stage::TransitionType Stage::ReachCheckPoint()
{
	if (player_->GetCenterPosition().y >= *checkPoint_.GetCheckPointHight() && transitionType_ == TransitionType::kNone)
	{
		if (currentCheckPoint_ < StageEditor::Instance()->GetMaxCheckPointNumber(currentStageNum_))
		{
			enemyManager_->SetStage(currentStageNum_);
			currentCheckPoint_++;
			transitionType_ = TransitionType::kCheckPoint;
		} else if (currentStageNum_ < StageEditor::Instance()->GetMaxStageNumber())
		{
			currentStageNum_++;
			currentCheckPoint_ = 0;
			enemyManager_->SetStage(currentStageNum_);
			transitionType_ = TransitionType::kStage;
			if (StageEditor::Instance()->GetMaxCheckPointNumber(currentStageNum_) == -1)
			{
				isClear_ = true;
				transitionType_ = TransitionType::kClear;
			}
		} else
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