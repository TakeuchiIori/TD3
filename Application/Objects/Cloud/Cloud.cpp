#include "Cloud.h"

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

    // JSON初期化（コンストラクタで設定されたIDを使用）
    InitJson();

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
}

void Cloud::Update()
{
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