#pragma once
#include "BaseObject/BaseObject.h"
#include "Loaders/Json/JsonManager.h"

class BaseStageObject :
    public BaseObject
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Camera* camera) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;


	void InitJson();


public: // getter & setter
	void SetName(std::string& objectName) { name_ = objectName; }

	void SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; }

	void SetTranslate(const Vector3& translate) { worldTransform_.translation_ = translate; }

	void SetRotate(const Vector3& rotate) { worldTransform_.rotation_ = rotate; }

private:

	std::unique_ptr <JsonManager> jsonManager_;

	std::string name_;
};

