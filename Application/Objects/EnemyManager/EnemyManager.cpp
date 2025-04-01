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
}

void EnemyManager::Update()
{
	// 毎フレーム敵を更新
	for (auto& enemy : enemies_) {
		enemy->Update();
	}

	RemoveDeadEnemies();

	Spawn();
	
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

void EnemyManager::Spawn()
{
	// スポーンタイマー処理
	spawnTimer_ += 1.0f / 60.0f; // 60FPS換算

	if (spawnTimer_ >= spawnInterval_) {

		Vector3 playerPos = player_->GetCenterPosition();
		// 敵を出現
		SpawnEnemiesAroundPlayer(playerPos, spawnDropCount_, spawnSideCount_);

		spawnTimer_ = 0.0f; // タイマーリセット
	}
}

void EnemyManager::SpawnEnemiesAroundPlayer(const Vector3& playerPos, int dropCount, int sideCount)
{
	

	// 落下型の敵を配置
	for (int i = 0; i < dropCount; ++i) {
		Vector3 pos = {
			playerPos.x + (i - dropCount / 2) * 2.0f,  // 横に広げて配置
			playerPos.y + 10.0f,                      // プレイヤーの上から
			playerPos.z
		};
		AddDropEnemy(pos);
	}

	// 横移動型の敵を配置
	for (int i = 0; i < sideCount; ++i) {
		Vector3 pos = {
			playerPos.x + (i - sideCount / 2) * 3.0f,  // 横に広げて配置
			playerPos.y,                               // 同じ高さ
			playerPos.z
		};
		AddSideEnemy(pos);
	}
}

