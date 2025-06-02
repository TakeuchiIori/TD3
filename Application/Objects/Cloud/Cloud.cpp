#include "Cloud.h"
#include "Systems/GameTime/GameTime.h"
Cloud::Cloud()
{
    // コンストラクタで自動的にIDを割り当て、次のIDを準備
    cloudId_ = nextCloudId_;
    nextCloudId_++;
}

Cloud::~Cloud()
{
    // WorldTransformのメモリを解放
    delete cloudWorldTransform_;
}

void Cloud::InitJson()
{
    std::string uniqueName = "Cloud_" + std::to_string(cloudId_);

    // JsonManagerの初期化
    jsonManager_ = std::make_unique<JsonManager>(uniqueName, "Resources/JSON/");
    jsonManager_->SetCategory("Objects");
    jsonManager_->SetSubCategory("Cloud");

    // 移動関連パラメータ
    jsonManager_->Register("移動速度", &moveSpeed_);
    jsonManager_->Register("リセット距離", &resetDistance_);

    // 雲の設定
    jsonManager_->Register("位置", &cloudPosition_);
    jsonManager_->Register("スケール", &cloudScale_);
}

void Cloud::Initialize(Camera* camera)
{
    camera_ = camera;



    // 雲を生成
    cloud_ = std::make_unique<Object3d>();
    cloud_->Initialize();
    cloud_->SetModel("cloud.obj"); // 雲モデル

    // WorldTransformを生成
    cloudWorldTransform_ = new WorldTransform();
    cloudWorldTransform_->Initialize();

    // 雲の位置とスケールを設定
    cloudWorldTransform_->translation_ = cloudPosition_;
    cloudWorldTransform_->scale_ = cloudScale_;

    // JSON初期化（コンストラクタで設定されたIDを使用）
    InitJson();
}

void Cloud::Update()
{
    // 雲の位置とスケールを更新
    cloudWorldTransform_->translation_ = cloudPosition_;
    cloudWorldTransform_->scale_ = cloudScale_;

    // WorldTransformを更新
    cloudWorldTransform_->UpdateMatrix();
}

void Cloud::UpdateScale()
{
    // アニメーションタイマーを更新
    animationTime_ += GameTime::GetDeltaTime();

    // sin波を使ってぷよぷよさせる
    float scaleOffset = std::sin(animationTime_ * animationSpeed_) * scaleAmplitude_;

    // 各軸で異なる位相を持たせることで、より自然な動きに
    float scaleX = baseScale_.x + scaleOffset;
    float scaleY = baseScale_.y + std::sin(animationTime_ * animationSpeed_ + 1.57f) * scaleAmplitude_; // 位相を90度ずらす
    float scaleZ = baseScale_.z + std::sin(animationTime_ * animationSpeed_ + 3.14f) * scaleAmplitude_ * 0.5f; // Z軸は控えめに

    // スケールを更新
    cloudScale_ = { scaleX, scaleY, scaleZ };

    // 雲の位置とスケールを更新
    cloudWorldTransform_->translation_ = cloudPosition_;
    cloudWorldTransform_->scale_ = cloudScale_;

    // WorldTransformを更新
    cloudWorldTransform_->UpdateMatrix();
}

void Cloud::Draw()
{
    // WorldTransformを渡して描画
    cloud_->Draw(camera_, *cloudWorldTransform_);
}