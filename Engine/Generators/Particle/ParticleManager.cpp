#include "ParticleManager.h"

// Engine
#include "DX./DirectXCommon.h"
#include "SrvManager./SrvManager.h"
#include "loaders./Texture./TextureManager.h"
#include "WinApp./WinApp.h"
#include "Debugger/Logger.h"

// C++
#include <numbers>
#include <cassert>
#include <algorithm>

#ifdef _DEBUG
#include "imgui.h"
#endif

// シングルトンインスタンスの初期化
std::unique_ptr<ParticleManager> ParticleManager::instance = nullptr;
std::once_flag ParticleManager::initInstanceFlag;

// ドロップダウンメニュー用の文字列
const char* blendModeNames[] = {
	"None",
	"Normal",
	"Add",
	"Subtract",
	"Multiply",
	"Screen"
};

const char* forceTypeNames[] = {
	"None",
	"Gravity",
	"Wind",
	"Vortex",
	"Radial",
	"Turbulence",
	"Spring",
	"Damping",
	"Magnet"
};

const char* emissionTypeNames[] = {
	"Point",
	"Sphere",
	"Box",
	"Circle",
	"Ring",
	"Cone",
	"Line",
	"Hemisphere"
};

const char* colorChangeTypeNames[] = {
	"None",
	"Fade",
	"Gradient",
	"Flash",
	"Rainbow",
	"Fire",
	"Electric"
};

const char* scaleChangeTypeNames[] = {
	"None",
	"Shrink",
	"Grow",
	"Pulse",
	"Stretch"
};

const char* rotationTypeNames[] = {
	"None",
	"ConstantX",
	"ConstantY",
	"ConstantZ",
	"Random",
	"Velocity",
	"Tumble"
};

const char* movementTypeNames[] = {
	"Linear",
	"Curve",
	"Spiral",
	"Wave",
	"Bounce",
	"Orbit",
	"Zigzag"
};

ParticleManager* ParticleManager::GetInstance()
{
	std::call_once(initInstanceFlag, []() {
		instance = std::make_unique<ParticleManager>();
		});
	return instance.get();
}

void ParticleManager::Finalize()
{
	instance.reset();
}

void ParticleManager::Initialize(SrvManager* srvManager)
{
	// ポインタを渡す
	this->dxCommon_ = DirectXCommon::GetInstance();
	this->srvManager_ = srvManager;

	rootSignature_ = PipelineManager::GetInstance()->GetRootSignature("Particle");
	graphicsPipelineState_ = PipelineManager::GetInstance()->GetPipeLineStateObject("Particle");

	// マテリアルリソース作成
	CreateMaterialResource();
	instancingData_.resize(kNumMaxInstance);

	// パフォーマンス情報初期化
	performanceInfo_ = {};
}

/// <summary>
/// パーティクルの更新処理
/// </summary>
void ParticleManager::Update()
{
	UpdateParticles();
}

void ParticleManager::Draw()
{
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// すべてのパーティクルグループを描画
	for (auto& [groupName, particleGroup] : particleGroups_) {
		if (particleGroup.instance > 0) {
			auto mesh = particleGroup.mesh;
			if (!mesh) { continue; };

			// ★ UV変換行列を更新する
			const Vector2& scale = particleGroup.uvScale;
			const Vector2& offset = particleGroup.uvTranslate;
			float rotate = particleGroup.uvRotate;

			Matrix4x4 uvS = MakeScaleMatrix({ scale.x, scale.y, 1.0f });
			Matrix4x4 uvR = MakeRotateMatrixZ(rotate);
			Matrix4x4 uvT = MakeTranslateMatrix({ offset.x, offset.y, 0.0f });

			materialData_->uvTransform = Multiply(uvS, Multiply(uvR, uvT));

			dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &mesh->GetMeshResource().vertexBufferView);
			dxCommon_->GetCommandList()->IASetIndexBuffer(&mesh->GetMeshResource().indexBufferView);
			dxCommon_->GetCommandList()->SetPipelineState(
				PipelineManager::GetInstance()->GetBlendModePSO(particleGroup.blendMode)
			);

			// マテリアルCBufferの場所を指定
			dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
			// SRVのDescriptorTableを設定
			srvManager_->SetGraphicsRootDescriptorTable(1, particleGroup.srvIndex);
			// テクスチャのSRVのDescriptorTableを設定
			D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = TextureManager::GetInstance()->GetsrvHandleGPU(particleGroup.materialData.textureFilePath);
			srvManager_->SetGraphicsRootDescriptorTable(2, particleGroup.materialData.textureIndexSRV);
			// 描画
			dxCommon_->GetCommandList()->DrawIndexedInstanced(
				mesh->GetIndexCount(),          // Index数
				particleGroup.instance,          // インスタンス数
				0, 0, 0                          // StartIndex, BaseVertex, StartInstance
			);
		}
	}
}

