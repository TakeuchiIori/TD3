#pragma once
#include "DropEnemy.h"
#include "SideEnemy.h"
#include "Loaders/Json/JsonManager.h"

class Player;
class EnemyManager
{
public:

	enum class EnemyType {
		Drop,
		Side
	};

	struct EnemySpawnData {
		EnemyType type;
		Vector3 position;
		float moveSpeed = 1.0f;
		float fallSpeed = 0.0f; // DropEnemy 専用
		bool triggered = false; //

		// JSON用の型変換（文字列との変換）
		static std::string ToString(EnemyType type) {
			return type == EnemyType::Drop ? "Drop" : "Side";
		}

		static EnemyType FromString(const std::string& str) {
			return (str == "Drop") ? EnemyType::Drop : EnemyType::Side;
		}
	};


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

	void ClearAll();

private:

	/// <summary>
	/// json
	/// </summary>
	void InitJson();


	void SpawnAllEnemies();

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
	//void CheckSpawnDropEnemy();

	void LoadEnemyDataFromJson(const std::string& path);
	void SaveEnemyDataToJson(const std::string& path);
	void ImGui();

	void CheckSpawnEnemies();

	void UpdateSpawnDataFromEnemies();


public:


	/// <summary>
	/// プレイヤーのセット
	/// </summary>
	/// <param name="player"></param>
	void SetPlayer(Player* player) { player_ = player; }
	//Player* GetPlayer() { return player_; }


	/// <summary>
	/// 現在のステージ + チェックポイント
	/// </summary>
	void SetCurrentCheckPoint(std::string checkPoint) 
	{ 
		checkPointPath_ = checkPoint;
		fullPath_ = directryPath_ + checkPointPath_ + json_;
		LoadEnemyDataFromJson(fullPath_);
	}

private:
	Camera* camera_ = nullptr;
	Player* player_ = nullptr;
	MapChipField* mapChipField_ = nullptr;
	std::vector<std::unique_ptr<BaseEnemy>> enemies_;

	float triggerDistance_ = 20.0f;
	std::vector<EnemySpawnPoint> dropSpawnPoints_;
	std::unique_ptr< JsonManager> jsonManager_;

	std::vector<EnemySpawnData> spawnDataList_;

	static const std::string directryPath_;
	static const std::string json_;
	std::string checkPointPath_ = "";

	std::string fullPath_;
};
