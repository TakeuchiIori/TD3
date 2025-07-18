#include "GrassManager.h"

void GrassManager::Initialize(Camera* camera)
{
	camera_ = camera;
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
		if (popPos_.size() > 0 && player_->StuckGrassPop())
		{
			PopGrass(popPos_.back());
			popPos_.pop_back();
		}
		/*for (Vector3 Pos : popPos_) 
		{
			PopGrass(Pos);
		}*/
		else if(popPos_.size() == 0)
		{
			popPos_.clear();
			isPop_ = false;
		}
	}
}
