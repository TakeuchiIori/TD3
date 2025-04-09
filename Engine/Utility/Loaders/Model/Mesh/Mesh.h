#pragma once

// C++
#include <wrl.h>
#include <d3d12.h>
#include <vector>
#include "../SkinCluster.h"


// Math
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

// Forward
class DirectXCommon;

class SkinCluster;
class Mesh {
public:
    // 頂点データ構造
    struct VertexData {
        Vector4 position;
        Vector2 texcoord;
        Vector3 normal;
    };

    // メッシュデータ（CPU側）
    struct MeshData {
        std::vector<VertexData> vertices;
        std::vector<uint32_t> indices;
        uint32_t materialIndex = 0;
    };

    // GPU用リソース
    struct MeshResource {
        Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
        Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
        D3D12_INDEX_BUFFER_VIEW indexBufferView{};
    };

public:
    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize();
	void Initialize(const std::vector<VertexData>& vertices, const std::vector<uint32_t>& indices);

	void RecordDrawCommands(ID3D12GraphicsCommandList* command);
    void RecordDrawCommands(ID3D12GraphicsCommandList* command, D3D12_VERTEX_BUFFER_VIEW vbv);
    void RecordDrawCommands(ID3D12GraphicsCommandList* command, SkinCluster& skinCluster);
	/// <summary>
	/// GPUへ転送
	/// </summary>
	void TransferData();



private:
    /// <summary>
    /// リソース生成
    /// </summary>
    void InitResources();
    
public:
    MeshData& GetMeshData() { return meshData_; }
    const MeshData& GetMeshData() const { return meshData_; }

    const MeshResource& GetMeshResource() const { return meshResources_; }

    VertexData* GetVertexData() const { return vertexData_; }
    uint32_t* GetIndexData() const { return indexData_; }

    uint32_t GetMaterialIndex() const { return meshData_.materialIndex; }
    void SetMaterialIndex(uint32_t index) { meshData_.materialIndex = index; }

	uint32_t GetVertexCount() const { return static_cast<uint32_t>(meshData_.vertices.size()); }
	uint32_t GetIndexCount() const { return static_cast<uint32_t>(meshData_.indices.size()); }

    void SetWorldMatrix(const Matrix4x4& mtx) { worldMatrix_ = mtx; }
    const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }


private:
    // システム参照
    DirectXCommon* dxCommon_ = nullptr;

    // データ
    MeshData meshData_;
    MeshResource meshResources_;
    VertexData* vertexData_ = nullptr;
    uint32_t* indexData_ = nullptr;
    Matrix4x4 worldMatrix_;
};
