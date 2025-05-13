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
	worldTransform_.scale_ = { 2.0f,2.0f,2.0f };

	//
	worldTransform_.translation_ = { 8.0f,5.0f,6.0f };
	worldTransform_.UpdateMatrix();

	growthAreaWT_.Initialize();
	growthAreaWT_.scale_ = { growthAreaScaleF_,growthAreaScaleF_,growthAreaScaleF_ };
	growthAreaWT_.translation_ = worldTransform_.translation_;
	growthAreaWT_.UpdateMatrix();

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("leafRed.obj");
	obj_->SetMaterialColor(defaultColor_);

	// 枝の初期化
	branch_ = std::make_unique<Branch>();
	branch_->SetGrassWorldTransform(&worldTransform_);
	branch_->Initialize(camera_);
	branch_->SetParentGrass(this);

	InitCollision();
	///InitJson();

	particleEmitter_ = std::make_unique<ParticleEmitter>("GrowthParticle", worldTransform_.translation_, 20);
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
	jsonCollider_ = std::make_unique<JsonManager>("grassCollider", "Resources/JSON/");
	aabbCollider_->InitJson(jsonCollider_.get());
}

void Grass::Update()
{
	branch_->SetPlayerBoost(player_->IsBoost());
	BehaviorInitialize();
	BehaviorUpdate();
	worldTransform_.UpdateMatrix();
	branch_->Update();
	growthAreaWT_.translation_ = worldTransform_.translation_;
	growthAreaWT_.UpdateMatrix();
	aabbCollider_->Update();
	aabbGrowthCollider_->Update();



	UpdateLeaf();

#ifdef _DEBUG
	DebugGrass();
#endif // _DEBUG
}

void Grass::UpdateLeaf()
{
	for (size_t i = 0; i < fallingLeaves_.size(); ++i) {
		auto& leaf = fallingLeaves_[i];

		// Swing: 左右ゆらゆら
		float swingX = std::sin(leaf.swingPhase + leaf.lifetime * 4.0f) * 0.02f;

		// Float: 上下ゆらゆら（微小）
		float floatY = std::sin(leaf.floatPhase + leaf.lifetime * 6.0f) * 0.003f;

		// 落下速度（弱めの重力）
		leaf.velocity.y -= 0.0006f;

		// 位置更新
		leaf.wt->translation_ += leaf.velocity;
		leaf.wt->translation_.x += swingX;
		leaf.wt->translation_.y += floatY;

		// 回転も少し変化
		leaf.wt->rotation_ += leaf.angularVelocity;

		// アップデート
		leaf.wt->UpdateMatrix();

		// 透明度フェード
		leaf.alpha -= deltaTime_ / leaf.lifetime;
		leaf.obj->SetAlpha(leaf.alpha);

		// 削除処理
		if (leaf.alpha <= 0.0f) {
			fallingLeaves_.erase(fallingLeaves_.begin() + i);
			--i;
		}
	}


}

void Grass::Draw()
{
	obj_->Draw(BaseObject::camera_, worldTransform_);
	branch_->Draw();
	for (auto& leaf : fallingLeaves_) {
		leaf.obj->Draw(camera_, *leaf.wt);
	}


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
		if (behavior_ == BehaviorGrass::Root)
		{
			if (player_->behavior_ != BehaviorPlayer::Return)
			{
				if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)) // プレイヤーなら
				{
					DropLeaves(10);
					//worldTransform_.scale_ = { 0.0f,0.0f,0.0f };
					aabbCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kNone));
					enter++;
					behaviortRquest_ = BehaviorGrass::Eaten;
				}
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
			if (behavior_ != BehaviorGrass::Eaten && behavior_ != BehaviorGrass::Growth)
			{
				if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)) // プレイヤーなら
				{
					if (player_->behavior_ != BehaviorPlayer::Return)
					{
						if (input_->TriggerKey(DIK_Q) || input_->IsPadTriggered(0, GamePadButton::B))
						{
							if (worldTransform_.scale_.x != 0.0f) {
								obj_->SetMaterialColor(growthColor_);
								behaviortRquest_ = BehaviorGrass::Growth;
								particleEmitter_->FollowEmit(worldTransform_.translation_);
							}
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
	growthAreaWT_.scale_ = { growthAreaScaleF_,growthAreaScaleF_,growthAreaScaleF_ };
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
		case BehaviorGrass::Falling:
			BehaviorFallingInit();
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
	case BehaviorGrass::Falling:
		BehaviorFallingUpdate();
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
	eatenTimer_ = kEatenTime_;
	eatenStartScale_ = worldTransform_.scale_;
	aabbCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kNone));
	aabbGrowthCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kNone));
}

