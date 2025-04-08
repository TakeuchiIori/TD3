#pragma once

// C++
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include <optional>
#include <span>
#include <algorithm>

// Engine
#include "DX./DirectXCommon.h"
#include "WorldTransform./WorldTransform.h"

#include "Material/Material.h"
#include "Material/MaterialManager.h"
#include "Mesh/Mesh.h"
#include "Node.h"

// Math
#include "MathFunc.h"
#include "Quaternion.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"

// assimp
#include <assimp/scene.h>
#include <map>
#include "Animation/AnimationSystem.h"


class SrvManager;
class Line;
class ModelCommon;
class Model
{
public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename ,bool isAnimation = false);

	/// <summary>
	/// 更新処理
	/// </summary>
	void UpdateAnimation();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	void Draw(Vector4 color);
	void DrawBone(Line& line);

private:
	/*=================================================================

								解析関数

	=================================================================*/
	void LoadModelIndexFile(const std::string& directoryPath, const std::string& filename);
	void LoadAnimationFile(const std::string& directoryPath, const std::string& filename);
	void LoadNode(const aiScene* scene);
	void ApplyNodeTransform(const aiScene* scene, const aiNode* node, const Matrix4x4& parentMatrix);
	void LoadMesh(const aiScene* scene);
	void LoadMaterial(const aiScene* scene, std::string directoryPath);
	void LoadSkinCluster(const aiScene* scene);
	static bool HasBones(const aiScene* scene);


public:
	/*=================================================================

							アクセッサ

	=================================================================*/

	bool GetHasBones() { return hasBones_; }
	Node GetRootNode() { return *rootNode_; }


	/// <summary>
	/// アニメーションを切り替える
	/// </summary>
	void SetChangeAnimation(const std::string& directoryPath, const std::string& filename);


private: 
	/*=================================================================

							ポインタ

	=================================================================*/
	ModelCommon* modelCommon_;
	std::vector<std::unique_ptr<Mesh>> meshes_;
	std::vector<std::unique_ptr<Material>> materials_;
	std::unique_ptr<AnimationSystem> animationSystem_;
	std::unique_ptr<Skeleton> skeleton_;
	std::unique_ptr<SkinCluster> skinCluster_;
	std::unique_ptr<Node> rootNode_;
	Animation animation_;



private: 

	SrvManager* srvManager_ = nullptr;
	std::string name_ = {};
	static const std::string binPath;
	std::map<std::string, Animation> animationCache_;

	bool hasBones_ = false;
	bool isAnimation_;

};
