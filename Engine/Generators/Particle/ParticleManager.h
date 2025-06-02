#pragma once

// C++
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include <dxcapi.h>
#include <string>
#include <vector>
#include <random>
#include <list>
#include <unordered_map>
#include <functional>

// Math
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Mathfunc.h"

// Engine
#include "Systems./Camera/Camera.h"
#include "PipelineManager/PipelineManager.h"
#include "Loaders/Model/Mesh/Mesh.h"
#include "Loaders/Model/Mesh/MeshPrimitive.h"
#include "Loaders/Json/JsonManager.h"
#include "Loaders/Json/EnumUtils.h"

// 前方宣言
class DirectXCommon;
class SrvManager;

class ParticleManager {
public:
    // enum定義はEnumUtils.hのものを使用
    using ForceType = ParticleManagerEnums::ForceType;
    using EmissionType = ParticleManagerEnums::EmissionType;
    using ColorChangeType = ParticleManagerEnums::ColorChangeType;
    using ScaleChangeType = ParticleManagerEnums::ScaleChangeType;
    using RotationType = ParticleManagerEnums::RotationType;
    using MovementType = ParticleManagerEnums::MovementType;

    // 基本構造体（既存のものを保持）
    struct Material {
        Vector4 color;
        int32_t enableLighting;
        float padding[3];
        Matrix4x4 uvTransform;
    };

    struct ParticleForGPU {
        Matrix4x4 WVP;
        Matrix4x4 World;
        Vector4 color;
    };

    struct MaterialData {
        std::string textureFilePath;
        uint32_t textureIndexSRV = 0;
    };

    struct ModelData {
        MaterialData material;
    };

    // 拡張力場構造体
    struct ForceField {
        ForceType type = ForceType::None;
        Vector3 position = { 0.0f, 0.0f, 0.0f };
        Vector3 direction = { 0.0f, -1.0f, 0.0f };
        float strength = 1.0f;
        float range = 10.0f;
        float falloffStart = 0.0f;      // 減衰開始距離
        float falloffEnd = 10.0f;       // 減衰終了距離
        bool enabled = true;

        // 特殊パラメータ
        float frequency = 1.0f;         // 周波数（乱流、波動用）
        float amplitude = 1.0f;         // 振幅
        Vector3 axis = { 0.0f, 1.0f, 0.0f }; // 回転軸（渦用）
        float innerRadius = 0.0f;       // 内半径
        float outerRadius = 5.0f;       // 外半径
        Vector3 windVariation = { 0.0f, 0.0f, 0.0f }; // 風の変動
    };

    // 衝突設定
    struct CollisionSettings {
        bool enabled = false;
        float bounciness = 0.5f;        // 反発係数
        float friction = 0.1f;          // 摩擦係数
        bool killOnCollision = false;   // 衝突時消滅
        bool stickOnCollision = false;  // 衝突時固着
        Vector3 groundLevel = { 0.0f, -10.0f, 0.0f }; // 地面レベル
        bool hasGroundCollision = false; // 地面衝突有効
    };

    // 拡張パーティクル構造体
    struct Particle {
        EulerTransform transform;
        Vector3 velocity;
        Vector3 acceleration = { 0.0f, 0.0f, 0.0f };
        Vector4 color;
        float lifeTime;
        float currentTime;

        // 🆕 拡張プロパティ
        Vector3 initialPosition;
        Vector3 initialVelocity;
        Vector4 initialColor;
        Vector3 initialScale;
        float mass = 1.0f;
        float drag = 0.0f;
        Vector3 angularVelocity = { 0.0f, 0.0f, 0.0f };
        bool hasCollided = false;
        float age = 0.0f;               // 正規化された年齢 (0.0 - 1.0)
        int textureFrame = 0;           // テクスチャフレーム
        float customFloat1 = 0.0f;      // カスタムパラメータ
        float customFloat2 = 0.0f;
        Vector3 customVector = { 0.0f, 0.0f, 0.0f };
    };

