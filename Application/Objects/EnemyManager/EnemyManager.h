#pragma once
#include "DropEnemy.h"
#include "SideEnemy.h"
#include "Loaders/Json/JsonManager.h"

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

private:

	/// <summary>
	/// json
	/// </summary>
	void InitJson();

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
	//Player* GetPlayer() { return player_; }

private:
	Camera* camera_ = nullptr;
	Player* player_ = nullptr;
	MapChipField* mapChipField_ = nullptr;
	std::vector<std::unique_ptr<BaseEnemy>> enemies_;

	float triggerDistance_ = 20.0f;
	std::vector<EnemySpawnPoint> dropSpawnPoints_;
	std::unique_ptr< JsonManager> jsonManager_;

};
