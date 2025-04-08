#include "EnemyManager.h"
#include "./Player/Player.h"

void EnemyManager::Initialize(Camera* camera, MapChipField* mapChipField)
{
	camera_ = camera;
	mapChipField_ = mapChipField;
	enemies_.clear();

	//InitJson();

	LoadEnemyDataFromJson("Resources/JSON/EnemyData/EnemyData.json");

}

void EnemyManager::Update()
{
	// 毎フレーム敵を更新
	for (auto& enemy : enemies_) {
		enemy->Update();
	}

	RemoveDeadEnemies();

	//CheckSpawnDropEnemy();

	SpawnAllEnemies();

	CheckSpawnEnemies();

	ImGui();

}


void EnemyManager::Draw()
{
	for (auto& enemy : enemies_) {
		enemy->Draw();
	}
}

void EnemyManager::DrawCollisions()
{
	for (auto& enemy : enemies_) {
		enemy->DrawCollision();
	}
}

void EnemyManager::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("EnemyManager", "Resources/JSON/EnemyManager");
	jsonManager_->SetCategory("EnemyManager");
	jsonManager_->Register("Playerdistance", &triggerDistance_);
}

void EnemyManager::SpawnAllEnemies()
{
	for (auto& data : spawnDataList_) {
		if (data.type == EnemyType::Side && !data.triggered) {
			auto enemy = std::make_unique<SideEnemy>(mapChipField_);
			enemy->Initialize(camera_);
			enemy->SetPlayer(player_);
			enemy->SetTranslate(data.position);
			enemy->SetMoveSpeed(data.moveSpeed);
			enemies_.emplace_back(std::move(enemy));
			data.triggered = true;
		}
	}
}


void EnemyManager::RemoveDeadEnemies()
{
	enemies_.erase(
		std::remove_if(
			enemies_.begin(),
			enemies_.end(),
			[](const std::unique_ptr<BaseEnemy>& enemy) {
				return enemy->IsDead();
			}
		),
		enemies_.end()
	);
}

void EnemyManager::ResetAll()
{
	Vector3 initialPosition{ 0.0f, 10.0f, 0.0f };
	for (auto& enemy : enemies_) {
		enemy->Reset(initialPosition); // 適当な初期位置にリセット（必要に応じて記録した位置に）
	}
}

void EnemyManager::AddDropEnemy(const Vector3& pos)
{
	auto enemy = std::make_unique<DropEnemy>(mapChipField_);
	enemy->Initialize(camera_);
	enemy->SetPlayer(player_);
	enemy->SetTranslate(pos);
	enemies_.emplace_back(std::move(enemy));
}

void EnemyManager::AddSideEnemy(const Vector3& pos)
{
	auto enemy = std::make_unique<SideEnemy>(mapChipField_);
	enemy->Initialize(camera_);
	enemy->SetPlayer(player_);
	enemy->SetTranslate(pos);
	enemies_.emplace_back(std::move(enemy));
}

//
//void EnemyManager::CheckSpawnDropEnemy()
//{
//	// DropEnemy 出現チェック
//	for (auto& point : dropSpawnPoints_) {
//		if (point.triggered) continue;
//
//		float dx = point.position.x - player_->GetWorldTransform().translation_.x;
//		float dy = point.position.y - player_->GetWorldTransform().translation_.y;
//		float distSq = dx * dx + dy * dy;
//
//		if (distSq <= triggerDistance_ * triggerDistance_) {
//			AddDropEnemy(point.position);
//			point.triggered = true;
//		}
//	}
//}

void EnemyManager::LoadEnemyDataFromJson(const std::string& path)
{
	nlohmann::json json;
	std::ifstream file(path);
	if (file.is_open()) {
		file >> json;
		file.close();

		spawnDataList_.clear();
		for (const auto& item : json) {
			EnemySpawnData data;
			data.type = EnemySpawnData::FromString(item["type"]);
			data.position = {
				item["position"]["x"],
				item["position"]["y"],
				item["position"]["z"]
			};
			data.moveSpeed = item["moveSpeed"];
			data.fallSpeed = item.value("fallSpeed", 0.0f); // Sideには無い場合がある
			spawnDataList_.push_back(data);
		}
	}
}

void EnemyManager::SaveEnemyDataToJson(const std::string& path)
{
	//// ファイルが見つからなかったら作成
	//if (!std::filesystem::exists(path))
	//{
	//	std::filesystem::create_directories(path);
	//}

	nlohmann::json json;
	for (const auto& data : spawnDataList_) {
		json.push_back({
			{ "type", EnemySpawnData::ToString(data.type) },
			{ "position", { {"x", data.position.x}, {"y", data.position.y}, {"z", data.position.z} } },
			{ "moveSpeed", data.moveSpeed },
			{ "fallSpeed", data.fallSpeed }
			});
	}
	std::ofstream file(path);
	if (file.is_open()) {
		file << std::setw(4) << json;
		file.close();
	}
}

