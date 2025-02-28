#pragma once
#include "BaseObject/BaseObject.h"
#include "Loaders/Json/JsonManager.h"

class BaseStageObject :
    public BaseObject
{
public:
	BaseStageObject() : id(next_id++) {
		++count;
	}
	~BaseStageObject() override {
		--count;
	}
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


	virtual void InitJson();


public: // getter & setter
	virtual void SetStageName(std::string& stageName) { stageName_ = stageName; }

	virtual void SetName(std::string& objectName) { name_ = objectName; }

	virtual void SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; }

	virtual void SetTranslate(const Vector3& translate) { worldTransform_.translation_ = translate; }

	virtual void SetRotate(const Vector3& rotate) { worldTransform_.rotation_ = rotate; }

	virtual void SetModel(const std::string& filePath, bool isAnimation = false) { obj_->SetModel(filePath, isAnimation); }

	int GetId() { return id; }

private:

	std::unique_ptr <JsonManager> jsonManager_;


	std::string stageName_;
	std::string name_;


	static int count;    // 現在のインスタンス数
	static int next_id;  // 次に割り当てるID
	int id;              // 各インスタンスのID
};