    struct ParticleGroup {
        MaterialData materialData;
        std::list<Particle> particles;
        uint32_t srvIndex;
        Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
        UINT instance;
        ParticleForGPU* instancingDataForGPU;

        BlendMode blendMode = BlendMode::kBlendModeAdd;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

        std::shared_ptr<Mesh> mesh = nullptr;
        std::vector<ParticleForGPU> instancingData;

        Vector2 uvScale = { 1.0f, 1.0f };
        Vector2 uvTranslate = { 0.0f, 0.0f };
        float uvRotate = 0.0f;
        bool uvAnimationEnable = false;
        float uvAnimSpeedX = 1.0f;
        float uvAnimSpeedY = 0.0f;

        // 🆕 テクスチャアニメーション
        int textureSheetX = 1;          // テクスチャシートX分割数
        int textureSheetY = 1;          // テクスチャシートY分割数
        float textureAnimSpeed = 1.0f;  // アニメーション速度
        bool textureAnimLoop = true;    // ループ再生
    };

    // 既存の設定構造体
    struct ParticleTransformSettings {
        Vector3 scaleMin;
        Vector3 scaleMax;
        Vector3 translateMin;
        Vector3 translateMax;
        Vector3 rotateMin;
        Vector3 rotateMax;
    };

    struct ParticleVelocitySettings {
        Vector3 velocityMin;
        Vector3 velocityMax;
    };

    struct ParticleColorSettings {
        Vector3 minColor;
        Vector3 maxColor;
        float alpha;
    };

    struct ParticleLifeSettings {
        Vector2 lifeTime;
    };

    // カラーアニメーション設定
    struct ColorAnimationSettings {
        ColorChangeType type = ColorChangeType::None;
        Vector4 startColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        Vector4 midColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        Vector4 endColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        float midPoint = 0.5f;          // 中間点の位置(0.0-1.0)
        float flashFrequency = 1.0f;    // 点滅頻度
        float rainbowSpeed = 1.0f;      // 虹色変化速度
        bool smoothTransition = true;   // 滑らかな遷移
    };

    // スケールアニメーション設定
    struct ScaleAnimationSettings {
        ScaleChangeType type = ScaleChangeType::None;
        Vector3 startScale = { 1.0f, 1.0f, 1.0f };
        Vector3 midScale = { 1.0f, 1.0f, 1.0f };
        Vector3 endScale = { 1.0f, 1.0f, 1.0f };
        float midPoint = 0.5f;
        float pulseFrequency = 1.0f;    // 脈動頻度
        float stretchFactor = 1.0f;     // 引き伸ばし係数
        bool uniformScale = true;       // 均等スケール
    };

    // 回転アニメーション設定
    struct RotationAnimationSettings {
        RotationType type = RotationType::None;
        Vector3 rotationSpeed = { 0.0f, 0.0f, 0.0f };
        Vector3 rotationAcceleration = { 0.0f, 0.0f, 0.0f };
        float randomMultiplier = 1.0f;  // ランダム回転の倍率
        bool faceVelocity = false;      // 速度方向を向く
        Vector3 faceDirection = { 0.0f, 0.0f, 1.0f }; // 向く方向
    };

    // 移動パターン設定
    struct MovementSettings {
        MovementType type = MovementType::Linear;
        float curveStrength = 1.0f;     // 曲線の強さ
        float spiralRadius = 1.0f;      // 螺旋半径
        float spiralSpeed = 1.0f;       // 螺旋速度
        Vector3 waveAmplitude = { 0.0f, 1.0f, 0.0f }; // 波の振幅
        float waveFrequency = 1.0f;     // 波の周波数
        float bounceHeight = 1.0f;      // バウンス高さ
        Vector3 orbitCenter = { 0.0f, 0.0f, 0.0f }; // 軌道中心
        float orbitRadius = 1.0f;       // 軌道半径
        float zigzagAngle = 45.0f;      // ジグザグ角度
    };

