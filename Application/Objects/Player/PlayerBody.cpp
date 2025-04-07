#include "PlayerBody.h"

#include "Collision/Core/ColliderFactory.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

int PlayerBody::count_ = 0;

PlayerBody::~PlayerBody()
{
	--count_;
	aabbCollider_->~AABBCollider();
}

void PlayerBody::Initialize(Camera* camera)
{
	BaseObject::camera_ = camera;

	// トランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.useAnchorPoint_ = true;

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitCube.obj");
	obj_->SetMaterialColor({ 0.90625f,0.87109f,0.125f,1.0f });

	InitCollision();
	InitJson();
	aabbCollider_->checkOutsideCamera = false;
}

void PlayerBody::InitCollision()
{
	aabbCollider_ = ColliderFactory::Create<AABBCollider>(
		this,
		&worldTransform_,
		camera_,
		static_cast<uint32_t>(CollisionTypeIdDef::kPlayerBody)
	);
}

void PlayerBody::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("playerBodyObj", "Resources/JSON/");

	jsonCollider_ = std::make_unique<JsonManager>("playerBodyCollider", "Resources/JSON/");
	//SphereCollider::InitJson(jsonCollider_.get());
	aabbCollider_->InitJson(jsonCollider_.get());
}

void PlayerBody::Update()
{
	worldTransform_.UpdateMatrix();
	ExtendUpdate();

	aabbCollider_->Update();

#ifdef _DEBUG
	std::string sid = std::to_string(id_);
	const char* id = sid.c_str();
	ImGui::Begin("PlayerBody");
	ImGui::DragFloat3(id, &worldTransform_.translation_.x);



	ImGui::End();
#endif // _DEBUG

}

void PlayerBody::Draw()
{
	obj_->Draw(BaseObject::camera_, worldTransform_);
}

void PlayerBody::DrawCollision()
{
	aabbCollider_->Draw();
}

void PlayerBody::UpExtend()
{
	worldTransform_.anchorPoint_ = { 0.0f,-1.0f,0.0f };
	worldTransform_.scale_ = verticalGrowthScale_;
	extendDirection_ = ExtendDirection::Up;
}

void PlayerBody::LeftExtend()
{
	worldTransform_.anchorPoint_ = { 1.0f,0.0f,0.0f };
	worldTransform_.scale_ = horizontalGrowthScale_;
	extendDirection_ = ExtendDirection::Left;
}

void PlayerBody::RightExtend()
{
	worldTransform_.anchorPoint_ = { -1.0f,0.0f,0.0f };
	worldTransform_.scale_ = horizontalGrowthScale_;
	extendDirection_ = ExtendDirection::Right;
}

void PlayerBody::DownExtend()
{
	worldTransform_.anchorPoint_ = { 0.0f,1.0f,0.0f };
	worldTransform_.scale_ = verticalGrowthScale_;
	extendDirection_ = ExtendDirection::Down;
}

void PlayerBody::OnEnterCollision(BaseCollider* self, BaseCollider* other)
{
}

void PlayerBody::OnCollision(BaseCollider* self, BaseCollider* other)
{
}

void PlayerBody::OnExitCollision(BaseCollider* self, BaseCollider* other)
{
}

void PlayerBody::OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir)
{
}

void PlayerBody::ExtendUpdate()
{
	float length = Length(endPos_ - startPos_);
	switch (extendDirection_)
	{
	default:
	case ExtendDirection::Up:
		worldTransform_.scale_ = verticalGrowthScale_ + (Vector3{ 0.0f,0.5f,0.0f } * length);

		break;

	case ExtendDirection::Left:
		worldTransform_.scale_ = horizontalGrowthScale_ + (Vector3{ 0.5f,0.0f,0.0f } * length);

		break;

	case ExtendDirection::Right:
		worldTransform_.scale_ = horizontalGrowthScale_ + (Vector3{ 0.5f,0.0f,0.0f } * length);

		break;

	case ExtendDirection::Down:
		worldTransform_.scale_ = verticalGrowthScale_ + (Vector3{ 0.0f,0.5f,0.0f } * length);

		break;
	}
}
