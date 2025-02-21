#include "PlayerCamera.h"

void PlayerCamera::Initialize()
{
	InitJson();
}

void PlayerCamera::Update()
{

	UpdateInput();

	FollowProsess();


	//jsonManager_->ImGui("FollowCamera");
}

void PlayerCamera::UpdateInput()
{
}

void PlayerCamera::FollowProsess()
{
	// ターゲットがない場合は処理しない
	if (target_ == nullptr)
	{
		return;
	}
	Vector3 offset = offset_;

	Vector3 rota = target_->rotation_;

	rotate_ = target_->rotation_;

	Matrix4x4 rotate = MakeRotateMatrixXYZ(rota);

	offset = TransformNormal(offset, rotate);

	translate_ = Vector3(target_->matWorld_.m[3][0], target_->matWorld_.m[3][1], target_->matWorld_.m[3][2]) + offset;

	matView_ = Inverse(MakeAffineMatrix(scale_, rotate_, translate_));
}

void PlayerCamera::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("PlayerCamera", "Resources/JSON");
	jsonManager_->Register("Rotate", &rotate_);
}

void PlayerCamera::JsonImGui()
{
}

void PlayerCamera::ImGui()
{
}
