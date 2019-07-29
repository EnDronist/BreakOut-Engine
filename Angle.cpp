#include "Angle.h"
#define _USE_MATH_DEFINES
#include <cmath>
Angle::Angle(double value):
	value(value) {
	Update();
}
Angle::Angle(double x, double y):
	Angle(((x == 0.0) ? (y > 0.0 ? M_PI_2 : M_PI + M_PI_2) : atan((double)y / (double)x)) +
	((x < 0.0) ? (M_PI) : ((y < 0.0) ? (2 * M_PI) : 0.0))) {
}
Angle::Angle(Angle &rhs):
	value(rhs.value) {
}
double Angle::Dist(const Angle &rhs) const {
	double difference = rhs.value - value;
	if (difference >= M_PI) difference -= 2*M_PI;
	else if (difference < -M_PI) difference += 2*M_PI;
	return difference;
}
bool Angle::Between(const Angle &start, const Angle &end) const {
	double start_angle = (start.value > end.value ? start.value - 2 * M_PI : start.value),
		value_angle = (value > end.value ? value - 2 * M_PI : value);
	return value_angle >= start_angle && value_angle <= end.value;
}
Angle Angle::Reverse() const {
	return Angle(value + M_PI);
}
Angle& Angle::operator=(double value) {
	this->value = value;
	Update();
	return *this;
}
Angle Angle::GetAngle(double x, double y) {
	return Angle(x, y);
}
Angle& Angle::operator=(const Angle &rhs) {
	value = rhs.value;
	return *this;
}
Angle Angle::operator-(const Angle &rhs) {
	Angle angle(value - rhs.value);
	angle.Update();
	return angle;
}
Angle& Angle::operator-=(const Angle &rhs) {
	return *this = *this - rhs;
}
Angle Angle::operator+(const Angle &rhs) {
	Angle angle(value + rhs.value);
	angle.Update();
	return angle;
}
Angle& Angle::operator+=(const Angle &rhs) {
	return *this = *this + rhs;
}
bool Angle::operator>(const Angle &rhs) {
	return rhs.Dist(*this) > 0;
}
bool Angle::operator>=(const Angle &rhs) {
	return rhs.Dist(*this) >= 0;
}
bool Angle::operator<(const Angle &rhs) {
	return rhs.Dist(*this) < 0;
}
bool Angle::operator<=(const Angle &rhs) {
	return rhs.Dist(*this) <= 0;
}
bool Angle::operator==(const Angle &rhs) {
	return rhs.Dist(*this) == 0;
}
bool Angle::operator!=(const Angle &rhs) {
	return rhs.Dist(*this) != 0;
}
Angle::operator double() const {
	return value;
}
void Angle::Update() {
	if (abs(value) > 2 * M_PI) value -= (float)((2 * M_PI) * (int)(value / (2 * M_PI)));
	if (value < 0) value += 2 * M_PI;
}