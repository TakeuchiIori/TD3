#pragma once
#include <string>
#include <vector>

// テンプレート宣言
template <typename T>
struct EnumTraits;

// 汎用関数（関数で呼び出すように）
template<typename T>
inline const std::vector<std::string>& GetEnumNames() {
	return EnumTraits<T>::Names();
}

// ---- BlendMode定義 ----
enum class BlendMode {
	kBlendModeNone,
	kBlendModeNormal,
	kBlendModeAdd,
	kBlendModeSubtract,
	kBlendModeMultiply,
	kBlendModeScreen,
	kCount0fBlendMode,
};

// ---- ParticleManager関連のenum定義 ----
namespace ParticleManagerEnums {
	// 🌪️ 力場タイプ
	enum class ForceType {
		None = 0,
		Gravity = 1,        // 重力
		Wind = 2,           // 風
		Vortex = 3,         // 渦
		Radial = 4,         // 放射状
		Turbulence = 5,     // 乱流
		Spring = 6,         // バネ
		Damping = 7,        // 減衰
		Magnet = 8          // 磁力
	};

	// 📐 発生形状タイプ
	enum class EmissionType {
		Point = 0,          // 点
		Sphere = 1,         // 球
		Box = 2,            // 立方体
		Circle = 3,         // 円
		Ring = 4,           // リング
		Cone = 5,           // コーン
		Line = 6,           // 線
		Hemisphere = 7      // 半球
	};

	// 🎨 カラー変化タイプ
	enum class ColorChangeType {
		None = 0,           // 変化なし
		Fade = 1,           // フェード
		Gradient = 2,       // グラデーション
		Flash = 3,          // 点滅
		Rainbow = 4,        // 虹色
		Fire = 5,           // 炎色
		Electric = 6        // 電気色
	};

	// 📏 スケール変化タイプ
	enum class ScaleChangeType {
		None = 0,           // 変化なし
		Shrink = 1,         // 縮小
		Grow = 2,           // 拡大
		Pulse = 3,          // 脈動
		Stretch = 4         // 引き伸ばし
	};

	// 🔄 回転タイプ
	enum class RotationType {
		None = 0,           // 回転なし
		ConstantX = 1,      // X軸定速回転
		ConstantY = 2,      // Y軸定速回転
		ConstantZ = 3,      // Z軸定速回転
		Random = 4,         // ランダム回転
		Velocity = 5,       // 速度方向
		Tumble = 6          // 乱回転
	};

	// 🏃 移動パターン
	enum class MovementType {
		Linear = 0,         // 直線
		Curve = 1,          // 曲線
		Spiral = 2,         // 螺旋
		Wave = 3,           // 波
		Bounce = 4,         // バウンス
		Orbit = 5,          // 軌道
		Zigzag = 6          // ジグザグ
	};
}

// ---- BlendModeに対する特殊化 ----
template <>
struct EnumTraits<BlendMode> {
	static const std::vector<std::string>& Names() {
		static const std::vector<std::string> names = {
			"None", "Normal", "Add", "Subtract", "Multiply", "Screen"
		};
		return names;
	}
};

// ---- ParticleManagerEnums::ForceType の特殊化 ----
template <>
struct EnumTraits<ParticleManagerEnums::ForceType> {
	static const std::vector<std::string>& Names() {
		static const std::vector<std::string> names = {
			"None", "Gravity", "Wind", "Vortex", "Radial", "Turbulence", "Spring", "Damping", "Magnet"
		};
		return names;
	}
};

// ---- ParticleManagerEnums::EmissionType の特殊化 ----
template <>
struct EnumTraits<ParticleManagerEnums::EmissionType> {
	static const std::vector<std::string>& Names() {
		static const std::vector<std::string> names = {
			"Point", "Sphere", "Box", "Circle", "Ring", "Cone", "Line", "Hemisphere"
		};
		return names;
	}
};

// ---- ParticleManagerEnums::ColorChangeType の特殊化 ----
template <>
struct EnumTraits<ParticleManagerEnums::ColorChangeType> {
	static const std::vector<std::string>& Names() {
		static const std::vector<std::string> names = {
			"None", "Fade", "Gradient", "Flash", "Rainbow", "Fire", "Electric"
		};
		return names;
	}
};

// ---- ParticleManagerEnums::ScaleChangeType の特殊化 ----
template <>
struct EnumTraits<ParticleManagerEnums::ScaleChangeType> {
	static const std::vector<std::string>& Names() {
		static const std::vector<std::string> names = {
			"None", "Shrink", "Grow", "Pulse", "Stretch"
		};
		return names;
	}
};

// ---- ParticleManagerEnums::RotationType の特殊化 ----
template <>
struct EnumTraits<ParticleManagerEnums::RotationType> {
	static const std::vector<std::string>& Names() {
		static const std::vector<std::string> names = {
			"None", "ConstantX", "ConstantY", "ConstantZ", "Random", "Velocity", "Tumble"
		};
		return names;
	}
};

// ---- ParticleManagerEnums::MovementType の特殊化 ----
template <>
struct EnumTraits<ParticleManagerEnums::MovementType> {
	static const std::vector<std::string>& Names() {
		static const std::vector<std::string> names = {
			"Linear", "Curve", "Spiral", "Wave", "Bounce", "Orbit", "Zigzag"
		};
		return names;
	}
};