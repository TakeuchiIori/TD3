#include "Material.h"
#include "../Core/DX/DirectXCommon.h"
#include <Loaders/Texture/TextureManager.h>
void Material::Initialize(std::string& textureFilePath)
{
	dxCommon_ = DirectXCommon::GetInstance();
	textureFilePath_ = textureFilePath;

	materialResource_ = dxCommon_->CreateBufferResource(sizeof(Material));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	SetTextureFilePath(textureFilePath_);

	TransferData();

	LoadTexture();
}

void Material::RecordDrawCommands(ID3D12GraphicsCommandList* command, UINT rootParameterIndexCBV, UINT rootParameterIndexSRV)
{
	// 元々は0番
	command->SetGraphicsRootConstantBufferView(rootParameterIndexCBV, materialResource_->GetGPUVirtualAddress());
	// 元々は2番
	command->SetGraphicsRootDescriptorTable(rootParameterIndexSRV, TextureManager::GetInstance()->GetsrvHandleGPU(mtlData_.textureFilePath));
}


std::shared_ptr<Material> Material::CreateFromAiMaterial(aiMaterial* src, const std::string& directoryPath, uint32_t materialIndex) {
	auto mat = std::make_shared<Material>();

	aiString name;
	std::string fullPath;
	if (src->Get(AI_MATKEY_NAME, name) == AI_SUCCESS) {
		mat->SetName(name.C_Str());
	}

	aiColor3D col;

	if (src->Get(AI_MATKEY_COLOR_AMBIENT, col) == AI_SUCCESS) {
		mat->SetKa({ col.r, col.g, col.b });
	}
	if (src->Get(AI_MATKEY_COLOR_SPECULAR, col) == AI_SUCCESS) {
		mat->SetKs({ col.r, col.g, col.b });
	}

	float shininess = 0.0f;
	if (src->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
		mat->SetMaterialShininess(shininess);
		mat->SetNs(shininess);
	}

	aiString textureFilePath;
	if (src->GetTextureCount(aiTextureType_DIFFUSE) > 0 &&
		src->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath) == AI_SUCCESS) {
		std::string fullPath = directoryPath + "/" + textureFilePath.C_Str();
		//mat->SetTextureFilePath(fullPath);
	}

	mat->Initialize(fullPath);
	return mat;
}


void Material::TransferData()
{
	materialData_->enableLighting = true;
	materialData_->shininess = 30.0f;
	materialData_->uvTransform = MakeIdentity4x4();
}

void Material::LoadTexture()
{
	TextureManager::GetInstance()->LoadTexture(mtlData_.textureFilePath);
}
