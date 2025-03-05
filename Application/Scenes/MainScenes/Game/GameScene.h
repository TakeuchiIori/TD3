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
#include "Ground/Ground.h"
#include "../Transitions/Fade/Fade.h"


// Math
#include "Vector3.h"
#include "../../../SystemsApp/Cameras/FollowCamera/FollowCamera.h"
#include "../../../SystemsApp/Cameras/TopDownCamera/TopDownCamera.h"

#include "../../../SystemsApp/Cameras/PlayerCamera/PlayerCamera.h"

// Application
#include "Player/Player.h"
#include "../Stages/StageManager/StageManager.h"


enum class CameraMode
{
    DEFAULT,
    FOLLOW,
    TOP_DOWN,
    FPS

};

class GameScene : public BaseScene
{
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

private:

    /// <summary>
    /// フェースの切り替え
    /// </summary>
    void ChangePahse();


    /// <summary>
    /// カメラモードを更新する
    /// </summary>
    void UpdateCameraMode();

    /// <summary>
    /// カメラを更新する
    /// </summary>
    void UpdateCamera();

    /// <summary>
    /// ImGui
    /// </summary>
    void ShowImGui();

    void CheckAllCollisions();


private:
    // カメラ
    CameraMode cameraMode_;
    std::shared_ptr<Camera> sceneCamera_;
	//std::shared_ptr<Camera> playerCamera_;
    CameraManager cameraManager_;
	FollowCamera followCamera_;
    TopDownCamera topDownCamera_;
    std::unique_ptr<PlayerCamera> playerCamera_;
    // サウンド
    Audio::SoundData soundData;
    IXAudio2SourceVoice* sourceVoice;
    // パーティクルエミッター
    std::unique_ptr<ParticleEmitter> particleEmitter_[2];
    Vector3 emitterPosition_;
    uint32_t particleCount_;

    Vector3 weaponPos;

    std::unique_ptr<Sprite> sprite_;


    StageManager stageManager_;


    // 3Dモデル
    std::unique_ptr<Player> player_;

    // 地面
    std::unique_ptr< Ground> ground_;

    // 2Dスプライト
    std::vector<std::unique_ptr<Sprite>> sprites;

     bool isClear_ = false;
 
};
