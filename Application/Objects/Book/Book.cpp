#include "Book.h"
#include "Collision/OBB/OBBCollider.h"
#include "Collision/Core/ColliderFactory.h"

#include "Easing.h"

Book::~Book()
{
	obbCollider_->~OBBCollider();
}

void Book::Initialize(Camera* camera)
{
	BaseObject::camera_ = camera;

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("book.obj");
	worldTransform_.Initialize();

	worldTransform_.translation_ = { 25.0f,2.0f,0.0f };

	input_ = Input::GetInstance();

	isDrawUI_ = true;

	InitCollision();
	InitJson();


	InitializeSprite();
	//InitEvent();

}

void Book::InitializeSprite()
{
	uiBook_[0] = std::make_unique<Sprite>();
	uiBook_[0]->Initialize("Resources/Textures/Option/yomu.png");
	uiBook_[0]->SetSize({ 150.0f, 100.0f });
	uiBook_[0]->SetAnchorPoint({ 0.5f, 0.5f });


	uiReadBook_[0] = std::make_unique<Sprite>();
	uiReadBook_[0]->Initialize("Resources/Textures/Option/book.png");
	uiReadBook_[0]->AdjustTaxtureSize();
	uiReadBook_[0]->SetAnchorPoint({ 0.5f, 0.5f });

	uiReadBook_[1] = std::make_unique<Sprite>();
	uiReadBook_[1]->Initialize("Resources/images/white.png");
	uiReadBook_[1]->SetSize({ 1280.0f, 720.0f });
	uiReadBook_[1]->SetColor({ 0.0f, 0.0f, 0.0f, 0.5f });

	uiBook_[1] = std::make_unique<Sprite>();
	uiBook_[1]->Initialize("Resources/Textures/Option/close.png");
	uiBook_[1]->SetSize({ 200.0f, 150.0f });
}

void Book::InitCollision()
{
	obbCollider_ = ColliderFactory::Create<OBBCollider>(
		this,
		&worldTransform_,
		camera_,
		static_cast<uint32_t>(CollisionTypeIdDef::kBook)
	);
}

void Book::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("Book", "Resources/JSON/");
	jsonManager_->SetCategory("Objects");
	jsonManager_->SetSubCategory("Book");
	jsonManager_->Register("Translate", &worldTransform_.translation_);
	jsonManager_->Register("Rotate", &worldTransform_.rotation_);
	jsonManager_->Register("Offset", &offset_);
	jsonManager_->Register("OffsetReadUI", &offsetReadUI_);
	jsonManager_->Register("offsetUI_", &offsetUI_);

	jsonCollider_ = std::make_unique<JsonManager>("BookCollider", "Resources/JSON/");
	obbCollider_->InitJson(jsonCollider_.get());
}

void Book::InitEvent()
{
	uiReadScaleT_ = 0.0f;
	uiReadScaleTarget_ = 0.0f;
	isScaling_ = false;
	uiReadScaleState_ = UIReadScaleState::Growing;
	isDrawReadUI_ = true;
	isDrawBack_ = true;
}

void Book::Update()
{
	float time = GameTime::GetTotalTime();
	float scale = 1.0f + std::sin(time * 1.5f) * 0.1f; // ±3%くらい
	worldTransform_.scale_ = { scale, scale, scale };
	UpdateMatrix();
	obbCollider_->Update();
	UpdateSprite();
	UpdateReadBook();
}

void Book::UpdateSprite()
{
	const float easeSpeed = 8.0f;  // 補間速度（大きいほど速い）
	float delta = GameTime::GetDeltaTime();

	// 線形補間ベースで補間（時間依存に）
	uiScaleCurrent_ += (uiScaleTarget_ - uiScaleCurrent_) * (1.0f - std::exp(-easeSpeed * delta));


	// サイズ反映
	Vector2 scaledSize = {
		uiSizeBase_.x * uiScaleCurrent_,
		uiSizeBase_.y * uiScaleCurrent_
	};
	uiBook_[0]->SetSize(scaledSize);

	// スケールが小さくなりすぎたら描画停止
	if (uiScaleCurrent_ < 0.01f) {

		return;
	}

	// 座標変換
	Vector3 newPos = worldTransform_.translation_;
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1);
	Matrix4x4 matViewProjectionViewport = Multiply(camera_->GetViewMatrix(), Multiply(camera_->GetProjectionMatrix(), matViewport));
	newPos = Transform(newPos, matViewProjectionViewport);
	newPos += offset_;
	uiBook_[0]->SetPosition(newPos);
	uiBook_[0]->Update();

	uiBook_[1]->SetPosition(offsetUI_);
	uiBook_[1]->Update();
}