    // ノイズ設定
    struct NoiseSettings {
        bool enabled = false;
        float strength = 1.0f;
        float frequency = 1.0f;
        Vector3 scrollSpeed = { 0.0f, 0.0f, 0.0f };
        float timeScale = 1.0f;
        int octaves = 3;
        float persistence = 0.5f;
        bool affectPosition = true;     // 位置に影響
        bool affectVelocity = false;    // 速度に影響
        bool affectColor = false;       // 色に影響
        bool affectScale = false;       // スケールに影響
    };

    // 発生形状設定
    struct EmissionShapeSettings {
        EmissionType type = EmissionType::Point;
        Vector3 size = { 1.0f, 1.0f, 1.0f };        // 形状サイズ
        float radius = 1.0f;                       // 半径
        float innerRadius = 0.0f;                  // 内半径
        float height = 1.0f;                       // 高さ
        float angle = 30.0f;                       // コーン角度
        Vector3 direction = { 0.0f, 1.0f, 0.0f };   // 方向
        bool shellEmission = false;                // 表面のみ
        bool edgeEmission = false;                 // エッジのみ
        float edgeThickness = 0.1f;               // エッジ厚み
    };

    // 物理設定
    struct PhysicsSettings {
        bool enabled = false;
        Vector3 gravity = { 0.0f, -9.8f, 0.0f };
        float mass = 1.0f;
        float drag = 0.0f;                        // 空気抵抗
        float angularDrag = 0.0f;                 // 角度抵抗
        float elasticity = 0.0f;                  // 弾性
        float magnetism = 0.0f;                   // 磁力
        Vector3 magneticField = { 0.0f, 0.0f, 0.0f }; // 磁場方向
        bool useComplexPhysics = false;           // 複雑な物理計算
    };

    // 軌跡設定
    struct TrailSettings {
        bool enabled = false;
        int maxLength = 10;                       // 最大軌跡長
        float width = 0.1f;                       // 軌跡幅
        float fadeSpeed = 1.0f;                   // フェード速度
        Vector4 trailColor = { 1.0f, 1.0f, 1.0f, 0.5f }; // 軌跡色
        bool inheritParticleColor = true;         // パーティクル色継承
        float minVelocity = 0.1f;                 // 軌跡生成最小速度
    };

    // LOD設定
    struct LODSettings {
        bool enabled = false;
        float nearDistance = 10.0f;               // 近距離
        float farDistance = 100.0f;               // 遠距離
        int maxParticlesNear = 1000;              // 近距離最大パーティクル数
        int maxParticlesFar = 100;                // 遠距離最大パーティクル数
        float cullingDistance = 200.0f;           // カリング距離
        bool dynamicLOD = true;                   // 動的LOD
    };

    struct RadialEmissionSettings {
        bool enabled = false;                           // 放射機能を有効にするか
        bool fromCenter = true;                         // 中心から外へ 
        Vector3 centerPosition = { 0.0f, 0.0f, 0.0f };  // 放射の中心点
        float minRadius = 0.0f;                         // 最小半径
        float maxRadius = 5.0f;                         // 最大半径
        float uniformSpeed = 1.0f;                      // 均等な放射速度
        bool useUniformSpeed = true;                    // 均等速度を使用するか
        float angleVariation = 0.0f;                    // 角度のばらつき（度）
        bool limitToHemisphere = false;                 // 半球に制限するか
        Vector3 hemisphereUp = { 0.0f, 1.0f, 0.0f };    // 半球の上方向
        float convergenceForce = 1.0f;                  // 収束力（外から中心へ向かう場合）
        bool maintainDistance = false;                  // 距離を維持するか（軌道運動）
    };

