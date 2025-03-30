#pragma once
#include "Systems/Camera/Camera.h"
#include "Grass/Grass.h"

class GrassManager
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();


	void PopGrass(Vector3& pos)
	{
		std::unique_ptr<Grass> grass = std::make_unique<Grass>();
		grass->Initialize(camera_);
		grass->SetPos(pos);
		grass->SetPlayer(player_);
		grassList_.push_back(std::move(grass));
	}

	void SetPlayer(Player* player) { player_ = player; }

private:
	Player* player_ = nullptr;
	std::list<std::unique_ptr<Grass>> grassList_;
	Camera* camera_ = nullptr;
};

