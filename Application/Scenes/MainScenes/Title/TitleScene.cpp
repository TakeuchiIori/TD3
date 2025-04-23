#include "TitleScene.h"
// Engine
#include "CoreScenes./Manager./SceneManager.h"
#include "Systems./Input./Input.h"
#include "Loaders./Texture./TextureManager.h"
#include "Particle./ParticleManager.h"
#include "Object3D/Object3dCommon.h"
#include "../Graphics/PipelineManager/SkinningManager.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // DEBUG
#include "LightManager/LightManager.h"
#include "Sprite/SpriteCommon.h"
#include <Collision/Core/CollisionManager.h>
#include <Systems/GameTime/GameTIme.h>
#include <Loaders/Json/JsonManager.h>

/// <summary>
/// 初期化処理
/// </summary>
void TitleScene::Initialize()
{
    GameTime::Initailzie();
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
    soundData = Audio::GetInstance()->LoadAudio(L"Resources./images./harpohikunezumi.mp3");
    //// オーディオの再生
    //sourceVoice = Audio::GetInstance()->SoundPlayAudio(soundData);
    //// 音量の設定（0.0f ～ 1.0f）
    //Audio::GetInstance()->SetVolume(sourceVoice, 0.05f); // 80%の音量に設定



	mpInfo_ = std::make_unique<MapChipInfo>();
	mpInfo_->Initialize();
	mpInfo_->SetCamera(sceneCamera_.get());



	player_ = std::make_unique<TitlePlayer>(mpInfo_->GetMapChipField());
	player_->Initialize(sceneCamera_.get());
	bookEventCamera_.SetTarget(player_->GetWorldTransform());

	book_ = std::make_unique<Book>(mpInfo_->GetMapChipField());
	book_->Initialize(sceneCamera_.get());




    // コールバック関数
    book_->OnBookTrigger_ = [this]() {
        cameraMode_ = CameraMode::BOOK_EVENT;
        this->isBookTrigger_ = true;
        // 時間止める
        GameTime::Pause();
     };

	bookEventCamera_.isFinishedMove_ = [this]() {
		this->isStartEvent_ = true;
		};

    
}

/// <summary>
/// 更新処理
/// </summary>
void TitleScene::Update()
{
    GameTime::Update();
    GameTime::ImGui();

    if (isStartEvent_) {
		// ここにイベントの処理を書く
		// 例えば、シーン遷移やアニメーションの開始など
		//SceneManager::GetInstance()->ChangeScene("Game");
		isStartEvent_ = false;
    }

    //if (Input::GetInstance()->PushKey(DIK_RETURN) || Input::GetInstance()->IsPadPressed(0,GamePadButton::A)) {
    //    sceneManager_->ChangeScene("Game");
    //}

#ifdef _DEBUG
    if ((Input::GetInstance()->TriggerKey(DIK_LCONTROL)) || Input::GetInstance()->IsPadTriggered(0, GamePadButton::RT)) {
        isDebugCamera_ = !isDebugCamera_;
    }
#endif // _DEBUG
 




	mpInfo_->Update();

    if (!isDebugCamera_) {
        player_->Update();
    }
	book_->Update();


    bookEventCamera_.RegisterControlPoints();
    UpdateCameraMode();
	UpdateCamera();
	cameraManager_.UpdateAllCameras();
    
    LightManager::GetInstance()->ShowLightingEditor();
    CollisionManager::GetInstance()->Update();
    JsonManager::ImGuiManager();
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

}

void TitleScene::DrawOffScreen()
{
    // Particle
    //----------
    ParticleManager::GetInstance()->Draw();
    //----------
    // Sprite
    //----------
    SpriteCommon::GetInstance()->DrawPreference();
    DrawSprite();


}

void TitleScene::DrawObject()
{
	mpInfo_->Draw();
	player_->Draw();
	book_->Draw();
	
    // 制御点描画
    //bookEventCamera_.Draw(sceneCamera_.get());
}

void TitleScene::DrawSprite()
{
    book_->DrawSprite();
}

void TitleScene::DrawAnimation()
{
}

void TitleScene::DrawLine()
{
    player_->DrawCollision();
	book_->DrawCollision();
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
            sceneCamera_->SetFovY(debugCamera_.GetFov());
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



