
// Engine
#include "Model.h"
#include "ModelCommon.h"
#include "SrvManager/SrvManager.h"
#include "Loaders./Texture./TextureManager.h"
#include "Drawer./LineManager/Line.h"
#include "ModelUtils.h"

// C++
#include <assert.h>
#include <fstream>
#include <sstream>


// assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <json.hpp>
#include <fstream>
#include <iostream>

const std::string Model::binPath = "Resources/Bin/";

void Model::Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename, bool isAnimation)
{
	isAnimation_ = isAnimation;
	// 引数から受け取ってメンバ変数に記録する
	modelCommon_ = modelCommon;

	srvManager_ = SrvManager::GetInstance();

	// モデル読み込み
	LoadModelIndexFile(directorypath, filename);

	animationSystem_ = std::make_unique<AnimationSystem>();

	LoadAnimationFile(directorypath, filename);


	if (hasBones_) {
		// 骨の作成
		skeleton_ = std::make_unique<Skeleton>();
		skeleton_->Create(*rootNode_);
		skinCluster_->CreateResource(skeleton_->GetJoints().size(), meshes_[0]->GetVertexCount(), skeleton_->GetJointMap());
		animationSystem_->Initialize(animation_, *skeleton_, *skinCluster_, rootNode_.get());

	} else {
		animationSystem_->Initialize(animation_, rootNode_.get());
	}



}

void Model::ChangeModel(const std::string& directoryPath, const std::string& filename, bool isAnimation)
{
	meshes_.clear();
	materials_.clear();
	rootNode_.reset();
	skinCluster_.reset();
	skeleton_.reset();
	animation_.Reset();

	isAnimation_ = isAnimation;

	LoadModelIndexFile(directoryPath, filename);

	animationSystem_ = std::make_unique<AnimationSystem>();


	LoadAnimationFile(directoryPath, filename);

	if (hasBones_) {
		skeleton_ = std::make_unique<Skeleton>();
		skeleton_->Create(*rootNode_);
		skinCluster_->CreateResource(skeleton_->GetJoints().size(), meshes_[0]->GetVertexCount(), skeleton_->GetJointMap());
		animationSystem_->Initialize(animation_, *skeleton_, *skinCluster_, rootNode_.get());
	} else {
		animationSystem_->Initialize(animation_, rootNode_.get());
	}

	// ★ アニメーション再生を自動で行うようにする
	animationSystem_->ResetPlay();
	animationSystem_->RequestPlay();
	animationSystem_->ResetPoseCache();
}

void Model::ChangeModelAnimation(const std::string& directoryPath, const std::string& filename, int count)
{
	meshes_.clear();
	materials_.clear();
	rootNode_.reset();
	skinCluster_.reset();
	skeleton_.reset();
	animation_.Reset();

	LoadModelIndexFile(directoryPath, filename);

	animationSystem_ = std::make_unique<AnimationSystem>();


	LoadAnimationFile(directoryPath, filename);

	if (hasBones_) {
		skeleton_ = std::make_unique<Skeleton>();
		skeleton_->Create(*rootNode_);
		skinCluster_->CreateResource(skeleton_->GetJoints().size(), meshes_[0]->GetVertexCount(), skeleton_->GetJointMap());
		animationSystem_->Initialize(animation_, *skeleton_, *skinCluster_, rootNode_.get());
	} else {
		animationSystem_->Initialize(animation_, rootNode_.get());
	}

	// ★ アニメーション再生を自動で行うようにする
	animationSystem_->SetLoopCount(count);
	animationSystem_->RequestPlay();
	animationSystem_->ResetPoseCache();
}


void Model::UpdateAnimation()
{
	if (!animationSystem_) return;

	animationSystem_->Update(1.0f / 60.0f);

	// Apply は「再生中だけ」に呼ぶ
	if (!animationSystem_->IsPlayFinished()) {
		animationSystem_->Apply();
	}
}

void Model::PlayAnimation() {
	if (animationSystem_) {
		animationSystem_->RequestPlay();
	}
}

bool Model::IsAnimationPlayFinished() const {
	if (animationSystem_) {
		return animationSystem_->IsPlayFinished();
	}
	return true;
}

void Model::ResetAnimationPlay() {
	if (animationSystem_) {
		animationSystem_->ResetPlay();
	}
}


