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

	accelerationField.acceleration = { 15.0f,0.0f,0.0f };
	accelerationField.area.min = { -10.0f,-10.0f,-10.0f };
	accelerationField.area.max = { 10.0f,10.0f,10.0f };

	rootSignature_ = PipelineManager::GetInstance()->GetRootSignature("Particle");
	graphicsPipelineState_ = PipelineManager::GetInstance()->GetPipeLineStateObject("Particle");

	// マテリアルリソース作成
	CreateMaterialResource();
	instancingData_.resize(kNumMaxInstance);
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
///  UpdateParticles<br/>
///  ランダム系パラメータをリアルタイムに反映しつつ
///  パーティクルを更新・GPU へ転送する
/// </summary>
void ParticleManager::UpdateParticles()
{
	// ───────── カメラ行列計算 ─────────
// ビュー・プロジェクション行列
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

	// ───────── 各パーティクルグループ ─────────
	for (auto& [name, group] : particleGroups_)
	{
		const ParticleParameters& prm = particleParameters_[name];
		bool useBB = prm.useBillboard;

		/* 乱数ディストリビューション ― リアルタイム値を毎フレーム取得 */
		std::uniform_real_distribution<float>
			rx(prm.baseTransform.translateMin.x, prm.baseTransform.translateMax.x),
			ry(prm.baseTransform.translateMin.y, prm.baseTransform.translateMax.y),
			rz(prm.baseTransform.translateMin.z, prm.baseTransform.translateMax.z),
			dx(prm.randomDirectionMin.x, prm.randomDirectionMax.x),
			dy(prm.randomDirectionMin.y, prm.randomDirectionMax.y),
			dz(prm.randomDirectionMin.z, prm.randomDirectionMax.z);

		uint32_t instanceCnt = 0;

		if (group.uvAnimationEnable) {
			group.uvTranslate.x += group.uvAnimSpeedX * kDeltaTime;
			group.uvTranslate.y += group.uvAnimSpeedY * kDeltaTime;

			// ループさせるなら下記を有効に
			group.uvTranslate.x = std::fmod(group.uvTranslate.x, 1.0f);
			group.uvTranslate.y = std::fmod(group.uvTranslate.y, 1.0f);
		}

		// ───────── 個々のパーティクル更新 ─────────
		for (auto it = group.particles.begin(); it != group.particles.end(); )
		{
			Particle& particle = *it;

			// 寿命判定
			if (particle.currentTime >= particle.lifeTime) {
				it = group.particles.erase(it);
				continue;
			}





			particle.currentTime += kDeltaTime;

			// 中心から放射するランダム加速
			if (prm.randomFromCenter) {
				// ① ランダム方向ベクトルを作る
				Vector3 randDir = {
					dx(randomEngine_),   // X
					dy(randomEngine_),   // Y
					dz(randomEngine_)    // Z
				};

				// ② 正規化して単位ベクトルへ
				Vector3 dir = Normalize(randDir);

				// ③ 加速度として付与
				particle.velocity += dir * prm.randomForce;
			}




			// 位置揺らぎ（Enable）
			if (prm.isRandom) {
				particle.transform.translate.x += rx(randomEngine_) * kDeltaTime;
				particle.transform.translate.y += ry(randomEngine_) * kDeltaTime;
				particle.transform.translate.z += rz(randomEngine_) * kDeltaTime;
			}

			//// 加速度フィールド
			//if (IsWithinAABB(particle.transform.translate, accelerationField.area)) {
			//	particle.velocity += accelerationField.acceleration * kDeltaTime;
			//}

			// 移動
			particle.transform.translate += particle.velocity * kDeltaTime;


			// α フェード
			float alpha = 1.0f - (particle.currentTime / particle.lifeTime);

			// スケール縮小処理
			if (prm.enableScale) {
				float scaleRate = alpha;  // 残り寿命に比例してスケーリング
				particle.transform.scale = {
					particle.transform.scale.x * scaleRate,
					particle.transform.scale.y * scaleRate,
					particle.transform.scale.z * scaleRate
				};
			}


			// 行列計算
			Matrix4x4 S = MakeScaleMatrix(particle.transform.scale);
			Matrix4x4 T = MakeTranslateMatrix(particle.transform.translate + prm.offset);
			Matrix4x4 world = useBB
				? S * bbBase * T
				: MakeAffineMatrix(
					particle.transform.scale,
					particle.transform.rotate,
					particle.transform.translate + prm.offset);

			Matrix4x4 wvp = Multiply(world, vp);

			// GPU へ書き込み
			if (instanceCnt < kNumMaxInstance) {
				group.instancingData[instanceCnt].WVP = wvp;
				group.instancingData[instanceCnt].World = world;
				group.instancingData[instanceCnt].color = particle.color;
				group.instancingData[instanceCnt].color.w = alpha;
				++instanceCnt;
			}
			++it;
		}

		// インスタンス数更新 & 転送
		group.instance = instanceCnt;
		if (group.instancingDataForGPU) {
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


ParticleManager::// MakeNewParticle 関数：新しい Vector3 ベースの min/max 対応実装
Particle ParticleManager::MakeNewParticle(const std::string& name, std::mt19937& randomEngine, const Vector3& position)
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

	// Transform
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

	particle.transform.translate = position + Vector3{
		getValue(params.baseTransform.translateMin.x, params.baseTransform.translateMax.x, params.isRandom, randomEngine),
		getValue(params.baseTransform.translateMin.y, params.baseTransform.translateMax.y, params.isRandom, randomEngine),
		getValue(params.baseTransform.translateMin.z, params.baseTransform.translateMax.z, params.isRandom, randomEngine)
	};

	if (params.isRandomRotate) {
		std::uniform_real_distribution<float> distRotate(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);
		particle.transform.rotate.z = distRotate(randomEngine);
	}
	if (params.isRandomScale) {
		std::uniform_real_distribution<float> distScale(params.minmaxScale.x, params.minmaxScale.y);
		particle.transform.scale.y = distScale(randomEngine);
	}


	// Velocity
	if (params.randomFromCenter) {
		auto safeMinMax = [](float a, float b) {
			return std::minmax(a, b);
			};
		auto [minX, maxX] = safeMinMax(params.randomDirectionMin.x, params.randomDirectionMax.x);
		auto [minY, maxY] = safeMinMax(params.randomDirectionMin.y, params.randomDirectionMax.y);
		auto [minZ, maxZ] = safeMinMax(params.randomDirectionMin.z, params.randomDirectionMax.z);

		std::uniform_real_distribution<float> randDirX(minX, maxX);
		std::uniform_real_distribution<float> randDirY(minY, maxY);
		std::uniform_real_distribution<float> randDirZ(minZ, maxZ);

		Vector3 dir = { randDirX(randomEngine), randDirY(randomEngine), randDirZ(randomEngine) };
		dir = Normalize(dir);

		Vector3 minV = params.baseVelocity.velocityMin;
		Vector3 maxV = params.baseVelocity.velocityMax;
		float averageSpeed = Length((minV + maxV) * 0.5f);
		particle.velocity = dir * averageSpeed;
	} else {
		particle.velocity = {
			getValue(params.baseVelocity.velocityMin.x, params.baseVelocity.velocityMax.x, params.isRandom, randomEngine),
			getValue(params.baseVelocity.velocityMin.y, params.baseVelocity.velocityMax.y, params.isRandom, randomEngine),
			getValue(params.baseVelocity.velocityMin.z, params.baseVelocity.velocityMax.z, params.isRandom, randomEngine)
		};
	}

	// Color
	particle.color = {
		getValue(params.baseColor.minColor.x, params.baseColor.maxColor.x, params.isRandom, randomEngine),
		getValue(params.baseColor.minColor.y, params.baseColor.maxColor.y, params.isRandom, randomEngine),
		getValue(params.baseColor.minColor.z, params.baseColor.maxColor.z, params.isRandom, randomEngine),
		params.baseColor.alpha
	};

	// LifeTime
	particle.lifeTime = getValue(params.baseLife.lifeTime.x, params.baseLife.lifeTime.y, params.isRandom, randomEngine);
	particle.currentTime = 0.0f;

	// offset

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

		params.isRandomRotate = false;
		params.isRandomScale = false;
		params.minmaxScale = { 0.0f, 1.0f };
	}

	particleGroup.uvScale = { 1.0f, 1.0f };
	particleGroup.uvTranslate = { 0.0f, 0.0f };
	particleGroup.uvRotate = 0.0f;

	particleGroup.blendMode = BlendMode::kBlendModeAdd;
	particleGroup.graphicsPipelineState = PipelineManager::GetInstance()->GetBlendModePSO(particleGroup.blendMode);

	InitJson(name);

}


std::list<ParticleManager::Particle> ParticleManager::Emit(const std::string& name, const Vector3& position, uint32_t count)
{
	auto it = particleGroups_.find(name);
	assert(it != particleGroups_.end());

	ParticleGroup& group = it->second;
	std::list<Particle> emittedParticles;

	std::mt19937 randomEngine = std::mt19937(seedGenerator_());
	// 各パーティクルを生成し追加
	for (uint32_t i = 0; i < count; ++i) {
		Particle newParticle = MakeNewParticle(name, randomEngine, position);
		// 生成したパーティクルをリストに追加
		emittedParticles.push_back(newParticle);
	}

	// 既存のパーティクルリストに新しいパーティクルを追加
	group.particles.splice(group.particles.end(), emittedParticles);

	return emittedParticles;
}

void ParticleManager::SetDefaultPrimitiveMesh(const std::shared_ptr<Mesh>& mesh)
{
	defaultMesh_ = mesh;
	instancingData_.resize(kNumMaxInstance);   // 従来の処理はそのまま
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

void ParticleManager::InitJson(const std::string& name)
{
	const std::string base = name + " : p"; // 名前空間を分けるためのプレフィックス
	jsonManagers_[name] = std::make_unique<JsonManager>(name, "Resources/Json/Particles");
	jsonManagers_[name]->SetCategory("ParticleParameter");
	jsonManagers_[name]->SetSubCategory(name + "Prm");


	// ---------------------- トランスフォーム系 ----------------------
	jsonManagers_[name]->SetTreePrefix("スケール");
	jsonManagers_[name]->Register("最小", &particleParameters_[name].baseTransform.scaleMin);
	jsonManagers_[name]->Register("最大", &particleParameters_[name].baseTransform.scaleMax);
	jsonManagers_[name]->Register("小さくなって消える", &particleParameters_[name].enableScale);
	jsonManagers_[name]->Register("Y軸をランダムのスケールで生成", &particleParameters_[name].isRandomScale);
	jsonManagers_[name]->Register("Y軸のスケールの最大最小", &particleParameters_[name].minmaxScale);



	jsonManagers_[name]->SetTreePrefix("位置");
	jsonManagers_[name]->Register("最小", &particleParameters_[name].baseTransform.translateMin);
	jsonManagers_[name]->Register("最大", &particleParameters_[name].baseTransform.translateMax);

	jsonManagers_[name]->SetTreePrefix("回転");
	jsonManagers_[name]->Register("最小", &particleParameters_[name].baseTransform.rotateMin);
	jsonManagers_[name]->Register("最大", &particleParameters_[name].baseTransform.rotateMax);


	jsonManagers_[name]->SetTreePrefix("UV");
	jsonManagers_[name]->Register("UVスケール", &particleGroups_[name].uvScale);
	jsonManagers_[name]->Register("UVT", &particleGroups_[name].uvTranslate);
	jsonManagers_[name]->Register("UV回転", &particleGroups_[name].uvRotate);
	jsonManagers_[name]->Register("UVアニメーションON", &particleGroups_[name].uvAnimationEnable);
	jsonManagers_[name]->Register("UV速度X", &particleGroups_[name].uvAnimSpeedX);
	jsonManagers_[name]->Register("UV速度Y", &particleGroups_[name].uvAnimSpeedY);


	// ---------------------- 移動速度系 ----------------------
	jsonManagers_[name]->SetTreePrefix("速度");
	jsonManagers_[name]->Register("最小", &particleParameters_[name].baseVelocity.velocityMin);
	jsonManagers_[name]->Register("最大", &particleParameters_[name].baseVelocity.velocityMax);

	// ---------------------- カラー設定 ----------------------
	jsonManagers_[name]->SetTreePrefix("カラー");
	jsonManagers_[name]->Register("最小", &particleParameters_[name].baseColor.minColor);
	jsonManagers_[name]->Register("最大", &particleParameters_[name].baseColor.maxColor);
	jsonManagers_[name]->Register("アルファ", &particleParameters_[name].baseColor.alpha);

	// ---------------------- ライフ設定 ----------------------
	jsonManagers_[name]->SetTreePrefix("寿命");
	jsonManagers_[name]->Register("時間", &particleParameters_[name].baseLife.lifeTime);

	// ---------------------- ランダム設定 ----------------------
	jsonManagers_[name]->SetTreePrefix("ランダム");
	jsonManagers_[name]->Register("有効", &particleParameters_[name].isRandom);
	jsonManagers_[name]->Register("中心から", &particleParameters_[name].randomFromCenter);
	jsonManagers_[name]->Register("方向最小", &particleParameters_[name].randomDirectionMin);
	jsonManagers_[name]->Register("方向最大", &particleParameters_[name].randomDirectionMax);
	jsonManagers_[name]->Register("加速度", &particleParameters_[name].randomForce);
	jsonManagers_[name]->Register("Z軸をランダムに回転", &particleParameters_[name].isRandomRotate);

	// ---------------------- その他 ----------------------
	jsonManagers_[name]->SetTreePrefix("その他");
	jsonManagers_[name]->Register("オフセット", &particleParameters_[name].offset);
	jsonManagers_[name]->Register("ブレンドモード", &particleGroups_[name].blendMode);
	jsonManagers_[name]->Register("ビルボード", &particleParameters_[name].useBillboard);

	//jsonManagers_[name]->ClearTreePrefix();


}