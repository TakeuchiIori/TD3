// EnumUtils.h

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

// ---- BlendModeに対する特殊化 ----
enum class BlendMode {
	kBlendModeNone,
	kBlendModeNormal,
	kBlendModeAdd,
	kBlendModeSubtract,
	kBlendModeMultiply,
	kBlendModeScreen,
	kCount0fBlendMode,
};

template <>
struct EnumTraits<BlendMode> {
	static const std::vector<std::string>& Names() {
		static const std::vector<std::string> names = {
			"None", "Normal", "Add", "Subtract", "Multiply", "Screen"
		};
		return names;
	}
};
