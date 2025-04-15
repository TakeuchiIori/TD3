#pragma once

// C++
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include <iostream>
#include <memory>

// Math
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"

// assimp
#include <assimp/material.h>


class DirectXCommon;
class Material
{
public:
	/*=================================================================
	 
								構造体

	=================================================================*/
	struct MaterialData {
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
		float shininess;
		bool enableSpecular;
		bool isHalfVector;
	};

	struct MtlData {
		std::string name;
		float Ns;
		Vector3 Ka;	// 環境光色
		Vector3 Kd;	// 拡散反射色
		Vector3 Ks;	// 鏡面反射光
		float Ni;
		float d;
		uint32_t illum;
		std::string textureFilePath;
		uint32_t textureIndex = 0;
	};

public:
	/*=================================================================

								メンバ関数

	=================================================================*/	

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(std::string& textureFilePath);

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="command"></param>
	/// <param name="rootParameterIndexCBV"></param>
	/// <param name="rootParameterIndexSRV"></param>
	void RecordDrawCommands(ID3D12GraphicsCommandList* command, UINT rootParameterIndexCBV, UINT rootParameterIndexSRV);


	// Material.h
	static std::shared_ptr<Material> CreateFromAiMaterial(aiMaterial* src, const std::string& directoryPath, uint32_t materialIndex);

	/// <summary>
	/// データ転送
	/// </summary>
	void TransferData();

private:


	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	void LoadTexture();
public:
	/*=================================================================

								アクセッサ

	=================================================================*/

	/////
	///// Lighting
	///// 
	//bool IsLightingEnabled() const { return materialData_->enableLighting != 0; }
	//void SetLightingEnabled(bool enabled) { materialData_->enableLighting = enabled ? 1 : 0; }

	/////
	///// Specular
	///// 
	//bool IsSpecularEnabled() const { return materialData_->enableSpecular; }
	//void SetMaterialSpecularEnabled(bool enable) { materialData_->enableSpecular = enable; }

	/////
	///// HalfVector
	///// 
	//bool IsHalfVectorEnabled() const { return materialData_->isHalfVector; }
	//void SetMaterialHalfVectorEnabled(bool enable) { materialData_->isHalfVector = enable; }

	/////
	///// Shininess
	/////
	//float GetMaterialShininess() const { return materialData_->shininess; }
	//void SetMaterialShininess(float shininess) { materialData_->shininess = shininess; }

	/////
	///// UV Transform
	///// 
	//const Matrix4x4& GetMaterialUVTransform() const { return materialData_->uvTransform; }
	//void SetMaterialUVTransform(const Matrix4x4& uvTransform) { materialData_->uvTransform = uvTransform; }

	/////
	///// Material Enable
	///// 
	//bool IsMaterialEnabled() const { return materialData_->enableLighting != 0; }
	//void SetMaterialEnabled(bool enable) { materialData_->enableLighting = enable; }

	///
	/// Resource
	/// 
	ID3D12Resource* GetMaterialResource() { return materialResource_.Get(); }
	MaterialData* GetMaterialData() { return materialData_; }


	///
	/// MTL Data
	///
	const std::string& GetName() const { return mtlData_.name; }
	void SetName(const std::string& name) { mtlData_.name = name; }

	float GetNs() const { return mtlData_.Ns; }
	void SetNs(float ns) { mtlData_.Ns = ns; }

	const Vector3& GetKa() const { return mtlData_.Ka; }
	void SetKa(const Vector3& ka) { mtlData_.Ka = ka; }

	const Vector3& GetKd() const { return mtlData_.Kd; }
	void SetKd(const Vector3& kd) { mtlData_.Kd = kd; }

	const Vector3& GetKs() const { return mtlData_.Ks; }
	void SetKs(const Vector3& ks) { mtlData_.Ks = ks; }

	float GetNi() const { return mtlData_.Ni; }
	void SetNi(float ni) { mtlData_.Ni = ni; }

	float GetD() const { return mtlData_.d; }
	void SetD(float d) { mtlData_.d = d; }

	uint32_t GetIllum() const { return mtlData_.illum; }
	void SetIllum(uint32_t illum) { mtlData_.illum = illum; }

	const std::string& GetTextureFilePath() const { return mtlData_.textureFilePath; }
	void SetTextureFilePath(const std::string& path) { mtlData_.textureFilePath = path; }

	uint32_t GetTextureIndex() const { return mtlData_.textureIndex; }
	void SetTextureIndex(uint32_t index) { mtlData_.textureIndex = index; }



private:
	/*=================================================================
	
								ポインタ

	=================================================================*/
	DirectXCommon* dxCommon_ = nullptr;



	/*=================================================================

								リソース作成

	=================================================================*/

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	MaterialData* materialData_ = nullptr;
	MtlData mtlData_;
	std::string textureFilePath_;

};

