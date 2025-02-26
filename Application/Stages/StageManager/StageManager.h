#pragma once
#include <Vector3.h>
#include <WorldTransform/WorldTransform.h>
#include "Loaders/Json/JsonManager.h"

class StageManager
{
public:

    void Initialize();

    void Update();

    void InitJson();

    void SelectStage();

private:

    std::unique_ptr<JsonManager> jsonManager_;

    std::string currentStage_;

    std::list<std::string> stageList_;
};

