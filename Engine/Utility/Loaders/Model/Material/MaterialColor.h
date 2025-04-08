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



class MaterialColor
{
public:

	struct ColorData {

		Vector4 color;

	};;


	void Initialize();

	void RecordDrawCommands(ID3D12GraphicsCommandList* commandList, UINT index);

	void SetColor(const Vector4& color) { colorData_->color = color; }
	const Vector4& GetColor() const { return colorData_->color; }


private:

	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	ColorData* colorData_ = nullptr;

	
};

