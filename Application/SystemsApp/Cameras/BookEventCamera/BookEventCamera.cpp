#include "BookEventCamera.h"


// c++
#include <numbers>

void BookEventCamera::Initialize()
{
    controlPoints_.clear();
    //controlPoints_.push_back({ 0, 10, -60 });
    //controlPoints_.push_back({ 0, 8, -40 });
    //controlPoints_.push_back({ 0, 6, -20 });
    //controlPoints_.push_back({ 0, 5, 0 });
    translate_.z = -40.0f;
    InitJson();
}

void BookEventCamera::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("BookEventCamera", "Resources/JSON/");
	jsonManager_->SetCategory("Cameras");
	jsonManager_->SetSubCategory("BookEventCamera");
	//jsonManager_->Register("Translate", &translate_);
	jsonManager_->Register("Rotate", &rotate_);
	jsonManager_->Register("カメラの移動速度", &speed_);
}

void BookEventCamera::Update()
{
    t_ += speed_;
    if (t_ >= float(controlPoints_.size() - 3)) {
        t_ = float(controlPoints_.size() - 3);
    }

    translate_ = EvaluateSpline(t_);

    if (target_) {
		matView_ = MakeLookAtMatrix(translate_, target_->translation_, Vector3(0, 1, 0));
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

Vector3 BookEventCamera::EvaluateSpline(float t)
{
	int count = static_cast<int>(controlPoints_.size());
	if (count < 4) {
        return Vector3(0, 0, 0);
	}

    int i = std::clamp(int(t), 1, count - 3);
    float localT = t - float(i);

    const Vector3& p0 = controlPoints_[i - 1];
    const Vector3& p1 = controlPoints_[i];
    const Vector3& p2 = controlPoints_[i + 1];
    const Vector3& p3 = controlPoints_[i + 2];

    return
        0.5f * (
            (2.0f * p1) +
            (-p0 + p2) * localT +
            (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * localT * localT +
            (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * localT * localT * localT
            );




}