void Model::Draw()
{

	auto commandList = modelCommon_->GetDxCommon()->GetCommandList().Get();

	for (auto& mesh : meshes_) {
		if (hasBones_) {
			mesh->RecordDrawCommands(commandList, skinCluster_->GetInfluenceBufferView());
			mesh->RecordDrawCommands(commandList, *skinCluster_);
		} else {
			mesh->RecordDrawCommands(commandList);
		}

		//materials_[mesh->GetMaterialIndex()]->TransferData();
		materials_[mesh->GetMaterialIndex()]->RecordDrawCommands(commandList, 0, 2);
		commandList->DrawIndexedInstanced(mesh->GetIndexCount(), 1, 0, 0, 0);
	}

}

void Model::Draw(Vector4 color)
{
	auto commandList = modelCommon_->GetDxCommon()->GetCommandList().Get();

	for (auto& mesh : meshes_) {
		if (hasBones_) {
			mesh->RecordDrawCommands(commandList, skinCluster_->GetInfluenceBufferView());
			mesh->RecordDrawCommands(commandList, *skinCluster_);
		} else {
			mesh->RecordDrawCommands(commandList);
		}

		//materials_[mesh->GetMaterialIndex()]->TransferData();
		materials_[mesh->GetMaterialIndex()]->RecordDrawCommands(commandList, 0, 2);
		commandList->DrawIndexedInstanced(mesh->GetIndexCount(), 1, 0, 0, 0);
	}
}


void Model::LoadModelIndexFile(const std::string& directoryPath, const std::string& filename)
{
	// ファイル読み込み
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());
	LoadNode(scene);
	hasBones_ = HasBones(scene);
	LoadMesh(scene);
	LoadMaterial(scene, directoryPath);
	if (hasBones_) {
		LoadSkinCluster(scene);
	}
	//ApplyNodeTransform(scene, scene->mRootNode, MakeIdentity4x4());
}

void Model::LoadAnimationFile(const std::string& directoryPath, const std::string& filename)
{
	std::string fullPath = directoryPath + "/" + filename;

	// キャッシュファイル名を拡張子変えて作る
	std::filesystem::path fileNameOnly(filename);
	fileNameOnly.replace_extension(".anim");

	std::string cachePath = binPath + fileNameOnly.string();

	// 既にキャッシュがあれば読み込む
	if (animationCache_.contains(fullPath)) {
		animation_ = animationCache_.at(fullPath);
		return;
	}

	// バイナリファイル読み込み
	if (std::filesystem::exists(cachePath)) {
		std::cout << "[LOAD] Reading animation from cache: " << cachePath << std::endl;
		animation_ = Animation::LoadFromBinary(cachePath);
		animationCache_[fullPath] = animation_;
		return;
	}

	// なければAssimpで読み込んで保存
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fullPath.c_str(), 0);
	if (!scene || scene->mNumAnimations == 0) {
		isAnimation_ = false;
		return;
	}

	// アニメーションの読み込み
	animation_ = Animation::LoadFromScene(scene, fullPath);

	// バイナリファイルに保存・ファイル作成
	animation_.SaveToBinary(cachePath);
	animationCache_[fullPath] = animation_;
	isAnimation_ = true;
}


bool Model::HasBones(const aiScene* scene)
{
	// メッシュがなければボーンもない
	if (!scene->HasMeshes()) {
		return false;
	}

	// 各メッシュをチェック
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];

		// ボーン数が0でなければボーンがあると判断
		if (mesh->mNumBones > 0) {
			return true;
		}
	}

	// どのメッシュにもボーンが含まれていない場合
	return false;
}

void Model::SetChangeAnimation(const std::string& directoryPath, const std::string& filename)
{
	// 新しいアニメーションファイルを読み込む
	LoadAnimationFile(directoryPath, filename);

	// AnimationSystem を再初期化
	if (hasBones_) {
		//animationSystem_->Initialize(animation_, *skeleton_, *skinCluster_, rootNode_.get());
		animationSystem_->StartBlend(animation_, 0.2f);
	} else {
		animationSystem_->Initialize(animation_, rootNode_.get());
	}
}

void Model::DrawBone(Line& line)
{
	if (skeleton_) {
		skeleton_->Draw(line);
	} else {
		//OutputDebugStringA("DrawBone: No skeleton path\n");
	}
}

