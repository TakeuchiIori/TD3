#include "TitlePlayer.h"
#include "Collision/OBB/OBBCollider.h"
#include "Collision/Core/ColliderFactory.h"


TitlePlayer::~TitlePlayer()
{
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


	worldTransform_.Initialize();
	neckTransform_.Initialize();
	bodyTransform_.Initialize();

	neckTransform_.SetParent(nullptr); // 首は独立
	bodyTransform_.SetParent(nullptr); // 体も独立
	//worldTransform_.SetParent(&neckTransform_); // 頭は首の上に乗る


	neckTransform_.useAnchorPoint_ = true;
	neckTransform_.SetAnchorPoint({ 0.0, -1.0f,0.0f });

	worldTransform_.translation_ = { 2.0f,2.0f,0.0f };

	input_ = Input::GetInstance();

	isFinishedReadBook_ = true;

	InitCollision();
	InitJson();

}

void TitlePlayer::InitCollision()
{
	obbCollider_ = ColliderFactory::Create<OBBCollider>(
		this,
		&worldTransform_,
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

	jsonManager_->SetTreePrefix("頭");
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

	if (isFinishedReadBook_) {
		if (Input::GetInstance()->IsPadPressed(0, GamePadButton::A)) {
			neckTransform_.scale_.y += 0.1f;

			Matrix4x4 neckMat = neckTransform_.matWorld_;
			Vector3 neckPos = {
				neckMat.m[3][0],
				neckMat.m[3][1],
				neckMat.m[3][2]
			};

			float stretchY = neckTransform_.scale_.y;
			worldTransform_.translation_ = neckPos + Vector3(0.0f, stretchY, 0.0f);
		}
	}

	// スケールによる伸びを考慮して頭を移動



	UpdateMatrix();
	obbCollider_->Update();
}

void TitlePlayer::UpdateMatrix()
{
	neckTransform_.UpdateMatrix();
	worldTransform_.UpdateMatrix(); // 首の後に更新（親子反映）
	bodyTransform_.UpdateMatrix();  // 体は独立

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

	Vector3 newPos = worldTransform_.translation_ + velocity_;
	Vector3 newNeckPos = neckTransform_.translation_ + velocity_;
	Vector3 newBodyPos = bodyTransform_.translation_ + velocity_;

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

	mpCollision_.DetectAndResolveCollision(
		colliderRect_,							// 衝突判定用矩形
		newNeckPos,									// 更新される位置（衝突解決後）
		velocity_,								// 更新される速度
		MapChipCollision::CollisionFlag::All,	// すべての方向をチェック
		[this](const CollisionInfo& info) {
			// 衝突時の処理（例：特殊ブロック対応）
			MapChipOnCollision(info);
		}
	);

	mpCollision_.DetectAndResolveCollision(
		colliderRect_,							// 衝突判定用矩形
		newBodyPos,									// 更新される位置（衝突解決後）
		velocity_,								// 更新される速度
		MapChipCollision::CollisionFlag::All,	// すべての方向をチェック
		[this](const CollisionInfo& info) {
			// 衝突時の処理（例：特殊ブロック対応）
			MapChipOnCollision(info);
		}
	);

	worldTransform_.translation_ = newPos;
	neckTransform_.translation_ = newNeckPos;
	bodyTransform_.translation_ = newBodyPos;

}

void TitlePlayer::OnEnterCollision(BaseCollider* self, BaseCollider* other)
{
}

void TitlePlayer::OnCollision(BaseCollider* self, BaseCollider* other)
{
}

void TitlePlayer::OnExitCollision(BaseCollider* self, BaseCollider* other)
{
}

void TitlePlayer::OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir)
{
}
