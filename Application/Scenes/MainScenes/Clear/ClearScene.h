#pragma once
// C++
#include <memory>
#include <map>

// Engine
#include "CoreScenes/Base/BaseScene.h"
#include "Systems/Camera/Camera.h"
#include "Systems/Camera/CameraManager.h"
#include "Systems/Audio/Audio.h"
#include "Particle/ParticleEmitter.h"
#include "Object3D/Object3d.h"
#include "Sprite/Sprite.h"
#include "WorldTransform./WorldTransform.h"
#include "Drawer/LineManager/Line.h"
#include "../Transitions/Fade/Fade.h"

// Cameras
#include "../../../SystemsApp/Cameras/DebugCamera/DebugCamera.h"
#include "../../../SystemsApp/Cameras/FollowCamera/FollowCamera.h"
#include "../../../SystemsApp/Cameras/TopDownCamera/TopDownCamera.h"
#include "../../../SystemsApp/Cameras/BookEventCamera/BookEventCamera.h"
#include "../../../SystemsApp/Cameras/DefaultCamera/DefaultCamera.h"

// Math
#include "Vector3.h"

// App
#include "../../../Objects/Player/ClearPlayer.h"
#include "../../../Objects/Planet/Planet.h"

class ClearScene : public BaseScene
{
	enum class CameraMode
	{
		DEFAULT,
		FOLLOW,
		DEBUG,
		BOOK_EVENT
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 
	/// </summary>
	void DrawOffScreen() override;


	Matrix4x4 GetViewProjection() override { return sceneCamera_->viewProjectionMatrix_; }


private:

	/// <summary>
	/// カメラモードを更新する
	/// </summary>
	void UpdateCameraMode();

	/// <summary>
	/// カメラを更新する
	/// </summary>
	void UpdateCamera();
private:
	/*=================================================================

								カメラ

	=================================================================*/
	CameraMode cameraMode_;
	std::shared_ptr<Camera> sceneCamera_;
	CameraManager cameraManager_;

	DefaultCamera defaultCamera_;
	FollowCamera followCamera_;
	DebugCamera debugCamera_;
	bool isDebugCamera_ = false;

	float cameraScrollStart_ = 16.0f;
	float cameraScrollEnd_ = 22.0f;
	float offsetY_ = -0.5f;

	/*=================================================================

								サウンド

	=================================================================*/
	Audio::SoundData soundData;


	/*=================================================================

								オブジェクト	

	=================================================================*/
	std::unique_ptr<ClearPlayer> player_;
	std::unique_ptr< Planet> planet_;



	/*=================================================================

								スプライト

	=================================================================*/
	std::unique_ptr<Sprite> sprite_;
};

