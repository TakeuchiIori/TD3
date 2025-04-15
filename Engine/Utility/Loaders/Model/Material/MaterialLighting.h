#pragma once

// C++
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>


// Math
#include "Matrix4x4.h"



class MaterialLighting
{
public:
	struct MaterialLight {
		int32_t enableLighting;
		float shininess;
		bool enableSpecular;
		bool isHalfVector;
	};


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// コマンドリスト積みます
	/// </summary>
	/// <param name="command"></param>
	/// <param name="rootParameterIndexCBV"></param>
	/// <param name="rootParameterIndexSRV"></param>
	void RecordDrawCommands(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndexCBV);

public:
	/*=================================================================

								アクセッサ

	=================================================================*/





private:

	/*=================================================================

								ポインタ

	=================================================================*/

	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	MaterialLight* materialLight_ = nullptr;

};

