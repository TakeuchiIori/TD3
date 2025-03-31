#include "GrassManager.h"

void GrassManager::Initialize(Camera* camera)
{
	camera_ = camera;

	PopGrass({ 8.0f,5.0f,6.0f });
	PopGrass({ 10.0f,7.0f,6.0f });
}

void GrassManager::Update()
{
	for (auto&& grass : grassList_)
	{
		grass->Update();
	}

	grassList_.remove_if([](const std::unique_ptr<Grass>& g) {
		return g->IsDelete();
		});
}

void GrassManager::Draw()
{
	for (auto&& grass : grassList_)
	{
		grass->Draw();
	}
}

void GrassManager::DrawCollision()
{
	for (auto&& grass : grassList_)
	{
		grass->DrawCollision();
	}
}

void GrassManager::hakuGrass(bool pop, Vector3 pos)
{
	if (pop)
	{
		popPos_.push_back(player_->GetCenterPosition());
		isPop_ = true;
	}

	if (player_->behavior_ == (BehaviorPlayer::Return) && isPop_)
	{
		for (Vector3 Pos : popPos_) 
		{
			PopGrass(Pos, true);
		}
		popPos_.clear();
		isPop_ = false;
	}
}
