#include "Animation.h"
#include "MathFunc.h"
#include <assert.h>
#include <fstream>
#include <filesystem>
#include <json.hpp>
#include "Quaternion.h"
#include <iostream>

Animation Animation::LoadFromScene(const aiScene* scene, const std::string& gltfFilePath)
{
	assert(scene && scene->mNumAnimations > 0);
	Animation anim;

	aiAnimation* animationAssimp = scene->mAnimations[0];
	anim.animation_.duration_ = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);

	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = anim.animation_.nodeAnimations_[nodeAnimationAssimp->mNodeName.C_Str()];

		std::string interpolation = ParseGLTFInterpolation(gltfFilePath, channelIndex);
		if (interpolation == "LINEAR") nodeAnimation.interpolationType = InterpolationType::Linear;
		else if (interpolation == "STEP") nodeAnimation.interpolationType = InterpolationType::Step;
		else if (interpolation == "CUBICSPLINE") nodeAnimation.interpolationType = InterpolationType::CubicSpline;
		else nodeAnimation.interpolationType = InterpolationType::Linear;

		for (uint32_t i = 0; i < nodeAnimationAssimp->mNumPositionKeys; ++i) {
			KeyframeVector3 kf;
			kf.time = float(nodeAnimationAssimp->mPositionKeys[i].mTime / animationAssimp->mTicksPerSecond);
			const auto& val = nodeAnimationAssimp->mPositionKeys[i].mValue;
			kf.value = { -val.x, val.y, val.z };
			nodeAnimation.translate.keyframes.push_back(kf);
		}

		for (uint32_t i = 0; i < nodeAnimationAssimp->mNumScalingKeys; ++i) {
			KeyframeVector3 kf;
			kf.time = float(nodeAnimationAssimp->mScalingKeys[i].mTime / animationAssimp->mTicksPerSecond);
			const auto& val = nodeAnimationAssimp->mScalingKeys[i].mValue;
			kf.value = { val.x, val.y, val.z };
			nodeAnimation.scale.keyframes.push_back(kf);
		}

		for (uint32_t i = 0; i < nodeAnimationAssimp->mNumRotationKeys; ++i) {
			KeyframeQuaternion kf;
			kf.time = float(nodeAnimationAssimp->mRotationKeys[i].mTime / animationAssimp->mTicksPerSecond);
			const auto& val = nodeAnimationAssimp->mRotationKeys[i].mValue;
			kf.value = { val.x, -val.y, -val.z, val.w };
			nodeAnimation.rotate.keyframes.push_back(kf);
		}
	}

	return anim;
}

std::string Animation::ParseGLTFInterpolation(const std::string& gltfFilePath, uint32_t samplerIndex) {
	// GLTFファイルを開く
	std::ifstream file(gltfFilePath);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open GLTF file: " + gltfFilePath);
	}

	// JSONを読み込む
	nlohmann::json gltfJson;
	file >> gltfJson;

	// サンプラー情報を取得
	const auto& samplers = gltfJson["animations"][0]["samplers"];
	if (samplerIndex >= samplers.size()) {
		return "LINEAR"; // デフォルト値
	}

	// 補間方法を取得
	if (samplers[samplerIndex].contains("interpolation")) {
		return samplers[samplerIndex]["interpolation"].get<std::string>();
	}

	return "LINEAR"; // デフォルト値
}

