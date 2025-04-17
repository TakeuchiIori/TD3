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
	//playerCamera_ = cameraManager_.AddCamera();
	GameTime::Initailzie();

    // 初期カメラモード設定
    cameraMode_ = CameraMode::FOLLOW;

	CollisionManager::GetInstance()->Initialize();

	//stageManager_.Initialize(sceneCamera_.get());

	picture_ = std::make_unique<Picture>();
	picture_->Initialize();
	picture_->SetCamera(sceneCamera_.get());


	followCamera_.Initialize();
	debugCamera_.Initialize();

	ParticleManager::GetInstance()->SetCamera(sceneCamera_.get());

	// 各オブジェクトの初期化
	mpInfo_ = std::make_unique<MapChipInfo>();
	mpInfo_->Initialize();
	mpInfo_->SetCamera(sceneCamera_.get());

	stageEditor_ = StageEditor::Instance();
	stageEditor_->Load("Resources/JSON/StageEditor/StageEditor.json");

	stageManager_ = std::make_unique<StageManager>();
	stageManager_->SetMapChipInfo(mpInfo_.get());
	stageManager_->Initialize(sceneCamera_.get());


	followCamera_.SetTarget(stageManager_->GetPlayer()->GetWorldTransform());


	// 敵
	/*enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->SetPlayer(stageManager_->GetPlayer());
	enemyManager_->Initialize(sceneCamera_.get(), mpInfo_->GetMapChipField());*/


    
    // 地面
    ground_ = std::make_unique<Ground>();
    ground_->Initialize(sceneCamera_.get());

	//test_ = std::make_unique<Object3d>();
	//test_->Initialize();
	//test_->SetModel("walk.gltf", true);
	//test->SetModel("sneakWalk.gltf", true);
	testWorldTransform_.Initialize();


	//// オーディオファイルのロード（例: MP3）
	soundData = Audio::GetInstance()->LoadAudio(L"Resources/Audio/BGM2.mp3");
	// オーディオの再生
	sourceVoice = Audio::GetInstance()->SoundPlayAudio(soundData,true);
	// 音量の設定（0.0f ～ 1.0f）
	Audio::GetInstance()->SetVolume(sourceVoice, 0.1f); // 80%の音量に設定

	


	gameScreen_ = std::make_unique<GameScreen>();
	gameScreen_->SetCamera(sceneCamera_.get());
	gameScreen_->SetPlayer(stageManager_->GetPlayer());
	gameScreen_->SetCheckPointPos(stageManager_->GetCheckPointPos());
	gameScreen_->Initialize();
	
	//=====================================================//
	/*                  これより下は触るな危険　　　　　　　   　*/
	//=====================================================//
	//OcclusionCullingManager::GetInstance()->Initialize();
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
	if ((Input::GetInstance()->TriggerKey(DIK_LCONTROL)) || Input::GetInstance()->IsPadTriggered(0, GamePadButton::RT)) {
		isDebugCamera_ = !isDebugCamera_;
	}
	stageEditor_->DrawEditorUI();
#endif // _DEBUG


	mpInfo_->Update();
	

	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		picture_->Update();
    }
    

	//test_->UpdateAnimation();
	testWorldTransform_.UpdateMatrix();

	if (!isDebugCamera_) {
		stageManager_->NotDebugCameraUpdate();
	}
	stageManager_->Update();


	//enemyManager_->Update();

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
	///line_->RegisterLine(start_, end_);
	///line_->DrawLine();

	//---------
	// Animation
	//---------
	SkinningManager::GetInstance()->DrawPreference();
	LightManager::GetInstance()->SetCommandList();
	DrawAnimation();
	DrawLine();

	//OcclusionCullingManager::GetInstance()->ResolvedOcclusionQuery();
}

void GameScene::DrawOffScreen()
{
	//----------
	// Sprite
	//----------
	SpriteCommon::GetInstance()->DrawPreference();
	DrawSprite();


	//----------
	// Particle
	//----------
	ParticleManager::GetInstance()->Draw();

}

void GameScene::DrawObject()
{
	mpInfo_->Draw();
	ground_->Draw();
	//enemyManager_->Draw();
	stageManager_->Draw();
}

void GameScene::DrawSprite()
{
	gameScreen_->Draw();
}

void GameScene::DrawAnimation()
{
	//test_->Draw(sceneCamera_.get(), testWorldTransform_);
}

void GameScene::DrawLine()
{
	stageManager_->DrawCollision();
	//enemyManager_->DrawCollisions();
}


/// <summary>
/// 解放処理
/// </summary>
void GameScene::Finalize()
{
	cameraManager_.RemoveCamera(sceneCamera_);
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
		//sceneCamera_->UpdateMatrix();
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
