#include "ClearScene.h"

#include "CoreScenes./Manager./SceneManager.h"
#include "Systems./Input./Input.h"
#include "Loaders./Texture./TextureManager.h"
#include "Loaders/Json/JsonManager.h"
#include "Particle./ParticleManager.h"
#include "Object3D/Object3dCommon.h"
#include "Sprite/SpriteCommon.h"
#include <LightManager/LightManager.h>
#include <PipelineManager/SkinningManager.h>

void ClearScene::Initialize()
{
	// カメラの生成
	sceneCamera_ = cameraManager_.AddCamera();
	Object3dCommon::GetInstance()->SetDefaultCamera(sceneCamera_.get());
	// 初期カメラモード設定
	cameraMode_ = CameraMode::FOLLOW;


	// 各カメラの初期化
	defaultCamera_.Initialize();
	followCamera_.Initialize();
	debugCamera_.Initialize();

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("Resources/Textures/BackGround/backGround.png");
	sprite_->SetSize(Vector2{ 1280.0f,720.0f });
	sprite_->SetTextureSize(Vector2{ 1280,720 });
	sprite_->SetColor(Vector4{ 0.05f,0.0f,0.2f,1.0f });
	player_ = std::make_unique<ClearPlayer>();
	player_->Initialize(sceneCamera_.get());
	followCamera_.SetTarget(player_->GetWorldTransform());

	planet_ = std::make_unique<Planet>();
	planet_->Initialize(sceneCamera_.get());


	clearScreen_ = std::make_unique<ClearScreen>();
	clearScreen_->Initialize();
}

void ClearScene::Finalize()
{
}

void ClearScene::Update()
{
#ifdef _DEBUG
	if ((Input::GetInstance()->TriggerKey(DIK_LCONTROL)) || Input::GetInstance()->IsPadTriggered(0, GamePadButton::RT)) {
		isDebugCamera_ = !isDebugCamera_;
	}
#endif // _DEBUG

	// プレイヤーの高度を取得
	float playerHeight = player_->GetWorldTransform().translation_.y;

	// プレイヤーのY座標が57以上の場合は追従を無効にする
	if (playerHeight >= 57.0f) {
		followCamera_.SetFollowEnabled(false);
	} else {
		followCamera_.SetFollowEnabled(true);

		// 追従が有効な場合のみカメラスクロール処理を実行
		if (player_->GetCenterPosition().y < cameraScrollStart_) {
			followCamera_.SetOffsetY(cameraScrollStart_ + offsetY_ - player_->GetCenterPosition().y);
		}
	}

	// プレイヤーのイベント状態に応じてカメラの動作を調整
	if (player_->IsEventActive()) {
		// イベント中は追従カメラの感度を下げる、または固定する
		followCamera_.SetFollowEnabled(false);
		clearScreen_->UpdateKirin();
		if (Input::GetInstance()->IsPadPressed(0, GamePadButton::A)) {
			sceneManager_->ChangeScene("Title");
		}
	}

	planet_->Update();
	player_->Update(); // プレイヤー内でイベント処理が行われる

	sprite_->Update();

	UpdateCameraMode();
	UpdateCamera();
	cameraManager_.UpdateAllCameras();

	JsonManager::ImGuiManager();
	LightManager::GetInstance()->ShowLightingEditor();

	clearScreen_->Update();

}

void ClearScene::Draw()
{
#pragma region 演出描画
	ParticleManager::GetInstance()->Draw();



#pragma endregion
#pragma region 2Dスプライト描画
	SpriteCommon::GetInstance()->DrawPreference();
	clearScreen_->Draw();
	/// <summary>
	/// ここから描画可能です
	/// </summary>
	/// 

	sprite_->Draw();


#pragma endregion

#pragma region 3Dオブジェクト描画
	Object3dCommon::GetInstance()->DrawPreference();
	LightManager::GetInstance()->SetCommandList();
	/// <summary>
	/// ここから描画可能です
	/// </summary>
	player_->Draw();
	planet_->Draw();


	//---------
	// Animation
	//---------
	SkinningManager::GetInstance()->DrawPreference();
	LightManager::GetInstance()->SetCommandList();
	player_->DrawAnimation();

#pragma endregion

	SpriteCommon::GetInstance()->DrawPreference();
	clearScreen_->Draw();
}

void ClearScene::DrawOffScreen()
{
}
void ClearScene::UpdateCameraMode()
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

	// プレイヤーのイベント情報の表示
	ImGui::Separator();
	ImGui::Text("Player Event Status:");
	ImGui::Text("Started: %s", player_->IsEventStarted() ? "Yes" : "No");
	ImGui::Text("Active: %s", player_->IsEventActive() ? "Yes" : "No");
	ImGui::Text("Timer: %.2f", player_->GetEventTimer());
	ImGui::Text("Player Height: %.2f", player_->GetWorldTransform().translation_.y);

	ImGui::End();
#endif
}

void ClearScene::UpdateCamera()
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
		//sceneCamera_->SetFovY(followCamera_.GetFov());
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

	default:
		break;
	}
}