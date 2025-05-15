#include "GameScene.h"
// Engine
#include "CoreScenes./Manager./SceneManager.h"
#include "Systems./Input./Input.h"
#include "Systems/Audio/Audio.h"
#include "Loaders./Texture./TextureManager.h"
#include "Particle./ParticleManager.h"
#include "Object3D/Object3dCommon.h"
#include "PipelineManager/SkinningManager.h"
#include "Loaders/Model/Model.h"
#include "Collision/Core/CollisionManager.h"
#include "../Graphics/Culling/OcclusionCullingManager.h"

// App
#include "../Application/SystemsApp/AppAudio/AudioVolumeManager.h"
#include "../Application/SpriteApp/ScreenApp/MenuOverlay.h"

// C++
#include <cstdlib>
#include <ctime>

#ifdef _DEBUG
#include "imgui.h"
#endif // DEBUG
#include "LightManager/LightManager.h"
#include "Sprite/SpriteCommon.h"
#include <Systems/GameTime/GameTIme.h>

/// <summary>
/// 初期化処理
/// </summary>
void GameScene::Initialize()
{
	srand(static_cast<unsigned int>(time(nullptr))); // 乱数シード設定
	// カメラの生成
	sceneCamera_ = cameraManager_.AddCamera();
	GameTime::Initailzie();

    // 初期カメラモード設定
    cameraMode_ = CameraMode::FOLLOW;

	CollisionManager::GetInstance()->Initialize();

	MenuOverlay::GetInstance()->Initialize();

	followCamera_.Initialize();
	debugCamera_.Initialize();

	ParticleManager::GetInstance()->SetCamera(sceneCamera_.get());

	// 各オブジェクトの初期化
	mpInfo_ = std::make_unique<MapChipInfo>();
	mpInfo_->Initialize();
	mpInfo_->SetCamera(sceneCamera_.get());

	// エディターの初期化
	stageEditor_ = StageEditor::Instance();
	stageEditor_->Load("Resources/JSON/StageEditor/StageEditor.json");
	// ステージの初期化
	stageManager_ = std::make_unique<StageManager>();
	stageManager_->SetMapChipInfo(mpInfo_.get());
	stageManager_->Initialize(sceneCamera_.get());
	stageManager_->SetFollowCamera(&followCamera_);


	followCamera_.SetTarget(stageManager_->GetPlayer()->GetWorldTransform());


	giraffe_ = std::make_unique<Giraffe>();
	giraffe_->Initialize();
	giraffe_->SetCamera(sceneCamera_.get());
    
    // 地面
    ground_ = std::make_unique<Ground>();
    ground_->Initialize(sceneCamera_.get());



	//// オーディオファイルのロード（例: MP3）
	soundData = Audio::GetInstance()->LoadAudio(L"Resources/Audio/BGM2.mp3");
	// オーディオの再生
	sourceVoice = Audio::GetInstance()->SoundPlayAudio(soundData,true);
	// 音量の設定（0.0f ～ 1.0f）
	Audio::GetInstance()->SetVolume(sourceVoice, 0.1f); // 80%の音量に設定

	AudioVolumeManager::GetInstance()->SetSourceToSubmix(sourceVoice, kBGM);


	gameScreen_ = std::make_unique<GameScreen>();
	gameScreen_->SetCamera(sceneCamera_.get());
	gameScreen_->SetPlayer(stageManager_->GetPlayer());
	gameScreen_->SetCheckPointPos(stageManager_->GetCheckPointPos());
	gameScreen_->Initialize();
	
	//=====================================================//
	/*                  これより下は触るな危険　　　　　　　   　*/
	//=====================================================//

}

