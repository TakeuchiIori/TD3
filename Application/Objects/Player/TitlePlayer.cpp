#include "TitlePlayer.h"
#include "Collision/OBB/OBBCollider.h"
#include "Collision/Core/ColliderFactory.h"


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
	neck_->SetModel("neck.obj");

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



	if (isFinishedReadBook_) {
		if (Input::GetInstance()->IsPadPressed(0, GamePadButton::A)) {
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



	obbCollider_->Update();

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
	if (isFinishedReadBook_) {
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
	if (input_->PushKey(DIK_LEFT) || input_->PushKey(DIK_A)) {
		moveDirection_ = { -1.0f,0.0f,0.0f };
	} else if (input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_D)) {
		moveDirection_ = { 1.0f,0.0f,0.0f };
	} else {
		moveDirection_ = { 0.0f,0.0f,0.0f };
	}

	// パッド入力の追加（左スティック）
	Vector2 padInput = input_->GetLeftStickInput(0); // 0番コントローラー前提
	moveDirection_.x += padInput.x;

	float dir = Length(moveDirection_);
	
	// 正規化（斜め移動防止）
	if (dir > 1.0f) {
		moveDirection_ = Normalize(moveDirection_);
	}

	deltaTime_ = GameTime::GetDeltaTime();

	velocity_ = moveDirection_ * defaultSpeed_ * deltaTime_;

	Vector3 newPos = rootTransform_.translation_ + velocity_;


	mpCollision_.DetectAndResolveCollision(
		colliderRect_,							// 衝突判定用矩形
		newPos,									// 更新される位置（衝突解決後）
		velocity_,								// 更新される速度
		MapChipCollision::CollisionFlag::All,	// すべての方向をチェック
		[this](const CollisionInfo& info) {
			// 衝突時の処理（例：特殊ブロック対応）
			MapChipOnCollision(info);
		}
	);

	//rootTransform_.translation_ = worldTransform_.translation_;
	rootTransform_.translation_ = newPos;
	
}

void TitlePlayer::OnEnterCollision(BaseCollider* self, BaseCollider* other)
{
}

void TitlePlayer::OnCollision(BaseCollider* self, BaseCollider* other)
{
	if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kBook))
	{
		isScaling_ = false;
	}
}

void TitlePlayer::OnExitCollision(BaseCollider* self, BaseCollider* other)
{
}

void TitlePlayer::OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir)
{
}
