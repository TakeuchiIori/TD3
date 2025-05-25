// Vector4.h の修正版（既存のVector4構造体に演算子を追加）

#pragma once
#include <cmath>

struct Vector4 {
    float x, y, z, w;

    // コンストラクタ
    Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    // 加算演算子
    Vector4 operator+(const Vector4& other) const {
        return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    // 減算演算子
    Vector4 operator-(const Vector4& other) const {
        return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    // スカラー乗算演算子（Vector4 * float）
    Vector4 operator*(float scalar) const {
        return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
    }

    // 要素ごとの乗算演算子（Vector4 * Vector4）
    Vector4 operator*(const Vector4& other) const {
        return Vector4(x * other.x, y * other.y, z * other.z, w * other.w);
    }

    // スカラー除算演算子
    Vector4 operator/(float scalar) const {
        return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
    }

    // 加算代入演算子
    Vector4& operator+=(const Vector4& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    // 減算代入演算子
    Vector4& operator-=(const Vector4& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    // スカラー乗算代入演算子
    Vector4& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }

    // 要素ごとの乗算代入演算子
    Vector4& operator*=(const Vector4& other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        w *= other.w;
        return *this;
    }

    // スカラー除算代入演算子
    Vector4& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }

    // 単項マイナス演算子
    Vector4 operator-() const {
        return Vector4(-x, -y, -z, -w);
    }

    // 等価演算子
    bool operator==(const Vector4& other) const {
        return (x == other.x && y == other.y && z == other.z && w == other.w);
    }

    // 非等価演算子
    bool operator!=(const Vector4& other) const {
        return !(*this == other);
    }

    // float * Vector4 の乗算演算子
    friend Vector4 operator*(float scalar, const Vector4& vec) {
        return Vector4(scalar * vec.x, scalar * vec.y, scalar * vec.z, scalar * vec.w);
    }
};

// Vector4用のlerp関数（関数宣言のみ - 実装はVector4.cppに移動）
Vector4 lerp(const Vector4& a, const Vector4& b, float t);