void Book::UpdateMatrix()
{
	worldTransform_.UpdateMatrix();
}

void Book::UpdateReadBook()
{
	const float speed = 10.0f; // イージング速度
	float delta = GameTime::GameTime::GetUnscaledDeltaTime();
	
	if (Input::GetInstance()->PushKey(DIK_SPACE) || Input::GetInstance()->IsPadTriggered(0, GamePadButton::A) || Input::GetInstance()->TriggerKey(DIK_E))
	{
		if (isScaling_) {
			uiReadScaleState_ = UIReadScaleState::Shrinking;
			isDrawBack_ = false;
			
		}
	}

	// 補間進行
	switch (uiReadScaleState_) {
	case UIReadScaleState::Growing: {
		uiReadScaleCurrent_ += (1.0f - uiReadScaleCurrent_) * (1.0f - std::exp(-speed * delta));
		if (std::abs(uiReadScaleCurrent_ - 1.0f) < 0.01f) {
			uiReadScaleCurrent_ = 1.0f;
			uiReadScaleState_ = UIReadScaleState::None;
			isScaling_ = true;
		}
		break;
	}
	case UIReadScaleState::Shrinking: {
		if (isCloseIconVisible_) {
			isCloseIconVisible_ = false;
		}
		uiReadScaleCurrent_ += (0.0f - uiReadScaleCurrent_) * (1.0f - std::exp(-speed * delta));
		if (uiReadScaleCurrent_ < 0.01f) {
			uiReadScaleCurrent_ = 0.0f;
			uiReadScaleState_ = UIReadScaleState::None;
			isDrawReadUI_ = false;  // 非表示


			hasReadBookShown_ = false;
			readBookDelayTimer_ = 0.0f;

			if (OffBookTrigger_) {
				OffBookTrigger_();
			}
		}
		break;
	}
	case UIReadScaleState::None: {
		
		break;
	}
	default:
		break;
	}

	// スケール反映
	Vector2 scaledSize = {
		uiSizeReadBase_.x * uiReadScaleCurrent_,
		uiSizeReadBase_.y * uiReadScaleCurrent_
	};
	uiReadBook_[0]->SetSize(scaledSize);

	// 読書UIが出てからの1秒タイマー
	if (hasReadBookShown_ && !isCloseIconVisible_ && uiReadScaleState_ != UIReadScaleState::Shrinking) {
		readBookDelayTimer_ += GameTime::GetUnscaledDeltaTime();
		if (readBookDelayTimer_ >= 1.0f) {
			isCloseIconVisible_ = true;
		}
	}



	// 表示中のみ位置更新
	if (isDrawReadUI_) {
		uiReadBook_[0]->SetPosition(offsetReadUI_);
		uiReadBook_[0]->Update();
		uiReadBook_[1]->Update();
	}
}


void Book::Draw()
{
	obj_->Draw(BaseObject::camera_, worldTransform_);

}

void Book::DrawSprite()
{
	if (isDrawUI_) {
		if (uiScaleCurrent_ > 0.01f) {
			uiBook_[0]->Draw();
		}
	}




	if (isDrawBack_)
		uiReadBook_[1]->Draw();


	if (isCloseIconVisible_) {
		uiBook_[1]->Draw();
	}

	if (isDrawReadUI_) {
		uiReadBook_[0]->Draw();

		if (!hasReadBookShown_) {
			hasReadBookShown_ = true;
			readBookDelayTimer_ = 0.0f;
		}
	}
}

void Book::DrawCollision()
{
	obbCollider_->Draw();
}

void Book::ReadEvent()
{

	UpdateReadBook();

}

void Book::UpdateUI()
{
}

void Book::MapChipOnCollision(const CollisionInfo& info)
{
}




void Book::OnEnterCollision(BaseCollider* self, BaseCollider* other)
{
	if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer))
	{
		uiScaleTarget_ = 1.0f;
		isColliding_ = true;
	}
}

void Book::OnCollision(BaseCollider* self, BaseCollider* other)
{
	if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer)) {
		
		if (GameTime::IsPause() == false) {
			if (Input::GetInstance()->PushKey(DIK_SPACE) || Input::GetInstance()->IsPadPressed(0, GamePadButton::A) || Input::GetInstance()->TriggerKey(DIK_E)) {
				if (OnBookTrigger_)
				{
					OnBookTrigger_();
				}

			}
		}
	}


}

void Book::OnExitCollision(BaseCollider* self, BaseCollider* other)
{
	if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer))
	{
		uiScaleTarget_ = 0.0f;
		isColliding_ = false;

	}
}

void Book::OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir)
{
}
