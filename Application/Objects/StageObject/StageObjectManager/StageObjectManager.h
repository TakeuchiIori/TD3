#pragma once
#include "StageObject/BaseStageObject/BaseStageObject.h"
#include "Loaders/Json/JsonManager.h"

class StageObjectManager
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Camera* camera);

	void ResetObjList();

	void Update(std::string& stageName);

	void AddObject(std::string& stageName);

	void InitStageJson(std::string& stageName);

	void Draw();

private:
	// ポインタ
	Camera* camera_;

	std::unique_ptr<JsonManager> jsonManager_;

	std::list<std::unique_ptr<BaseStageObject>> stageObjects_;

	int totalObjNum_ = 0;
};

