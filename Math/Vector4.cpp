// Vector4.cpp に追加する実装

#include "Vector4.h"

// Vector4用のlerp関数の実装
Vector4 lerp(const Vector4& a, const Vector4& b, float t) {
    return Vector4(
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t,
        a.w + (b.w - a.w) * t
    );
}