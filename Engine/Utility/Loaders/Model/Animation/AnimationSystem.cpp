#include "AnimationSystem.h"

// Engine
#include "../ModelUtils.h"


// C++
#include <Windows.h>
#include <unordered_set>

// MAth
#include "Vector3.h"
#include "Quaternion.h"
#include <Debugger/Logger.h>
void AnimationSystem::Initialize(Animation& animation, Skeleton& skeleton, SkinCluster& skinCluster, Node* node)
{
	animation_ = &animation;
	skeleton_ = &skeleton;
	skinCluster_ = &skinCluster;
	node_ = node;
	animationTime_ = 0.0f;
}

void AnimationSystem::Initialize(Animation& animation, Node* rootNode)
{
	animation_ = &animation;
	node_ = rootNode;
	animationTime_ = 0.0f;
}

void AnimationSystem::Update(float deltaTime)
{
	if (!animation_ || !isPlayRequested_) return;

	animationTime_ += deltaTime;
	float duration = animation_->GetDuration();

	if (animationTime_ >= duration) {
		if (isLoop_) {
			animationTime_ = 0.0f; // ループ
		} else {
			animationTime_ = duration;
			isPlayRequested_ = false;
			isPlayFinished_ = true;
			RestoreInitialPose();

			skeleton_->Update();
			if (skinCluster_) {
				skinCluster_->Update(skeleton_->GetJoints());
			}
			return;
		}
	}

}



/// <summary>
///  アニメーション適用（ブレンド対応）
/// </summary>
void AnimationSystem::Apply()
{
	if (!animation_) return;

	// 再生が終わっていれば何も適用しない（戻し済み）
	if (isPlayOnce_ && isPlayFinished_) {
		return;
	}
	// ブレンド中
	// ・ブレンドしていない通常再生
	// 　・ボーン無しモデル
	if (animationBlendState_.isBlending && skeleton_) {
		float t = animationBlendState_.currentTime / animationBlendState_.blendTime;
		t = std::clamp(t, 0.0f, 1.0f);
		BlendAndApplyAnimation(animationBlendState_.from, animationBlendState_.to, t);

		skeleton_->Update();
		if (skinCluster_) {
			skinCluster_->Update(skeleton_->GetJoints());
		}

	}else if (skeleton_) {
		animation_->ApplyAnimation(skeleton_->GetJoints(), animationTime_);
		skeleton_->Update();
		if (skinCluster_) {
			skinCluster_->Update(skeleton_->GetJoints());
		}

	}else if (node_) {
		animation_->PlayerAnimation(animationTime_, *node_);
	}
}


void AnimationSystem::StartBlend(Animation& toAnimation, float blendDuration) {
	// チェック済みブレンド可能なジョイントだけ処理
	std::unordered_set<std::string> toAnimNodes;
	for (const auto& [nodeName, _] : toAnimation.animation_.nodeAnimations_) {
		toAnimNodes.insert(NormalizeNodeName(nodeName));
	}

	// ブレンド対象ノードをフィルタリング
	for (Joint& joint : skeleton_->GetJoints()) {
		std::string name = NormalizeNodeName(joint.GetName());

		if (ignoreNodes.count(name)) {
			continue; // 無視
		}

		if (toAnimNodes.count(name) == 0) {
			Logger("[Blend Warning] Node not found in destination animation: " + name + "\n");
			continue;
		}

	}

	// アニメーションのブレンド初期化
	animationBlendState_.from = *animation_;
	animationBlendState_.fromTime = animationTime_;
	animationBlendState_.to = toAnimation;
	animationBlendState_.toTime = 0.0f;
	animationBlendState_.blendTime = blendDuration;
	animationBlendState_.currentTime = 0.0f;
	animationBlendState_.isBlending = true;
	animation_ = &animationBlendState_.to;
}


bool AnimationSystem::IsIgnoredJoint(const std::string& name) {
	static const std::unordered_set<std::string> ignored = {
		"Armature", "Retopology_hp_Plane.002"
	};
	return ignored.count(name) > 0;
}




