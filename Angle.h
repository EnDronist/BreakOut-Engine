#pragma once
class Angle {
public:
	Angle(double value);
	Angle(double x, double y);
	Angle(Angle &rhs);
	// From -180 to +180 degrees
	double Dist(const Angle &rhs) const;
	// From 0 to 360 degrees
	bool Between(const Angle &start, const Angle &end) const;
	Angle Reverse() const;
	Angle& operator=(double value);
	Angle GetAngle(double x, double y);
	Angle& operator=(const Angle &rhs);
	Angle operator-(const Angle &rhs);
	Angle& operator-=(const Angle &rhs);
	Angle operator+(const Angle &rhs);
	Angle& operator+=(const Angle &rhs);
	bool operator>(const Angle &rhs);
	bool operator>=(const Angle &rhs);
	bool operator<(const Angle &rhs);
	bool operator<=(const Angle &rhs);
	bool operator==(const Angle &rhs);
	bool operator!=(const Angle &rhs);
	operator double() const;
private:
	void Update();
private:
	double value;
};