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
	bodyTransform_.SetParent(&rootTransform_); // ← これを追加！


	neckTransform_.useAnchorPoint_ = true;
	neckTransform_.SetAnchorPoint({ 0.0, -1.0f,0.0f });

	rootTransform_.translation_ = { 2.0f,2.0f,0.0f };
	worldTransform_.translation_ = { 2.0f,2.0f,0.0f };

	input_ = Input::GetInstance();

	isFinishedReadBook_ = false;

	InitCollision();
	InitJson();


	uiA_ = std::make_unique<Sprite>();
	uiA_->Initialize("Resources/Textures/Option/A.png");
	uiA_->SetSize({ 50.0f, 50.0f });
	uiA_->SetAnchorPoint({ 0.5f, 0.5f });
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

	if (input_->IsPadPressed(0, GamePadButton::B)) {
		neckTransform_.scale_.y += 0.1f;
	}


	if (isFinishedReadBook_) {
		if (Input::GetInstance()->PushKey(DIK_SPACE) || Input::GetInstance()->IsPadPressed(0, GamePadButton::A) || Input::GetInstance()->TriggerKey(DIK_E)) {
			isScaling_ = true;

		}

		if (isScaling_) {
			neckTransform_.scale_.y += 0.1f;
		}

	}
	float stretchY = neckTransform_.scale_.y;
	worldTransform_.translation_ = neckPos + Vector3(0.0f, stretchY + 1.0f, 0.0f);
	worldTransform_.UpdateMatrix();
	// スケールによる伸びを考慮して頭を移動

	auto result = mpCollision_.CheckHitAtPosition(worldTransform_.translation_);
	if (result && result->blockType == MapChipType::kCeiling) {
		auto& info = *result;

		mpCollision_.GetMapChipField()->SetMapChipTypeByIndex(info.xIndex, info.yIndex, MapChipType::kBlank);
		GenerateCeilingBreakParticle(worldTransform_.translation_);
	}
	for (auto it = breakParticles_.begin(); it != breakParticles_.end(); ) {
		auto& p = *it;
		float dt = GameTime::GetDeltaTime();
		p.lifetime -= dt;

		if (p.lifetime <= 0.0f) {
			it = breakParticles_.erase(it);
			continue;
		}

		Vector3 gravity = { 0.0f, -9.8f, 0.0f };

		if (!p.hasSwitched) {
			// 打ち上げ中（重力なし）
			p.wt->translation_ += p.velocity * dt;

			if (p.lifetime <= (3.0f - p.switchTime)) {
				p.velocity = MakeExplosionVelocity(0.3f, 1.6f); // 放射切替
				p.hasSwitched = true;
			}
		} else {
			// 放射中（重力あり）
			p.velocity += gravity * dt;
			p.wt->translation_ += p.velocity * dt;
		}

		p.wt->rotation_ += p.rotationVelocity * dt;
		p.wt->UpdateMatrix();

		++it;
	}






	obbCollider_->Update();
	neck_->uvScale = { neckTransform_.scale_.x, neckTransform_.scale_.y };
	neck_->uvTranslate.y = -(neckTransform_.scale_.y - 1.0f) * 0.6855f;

	UpdateSprite();
}

void TitlePlayer::UpdateMatrix()
{
	rootTransform_.UpdateMatrix();
	//worldTransform_.UpdateMatrix();
	neckTransform_.UpdateMatrix();  // 首が先！
	bodyTransform_.UpdateMatrix();  // 体もrootの子



}

void TitlePlayer::UpdateSprite()
{

	// ぷりぷり処理（sin波）
	const float pulseSpeed = 6.0f; // 速度（数値が大きいほど速く変動）
	const float pulseScale = 0.1f; // 変動幅
	float time = GameTime::GetTotalTime(); // 時間取得（秒）
	float scale = 1.0f + std::sin(time * pulseSpeed) * pulseScale;

	uiA_->SetSize({ 50.0f * scale, 50.0f * scale }); // ぷりぷりスケール反映


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

void TitlePlayer::Reset()
{

}

void TitlePlayer::Move()
{
	Vector3 oldDirection = moveDirection_;


	if (!isScaling_) {
		if (input_->PushKey(DIK_LEFT) || input_->PushKey(DIK_A)) {
			moveDirection_ = { -1.0f, 0.0f, 0.0f };
		}
		else if (input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_D)) {
			moveDirection_ = { 1.0f, 0.0f, 0.0f };
		}
		else {
			moveDirection_ = { 0.0f, 0.0f, 0.0f };
		}
	}
	else {
		moveDirection_ = { 0.0f, 0.0f, 0.0f };
	}

	Vector2 padInput = input_->GetLeftStickInput(0);
	moveDirection_.x += padInput.x;

	float dir = Length(moveDirection_);
	if (dir > 1.0f) {
		moveDirection_ = Normalize(moveDirection_);
	}

	deltaTime_ = GameTime::GetDeltaTime();
	velocity_ = moveDirection_ * defaultSpeed_ * deltaTime_;
	Vector3 newPos = rootTransform_.translation_ + velocity_;

	// ▼ X軸を前方とした向き計算と補間
	if (LengthSquared(moveDirection_) > 0.0001f) {
		float targetAngle = std::atan2(moveDirection_.z, moveDirection_.x); // X軸前提
		targetRotationY_ = -targetAngle; // 左手座標系ならマイナスをつける

		// 滑らかに回転補間
		rootTransform_.rotation_.y += (targetRotationY_ - rootTransform_.rotation_.y) * (1.0f - std::exp(-10.0f * deltaTime_));
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
	const int kParticleCount = 15;
	for (int i = 0; i < kParticleCount; ++i) {
		auto obj = std::make_unique<Object3d>();
		obj->Initialize();
		obj->SetModel("cube.obj");

		auto wt = std::make_unique<WorldTransform>();
		wt->Initialize();
		wt->translation_ = position;
		wt->scale_ = { 0.7f, 0.7f, 0.7f };

		// 少しばらつきのある初期打ち上げ方向
		Vector3 offset = {
			((rand() % 100) / 100.0f - 0.5f) * 0.5f,  // X方向に±0.25
			1.0f,  // Y方向固定
			((rand() % 100) / 100.0f - 0.5f) * 0.5f   // Z方向に±0.25
		};
		Vector3 initialVelocity = Normalize(offset) * 8.0f + Vector3{ 0, 8.0f, 0 }; // 少し中心寄せつつ打ち上げ

		// ランダムな回転速度
		Vector3 rotationVel = {
			((rand() % 100) / 100.0f - 0.5f) * 1.5f,
			((rand() % 100) / 100.0f - 0.5f) * 1.5f,
			((rand() % 100) / 100.0f - 0.5f) * 1.5f,
		};

		breakParticles_.push_back({
			std::move(obj),
			std::move(wt),
			initialVelocity,
			3.0f,
			0.15f,
			false,
			rotationVel
			});
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
