#pragma once
#include "GUI.h"
class Shape2 {
//Global set
private:
	static std::set<Shape2*> all_shapes;
//Exception
public:
	class Exception : public GameException {
	public:
		using GameException::GameException;
		std::wstring GetExceptionType() const override { return L"Shape2 Exception"; }
	};
//Operators
public:
	Shape2(std::vector<Line2*> &lines, bool is_traversal_shape = false);
	~Shape2();
//Funtions
public:
	void AddPoint(Point2 *point, Line2 *where);
	void RemovePoint(Point2 *point, bool test = false);
	void Replace(Point2 *old_point, Point2 *new_point);
	void Triangulate();
	std::set<Polygon2*>& GetPolygons();
	std::vector<Line2*>& GetLines();
	std::set<Point2*>& GetPoints();
	static std::set<Shape2*>& GetSet();
//Variables
public:
	std::set<Polygon2*> polygons;
	std::vector<Line2*> lines;
	std::set<Point2*> points;
	bool is_traversal_shape;
};