void AnimationSystem::CacheInitialPose() {
	if (!skeleton_ || !animation_) return;

	initialPoseMap_.clear();

	for ( Joint& joint : skeleton_->GetJoints()) {
		initialPoseMap_[joint.GetName()] = joint.GetTransform();
	}

	hasCachedInitialPose_ = true;
}


void AnimationSystem::RestoreInitialPose() {
	if (!skeleton_ || !hasCachedInitialPose_) return;

	for (Joint& joint : skeleton_->GetJoints()) {
		const std::string& name = joint.GetName();
		if (initialPoseMap_.count(name)) {
			joint.SetTransform(initialPoseMap_[name]); // キャッシュからそのまま適用
		}
	}

	skeleton_->Update();

	if (skinCluster_) {
		skinCluster_->Update(skeleton_->GetJoints());
	}
}



std::string AnimationSystem::GetNormalizedName(const std::string& name) {
	auto it = normalizedNameCache_.find(name);
	if (it != normalizedNameCache_.end()) return it->second;
	std::string normalized = NormalizeNodeName(name);
	normalizedNameCache_[name] = normalized;
	return normalized;
}

QuaternionTransform AnimationSystem::GetTransformAnimation(const Animation& anim, const std::string& nodeName, float time)
{
	QuaternionTransform qTransform{};
	const auto& animMap = anim.animation_.nodeAnimations_;

	// 読み込み先と比較してノード名探し
	auto it = std::find_if(animMap.begin(), animMap.end(),
		[&](const auto& pair) {
			return GetNormalizedName(pair.first) == GetNormalizedName(nodeName);
		});

	if (it != animMap.end()) {
		const auto& nodeAnim = it->second;
		qTransform.translate = const_cast<Animation&>(anim).CalculateValueNew(nodeAnim.translate.keyframes, time, nodeAnim.interpolationType);
		qTransform.rotate = const_cast<Animation&>(anim).CalculateValueNew(nodeAnim.rotate.keyframes, time, nodeAnim.interpolationType);
		qTransform.scale = const_cast<Animation&>(anim).CalculateValueNew(nodeAnim.scale.keyframes, time, nodeAnim.interpolationType);
	} else {
		qTransform.translate = { 0.0f, 0.0f, 0.0f };
		qTransform.rotate = { 0.0f, 0.0f, 0.0f, 1.0f };
		qTransform.scale = { 1.0f, 1.0f, 1.0f };
	}
	return qTransform;
}

void AnimationSystem::BlendAndApplyAnimation(const Animation& from, const Animation& to, float t)
{
	float fromSampleTime = animationBlendState_.fromTime + animationBlendState_.currentTime;
	float toSampleTime = animationBlendState_.toTime + animationBlendState_.currentTime;

	for (Joint& joint : skeleton_->GetJoints()) {
		std::string name = GetNormalizedName(joint.GetName());

		// ノード名を除く
		if (IsIgnoredJoint(name)) { continue; }

		QuaternionTransform fromTr = GetTransformAnimation(from, name, fromSampleTime);
		QuaternionTransform toTr = GetTransformAnimation(to, name, toSampleTime);

		QuaternionTransform blended;
		blended.translate = Lerp(fromTr.translate, toTr.translate, t);
		blended.rotate = Slerp(fromTr.rotate, toTr.rotate, t);
		blended.scale = Lerp(fromTr.scale, toTr.scale, t);

		joint.SetTransform(blended);
	}
}


void AnimationSystem::RequestPlay() {
	if (!isPlayRequested_) {
		CacheInitialPose();
	}

	isPlayRequested_ = true;
	isPlayFinished_ = false;
	animationTime_ = 0.0f;
}

bool AnimationSystem::IsPlayFinished() const {
	return isPlayFinished_;
}

void AnimationSystem::ResetPlay() {
	isPlayRequested_ = false;
	isPlayFinished_ = false;
	isPlayOnce_ = false;
	animationTime_ = 0.0f;
}

void AnimationSystem::ResetPoseCache() {
	initialPoseMap_.clear();
	hasCachedInitialPose_ = false;
}
