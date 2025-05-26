//#include "ParticleEffectManager.h"
//#include <filesystem>
//
//#ifdef _DEBUG
//#include <imgui.h>
//#endif // _DEBUG
//
//
//std::unique_ptr<ParticleEffectManager> ParticleEffectManager::instance_ = nullptr;
//
//// ParticleEffectConfig JSON変換
//nlohmann::json ParticleEffectConfig::toJson() const {
//    return nlohmann::json{
//        {"name", name},
//        {"texturePath", texturePath},
//        {"position", {position.x, position.y, position.z}},
//        {"maxParticles", maxParticles},
//        {"behaviorName", behaviorName},
//        {"lifeTime", lifeTime},
//        {"velocity", {velocity.x, velocity.y, velocity.z}},
//        {"acceleration", {acceleration.x, acceleration.y, acceleration.z}},
//        {"scale", scale},
//        {"color", {color.x, color.y, color.z}},
//        {"emissionRate", emissionRate},
//        {"isActive", isActive}
//    };
//}
//
//ParticleEffectConfig ParticleEffectConfig::fromJson(const nlohmann::json& json) {
//    ParticleEffectConfig config;
//    config.name = json.value("name", "");
//    config.texturePath = json.value("texturePath", "");
//
//    auto pos = json.value("position", std::vector<float>{0.0f, 0.0f, 0.0f});
//    config.position = { pos[0], pos[1], pos[2] };
//
//    config.maxParticles = json.value("maxParticles", 100);
//    config.behaviorName = json.value("behaviorName", "Default");
//    config.lifeTime = json.value("lifeTime", 3.0f);
//
//    auto vel = json.value("velocity", std::vector<float>{0.0f, 1.0f, 0.0f});
//    config.velocity = { vel[0], vel[1], vel[2] };
//
//    auto acc = json.value("acceleration", std::vector<float>{0.0f, -0.5f, 0.0f});
//    config.acceleration = { acc[0], acc[1], acc[2] };
//
//    config.scale = json.value("scale", 1.0f);
//
//    auto col = json.value("color", std::vector<float>{1.0f, 1.0f, 1.0f});
//    config.color = { col[0], col[1], col[2] };
//
//    config.emissionRate = json.value("emissionRate", 10.0f);
//    config.isActive = json.value("isActive", true);
//
//    return config;
//}
//
//ParticleEffectManager* ParticleEffectManager::GetInstance() {
//    if (!instance_) {
//        instance_ = std::make_unique<ParticleEffectManager>();
//    }
//    return instance_.get();
//}
//
//void ParticleEffectManager::Initialize(SrvManager* srvManager) {
//    srvManager_ = srvManager;
//
//    // プリミティブメッシュの作成
//    planeMesh_ = MeshPrimitive::Createplane(1.0f, 1.0f);
//
//    // ParticleManagerの初期化
//    ParticleManager::GetInstance()->Initialize(srvManager_);
//
//    // プリセットエフェクトの作成
//    CreatePresetEffects();
//}
//
//void ParticleEffectManager::CreateEffect(const std::string& effectName, const ParticleEffectConfig& config) {
//    // パーティクルグループの設定
//    SetupParticleGroup(effectName, config.texturePath);
//
//    // エミッターの作成
//    auto emitter = std::make_unique<ParticleEmitter>(effectName, config.position, config.maxParticles);
//    emitter->Initialize(config.behaviorName);
//
//    // 設定を適用
//    ApplyConfigToEmitter(emitter.get(), config);
//
//    // 管理マップに追加
//    emitters_[effectName] = std::move(emitter);
//    effectConfigs_[effectName] = config;
//}
//
//void ParticleEffectManager::RemoveEffect(const std::string& effectName) {
//    emitters_.erase(effectName);
//    effectConfigs_.erase(effectName);
//}
//
//void ParticleEffectManager::UpdateEffect(const std::string& effectName, const ParticleEffectConfig& config) {
//    auto it = emitters_.find(effectName);
//    if (it != emitters_.end()) {
//        ApplyConfigToEmitter(it->second.get(), config);
//        effectConfigs_[effectName] = config;
//    }
//}
//
//void ParticleEffectManager::PlayEffect(const std::string& effectName, const Vector3& position) {
//    //auto it = emitters_.find(effectName);
//    //if (it != emitters_.end()) {
//    //    it->second->SetPosition(position);
//    //    it->second->SetActive(true);
//    //}
//}
//
//void ParticleEffectManager::StopEffect(const std::string& effectName) {
//    //auto it = emitters_.find(effectName);
//    //if (it != emitters_.end()) {
//    //    it->second->SetActive(false);
//    //}
//}
//
//void ParticleEffectManager::PauseEffect(const std::string& effectName) {
//    //auto it = emitters_.find(effectName);
//    //if (it != emitters_.end()) {
//    //    it->second->SetPaused(true);
//    //}
//}
//
//void ParticleEffectManager::ResumeEffect(const std::string& effectName) {
//    //auto it = emitters_.find(effectName);
//    //if (it != emitters_.end()) {
//    //    it->second->SetPaused(false);
//    //}
//}
//
//void ParticleEffectManager::Update() {
//    //for (auto& [name, emitter] : emitters_) {
//    //    if (emitter && effectConfigs_[name].isActive) {
//    //        emitter->Update();
//    //    }
//    //}
//}
//
//void ParticleEffectManager::SaveEffectsToJson(const std::string& filePath) {
//    nlohmann::json json;
//    json["effects"] = nlohmann::json::array();
//
//    for (const auto& [name, config] : effectConfigs_) {
//        json["effects"].push_back(config.toJson());
//    }
//
//    std::ofstream file(filePath);
//    if (file.is_open()) {
//        file << json.dump(4);
//        file.close();
//    }
//}
//
//void ParticleEffectManager::LoadEffectsFromJson(const std::string& filePath) {
//    if (!std::filesystem::exists(filePath)) {
//        return;
//    }
//
//    std::ifstream file(filePath);
//    if (!file.is_open()) {
//        return;
//    }
//
//    nlohmann::json json;
//    file >> json;
//    file.close();
//
//    if (json.contains("effects")) {
//        for (const auto& effectJson : json["effects"]) {
//            auto config = ParticleEffectConfig::fromJson(effectJson);
//            CreateEffect(config.name, config);
//        }
//    }
//}
//
//void ParticleEffectManager::ShowImGuiEditor() {
//    if (ImGui::Begin("Particle Effect Manager")) {
//        // タブ表示
//        if (ImGui::BeginTabBar("ParticleEffectTabs")) {
//            if (ImGui::BeginTabItem("Effect List")) {
//                ShowEffectListImGui();
//                ImGui::EndTabItem();
//            }
//
//            if (ImGui::BeginTabItem("Effect Editor")) {
//                ShowEffectEditorImGui();
//                ImGui::EndTabItem();
//            }
//
//            if (ImGui::BeginTabItem("Presets")) {
//                ShowPresetEffectsImGui();
//                ImGui::EndTabItem();
//            }
//
//            ImGui::EndTabBar();
//        }
//    }
//    ImGui::End();
//}
//
//void ParticleEffectManager::CreatePresetEffects() {
//    // 成長エフェクト
//    ParticleEffectConfig growthConfig;
//    growthConfig.name = "GrowthEffect";
//    growthConfig.texturePath = "Resources/textures/growth.png";
//    growthConfig.position = { 0.0f, 0.0f, 0.0f };
//    growthConfig.maxParticles = 50;
//    growthConfig.behaviorName = "Growth";
//    growthConfig.lifeTime = 2.0f;
//    growthConfig.velocity = { 0.0f, 2.0f, 0.0f };
//    growthConfig.acceleration = { 0.0f, -1.0f, 0.0f };
//    growthConfig.scale = 0.5f;
//    growthConfig.color = { 0.2f, 1.0f, 0.2f };
//    growthConfig.emissionRate = 15.0f;
//
//    // よだれエフェクト
//    ParticleEffectConfig yodareConfig;
//    yodareConfig.name = "YodareEffect";
//    yodareConfig.texturePath = "Resources/textures/yodare.png";
//    yodareConfig.position = { 0.0f, 0.0f, 0.0f };
//    yodareConfig.maxParticles = 30;
//    yodareConfig.behaviorName = "Yodare";
//    yodareConfig.lifeTime = 4.0f;
//    yodareConfig.velocity = { 0.0f, -0.5f, 0.0f };
//    yodareConfig.acceleration = { 0.0f, -2.0f, 0.0f };
//    yodareConfig.scale = 0.3f;
//    yodareConfig.color = { 0.8f, 0.8f, 1.0f };
//    yodareConfig.emissionRate = 8.0f;
//
//    // タイトルエフェクト
//    ParticleEffectConfig titleConfig;
//    titleConfig.name = "TitleEffect";
//    titleConfig.texturePath = "Resources/textures/circle2.png";
//    titleConfig.position = { 0.0f, 0.0f, 0.0f };
//    titleConfig.maxParticles = 100;
//    titleConfig.behaviorName = "Title";
//    titleConfig.lifeTime = 5.0f;
//    titleConfig.velocity = { 0.0f, 0.5f, 0.0f };
//    titleConfig.acceleration = { 0.0f, 0.0f, 0.0f };
//    titleConfig.scale = 1.0f;
//    titleConfig.color = { 1.0f, 1.0f, 0.2f };
//    titleConfig.emissionRate = 20.0f;
//
//    CreateEffect("GrowthEffect", growthConfig);
//    CreateEffect("YodareEffect", yodareConfig);
//    CreateEffect("TitleEffect", titleConfig);
//}
//
//bool ParticleEffectManager::IsEffectActive(const std::string& effectName) const {
//    auto it = effectConfigs_.find(effectName);
//    return it != effectConfigs_.end() && it->second.isActive;
//}
//
//void ParticleEffectManager::SetupParticleGroup(const std::string& groupName, const std::string& texturePath) {
//    ParticleManager::GetInstance()->CreateParticleGroup(groupName, texturePath);
//    ParticleManager::GetInstance()->SetPrimitiveMesh(groupName, planeMesh_);
//}
//
//void ParticleEffectManager::ApplyConfigToEmitter(ParticleEmitter* emitter, const ParticleEffectConfig& config) {
//    if (!emitter) return;
//
//}
//
//void ParticleEffectManager::ShowEffectListImGui() {
//    ImGui::Text("Active Effects: %zu", emitters_.size());
//    ImGui::Separator();
//
//    // 新しいエフェクト作成
//    ImGui::InputText("New Effect Name", newEffectName_, sizeof(newEffectName_));
//    if (ImGui::Button("Create New Effect")) {
//        if (strlen(newEffectName_) > 0) {
//            ParticleEffectConfig config;
//            config.name = newEffectName_;
//            config.texturePath = "Resources/textures/circle2.png";
//            CreateEffect(newEffectName_, config);
//            memset(newEffectName_, 0, sizeof(newEffectName_));
//        }
//    }
//
//    ImGui::Separator();
//
//    // エフェクト一覧
//    for (auto& [name, config] : effectConfigs_) {
//        ImGui::PushID(name.c_str());
//
//        bool isActive = config.isActive;
//        if (ImGui::Checkbox(("##" + name).c_str(), &isActive)) {
//            config.isActive = isActive;
//            UpdateEffect(name, config);
//        }
//
//        ImGui::SameLine();
//        if (ImGui::Button(name.c_str())) {
//            strcpy_s(selectedEffectName_, name.c_str());
//        }
//
//        ImGui::SameLine();
//        if (ImGui::Button(("Play##" + name).c_str())) {
//            PlayEffect(name, config.position);
//        }
//
//        ImGui::SameLine();
//        if (ImGui::Button(("Stop##" + name).c_str())) {
//            StopEffect(name);
//        }
//
//        ImGui::SameLine();
//        if (ImGui::Button(("Remove##" + name).c_str())) {
//            RemoveEffect(name);
//        }
//
//        ImGui::PopID();
//    }
//
//    ImGui::Separator();
//
//    // ファイル操作
//    if (ImGui::Button("Save All Effects")) {
//        SaveEffectsToJson("Resources/data/particle_effects.json");
//    }
//
//    ImGui::SameLine();
//    if (ImGui::Button("Load Effects")) {
//        LoadEffectsFromJson("Resources/data/particle_effects.json");
//    }
//}
//
//void ParticleEffectManager::ShowEffectEditorImGui() {
//    ImGui::Text("Selected Effect: %s", selectedEffectName_);
//
//    if (strlen(selectedEffectName_) == 0) {
//        ImGui::Text("Select an effect from the Effect List tab");
//        return;
//    }
//
//    auto it = effectConfigs_.find(selectedEffectName_);
//    if (it == effectConfigs_.end()) {
//        ImGui::Text("Effect not found");
//        return;
//    }
//
//    ParticleEffectConfig& config = it->second;
//    bool configChanged = false;
//
//    // 基本設定
//    if (ImGui::CollapsingHeader("Basic Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
//        configChanged |= ImGui::InputText("Texture Path", &config.texturePath[0], config.texturePath.capacity());
//        configChanged |= ImGui::InputInt("Max Particles", &config.maxParticles);
//        configChanged |= ImGui::InputText("Behavior Name", &config.behaviorName[0], config.behaviorName.capacity());
//        configChanged |= ImGui::Checkbox("Active", &config.isActive);
//    }
//
//    // 位置設定
//    if (ImGui::CollapsingHeader("Position", ImGuiTreeNodeFlags_DefaultOpen)) {
//        configChanged |= ImGui::DragFloat3("Position", &config.position.x, 0.1f);
//    }
//
//    // パーティクル設定
//    if (ImGui::CollapsingHeader("Particle Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
//        configChanged |= ImGui::DragFloat("Life Time", &config.lifeTime, 0.1f, 0.1f, 20.0f);
//        configChanged |= ImGui::DragFloat3("Velocity", &config.velocity.x, 0.1f);
//        configChanged |= ImGui::DragFloat3("Acceleration", &config.acceleration.x, 0.1f);
//        configChanged |= ImGui::DragFloat("Scale", &config.scale, 0.01f, 0.01f, 5.0f);
//        configChanged |= ImGui::ColorEdit3("Color", &config.color.x);
//        configChanged |= ImGui::DragFloat("Emission Rate", &config.emissionRate, 0.1f, 0.1f, 100.0f);
//    }
//
//    if (configChanged) {
//        UpdateEffect(selectedEffectName_, config);
//    }
//
//    // テスト再生
//    if (ImGui::Button("Test Play")) {
//        PlayEffect(selectedEffectName_, config.position);
//    }
//}
//
//void ParticleEffectManager::ShowPresetEffectsImGui() {
//    ImGui::Text("Preset Particle Effects");
//    ImGui::Separator();
//
//    if (ImGui::Button("Create Growth Effect")) {
//        CreatePresetEffects();
//    }
//
//    ImGui::Text("Available Presets:");
//    ImGui::BulletText("Growth Effect - Green upward particles");
//    ImGui::BulletText("Yodare Effect - Blue downward droplets");
//    ImGui::BulletText("Title Effect - Yellow floating circles");
//}