/// <summary>
/// 更新処理
/// </summary>
void GameScene::Update()
{
	GameTime::Update();
	GameTime::ImGui();
	gameScreen_->SetCheckPoint(stageManager_->GetCheckPoint());
#ifdef _DEBUG
	if ((Input::GetInstance()->TriggerKey(DIK_LCONTROL)) || Input::GetInstance()->IsPadTriggered(0, GamePadButton::RT)) 
	{
		isDebugCamera_ = !isDebugCamera_;
	}
	stageEditor_->DrawEditorUI();
#endif // _DEBUG

	// クリアしたとき
	if (stageManager_->IsClear()) 
	{
		sceneManager_->ChangeScene("Clear");
	}
	// プレイ中
	else
	{

		MenuOverlay::GetInstance()->ShowHide();
		if (MenuOverlay::GetInstance()->IsVisible())
		{
			MenuOverlay::GetInstance()->Update();
		}
		else
		{
			mpInfo_->Update();



			if (!stageManager_->CheckPointTransition())
			{
				if (!isDebugCamera_)
				{
					stageManager_->NotDebugCameraUpdate();
				}
				stageManager_->Update();
			}
		}

	}

	giraffe_->Update();
	ground_->Update();

	ParticleManager::GetInstance()->Update();
	// カメラ更新
	UpdateCameraMode();
	UpdateCamera();


	ShowImGui();



	JsonManager::ImGuiManager();
	// ワールドトランスフォーム更新
	cameraManager_.UpdateAllCameras();

	//=====================================================//
	/*                  これより下は触るな危険　　　　　　　   　*/
	//=====================================================//

	// ライティング
	LightManager::GetInstance()->ShowLightingEditor();
	CollisionManager::GetInstance()->Update();

	gameScreen_->Update();

}


/// <summary>
/// 描画処理
/// </summary>
void GameScene::Draw()
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


}

void GameScene::DrawOffScreen()
{

	//----------
	// Particle
	//----------
	ParticleManager::GetInstance()->Draw();
	//----------
	ParticleManager::GetInstance()->Draw();
	//----------
	// Sprite
	//----------
	SpriteCommon::GetInstance()->DrawPreference();
	DrawSprite();


}


void GameScene::DrawObject()
{
	giraffe_->Draw();
	mpInfo_->Draw();
	ground_->Draw();
	stageManager_->Draw();
}

void GameScene::DrawSprite()
{
	gameScreen_->Draw();
	stageManager_->DrawTransition();
	if (MenuOverlay::GetInstance()->IsVisible()) {
		MenuOverlay::GetInstance()->Draw();
	}
}

void GameScene::DrawAnimation()
{
	stageManager_->DrawAnimation();
}

void GameScene::DrawLine()
{
#ifdef _DEBUG
	stageManager_->DrawCollision();
#endif // _DEBUG

}


/// <summary>
/// 解放処理
/// </summary>
void GameScene::Finalize()
{
	cameraManager_.RemoveCamera(sceneCamera_);
	Audio::GetInstance()->PauseAudio(sourceVoice);
}


void GameScene::UpdateCameraMode()
{
#ifdef _DEBUG
	ImGui::Begin("Camera Mode");
	if (ImGui::Button("DEFAULT Camera")) {
		cameraMode_ = CameraMode::DEFAULT;
	}
	if (ImGui::Button("Follow Camera")) {
		cameraMode_ = CameraMode::FOLLOW;
	}
	if (ImGui::Button("Top-Down Camera")) {
		cameraMode_ = CameraMode::TOP_DOWN;
	}
	if (ImGui::Button("FPS Camera")) {
		cameraMode_ = CameraMode::FPS;
	}
	if (ImGui::Button("DEBUG Camera")) {
		cameraMode_ = CameraMode::DEBUG;
	}
	ImGui::End();
#endif
}

void GameScene::UpdateCamera()
{
	switch (cameraMode_)
	{
	case CameraMode::DEFAULT:
	{
		sceneCamera_->DefaultCamera();
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
	case CameraMode::TOP_DOWN:
	{
		topDownCamera_.Update();
		sceneCamera_->viewMatrix_ = topDownCamera_.matView_;
		sceneCamera_->transform_.translate = topDownCamera_.translate_;
		sceneCamera_->transform_.rotate = topDownCamera_.rotate_;
		sceneCamera_->UpdateMatrix();
	}
	break;
	case CameraMode::FPS:
	{
	}
	break;

	case CameraMode::DEBUG:
	{
		if (isDebugCamera_) {
			debugCamera_.Update();
			sceneCamera_->SetFovY(debugCamera_.GetFov());
			sceneCamera_->viewMatrix_ = debugCamera_.matView_;
			sceneCamera_->transform_.translate = debugCamera_.translate_;
			sceneCamera_->transform_.rotate = debugCamera_.rotate_;
			sceneCamera_->UpdateMatrix();
		}
	}

	default:
		break;
	}
}

void GameScene::ShowImGui()
{
#ifdef _DEBUG
	ImGui::Begin("FPS");
	ImGui::Text("FPS:%.1f", ImGui::GetIO().Framerate);
	ImGui::End();
#endif // _DEBUG
}
