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

public:
    bool IsVisible() const { return isVisible_; }

private:
    bool isInit_ = false;
    bool isVisible_ = false;

    Input* input_ = nullptr;

    // ボリューム関係
    AudioVolumeManager* volumeManager_ = nullptr;

    Vector2 volumeUIOffset_ = { 10.0f, 10.0f };

    float minX = 0.0f;
    float maxX = 100.0f;
    float intervalY_ = 30.0f;

    bool isView = false;
    bool isPush = false;
    struct VolumeControl
    {
        Vector2 pos = {};
        float radius = 10.0f;
        bool isPush = false;
    };
    float volume[static_cast<int>(AudioType::kAudioTypeNum)] = {1.0f,1.0f,1.0f,1.0f};
    std::vector<VolumeControl> volumeControl_ = std::vector<VolumeControl>(static_cast<int>(AudioType::kAudioTypeNum));
    std::array<std::unique_ptr<Sprite>, static_cast<int>(AudioType::kAudioTypeNum)> volumeSprites_;

    std::unique_ptr<Sprite> backGround_;

    std::unique_ptr<JsonManager> jsonManager_;

};

