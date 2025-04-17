#pragma once
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <json.hpp>
#include "Vector3.h"
#ifdef _DEBUG

#include "imgui.h"

#endif // _DEBUG
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


public: // getter setter
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

    int GetMaxStageNumber() const {
        int maxNum = -1;
        for (const auto& stage : stages_) {
            if (stage.stageNumber > maxNum) {
                maxNum = stage.stageNumber;
            }
        }
        return maxNum;
    }

    int GetMaxCheckPointNumber(int stageNum) const {
        for (const auto& stage : stages_) {
            if (stage.stageNumber == stageNum) {
                int maxNum = -1;
                for (const auto& cp : stage.checkPoints) {
                    if (cp.checkPointNumber > maxNum) {
                        maxNum = cp.checkPointNumber;
                    }
                }
                return maxNum;
            }
        }
        return -1;
    }

private:
    StageEditor() = default;
    constexpr static int kMaxStages = 10;
    std::vector<StageStruct> stages_;
};


