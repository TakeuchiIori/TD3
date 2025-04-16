#include "Grass.h"

//



#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

int Grass::count_ = 0;

Grass::~Grass()
{
	--count_;
	aabbCollider_->~AABBCollider();
	aabbGrowthCollider_->~AABBCollider();
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

	growthAreaWT_.Initialize();
	growthAreaWT_.scale_ = { 2.5f,2.5f,2.5f };
	growthAreaWT_.translation_ = worldTransform_.translation_;
	growthAreaWT_.UpdateMatrix();

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitCube.obj");
	obj_->SetMaterialColor({ 0.3f,1.0f,0.3f,1.0f });

	// 枝の初期化
	branch_ = std::make_unique<Branch>();
	branch_->SetGrassWorldTransform(&worldTransform_);
	branch_->Initialize(camera_);

	InitCollision();
	InitJson();

	particleEmitter_ = std::make_unique<ParticleEmitter>("GrowthParticle", worldTransform_.translation_, 5);
}

void Grass::InitCollision()
{
	aabbCollider_ = ColliderFactory::Create<AABBCollider>(
		this,
		&worldTransform_,
		camera_,
		static_cast<uint32_t>(CollisionTypeIdDef::kGrass)
	);
	aabbGrowthCollider_ = ColliderFactory::Create<AABBCollider>(
		this,
		&growthAreaWT_,
		camera_,
		static_cast<uint32_t>(CollisionTypeIdDef::kGrowthArea)
	);
}

void Grass::InitJson()
{
	/*jsonManager_ = std::make_unique<JsonManager>("grassObj", "Resources/JSON/");

	jsonCollider_ = std::make_unique<JsonManager>("grassCollider", "Resources/JSON/");
	aabbCollider_->InitJson(jsonCollider_.get());*/
}

void Grass::Update()
{
	branch_->SetPlayerBoost(player_->IsBoost());
	if (branch_->IsDelete())
	{
		behaviortRquest_ = BehaviorGrass::Delete;
	}
	BehaviorInitialize();
	BehaviorUpdate();
	worldTransform_.UpdateMatrix();
	branch_->Update();
	growthAreaWT_.translation_ = worldTransform_.translation_;
	growthAreaWT_.UpdateMatrix();
	aabbCollider_->Update();
	aabbGrowthCollider_->Update();


#ifdef _DEBUG
	DebugGrass();
#endif // _DEBUG
}

void Grass::Draw()
{
	obj_->Draw(BaseObject::camera_, worldTransform_);
	branch_->Draw();
}

void Grass::DrawCollision()
{
	aabbCollider_->Draw();
	aabbGrowthCollider_->Draw();
	branch_->DrawCollision();
}

void Grass::OnEnterCollision(BaseCollider* self, BaseCollider* other)
{
	if(self->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kGrass))
	{
		if (player_->behavior_ != BehaviorPlayer::Return)
		{
			if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)) // プレイヤーなら
			{
				worldTransform_.scale_ = { 0.0f,0.0f,0.0f };
				aabbCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kNone));
				enter++;
				behaviortRquest_ = BehaviorGrass::Eaten;
			}
		}
	}
}

void Grass::OnCollision(BaseCollider* self, BaseCollider* other)
{
	if(!isLarge_)
	{
		if (self->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kGrowthArea))
		{
			if (behavior_ != BehaviorGrass::Eaten)
			{
				if (player_->behavior_ != BehaviorPlayer::Return)
				{
					if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)) // プレイヤーなら
					{
						if (input_->TriggerKey(DIK_Q) || input_->IsPadTriggered(0, GamePadButton::B))
						{
							behaviortRquest_ = BehaviorGrass::Growth;
							particleEmitter_->FollowEmit("GrowthParticle", worldTransform_.translation_);
						}
					}
				}
			}
		}
	}
}

void Grass::OnExitCollision(BaseCollider* self, BaseCollider* other)
{
}

void Grass::OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir)
{
}


#ifdef _DEBUG
void Grass::DebugGrass()
{
	float t = 1.0f - (growthTimer_ / kGrowthTime_);
	ImGui::Begin("DebugGrass");
	ImGui::DragFloat("t", &t);
	ImGui::Text("%d", enter);

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
		case BehaviorGrass::Eaten:
			BehaviorEatenInit();
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
	case BehaviorGrass::Eaten:
		BehaviorEatenUpdate();
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
	aabbCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kGrass));
}

void Grass::BehaviorRootUpdate()
{
}

void Grass::BehaviorEatenInit()
{
}

void Grass::BehaviorEatenUpdate()
{
}

void Grass::BehaviorGrowthInit()
{
	growthWait_ = true;
	growthTimer_ = kGrowthTime_;
}

void Grass::BehaviorGrowthUpdate()
{
	if(!growthWait_)
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
	aabbCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kNone));
}

void Grass::BehaviorDeleteUpdate()
{
}

void Grass::SetPos(Vector3 pos)
{
	worldTransform_.translation_ = pos;
	branch_->SetPos(pos);
	if (worldTransform_.translation_.x <= centerX_)
	{
		// 左にはやす
		branch_->SetLeft();
	}
	else
	{
		// 右にはやす
		branch_->SetRight();
	}
}
