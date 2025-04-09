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

	void DrawCollision();

	void Repop()
	{
		for (const auto& g : grassList_)
		{
			g->Repop();
		}
	}

	void hakuGrass(bool pop, Vector3 pos);

	void PopGrass(Vector3 pos, bool madeByPlayer = false)
	{
		std::unique_ptr<Grass> grass = std::make_unique<Grass>();
		grass->Initialize(camera_);
		grass->SetPos(pos);
		grass->SetPlayer(player_);
		grass->SetMadeByPlayer(madeByPlayer);
		grassList_.push_back(std::move(grass));
	}

	void SetPlayer(Player* player) { player_ = player; }

	void ClearGrass()
	{
		grassList_.remove_if([](const std::unique_ptr<Grass>& g) {
			return true;
			});
	}

private:
	Player* player_ = nullptr;
	std::list<std::unique_ptr<Grass>> grassList_;
	Camera* camera_ = nullptr;

	std::list<Vector3> popPos_;
	bool isPop_ = false;
};

