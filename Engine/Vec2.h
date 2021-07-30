#pragma once
#include <string>
#include <cmath>
template<typename T>
class Vec2_ {
public:
	Vec2_() = default;
	Vec2_(T x, T y):
		x(x),
		y(y)
	{}
	template<typename S>
	explicit Vec2_(const Vec2_<S>& src):
		x((T)src.x),
		y((T)src.y)
	{}
	// Vec2 + Vec2
	Vec2_ operator+(const Vec2_& rhs) const {
		return Vec2_(x + rhs.x, y + rhs.y);
	}
	Vec2_& operator+=(const Vec2_& rhs) {
		return *this = *this + rhs;
	}
	// Vec2 - Vec2
	Vec2_ operator-(const Vec2_& rhs) const {
		return Vec2_(x - rhs.x, y - rhs.y);
	}
	Vec2_& operator-=(const Vec2_& rhs) {
		return *this = *this - rhs;
	}
	// Vec2 * value
	Vec2_ operator*(T rhs) const {
		return Vec2_(x * rhs, y * rhs);
	}
	Vec2_& operator*=(T rhs) {
		return *this = *this * rhs;
	}
	// Vec2 * Vec2
	Vec2_ operator*(const Vec2_& rhs) const {
		return Vec2_(x * rhs.x, y * rhs.y);
	}
	Vec2_& operator*=(const Vec2_& rhs) {
		return *this = *this * rhs;
	}
	// Vec2 / value
	Vec2_ operator/(T rhs) const {
		return Vec2_(x / rhs, y / rhs);
	}
	Vec2_& operator/=(T rhs) {
		return *this = *this / rhs;
	}
	// Vec2 / Vec2
	Vec2_ operator/(const Vec2_& rhs) const {
		return Vec2_(x / rhs.x, y / rhs.y);
	}
	Vec2_& operator/=(const Vec2_& rhs) {
		return *this = *this / rhs;
	}
	// Gets
	T GetLength() const {
		return (T)std::sqrt(GetLengthSq());
	}
	T GetLengthSq() const {
		return x * x + y * y;
	}
	Vec2_& Normalize() {
		return *this = GetNormalized();
	}
	Vec2_ GetNormalized() const {
		const T len = GetLength();
		if (len != (T)0){
			return *this * ((T)1 / len);
		}
		return *this;
	}
	T GetAngle() const {
		return ((x == (T)0) ? acos(0) : atan(y / x)) +
			((x < (T)0) ? (T)(2 * acos(0)) : ((y < (T)0) ? (T)(4 * acos(0)) : (T)0));
	}
	// Static Gets
	static T GetAngle(Vec2_ first_point, Vec2_ second_point) {
		return Vec2_(second_point - first_point).GetAngle();
	}
	// Info
	void ToString(std::string &object_text) {
		// T x
		object_text += std::to_string(x); object_text += " ";
		// T y
		object_text += std::to_string(y); object_text += " ";
	}
	void FromString(std::string &object_text) {
		// T x
		if (typeid(int) == typeid(T)) this->x = (T)atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
		else if (typeid(double) == typeid(T)) this->x = (T)atof(object_text.substr(size_t(0), object_text.find(' ')).c_str());
		object_text.erase(0, object_text.find(' ') + 1);
		// T y
		if (typeid(int) == typeid(T)) this->y = (T)atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
		else if (typeid(double) == typeid(T)) this->y = (T)atof(object_text.substr(size_t(0), object_text.find(' ')).c_str());
		object_text.erase(0, object_text.find(' ') + 1);
	}
public:
	T x;
	T y;
};
typedef Vec2_<double> Vec2;
typedef Vec2_<int> Vei2;