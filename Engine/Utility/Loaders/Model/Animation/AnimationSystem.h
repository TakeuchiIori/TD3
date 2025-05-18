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

	void CacheInitialPose();
	void RestoreInitialPose();

	std::string GetNormalizedName(const std::string& name);
	void StartBlend(Animation& toAnimation, float blendDuration);



	void RequestPlay();
	void ResetPlay();

	void ResetPoseCache();


	// Jointの名前を除く
	bool IsIgnoredJoint(const std::string& name);
	bool IsPlayFinished() const;
	bool IsPlayOnceFinished() const { return isPlayedOnceFinished_; }

	void SetLoop(bool flag) { isLoop_ = flag; }
	bool IsLoop() const { return isLoop_; }

private:
	QuaternionTransform GetTransformAnimation(const Animation& anim, const std::string& nodeName,float time);
	void BlendAndApplyAnimation(const Animation& from, const Animation& to, float t);

private:

	Animation* animation_ = nullptr;					// アニメーションデータ
	Skeleton* skeleton_ = nullptr;						// スケルトンデータ
	SkinCluster* skinCluster_ = nullptr;				// スキンクラスター
	Node* node_ = nullptr;								// ノードデータ
	float animationTime_ = 0.0f;						// アニメーション時間




	bool isPlayOnce_ = false;							// アニメーションを1回だけ再生するか
	bool isPlayedOnceFinished_ = false;					// アニメーションの再生が終わったか

	bool isPlayRequested_ = false;
	bool isPlayFinished_ = false;						

	bool isLoop_ = false;								// 無限ループ


	// 初期ポーズの保存
	std::unordered_map<std::string, QuaternionTransform> initialPoseMap_;
	bool hasCachedInitialPose_ = false;

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

