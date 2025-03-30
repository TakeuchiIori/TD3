#include "Grass.h"

//



#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

int Grass::count_ = 0;

void Grass::Initialize(Camera* camera)
{
	input_ = Input::GetInstance();

	camera_ = camera;

	// トランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = { 2.0f,2.0f,2.0f };

	//
	worldTransform_.translation_ = { 8.0f,2.0f,5.0f };

	// オブジェクトの初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitCube.obj");
	obj_->SetMaterialColor({ 0.3f,1.0f,0.3f,1.0f });
}

void Grass::Update()
{
	worldTransform_.UpdateMatrix();



#ifdef _DEBUG
	DebugGrass();
#endif // _DEBUG
}

void Grass::Draw()
{
	obj_->Draw(camera_, worldTransform_);
}


#ifdef _DEBUG
void Grass::DebugGrass()
{
	float t = 1.0f - (growthTimer_ / kGrowthTime_);
	ImGui::Begin("DebugGrass");
	ImGui::Text("Growth : G ");
	ImGui::DragFloat("t", &t);


	ImGui::End();

	if (input_->PushKey(DIK_G))
	{
		growthTimer_ = kGrowthTime_;
	}
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
}

void Grass::BehaviorRepopUpdate()
{
}

void Grass::BehaviorDeleteInit()
{
}

void Grass::BehaviorDeleteUpdate()
{
}