void Model::LoadMesh(const aiScene* scene)
{
	meshes_.resize(scene->mNumMeshes); // メッシュ数分のメモリを確保
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {

		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());        // 法線が無い場合のMeshは非対応
		assert(mesh->HasTextureCoords(0)); // Texcoordがない場合は非対応

		std::unique_ptr<Mesh> meshs = std::make_unique<Mesh>();
		// 初期化（リソース確保、転送など）
		meshs->Initialize();

		// メッシュデータの取得と頂点バッファのサイズ設定
		auto& meshData = meshs->GetMeshData();
		meshData.vertices.resize(mesh->mNumVertices);

		// 頂点データの設定
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoords = mesh->mTextureCoords[0][vertexIndex];

			// 右手系->左手系への変換を考慮して頂点データを設定
			meshData.vertices[vertexIndex].position = { -position.x, position.y, position.z, 1.0f };
			meshData.vertices[vertexIndex].normal = { -normal.x, normal.y, normal.z };
			meshData.vertices[vertexIndex].texcoord = { texcoords.x, texcoords.y };
		}

		// インデックスデータの設定
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			if (face.mNumIndices == 4) {
				// 四角形の場合、2つの三角形に分割
				uint32_t i0 = face.mIndices[0];
				uint32_t i1 = face.mIndices[1];
				uint32_t i2 = face.mIndices[2];
				uint32_t i3 = face.mIndices[3];

				// 1つ目の三角形
				meshData.indices.push_back(i0);
				meshData.indices.push_back(i1);
				meshData.indices.push_back(i2);

				// 2つ目の三角形
				meshData.indices.push_back(i0);
				meshData.indices.push_back(i2);
				meshData.indices.push_back(i3);
			} else if (face.mNumIndices == 3) {
				// 三角形の場合はそのまま追加
				for (uint32_t element = 0; element < face.mNumIndices; ++element) {
					meshData.indices.push_back(face.mIndices[element]);
				}
			}
		}

		// マテリアルインデックスを設定（マルチマテリアル対応）
		meshData.materialIndex = mesh->mMaterialIndex;

		meshs->SetMaterialIndex(mesh->mMaterialIndex);
		meshs->TransferData();

		meshes_[meshIndex] = std::move(meshs); // メッシュを格納
	}
}

void Model::LoadMaterial(const aiScene* scene, std::string directoryPath)
{
	materials_.resize(scene->mNumMaterials);

	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* materialSrc = scene->mMaterials[materialIndex];

		aiString textureFilePath;
		std::string fullPath;

		bool hasTexture = false;
		if (materialSrc->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			materialSrc->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			fullPath = directoryPath + "/" + textureFilePath.C_Str(); // スペース消して正しいパス連結
			hasTexture = true;

		}

		materials_[materialIndex] = std::make_unique<Material>();
		Material& material = *materials_[materialIndex];
		// テクスチャパスが取得できたときだけ Initialize を呼ぶ
		if (hasTexture) {
			material.Initialize(fullPath);
			material.SetTextureFilePath(fullPath);
		} else {
			fullPath = "Resources/images/white.png"; // テクスチャが無い場合は白を指定
			material.Initialize(fullPath);
			material.SetTextureFilePath(fullPath);
		}

		aiColor3D color;
		if (materialSrc->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
			material.SetKa({ color.r, color.g, color.b });
		}
		if (materialSrc->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
			material.SetKs({ color.r, color.g, color.b });
		}

		float shininess = 0.0f;
		if (materialSrc->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
			material.SetNs(shininess);
			//material.SetMaterialShininess(shininess);
		}

		float refractionIndex = 1.0f;
		if (materialSrc->Get(AI_MATKEY_REFRACTI, refractionIndex) == AI_SUCCESS) {
			material.SetNi(refractionIndex);
		}

		unsigned int illumModel = 0;
		if (materialSrc->Get(AI_MATKEY_SHADING_MODEL, illumModel) == AI_SUCCESS) {
			material.SetIllum(illumModel);
		}

		if (materialSrc->Get(AI_MATKEY_SHININESS_STRENGTH, shininess) == AI_SUCCESS && shininess > 0.0f) {
			//material.SetMaterialSpecularEnabled(true);
		}
	}
}

void Model::LoadSkinCluster(const aiScene* scene)
{
	skinCluster_ = std::make_unique<SkinCluster>();
	skinCluster_->LoadFromScene(scene); // SkinClusterの情報を取得
}

void Model::LoadNode(const aiScene* scene)
{
	rootNode_ = std::make_unique<Node>(Node::ReadNode(scene->mRootNode));
}
void Model::ApplyNodeTransform(const aiScene* scene, const aiNode* node, const Matrix4x4& parentMatrix) {
	Matrix4x4 local = ConvertMatrix(node->mTransformation);
	Matrix4x4 world = MultiplyMatrix(parentMatrix, local);

	for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
		uint32_t meshIndex = node->mMeshes[i];
		if (meshIndex < meshes_.size()) {
			meshes_[meshIndex]->SetWorldMatrix(world);
		}
	}

	for (uint32_t i = 0; i < node->mNumChildren; ++i) {
		ApplyNodeTransform(scene, node->mChildren[i], world);
	}
}


