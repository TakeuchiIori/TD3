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
    // 初期カメラモード設定
    cameraMode_ = CameraMode::DEFAULT;
    followCamera_.Initialize();
    debugCamera_.Initialize();

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

}

/// <summary>
/// 更新処理
/// </summary>
void TitleScene::Update()
{
    GameTime::Update();
    GameTime::ImGui();

    if (Input::GetInstance()->PushKey(DIK_RETURN) || Input::GetInstance()->IsPadPressed(0,GamePadButton::A)) {
        sceneManager_->ChangeScene("Game");
    }

#ifdef _DEBUG
    if ((Input::GetInstance()->TriggerKey(DIK_LCONTROL)) || Input::GetInstance()->IsPadTriggered(0, GamePadButton::RT)) {
        isDebugCamera_ = !isDebugCamera_;
    }
#endif // _DEBUG
 




	mpInfo_->Update();

    if (isDebugCamera_) {
        player_->Update();
    }

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
}

void TitleScene::DrawSprite()
{
}

void TitleScene::DrawAnimation()
{
}

void TitleScene::DrawLine()
{
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
    ImGui::End();
#endif
}

void TitleScene::UpdateCamera()
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