/// <summary>
///  拡張されたパーティクル更新処理
/// </summary>
void ParticleManager::UpdateParticles()
{
	// カメラ行列計算
	Matrix4x4 view = camera_->viewMatrix_;
	Matrix4x4 proj = camera_->projectionMatrix_;
	Matrix4x4 vp = Multiply(view, proj);

	// ビルボード行列（カメラの回転のみを逆行列で取り出す）
	Matrix4x4 billboardMatrix = view;
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;
	billboardMatrix.m[3][3] = 1.0f;
	Matrix4x4 bbBase = Inverse(billboardMatrix);

	// パフォーマンス統計リセット
	performanceInfo_.totalParticles = 0;
	performanceInfo_.activeGroups = 0;

	// 各パーティクルグループを更新
	for (auto& [name, group] : particleGroups_) {
		if (group.particles.empty()) continue;

		performanceInfo_.activeGroups++;
		const ParticleParameters& params = particleParameters_[name];
		uint32_t instanceCnt = 0;

		// UVアニメーション更新
		if (group.uvAnimationEnable) {
			group.uvTranslate.x += group.uvAnimSpeedX * kDeltaTime;
			group.uvTranslate.y += group.uvAnimSpeedY * kDeltaTime;

			// ループさせる
			group.uvTranslate.x = std::fmod(group.uvTranslate.x, 1.0f);
			group.uvTranslate.y = std::fmod(group.uvTranslate.y, 1.0f);
		}

		// 個々のパーティクル更新
		for (auto it = group.particles.begin(); it != group.particles.end(); ) {
			Particle& particle = *it;

			// 寿命判定
			if (particle.currentTime >= particle.lifeTime) {
				it = group.particles.erase(it);
				continue;
			}

			particle.currentTime += kDeltaTime;
			particle.age = particle.currentTime / particle.lifeTime; // 正規化された年齢

			// 🌪️ 力場の影響
			UpdateParticleForces(particle, params, kDeltaTime);

			// 🏃 移動パターンの適用
			UpdateParticleMovement(particle, params, kDeltaTime);

			// 🎨 カラーアニメーション
			UpdateParticleColor(particle, params);

			// 📏 スケールアニメーション
			UpdateParticleScale(particle, params);

			// 🔄 回転アニメーション
			UpdateParticleRotation(particle, params, kDeltaTime);

			// 🎪 物理計算
			if (params.physics.enabled) {
				UpdateParticlePhysics(particle, params, kDeltaTime);
			}

			// 🎵 ノイズ適用
			if (params.noise.enabled) {
				ApplyNoise(particle, params.noise, kDeltaTime);
			}

			// 🎯 衝突判定
			if (params.collision.enabled) {
				UpdateParticleCollision(particle, params);
			}

			// 基本移動
			particle.transform.translate += particle.velocity * kDeltaTime;

			// テクスチャアニメーション
			if (group.textureSheetX > 1 || group.textureSheetY > 1) {
				int totalFrames = group.textureSheetX * group.textureSheetY;
				float animTime = particle.age * group.textureAnimSpeed;
				if (group.textureAnimLoop) {
					animTime = std::fmod(animTime, 1.0f);
				}
				particle.textureFrame = static_cast<int>(animTime * totalFrames) % totalFrames;
			}

			// 軌跡更新
			if (params.trail.enabled) {
				auto& trail = particleTrails_[name + "_" + std::to_string(particle.textureFrame)];
				trail.push_back(particle.transform.translate);
				if (trail.size() > static_cast<size_t>(params.trail.maxLength)) {
					trail.erase(trail.begin());
				}
			}

			// 従来のスケール縮小処理（後方互換性）
			if (params.enableScale && params.scaleAnimation.type == ScaleChangeType::None) {
				float alpha = 1.0f - particle.age;
				particle.transform.scale = particle.initialScale * alpha;
			}

			// 行列計算
			Matrix4x4 S = MakeScaleMatrix(particle.transform.scale);
			Matrix4x4 R = MakeRotateMatrixXYZ(particle.transform.rotate);
			Matrix4x4 T = MakeTranslateMatrix(particle.transform.translate + params.offset);

			Matrix4x4 world = params.useBillboard ? S * bbBase * T : S * R * T;
			Matrix4x4 wvp = Multiply(world, vp);

			// GPU へ書き込み
			if (instanceCnt < kNumMaxInstance) {
				group.instancingData[instanceCnt].WVP = wvp;
				group.instancingData[instanceCnt].World = world;
				group.instancingData[instanceCnt].color = particle.color;

				// 従来のアルファフェード（後方互換性）
				if (params.colorAnimation.type == ColorChangeType::None) {
					group.instancingData[instanceCnt].color.w = 1.0f - particle.age;
				}

				++instanceCnt;
			}
			++it;
		}

		performanceInfo_.totalParticles += static_cast<int>(group.particles.size());

		// インスタンス数更新 & 転送
		group.instance = instanceCnt;
		if (group.instancingDataForGPU && instanceCnt > 0) {
			std::memcpy(group.instancingDataForGPU,
				group.instancingData.data(),
				sizeof(ParticleForGPU) * instanceCnt);
		}
	}
}

void ParticleManager::CreateMaterialResource()
{
	// リソース作成
	materialResource_ = dxCommon_->CreateBufferResource(sizeof(Material));
	// データを書き込むためのアドレスを取得して割り当て
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// マテリアルデータの初期化
	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentity4x4();
}

// 🆕 拡張されたパーティクル生成
ParticleManager::Particle ParticleManager::MakeNewParticle(const std::string& name, std::mt19937& randomEngine, const Vector3& position)
{
	Particle particle;
	ParticleParameters& params = particleParameters_[name];

	auto getValue = [](float min, float max, bool isRandom, std::mt19937& rng) {
		if (isRandom) {
			if (min > max) std::swap(min, max);
			std::uniform_real_distribution<float> dist(min, max);
			return dist(rng);
		} else {
			return min;
		}
		};

	// 発生形状からの位置サンプリング
	Vector3 shapeOffset = SampleEmissionShape(params.emissionShape, randomEngine);

	// Transform設定
	particle.transform.scale = {
		getValue(params.baseTransform.scaleMin.x, params.baseTransform.scaleMax.x, params.isRandom, randomEngine),
		getValue(params.baseTransform.scaleMin.y, params.baseTransform.scaleMax.y, params.isRandom, randomEngine),
		getValue(params.baseTransform.scaleMin.z, params.baseTransform.scaleMax.z, params.isRandom, randomEngine)
	};

	particle.transform.rotate = {
		getValue(params.baseTransform.rotateMin.x, params.baseTransform.rotateMax.x, params.isRandom, randomEngine),
		getValue(params.baseTransform.rotateMin.y, params.baseTransform.rotateMax.y, params.isRandom, randomEngine),
		getValue(params.baseTransform.rotateMin.z, params.baseTransform.rotateMax.z, params.isRandom, randomEngine)
	};

	particle.transform.translate = position + shapeOffset + Vector3{
		getValue(params.baseTransform.translateMin.x, params.baseTransform.translateMax.x, params.isRandom, randomEngine),
		getValue(params.baseTransform.translateMin.y, params.baseTransform.translateMax.y, params.isRandom, randomEngine),
		getValue(params.baseTransform.translateMin.z, params.baseTransform.translateMax.z, params.isRandom, randomEngine)
	};

	// 初期値保存
	particle.initialPosition = particle.transform.translate;
	particle.initialScale = particle.transform.scale;

	// ランダム回転
	if (params.isRandomRotate) {
		std::uniform_real_distribution<float> distRotate(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);
		particle.transform.rotate.z = distRotate(randomEngine);
	}

	// ランダムスケール
	if (params.isRandomScale) {
		std::uniform_real_distribution<float> distScale(params.minmaxScale.x, params.minmaxScale.y);
		particle.transform.scale.y = distScale(randomEngine);
	}

	// Velocity設定
	if (params.randomFromCenter) {
		// 中心からランダム方向
		std::uniform_real_distribution<float>
			dx(params.randomDirectionMin.x, params.randomDirectionMax.x),
			dy(params.randomDirectionMin.y, params.randomDirectionMax.y),
			dz(params.randomDirectionMin.z, params.randomDirectionMax.z);

		Vector3 dir = Normalize(Vector3{ dx(randomEngine), dy(randomEngine), dz(randomEngine) });
		particle.velocity = dir * params.speed;
	} else {
		if (params.isUnRandomSpeed) {
			particle.velocity = {
				getValue(params.baseVelocity.velocityMin.x, params.baseVelocity.velocityMax.x, params.isRandom, randomEngine),
				getValue(params.baseVelocity.velocityMin.y, params.baseVelocity.velocityMax.y, params.isRandom, randomEngine),
				getValue(params.baseVelocity.velocityMin.z, params.baseVelocity.velocityMax.z, params.isRandom, randomEngine)
			};
		} else {
			Vector3 dir = Normalize(params.direction);
			particle.velocity = dir * params.speed;
		}
	}

	particle.initialVelocity = particle.velocity;

	// Color設定
	particle.color = {
		getValue(params.baseColor.minColor.x, params.baseColor.maxColor.x, params.isRandom, randomEngine),
		getValue(params.baseColor.minColor.y, params.baseColor.maxColor.y, params.isRandom, randomEngine),
		getValue(params.baseColor.minColor.z, params.baseColor.maxColor.z, params.isRandom, randomEngine),
		params.baseColor.alpha
	};
	particle.initialColor = particle.color;

	// LifeTime設定
	particle.lifeTime = getValue(params.baseLife.lifeTime.x, params.baseLife.lifeTime.y, params.isRandom, randomEngine);
	particle.currentTime = 0.0f;
	particle.age = 0.0f;

	// 物理プロパティ
	particle.mass = params.physics.mass;
	particle.drag = params.physics.drag;

	return particle;
}

void ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath)
{
	// 登録済みの名前かチェック
	if (particleGroups_.contains(name)) {
		// 登録済みの名前なら早期リターン
		return;
	}

	// グループを追加
	particleGroups_[name] = ParticleGroup();
	ParticleGroup& particleGroup = particleGroups_[name];

	// マテリアルデータにテクスチャファイルパスを設定
	particleGroup.materialData.textureFilePath = textureFilePath;
	// テクスチャ読み込み
	TextureManager::GetInstance()->LoadTexture(particleGroup.materialData.textureFilePath);
	// マテリアルデータにテクスチャのSRVインデックスを記録
	particleGroup.materialData.textureIndexSRV = TextureManager::GetInstance()->GetTextureIndexByFilePath(particleGroup.materialData.textureFilePath);

	// Instancing用のリソースを生成
	particleGroup.instancingResource = dxCommon_->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);
	particleGroup.srvIndex = srvManager_->Allocate();
	// 書き込むためのアドレスを取得
	particleGroup.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroup.instancingDataForGPU));

	srvManager_->CreateSRVforStructuredBuffer(particleGroup.srvIndex, particleGroup.instancingResource.Get(), kNumMaxInstance, sizeof(ParticleForGPU));
	// インスタンス数を初期化
	particleGroup.instancingData.resize(kNumMaxInstance);

	// パラメータ初期化
	if (particleParameters_.find(name) == particleParameters_.end()) {
		ParticleParameters& params = particleParameters_[name];

		// Transform初期値
		params.baseTransform.scaleMin = { 1.0f, 1.0f, 1.0f };
		params.baseTransform.scaleMax = { 1.0f, 1.0f, 1.0f };
		params.baseTransform.translateMin = { 0.0f, 0.0f, 0.0f };
		params.baseTransform.translateMax = { 0.0f, 0.0f, 0.0f };
		params.baseTransform.rotateMin = { 0.0f, 0.0f, 0.0f };
		params.baseTransform.rotateMax = { 0.0f, 0.0f, 0.0f };

		// Velocity初期値
		params.baseVelocity.velocityMin = { -1.0f, -1.0f, -1.0f };
		params.baseVelocity.velocityMax = { 1.0f, 1.0f, 1.0f };

		// Color初期値
		params.baseColor.minColor = { 0.8f, 0.8f, 0.8f };
		params.baseColor.maxColor = { 1.0f, 1.0f, 1.0f };
		params.baseColor.alpha = 1.0f;

		// Life初期値
		params.baseLife.lifeTime = { 1.0f, 2.0f };

		// 既存フラグ
		params.isRandomRotate = false;
		params.isRandomScale = false;
		params.minmaxScale = { 0.0f, 1.0f };

		// 🆕 新しいパラメータのデフォルト値
		params.maxParticles = 1000;
		params.spawnRate = 10.0f;
		params.burstMode = false;
		params.burstCount = 10;
		params.burstInterval = 1.0f;
		params.looping = true;
		params.duration = 5.0f;
		params.startDelay = 0.0f;
		params.sortParticles = false;
		params.frustumCulling = true;
		params.showDebugInfo = false;
		params.debugColor = { 1.0f, 0.0f, 0.0f, 1.0f };
	}

	// UV設定
	particleGroup.uvScale = { 1.0f, 1.0f };
	particleGroup.uvTranslate = { 0.0f, 0.0f };
	particleGroup.uvRotate = 0.0f;
	particleGroup.textureSheetX = 1;
	particleGroup.textureSheetY = 1;
	particleGroup.textureAnimSpeed = 1.0f;
	particleGroup.textureAnimLoop = true;

	particleGroup.blendMode = BlendMode::kBlendModeAdd;
	particleGroup.graphicsPipelineState = PipelineManager::GetInstance()->GetBlendModePSO(particleGroup.blendMode);

	// JSON初期化
	InitJson(name);
}

std::list<ParticleManager::Particle> ParticleManager::Emit(const std::string& name, const Vector3& position, uint32_t count)
{
	auto it = particleGroups_.find(name);
	assert(it != particleGroups_.end());

	ParticleGroup& group = it->second;
	const ParticleParameters& params = particleParameters_[name];
	std::list<Particle> emittedParticles;

	// 最大パーティクル数制限
	if (group.particles.size() + count > static_cast<size_t>(params.maxParticles)) {
		count = (std::max)(0, params.maxParticles - static_cast<int>(group.particles.size()));
	}

	std::mt19937 randomEngine = std::mt19937(seedGenerator_());

	// 各パーティクルを生成し追加
	for (uint32_t i = 0; i < count; ++i) {
		Particle newParticle = MakeNewParticle(name, randomEngine, position);
		emittedParticles.push_back(newParticle);
	}

	// 既存のパーティクルリストに新しいパーティクルを追加
	group.particles.splice(group.particles.end(), emittedParticles);

	return emittedParticles;
}

void ParticleManager::SetDefaultPrimitiveMesh(const std::shared_ptr<Mesh>& mesh)
{
	defaultMesh_ = mesh;
	instancingData_.resize(kNumMaxInstance);
}

void ParticleManager::SetPrimitiveMesh(const std::string& groupName, const std::shared_ptr<Mesh>& mesh)
{
	auto it = particleGroups_.find(groupName);
	if (it == particleGroups_.end()) {
		Logger("SetPrimitiveMesh: Group not found : " + groupName);
		return;
	}
	it->second.mesh = mesh;
}

