#include "BookEventCamera.h"


// c++
#include <numbers>
#include <string>

void BookEventCamera::Initialize()
{
    translate_.z = -82.5f;
    InitJson();
	t_ = 0.0f;
}

void BookEventCamera::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("BookEventCamera", "Resources/JSON/");
	jsonManager_->SetCategory("Cameras");
	jsonManager_->SetSubCategory("BookEventCamera");
	jsonManager_->Register("Rotate", &rotate_);
	jsonManager_->Register("カメラの移動速度", &speed_);


    jsonManager_->Register("制御点", &controlPoints_);
}

void BookEventCamera::Update()
{
    t_ += speed_;
    if (t_ > float(controlPoints_.size()-1)) {
        t_ = float(controlPoints_.size()-1);
    }

    translate_ = EvaluateSpline(t_);

    // ターゲットがあれば、その位置を向くようにビュー行列を作る
    if (target_) {
        rotate_ = GetEulerAnglesFromToDirection(translate_, target_->translation_);
        matView_ = Inverse(MakeAffineMatrix(scale_, rotate_, translate_));
    }
}

void BookEventCamera::FollowProsess()
{
    // ターゲットがない場合は処理しない
    if (target_ == nullptr)
    {
        return;
    }
    Vector3 offset = offset_;



    Matrix4x4 rotate = MakeRotateMatrixXYZ(rotate_);

    offset = TransformNormal(offset, rotate);

    // このゲーム用にX、Z軸は無視する
    Vector3 targetTranslate = Vector3(0.0f, target_->translation_.y, 0);

    translate_ = targetTranslate + offset;

    matView_ = Inverse(MakeAffineMatrix(scale_, rotate_, translate_));
}



void BookEventCamera::Draw(Camera* camera)
{
    for (size_t i = 0; i < obj_.size(); ++i) {
        wt_[i]->UpdateMatrix();
        obj_[i]->Draw(camera,*wt_[i]);
    }
}


void BookEventCamera::RegisterControlPoints()
{
    if (wt_.size() < controlPoints_.size()) {
        for (size_t i = wt_.size(); i < controlPoints_.size(); ++i) {
            auto wt = std::make_unique<WorldTransform>();
            wt->Initialize();
            wt->translation_ = controlPoints_[i];
            wt_.push_back(std::move(wt));

            auto obj = std::make_unique<Object3d>();
            obj->Initialize();
            obj->SetModel("unitCube.obj");
            obj->SetMaterialColor({ 1.0f, 0.0f, 1.0f, 1.0f });
            obj_.push_back(std::move(obj));
        }
    }

    // 制御点からワールドトランスフォームを更新
    for (size_t i = 0; i < std::min(wt_.size(), controlPoints_.size()); ++i) {
        wt_[i]->translation_ = controlPoints_[i];
    }
}

Vector3 BookEventCamera::EvaluateSpline(float t)
{
    if (controlPoints_.size() < 2) return Vector3{};

    int seg = std::clamp(int(t), 0, int(controlPoints_.size() - 2));
    float localT = t - float(seg);

    return Lerp(controlPoints_[seg], controlPoints_[seg + 1], localT);




}

