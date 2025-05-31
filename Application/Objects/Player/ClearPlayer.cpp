#include "ClearPlayer.h"
#include <Systems/Input/Input.h>
#include <cmath>

void ClearPlayer::Initialize(Camera* camera)
{
	camera_ = camera;
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("kirin.gltf", true);
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

	// 元の色を保存
	originalHeadColor_ = { defaultColorV4_.x, defaultColorV4_.y, defaultColorV4_.z };

	InitJson();
}

void ClearPlayer::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("clearPlayer", "Resources/Json/");
	jsonManager_->SetCategory("ClearPlayer");
	jsonManager_->Register("頭の位置", &worldTransform_.translation_);
	jsonManager_->Register("頭の回転", &worldTransform_.rotation_);
	jsonManager_->Register("首の位置", &neckTransform_.translation_);
	jsonManager_->Register("首の回転", &neckTransform_.rotation_);
}

void ClearPlayer::Update()
{
	Matrix4x4 neckMat = neckTransform_.matWorld_;
	Vector3 neckPos = {
	neckMat.m[3][0],
	neckMat.m[3][1],
	neckMat.m[3][2]
	};

	// プレイヤーの現在の高度を取得
	float currentHeight = worldTransform_.translation_.y;

	// イベント発動チェック
	if (!isEventStarted_ && currentHeight >= eventTriggerHeight_) {
		StartEvent();
	}

	// イベントが進行中の場合は専用の更新処理
	if (isEventActive_) {
		UpdateEvent();
	}

	// 左スティックの入力を取得
	Vector2 leftStick = Input::GetInstance()->GetLeftStickInput(0);

	if (worldTransform_.translation_.y <= 55.0f) {
		if (Input::GetInstance()->TriggerKey(DIK_W) ||
			leftStick.y > up_) {
			neckTransform_.scale_.y += up_;
		}
	}

	float stretchY = neckTransform_.scale_.y;
	worldTransform_.translation_ = neckPos + Vector3(0.0f, stretchY + 1.0f, 0.0f);
	worldTransform_.UpdateMatrix();
	UpdateMatrix();
	obj_->UpdateAnimation();
	neck_->uvScale = { neckTransform_.scale_.x, neckTransform_.scale_.y };
	neck_->uvTranslate.y = -(neckTransform_.scale_.y - 1.0f) * 0.6855f;
}

void ClearPlayer::StartEvent()
{
	isEventStarted_ = true;
	isEventActive_ = true;
	eventTimer_ = 0.0f;
	colorChangeTimer_ = 0.0f;

	// アニメーション切り替え用フラグを初期化
	wasAnimationFinished_ = false;
	isRotationToggled_ = false;
	obj_->ChangeModel("eat_1.gltf", true);
	// イベント開始時の処理
#ifdef _DEBUG
	printf("Player Event started at height: %.2f\n", eventTriggerHeight_);
#endif
}

void ClearPlayer::UpdateEvent()
{
	
	// 現在のアニメーション終了状態を取得
	bool isCurrentlyFinished = obj_->IsAnimationPlayFinished();

	// アニメーションが終了した瞬間を検出（前フレームは未終了、今フレームは終了）
	if (isCurrentlyFinished && !wasAnimationFinished_) {
		// アニメーションが終了した瞬間に向きを切り替え
		isRotationToggled_ = !isRotationToggled_;

#ifdef _DEBUG
		printf("Animation finished! Rotation toggled to: %.1f\n", isRotationToggled_ ? 3.2f : 0.0f);
#endif
	}

	// 向きの状態に応じてrotation.yを設定
	if (isRotationToggled_) {
		
		worldTransform_.rotation_.y = 3.2f;
	} else {
		
		worldTransform_.rotation_.y = 0.0f;
	}

	// 前フレームの状態を保存
	wasAnimationFinished_ = isCurrentlyFinished;
}

void ClearPlayer::UpdateMatrix()
{
	worldTransform_.UpdateMatrix();
	neckTransform_.UpdateMatrix();
}

void ClearPlayer::Draw()
{
	
	neck_->Draw(camera_, neckTransform_);
}

void ClearPlayer::DrawAnimation() {
	obj_->Draw(camera_, worldTransform_);
}