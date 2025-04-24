#pragma once
struct Vector2  final {
	float x;
	float y;
	Vector2 operator-(const Vector2& other) const {
		return { x - other.x, y - other.y };
	}

};

inline Vector2 operator*(const Vector2& v, float s) {
	return { v.x * s, v.y * s };
}