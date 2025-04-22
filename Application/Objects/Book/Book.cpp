#include "Book.h"
#include "Collision/OBB/OBBCollider.h"
#include "Collision/Core/ColliderFactory.h"

Book::~Book()
{
}

void Book::Initialize(Camera* camera)
{
	BaseObject::camera_ = camera;

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitCube.obj");
	obj_->SetMaterialColor(Vector3{0.0,1.0f,0.0f});
	worldTransform_.Initialize();

	worldTransform_.translation_ = { 25.0f,2.0f,0.0f };

	input_ = Input::GetInstance();


	InitCollision();
	InitJson();

}

void Book::InitCollision()
{
	obbCollider_ = ColliderFactory::Create<OBBCollider>(
		this,
		&worldTransform_,
		camera_,
		static_cast<uint32_t>(CollisionTypeIdDef::kBook)
	);
}

void Book::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("Book", "Resources/JSON/");
	jsonManager_->SetCategory("Objects");
	jsonManager_->SetSubCategory("Book");

	jsonCollider_ = std::make_unique<JsonManager>("BookCollider", "Resources/JSON/");
	jsonCollider_->SetCategory("Collider");
	jsonCollider_->SetSubCategory("BookCollider");
}

void Book::Update()
{
	UpdateMatrix();
	obbCollider_->Update();
}

void Book::UpdateMatrix()
{
	worldTransform_.UpdateMatrix();
}

void Book::Draw()
{
	obj_->Draw(BaseObject::camera_, worldTransform_);

}

void Book::DrawCollision()
{
	obbCollider_->Draw();
}

void Book::MapChipOnCollision(const CollisionInfo& info)
{
}

void Book::Reset()
{
}

void Book::Move()
{

}

void Book::OnEnterCollision(BaseCollider* self, BaseCollider* other)
{
	if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer))
	{
		obj_->SetMaterialColor(Vector3{ 1.0,1.0f,0.0f });
	}
}

void Book::OnCollision(BaseCollider* self, BaseCollider* other)
{

}

void Book::OnExitCollision(BaseCollider* self, BaseCollider* other)
{
}

void Book::OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir)
{
}
