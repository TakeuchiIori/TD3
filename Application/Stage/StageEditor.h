#pragma once
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <json.hpp>
#include "imgui.h"
#include "Vector3.h"

struct ObjectDefinition {
    int id;
    std::string name;
};

struct PlacedObject {
    int id;
    Vector3 position;
};

struct CheckPointStruct {
    int checkPointNumber;
    float height = 0.0f;
    std::vector<PlacedObject> objects;
};

struct StageStruct {
    int stageNumber;
    std::vector<ObjectDefinition> objectDefinitions;
    std::vector<CheckPointStruct> checkPoints;
};

class StageEditor {
public:
    static StageEditor* Instance() {
        static StageEditor instance;
        return &instance;
    }

    void Save(const std::string& filename);

    void Load(const std::string& filename);

    void DrawEditorUI();

    const std::vector<ObjectDefinition>* GetObjectDefinitions(int stageNum) const {
        for (const auto& s : stages_) if (s.stageNumber == stageNum) return &s.objectDefinitions;
        return nullptr;
    }

    const std::vector<PlacedObject>* GetObjects(int stageNum, int checkPointNum) const {
        for (const auto& s : stages_) {
            if (s.stageNumber == stageNum) {
                for (const auto& cp : s.checkPoints) {
                    if (cp.checkPointNumber == checkPointNum) return &cp.objects;
                }
            }
        }
        return nullptr;
    }

    const float* GetCheckHight(int stageNum, int checkPointNum) const {
        for (const auto& s : stages_) {
            if (s.stageNumber == stageNum) {
                for (const auto& cp : s.checkPoints) {
                    if (cp.checkPointNumber == checkPointNum) return &cp.height;
                }
            }
        }
        return nullptr;
    }

private:
    StageEditor() = default;
    constexpr static int kMaxStages = 10;
    std::vector<StageStruct> stages_;
};


