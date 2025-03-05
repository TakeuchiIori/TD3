#include "BaseStage.h"

void BaseStage::Initialize(Camera* camera)
{
	camera_ = camera;

	stageCamera_.SetStageName(stageName_);
	stageCamera_.Initialize();
}
