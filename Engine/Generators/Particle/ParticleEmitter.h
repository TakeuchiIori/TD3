#pragma once
// C++
#include <string>

// Engine
#include "ParticleManager.h"
#include "WorldTransform./WorldTransform.h"
#include "Loaders/Json/JsonManager.h"

// Math
#include  "Vector3.h"



class ParticleEmitter
{
public: // メンバ関数


	/// <summary>
	/// 既定のコンストラクタ
	/// </summary>
	ParticleEmitter() = default;

	/// <summary>
	/// コンストラクタ
	/// </summary>
	ParticleEmitter(const std::string& name, const Vector3& transform, uint32_t count);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const std::string& jsonPath);

	/// <summary>
	/// Json
	/// </summary>
	void InitJson(const std::string& jsonPath);

public:
	/// <summary>
	/// 更新
	/// </summary>
	void UpdateTime();

	/// <summary>
	/// 設定したオブジェクトに追従する
	/// </summary>
	/// <param name="name"></param>
	/// <param name="transform"></param>
	void FollowEmit(const Vector3& transform);

	/// <summary>
	/// パーティクルを指定した位置で発生
	/// </summary>
	void Emit();

	/// <summary>
	/// fromからtoの方向へ飛ばす
	/// </summary>
	/// <param name="from"></param>
	/// <param name="to"></param>
	void EmitFromTo(const Vector3& from, const Vector3& to);

	void EmitFrom(const Vector3& from, const Vector3& to);

private:



	

	/// <summary>
	/// ImGui
	/// </summary>
	void ShowImGui();

	/// <summary>
	/// Json
	/// </summary>
	void InitJson();


public:

	void SetPosition(const Vector3& position) { emitter_.transform = position; };
	void SetCount(uint32_t& setcount) { emitter_.count = setcount; };

private:

	/// <summary>
	/// エミッター構造体
	/// </summary>
	struct Emitter {
		std::string name;
		Vector3 transform;
		uint32_t count;
		float frequency;			// 頻度
		float frequencyTime;		// 頻度時間
	};

	// エミッター
	Emitter emitter_{};
	float deltaTime_ = 1.0f / 60.0f;

	std::unique_ptr<JsonManager> jsonManager_;;

};