void EnemyManager::ImGui()
{
	if (ImGui::Begin("Enemy Manager")) {
		if (ImGui::Button("Add DropEnemy")) {
			spawnDataList_.push_back({ EnemyType::Drop, {0,0,0}, 1.0f, 2.0f });
		}
		if (ImGui::Button("Add SideEnemy")) {
			spawnDataList_.push_back({ EnemyType::Side, {0,0,0}, 1.0f, 0.0f });
		}

		for (size_t i = 0; i < spawnDataList_.size(); ++i) {
			ImGui::PushID(static_cast<int>(i));
			auto& data = spawnDataList_[i];

			ImGui::Text("Enemy %zu", i);
			ImGui::InputFloat3("Position", &data.position.x);
			ImGui::InputFloat("MoveSpeed", &data.moveSpeed);
			if (data.type == EnemyType::Drop) {
				ImGui::InputFloat("FallSpeed", &data.fallSpeed);
			}
			if (ImGui::Button("Remove")) {
				spawnDataList_.erase(spawnDataList_.begin() + i);
				ImGui::PopID();
				break;
			}
			ImGui::Separator();
			ImGui::PopID();
		}

		if (ImGui::Button("Save to JSON")) {
			SaveEnemyDataToJson("Resources/JSON/EnemyData/EnemyData.json");
		}
		if (ImGui::Button("Load from JSON")) {
			LoadEnemyDataFromJson("Resources/JSON/EnemyData/EnemyData.json");
		}
	}
	ImGui::End();

	if (ImGui::Begin("Spawned Enemies")) {
		for (size_t i = 0; i < enemies_.size(); ++i) {
			ImGui::PushID(static_cast<int>(i));
			BaseEnemy* enemy = enemies_[i].get();

			ImGui::Text("Enemy %zu", i);
			ImGui::Text("Type: %s", enemy->GetTypeName());

			Vector3 pos = enemy->GetTranslate();
			if (ImGui::InputFloat3("Position", &pos.x)) {
				enemy->SetTranslate(pos);
			}

			float speed = enemy->GetMoveSpeed();
			if (ImGui::InputFloat("MoveSpeed", &speed)) {
				enemy->SetMoveSpeed(speed);
			}

			if (auto drop = dynamic_cast<DropEnemy*>(enemy)) {
				float fallSpeed = drop->GetFallSpeed();
				if (ImGui::InputFloat("FallSpeed", &fallSpeed)) {
					drop->SetFallSpeed(fallSpeed);
				}
			}

			ImGui::Separator();
			ImGui::PopID();
		}
	}
	if (ImGui::Button("Save Current Enemies to JSON")) {
		UpdateSpawnDataFromEnemies();
		SaveEnemyDataToJson("Resources/JSON/EnemyData/EnemyData.json");
	}

	ImGui::End();

}


void EnemyManager::CheckSpawnEnemies()
{
	for (auto& data : spawnDataList_) {
		if (data.triggered) continue;

		if (data.type == EnemyType::Drop && !data.triggered) {
			float dx = data.position.x - player_->GetWorldTransform().translation_.x;
			float dy = data.position.y - player_->GetWorldTransform().translation_.y;
			float distSq = dx * dx + dy * dy;

			if (distSq <= triggerDistance_ * triggerDistance_) {
				auto enemy = std::make_unique<DropEnemy>(mapChipField_);
				enemy->Initialize(camera_);
				enemy->SetPlayer(player_);
				enemy->SetTranslate(data.position);
				enemy->SetMoveSpeed(data.moveSpeed);
				enemy->SetFallSpeed(data.fallSpeed);
				enemies_.emplace_back(std::move(enemy));

				data.triggered = true;
			}
		}
	}
}

void EnemyManager::UpdateSpawnDataFromEnemies()
{
	spawnDataList_.clear();

	for (const auto& enemy : enemies_) {
		EnemySpawnData data;

		std::string typeStr = enemy->GetTypeName();
		if (typeStr == "Drop") {
			data.type = EnemyType::Drop;
			data.fallSpeed = enemy->GetFallSpeed();
		} else {
			data.type = EnemyType::Side;
			data.fallSpeed = 0.0f;
		}

		data.position = enemy->GetTranslate();
		data.moveSpeed = enemy->GetMoveSpeed();
		data.triggered = true;

		spawnDataList_.push_back(data);
	}
}
