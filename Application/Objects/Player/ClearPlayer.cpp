#include "ClearPlayer.h"
#include <Systems/Input/Input.h>

void ClearPlayer::Initialize(Camera* camera)
{
	camera_ = camera;
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("head.obj");
	obj_->SetMaterialColor(defaultColorV4_);

	worldTransform_.Initialize();

	neck_ = std::make_unique<Object3d>();
	neck_->Initialize();
	neck_->SetModel("neck2.obj");
	neck_->SetMaterialColor(defaultColorV4_);
	neckTransform_.Initialize();
	neckTransform_.useAnchorPoint_ = true;
	neckTransform_.SetAnchorPoint({ 0.0, -1.0f,0.0f });
	neckTransform_.scale_.y = 0;


	InitJson();
}

void ClearPlayer::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("clearPlayer","Reources/Json/");
	jsonManager_->SetCategory("ClearPlayer");
	jsonManager_->Register("頭の位置", &worldTransform_.translation_);
	jsonManager_->Register("首の位置", &neckTransform_.translation_);
}



void ClearPlayer::Update()
{
	Matrix4x4 neckMat = neckTransform_.matWorld_;
	Vector3 neckPos = {
	neckMat.m[3][0],
	neckMat.m[3][1],
	neckMat.m[3][2]
	};



	// 左スティックの入力を取得
	Vector2 leftStick = Input::GetInstance()->GetLeftStickInput(0);

	// 左スティックが上方向（Y軸正の方向）に倒されているかチェック
	// 閾値は0.5f程度に設定（スティックの感度調整）
	if (Input::GetInstance()->TriggerKey(DIK_W) ||
		leftStick.y > up_) {
		neckTransform_.scale_.y += up_;
	}


	float stretchY = neckTransform_.scale_.y;
	worldTransform_.translation_ = neckPos + Vector3(0.0f, stretchY + 1.0f, 0.0f);
	worldTransform_.UpdateMatrix();
	UpdateMatrix();

	neck_->uvScale = { neckTransform_.scale_.x, neckTransform_.scale_.y };
	neck_->uvTranslate.y = -(neckTransform_.scale_.y - 1.0f) * 0.6855f;

}

void ClearPlayer::UpdateMatrix()
{
	worldTransform_.UpdateMatrix();
	neckTransform_.UpdateMatrix();
}



void ClearPlayer::Draw()
{
	obj_->Draw(camera_, worldTransform_);
	neck_->Draw(camera_, neckTransform_);
}
