#pragma once
#include <Vector3.h>
#include <WorldTransform/WorldTransform.h>
#include "Loaders/Json/JsonManager.h"
#include "Systems/Camera/Camera.h"
#include "StageObject/StageObjectManager/StageObjectManager.h"

class StageManager
{
public:

    void Initialize(Camera* camera);

    void Update();

    void InitJson();

    void SelectStage();

    void Draw();

private:
    Camera* camera_;

    std::unique_ptr<JsonManager> jsonManager_;

    std::string currentStage_;

    std::vector<std::string> stageVector_;

    int totalStageNum_ = 0;

    StageObjectManager stageObjManager_;
};

