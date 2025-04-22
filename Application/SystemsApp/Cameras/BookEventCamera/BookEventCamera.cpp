#include "BookEventCamera.h"


// c++
#include <numbers>
#include <string>

void BookEventCamera::Initialize()
{
    translate_.z = -40.0f;
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

    if (target_) {
		matView_ = MakeLookAtMatrix(translate_, target_->translation_, Vector3(0, 3, 0));
    }
}


void BookEventCamera::Draw(Camera* camera)
{
    for (size_t i = 0; i < obj_.size(); ++i) {
        wt_[i]->UpdateMatrix();
        obj_[i]->Draw(camera,*wt_[i]);
    }
}


void BookEventCamera::GenerateControlPoints(const Vector3& endPos, int num)
{
	controlPoints_.clear();

    num = (std::max)(num, 4);

    Vector3 startPos = translate_;


    for (int i = 0; i < num; i++) {
		float t = static_cast<float>(i) / static_cast<float>(num - 1);

		Vector3 point = Lerp(startPos, endPos, t);

        ///point.x -= std::sin(t * std::numbers::pi_v<float>) * 20.0f;

		controlPoints_.push_back(point);
    }

    t_ = 0.0f;
}

void BookEventCamera::AddControlPoint(const Vector3& point)
{
	controlPoints_.push_back(point);
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

void BookEventCamera::ImGui()
{
//#ifdef _DEBUG
//    if (ImGui::TreeNode("Control Points")) {
//        for (size_t i = 0; i < controlPoints_.size(); ++i) {
//            std::string label = "Point " + std::to_string(i);
//            ImGui::DragFloat3(label.c_str(), &controlPoints_[i].x, 0.1f);
//        }
//
//        if (ImGui::Button("Add Point")) {
//            controlPoints_.push_back({ 0, 0, 0 });
//        }
//        if (ImGui::Button("Clear Points")) {
//            controlPoints_.clear();
//        }
//        ImGui::TreePop();
//    }
//#endif // _DEBUG

}
