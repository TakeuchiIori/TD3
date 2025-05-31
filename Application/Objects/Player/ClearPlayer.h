#pragma once
#include "BaseObject/BaseObject.h"
#include "Systems/GameTime/GameTime.h"
#include "Loaders/Json/JsonManager.h"


class ClearPlayer : public BaseObject
{
public:
	ClearPlayer() = default;
	~ClearPlayer() override = default;
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="camera"></param>
	void Initialize(Camera* camera) override;
	void InitJson();


	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;
	void UpdateMatrix();


	/// <summary>
	///  描画処理
	/// </summary>
	void Draw() override;


	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	Vector3 GetCenterPosition() const {
		return
		{
			worldTransform_.matWorld_.m[3][0],
			worldTransform_.matWorld_.m[3][1],
			worldTransform_.matWorld_.m[3][2]
		};
	}
private:

	std::unique_ptr<JsonManager> jsonManager_;
	std::unique_ptr<Object3d> neck_;
	WorldTransform neckTransform_;


	float up_ = 0.1f;
	const Vector4 defaultColorV4_ = { 0.90625f,0.87109f,0.125f,1.0f };
};

