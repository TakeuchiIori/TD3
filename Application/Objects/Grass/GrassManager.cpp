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

	grassList_.remove_if([](Grass& g) {
		return g.IsDelete();
		});
}

void GrassManager::Draw()
{
	for (auto&& grass : grassList_)
	{
		grass->Draw();
	}
}
