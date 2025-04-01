#pragma once
#include "DropEnemy.h"
#include "SideEnemy.h"


class Player;
class EnemyManager
{
public:
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
	/// 全リセット
	/// </summary>
	void ResetAll();

	/// <summary>
	/// 敵を追加
	/// </summary>
	void AddDropEnemy(const Vector3& pos);
	void AddSideEnemy(const Vector3& pos);


	/// <summary>
	/// プレイヤー位置に基づいて敵をまとめて生成
	/// </summary>
	/// <param name="playerPos">プレイヤーの座標</param>
	/// <param name="dropCount">落下型敵の数</param>
	/// <param name="sideCount">横移動型敵の数</param>
	void SpawnEnemiesAroundPlayer(const Vector3& playerPos, int dropCount, int sideCount);


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


};
