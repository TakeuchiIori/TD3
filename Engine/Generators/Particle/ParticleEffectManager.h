//
//#pragma once
//#include <memory>
//#include <unordered_map>
//#include <string>
//#include <vector>
//#include <fstream>
//#include "ParticleEmitter.h"
//#include "ParticleManager.h"
//#include "Loaders/Model/Mesh/MeshPrimitive.h"
//#include "Vector3.h"
//#include "json.hpp"
//
//// パーティクル設定構造体
//struct ParticleEffectConfig {
//    std::string name;
//    std::string texturePath;
//    Vector3 position;
//    int maxParticles;
//    std::string behaviorName;
//
//    // パーティクル設定
//    float lifeTime = 3.0f;
//    Vector3 velocity = { 0.0f, 1.0f, 0.0f };
//    Vector3 acceleration = { 0.0f, -0.5f, 0.0f };
//    float scale = 1.0f;
//    Vector3 color = { 1.0f, 1.0f, 1.0f };
//    float emissionRate = 10.0f;
//    bool isActive = true;
//
//    // JSON変換
//    nlohmann::json toJson() const;
//    static ParticleEffectConfig fromJson(const nlohmann::json& json);
//};
//
//// パーティクルエフェクト管理クラス
//class ParticleEffectManager {
//public:
//    static ParticleEffectManager* GetInstance();
//
//    // 初期化
//    void Initialize(SrvManager* srvManager);
//
//    // エフェクト作成・管理
//    void CreateEffect(const std::string& effectName, const ParticleEffectConfig& config);
//    void RemoveEffect(const std::string& effectName);
//    void UpdateEffect(const std::string& effectName, const ParticleEffectConfig& config);
//
//    // エフェクト制御
//    void PlayEffect(const std::string& effectName, const Vector3& position);
//    void StopEffect(const std::string& effectName);
//    void PauseEffect(const std::string& effectName);
//    void ResumeEffect(const std::string& effectName);
//
//    // 更新
//    void Update();
//
//    // JSON設定の保存・読み込み
//    void SaveEffectsToJson(const std::string& filePath);
//    void LoadEffectsFromJson(const std::string& filePath);
//
//    // ImGui設定画面
//    void ShowImGuiEditor();
//
//    // プリセット管理
//    void CreatePresetEffects();
//
//    // ゲッター
//    const std::unordered_map<std::string, ParticleEffectConfig>& GetEffectConfigs() const { return effectConfigs_; }
//    bool IsEffectActive(const std::string& effectName) const;
//
//private:
//    ParticleEffectManager() = default;
//    ~ParticleEffectManager() = default;
//    ParticleEffectManager(const ParticleEffectManager&) = delete;
//    ParticleEffectManager& operator=(const ParticleEffectManager&) = delete;
//
//    // 内部データ
//    static std::unique_ptr<ParticleEffectManager> instance_;
//    SrvManager* srvManager_ = nullptr;
//
//    // エフェクト管理
//    std::unordered_map<std::string, std::unique_ptr<ParticleEmitter>> emitters_;
//    std::unordered_map<std::string, ParticleEffectConfig> effectConfigs_;
//
//    // プリミティブメッシュ
//    std::shared_ptr<Mesh> planeMesh_;
//
//    // ImGui用
//    char newEffectName_[256] = "";
//    char selectedEffectName_[256] = "";
//    bool showEffectEditor_ = false;
//
//    // 内部関数
//    void SetupParticleGroup(const std::string& groupName, const std::string& texturePath);
//    void ApplyConfigToEmitter(ParticleEmitter* emitter, const ParticleEffectConfig& config);
//    void ShowEffectListImGui();
//    void ShowEffectEditorImGui();
//    void ShowPresetEffectsImGui();
//};