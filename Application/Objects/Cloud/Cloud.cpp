#include "Cloud.h"

Cloud::~Cloud()
{
    // WorldTransformのメモリを解放
    for (auto* wt : wt_) {
        delete wt;
    }
    wt_.clear();
}

void Cloud::InitJson()
{
    // JsonManagerの初期化
    jsonManager_ = std::make_unique<JsonManager>("Cloud", "Resources/JSON/");
    jsonManager_->SetCategory("Objects");
    jsonManager_->SetSubCategory("Cloud");

    // 移動関連パラメータ
    jsonManager_->Register("移動速度", &moveSpeed_);
    jsonManager_->Register("リセット距離", &resetDistance_);

    // 雲1の設定
    jsonManager_->SetTreePrefix("雲1");
    jsonManager_->Register("位置", &cloud1Position_);
    jsonManager_->Register("スケール", &cloud1Scale_);

    // 雲2の設定
    jsonManager_->SetTreePrefix("雲2");
    jsonManager_->Register("位置", &cloud2Position_);
    jsonManager_->Register("スケール", &cloud2Scale_);

    jsonManager_->ClearTreePrefix();
}

void Cloud::Initialize(Camera* camera)
{
    // パラメータの初期値設定
    //moveSpeed_ = 0.01f;
    //resetDistance_ = 200.0f;
    //cloud1Position_ = { -50.0f, 20.0f, 100.0f };
    //cloud1Scale_ = { 2.0f, 1.5f, 2.0f };
    //cloud2Position_ = { 80.0f, 25.0f, 150.0f };
    //cloud2Scale_ = { 2.0f, 1.5f, 2.0f };

	camera_ = camera;


    // JSON初期化
    InitJson();

    // 2つの雲を生成
    for (int i = 0; i < 2; ++i) {
        // Object3dを生成
        std::unique_ptr<Object3d> cloud = std::make_unique<Object3d>();
        cloud->Initialize();
        cloud->SetModel("cloud.obj"); // 雲モデル（適切なモデル名に変更してください）

        // WorldTransformを生成
        WorldTransform* worldTransform = new WorldTransform();
        worldTransform->Initialize();

        // 雲の位置とスケールを設定
        if (i == 0) {
            // 1つ目の雲
            worldTransform->translation_ = cloud1Position_;
            worldTransform->scale_ = cloud1Scale_;
        } else if (i == 1) {
            // 2つ目の雲
            worldTransform->translation_ = cloud2Position_;
            worldTransform->scale_ = cloud2Scale_;
        }

        obj_.emplace_back(std::move(cloud));
        wt_.emplace_back(worldTransform);
    }


}

void Cloud::Update()
{
    for (size_t i = 0; i < obj_.size(); ++i) {
        //// 雲をゆっくりと移動させる（JsonManagerで調整可能な速度を使用）
        //wt_[i]->translation_.x += moveSpeed_;

        //// 範囲外に出たら反対側から出現（JsonManagerで調整可能な距離を使用）
        //if (wt_[i]->translation_.x > resetDistance_) {
        //    wt_[i]->translation_.x = -resetDistance_;
        //}

        if (i == 0) {
            wt_[i]->translation_= cloud1Position_;
            wt_[i]->scale_ = cloud1Scale_;
        } else if (i == 1) {
            wt_[i]->translation_ = cloud2Position_;
            wt_[i]->scale_ = cloud2Scale_;
        }

        // WorldTransformを更新
        wt_[i]->UpdateMatrix();

    }
}

void Cloud::Draw()
{
    for (size_t i = 0; i < obj_.size(); ++i) {
        // WorldTransformを渡して描画
        obj_[i]->Draw(camera_,*wt_[i]);
    }
}