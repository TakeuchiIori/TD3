#pragma once

// C++
#include <optional>
#include <map>
#include <vector>
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <vector>
#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <span>

// Engine
#include "Joint.h"
#include "Node.h"
#include "Skeleton.h"
#include "Joint.h"


// Math
#include "Quaternion.h"
#include "Vector3.h"



class SkinCluster
{
public:

	struct VertexWeightData {
		float weight;
		uint32_t vertexIndex;
	};
	struct JointWeightData {
		Matrix4x4 inverseBindPoseMatrix;
		std::vector<VertexWeightData> vertexWeights;
	};
	// インフルエンス
	const static uint32_t kNumMaxInfluence = 4;
	struct VertexInfluence {
		std::array<float, kNumMaxInfluence> weights;
		std::array<int32_t, kNumMaxInfluence> jointindices;
	};
	// マトリックスパレット
	struct WellForGPU {
		Matrix4x4 skeletonSpaceMatrix;  // 位置用
		Matrix4x4 skeletonSpaceInverseTransposeMatrix; // 法線用
	};

public:


	/// <summary>
	/// 更新
	/// </summary>
	void Update(std::vector<Joint> joints_);

	/// <summary>
	/// リソース
	/// </summary>
	void CreateResource(size_t jointsSize,size_t verticesSize, std::map<std::string, int32_t> jointMap);

	void LoadFromScene(const aiScene* scene);

public:

	// skinClusterData_
	const std::map<std::string, JointWeightData>& GetSkinClusterData() const { return skinClusterData_; }
	void SetSkinClusterData(const std::map<std::string, JointWeightData>& data) { skinClusterData_ = data; }

	// inverseBindposeMatrices_
	const std::vector<Matrix4x4>& GetInverseBindposeMatrices() const { return inverseBindposeMatrices_; }
	void SetInverseBindposeMatrices(const std::vector<Matrix4x4>& matrices) { inverseBindposeMatrices_ = matrices; }

	// influenceResource_
	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetInfluenceResource() const { return influenceResource_; }
	void SetInfluenceResource(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource) { influenceResource_ = resource; }

	// influenceBufferView_
	const D3D12_VERTEX_BUFFER_VIEW& GetInfluenceBufferView() const { return influenceBufferView_; }
	void SetInfluenceBufferView(const D3D12_VERTEX_BUFFER_VIEW& view) { influenceBufferView_ = view; }

	// mappedInfluence_
	std::span<VertexInfluence> GetMappedInfluence() const { return mappedInfluence_; }
	void SetMappedInfluence(std::span<VertexInfluence> influence) { mappedInfluence_ = influence; }

	// influSRVIndex_
	uint32_t GetInfluSRVIndex() const { return influSRVIndex_; }
	void SetInfluSRVIndex(uint32_t index) { influSRVIndex_ = index; }

	// paletteResource_
	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetPaletteResource() const { return paletteResource_; }
	void SetPaletteResource(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource) { paletteResource_ = resource; }

	// mappedPalette_
	std::span<WellForGPU> GetMappedPalette() const { return mappedPalette_; }
	void SetMappedPalette(std::span<WellForGPU> palette) { mappedPalette_ = palette; }

	// paletteSrvHandle_
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> GetPaletteSrvHandle() const { return paletteSrvHandle_; }
	void SetPaletteSrvHandle(std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> handle) { paletteSrvHandle_ = handle; }

	// srvIndex_
	uint32_t GetSRVIndex() const { return srvIndex_; }
	void SetSRVIndex(uint32_t index) { srvIndex_ = index; }


private:

	std::map<std::string, JointWeightData> skinClusterData_;
	std::vector<Matrix4x4> inverseBindposeMatrices_;
	Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource_;
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView_;
	std::span<VertexInfluence> mappedInfluence_;
	uint32_t influSRVIndex_;
	Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource_;
	std::span<WellForGPU> mappedPalette_;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle_;
	uint32_t srvIndex_;

};