void Grass::BehaviorEatenUpdate()
{
	if (0 < eatenTimer_) {
		eatenTimer_ -= deltaTime_;
		float t = 1.0f - eatenTimer_ / kEatenTime_;

		// スケールを0に向かって補間
		worldTransform_.scale_ = Lerp(eatenStartScale_, { 0.0f, 0.0f, 0.0f }, t);
	} else {
		// 補間完了後はRootに戻す（またはそのまま消えるならDeleteでもOK）
		worldTransform_.scale_ = { 0.0f, 0.0f, 0.0f };
		//behaviortRquest_ = BehaviorGrass::Root;
	}
}

void Grass::BehaviorGrowthInit()
{
	growthWait_ = false;
	growthTimer_ = kGrowthTime_;
	aabbCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kNone));
	aabbGrowthCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kNone));
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
			if(player_->behavior_ == BehaviorPlayer::Return) behaviortRquest_ = BehaviorGrass::Root;
		}
	}
}

void Grass::BehaviorRepopInit()
{
	worldTransform_.scale_ = { 0.0f, 0.0f, 0.0f }; // ここもスケール初期化しといた方が無難
	repopWait_ = false;
	repopTimer_ = kRepopTime_;
	aabbCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kNone));
}

void Grass::BehaviorRepopUpdate()
{
	if (repopWait_) {
		repopWait_ = false; // ← ここで最初の1回だけ false にする
		return;
	}
	if (0 < repopTimer_)
	{
		repopTimer_ -= deltaTime_;
		float t = 1.0f - repopTimer_ / kRepopTime_;
		worldTransform_.scale_ = Lerp(Vector3{ 0,0,0 }, defaultScale_, t);
	} else
	{
		behaviortRquest_ = BehaviorGrass::Root;
	}
}

void Grass::BehaviorDeleteInit()
{
	aabbCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kNone));
}

void Grass::BehaviorDeleteUpdate()
{
}

void Grass::BehaviorFallingInit() {
	fallVelocity_ = { 0.0f, -0.2f, 0.0f }; // 落下速度
	fallTimer_ = kFallTime_;              // タイマー初期化
	aabbCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kNone));        // タイマー初期化
}

void Grass::BehaviorFallingUpdate() {
	worldTransform_.translation_ += fallVelocity_;
	//SetPos(worldTransform_.translation_);
	worldTransform_.UpdateMatrix();

	// タイマー減算
	if (fallTimer_ > 0.0f) {
		obj_->SetAlpha(fallTimer_ / kFallTime_);
		fallTimer_ -= deltaTime_;
	}
	else {
		behaviortRquest_ = BehaviorGrass::Delete;
	}
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

/// <summary>
/// 葉っぱを落とす
/// </summary>
void Grass::DropLeaves(int count) {
	for (int i = 0; i < count; ++i) {
		FallingLeaf leaf;
		leaf.obj = std::make_unique<Object3d>();
		leaf.obj->Initialize();
		leaf.obj->SetModel("leaf.obj");

		leaf.wt = std::make_unique<WorldTransform>();
		leaf.wt->Initialize();
		leaf.wt->translation_ = worldTransform_.translation_ + Vector3{ 0.0f, 1.0f, 0.0f };
		leaf.wt->scale_ = { 1.5f, 1.5f, 1.5f };

		// 初期落下速度はほぼ0（ふわふわ）
		leaf.velocity = {
			((rand() % 100) / 100.0f - 0.5f) * 0.005f,
			-((rand() % 10) / 100.0f + 0.005f),
			((rand() % 100) / 100.0f - 0.5f) * 0.005f
		};

		leaf.angularVelocity = {
			((rand() % 100) / 100.0f - 0.5f) * 0.02f,
			((rand() % 100) / 100.0f - 0.5f) * 0.02f,
			((rand() % 100) / 100.0f - 0.5f) * 0.02f
		};

		leaf.swingPhase = ((rand() % 100) / 100.0f) * 6.28f;
		leaf.floatPhase = ((rand() % 100) / 100.0f) * 6.28f;

		fallingLeaves_.emplace_back(std::move(leaf));
	}
}