// 🎨 拡張されたJSON登録システム
void ParticleManager::InitJson(const std::string& name)
{
	jsonManagers_[name] = std::make_unique<JsonManager>(name, "Resources/Json/Particles");
	jsonManagers_[name]->SetCategory("ParticleParameter");
	jsonManagers_[name]->SetSubCategory(name + "Prm");

	auto& pm = jsonManagers_[name];
	auto& params = particleParameters_[name];
	auto& group = particleGroups_[name];

	// ---------------------- 基本設定 ----------------------
	pm->SetTreePrefix("基本設定");
	pm->Register("最大パーティクル数", &params.maxParticles);
	pm->Register("ループ再生", &params.looping);
	pm->Register("持続時間", &params.duration);
	pm->Register("開始遅延", &params.startDelay);
	pm->Register("生成レート", &params.spawnRate);
	pm->Register("バーストモード", &params.burstMode);
	pm->Register("バースト数", &params.burstCount);
	pm->Register("バースト間隔", &params.burstInterval);

	// ---------------------- 発生形状設定 ----------------------
	pm->SetTreePrefix("発生形状");
	pm->Register("形状タイプ", &params.emissionShape.type);
	pm->Register("サイズ", &params.emissionShape.size);
	pm->Register("半径", &params.emissionShape.radius);
	pm->Register("内半径", &params.emissionShape.innerRadius);
	pm->Register("高さ", &params.emissionShape.height);
	pm->Register("角度", &params.emissionShape.angle);
	pm->Register("方向", &params.emissionShape.direction);
	pm->Register("表面のみ発生", &params.emissionShape.shellEmission);
	pm->Register("エッジのみ発生", &params.emissionShape.edgeEmission);
	pm->Register("エッジ厚み", &params.emissionShape.edgeThickness);

	// ---------------------- トランスフォーム系 ----------------------
	pm->SetTreePrefix("スケール");
	pm->Register("最小", &params.baseTransform.scaleMin);
	pm->Register("最大", &params.baseTransform.scaleMax);
	pm->Register("小さくなって消える", &params.enableScale);
	pm->Register("Y軸をランダムのスケールで生成", &params.isRandomScale);
	pm->Register("Y軸のスケールの最大最小", &params.minmaxScale);

	pm->SetTreePrefix("位置");
	pm->Register("最小", &params.baseTransform.translateMin);
	pm->Register("最大", &params.baseTransform.translateMax);

	pm->SetTreePrefix("回転");
	pm->Register("最小", &params.baseTransform.rotateMin);
	pm->Register("最大", &params.baseTransform.rotateMax);
	pm->Register("方向へ回転", &params.isRotateDirection);

	// ---------------------- 速度・移動設定 ----------------------
	pm->SetTreePrefix("速度");
	pm->Register("最小", &params.baseVelocity.velocityMin);
	pm->Register("最大", &params.baseVelocity.velocityMax);
	pm->Register("方向", &params.direction);
	pm->Register("速度", &params.speed);
	pm->Register("ランダムの方向に飛ばす", &params.isUnRandomSpeed);

	pm->SetTreePrefix("移動パターン");
	pm->Register("移動タイプ", &params.movement.type);
	pm->Register("曲線の強さ", &params.movement.curveStrength);
	pm->Register("螺旋半径", &params.movement.spiralRadius);
	pm->Register("螺旋速度", &params.movement.spiralSpeed);
	pm->Register("波の振幅", &params.movement.waveAmplitude);
	pm->Register("波の周波数", &params.movement.waveFrequency);
	pm->Register("バウンス高さ", &params.movement.bounceHeight);
	pm->Register("軌道中心", &params.movement.orbitCenter);
	pm->Register("軌道半径", &params.movement.orbitRadius);
	pm->Register("ジグザグ角度", &params.movement.zigzagAngle);

	pm->SetTreePrefix("カラー");
	pm->Register("最小", &params.baseColor.minColor);
	pm->Register("最大", &params.baseColor.maxColor);
	pm->Register("アルファ", &params.baseColor.alpha);

	pm->SetTreePrefix("カラーアニメーション");
	pm->Register("変化タイプ", &params.colorAnimation.type);
	pm->Register("開始色", &params.colorAnimation.startColor);
	pm->Register("中間色", &params.colorAnimation.midColor);
	pm->Register("終了色", &params.colorAnimation.endColor);
	pm->Register("中間点位置", &params.colorAnimation.midPoint);
	pm->Register("点滅頻度", &params.colorAnimation.flashFrequency);
	pm->Register("虹色変化速度", &params.colorAnimation.rainbowSpeed);
	pm->Register("滑らかな遷移", &params.colorAnimation.smoothTransition);

	// ---------------------- スケールアニメーション ----------------------
	pm->SetTreePrefix("スケールアニメーション");
	pm->Register("変化タイプ", &params.scaleAnimation.type);
	pm->Register("開始スケール", &params.scaleAnimation.startScale);
	pm->Register("中間スケール", &params.scaleAnimation.midScale);
	pm->Register("終了スケール", &params.scaleAnimation.endScale);
	pm->Register("中間点位置", &params.scaleAnimation.midPoint);
	pm->Register("脈動頻度", &params.scaleAnimation.pulseFrequency);
	pm->Register("引き伸ばし係数", &params.scaleAnimation.stretchFactor);
	pm->Register("均等スケール", &params.scaleAnimation.uniformScale);

	// ---------------------- 回転アニメーション ----------------------
	pm->SetTreePrefix("回転アニメーション");
	pm->Register("回転タイプ", &params.rotationAnimation.type);
	pm->Register("回転速度", &params.rotationAnimation.rotationSpeed);
	pm->Register("回転加速度", &params.rotationAnimation.rotationAcceleration);
	pm->Register("ランダム倍率", &params.rotationAnimation.randomMultiplier);
	pm->Register("速度方向を向く", &params.rotationAnimation.faceVelocity);
	pm->Register("向く方向", &params.rotationAnimation.faceDirection);

	// ---------------------- 物理設定 ----------------------
	pm->SetTreePrefix("物理");
	pm->Register("物理有効", &params.physics.enabled);
	pm->Register("重力", &params.physics.gravity);
	pm->Register("質量", &params.physics.mass);
	pm->Register("空気抵抗", &params.physics.drag);
	pm->Register("角度抵抗", &params.physics.angularDrag);
	pm->Register("弾性", &params.physics.elasticity);
	pm->Register("磁力", &params.physics.magnetism);
	pm->Register("磁場方向", &params.physics.magneticField);
	pm->Register("複雑な物理計算", &params.physics.useComplexPhysics);

	// ---------------------- ノイズ設定 ----------------------
	pm->SetTreePrefix("ノイズ");
	pm->Register("ノイズ有効", &params.noise.enabled);
	pm->Register("強度", &params.noise.strength);
	pm->Register("周波数", &params.noise.frequency);
	pm->Register("スクロール速度", &params.noise.scrollSpeed);
	pm->Register("時間スケール", &params.noise.timeScale);
	pm->Register("オクターブ数", &params.noise.octaves);
	pm->Register("持続性", &params.noise.persistence);
	pm->Register("位置に影響", &params.noise.affectPosition);
	pm->Register("速度に影響", &params.noise.affectVelocity);
	pm->Register("色に影響", &params.noise.affectColor);
	pm->Register("スケールに影響", &params.noise.affectScale);

	// ---------------------- 衝突設定 ----------------------
	pm->SetTreePrefix("衝突");
	pm->Register("衝突有効", &params.collision.enabled);
	pm->Register("反発係数", &params.collision.bounciness);
	pm->Register("摩擦係数", &params.collision.friction);
	pm->Register("衝突時消滅", &params.collision.killOnCollision);
	pm->Register("衝突時固着", &params.collision.stickOnCollision);
	pm->Register("地面レベル", &params.collision.groundLevel);
	pm->Register("地面衝突有効", &params.collision.hasGroundCollision);

	// ---------------------- 軌跡設定 ----------------------
	pm->SetTreePrefix("軌跡");
	pm->Register("軌跡有効", &params.trail.enabled);
	pm->Register("最大長", &params.trail.maxLength);
	pm->Register("幅", &params.trail.width);
	pm->Register("フェード速度", &params.trail.fadeSpeed);
	pm->Register("軌跡色", &params.trail.trailColor);
	pm->Register("パーティクル色継承", &params.trail.inheritParticleColor);
	pm->Register("最小速度", &params.trail.minVelocity);

	// ---------------------- LOD設定 ----------------------
	pm->SetTreePrefix("LOD");
	pm->Register("LOD有効", &params.lod.enabled);
	pm->Register("近距離", &params.lod.nearDistance);
	pm->Register("遠距離", &params.lod.farDistance);
	pm->Register("近距離最大数", &params.lod.maxParticlesNear);
	pm->Register("遠距離最大数", &params.lod.maxParticlesFar);
	pm->Register("カリング距離", &params.lod.cullingDistance);
	pm->Register("動的LOD", &params.lod.dynamicLOD);

	// ---------------------- UV設定 ----------------------
	pm->SetTreePrefix("UV");
	pm->Register("UVスケール", &group.uvScale);
	pm->Register("UV座標", &group.uvTranslate);
	pm->Register("UV回転", &group.uvRotate);
	pm->Register("UVアニメーション有効", &group.uvAnimationEnable);
	pm->Register("UV速度X", &group.uvAnimSpeedX);
	pm->Register("UV速度Y", &group.uvAnimSpeedY);
	pm->Register("テクスチャシートX", &group.textureSheetX);
	pm->Register("テクスチャシートY", &group.textureSheetY);
	pm->Register("テクスチャアニメ速度", &group.textureAnimSpeed);
	pm->Register("テクスチャアニメループ", &group.textureAnimLoop);

	// ---------------------- 寿命設定 ----------------------
	pm->SetTreePrefix("寿命");
	pm->Register("時間", &params.baseLife.lifeTime);

	// ---------------------- ランダム設定 ----------------------
	pm->SetTreePrefix("ランダム");
	pm->Register("有効", &params.isRandom);
	pm->Register("中心からランダム", &params.randomFromCenter);
	pm->Register("方向最小", &params.randomDirectionMin);
	pm->Register("方向最大", &params.randomDirectionMax);
	pm->Register("加速度", &params.randomForce);
	pm->Register("Z軸をランダムに回転", &params.isRandomRotate);

	// ---------------------- その他 ----------------------
	pm->SetTreePrefix("その他");
	pm->Register("オフセット", &params.offset);
	pm->Register("ブレンドモード", &group.blendMode);
	pm->Register("ビルボード", &params.useBillboard);
	pm->Register("パーティクルソート", &params.sortParticles);
	pm->Register("フラスタムカリング", &params.frustumCulling);

	// ---------------------- デバッグ設定 ----------------------
	pm->SetTreePrefix("デバッグ");
	pm->Register("デバッグ情報表示", &params.showDebugInfo);
	pm->Register("デバッグ色", &params.debugColor);

	pm->ClearTreePrefix();
}// ParticleManager.cppの末尾に追加する実装

