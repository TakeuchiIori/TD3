#pragma once
#include "DropEnemy.h"
#include "SideEnemy.h"


class Player;
class EnemyManager
{
public:

	struct EnemySpawnPoint {
		Vector3 position;
		bool triggered = false;
	};


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Camera* camera, MapChipField* mapChipField);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// コライダー描画
	/// </summary>
	void DrawCollisions();

	/// <summary>
	/// マップチップで敵を生成
	/// </summary>
	/// <param name="field"></param>
	void SpawnFromMapChip(MapChipField* field);

private:

	/// <summary>
	/// 死亡フラグのたった敵の削除
	/// </summary>
	void RemoveDeadEnemies();

	/// <summary>
	/// 全リセット
	/// </summary>
	void ResetAll();

	/// <summary>
	/// 敵を追加
	/// </summary>
	void AddDropEnemy(const Vector3& pos);
	void AddSideEnemy(const Vector3& pos);

	/// <summary>
	/// 出現チェック
	/// </summary>
	void CheckSpawnDropEnemy();

public:


	/// <summary>
	/// プレイヤーのセット
	/// </summary>
	/// <param name="player"></param>
	void SetPlayer(Player* player) { player_ = player; }

private:
	Camera* camera_ = nullptr;
	Player* player_ = nullptr;
	MapChipField* mapChipField_ = nullptr;
	std::vector<std::unique_ptr<BaseEnemy>> enemies_;



	float spawnTimer_ = 0.0f;
	float spawnInterval_ = 3.0f; // 3秒間隔
	int spawnDropCount_ = 2;
	int spawnSideCount_ = 2;
	const float triggerDistance_ = 10.0f;
	std::vector<EnemySpawnPoint> dropSpawnPoints_;

};