void Animation::SaveToBinary(const std::string& path) const
{
	// ★ パスからディレクトリ部分を取得して作成
	std::filesystem::path dir = std::filesystem::path(path).parent_path();
	std::filesystem::create_directories(dir); // 存在しないときだけ作ってくれる

	std::ofstream ofs(path, std::ios::binary);
	if (!ofs) {
		std::cerr << "[ERROR] Failed to open file for writing: " << path << std::endl;
		return;
	}

	ofs.write("ANIM", 4); // マジックナンバー
	ofs.write(reinterpret_cast<const char*>(&animation_.duration_), sizeof(float));

	size_t nodeCount = animation_.nodeAnimations_.size();
	ofs.write(reinterpret_cast<const char*>(&nodeCount), sizeof(size_t));

	for (const auto& [name, nodeAnim] : animation_.nodeAnimations_) {
		size_t nameLen = name.size();
		ofs.write(reinterpret_cast<const char*>(&nameLen), sizeof(size_t));
		ofs.write(name.c_str(), nameLen);

		auto writeVec3Keyframes = [&](const auto& keyframes) {
			size_t count = keyframes.size();
			ofs.write(reinterpret_cast<const char*>(&count), sizeof(size_t));
			for (const auto& kf : keyframes) {
				ofs.write(reinterpret_cast<const char*>(&kf.time), sizeof(float));
				ofs.write(reinterpret_cast<const char*>(&kf.value), sizeof(Vector3));
			}
			};

		auto writeQuatKeyframes = [&](const auto& keyframes) {
			size_t count = keyframes.size();
			ofs.write(reinterpret_cast<const char*>(&count), sizeof(size_t));
			for (const auto& kf : keyframes) {
				ofs.write(reinterpret_cast<const char*>(&kf.time), sizeof(float));
				ofs.write(reinterpret_cast<const char*>(&kf.value), sizeof(Quaternion));
			}
			};

		writeVec3Keyframes(nodeAnim.translate.keyframes);
		writeQuatKeyframes(nodeAnim.rotate.keyframes);
		writeVec3Keyframes(nodeAnim.scale.keyframes);

		int interp = static_cast<int>(nodeAnim.interpolationType);
		ofs.write(reinterpret_cast<const char*>(&interp), sizeof(int));
	}
}

Animation Animation::LoadFromBinary(const std::string& path)
{
	std::ifstream ifs(path, std::ios::binary);
	Animation anim;
	if (!ifs) return anim;

	char magic[4];
	ifs.read(magic, 4);
	if (std::string(magic, 4) != "ANIM") return anim;

	ifs.read(reinterpret_cast<char*>(&anim.animation_.duration_), sizeof(float));

	size_t nodeCount = 0;
	ifs.read(reinterpret_cast<char*>(&nodeCount), sizeof(size_t));

	for (size_t i = 0; i < nodeCount; ++i) {
		size_t nameLen;
		ifs.read(reinterpret_cast<char*>(&nameLen), sizeof(size_t));
		std::string name(nameLen, ' ');
		ifs.read(name.data(), nameLen);

		Animation::NodeAnimation nodeAnim;

		auto readVec3Keyframes = [&](auto& keyframes) {
			size_t count;
			ifs.read(reinterpret_cast<char*>(&count), sizeof(size_t));
			keyframes.resize(count);
			for (auto& kf : keyframes) {
				ifs.read(reinterpret_cast<char*>(&kf.time), sizeof(float));
				ifs.read(reinterpret_cast<char*>(&kf.value), sizeof(Vector3));
			}
			};

		auto readQuatKeyframes = [&](auto& keyframes) {
			size_t count;
			ifs.read(reinterpret_cast<char*>(&count), sizeof(size_t));
			keyframes.resize(count);
			for (auto& kf : keyframes) {
				ifs.read(reinterpret_cast<char*>(&kf.time), sizeof(float));
				ifs.read(reinterpret_cast<char*>(&kf.value), sizeof(Quaternion));
			}
			};

		readVec3Keyframes(nodeAnim.translate.keyframes);
		readQuatKeyframes(nodeAnim.rotate.keyframes);
		readVec3Keyframes(nodeAnim.scale.keyframes);

		int interp;
		ifs.read(reinterpret_cast<char*>(&interp), sizeof(int));
		nodeAnim.interpolationType = static_cast<Animation::InterpolationType>(interp);

		anim.animation_.nodeAnimations_[name] = nodeAnim;
	}

	return anim;
}

void Animation::ApplyAnimation(std::vector<Joint>& joints,float animationtime)
{
	for (Joint& joint :joints) {
		// 対象のJointのAnimationがあれば、値の適用を行う。
		// 下記のif文はC++17から可能になったinit-statement付きのif文。
		if (auto it = animation_.nodeAnimations_.find(joint.GetName()); it != animation_.nodeAnimations_.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			QuaternionTransform transform;
			transform.translate = CalculateValueNew(rootNodeAnimation.translate.keyframes, animationtime,rootNodeAnimation.interpolationType); // 指定時刻の値を取得
			transform.rotate = CalculateValueNew(rootNodeAnimation.rotate.keyframes, animationtime, rootNodeAnimation.interpolationType);
			transform.scale = CalculateValueNew(rootNodeAnimation.scale.keyframes, animationtime, rootNodeAnimation.interpolationType);
			joint.SetTransform(transform);
		}
	}
}

