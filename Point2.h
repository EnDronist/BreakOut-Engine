#pragma once
#include "GUI.h"
class Point2 {
//Global set
private:
	static std::set<Point2*> all_points;
//Operators
public:
	Point2(Vec2 pos, bool test = false);
	~Point2();
//Functions
public:
	void Draw(Graphics &gfx, Color color);
	Vec2 GetPos() const;
	void SetPos(Vec2 pos);
	void AddPos(Vec2 pos);
	void Replace(Point2 *new_point);
	std::set<Shape2*>& GetShapes();
	std::set<Polygon2*>& GetPolygons();
	std::set<Line2*>& GetLines();
	static std::set<Point2*>& GetSet();
//Variables
private:
	Vec2 pos;
	std::set<Shape2*> shapes;
	std::set<Polygon2*> polygons;
	std::set<Line2*> lines;
	bool test;
};