// 🌪️ 力場の影響を適用
// ParticleManager.cppの末尾に追加する実装

// 🌪️ 力場の影響を適用
void ParticleManager::UpdateParticleForces(Particle& particle, const ParticleParameters& params, float deltaTime)
{
	for (const auto& forceField : params.forceFields) {
		if (!forceField.enabled) continue;

		Vector3 toParticle = particle.transform.translate - forceField.position;
		float distance = Vector3::Length(toParticle);

		// 範囲外なら無視
		if (distance > forceField.range) continue;

		// 減衰計算
		float falloff = 1.0f;
		if (distance > forceField.falloffStart) {
			float falloffRange = forceField.falloffEnd - forceField.falloffStart;
			if (falloffRange > 0.0f) {
				falloff = 1.0f - ((distance - forceField.falloffStart) / falloffRange);
				falloff = (std::max)(0.0f, falloff);
			}
		}

		Vector3 force = { 0.0f, 0.0f, 0.0f };

		switch (forceField.type) {
		case ForceType::Gravity:
			force = forceField.direction * forceField.strength;
			break;

		case ForceType::Wind:
			force = forceField.direction * forceField.strength;
			// 風の変動を追加
			force += forceField.windVariation * std::sin(particle.currentTime * forceField.frequency);
			break;

		case ForceType::Vortex:
			if (distance > 0.0f) {
				Vector3 normalizedToParticle = Vector3::Normalize(toParticle);
				Vector3 tangent = Vector3::Cross(forceField.axis, normalizedToParticle);
				force = tangent * forceField.strength * (1.0f / distance);
			}
			break;

		case ForceType::Radial:
			if (distance > 0.0f) {
				Vector3 direction = Vector3::Normalize(toParticle);
				force = direction * forceField.strength;
			}
			break;

		case ForceType::Turbulence:
		{
			float noise = PerlinNoise(particle.transform.translate, forceField.frequency);
			Vector3 turbulence = {
				std::sin(noise * 2.0f * std::numbers::pi_v<float>),
				std::cos(noise * 2.0f * std::numbers::pi_v<float>),
				std::sin(noise * 4.0f * std::numbers::pi_v<float>)
			};
			force = turbulence * forceField.strength * forceField.amplitude;
		}
		break;

		case ForceType::Spring:
			force = -toParticle * forceField.strength;
			break;

		case ForceType::Damping:
			force = -particle.velocity * forceField.strength;
			break;

		case ForceType::Magnet:
			if (distance > 0.0f) {
				Vector3 direction = -Vector3::Normalize(toParticle);
				float magneticForce = forceField.strength / (distance * distance + 1.0f);
				force = direction * magneticForce;
			}
			break;
		}

		// 質量で割って加速度に変換
		Vector3 acceleration = force * falloff / particle.mass;
		particle.velocity += acceleration * deltaTime;
	}
}

