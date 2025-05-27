#include "TitlePlayer.h"
#include "Collision/OBB/OBBCollider.h"
#include "Collision/Core/ColliderFactory.h"
#include <Debugger/Logger.h>


TitlePlayer::~TitlePlayer()
{
	obbCollider_->~OBBCollider();
}

void TitlePlayer::Initialize(Camera* camera)
{
	BaseObject::camera_ = camera;

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("head.obj");

	neck_ = std::make_unique<Object3d>();
	neck_->Initialize();
	neck_->SetModel("neck2.obj");

	body_ = std::make_unique<Object3d>();
	body_->Initialize();
	body_->SetModel("body.obj");


	rootTransform_.Initialize();
	worldTransform_.Initialize();
	neckTransform_.Initialize();
	bodyTransform_.Initialize();

	neckTransform_.SetParent(&rootTransform_);
	bodyTransform_.SetParent(&rootTransform_);


	neckTransform_.useAnchorPoint_ = true;
	neckTransform_.SetAnchorPoint({ 0.0, -1.0f,0.0f });
	neckTransform_.scale_.y = 0;

	rootTransform_.translation_ = { 2.0f,2.0f,0.0f };
	worldTransform_.translation_ = { 2.0f,2.0f,0.0f };

	input_ = Input::GetInstance();

	isFinishedReadBook_ = false;

	InitCollision();
	InitJson();


	uiA_ = std::make_unique<Sprite>();
	uiA_->Initialize("Resources/Textures/Option/controller.png");
	uiA_->SetSize({ 50.0f, 50.0f });
	uiA_->SetAnchorPoint({ 0.5f, 0.5f });

	isScaling_ = false;

}

void TitlePlayer::InitCollision()
{
	obbCollider_ = ColliderFactory::Create<OBBCollider>(
		this,
		&rootTransform_,
		camera_,
		static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)
	);
}

void TitlePlayer::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("TitlePlayer", "Resources/JSON/");
	jsonManager_->SetCategory("Objects");
	jsonManager_->SetSubCategory("TitlePlayer");
	jsonManager_->Register("通常時の移動速度", &defaultSpeed_);
	jsonManager_->Register("offset", &offsetUI_);

	jsonManager_->SetTreePrefix("頭");
	jsonManager_->Register("root", &rootTransform_.translation_);
	jsonManager_->Register("頭の位置", &worldTransform_.translation_);
	jsonManager_->Register("頭の回転", &worldTransform_.rotation_);

	jsonManager_->SetTreePrefix("首");
	jsonManager_->Register("首の位置", &neckTransform_.translation_);
	jsonManager_->Register("首の回転", &neckTransform_.rotation_);

	jsonManager_->SetTreePrefix("体");
	jsonManager_->Register("体の位置", &bodyTransform_.translation_);
	jsonManager_->Register("体の回転", &bodyTransform_.rotation_);

	jsonManager_->Register("上がる力", &UpPower_);

	jsonManager_->ClearTreePrefix();

	jsonManager_->Register("up_", &up_);

	jsonCollider_ = std::make_unique<JsonManager>("TitlePlayerCollider", "Resources/JSON/");
	obbCollider_->InitJson(jsonCollider_.get());

}

void TitlePlayer::Update()
{

	Move();


	UpdateMatrix();

	Matrix4x4 neckMat = neckTransform_.matWorld_;
	Vector3 neckPos = {
		neckMat.m[3][0],
		neckMat.m[3][1],
		neckMat.m[3][2]
	};

	if (isFinishedReadBook_) {
		// 左スティックの入力を取得
		Vector2 leftStick = Input::GetInstance()->GetLeftStickInput(0);

		// 左スティックが上方向（Y軸正の方向）に倒されているかチェック
		// 閾値は0.5f程度に設定（スティックの感度調整）
		if (Input::GetInstance()->PushKey(DIK_SPACE) ||
			Input::GetInstance()->TriggerKey(DIK_E) ||
			leftStick.y > up_) {
			isScaling_ = true;
		}

		if (isScaling_) {
			neckTransform_.scale_.y += up_;
		}

	}

	float stretchY = neckTransform_.scale_.y;
	worldTransform_.translation_ = neckPos + Vector3(0.0f, stretchY + 1.0f, 0.0f);
	worldTransform_.UpdateMatrix();
	MapChipOnCollision();

	UpdateParticle();

	UpdateSprite();

	Shake();

	obbCollider_->Update();
	neck_->uvScale = { neckTransform_.scale_.x, neckTransform_.scale_.y };
	neck_->uvTranslate.y = -(neckTransform_.scale_.y - 1.0f) * 0.6855f;


}

