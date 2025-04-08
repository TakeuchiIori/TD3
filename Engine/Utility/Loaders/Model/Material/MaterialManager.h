#pragma once

#include "Material.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <assimp/material.h>
class MaterialManager {
public:
	/// <summary>
	/// シングルトン取得
	/// </summary>
	static MaterialManager* GetInstance();

	///// <summary>
	///// マテリアルを登録（既に同名があれば再利用）
	///// </summary>
	//uint32_t LoadMaterial(const std::string& name, std::shared_ptr<Material> material);

	///// <summary>
	///// ファイル名からマテリアルを作成・登録（仮想的な読み込み）
	///// </summary>
	//uint32_t LoadMaterial(const std::string& name);

	/// <summary>
	/// インデックスからマテリアル取得
	/// </summary>
	Material* GetMaterial(uint32_t index);

	/// <summary>
	/// 名前からマテリアル取得（存在しなければnullptr）
	/// </summary>
	Material* GetMaterialByName(const std::string& name);

	/// <summary>
	/// 全マテリアル取得（GUIなどに使える）
	/// </summary>
	const std::vector<std::shared_ptr<Material>>& GetAllMaterials() const;


private:
	// 名前→インデックスマップ
	std::unordered_map<std::string, uint32_t> nameToIndex_;

	// マテリアル配列
	std::vector<std::shared_ptr<Material>> materials_;

	// シングルトン
	static MaterialManager* instance_;
};