// 🏃 移動パターンの適用
void ParticleManager::UpdateParticleMovement(Particle& particle, const ParticleParameters& params, float deltaTime)
{
	const auto& movement = params.movement;

	switch (movement.type) {
	case MovementType::Linear:
		// 基本の直線移動（既存の処理で十分）
		break;

	case MovementType::Curve:
	{
		Vector3 curveForce = Vector3::Cross(particle.velocity, Vector3{ 0.0f, 1.0f, 0.0f });
		curveForce = Vector3::Normalize(curveForce) * movement.curveStrength;
		particle.velocity += curveForce * deltaTime;
	}
	break;

	case MovementType::Spiral:
	{
		float angle = particle.currentTime * movement.spiralSpeed;
		Vector3 spiralOffset = {
			std::cos(angle) * movement.spiralRadius,
			particle.velocity.y * deltaTime,
			std::sin(angle) * movement.spiralRadius
		};
		particle.transform.translate = particle.initialPosition + spiralOffset * particle.age;
	}
	break;

	case MovementType::Wave:
	{
		Vector3 wave = movement.waveAmplitude * std::sin(particle.currentTime * movement.waveFrequency);
		particle.velocity += wave * deltaTime;
	}
	break;

	case MovementType::Bounce:
	{
		float bouncePhase = std::sin(particle.currentTime * 4.0f);
		if (bouncePhase > 0.0f) {
			particle.velocity.y += movement.bounceHeight * bouncePhase * deltaTime;
		}
	}
	break;

	case MovementType::Orbit:
	{
		Vector3 toCenter = particle.transform.translate - movement.orbitCenter;
		float distance = Vector3::Length(toCenter);
		if (distance > 0.0f) {
			Vector3 tangent = Vector3::Cross(Vector3{ 0.0f, 1.0f, 0.0f }, Vector3::Normalize(toCenter));
			particle.velocity += tangent * movement.orbitRadius * deltaTime;
		}
	}
	break;

	case MovementType::Zigzag:
	{
		float zigzagPhase = std::sin(particle.currentTime * 5.0f);
		float angleRad = movement.zigzagAngle * std::numbers::pi_v<float> / 180.0f;
		Vector3 zigzagDir = {
			std::cos(angleRad) * zigzagPhase,
			0.0f,
			std::sin(angleRad) * zigzagPhase
		};
		particle.velocity += zigzagDir * deltaTime;
	}
	break;
	}
}

// 🎨 カラーアニメーション
void ParticleManager::UpdateParticleColor(Particle& particle, const ParticleParameters& params)
{
	const auto& colorAnim = params.colorAnimation;

	switch (colorAnim.type) {
	case ColorChangeType::None:
		// 何もしない
		break;

	case ColorChangeType::Fade:
		particle.color = particle.initialColor;
		particle.color.w = particle.initialColor.w * (1.0f - particle.age);
		break;

	case ColorChangeType::Gradient:
	{
		if (particle.age < colorAnim.midPoint) {
			float t = particle.age / colorAnim.midPoint;
			particle.color = lerp(colorAnim.startColor, colorAnim.midColor, t);
		} else {
			float t = (particle.age - colorAnim.midPoint) / (1.0f - colorAnim.midPoint);
			particle.color = lerp(colorAnim.midColor, colorAnim.endColor, t);
		}
	}
	break;

	case ColorChangeType::Flash:
	{
		float flash = std::sin(particle.currentTime * colorAnim.flashFrequency * 2.0f * std::numbers::pi_v<float>);
		float intensity = (flash + 1.0f) * 0.5f; // 0-1に正規化
		particle.color = lerp(colorAnim.startColor, colorAnim.endColor, intensity);
	}
	break;

	case ColorChangeType::Rainbow:
	{
		float hue = std::fmod(particle.currentTime * colorAnim.rainbowSpeed, 1.0f);
		// HSVからRGBへの簡易変換
		float h = hue * 6.0f;
		float c = 1.0f;
		float x = c * (1.0f - std::abs(std::fmod(h, 2.0f) - 1.0f));

		if (h < 1.0f) {
			particle.color = { c, x, 0.0f, particle.initialColor.w };
		} else if (h < 2.0f) {
			particle.color = { x, c, 0.0f, particle.initialColor.w };
		} else if (h < 3.0f) {
			particle.color = { 0.0f, c, x, particle.initialColor.w };
		} else if (h < 4.0f) {
			particle.color = { 0.0f, x, c, particle.initialColor.w };
		} else if (h < 5.0f) {
			particle.color = { x, 0.0f, c, particle.initialColor.w };
		} else {
			particle.color = { c, 0.0f, x, particle.initialColor.w };
		}
	}
	break;

	case ColorChangeType::Fire:
	{
		// 炎のような色変化（赤→オレンジ→黄色→白）
		float t = particle.age;
		if (t < 0.33f) {
			particle.color = lerp(Vector4{ 1.0f, 0.0f, 0.0f, 1.0f }, Vector4{ 1.0f, 0.5f, 0.0f, 1.0f }, t * 3.0f);
		} else if (t < 0.66f) {
			particle.color = lerp(Vector4{ 1.0f, 0.5f, 0.0f, 1.0f }, Vector4{ 1.0f, 1.0f, 0.0f, 1.0f }, (t - 0.33f) * 3.0f);
		} else {
			particle.color = lerp(Vector4{ 1.0f, 1.0f, 0.0f, 1.0f }, Vector4{ 1.0f, 1.0f, 1.0f, 0.0f }, (t - 0.66f) * 3.0f);
		}
	}
	break;

	case ColorChangeType::Electric:
	{
		// 電気のような色変化（青→白→青紫）
		float flash = std::sin(particle.currentTime * 20.0f);
		float intensity = (flash + 1.0f) * 0.5f;
		Vector4 baseColor = { 0.0f, 0.5f, 1.0f, 1.0f };
		Vector4 flashColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		particle.color = lerp(baseColor, flashColor, intensity);
		particle.color.w *= (1.0f - particle.age);
	}
	break;
	}
}

// 📏 スケールアニメーション
void ParticleManager::UpdateParticleScale(Particle& particle, const ParticleParameters& params)
{
	const auto& scaleAnim = params.scaleAnimation;

	switch (scaleAnim.type) {
	case ScaleChangeType::None:
		// 何もしない
		break;

	case ScaleChangeType::Shrink:
	{
		float scaleFactor = 1.0f - particle.age;
		particle.transform.scale.x = particle.initialScale.x * scaleFactor;
		particle.transform.scale.y = particle.initialScale.y * scaleFactor;
		particle.transform.scale.z = particle.initialScale.z * scaleFactor;
	}
	break;

	case ScaleChangeType::Grow:
	{
		float scaleFactor = 1.0f + particle.age;
		particle.transform.scale.x = particle.initialScale.x * scaleFactor;
		particle.transform.scale.y = particle.initialScale.y * scaleFactor;
		particle.transform.scale.z = particle.initialScale.z * scaleFactor;
	}
	break;

	case ScaleChangeType::Pulse:
	{
		float pulse = std::sin(particle.currentTime * scaleAnim.pulseFrequency * 2.0f * std::numbers::pi_v<float>);
		float scaleFactor = 1.0f + pulse * 0.2f;
		particle.transform.scale.x = particle.initialScale.x * scaleFactor;
		particle.transform.scale.y = particle.initialScale.y * scaleFactor;
		particle.transform.scale.z = particle.initialScale.z * scaleFactor;
	}
	break;

	case ScaleChangeType::Stretch:
	{
		// 要素ごとに計算
		float stretchX = 1.0f + particle.age * scaleAnim.stretchFactor;
		float stretchY = 1.0f - particle.age * 0.5f;
		float stretchZ = 1.0f;

		if (scaleAnim.uniformScale) {
			float avgStretch = (stretchX + stretchY + stretchZ) / 3.0f;
			stretchX = stretchY = stretchZ = avgStretch;
		}

		// 要素ごとの乗算
		particle.transform.scale.x = particle.initialScale.x * stretchX;
		particle.transform.scale.y = particle.initialScale.y * stretchY;
		particle.transform.scale.z = particle.initialScale.z * stretchZ;
	}
	break;
	}
}

