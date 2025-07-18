#include "CheckPoint.h"


void CheckPoint::Initialize(Camera* camera)
{
//#ifdef _DEBUG
	DebugInitialize(camera);
//#endif // _DEBUG

}

void CheckPoint::InitJson()
{
}


//#ifdef _DEBUG
void CheckPoint::DebugInitialize(Camera* camera)
{
	worldTransform_.Initialize();
	worldTransform_.translation_ = { 17.0f, checkPointHight_, 0.2f };
	worldTransform_.scale_ = { 20,1,1 };
	//worldTransform_.UpdateMatrix();

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("check.obj");
	obj_->SetMaterialColor(Vector3{ 1,1,1 });
	obj_->uvScale.x = worldTransform_.scale_.x;
	camera_ = camera;
}

void CheckPoint::DebugUpdate()
{
	worldTransform_.translation_.y = checkPointHight_;
	worldTransform_.UpdateMatrix();
}

void CheckPoint::DebugDraw()
{
	obj_->Draw(camera_, worldTransform_);
}
//#endif // _DEBUG

