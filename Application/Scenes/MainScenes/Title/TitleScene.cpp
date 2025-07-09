#include "TitleScene.h"
// Engine
#include "CoreScenes./Manager./SceneManager.h"
#include "Systems./Input./Input.h"
#include "Loaders./Texture./TextureManager.h"
#include "Particle./ParticleManager.h"
#include "Object3D/Object3dCommon.h"
#include "../Graphics/PipelineManager/SkinningManager.h"
#include "../Application/SystemsApp/AppAudio/AudioVolumeManager.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // DEBUG
#include "LightManager/LightManager.h"
#include "Sprite/SpriteCommon.h"
#include <Collision/Core/CollisionManager.h>
#include <Systems/GameTime/GameTIme.h>
#include <Loaders/Json/JsonManager.h>

#include "../../../SpriteApp/MenuOverlay.h"

/// <summary>
/// 初期化処理
/// </summary>
void TitleScene::Initialize()
{
	GameTime::Initailzie();
	MenuOverlay::GetInstance()->Initialize();
	// カメラの生成
	sceneCamera_ = cameraManager_.AddCamera();
	Object3dCommon::GetInstance()->SetDefaultCamera(sceneCamera_.get());
	ParticleManager::GetInstance()->SetCamera(sceneCamera_.get());
	CollisionManager::GetInstance()->Initialize();
	// 初期カメラモード設定
	cameraMode_ = CameraMode::DEFAULT;

	// 各カメラの初期化
	defaultCamera_.Initialize();
	followCamera_.Initialize();
	debugCamera_.Initialize();
	bookEventCamera_.Initialize();

	// オーディオファイルのロード（例: MP3）
	soundData = Audio::GetInstance()->LoadAudio(L"Resources/Audio/title.mp3");
	// オーディオの再生
	sourceVoice = Audio::GetInstance()->SoundPlayAudio(soundData, true);
	//Audio::GetInstance()->FadeInPlay(sourceVoice, 2.0f);
	AudioVolumeManager::GetInstance()->SetSourceToSubmix(sourceVoice, kBGM);
	// 音量の設定（0.0f ～ 1.0f）
	//Audio::GetInstance()->SetVolume(sourceVoice, 1.0f); // 80%の音量に設定



	mpInfo_ = std::make_unique<MapChipInfo>();
	mpInfo_->GetMapChipField()->SetNumBlockVertical(19);
	mpInfo_->Initialize();
	mpInfo_->SetCamera(sceneCamera_.get());
	mpInfo_->SetMapChip("Resources/images/MapChip_Title.csv");


	player_ = std::make_unique<TitlePlayer>(mpInfo_->GetMapChipField());
	player_->Initialize(sceneCamera_.get());
	bookEventCamera_.SetTarget(player_->GetWorldTransform());
	player_->SetMapChipInfo(mpInfo_.get());

	book_ = std::make_unique<Book>(mpInfo_->GetMapChipField());
	book_->Initialize(sceneCamera_.get());


	titleScreen_ = std::make_unique<TitleScreen>();
	titleScreen_->Initialize();
	titleScreen_->SetCamera(sceneCamera_.get());

	for (int i = 0; i < numClouds_; ++i) {
		auto cloud = std::make_unique<Cloud>();
		cloud->Initialize(sceneCamera_.get());
		clouds_.emplace_back(std::move(cloud));
	}

	// 本を読み始めたら
	book_->OnBookTrigger_ = [this]() {
		// 時間止める
		if (!isAlreadyRead_) {
			cameraMode_ = CameraMode::BOOK_EVENT; // 最初だけカメラ演出あり
			this->isBookTrigger_ = true;
		} else {
			book_->InitEvent(); // その場で読書開始
		}
		book_->SetIsDrawUI(false);
		GameTime::Pause();
		};

	// カメラが終点に着いたら
	bookEventCamera_.isFinishedMove_ = [this]() {
		this->isStartEvent_ = true;
		};

	// 本を読み終えてAボタンが押されたら
	book_->OffBookTrigger_ = [this]() {
		isAlreadyRead_ = true;
		player_->SetShowUI(true);
		cameraMode_ = CameraMode::DEFAULT;
		GameTime::Resume();
		};

	emitter_ = std::make_unique<ParticleEmitter>("TitleParticle", Vector3{ 0.0f,0.0f,0.0f }, 3);
	emitter_->Initialize("Title");
}

