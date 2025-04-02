#include "StuckGrass.h"

#include "Player.h"

#include "Collision/Core/ColliderFactory.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

int StuckGrass ::count_ = 0;

StuckGrass::~StuckGrass()
{
	--count_;
	aabbCollider_->~AABBCollider();
}

void StuckGrass::Initialize(Camera* camera)
{
	BaseObject::camera_ = camera;

	// トランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = { 1.2f,1.2f,1.2f };

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitCube.obj");
	obj_->SetMaterialColor({ 0.90625f,0.87109f,0.125f,1.0f });

	InitCollision();
	InitJson();
}

void StuckGrass::InitCollision()
{
	aabbCollider_ = ColliderFactory::Create<AABBCollider>(
		this,
		&worldTransform_,
		camera_,
		static_cast<uint32_t>(CollisionTypeIdDef::kStuckGrass)
	);
}

void StuckGrass::InitJson()
{
}

void StuckGrass::Update()
{
	worldTransform_.UpdateMatrix();

	aabbCollider_->Update();
}

void StuckGrass::Draw()
{
	obj_->Draw(BaseObject::camera_, worldTransform_);
}

void StuckGrass::DrawCollision()
{
	aabbCollider_->Draw();
}

void StuckGrass::OnEnterCollision(BaseCollider* self, BaseCollider* other)
{
}

void StuckGrass::OnCollision(BaseCollider* self, BaseCollider* other)
{
	if (player_->behavior_ == BehaviorPlayer::Return)
	{
		if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer))
		{
			isDelete_ = true;
		}
	}
}

void StuckGrass::OnExitCollision(BaseCollider* self, BaseCollider* other)
{
}
