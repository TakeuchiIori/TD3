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

	// パイプライン生成
	CreateGraphicsPipeline();

	// 四角形の頂点を作成
	CreateVertexResource();

	// マテリアルリソース作成
	CreateMaterialResource();
}

/// <summary>
/// パーティクルの更新処理
/// </summary>
void ParticleManager::Update()
{
	UpdateParticles();

	// ブレンドモードの設定を反映
	Render(blendDesc_, currentBlendMode_);
}


void ParticleManager::Draw()
{


	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// すべてのパーティクルグループを描画
	for (auto& [groupName, particleGroup] : particleGroups_) {
		if (particleGroup.instance > 0) {
			// マテリアルCBufferの場所を指定
			dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
			// SRVのDescriptorTableを設定
			srvManager_->SetGraphicsRootDescriptorTable(1, particleGroup.srvIndex);
			// テクスチャのSRVのDescriptorTableを設定
			D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = TextureManager::GetInstance()->GetsrvHandleGPU(particleGroup.materialData.textureFilePath);
			srvManager_->SetGraphicsRootDescriptorTable(2, particleGroup.materialData.textureIndexSRV);
			// 描画
			dxCommon_->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), particleGroup.instance, 0, 0);

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
			particle.transform.translate += particle.velocity * kDeltaTime ;

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
				instancingData_[instanceCnt].WVP = wvp;
				instancingData_[instanceCnt].World = world;
				instancingData_[instanceCnt].color = particle.color;
				instancingData_[instanceCnt].color.w = alpha;
				++instanceCnt;
			}
			++it;
		}

		// インスタンス数更新 & 転送
		group.instance = instanceCnt;
		if (group.instancingData) {
			std::memcpy(group.instancingData,
				instancingData_,
				sizeof(ParticleForGPU) * instanceCnt);
		}
	}
}