void Animation::PlayerAnimation(float animationTime, Node& node)
{
	NodeAnimation& rootNodeAnimation = animation_.nodeAnimations_[node.name_]; // rootNodeのAnimationを取得
	Vector3 translate = CalculateValueNew(rootNodeAnimation.translate.keyframes, animationTime, rootNodeAnimation.interpolationType); // 指定時刻の値を取得
	Quaternion rotate = CalculateValueNew(rootNodeAnimation.rotate.keyframes, animationTime, rootNodeAnimation.interpolationType);
	Vector3 scale = CalculateValueNew(rootNodeAnimation.scale.keyframes, animationTime, rootNodeAnimation.interpolationType);

	node.localMatrix_ = MakeAffineMatrix(scale, rotate, translate);
}

Vector3 Animation::CalculateValue(const AnimationCurve<Vector3>& curve, float time)
{
	assert(!curve.keyframes.empty());
	if (curve.keyframes.size() == 1 || time <= curve.keyframes[0].time) {
		return curve.keyframes[0].value;
	}

	for (size_t index = 0; index < curve.keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの2つのkeyframeを取得して範囲内に時刻があるか判定
		if (curve.keyframes[index].time <= time && time <= curve.keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - curve.keyframes[index].time) / (curve.keyframes[nextIndex].time - curve.keyframes[index].time);
			return Lerp(curve.keyframes[index].value, curve.keyframes[nextIndex].value, t);
		}
	}
	// ここまできた場合は一番後の時刻よりも後ろなので最後の値を返すことになる
	return (*curve.keyframes.rbegin()).value;
}

Quaternion Animation::CalculateValue(const AnimationCurve<Quaternion>& curve, float time)
{
	assert(!curve.keyframes.empty());
	if (curve.keyframes.size() == 1 || time <= curve.keyframes[0].time) {
		return curve.keyframes[0].value;
	}

	for (size_t index = 0; index < curve.keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの2つのkeyframeを取得して範囲内に時刻があるか判定
		if (curve.keyframes[index].time <= time && time <= curve.keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - curve.keyframes[index].time) / (curve.keyframes[nextIndex].time - curve.keyframes[index].time);
			return Slerp(curve.keyframes[index].value, curve.keyframes[nextIndex].value, t);
		}
	}
	// ここまできた場合は一番後の時刻よりも後ろなので最後の値を返すことになる
	return (*curve.keyframes.rbegin()).value;
}




Vector3 Animation::CalculateValueNew(const std::vector<KeyframeVector3>& keyframes, float time, InterpolationType interpolationType) {
	assert(!keyframes.empty());

	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value; // 最初のキー値を返す
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;

		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);

			switch (interpolationType) {
			case InterpolationType::Linear:
				return Lerp(keyframes[index].value, keyframes[nextIndex].value, t);

			case InterpolationType::Step:
				return keyframes[index].value;

			case InterpolationType::CubicSpline: {
				size_t prevIndex = (index == 0) ? index : index - 1;
				size_t nextNextIndex = (nextIndex + 1 < keyframes.size()) ? nextIndex + 1 : nextIndex;

				return CubicSplineInterpolate(
					keyframes[prevIndex].value,
					keyframes[index].value,
					keyframes[nextIndex].value,
					keyframes[nextNextIndex].value,
					t
				);
			}

			default:
				return Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
			}
		}
	}

	return (*keyframes.rbegin()).value;
}

Quaternion Animation::CalculateValueNew(const std::vector<KeyframeQuaternion>& keyframes, float time, InterpolationType interpolationType) {
	assert(!keyframes.empty());

	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value; // 最初のキー値を返す
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;

		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);

			switch (interpolationType) {
			case InterpolationType::Linear:
				return Slerp(keyframes[index].value, keyframes[nextIndex].value, t);

			case InterpolationType::Step:
				return keyframes[index].value;

			case InterpolationType::CubicSpline: {

			}

			default:
				return Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
			}
		}
	}

	return (*keyframes.rbegin()).value;
}

void Animation::Reset() {
	animationTime_ = 0.0f;
	localMatrix_ = MakeIdentity4x4();
}
