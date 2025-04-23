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
	obj_->SetModel("book.obj");
	obj_->SetMaterialColor(Vector3{0.0,1.0f,0.0f});
	worldTransform_.Initialize();

	worldTransform_.translation_ = { 25.0f,2.0f,0.0f };

	input_ = Input::GetInstance();


	InitCollision();
	InitJson();


	uiBook_ = std::make_unique<Sprite>();
	uiBook_->Initialize("Resources/Textures/Option/operation_yodare.png");
	uiBook_->SetSize({ 150.0f, 100.0f });

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
	jsonManager_->Register("Rotate", &worldTransform_.rotation_);

	jsonCollider_ = std::make_unique<JsonManager>("BookCollider", "Resources/JSON/");
	obbCollider_->InitJson(jsonCollider_.get());
}

void Book::Update()
{
	UpdateMatrix();
	obbCollider_->Update();
	UpdateSprite();
}

void Book::UpdateSprite()
{
	Vector3 newPos = worldTransform_.translation_;
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1);
	Matrix4x4 matViewProjectionViewport = Multiply(camera_->GetViewMatrix(), Multiply(camera_->GetProjectionMatrix(), matViewport));
	newPos = Transform(newPos, matViewProjectionViewport);
	newPos += offset_;
	uiBook_->SetPosition(newPos);

	uiBook_->Update();
	//uiBook_->ImGUi();
}

void Book::UpdateMatrix()
{
	worldTransform_.UpdateMatrix();
}

void Book::Draw()
{
	obj_->Draw(BaseObject::camera_, worldTransform_);

}

void Book::DrawSprite()
{
	uiBook_->Draw();
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
		if (OnBookTrigger_)
		{
			OnBookTrigger_();
		}
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
