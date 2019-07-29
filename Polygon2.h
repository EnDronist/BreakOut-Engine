#pragma once
#include "GUI.h"
class Polygon2 {
//Global set
private:
	static std::set<Polygon2*> all_polygons;
//Exception
public:
	class Exception : public GameException {
	public:
		using GameException::GameException;
		std::wstring GetExceptionType() const override { return L"Polygon2 Exception"; }
	};
//Operators
public:
	Polygon2(std::vector<Point2*> &points);
	~Polygon2();
//Funtions
public:
	void Draw(Graphics &gfx, Color color);
	void Replace(Point2 *old_point, Point2 *new_point);
	std::set<Shape2*>& GetShapes();
	std::set<Line2*>& GetLines();
	std::set<Point2*>& GetPoints();
	static std::set<Polygon2*>& GetSet();
	//Надо сделать коллизию полигонов и разбиение фигуры на полигоны, добавление полигонов в фигуру в функциях
	//Надо сделать функцию создания фигуры (+кнопка)
//Variables
public:
	std::set<Shape2*> shapes;
	std::set<Line2*> lines;
	std::set<Point2*> points;
	Color color;
};