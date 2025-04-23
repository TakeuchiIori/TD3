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
#include "Systems/MapChip/MapChipInfo.h"



// Math
#include "Vector3.h"

// Cameras
#include "../../../SystemsApp/Cameras/DebugCamera/DebugCamera.h"
#include "../../../SystemsApp/Cameras/FollowCamera/FollowCamera.h"
#include "../../../SystemsApp/Cameras/TopDownCamera/TopDownCamera.h"
#include "../../../SystemsApp/Cameras/BookEventCamera/BookEventCamera.h"
#include "../../../SystemsApp/Cameras/DefaultCamera/DefaultCamera.h"
// app
#include "Player/TitlePlayer.h" 
#include "Book/Book.h"

class TitleScene : public BaseScene
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
	/// 3Dオブジェクトの描画
	/// </summary>
	void DrawObject();

	/// <summary>
	/// 2Dスプライトの描画
	/// </summary>
	void DrawSprite();

	/// <summary>
	/// アニメーション描画
	/// </summary>
	void DrawAnimation();

	/// <summary>
	/// 線描画
	/// </summary>
	void DrawLine();


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
	BookEventCamera bookEventCamera_;
	bool isDebugCamera_ = false;

	/*=================================================================

								サウンド

	=================================================================*/
	Audio::SoundData soundData;
	IXAudio2SourceVoice* sourceVoice;


	/*=================================================================

								オブジェクト

	=================================================================*/
	std::unique_ptr<TitlePlayer> player_;
	std::unique_ptr<Book> book_;


	/*=================================================================

								その他

	=================================================================*/
	
	std::unique_ptr<MapChipInfo> mpInfo_;
	bool isBookTrigger_ = false;
	bool isStartEvent_ = false;

	/*=================================================================

							ゲームシーンに移行

	=================================================================*/
	bool isGameScene_ = false;

};

