#include "Line2.h"
#include <assert.h>
std::set<Line2*> Line2::all_lines = std::set<Line2*>();
Line2::Line2(const Point2 first, const Point2 second, bool test):
	Line2(new Point2(first), new Point2(second)) {
}
Line2::Line2(Line2 &other, bool test):
	Line2(other.first, other.second) {
}
Line2::Line2(Point2 *first, Point2 *second, bool test):
	first(first), second(second), test(test),
	points{ this->first, this->second } {
	if (first == second) {
		throw Exception(L"Points are the same.", _CRT_WIDE(__FILE__), __LINE__);
	}
	if (!test) {
		this->first->GetLines().insert(this);
		this->second->GetLines().insert(this);
		all_lines.insert(this);
	}
	Update();
}
Line2::~Line2() {
	if (first != nullptr) first->GetLines().erase(this);
	if (second != nullptr) second->GetLines().erase(this);
	std::vector<Shape2*> shapes_vector(shapes.begin(), shapes.end());
	for (; shapes_vector.size() > 0;) {
		delete shapes_vector[0];
		shapes_vector.erase(shapes_vector.begin());
	}
	std::vector<Polygon2*> polygons_vector(polygons.begin(), polygons.end());
	for (; polygons_vector.size() > 0;) {
		delete polygons_vector[0];
		polygons_vector.erase(polygons_vector.begin());
	}
	if (!test) all_lines.erase(this);
}
void Line2::Update() {
	Vec2 pos_vector = second->GetPos() - first->GetPos();
	if (pos_vector.x == 0) { this->vertical = true; return; }
	this->vertical = false;
	this->k = pos_vector.y / pos_vector.x;
	Vec2 point_pos = first->GetPos();
	this->c = point_pos.y - this->k * point_pos.x;
}
void Line2::Draw(Graphics &gfx, Color color) const {
	if (Util::havenullptr(points)) {
		throw Exception(L"Point was nullptr.", _CRT_WIDE(__FILE__), __LINE__);
	}
	gfx.DrawLine(first->GetPos(), second->GetPos(), color, SpriteEffect::Copy());
}
bool Line2::IsIntersect(Line2 *rhs) const {
	if (this->k == rhs->k) return false;
	double collision = (rhs->c - this->c) / (this->k - rhs->k);
	//Vec2 collision_point = { collision_x, this->k * collision_x + this->c };
	double left = (first->GetPos().x < second->GetPos().x ? first->GetPos().x : second->GetPos().x),
		right = (first->GetPos().x >= second->GetPos().x ? first->GetPos().x : second->GetPos().x),
		up = 0, down = 0,
		rhs_left = (rhs->first->GetPos().x < rhs->second->GetPos().x ? rhs->first->GetPos().x : rhs->second->GetPos().x),
		rhs_right = (rhs->first->GetPos().x >= rhs->second->GetPos().x ? rhs->first->GetPos().x : rhs->second->GetPos().x),
		rhs_up = 0, rhs_down = 0;
	if (vertical) {
		collision = rhs->k * left + rhs->c;
		up = (first->GetPos().y >= second->GetPos().y ? first->GetPos().y : second->GetPos().y);
		down = (first->GetPos().y < second->GetPos().y ? first->GetPos().y : second->GetPos().y);
	}
	if (rhs->vertical) {
		collision = k * rhs_left + c;
		rhs_up = (rhs->first->GetPos().y >= rhs->second->GetPos().y ? rhs->first->GetPos().y : rhs->second->GetPos().y);
		rhs_down = (rhs->first->GetPos().y < rhs->second->GetPos().y ? rhs->first->GetPos().y : rhs->second->GetPos().y);
	}
	double anti_fault = 0.000001;
	bool conditions[4] = {
		(!vertical && !rhs->vertical ? collision > left + anti_fault && collision < right - anti_fault && collision > rhs_left + anti_fault && collision < rhs_right - anti_fault : false),
		(vertical && !rhs->vertical ? left < rhs_right && right > rhs_left && collision < up && collision > down : false),
		(!vertical && rhs->vertical ? rhs_left < right && rhs_right > left && collision < rhs_up && collision > rhs_down : false),
		(vertical && rhs->vertical ? left == rhs_left && up > rhs_down && down < rhs_up : false)
	};
	if (conditions[0] || conditions[1] || conditions[2] || conditions[3]) return true;
	else return false;
}
bool Line2::IsIntersect(const Vec2 &other) const {
	if (vertical) {
		double up = (first->GetPos().y >= second->GetPos().y ? first->GetPos().y : second->GetPos().y),
			down = (first->GetPos().y < second->GetPos().y ? first->GetPos().y : second->GetPos().y);
		if (other.x == first->GetPos().x && other.y < up && other.y > down) return true;
	}
	else {
		double collision = k * other.x + c,
			left = (first->GetPos().x < second->GetPos().x ? first->GetPos().x : second->GetPos().x),
			right = (first->GetPos().x >= second->GetPos().x ? first->GetPos().x : second->GetPos().x);
		if (collision > left && collision < right) return true;
	}
	return false;
}
bool Line2::IsTouching(Line2 *rhs) const {
	if (this->k == rhs->k) {
		if (this->c == rhs->c) return true;
		else return false;
	}
	double collision_x = (rhs->c - this->c) / (this->k - rhs->k);
	//Vec2 collision_point = { collision_x, this->k * collision_x + this->c };
	double left = (first->GetPos().x < second->GetPos().x ? first->GetPos().x : second->GetPos().x),
		right = (first->GetPos().x >= second->GetPos().x ? first->GetPos().x : second->GetPos().x),
		rhs_left = (rhs->first->GetPos().x < rhs->second->GetPos().x ? rhs->first->GetPos().x : rhs->second->GetPos().x),
		rhs_right = (rhs->first->GetPos().x >= rhs->second->GetPos().x ? rhs->first->GetPos().x : rhs->second->GetPos().x);
	if (collision_x >= left && collision_x <= right && collision_x >= rhs_left && collision_x <= rhs_right)
		return true;
	else return false;
}
bool Line2::IsTouching(const Vec2 &other) const {
	if (vertical) {
		double up = (first->GetPos().y >= second->GetPos().y ? first->GetPos().y : second->GetPos().y),
			down = (first->GetPos().y < second->GetPos().y ? first->GetPos().y : second->GetPos().y);
		if (other.x == first->GetPos().x && other.y <= up && other.y >= down) return true;
	}
	else {
		double collision = k * other.x + c,
			left = (first->GetPos().x < second->GetPos().x ? first->GetPos().x : second->GetPos().x),
			right = (first->GetPos().x >= second->GetPos().x ? first->GetPos().x : second->GetPos().x);
		if (collision >= left && collision <= right) return true;
	}
	return false;
}
Vec2 Line2::GetVector() const {
	return second->GetPos() - first->GetPos();
}
Vec2 Line2::GetPos() const {
	if (first == nullptr || second == nullptr) {
		throw Exception(L"Point was nullptr.", _CRT_WIDE(__FILE__), __LINE__);
	}
	return (first->GetPos() + second->GetPos()) / 2.0;
}
void Line2::AddShape(Shape2 *shape) {
	shapes.insert(shape);
	first->GetShapes().insert(shape);
	second->GetShapes().insert(shape);
}
void Line2::AddPolygon(Polygon2 *polygon) {
	polygons.insert(polygon);
	first->GetPolygons().insert(polygon);
	second->GetPolygons().insert(polygon);
}
Point2* Line2::GetFirst() const {
	return first;
}
void Line2::SetFirst(Point2 *point) {
	if (points.size() == 2) points.erase(first);
	first = point;
	points.insert(first);
}
Point2* Line2::GetSecond() const {
	return second;
}
void Line2::SetSecond(Point2 *point) {
	if (points.size() == 2) points.erase(second);
	second = point;
	points.insert(second);
}
void Line2::Replace(Point2 *old_point, Point2 *new_point) {
	if (first == old_point) SetFirst(new_point);
	else if (second == old_point) SetSecond(new_point);
}
Angle Line2::GetAngle() const {
	if (first == nullptr || second == nullptr) {
		throw Exception(L"Point was nullptr.", _CRT_WIDE(__FILE__), __LINE__);
	}
	return (second->GetPos() - first->GetPos()).GetAngle();
}
std::set<Shape2*>& Line2::GetShapes() { return shapes; }
std::set<Polygon2*>& Line2::GetPolygons() { return polygons; }
std::set<Point2*>& Line2::GetPoints() { return points; }
std::set<Line2*>& Line2::GetSet() { return all_lines; }