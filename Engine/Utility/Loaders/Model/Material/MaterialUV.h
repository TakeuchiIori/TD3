#pragma once

// C++
#include <wrl.h>
#include <d3d12.h>


// Math
#include "Matrix4x4.h"



class MaterialUV
{
public:
	struct MaterialUVData {
		Matrix4x4 uvTransform;
		//float padding[3];
	};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// コマンドリストにセット
	/// </summary>
	/// <param name="commandList"></param>
	/// <param name="rootParameterIndexCBV"></param>
	void RecordDrawCommands(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndexCBV);




public:

	void SetUVTransform(const Matrix4x4& uvTransform){materialUV_->uvTransform = uvTransform;}

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	MaterialUVData* materialUV_ = nullptr;
};

