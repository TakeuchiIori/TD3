#include "Grass.h"

//



#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

int Grass::count_ = 0;

Grass::~Grass()
{
	--count_;
	sphereCollider_->~SphereCollider();
}

void Grass::Initialize(Camera* camera)
{
	input_ = Input::GetInstance();

	BaseObject::camera_ = camera;


	// トランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = { 1.0f,1.0f,1.0f };

	//
	worldTransform_.translation_ = { 8.0f,5.0f,6.0f };
	worldTransform_.UpdateMatrix();

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitCube.obj");
	obj_->SetMaterialColor({ 0.3f,1.0f,0.3f,1.0f });

	InitCollision();
	InitJson();
}

void Grass::InitCollision()
{
	// OBB
	sphereCollider_ = ColliderFactory::Create<SphereCollider>(
		this,
		&worldTransform_,
		camera_,
		static_cast<uint32_t>(CollisionTypeIdDef::kGrass)
	);
}

void Grass::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("grassObj", "Resources/JSON/");

	jsonCollider_ = std::make_unique<JsonManager>("grassCollider", "Resources/JSON/");
	sphereCollider_->InitJson(jsonCollider_.get());
}

void Grass::Update()
{
	BehaviorInitialize();
	BehaviorUpdate();

	worldTransform_.UpdateMatrix();
	sphereCollider_->Update();


#ifdef _DEBUG
	DebugGrass();
#endif // _DEBUG
}

void Grass::Draw()
{
	obj_->Draw(BaseObject::camera_, worldTransform_);
}

void Grass::DrawCollision()
{
	sphereCollider_->Draw();
}

void Grass::OnEnterCollision(BaseCollider* self, BaseCollider* other)
{
	if (player_->behavior_ != BehaviorPlayer::Return)
	{
		if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)) // プレイヤーなら
		{
			worldTransform_.scale_ = { 0.0f,0.0f,0.0f };
			if (isMadeByPlayer_)
			{
				behaviortRquest_ = BehaviorGrass::Delete;
			}
		}
	}
}

void Grass::OnCollision(BaseCollider* self, BaseCollider* other)
{
}

void Grass::OnExitCollision(BaseCollider* self, BaseCollider* other)
{
	if (player_->behavior_ == BehaviorPlayer::Return)
	{
		if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)) // プレイヤーなら
		{
			if (!isMadeByPlayer_)
			{
				behaviortRquest_ = BehaviorGrass::Repop;
			}
		}
	}
}


#ifdef _DEBUG
void Grass::DebugGrass()
{
	float t = 1.0f - (growthTimer_ / kGrowthTime_);
	ImGui::Begin("DebugGrass");
	ImGui::DragFloat("t", &t);


	ImGui::End();
}
#endif // _DEBUG


void Grass::BehaviorInitialize()
{
	if (behaviortRquest_)
	{
		// 振る舞いを変更する
		behavior_ = behaviortRquest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_)
		{
		case BehaviorGrass::Root:
		default:
			BehaviorRootInit();
			break;
		case BehaviorGrass::Growth:
			BehaviorGrowthInit();
			break;
		case BehaviorGrass::Repop:
			BehaviorRepopInit();
			break;
		case BehaviorGrass::Delete:
			BehaviorDeleteInit();
			break;
		}
		// 振る舞いリクエストをリセット
		behaviortRquest_ = std::nullopt;
	}
}

void Grass::BehaviorUpdate()
{
	switch (behavior_)
	{
	case BehaviorGrass::Root:
	default:
		BehaviorRootUpdate();
		break;
	case BehaviorGrass::Growth:
		BehaviorGrowthUpdate();
		break;
	case BehaviorGrass::Repop:
		BehaviorRepopUpdate();
		break;
	case BehaviorGrass::Delete:
		BehaviorDeleteUpdate();
		break;
	}
}

void Grass::BehaviorRootInit()
{
}

void Grass::BehaviorRootUpdate()
{
}

void Grass::BehaviorGrowthInit()
{
	growthTimer_ = kGrowthTime_;
}

void Grass::BehaviorGrowthUpdate()
{
	if (0 < growthTimer_)
	{
		growthTimer_ -= deltaTime_;
		float t = 1.0f - growthTimer_ / kGrowthTime_;

		worldTransform_.scale_ = Lerp(defaultScale_, growthScale_, t);
	}
	else
	{
		isLarge_ = true;
		behaviortRquest_ = BehaviorGrass::Root;
	}
}

void Grass::BehaviorRepopInit()
{
	worldTransform_.scale_ = defaultScale_;
}

void Grass::BehaviorRepopUpdate()
{
	behaviortRquest_ = BehaviorGrass::Root;
}

void Grass::BehaviorDeleteInit()
{
}

void Grass::BehaviorDeleteUpdate()
{
}