void TitlePlayer::UpdateMatrix()
{
	rootTransform_.UpdateMatrix();
	//worldTransform_.UpdateMatrix();
	neckTransform_.UpdateMatrix();
	bodyTransform_.UpdateMatrix();



}

void TitlePlayer::UpdateSprite()
{

	time_ += GameTime::GetDeltaTime();
	if (time_ >= 1.0f) {
		uiA_->ChangeTexture("Resources/Textures/Option/controller2.png");
		uiA_->SetSize({ 50.0f, 50.0f });
		uiA_->SetAnchorPoint({ 0.5f, 0.5f });
	}

	if (time_ >= 2.0f) {
		time_ = 0.0f;
		uiA_->ChangeTexture("Resources/Textures/Option/controller.png");
		uiA_->SetSize({ 50.0f, 50.0f });
		uiA_->SetAnchorPoint({ 0.5f, 0.5f });

	}




	Vector3 playerPos = rootTransform_.translation_;
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1);
	Matrix4x4 matViewProjectionViewport = Multiply(camera_->GetViewMatrix(), Multiply(camera_->GetProjectionMatrix(), matViewport));
	playerPos = Transform(playerPos, matViewProjectionViewport);
	playerPos += offsetUI_;
	uiA_->SetPosition(playerPos);
	uiA_->Update();
}

void TitlePlayer::Draw()
{
	for (auto& p : breakParticles_) {
		p.obj->Draw(BaseObject::camera_, *p.wt);
	}

	obj_->Draw(BaseObject::camera_, worldTransform_);
	neck_->Draw(BaseObject::camera_, neckTransform_);
	body_->Draw(BaseObject::camera_, bodyTransform_);



}

void TitlePlayer::DrawCollision()
{
	obbCollider_->Draw();
}

void TitlePlayer::DrawSprite()
{
	if (showUI_) {
		uiA_->Draw();
	}
}

void TitlePlayer::MapChipOnCollision(const CollisionInfo& info)
{
}

void TitlePlayer::MapChipOnCollision()
{
	float checkWidth = 2.0f;
	float checkHeight = 0.0f;
	// 中心と左右で3点チェック
	std::vector<Vector3> checkPositions = {
		worldTransform_.translation_,
		worldTransform_.translation_ + Vector3(checkWidth, checkHeight, 0.0f),
		worldTransform_.translation_ + Vector3(-checkWidth, -checkHeight, 0.0f)
	};

	for (const auto& pos : checkPositions) {
		auto result = mpCollision_.CheckHitAtPosition(pos);
		if (result && result->blockType == MapChipType::kCeiling) {
			const auto& info = *result;
			isShake = true;
			mpCollision_.GetMapChipField()->SetMapChipTypeByIndex(info.xIndex, info.yIndex, MapChipType::kBlank);
			GenerateCeilingBreakParticle(pos); // 当たった位置で生成
		}
	}


}

void TitlePlayer::Reset()
{

}
/// <summary>
/// Move関数（プレイヤーの移動と回転を処理）
/// </summary>
/// <summary>
/// Move（プレイヤーの移動処理＋自然な回転補間）
/// </summary>
void TitlePlayer::Move()
{
	Vector3 oldDirection = moveDirection_;

	deltaTime_ = GameTime::GetDeltaTime();

	// スケーリング中は移動無効
	if (isScaling_) {
		moveDirection_ = { 0.0f, 0.0f, 0.0f };
		velocity_ = { 0.0f, 0.0f, 0.0f };
		return;
	}

	// キーボード入力
	if (input_->PushKey(DIK_LEFT) || input_->PushKey(DIK_A)) {
		moveDirection_ = { -1.0f, 0.0f, 0.0f };
	} else if (input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_D)) {
		moveDirection_ = { 1.0f, 0.0f, 0.0f };
	} else {
		moveDirection_ = { 0.0f, 0.0f, 0.0f };
	}

	// パッド入力
	Vector2 padInput = input_->GetLeftStickInput(0);
	moveDirection_.x += padInput.x;

	float dir = Length(moveDirection_);
	if (dir > 1.0f) {
		moveDirection_ = Normalize(moveDirection_);
	}

	velocity_ = moveDirection_ * defaultSpeed_ * deltaTime_;
	Vector3 newPos = rootTransform_.translation_ + velocity_;
	// 回転処理を「方向が変わったときだけ」行う
	if (LengthSquared(moveDirection_) > 0.0001f && moveDirection_ != oldDirection) {
		float targetAngle = std::atan2(moveDirection_.z, moveDirection_.x);
		rootTransform_.rotation_.y = -targetAngle;
		worldTransform_.rotation_.y = rootTransform_.rotation_.y;
	}


	mpCollision_.DetectAndResolveCollision(
		colliderRect_,
		newPos,
		velocity_,
		MapChipCollision::CollisionFlag::All,
		[this](const CollisionInfo& info) {
			MapChipOnCollision(info);
		}
	);

	rootTransform_.translation_ = newPos;
}