// 🔄 回転アニメーション
void ParticleManager::UpdateParticleRotation(Particle& particle, const ParticleParameters& params, float deltaTime)
{
	const auto& rotAnim = params.rotationAnimation;

	switch (rotAnim.type) {
	case RotationType::None:
		// 何もしない
		break;

	case RotationType::ConstantX:
		particle.transform.rotate.x += rotAnim.rotationSpeed.x * deltaTime;
		break;

	case RotationType::ConstantY:
		particle.transform.rotate.y += rotAnim.rotationSpeed.y * deltaTime;
		break;

	case RotationType::ConstantZ:
		particle.transform.rotate.z += rotAnim.rotationSpeed.z * deltaTime;
		break;

	case RotationType::Random:
		particle.angularVelocity += Vector3{
			(std::rand() / float(RAND_MAX) - 0.5f) * rotAnim.randomMultiplier,
			(std::rand() / float(RAND_MAX) - 0.5f) * rotAnim.randomMultiplier,
			(std::rand() / float(RAND_MAX) - 0.5f) * rotAnim.randomMultiplier
		} *deltaTime;
		particle.transform.rotate += particle.angularVelocity * deltaTime;
		break;

	case RotationType::Velocity:
		if (Vector3::Length(particle.velocity) > 0.0f) {
			Vector3 forward = Vector3::Normalize(particle.velocity);
			// 速度方向に向けるための回転計算（簡易版）
			particle.transform.rotate.y = std::atan2(forward.x, forward.z);
			particle.transform.rotate.x = -std::asin(forward.y);
		}
		break;

	case RotationType::Tumble:
		particle.transform.rotate += rotAnim.rotationSpeed * deltaTime;
		break;
	}

	// 回転加速度を適用
	particle.angularVelocity += rotAnim.rotationAcceleration * deltaTime;
	particle.transform.rotate += particle.angularVelocity * deltaTime;
}

// 🎪 物理計算
void ParticleManager::UpdateParticlePhysics(Particle& particle, const ParticleParameters& params, float deltaTime)
{
	const auto& physics = params.physics;

	// 重力適用
	particle.velocity += physics.gravity * deltaTime;

	// 空気抵抗
	particle.velocity *= (1.0f - physics.drag * deltaTime);

	// 角度抵抗
	particle.angularVelocity *= (1.0f - physics.angularDrag * deltaTime);

	// 磁場の影響
	if (Vector3::Length(physics.magneticField) > 0.0f) {
		Vector3 magneticForce = Vector3::Cross(particle.velocity, physics.magneticField) * physics.magnetism;
		particle.velocity += magneticForce * deltaTime;
	}

	// 複雑な物理計算
	if (physics.useComplexPhysics) {
		// より高度な物理シミュレーション（簡易版）
		particle.acceleration *= physics.elasticity;
		particle.velocity += particle.acceleration * deltaTime;
		particle.acceleration *= 0.9f; // 減衰
	}
}

// 🎯 衝突判定
void ParticleManager::UpdateParticleCollision(Particle& particle, const ParticleParameters& params)
{
	const auto& collision = params.collision;

	if (!collision.enabled) return;

	// 地面衝突
	if (collision.hasGroundCollision && particle.transform.translate.y <= collision.groundLevel.y) {
		particle.transform.translate.y = collision.groundLevel.y;

		if (collision.killOnCollision) {
			particle.currentTime = particle.lifeTime; // 即座に消滅
			return;
		}

		if (collision.stickOnCollision) {
			particle.velocity = { 0.0f, 0.0f, 0.0f };
		} else {
			// 反発
			particle.velocity.y = -particle.velocity.y * collision.bounciness;
			// 摩擦
			particle.velocity.x *= (1.0f - collision.friction);
			particle.velocity.z *= (1.0f - collision.friction);
		}

		particle.hasCollided = true;
	}
}

// 🎵 ノイズ適用
void ParticleManager::ApplyNoise(Particle& particle, const NoiseSettings& noise, float deltaTime)
{
	Vector3 noisePos = particle.transform.translate + noise.scrollSpeed * particle.currentTime;
	float noiseValue = PerlinNoise(noisePos, noise.frequency);

	// オクターブを重ねる
	float amplitude = 1.0f;
	float totalNoise = 0.0f;
	float maxValue = 0.0f;

	for (int i = 0; i < noise.octaves; ++i) {
		totalNoise += PerlinNoise(noisePos * std::pow(2.0f, i), noise.frequency) * amplitude;
		maxValue += amplitude;
		amplitude *= noise.persistence;
	}

	totalNoise /= maxValue; // 正規化
	totalNoise *= noise.strength;

	Vector3 noiseVector = {
		std::sin(totalNoise * 2.0f * std::numbers::pi_v<float>),
		std::cos(totalNoise * 2.0f * std::numbers::pi_v<float>),
		std::sin(totalNoise * 4.0f * std::numbers::pi_v<float>)
	};

	if (noise.affectPosition) {
		particle.transform.translate += noiseVector * deltaTime;
	}

	if (noise.affectVelocity) {
		particle.velocity += noiseVector * deltaTime;
	}

	if (noise.affectColor) {
		Vector4 colorNoise = { noiseVector.x, noiseVector.y, noiseVector.z, 0.0f };
		particle.color += colorNoise * (0.1f * deltaTime);
		// 色の値を0-1にクランプ
		particle.color.x = (std::max)(0.0f, (std::min)(1.0f, particle.color.x));
		particle.color.y = (std::max)(0.0f, (std::min)(1.0f, particle.color.y));
		particle.color.z = (std::max)(0.0f, (std::min)(1.0f, particle.color.z));
	}

	if (noise.affectScale) {
		float scaleNoise = 1.0f + totalNoise * 0.2f;
		particle.transform.scale = particle.initialScale * scaleNoise;
	}
}

