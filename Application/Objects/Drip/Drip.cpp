#include "Drip.h"
#include <cmath>

Drip::Drip()
{
	jsonId_ = nextJsonId_;
	++nextJsonId_;
}

Drip::~Drip()
{
}

void Drip::Initialize(Camera* camera)
{
	// カメラをセット
	camera_ = camera;

	// Object3dの生成と初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();

	// モデルの読み込み（適切なモデルパスに変更してください）
	obj_->SetModel("Drip.obj");

	// WorldTransformの初期化
	worldTransform_.Initialize();

	// 非アクティブ状態で開始
	isActive_ = false;

	// JSON初期化（必要に応じて）
	//InitJson();
}

void Drip::Update()
{
	if (!isActive_) return;

	// 重力を適用
	velocity_.y -= gravity_;

	// 位置を更新
	worldTransform_.translation_ += velocity_;

	// フェードアウト処理
	alpha_ -= fadeSpeed_;
	if (alpha_ < 0.0f)
	{
		alpha_ = 0.0f;
		isActive_ = false;
	}

	// 透明度をオブジェクトに適用（Object3dにSetAlpha等のメソッドがある場合）
	obj_->SetAlpha(alpha_);

	// 画面外に出たら非アクティブに
	if (worldTransform_.translation_.y < -50.0f) // 適切な値に調整
	{
		isActive_ = false;
	}

	// WorldTransformの更新
	worldTransform_.UpdateMatrix();
}

void Drip::Draw()
{
	if (!isActive_) return;
	obj_->Draw(camera_,worldTransform_);
}

void Drip::Shoot(Vector3& startpos, const Vector3& targetPos)
{
	// アクティブ化
	isActive_ = true;

	// 初期位置を設定（現在の位置から開始）
	worldTransform_.translation_ = startpos;
	startPos_.y += static_cast<float>(jsonId_);

	// ターゲットへの方向ベクトルを計算
	Vector3 direction = targetPos - startPos_;

	// 方向ベクトルを正規化
	float length = std::sqrt(direction.x * direction.x +
		direction.y * direction.y +
		direction.z * direction.z);

	if (length > 0.0f)
	{
		direction.x /= length;
		direction.y /= length;
		direction.z /= length;
	}

	// 初速度を設定
		// 初速度を設定
	velocity_ = direction * speed_;

	// X方向の速度を左右判定で調整
	if (targetPos.x > startPos_.x) {
		// ターゲットが右側にある場合
		velocity_.x = std::abs(velocity_.x);  // 確実に正の値にする
	} else {
		// ターゲットが左側にある場合
		velocity_.x = -std::abs(velocity_.x); // 確実に負の値にする
	}


	// 透明度をリセット
	alpha_ = 1.0f;

	// Z軸回転の設定
	// 雫の下部分（元の画像では下向き）が進行方向を向くように設定
	// atan2は右向き(1,0)を0度とするため、下向き(0,-1)からの角度を計算
	worldTransform_.rotation_.z = std::atan2(direction.x, -direction.y);

	// もし雫の画像が上下逆の場合は以下を使用
	// worldTransform_.rotation_.z = std::atan2(-direction.x, direction.y);
}

void Drip::InitJson()
{
	std::string uniqueName = "Yodare" + std::to_string(jsonId_);
	 jsonManager_ = std::make_unique<JsonManager>(uniqueName,"Resources/Json/");
	 jsonManager_->SetCategory("Objects");
	 jsonManager_->SetTreePrefix("パラメータ");
	 jsonManager_->Register("重力", &gravity_);
	 jsonManager_->Register("速度", &speed_);
	 jsonManager_->Register("フェード速度", &fadeSpeed_);

	 jsonManager_->SetTreePrefix("SRT");
	 jsonManager_->Register("位置", &worldTransform_.translation_);
	 jsonManager_->Register("回転", &worldTransform_.rotation_);
	 jsonManager_->Register("拡縮", &worldTransform_.scale_);
}