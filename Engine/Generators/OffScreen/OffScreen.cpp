#include "OffScreen.h"
#include "../Core/DX/DirectXCommon.h"
#include "PipelineManager/PipelineManager.h"

void OffScreen::Initialize()
{
	dxCommon_ = DirectXCommon::GetInstance();

	switch (effectType_) {
	case OffScreenEffectType::Copy:
		rootSignature_ = PipelineManager::GetInstance()->GetRootSignature("OffScreen");
		pipelineState_ = PipelineManager::GetInstance()->GetPipeLineStateObject("OffScreen");
		break;
	case OffScreenEffectType::GaussSmoothing:
		rootSignature_ = PipelineManager::GetInstance()->GetRootSignature("OffScreen_GaussSmoothing");
		pipelineState_ = PipelineManager::GetInstance()->GetPipeLineStateObject("OffScreen_GaussSmoothing");
		break;
	case OffScreenEffectType::DepthOutline:
		rootSignature_ = PipelineManager::GetInstance()->GetRootSignature("OffScreen_DepthOutLine");
		pipelineState_ = PipelineManager::GetInstance()->GetPipeLineStateObject("OffScreen_DepthOutLine");
		break;
	case OffScreenEffectType::Sepia:
		rootSignature_ = PipelineManager::GetInstance()->GetRootSignature("OffScreen_Sepia");
		pipelineState_ = PipelineManager::GetInstance()->GetPipeLineStateObject("OffScreen_Sepia");
		break;
	case OffScreenEffectType::Grayscale:
		rootSignature_ = PipelineManager::GetInstance()->GetRootSignature("OffScreen_Grayscale");
		pipelineState_ = PipelineManager::GetInstance()->GetPipeLineStateObject("OffScreen_Grayscale");
		break;
	case OffScreenEffectType::Vignette:
		rootSignature_ = PipelineManager::GetInstance()->GetRootSignature("OffScreen_Vignette");
		pipelineState_ = PipelineManager::GetInstance()->GetPipeLineStateObject("OffScreen_Vignette");
		break;
	}

	CreateMaterialResource();
}




void OffScreen::Draw()
{
	// 共通設定
	materialData_->Inverse = Inverse(projectionInverse_);
	dxCommon_->GetCommandList()->SetPipelineState(pipelineState_.Get());
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	switch (effectType_) {
	case OffScreenEffectType::Copy:
		// RootParameter = 1つ: テクスチャのみ
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, dxCommon_->GetOffScreenGPUHandle());
		break;

	case OffScreenEffectType::GaussSmoothing:
		// RootParameter = 2つ: テクスチャ + ガウスパラメータ
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, dxCommon_->GetOffScreenGPUHandle());
		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, gaussResource_->GetGPUVirtualAddress());
		break;

	case OffScreenEffectType::DepthOutline:
		// RootParameter = 3つ: テクスチャ + Depth + Material
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, dxCommon_->GetOffScreenGPUHandle());
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(1, dxCommon_->GetDepthGPUHandle());
		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(2, materialResource_->GetGPUVirtualAddress());
		break;

	case OffScreenEffectType::Sepia:
	case OffScreenEffectType::Grayscale:
	case OffScreenEffectType::Vignette:
		// BaseOffScreen系: RootParameter = 1つのみ（テクスチャ）
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, dxCommon_->GetOffScreenGPUHandle());
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