/// <summary>
/// 更新処理
/// </summary>
void TitleScene::Update()
{
	GameTime::Update();
	GameTime::ImGui();

	/*=================================================================

								イベントの処理

	=================================================================*/
	if (isStartEvent_) {

		book_->InitEvent();
		isStartEvent_ = false;
	}
	else if(player_->GetWorldTransform().translation_.y < 35.0f)
	{
		MenuOverlay::GetInstance()->ShowHide();
	}
	if (MenuOverlay::GetInstance()->IsVisible())
	{
		MenuOverlay::GetInstance()->Update();
	}
	else
	{
		if (player_->GetWorldTransform().translation_.y > 35.0f) {
			SceneManager::GetInstance()->SetTitleToGame(true);
			SceneManager::GetInstance()->ChangeScene("Game");
			Audio::GetInstance()->FadeOutStop(sourceVoice, 1.0f, 2.0f);
		}


		emitter_->Emit();


		mpInfo_->Update();

		if (!isDebugCamera_) {
			player_->Update();
		}
		book_->Update();

		for (auto& cloud : clouds_) {
			cloud->Update();
		}

		titleScreen_->Update();
	}

#ifdef _DEBUG
	if ((Input::GetInstance()->TriggerKey(DIK_LCONTROL)) || Input::GetInstance()->IsPadTriggered(0, GamePadButton::RT)) {
		isDebugCamera_ = !isDebugCamera_;
	}
#endif // _DEBUG

	if (isAlreadyRead_) {
		// 読書が終わっている前提で
		player_->SetIsFinishedReadBook(true);
	}





	bookEventCamera_.RegisterControlPoints();
	UpdateCameraMode();
	UpdateCamera();
	cameraManager_.UpdateAllCameras();

	ParticleManager::GetInstance()->Update();
	LightManager::GetInstance()->ShowLightingEditor();
	CollisionManager::GetInstance()->Update();
	JsonManager::DrawImGui();
}


/// <summary>
/// 描画処理
/// </summary>
void TitleScene::Draw()
{
	//---------
	// 3D
	//---------
	Object3dCommon::GetInstance()->DrawPreference();
	LightManager::GetInstance()->SetCommandList();
	DrawObject();


	//---------
	// Animation
	//---------
	SkinningManager::GetInstance()->DrawPreference();
	LightManager::GetInstance()->SetCommandList();
	DrawAnimation();
	DrawLine();

	// Particle

	ParticleManager::GetInstance()->Draw();


	SpriteCommon::GetInstance()->DrawPreference();
	DrawSprite();

}

void TitleScene::DrawOffScreen()
{

	//----------
	// Sprite
	//----------



}

void TitleScene::DrawObject()
{
	mpInfo_->Draw();
	player_->Draw();
	book_->Draw();
	for (auto& cloud : clouds_) {
		cloud->Draw();
	}
	// 制御点描画
	//bookEventCamera_.Draw(sceneCamera_.get());
}

void TitleScene::DrawSprite()
{
	player_->DrawSprite();
	titleScreen_->Draw();
	book_->DrawSprite();
	if (MenuOverlay::GetInstance()->IsVisible())
	{
		MenuOverlay::GetInstance()->Draw();
	}
}

void TitleScene::DrawAnimation()
{
}

void TitleScene::DrawLine()
{
#ifdef _DEBUG
	player_->DrawCollision();
	book_->DrawCollision();

#endif // _DEBUG

}

/// <summary>
/// 解放処理
/// </summary>
void TitleScene::Finalize()
{
	cameraManager_.RemoveCamera(sceneCamera_);
}


void TitleScene::UpdateCameraMode()
{
#ifdef _DEBUG
	ImGui::Begin("Camera Mode");
	if (ImGui::Button("DEFAULT Camera")) {
		cameraMode_ = CameraMode::DEFAULT;
	}
	if (ImGui::Button("Follow Camera")) {
		cameraMode_ = CameraMode::FOLLOW;
	}
	if (ImGui::Button("Debug Camera")) {
		cameraMode_ = CameraMode::DEBUG;
	}
	if (ImGui::Button("Book Event")) {
		cameraMode_ = CameraMode::BOOK_EVENT;
	}
	ImGui::End();
#endif
}

void TitleScene::UpdateCamera()
{
	switch (cameraMode_)
	{
	case CameraMode::DEFAULT:
	{
		defaultCamera_.Update();
		sceneCamera_->SetFovY(defaultCamera_.GetFov());
		sceneCamera_->viewMatrix_ = defaultCamera_.matView_;
		sceneCamera_->transform_.translate = defaultCamera_.translate_;
		sceneCamera_->transform_.rotate = defaultCamera_.rotate_;
		sceneCamera_->UpdateMatrix();
	}
	break;
	case CameraMode::FOLLOW:
	{

		followCamera_.Update();
		sceneCamera_->viewMatrix_ = followCamera_.matView_;
		sceneCamera_->transform_.translate = followCamera_.translate_;
		sceneCamera_->transform_.rotate = followCamera_.rotate_;

		sceneCamera_->UpdateMatrix();
	}
	break;
	case CameraMode::DEBUG:
	{
		if (isDebugCamera_) {
			debugCamera_.Update();
			//sceneCamera_->SetFovY(debugCamera_.GetFov());
			sceneCamera_->viewMatrix_ = debugCamera_.matView_;
			sceneCamera_->transform_.translate = debugCamera_.translate_;
			sceneCamera_->transform_.rotate = debugCamera_.rotate_;
			sceneCamera_->UpdateMatrix();
		}
	}

	break;
	case CameraMode::BOOK_EVENT:
	{
		if (isBookTrigger_) {

			bookEventCamera_.Update();
			sceneCamera_->SetFovY(bookEventCamera_.GetFov());
			sceneCamera_->viewMatrix_ = bookEventCamera_.matView_;
			sceneCamera_->transform_.translate = bookEventCamera_.translate_;
			sceneCamera_->transform_.rotate = bookEventCamera_.rotate_;

			sceneCamera_->UpdateMatrix();
		}
	}
	break;

	default:
		break;
	}
}



