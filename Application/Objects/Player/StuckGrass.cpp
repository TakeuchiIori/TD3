#include "StuckGrass.h"

#include "Player.h"
#include "../Application/SystemsApp/AppAudio/AudioVolumeManager.h"

#include "Collision/Core/ColliderFactory.h"
#include "Systems/GameTime/GameTIme.h"


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
	obj_->SetModel("tumari.obj");
	obj_->SetMaterialColor({ 0.90625f,0.87109f,0.125f,1.0f });

	InitCollision();
	InitJson();

	hakuSoundData_ = Audio::GetInstance()->LoadAudio(L"Resources/Audio/spitOut.mp3");
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
	timer_ += GameTime::GetDeltaTime();
	if (timer_ <= kScaleTime_)
	{
		worldTransform_.scale_ = Lerp(Vector3(0, 0, 0), Vector3(1.2f, 1.2f, 1.2f), timer_ / kScaleTime_);
	};

	worldTransform_.UpdateMatrix();

	aabbCollider_->Update();
}

void StuckGrass::Draw()
{
	if(isVisible_)
	{
		obj_->Draw(camera_, worldTransform_);
	}
}

void StuckGrass::DrawCollision()
{
	aabbCollider_->Draw();
}

void StuckGrass::OnEnterCollision(BaseCollider* self, BaseCollider* other)
{
	if (player_->behavior_ == BehaviorPlayer::Return)
	{
		if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer))
		{
			hakuSourceVoice_ = Audio::GetInstance()->SoundPlayAudio(hakuSoundData_);
			AudioVolumeManager::GetInstance()->SetSourceToSubmix(hakuSourceVoice_, kSE);
		}
	}
}

void StuckGrass::OnCollision(BaseCollider* self, BaseCollider* other)
{
	if (player_->behavior_ == BehaviorPlayer::Return)
	{
		if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer))
		{
			isVisible_ = false;
		}
	}
}

void StuckGrass::OnExitCollision(BaseCollider* self, BaseCollider* other)
{
	if (player_->behavior_ == BehaviorPlayer::Return)
	{
		if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer))
		{
			isPop_ = true;
		}
	}
}

void StuckGrass::OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir)
{
}