void ParticleManager::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; // 0から始まる
	descriptorRange[0].NumDescriptors = 1; // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRV
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算


	// 1. RootSignatureの作成

	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// RootParameter作成。複数設定できるので配列。
	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		 			// CBVを使う
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	rootParameters_[0].Descriptor.ShaderRegister = 0;									// レジスタ番号0とバインド

	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;					// CBVを使う
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;				// VertexShaderで使う
	rootParameters_[1].Descriptor.ShaderRegister = 0;									// レジスタ番号0とバインド

	rootParameters_[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// DescriptorTableを使う
	rootParameters_[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;				// PixelShaderで使う
	rootParameters_[2].DescriptorTable.pDescriptorRanges = descriptorRange;				// Tableの中身の配列を指定
	rootParameters_[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	// Tableで利用する数

	rootParameters_[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;					// CBVを使う
	rootParameters_[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// VertexShaderで使う
	rootParameters_[3].Descriptor.ShaderRegister = 1;									// レジスタ番号1を使う
	descriptionRootSignature_.pParameters = rootParameters_;								// ルートパラメーター配列へのポインタ
	descriptionRootSignature_.NumParameters = _countof(rootParameters_);					// 配列の長さ


	// 1. パーティクルのRootSignatureの作成
	descriptorRangeForInstancing_[0].BaseShaderRegister = 0;
	descriptorRangeForInstancing_[0].NumDescriptors = 1;
	descriptorRangeForInstancing_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters_[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing_;
	rootParameters_[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing_);


	// Samplerの設定
	staticSamplers_[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;							// バイリニアフィルタ
	staticSamplers_[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;						// 0~1の範囲外をリピート
	staticSamplers_[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers_[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers_[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;						// 比較しない
	staticSamplers_[0].MaxLOD = D3D12_FLOAT32_MAX;										// ありったけのMipmapｗｐ使う
	staticSamplers_[0].ShaderRegister = 0;												// レジスタ番号0を使う
	staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	descriptionRootSignature_.pStaticSamplers = staticSamplers_;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers_);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Logger(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成

	hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));


	// 2. InputLayoutの設定
	inputElementDescs_[0].SemanticName = "POSITION";
	inputElementDescs_[0].SemanticIndex = 0;
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[1].SemanticName = "TEXCOORD";
	inputElementDescs_[1].SemanticIndex = 0;
	inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[2].SemanticName = "NORMAL";
	inputElementDescs_[2].SemanticIndex = 0;
	inputElementDescs_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs_);

	// 3. BlendDtateの設定
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc_.RenderTarget[0].BlendEnable = true;

	SetBlendMode(blendDesc_, blendMode_);
	currentBlendMode_ = kBlendModeAdd;  // 現在のブレンドモード
	// α値のブレンド
	blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	// RasterrizerStateの設定
	// 裏面（時計回り）を表示しない  [カリング]
	rasterrizerDesc_.CullMode = D3D12_CULL_MODE_NONE;/* D3D12_CULL_MODE_*/
	// 三角形の中を塗りつぶす
	rasterrizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
	// 4. Shaderをコンパイルする
	vertexShaderBlob_ = dxCommon_->CompileShader(L"Resources/Shaders/Particle/Particle.VS.hlsl",
		L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);
	pixelShaderBlob_ = dxCommon_->CompileShader(L"Resources/Shaders/Particle/Particle.PS.hlsl",
		L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);

	// DepthStencilStateの設定
	// Depthの機能を有効化する
	depthStencilDesc_.DepthEnable = true;
	//書き込みします
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void ParticleManager::CreateGraphicsPipeline()
{
	// ルートシグネチャ
	CreateRootSignature();

	// パイプライン
	SetGraphicsPipeline();
}

void ParticleManager::CreateVertexResource()
{

	// インスタンス用のTransformationMatrixリソースを作る
	instancingResource_ = dxCommon_->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);
	// 書き込むためのアドレスを取得
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
	// 単位行列を書き込んでおく
	for (uint32_t index = 0; index < kNumMaxInstance; ++index) {
		instancingData_[index].WVP = MakeIdentity4x4();
		instancingData_[index].World = MakeIdentity4x4();
		instancingData_[index].color = { 1.0f, 1.0f, 1.0f, 1.0f };
	}

	// 四角形
	modelData_.vertices.push_back({ .position = {1.0f, 1.0f, 0.0f, 1.0f}, .texcoord = {0.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {-1.0f, 1.0f, 0.0f, 1.0f}, .texcoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {1.0f, -1.0f, 0.0f, 1.0f}, .texcoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {1.0f, -1.0f, 0.0f, 1.0f}, .texcoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {-1.0f, 1.0f, 0.0f, 1.0f}, .texcoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {-1.0f, -1.0f, 0.0f, 1.0f}, .texcoord = {1.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });

	// バッファビュー作成
	CreateVertexVBV();
}


void ParticleManager::CreateVertexVBV()
{
	//頂点リソース
	vertexResource_ = dxCommon_->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());
	// 頂点バッファービュー
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();// リソースデータの先頭アドレスから使う
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());// 使用するリソースのサイズは1頂点のサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);// 1頂点のサイズ
	//　データ書き込み
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
	vertexResource_->Unmap(0, nullptr);
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



void ParticleManager::SetGraphicsPipeline()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};//graphicsPipelineState_
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();					 // Rootsignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_;					 // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob_->GetBufferPointer(),
	vertexShaderBlob_->GetBufferSize() };										 // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(),
	pixelShaderBlob_->GetBufferSize() };											 // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc_;							 // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterrizerDesc_;   				 // RasterrizerState
	// Depthstencitの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc_;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むのか設定（気にしなくて良い）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));
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
	particleGroup.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroup.instancingData));


	srvManager_->CreateSRVforStructuredBuffer(particleGroup.srvIndex, particleGroup.instancingResource.Get(), kNumMaxInstance, sizeof(ParticleForGPU));
	// インスタンス数を初期化
	particleGroup.instance = 0;

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
	}

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
void ParticleManager::SetBlendMode(D3D12_BLEND_DESC& blendDesc, BlendMode blendMode)
{
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		blendDesc.RenderTarget[i].BlendEnable = TRUE;
		blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}
	switch (blendMode)
	{
	case kBlendModeNormal:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	case kBlendModeAdd:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		break;
	case kBlendModeSubtract:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		break;
	case kBlendModeMultiply:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
		break;
	case kBlendModeScreen:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		break;
	default:
		// 他のモードは処理なし
		 // kBlendModeNone や kBlendModeNormal の場合はデフォルト設定を使用
		for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			blendDesc.RenderTarget[i].BlendEnable = FALSE;
		}
		break;
	}
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


	jsonManagers_[name]->SetTreePrefix("位置");
	jsonManagers_[name]->Register("最小", &particleParameters_[name].baseTransform.translateMin);
	jsonManagers_[name]->Register("最大", &particleParameters_[name].baseTransform.translateMax);

	jsonManagers_[name]->SetTreePrefix("回転");
	jsonManagers_[name]->Register("最小", &particleParameters_[name].baseTransform.rotateMin);
	jsonManagers_[name]->Register("最大", &particleParameters_[name].baseTransform.rotateMax);

	jsonManagers_[name]->Register("スケール縮小", &particleParameters_[name].enableScale);

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

	// ---------------------- その他 ----------------------
	jsonManagers_[name]->SetTreePrefix("その他");
	jsonManagers_[name]->Register("オフセット", &particleParameters_[name].offset);
	jsonManagers_[name]->Register("ブレンドモード", reinterpret_cast<int*>(&currentBlendMode_));
	jsonManagers_[name]->Register("ビルボード", &particleParameters_[name].useBillboard);

	//jsonManagers_[name]->ClearTreePrefix();


}




// メインループの一部として呼び出す
void ParticleManager::Render(D3D12_BLEND_DESC& blendDesc, BlendMode& currentBlendMode)
{
	//ShowBlendModeDropdown(currentBlendMode);

	static BlendMode lastBlendMode = kBlendModeNone;

	//ShowBlendModeDropdown(currentBlendMode);

	if (currentBlendMode != lastBlendMode)
	{
		// ブレンドモードの変更を適用
		SetBlendMode(blendDesc, currentBlendMode);
		// PSOを再作成
		SetGraphicsPipeline();
		lastBlendMode = currentBlendMode;
	}

}
void ParticleManager::ShowUpdateModeDropdown()
{
}
