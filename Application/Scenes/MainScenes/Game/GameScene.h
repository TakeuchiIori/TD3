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
#include "Systems/UI/UIBase.h"
#include "EnemyManager/EnemyManager.h"

// Math
#include "Vector3.h"

// Cameras
#include "../../../SystemsApp/Cameras/DebugCamera/DebugCamera.h"
#include "../../../SystemsApp/Cameras/FollowCamera/FollowCamera.h"
#include "../../../SystemsApp/Cameras/TopDownCamera/TopDownCamera.h"

// App
#include "../SystemsApp/Picture/Picture.h"
#include "Player/Player.h"
#include "Grass/GrassManager.h"
#include "../../../SpriteApp/GameScreen.h"
#include "StageManager.h"
#include "StageEditor.h"
#include "Giraffe/Giraffe.h"
#include "Particle/ParticleEmitter.h"

enum class CameraMode
{
    DEFAULT,
    FOLLOW,
    TOP_DOWN,
    FPS,
    DEBUG

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

private:

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

private:
    /*=================================================================

                               カメラ関連

    =================================================================*/
    CameraMode cameraMode_;
    std::shared_ptr<Camera> sceneCamera_;
    CameraManager cameraManager_;
    FollowCamera followCamera_;
    TopDownCamera topDownCamera_;
    DebugCamera debugCamera_;
    bool isDebugCamera_ = false;


    /*=================================================================

                               サウンド関連

    =================================================================*/
    Audio::SoundData soundData;
    IXAudio2SourceVoice* sourceVoice;

    /*=================================================================

                              パーティクル関連

    =================================================================*/



    /*=================================================================

                               スプライト関連

    =================================================================*/
	std::unique_ptr<GameScreen> gameScreen_;
    std::unique_ptr<Sprite> back_;

    /*=================================================================

                               オブジェクト関連

    =================================================================*/
    std::unique_ptr<Giraffe> giraffe_;

    // 3Dモデル

    /*=================================================================

                                   線

    =================================================================*/


    /*=================================================================

                                 その他

    =================================================================*/
    std::unique_ptr<MapChipInfo> mpInfo_;
	std::unique_ptr<JsonManager> jsonManager_;
    bool isClear_ = false;

    // ステージ関連（3Dモデル含む）
    StageEditor* stageEditor_ = nullptr;
    std::unique_ptr<StageManager> stageManager_;
};
