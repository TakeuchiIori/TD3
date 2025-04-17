#include "Giraffe.h"

void Giraffe::Initialize()
{
	// Initialize the head, neck, and body objects
	head = std::make_unique<Object3d>();
	neck = std::make_unique<Object3d>();
	body = std::make_unique<Object3d>();

	head->Initialize();
	neck->Initialize();
	body->Initialize();

	head->SetModel("head.obj");
	neck->SetModel("neck.obj");
	body->SetModel("body.obj");

	// Initialize the world transforms
	headTransform.Initialize();
	neckTransform.Initialize();
	bodyTransform.Initialize();

	InitJson();
}

void Giraffe::InitJson()
{
	headJson_ = std::make_unique<JsonManager>("head", "Resources/JSON/");
	neckJson_ = std::make_unique<JsonManager>("neck", "Resources/JSON/");
	bodyJson_ = std::make_unique<JsonManager>("body", "Resources/JSON/");

	headJson_->SetCategory("Objects");
	headJson_->SetSubCategory("head");
	headJson_->Register("scale", &headTransform.scale_);
	headJson_->Register("rotate", &headTransform.rotation_);
	headJson_->Register("translate", &headTransform.translation_);

	neckJson_->SetCategory("Objects");
	neckJson_->SetSubCategory("neck");
	neckJson_->Register("scale", &neckTransform.scale_);
	neckJson_->Register("rotate", &neckTransform.rotation_);
	neckJson_->Register("translate", &neckTransform.translation_);

	bodyJson_->SetCategory("Objects");
	bodyJson_->SetSubCategory("body");
	bodyJson_->Register("scale", &bodyTransform.scale_);
	bodyJson_->Register("rotate", &bodyTransform.rotation_);
	bodyJson_->Register("translate", &bodyTransform.translation_);

}

void Giraffe::Update()
{


	headTransform.UpdateMatrix();
	neckTransform.UpdateMatrix();
	bodyTransform.UpdateMatrix();
}

void Giraffe::Draw()
{
	head->Draw(camera_, headTransform);
	neck->Draw(camera_, neckTransform);
	body->Draw(camera_, bodyTransform);
}
