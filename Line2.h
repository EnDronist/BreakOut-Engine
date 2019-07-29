#pragma once
#include "GUI.h"
class Line2 {
//Global set
private:
	static std::set<Line2*> all_lines;
//Exception
	class Exception : public GameException {
	public:
		using GameException::GameException;
		std::wstring GetExceptionType() const override { return L"Line2 Exception"; }
	};
//Operators
public:
	Line2(const Point2 first, const Point2 second, bool test = false);
	Line2(Line2 &other, bool test = false);
	Line2(Point2 *first, Point2 *second, bool test = false);
	~Line2();
//Functions
public:
	void Update();
	void Draw(Graphics &gfx, Color color) const;
	bool IsIntersect(Line2 *rhs) const;
	bool IsIntersect(const Vec2 &pos) const;
	bool IsTouching(Line2 *rhs) const;
	bool IsTouching(const Vec2 &pos) const;
	Vec2 GetVector() const;
	Vec2 GetPos() const;
	void AddShape(Shape2 *shape);
	void AddPolygon(Polygon2 *polygon);
	Point2* GetFirst() const;
	void SetFirst(Point2 *point);
	Point2* GetSecond() const;
	void SetSecond(Point2* point);
	void Replace(Point2 *old_point, Point2 *new_point);
	Angle GetAngle() const;
	std::set<Shape2*>& GetShapes();
	std::set<Polygon2*>& GetPolygons();
	std::set<Point2*>& GetPoints();
	static std::set<Line2*>& GetSet();
//Variables
private:
	Point2 *first, *second;
	std::set<Shape2*> shapes;
	std::set<Polygon2*> polygons;
	std::set<Point2*> points;
	//Position (y = k*x + c)
	double k, c;
	bool vertical;
	bool test;
};