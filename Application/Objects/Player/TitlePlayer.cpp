#include "TitlePlayer.h"

TitlePlayer::~TitlePlayer()
{
}

void TitlePlayer::Initialize(Camera* camera)
{
	BaseObject::camera_ = camera;

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitCube.obj");

	worldTransform_.Initialize();

	worldTransform_.translation_ = { 2.0f,2.0f,0.0f };

	input_ = Input::GetInstance();



	InitJson();

}

void TitlePlayer::InitCollision()
{
}

void TitlePlayer::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("TitlePlayer", "Resources/JSON/");
	jsonManager_->SetCategory("Objects");
	jsonManager_->SetSubCategory("TitlePlayer");
	jsonManager_->Register("通常時の移動速度", &defaultSpeed_);




}

void TitlePlayer::Update()
{

	Move();


	UpdateMatrix();
}

void TitlePlayer::UpdateMatrix()
{
	worldTransform_.UpdateMatrix();
}

void TitlePlayer::Draw()
{
	obj_->Draw(BaseObject::camera_, worldTransform_);

}

void TitlePlayer::DrawCollision()
{
}

void TitlePlayer::MapChipOnCollision(const CollisionInfo& info)
{
}

void TitlePlayer::Reset()
{
}

void TitlePlayer::Move()
{
	if (input_->PushKey(DIK_UP) || input_->PushKey(DIK_W)) {
		moveDirection_ = { 0.0f,1.0f,0.0f };
	} else if (input_->PushKey(DIK_DOWN) || input_->PushKey(DIK_S)) {
		moveDirection_ = { 0.0f,-1.0f,0.0f };
	} else if (input_->PushKey(DIK_LEFT) || input_->PushKey(DIK_A)) {
		moveDirection_ = { -1.0f,0.0f,0.0f };
	} else if (input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_D)) {
		moveDirection_ = { 1.0f,0.0f,0.0f };
	} else {
		moveDirection_ = { 0.0f,0.0f,0.0f };
	}



	deltaTime_ = GameTime::GetDeltaTime();

	velocity_ = moveDirection_ * defaultSpeed_ * deltaTime_;

	Vector3 newPos = worldTransform_.translation_ + velocity_;

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

	worldTransform_.translation_ = newPos;


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
