#include "EnemyManager.h"
#include "./Player/Player.h"

void EnemyManager::Initialize(Camera* camera, MapChipField* mapChipField)
{
	camera_ = camera;
	mapChipField_ = mapChipField;
	enemies_.clear();

	spawnTimer_ = 0.0f;
	spawnInterval_ = 3.0f;     // 3秒間隔で生成
	spawnDropCount_ = 2;       // 毎回2体ずつ
	spawnSideCount_ = 2;

	SpawnFromMapChip(mapChipField);
}

void EnemyManager::Update()
{
	// 毎フレーム敵を更新
	for (auto& enemy : enemies_) {
		enemy->Update();
	}

	RemoveDeadEnemies();
	

	CheckSpawnDropEnemy();


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
	enemy->SetTranslate(pos);
	enemies_.emplace_back(std::move(enemy));
}

void EnemyManager::AddSideEnemy(const Vector3& pos)
{
	auto enemy = std::make_unique<SideEnemy>(mapChipField_);
	enemy->Initialize(camera_);
	enemy->SetTranslate(pos);
	enemies_.emplace_back(std::move(enemy));
}

void EnemyManager::SpawnFromMapChip(MapChipField* field)
{
	int mapWidth = field->GetNumBlockHorizontal();
	int mapHeight = field->GetNumBlockVertical();

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {

			MapChipType type = field->GetMapChipTypeByIndex(x, y);
			Vector3 pos = field->GetMapChipPositionByIndex(x, y);

			switch (type) {
			case MapChipType::kDropEnemy:
				pos.y += 2.0f;
				dropSpawnPoints_.push_back({ pos, false }); // ← ここで予約
				field->SetMapChipTypeByIndex(x, y, MapChipType::kBlank); // チップ消す
				break;

			case MapChipType::kSideEnemy:
				pos.y += 1.0f;
				AddSideEnemy(pos); // 横敵は即出現
				field->SetMapChipTypeByIndex(x, y, MapChipType::kBlank); // チップ消す
				break;

			default:
				break;
			}
		}
	}
}

void EnemyManager::CheckSpawnDropEnemy()
{
	// DropEnemy 出現チェック
	for (auto& point : dropSpawnPoints_) {
		if (point.triggered) continue;

		float dx = point.position.x - player_->GetWorldTransform().translation_.x;
		float dy = point.position.y - player_->GetWorldTransform().translation_.y;
		float distSq = dx * dx + dy * dy;

		if (distSq <= triggerDistance_ * triggerDistance_) {
			AddDropEnemy(point.position);
			point.triggered = true;
		}
	}
}