void TitlePlayer::UpdateParticle()
{
	for (auto it = breakParticles_.begin(); it != breakParticles_.end();) {
		auto& p = *it;
		float dt = GameTime::GetDeltaTime();
		p.lifetime -= dt;

		if (p.lifetime <= 0.0f) {
			it = breakParticles_.erase(it);
			continue;
		}
		/// 重力処理
		Vector3 lightGravity = { 0.0f, -120.0f, 0.0f };
		Vector3 fullGravity = { 0.0f, -9.8f, 0.0f };

		if (!p.hasSwitched) {
			p.velocity += lightGravity * dt;
			p.wt->translation_ += p.velocity * dt;

			if (p.lifetime <= (5.0f - p.switchTime)) {
				p.hasSwitched = true;
			}
		} else {
			p.velocity += fullGravity * dt;
			p.wt->translation_ += p.velocity * dt;
		}

		p.wt->rotation_ += p.rotationVelocity * dt;
		p.wt->UpdateMatrix();

		++it;
	}

}

void TitlePlayer::OnEnterCollision(BaseCollider* self, BaseCollider* other)
{
}

void TitlePlayer::OnCollision(BaseCollider* self, BaseCollider* other)
{
	if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kBook))
	{
		//isScaling_ = false;
	}
}

void TitlePlayer::OnExitCollision(BaseCollider* self, BaseCollider* other)
{
}

void TitlePlayer::OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir)
{
}

/// <summary>
///  天井破壊モデルパーティクルを生成する
/// </summary>
void TitlePlayer::GenerateCeilingBreakParticle(const Vector3& position)
{
	const int kParticleCount = 40;
	for (int i = 0; i < kParticleCount; ++i) {
		auto obj = std::make_unique<Object3d>();
		obj->Initialize();
		obj->SetModel("cube.obj");
		obj->SetMaterialColor(Vector3{ 0.0f, 0.0f, 0.0f });

		auto wt = std::make_unique<WorldTransform>();
		wt->Initialize();
		float randomX = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
		wt->translation_ = position;
		wt->translation_.x += randomX;

		float scale = 0.1f + ((rand() % 100) / 100.0f) * 0.4f;
		wt->scale_ = { scale, scale, scale };

		Vector3 offset = {
			((rand() % 100) / 100.0f - 0.5f) * 0.5f,
			((rand() % 100) / 100.0f) * 0.5f + 0.8f,
			((rand() % 100) / 100.0f - 0.5f) * 0.5f
		};
		Vector3 initialVelocity = Normalize(offset) * UpPower_;

		// ランダムな回転速度
		Vector3 rotationVel = {
			((rand() % 100) / 100.0f - 0.5f) * 6.0f,
			((rand() % 100) / 100.0f - 0.5f) * 6.0f,
			((rand() % 100) / 100.0f - 0.5f) * 6.0f,
		};

		breakParticles_.push_back({
			std::move(obj),
			std::move(wt),
			initialVelocity,
			5.0f,
			0.15f,
			false,
			rotationVel
			});
	}
}

void TitlePlayer::Shake()
{
	if (isShake) {
		camera_->Shake(2.5f, Vector2{ -0.1f, -0.1f }, Vector2{ 0.1f,0.1f });
		isShake = false;
	}
}


Vector3 MakeExplosionVelocity(float minSpeed, float maxSpeed) {
	// 水平方向のランダム角度 [0, 2π]
	float theta = static_cast<float>(rand()) / RAND_MAX * 2.0f * 3.1415926f;

	// 垂直方向の角度 [0, π/2]（上方向だけ）
	float phi = static_cast<float>(rand()) / RAND_MAX * (3.1415926f / 2.0f);

	// 球面座標 → XYZ（XとZがしっかり出る）
	Vector3 dir = {
		std::sin(phi) * std::cos(theta), // X
		std::sin(phi),                   // Y（上方向）
		std::sin(phi) * std::sin(theta)  // Z
	};

	float speed = minSpeed + ((rand() % 100) / 100.0f) * (maxSpeed - minSpeed);

	return dir * speed;
}
