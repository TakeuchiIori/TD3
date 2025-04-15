#include "MaterialLighting.h"
#include "../Core/DX/DirectXCommon.h"

void MaterialLighting::Initialize()
{
	resource_ = DirectXCommon::GetInstance()->CreateBufferResource(sizeof(MaterialLight));
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&materialLight_));
	materialLight_->enableLighting = true;
	materialLight_->shininess = 30.0f;
	materialLight_->enableSpecular = false;
	materialLight_->isHalfVector = false;


}

void MaterialLighting::RecordDrawCommands(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndexCBV)
{
	commandList->SetGraphicsRootConstantBufferView(rootParameterIndexCBV, resource_->GetGPUVirtualAddress());
}
