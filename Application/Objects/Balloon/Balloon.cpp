#include "Balloon.h"
#include "../../../Engine/Utility/Systems/GameTime/GameTIme.h"

void Balloon::Initialize(Camera* camera)
{
	input_ = Input::GetInstance();

	BaseObject::camera_ = camera;

	behaviortRquest_ = BehaviorBalloon::kROOT;

	InitJson();
	InitCollision();

	// トランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = balloonScale_;

	worldTransform_.UpdateMatrix();
	worldTransform_.translation_.z = 1.2f;

	colliderWT_.Initialize();
	colliderWT_.scale_ = worldTransform_.scale_;
	colliderWT_.translation_ = worldTransform_.translation_;
	colliderWT_.UpdateMatrix();

	stopAreaWT_.Initialize();
	stopAreaWT_.scale_ = stopAreaScale_;
	stopAreaWT_.translation_ = worldTransform_.translation_;
	stopAreaWT_.UpdateMatrix();

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("Balloon.obj");

	timerObj_ = std::make_unique<Object3d>();
	timerObj_->Initialize();
	timerObj_->SetModel("BalloonTimer.obj");

	soundData_ = Audio::GetInstance()->LoadAudio(L"Resources/Audio/balloon.mp3");

	uiTime_ = std::make_unique<Sprite>();
	uiTime_->Initialize("Resources/Textures/In_Game/timerPlus.png");
	uiTime_->SetPosition(endTimePos_);
	uiTime_->SetSize({150.0f, 75.0f});
	uiTime_->SetAnchorPoint({ 0.5f, 1.0f });

	yodare_ = std::make_unique<Sprite>();
	yodare_->Initialize("Resources/Textures/In_Game/balloonYodare.png");
	yodare_->SetAnchorPoint({ 0.5f, 0.9f });
	yodare_->SetSize(yodare_->GetTextureSize() * 0.4f);
}

void Balloon::Update()
{
	BehaviorInitialize();
	BehaviorUpdate();

	ColliderOffset();

	worldTransform_.UpdateMatrix();
	colliderWT_.UpdateMatrix();
	stopAreaWT_.UpdateMatrix();
	if (timer < kTime)
	{
		uiTime_->SetPosition(startTimepPos_);
		timer += GameTime::GetDeltaTime();
		if (timer > kMidTime)
		{
			float t = (timer - kMidTime) / (kTime - kMidTime);

			uiTime_->SetPosition(Lerp(startTimepPos_, endTimePos_, t));
		}
	}
	uiTime_->Update();
	yodare_->Update();
}

void Balloon::Draw()
{
	if (isVisible_)
	{
		obj_->Draw(camera_, worldTransform_);
		timerObj_->Draw(camera_, worldTransform_);
	}
}

void Balloon::DrawSprite()
{
	if (timer < kTime)
	{
		uiTime_->Draw();
	}
	if (behavior_ == BehaviorBalloon::kSTOP)
	{
		yodare_->Draw();
	}
}

void Balloon::BehaviorInitialize()
{
	if (behaviortRquest_)
	{
		// 振る舞いを変更する
		behavior_ = behaviortRquest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_)
		{
		case BehaviorBalloon::kROOT:
		default:
			BehaviorRootInit();
			break;
		case BehaviorBalloon::kUP:
			BehaviorUPInit();
			break;
		case BehaviorBalloon::kSTOP:
			BehaviorSTOPInit();
			break;
		}
		// 振る舞いリクエストをリセット
		behaviortRquest_ = std::nullopt;
	}
}

void Balloon::BehaviorUpdate()
{
	switch (behavior_)
	{
	case BehaviorBalloon::kROOT:
	default:
		BehaviorRootUpdate();
		break;
	case BehaviorBalloon::kUP:
		BehaviorUPUpdate();
		break;
	case BehaviorBalloon::kSTOP:
		BehaviorSTOPUpdate();
		break;
	}
}

void Balloon::BehaviorRootInit()
{
	isVisible_ = false;
	aabbCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kNone));
	aabbStopCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kNone));
	worldTransform_.translation_.y = defaultY_;
}

void Balloon::BehaviorRootUpdate()
{
}

void Balloon::BehaviorUPInit()
{
	isVisible_ = true;
	aabbCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kBalloon));
	aabbStopCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kStopArea));
	worldTransform_.translation_.y = defaultY_;

	// X座標をランダムでスポーン
	std::mt19937 random(seedGene_());
	std::uniform_real_distribution<float> posX(minX_, maxX_);
	spawnX_ = posX(random);
	worldTransform_.translation_.x = spawnX_;
}

