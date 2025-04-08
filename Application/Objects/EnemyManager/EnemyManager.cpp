#include "EnemyManager.h"
#include "./Player/Player.h"

void EnemyManager::Initialize(Camera* camera, MapChipField* mapChipField)
{
	camera_ = camera;
	mapChipField_ = mapChipField;
	enemies_.clear();

	InitJson();

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

void EnemyManager::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("EnemyManager", "Resources/JSON/EnemyManager");
	jsonManager_->SetCategory("EnemyManager");
	jsonManager_->Register("Playerdistance", &triggerDistance_);
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



