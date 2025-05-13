#include "OffScreen.h"
#include "../Core/DX/DirectXCommon.h"
#include "PipelineManager/PipelineManager.h"

void OffScreen::Initialize()
{
	dxCommon_ = DirectXCommon::GetInstance();

	// 全種類のエフェクト用PSO/RSを一括で登録
	auto pipelineManager = PipelineManager::GetInstance();

	auto Register = [&](OffScreenEffectType type, const std::string& name) {
		OffScreenPipeline p;
		p.rootSignature = pipelineManager->GetRootSignature(name);
		p.pipelineState = pipelineManager->GetPipeLineStateObject(name);
		pipelineMap_[type] = p;
		};

	Register(OffScreenEffectType::Copy, "OffScreen");
	Register(OffScreenEffectType::GaussSmoothing, "GaussSmoothing");
	Register(OffScreenEffectType::DepthOutline, "DepthOutLine");
	Register(OffScreenEffectType::Sepia, "Sepia");
	Register(OffScreenEffectType::Grayscale, "Grayscale");
	Register(OffScreenEffectType::Vignette, "Vignette");
	Register(OffScreenEffectType::RadialBlur, "RadialBlur");

	CreateMaterialResource();
	CreateRadialBlurResource();
}


void OffScreen::Draw()
{
	auto& pipeline = pipelineMap_[effectType_];
	dxCommon_->GetCommandList()->SetPipelineState(pipeline.pipelineState.Get());
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(pipeline.rootSignature.Get());
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	materialData_->Inverse = Inverse(projectionInverse_);

	// 各種 RootParameter 設定（省略せず維持）
	switch (effectType_) {
	case OffScreenEffectType::Copy:
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, dxCommon_->GetOffScreenGPUHandle());
		break;
	case OffScreenEffectType::GaussSmoothing:
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, dxCommon_->GetOffScreenGPUHandle());
		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, gaussResource_->GetGPUVirtualAddress());
		break;
	case OffScreenEffectType::DepthOutline:
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, dxCommon_->GetOffScreenGPUHandle());
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(1, dxCommon_->GetDepthGPUHandle());
		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(2, materialResource_->GetGPUVirtualAddress());
		break;
	case OffScreenEffectType::Sepia:
	case OffScreenEffectType::Grayscale:
	case OffScreenEffectType::Vignette:
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, dxCommon_->GetOffScreenGPUHandle());
		break;
	case OffScreenEffectType::RadialBlur:
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, dxCommon_->GetOffScreenGPUHandle());
		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, radialBlurResource_->GetGPUVirtualAddress());
		break;
	}

	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}



void OffScreen::CreateBoxFilterResource()
{
	boxResource_ = dxCommon_->CreateBufferResource(sizeof(KernelForGPU));
	boxResource_->Map(0, nullptr, reinterpret_cast<void**>(&boxData_));
	boxResource_->Unmap(0, nullptr);
	boxData_->kernelSize = 5;
}

void OffScreen::CreateGaussFilterResource()
{
	gaussResource_ = dxCommon_->CreateBufferResource(sizeof(GaussKernelForGPU));
	gaussResource_->Map(0, nullptr, reinterpret_cast<void**>(&gaussData_));
	gaussResource_->Unmap(0, nullptr);
	gaussData_->kernelSize = 3;
	gaussData_->sigma = 2.0f;
}

void OffScreen::CreateMaterialResource()
{
	materialResource_ = dxCommon_->CreateBufferResource(sizeof(Material));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialResource_->Unmap(0, nullptr);
	materialData_->Inverse = MakeIdentity4x4();
	materialData_->kernelSize = 5;
	materialData_->outlineColor = { 1.0f, 0.0f, 1.0f, 1.0f };
}

void OffScreen::CreateRadialBlurResource()
{
	radialBlurResource_ = dxCommon_->CreateBufferResource(sizeof(RadialBlurForGPU));
	radialBlurResource_->Map(0, nullptr, reinterpret_cast<void**>(&radialBlurData_));
	radialBlurData_->direction = { 0.0f, 0.0f };
	radialBlurData_->center = { 0.5f, 0.5f };
	radialBlurData_->width = 0.001f;
	radialBlurData_->sampleCount = 10;
	radialBlurData_->isRadial = true;
	radialBlurResource_->Unmap(0, nullptr);
}


/*=================================================================

					 　	実際にゲームで扱う関数

=================================================================*/

void OffScreen::UpdateBlur(float deltaTime)
{
	if (isBlurMotion_) {
		blurTime_ += deltaTime;
		float t = std::clamp(blurTime_ / blurDuration_, 0.0f, 1.0f);
		float easeT = 1.0f - t; // 最初1.0 → 最後0.0

		// 幅とサンプル数を減衰させる
		radialBlurData_->width = initialWidth_ * easeT;
		radialBlurData_->sampleCount = (std::max)(1, static_cast<int>(initialSampleCount_ * easeT));

		if (t >= 1.0f) {
			isBlurMotion_ = false;
			SetEffectType(OffScreenEffectType::Copy); // 通常状態に戻す
		}
	}
}

void OffScreen::StartBlurMotion(RadialBlurPrams radialBlurPrams)
{
	radialBlurPrams_ = radialBlurPrams;

	// 初期値を設定
	blurDuration_ = 1.0f;
	blurTime_ = 0.0f;
	isBlurMotion_ = true;
	
	// GPUに値を転送
	radialBlurData_->direction = radialBlurPrams_.direction;
	radialBlurData_->center = radialBlurPrams_.center;
	radialBlurData_->width = radialBlurPrams_.width;
	radialBlurData_->sampleCount = radialBlurPrams_.sampleCount;
	radialBlurData_->isRadial = radialBlurPrams_.isRadial;

	SetEffectType(OffScreenEffectType::RadialBlur);
}

