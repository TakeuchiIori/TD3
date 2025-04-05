#include "Branch.h"

#include "Collision/Core/ColliderFactory.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

void Branch::Initialize(Camera* camera)
{
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.parent_ = grassWorldTransform_;
	worldTransform_.useAnchorPoint_ = true;
	

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitCube.obj");
	obj_->SetMaterialColor({ 0.4627f,0.3412f,0.2353f,1.0f });// 茶色(#76573C)

	InitCollision();
	InitJson();
}

void Branch::InitCollision()
{
	aabbCollider_ = ColliderFactory::Create<AABBCollider>(
		this,
		&worldTransform_,
		camera_,
		static_cast<uint32_t>(CollisionTypeIdDef::kGrass)
	);
}

void Branch::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("branchObj", "Resources/JSON/");

	jsonCollider_ = std::make_unique<JsonManager>("branchCollider", "Resources/JSON/");
	aabbCollider_->InitJson(jsonCollider_.get());
}

void Branch::Update()
{
	worldTransform_.UpdateMatrix();
	aabbCollider_->Update();
}

void Branch::Draw()
{
	obj_->Draw(camera_, worldTransform_);
}

void Branch::DrawCollision()
{
	aabbCollider_->Draw();
}

void Branch::SetRight()
{
	worldTransform_.translation_.x = 2.0f;
	worldTransform_.anchorPoint_ = { -1.0f,0.0f,0.0f };
	float scaleX = rightLimit_ - grassWorldTransform_->translation_.x;
	worldTransform_.scale_ = { scaleX * 0.5f,0.5f,0.5f };
}

void Branch::SetLeft()
{
	worldTransform_.translation_.x = -2.0f;
	worldTransform_.anchorPoint_ = { 1.0f,0.0f,0.0f };
	float scaleX = grassWorldTransform_->translation_.x - leftLimit_;
	worldTransform_.scale_ = { scaleX * 0.5f,0.5f,0.5f };
}

void Branch::OnEnterCollision(BaseCollider* self, BaseCollider* other)
{
}

void Branch::OnCollision(BaseCollider* self, BaseCollider* other)
{
}

void Branch::OnExitCollision(BaseCollider* self, BaseCollider* other)
{
}

void Branch::OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir)
{
}
