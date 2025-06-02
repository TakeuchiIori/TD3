#include "StageEditor.h"

void StageEditor::Save(const std::string& filename)
{
    nlohmann::json j;
    for (const auto& stage : stages_) {
        nlohmann::json stageJson;
        stageJson["stageNumber"] = stage.stageNumber;

        for (const auto& def : stage.objectDefinitions) {
            stageJson["objectDefinitions"].push_back({ {"id", def.id}, {"name", def.name} });
        }

        for (const auto& cp : stage.checkPoints) {
            nlohmann::json cpJson;
            cpJson["checkPointNumber"] = cp.checkPointNumber;
            cpJson["height"] = cp.height;
            cpJson["initX"] = cp.initX;
            cpJson["timer"] = cp.timer;

            for (const auto& obj : cp.objects) {
                cpJson["objects"].push_back({
                    {"id", obj.id},
                    {"position", {{"x", obj.position.x}, {"y", obj.position.y}, {"z", obj.position.z}}}
                    });
            }
            stageJson["checkPoints"].push_back(cpJson);
        }

        j["stages"].push_back(stageJson);
    }

    std::ofstream file(filename);
    file << j.dump(4);
}

void StageEditor::Load(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) return;

    nlohmann::json j;
    file >> j;
    stages_.clear();

    if (!j.contains("stages")) return;

    for (const auto& stageJson : j["stages"]) {
        StageStruct stage;

        if (stageJson.contains("stageNumber")) {
            stage.stageNumber = stageJson["stageNumber"];
        }

        if (stageJson.contains("objectDefinitions")) {
            for (const auto& def : stageJson["objectDefinitions"]) {
                ObjectDefinition d;
                if (def.contains("id")) d.id = def["id"];
                if (def.contains("name")) d.name = def["name"];
                stage.objectDefinitions.push_back(d);
            }
        }

        if (stageJson.contains("checkPoints")) {
            for (const auto& cpJson : stageJson["checkPoints"]) {
                CheckPointStruct cp;
                if (cpJson.contains("checkPointNumber")) cp.checkPointNumber = cpJson["checkPointNumber"];
                if (cpJson.contains("height")) cp.height = cpJson["height"];
                if (cpJson.contains("initX")) cp.initX = cpJson["initX"];
                if (cpJson.contains("timer")) cp.timer = cpJson["timer"];

                if (cpJson.contains("objects")) {
                    for (const auto& objJson : cpJson["objects"]) {
                        PlacedObject obj;
                        if (objJson.contains("id")) obj.id = objJson["id"];
                        if (objJson.contains("position")) {
                            if (objJson["position"].contains("x")) obj.position.x = objJson["position"]["x"];
                            if (objJson["position"].contains("y")) obj.position.y = objJson["position"]["y"];
                            if (objJson["position"].contains("z")) obj.position.z = objJson["position"]["z"];
                        }
                        cp.objects.push_back(obj);
                    }
                }

                stage.checkPoints.push_back(cp);
            }
        }

        stages_.push_back(stage);
    }
}

void StageEditor::DrawEditorUI()
{
    static int selectedStageIndex = -1;
    static int selectedCheckPointIndex = -1;
#ifdef _DEBUG

    if (ImGui::Begin("Stage Editor")) {
        ImGui::Text("Vで配置をリロード"); // 処理はStageManager::Update()に
        if (ImGui::CollapsingHeader("ステージ一覧", ImGuiTreeNodeFlags_DefaultOpen)) {
            for (int i = 0; i < stages_.size(); ++i) {
                std::string label = "ステージ : " + std::to_string(stages_[i].stageNumber);
                if (ImGui::Selectable(label.c_str(), selectedStageIndex == i)) {
                    selectedStageIndex = i;
                    selectedCheckPointIndex = -1;
                }
            }

            if (ImGui::Button("ステージ追加") && stages_.size() < kMaxStages) {
                StageStruct newStage;
                newStage.stageNumber = static_cast<int>(stages_.size());
                stages_.push_back(newStage);
            }

            if (ImGui::Button("Save")) {
                Save("Resources/JSON/StageEditor/StageEditor.json");
            }
            if (ImGui::Button("Load")) {
                Load("Resources/JSON/StageEditor/StageEditor.json");
            }
        }

        if (selectedStageIndex >= 0 && selectedStageIndex < stages_.size()) {
            StageStruct& stage = stages_[selectedStageIndex];

            if (ImGui::CollapsingHeader("今は未使用", ImGuiTreeNodeFlags_DefaultOpen)) {
                for (size_t i = 0; i < stage.objectDefinitions.size(); ++i) {
                    ObjectDefinition& def = stage.objectDefinitions[i];
                    ImGui::PushID(static_cast<int>(i));
                    ImGui::InputInt("ID", &def.id);
                    char buffer[128];
                    strncpy_s(buffer, sizeof(buffer), def.name.c_str(), _TRUNCATE);
                    if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
                        def.name = buffer;
                    }
                    if (ImGui::Button("Delete Definition")) {
                        stage.objectDefinitions.erase(stage.objectDefinitions.begin() + i);
                        --i;
                    }
                    ImGui::PopID();
                }
                if (ImGui::Button("Add Object Definition")) {
                    stage.objectDefinitions.push_back({ 0, "NewObject" });
                }
            }

            if (ImGui::CollapsingHeader("チェックポイント一覧", ImGuiTreeNodeFlags_DefaultOpen)) {
                for (int i = 0; i < stage.checkPoints.size(); ++i) {
                    std::string label = "チェックポイント : " + std::to_string(stage.checkPoints[i].checkPointNumber);
                    if (ImGui::Selectable(label.c_str(), selectedCheckPointIndex == i)) {
                        selectedCheckPointIndex = i;
                    }
                }
                if (ImGui::Button("チェックポイント追加")) {
                    CheckPointStruct cp;
                    cp.checkPointNumber = static_cast<int>(stage.checkPoints.size());
                    stage.checkPoints.push_back(cp);
                }

                if (selectedCheckPointIndex >= 0 && selectedCheckPointIndex < stage.checkPoints.size()) {
                    CheckPointStruct& cp = stage.checkPoints[selectedCheckPointIndex];
                    ImGui::InputInt("チェックポイント番号", &cp.checkPointNumber);
                    ImGui::DragFloat("チェックポイントの高さ", &cp.height, 0.1f);
                    ImGui::DragFloat("制限時間", &cp.timer, 0.1f);
                    ImGui::DragFloat("初期のX座標", &cp.initX, 0.1f);

                    for (int i = 0; i < cp.objects.size(); ++i) {
                        PlacedObject& obj = cp.objects[i];
                        ImGui::PushID(i);
                        ImGui::InputInt("Object ID", &obj.id);
                        ImGui::DragFloat3("Position", (float*)&obj.position, 0.1f);
                        if (ImGui::Button("削除")) {
                            cp.objects.erase(cp.objects.begin() + i);
                            --i;
                        }
                        ImGui::PopID();
                    }
                    if (ImGui::Button("追加")) {
                        cp.objects.push_back({ 0, {0, 0, 0} });
                    }
                    if (ImGui::Button("チェックポイントの削除")) {
                        stage.checkPoints.erase(stage.checkPoints.begin() + selectedCheckPointIndex);
                        selectedCheckPointIndex = -1;
                    }
                }
            }
        }
    }
    ImGui::End();
#endif // _DEBUG

}
