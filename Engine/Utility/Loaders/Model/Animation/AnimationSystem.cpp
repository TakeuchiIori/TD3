#include "AnimationSystem.h"

// Engine
#include "../ModelUtils.h"


// C++
#include <Windows.h>
#include <unordered_set>

// MAth
#include "Vector3.h"
#include "Quaternion.h"
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
	if (!animation_) return;

	if (animationBlendState_.isBlending) {
		animationBlendState_.currentTime += deltaTime;
		animationTime_ += deltaTime;
		if (animationBlendState_.currentTime >= animationBlendState_.blendTime) {
			animationBlendState_.isBlending = false;
			animationTime_ = 0.0f;
		}
	} else {
		animationTime_ += deltaTime;
		float duration = animation_->GetDuration();
		if (animationTime_ > duration) {
			animationTime_ = 0.0f;
		}
	}


}

/// <summary>
///  アニメーション適用（ブレンド対応）
/// </summary>
void AnimationSystem::Apply()
{
	if (!animation_) { return; }

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

	for (Joint& joint : skeleton_->GetJoints()) {
		std::string name = NormalizeNodeName(joint.GetName());

		if (ignoreNodes.count(name)) {
			continue; // 無視
		}
		bool found = false;

		for (const auto& [nodeName, _] : toAnimation.animation_.nodeAnimations_) {
			if (NormalizeNodeName(nodeName) == name) {
				found = true;
				break;
			}
		}

		if (!found) {
			throw std::runtime_error("Animation" + name + "Not Blend Destination"); // ブレンド先が見つからない
		}
	}

	/// アニメーションのブレンドの初期化
	animationBlendState_.from = *animation_;
	animationBlendState_.fromTime = animationTime_;		// 現在の再生位置を保存
	animationBlendState_.to = toAnimation;
	animationBlendState_.toTime = 0.0f;					// 必要なら to 側も途中から再生可
	animationBlendState_.blendTime = blendDuration;
	animationBlendState_.currentTime = 0.0f;
	animationBlendState_.isBlending = true;
	animation_ = &animationBlendState_.to;				// 今後は to を再生
}


bool AnimationSystem::IsIgnoredJoint(const std::string& name) {
	static const std::unordered_set<std::string> ignored = {
		"Armature", "Retopology_hp_Plane.002"
	};
	return ignored.count(name) > 0;
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