// 📐 発生形状からの位置サンプリング
Vector3 ParticleManager::SampleEmissionShape(const EmissionShapeSettings& shape, std::mt19937& rng)
{
	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
	std::uniform_real_distribution<float> dist01(0.0f, 1.0f);

	switch (shape.type) {
	case EmissionType::Point:
		return { 0.0f, 0.0f, 0.0f };

	case EmissionType::Sphere:
	{
		// 球面上の均等分布
		float theta = dist01(rng) * 2.0f * std::numbers::pi_v<float>;
		float phi = std::acos(1.0f - 2.0f * dist01(rng));
		float r = shape.shellEmission ? shape.radius : shape.radius * std::cbrt(dist01(rng));

		return {
			r * std::sin(phi) * std::cos(theta),
			r * std::cos(phi),
			r * std::sin(phi) * std::sin(theta)
		};
	}

	case EmissionType::Box:
		return {
			dist(rng) * shape.size.x,
			dist(rng) * shape.size.y,
			dist(rng) * shape.size.z
		};

	case EmissionType::Circle:
	{
		float angle = dist01(rng) * 2.0f * std::numbers::pi_v<float>;
		float r = shape.shellEmission ? shape.radius : shape.radius * std::sqrt(dist01(rng));
		return {
			r * std::cos(angle),
			0.0f,
			r * std::sin(angle)
		};
	}

	case EmissionType::Ring:
	{
		float angle = dist01(rng) * 2.0f * std::numbers::pi_v<float>;
		float r = shape.innerRadius + (shape.radius - shape.innerRadius) * dist01(rng);
		return {
			r * std::cos(angle),
			0.0f,
			r * std::sin(angle)
		};
	}

	case EmissionType::Cone:
	{
		float angle = dist01(rng) * 2.0f * std::numbers::pi_v<float>;
		float height = dist01(rng) * shape.height;
		float radius = (height / shape.height) * shape.radius * std::tan(shape.angle * std::numbers::pi_v<float> / 180.0f);
		float r = shape.shellEmission ? radius : radius * std::sqrt(dist01(rng));

		return {
			r * std::cos(angle),
			height,
			r * std::sin(angle)
		};
	}

	case EmissionType::Line:
		return {
			0.0f,
			dist(rng) * shape.height,
			0.0f
		};

	case EmissionType::Hemisphere:
	{
		float theta = dist01(rng) * 2.0f * std::numbers::pi_v<float>;
		float phi = std::acos(dist01(rng)); // 0からπ/2まで
		float r = shape.shellEmission ? shape.radius : shape.radius * std::cbrt(dist01(rng));

		return {
			r * std::sin(phi) * std::cos(theta),
			r * std::cos(phi),
			r * std::sin(phi) * std::sin(theta)
		};
	}
	}

	return { 0.0f, 0.0f, 0.0f };
}

// パーリンノイズの簡易実装
float ParticleManager::PerlinNoise(const Vector3& position, float frequency)
{
	// 簡易的なパーリンノイズ実装
	Vector3 p = position * frequency;

	// 整数部分と小数部分を分離
	int xi = static_cast<int>(std::floor(p.x)) & 255;
	int yi = static_cast<int>(std::floor(p.y)) & 255;
	int zi = static_cast<int>(std::floor(p.z)) & 255;

	float xf = p.x - std::floor(p.x);
	float yf = p.y - std::floor(p.y);
	float zf = p.z - std::floor(p.z);

	// フェード関数
	auto fade = [](float t) { return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f); };

	float u = fade(xf);
	float v = fade(yf);
	float w = fade(zf);

	// 疑似ランダム関数（簡易版）
	auto hash = [](int x, int y, int z) {
		return ((x * 73856093) ^ (y * 19349663) ^ (z * 83492791)) % 256;
		};

	// グラデーション計算（簡易版）
	auto grad = [](int hash, float x, float y, float z) {
		int h = hash & 15;
		float u = h < 8 ? x : y;
		float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
		};

	// 8つの角の値を計算
	int aaa = hash(xi, yi, zi);
	int aba = hash(xi, yi + 1, zi);
	int aab = hash(xi, yi, zi + 1);
	int abb = hash(xi, yi + 1, zi + 1);
	int baa = hash(xi + 1, yi, zi);
	int bba = hash(xi + 1, yi + 1, zi);
	int bab = hash(xi + 1, yi, zi + 1);
	int bbb = hash(xi + 1, yi + 1, zi + 1);

	// 線形補間
	auto lerp = [](float a, float b, float t) { return a + t * (b - a); };

	float x1 = lerp(grad(aaa, xf, yf, zf), grad(baa, xf - 1, yf, zf), u);
	float x2 = lerp(grad(aba, xf, yf - 1, zf), grad(bba, xf - 1, yf - 1, zf), u);
	float y1 = lerp(x1, x2, v);

	x1 = lerp(grad(aab, xf, yf, zf - 1), grad(bab, xf - 1, yf, zf - 1), u);
	x2 = lerp(grad(abb, xf, yf - 1, zf - 1), grad(bbb, xf - 1, yf - 1, zf - 1), u);
	float y2 = lerp(x1, x2, v);

	return lerp(y1, y2, w);
}

// 🌟 便利メソッドの実装
void ParticleManager::AddForceField(const std::string& groupName, const ForceField& field)
{
	if (particleParameters_.find(groupName) != particleParameters_.end()) {
		particleParameters_[groupName].forceFields.push_back(field);
	}
}

void ParticleManager::ApplyPreset(const std::string& groupName, const std::string& presetName)
{
	if (particleParameters_.find(groupName) == particleParameters_.end()) return;

	ParticleParameters& params = particleParameters_[groupName];

	if (presetName == "Fire") {
		params.colorAnimation.type = ColorChangeType::Fire;
		params.movement.type = MovementType::Wave;
		params.physics.gravity = { 0.0f, 2.0f, 0.0f }; // 上向きの力
		params.scaleAnimation.type = ScaleChangeType::Shrink;
	} else if (presetName == "Smoke") {
		params.colorAnimation.type = ColorChangeType::Fade;
		params.movement.type = MovementType::Spiral;
		params.physics.gravity = { 0.0f, 1.0f, 0.0f };
		params.noise.enabled = true;
		params.noise.strength = 0.5f;
	} else if (presetName == "Explosion") {
		params.movement.type = MovementType::Linear;
		params.randomFromCenter = true;
		params.speed = 5.0f;
		params.colorAnimation.type = ColorChangeType::Fire;
		params.scaleAnimation.type = ScaleChangeType::Grow;
	}
	// 他のプリセットも追加可能
}

void ParticleManager::EmitBurst(const std::string& groupName, const Vector3& position, int count)
{
	if (particleGroups_.find(groupName) != particleGroups_.end()) {
		Emit(groupName, position, count);
	}
}

ParticleManager::PerformanceInfo ParticleManager::GetPerformanceInfo() const
{
	return performanceInfo_;
}