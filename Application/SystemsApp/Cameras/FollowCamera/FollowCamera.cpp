#include "FollowCamera.h"
#include "MathFunc.h"
#include "Matrix4x4.h"
#include <Systems/Input/Input.h>
#include <DirectXMath.h>

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

void FollowCamera::Initialize()
{
	InitJson();
}

void FollowCamera::Update()
{

	//UpdateInput();

	FollowProsess();
	
	ImGui();
	
	//jsonManager_->ImGui("FollowCamera");
}

void FollowCamera::UpdateInput()
{
	if (Input::GetInstance()->IsControllerConnected())
	{

		XINPUT_STATE joyState;
		if (Input::GetInstance()->GetJoystickState(0, joyState)) {
			const float kRotateSpeed = 0.07f;

			Vector3 move{};
			move.x = 0;
			move.y += static_cast<float>(joyState.Gamepad.sThumbRX);
			move.z = 0;

			// 移動量に速さを反映
			if (Length(move) > 0.0f) {
				move = Normalize(move) * kRotateSpeed;
			} else {
				move = { 0.0f, 0.0f, 0.0f };
			}

			rotate_ += move;
		}
	}
}

void FollowCamera::FollowProsess()
{
	// ターゲットがない場合、または追従が無効の場合は処理しない
	if (target_ == nullptr || !isFollowEnabled_)
	{
		// 追従が無効の場合は現在の位置を維持
		matView_ = Inverse(MakeAffineMatrix(scale_, rotate_, translate_));
		return;
	}
	if (isZoom_)
	{
		translate_ = ZoomPos_;
		translate_.z += zoomOffsetZ_;
	}
	else
	{
		Vector3 offset = offset_;



		Matrix4x4 rotate = MakeRotateMatrixXYZ(rotate_);

		offset = TransformNormal(offset, rotate);

		// このゲーム用にX、Z軸は無視する
		Vector3 targetTranslate = Vector3(0.0f, target_->translation_.y, 0);

		translate_ = targetTranslate + offset;
	}

	matView_ = Inverse(MakeAffineMatrix(scale_, rotate_, translate_));
}

void FollowCamera::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("FollowCamera", "Resources/JSON/Cameras");
	jsonManager_->SetCategory("Cameras");
	jsonManager_->Register("OffSet Translate", &offset_);
	jsonManager_->Register("Rotate", &rotate_);
	jsonManager_->Register("ズーム時のZのオフセット", &zoomOffsetZ_);
}

void FollowCamera::JsonImGui()
{
	
}

void FollowCamera::ImGui()
{
	/*ImGui::Begin("FollowCamera Info");

	ImGui::DragFloat3("Translate", &translate_.x);

	ImGui::DragFloat3("Rotation", &rotate_.x);

	ImGui::DragFloat3("Scale", &scale_.x);

	ImGui::End();*/
}


