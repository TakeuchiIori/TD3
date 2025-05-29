#pragma once

#include "Systems/Input/Input.h"
#include "Loaders/Json/JsonManager.h"
#include "Sprite/Sprite.h"


#include "../Application/SystemsApp/AppAudio/AudioVolumeManager.h"

class MenuOverlay
{
public:
    static MenuOverlay* GetInstance() {
        static MenuOverlay instance;
        return &instance;
    }

public:
    void Initialize();
    void InitJson();
    void Update();
    void Draw();
    void ShowHide();


private:
    void Show();
    void Hide();

    void VolumeSlider();

    float Length(const Vector2& v);

    void StartScaling();

    void ButtonScaling();

public:
    bool IsVisible() const { return isVisible_; }

private:
    bool isInit_ = false;
    bool isVisible_ = false;
    
    Input* input_ = nullptr;

    // ボリューム関係
    AudioVolumeManager* volumeManager_ = nullptr;

    Vector2 volumeUIOffset_ = { 278.0f, 427.0f };

    float maxY = 0.0f;
    float minY = 348.0f;
    float intervalX_ = 115.0f;

    float changeVolume_ = 10.0f;

    bool isView = false;
    bool isPush = false;
    struct VolumeControl
    {
        Vector2 pos = {};
        float radius = 35.0f;
        bool isPush = false;
    };
    float volume[static_cast<int>(AudioType::kAudioTypeNum)] = {1.0f,1.0f};
    std::vector<VolumeControl> volumeControl_ = std::vector<VolumeControl>(static_cast<int>(AudioType::kAudioTypeNum));
    std::array<std::unique_ptr<Sprite>, static_cast<int>(AudioType::kAudioTypeNum)> volumeSprites_;

    float volStartScale_ = 1.0f;
    float volGoalScale_ = 1.5f;
    float kVolTime_ = 0.4f;
    float volTimer_ = 0;
    float volT_ = 0;

    std::unique_ptr<Sprite> backGround_;

    std::unique_ptr<JsonManager> jsonManager_;

    float kMenuTime_ = 1.0f;
    float menuTimer_ = 0;
    float kirinTime_ = 0.5f;

    std::unique_ptr<Sprite> menuTex_;
    Vector3 menuStart_ = Vector3(400, 1920, 0);
    Vector3 menuGoal_ = Vector3(640, 360, 0);
    std::string controllerPath_ = "Resources/Textures/Menu/menu.png";
    std::string mousePath_ = "Resources/Textures/Menu/menuMouse.png";

    std::unique_ptr<Sprite> kirinL_;
    Vector3 kirinLStart_ = Vector3(-1000, 0, 0);
    Vector3 kirinLGoal_ = Vector3(0, 0, 0);
    std::unique_ptr<Sprite> kirinR_;
    Vector3 kirinRStart_ = Vector3(1764, 0, 0);
    Vector3 kirinRGoal_ = Vector3(764, 0, 0);

    float goalScale_ = 1.2f;
    float startScale_ = 0.0f;

    bool isAnimation_ = false;
    bool isEndAnimation_ = false;

    Audio::SoundData menuSoundData_ = {};
    IXAudio2SourceVoice* menuSourceVoice_ = nullptr;

};

