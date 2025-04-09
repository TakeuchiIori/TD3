#include "Object3d.h"
#include "Object3dCommon.h"
#include <assert.h>
// C++
#include <fstream>
#include <sstream>
#include <algorithm>

// Engine
#include "Loaders./Texture./TextureManager.h"
#include "Loaders./Model/ModelManager.h"
#include "Loaders./Model/Model.h"
#include "WorldTransform./WorldTransform.h"


#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


const std::string  Object3d::defaultModelPath_ = "Resources./Models./";

Object3d::Object3d()
{
	//queryIndex_ = OcclusionCullingManager::GetInstance()->AddOcclusionQuery();
}

void Object3d::Initialize()
{
	// 引数で受け取ってメンバ変数に記録する
	this->object3dCommon_ = Object3dCommon::GetInstance();

	CreateCameraResource();

	materialColor_ = std::make_unique<MaterialColor>();
	materialColor_->Initialize();

}
void Object3d::UpdateAnimation()
{
	// アニメーションの更新
	model_->UpdateAnimation();
}


void Object3d::Draw(Camera* camera, WorldTransform& worldTransform)
{


	//// クエリは毎回実行する
	//OcclusionCullingManager::GetInstance()->BeginOcclusionQuery(queryIndex_);

	//// 数フレーム連続で見えてなかったら描画しない
	//bool shouldDraw = OcclusionCullingManager::GetInstance()->ShouldDraw(queryIndex_, 10);
	//static uint32_t globalFrameCounter = 0;
	//globalFrameCounter++;
	//if (!shouldDraw && globalFrameCounter % 1 == 0) {
	//	shouldDraw = true;
	//}


	//if (shouldDraw) {
		Matrix4x4 worldViewProjectionMatrix;
		Matrix4x4 worldMatrix;
		if (model_) {
			if (camera) {
				const Matrix4x4& viewProjectionMatrix = camera->GetViewProjectionMatrix();

				// 
				if (!model_->GetHasBones()) {
					worldViewProjectionMatrix = worldTransform.GetMatWorld() * model_->GetRootNode().GetLocalMatrix() * viewProjectionMatrix;
					worldMatrix = worldTransform.GetMatWorld() * model_->GetRootNode().GetLocalMatrix();
				} else {
					worldViewProjectionMatrix = worldTransform.GetMatWorld() * viewProjectionMatrix;
					worldMatrix = worldTransform.GetMatWorld();
				}
			} else {

				worldViewProjectionMatrix = worldTransform.GetMatWorld();
				worldMatrix = worldTransform.GetMatWorld(); // 初期化が必要
			}
		}

		worldTransform.SetMapWVP(worldViewProjectionMatrix);
		worldTransform.SetMapWorld(worldMatrix);

		object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform.GetConstBuffer()->GetGPUVirtualAddress());
		// カメラ
		object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
		materialColor_->RecordDrawCommands(object3dCommon_->GetDxCommon()->GetCommandList().Get(), 7);

		if (model_) {
			model_->Draw();
		}
	//}
	//OcclusionCullingManager::GetInstance()->EndOcclusionQuery(queryIndex_);
}

void Object3d::DrawBone(Line& line)
{
	if (model_) {
		model_->DrawBone(line);
	}
}


void Object3d::CreateCameraResource()
{
	cameraResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(CameraForGPU));
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
	cameraData_->worldPosition = { 0.0f, 0.0f, 0.0f };
}





void Object3d::SetModel(const std::string& filePath, bool isAnimation)
{
	// 拡張子を取り除く処理
	std::string basePath = filePath;
	std::string fileName;
	if (basePath.size() > 4) {
		// .obj または .gltf の場合に削除
		if (basePath.substr(basePath.size() - 4) == ".obj") {
			basePath = basePath.substr(0, basePath.size() - 4);
			fileName = basePath + ".obj";
		} else if (basePath.size() > 5 && basePath.substr(basePath.size() - 5) == ".gltf") {
			basePath = basePath.substr(0, basePath.size() - 5);
			fileName = basePath + ".gltf";
		}
	}

	// .obj 読み込み (第一引数には拡張子なしのパス)
	ModelManager::GetInstance()->LoadModel(defaultModelPath_ + basePath, fileName, isAnimation);

	// モデルを検索してセットする
	model_ = ModelManager::GetInstance()->FindModel(fileName);

}

void Object3d::SetChangeAnimation(const std::string& filePath)
{
	std::string basePath = filePath;
	std::string fileName;
	if (basePath.size() > 5 && basePath.substr(basePath.size() - 5) == ".gltf") {
		basePath = basePath.substr(0, basePath.size() - 5);
		fileName = basePath + ".gltf";
	}

	if (model_) {
		model_->SetChangeAnimation(defaultModelPath_ + basePath, fileName);
	}
}
