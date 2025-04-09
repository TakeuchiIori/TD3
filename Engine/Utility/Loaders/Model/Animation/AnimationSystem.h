#pragma once

// Engine
#include "Animation.h"
#include "../Skeleton.h"
#include "../SkinCluster.h"
#include "../Node.h"
#include "Quaternion.h"
#include <unordered_map>

class AnimationSystem {

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Animation& animation, Skeleton& skeleton, SkinCluster& skinCluster, Node* node);
	void Initialize(Animation& animation, Node* rootNode);
	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="animationTime"></param>
	void Update(float deltaTime);

	/// <summary>
	/// アニメーション適用
	/// </summary>
	void Apply();

	std::string GetNormalizedName(const std::string& name);

	void StartBlend(Animation& toAnimation, float blendDuration);

private:

	// Jointの名前を除く
	bool IsIgnoredJoint(const std::string& name);

	QuaternionTransform GetTransformAnimation(const Animation& anim, const std::string& nodeName,float time);
	void BlendAndApplyAnimation(const Animation& from, const Animation& to, float t);

private:

	Animation* animation_ = nullptr;					// アニメーションデータ
	Skeleton* skeleton_ = nullptr;						// スケルトンデータ
	SkinCluster* skinCluster_ = nullptr;				// スキンクラスター
	Node* node_ = nullptr;								// ノードデータ
	float animationTime_ = 0.0f;						// アニメーション時間


	struct AnimationBlendState {
		Animation  from;
		Animation  to;
		float      fromTime = 0.0f;
		float      toTime = 0.0f;
		float      blendTime = 0.0f;
		float      currentTime = 0.0f;
		bool       isBlending = false;
	} animationBlendState_;

	// クラスメンバとして追加
	std::unordered_map<std::string, std::string> normalizedNameCache_;

};