    // 全体のパーティクル設定構造体
    struct ParticleParameters {
        // 既存の基本設定
        ParticleTransformSettings baseTransform;
        ParticleVelocitySettings baseVelocity;
        ParticleColorSettings baseColor;
        ParticleLifeSettings baseLife;

        // 新しい拡張設定
        EmissionShapeSettings emissionShape;
        ColorAnimationSettings colorAnimation;
        ScaleAnimationSettings scaleAnimation;
        RotationAnimationSettings rotationAnimation;
        MovementSettings movement;
        PhysicsSettings physics;
        NoiseSettings noise;
        CollisionSettings collision;
        TrailSettings trail;
        LODSettings lod;
        RadialEmissionSettings radialEmission;
        // 力場（複数設定可能）
        std::vector<ForceField> forceFields;

        // 既存のフラグ類
        bool useBillboard = true;
        bool isRandom = true;
        bool randomFromCenter = false;
        Vector3 randomDirectionMin = { -1.0f, -1.0f, -1.0f };
        Vector3 randomDirectionMax = { 1.0f, 1.0f, 1.0f };
        float randomForce = 0.02f;
        Vector3 offset = {};
        bool enableScale = false;
        bool isRandomRotate = false;
        bool isRandomScale = false;
        Vector2 minmaxScale = { 0.0f, 1.0f };
        Vector3 direction = { 1.0f, 1.0f, 0.0f };
        bool isUnRandomSpeed = false;
        bool isRotateDirection = false;
        float speed = 1.0f;

        // 🆕 追加設定
        float spawnRate = 1.0f;                   // 生成レート
        bool burstMode = false;                   // バーストモード
        int burstCount = 10;                      // バースト数
        float burstInterval = 1.0f;               // バースト間隔
        bool looping = true;                      // ループ再生
        float duration = 5.0f;                    // 持続時間
        float startDelay = 0.0f;                  // 開始遅延

        // パフォーマンス設定
        int maxParticles = 1000;                  // 最大パーティクル数
        bool sortParticles = false;               // パーティクルソート
        bool frustumCulling = true;               // フラスタムカリング

        // デバッグ設定
        bool showDebugInfo = false;               // デバッグ情報表示
        Vector4 debugColor = { 1.0f, 0.0f, 0.0f, 1.0f };    // デバッグ色

    };

    struct AccelerationField {
        Vector3 acceleration;
        AABB area;
    };

public: // シングルトン
    static ParticleManager* GetInstance();
    void Finalize();
    ParticleManager() = default;
    ~ParticleManager() = default;
    ParticleManager(const ParticleManager&) = delete;
    ParticleManager& operator=(const ParticleManager&) = delete;

public: // メンバ関数
    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize(SrvManager* srvManager);

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    /// <summary>
    /// 描画
    /// </summary>
    void Draw();

    /// <summary>
    /// パーティクルグループ生成
    /// </summary>
    /// <param name="name"></param>
    /// <param name="textureFilePath"></param>
    void CreateParticleGroup(const std::string name, const std::string textureFilePath);

    /// <summary>
    /// パーティクルの発生
    /// </summary>
    /// <param name="name"></param>
    /// <param name="position"></param>
    /// <param name="count"></param>
    std::list<Particle> Emit(const std::string& name, const Vector3& position, uint32_t count);

    /// <summary>
    ///  マネージャ全体のデフォルトメッシュを設定  
    ///  指定しないグループはこれを使う
    /// </summary>
    void SetDefaultPrimitiveMesh(const std::shared_ptr<Mesh>& mesh);

    /// <summary>
    ///  指定したパーティクル グループ専用のメッシュを設定
    /// </summary>
    void SetPrimitiveMesh(const std::string& groupName, const std::shared_ptr<Mesh>& mesh);

    /// <summary>
    /// パラメータ参照取得
    /// </summary>
    ParticleParameters& GetParameters(const std::string& name) { return particleParameters_[name]; }

