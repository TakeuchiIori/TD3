#pragma once

// C++
#include <optional>
#include <map>
#include <vector>


// Engine
#include "../Joint.h"
#include "../Node.h"


// Math
#include "Quaternion.h"
#include "Vector3.h"



class Animation
{
public:
	enum class InterpolationType {
		Linear,
		Step,
		CubicSpline
	};

	template <typename tValue>
	struct Keyframe {
		float time;
		tValue value;
	};
	using KeyframeVector3 = Keyframe<Vector3>;
	using KeyframeQuaternion = Keyframe<Quaternion>;

	template<typename tValue>
	struct AnimationCurve {
		std::vector<Keyframe<tValue>> keyframes;
	};

	struct NodeAnimation {
		AnimationCurve<Vector3> translate;
		AnimationCurve<Quaternion> rotate;
		AnimationCurve<Vector3> scale;
		InterpolationType interpolationType;
	};

	struct AnimationModel {
		float duration_ ;										 // アニメーション全体の尺（秒）
		std::map<std::string, NodeAnimation> nodeAnimations_;	 // NodeAnimationの集合。Node名で開けるように
	};

public:

	static Animation LoadFromScene(const aiScene* scene, const std::string& gltfFilePath);
	static std::string ParseGLTFInterpolation(const std::string& gltfFilePath, uint32_t samplerIndex);

	void SaveToBinary(const std::string& path) const;
	static Animation LoadFromBinary(const std::string& path);

	/// <summary>
	/// アニメーション適用
	/// </summary>
	/// <param name="skeleton"></param>
	/// <param name="animation"></param>
	/// <param name="animationTime"></param>
	void ApplyAnimation(std::vector<Joint>& joints,float animationtime);
	void PlayerAnimation(float animationTime,Node& node);

	/// <summary>
	/// 任意の時刻を取得
	/// </summary>
	Vector3 CalculateValue(const AnimationCurve<Vector3>& curve, float time);
	Quaternion CalculateValue(const AnimationCurve<Quaternion>& curve, float time);

	Vector3 CalculateValueNew(const std::vector<KeyframeVector3>& keyframes, float time, InterpolationType interpolationType);
	Quaternion CalculateValueNew(const std::vector<KeyframeQuaternion>& keyframes, float time, InterpolationType interpolationType);


	
public:

	float GetDuration() const { return animation_.duration_; }
	void SetDuration(float duration) { animation_.duration_ = duration; }

	AnimationModel animation_;
	Matrix4x4 localMatrix_;
	float animationTime_ = 0.0f;

};