void Balloon::BehaviorUPUpdate()
{
	worldTransform_.translation_.y += upY_;
}

void Balloon::BehaviorSTOPInit()
{
}

void Balloon::BehaviorSTOPUpdate()
{
	Vector3 pos = worldTransform_.translation_;
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1);
	Matrix4x4 matViewProjectionViewport = Multiply(camera_->GetViewMatrix(), Multiply(camera_->GetProjectionMatrix(), matViewport));
	pos = Transform(pos, matViewProjectionViewport);
	yodare_->SetPosition(pos + offsetYodare_);
}

void Balloon::ColliderOffset()
{
	colliderWT_.translation_ = worldTransform_.translation_;
	colliderWT_.translation_.y += collisionOffsetY_;
	colliderWT_.scale_ = worldTransform_.scale_;
	colliderWT_.scale_.y = worldTransform_.scale_.y * scaleOffsetY_;
	stopAreaWT_.translation_ = colliderWT_.translation_;
	stopAreaWT_.scale_ = stopAreaScale_;
	stopAreaWT_.scale_.y = stopAreaScale_.y * scaleOffsetY_;

	aabbCollider_->Update();
	aabbStopCollider_->Update();
}

void Balloon::InitCollision() 
{
	aabbCollider_ = ColliderFactory::Create<AABBCollider>(
		this,
		&colliderWT_,
		camera_,
		static_cast<uint32_t>(CollisionTypeIdDef::kNone)
	);


	aabbStopCollider_ = ColliderFactory::Create<AABBCollider>(
		this,
		&stopAreaWT_,
		camera_,
		static_cast<uint32_t>(CollisionTypeIdDef::kNone)
	);
}

void Balloon::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("Balloon", "Resources/JSON/");
	jsonManager_->SetCategory("Objects");
	jsonManager_->SetSubCategory("Balloon");
	jsonManager_->Register("気球の大きさ", &balloonScale_);
	jsonManager_->Register("よだれエリアの大きさ", &stopAreaScale_);
	jsonManager_->Register("X座標の最小", &minX_);
	jsonManager_->Register("X座標の最大", &maxX_);
	jsonManager_->Register("Y座標の初期位置", &defaultY_);
	jsonManager_->Register("Y座標の上昇速度", &upY_);
	jsonManager_->Register("コライダーのY座標オフセット", &collisionOffsetY_);
	jsonManager_->Register("コライダーのYスケールオフセット", &scaleOffsetY_);
	jsonManager_->Register("何個目のチェックポイントから気球が出てくるか", &enableMapNum_);
}

void Balloon::DrawCollision() 
{
	aabbCollider_->Draw();
	aabbStopCollider_->Draw();
}

void Balloon::OnEnterCollision(BaseCollider* self, BaseCollider* other)
{
	if (self->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kBalloon))
	{
		if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)) // プレイヤーなら
		{
			Vector3 pos = worldTransform_.translation_;
			Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1);
			Matrix4x4 matViewProjectionViewport = Multiply(camera_->GetViewMatrix(), Multiply(camera_->GetProjectionMatrix(), matViewport));
			pos = Transform(pos, matViewProjectionViewport);
			startTimepPos_ = pos + offsetPos;
			timer = 0;
			getSoundSource_ = Audio::GetInstance()->SoundPlayAudio(soundData_);
			AudioVolumeManager::GetInstance()->SetSourceToSubmix(getSoundSource_, kSE);
			aabbCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kNone));
			aabbStopCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kNone));
			behaviortRquest_ = BehaviorBalloon::kROOT;
		}
	}
}

void Balloon::OnCollision(BaseCollider* self, BaseCollider* other)
{
	if (self->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kStopArea))
	{
		if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)) // プレイヤーなら
		{
			if (input_->IsPadTriggered(0, GamePadButton::B))
			{
				
				behaviortRquest_ = BehaviorBalloon::kSTOP;
			}
		}
	}
}

void Balloon::OnExitCollision(BaseCollider* self, BaseCollider* other) 
{
}

void Balloon::OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir) 
{
}

void Balloon::BehaviorTransition()
{
	behaviortRquest_ = BehaviorBalloon::kUP;
}
