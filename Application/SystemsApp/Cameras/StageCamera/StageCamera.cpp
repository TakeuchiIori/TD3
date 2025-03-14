#include "StageCamera.h"

void StageCamera::Initialize()
{
	InitJson();
}

void StageCamera::Update()
{

	UpdateInput();

	FollowProsess();
}

void StageCamera::UpdateInput()
{
}

void StageCamera::FollowProsess()
{
	// ターゲットがない場合は処理しない
	if (target_ == nullptr)
	{
		return;
	}
	Vector3 offset = offset_;



	Matrix4x4 rotate = MakeRotateMatrixXYZ(rotate_);

	offset = TransformNormal(offset, rotate);

	translate_ = target_->translation_ + offset;

	matView_ = Inverse(MakeAffineMatrix(scale_, rotate_, translate_));
}

void StageCamera::InitJson()
{
	std::string name = stageName_ + "Camera";
	jsonManager_ = std::make_unique<JsonManager>(name, "Resources/JSON");
	jsonManager_->Register("OffSet Translate", &offset_);
	jsonManager_->Register("Rotate", &rotate_);
}

void StageCamera::JsonImGui()
{
}