    // 🆕 新しい便利メソッド
    /// <summary>
    /// 力場を追加
    /// </summary>
    void AddForceField(const std::string& groupName, const ForceField& field);

    /// <summary>
    /// エフェクトプリセットを適用（簡易版）
    /// </summary>
    void ApplyPreset(const std::string& groupName, const std::string& presetName);

    /// <summary>
    /// バーストエミッション
    /// </summary>
    void EmitBurst(const std::string& groupName, const Vector3& position, int count);

    /// <summary>
    /// パフォーマンス統計取得
    /// </summary>
    struct PerformanceInfo {
        int totalParticles = 0;
        int activeGroups = 0;
        float updateTime = 0.0f;
        float renderTime = 0.0f;
    };
    PerformanceInfo GetPerformanceInfo() const;

private:
    void InitJson(const std::string& name);
    void UpdateRadialParticle(Particle& particle, const ParticleParameters& params, float deltaTime);
    void UpdateRadialDynamicParameters(Particle& particle, const ParticleParameters& params);
    void UpdateParticles();
    void CreateMaterialResource();
    Particle MakeNewParticle(const std::string& name, std::mt19937& randomEngine, const Vector3& position);

    // 🆕 新しい内部処理メソッド
    void UpdateParticleForces(Particle& particle, const ParticleParameters& params, float deltaTime);
    void UpdateParticleMovement(Particle& particle, const ParticleParameters& params, float deltaTime);
    void UpdateParticleColor(Particle& particle, const ParticleParameters& params);
    void UpdateParticleScale(Particle& particle, const ParticleParameters& params);
    void UpdateParticleRotation(Particle& particle, const ParticleParameters& params, float deltaTime);
    void UpdateParticlePhysics(Particle& particle, const ParticleParameters& params, float deltaTime);
    void UpdateParticleCollision(Particle& particle, const ParticleParameters& params);
    void ApplyNoise(Particle& particle, const NoiseSettings& noise, float deltaTime);
    Vector3 SampleEmissionShape(const EmissionShapeSettings& shape, std::mt19937& rng);
    float PerlinNoise(const Vector3& position, float frequency);
    Vector3 GenerateRadialDirection(const RadialEmissionSettings& radial, std::mt19937& rng);
public:
    void SetCamera(Camera* camera) { camera_ = camera; }

private: // メンバ変数
    // シングルトン
    static std::unique_ptr<ParticleManager> instance;
    static std::once_flag initInstanceFlag;

    // ポインタ
    DirectXCommon* dxCommon_ = nullptr;
    SrvManager* srvManager_ = nullptr;
    Material* materialData_ = nullptr;
    Camera* camera_ = nullptr;
    std::shared_ptr<Mesh> defaultMesh_ = nullptr;

    std::vector<ParticleForGPU> instancingData_;
    std::unordered_map<std::string, std::unique_ptr<JsonManager>> jsonManagers_;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

    D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
    D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;

    // SRV切り替え
    bool useTexture = true;
    bool particleUpdate = false;
    bool useBillboard = true;

    AccelerationField accelerationField;

    ModelData modelData_;
    std::random_device seedGenerator_;
    std::mt19937 randomEngine_ = std::mt19937(seedGenerator_());
    BlendMode currentBlendMode_;
    std::unordered_map<std::string, ParticleGroup> particleGroups_;
    std::unordered_map<std::string, ParticleParameters> particleParameters_;
    const float kDeltaTime = 1.0f / 60.0f;
    const uint32_t kNumMaxInstance = 10000;
    std::unordered_map<BlendMode, Microsoft::WRL::ComPtr<ID3D12PipelineState>> pipelineStates_;

    Matrix4x4 scaleMatrix;
    Matrix4x4 translateMatrix;

    // 🆕 パフォーマンス追跡
    mutable PerformanceInfo performanceInfo_;
    std::unordered_map<std::string, std::vector<Vector3>> particleTrails_;
};