#include "Polygon2.h"
#include <algorithm>
#include <iostream>
#include <cstdlib>
std::set<Polygon2*> Polygon2::all_polygons = std::set<Polygon2*>();
Polygon2::Polygon2(std::vector<Point2*> &points) {
	srand((unsigned int)time(NULL));
	this->color = Color{
		(unsigned char)(std::rand() % 191 + 64),
		(unsigned char)(std::rand() % 191 + 64),
		(unsigned char)(std::rand() % 191 + 64)
	};
	//Start checks
	if (points.size() < 3) {
		throw Exception(L"Not enough points to create a polygon (creating polygon).", _CRT_WIDE(__FILE__), __LINE__);
	}
	else if (points.size() > 3) {
		throw Exception(L"Too much points to create a polygon (creating polygon).", _CRT_WIDE(__FILE__), __LINE__);
	}
	//Creating lines
	std::vector<Line2*> new_lines;
	for (int i = 0; i < points.size(); i++) new_lines.push_back(new Line2(points[i == 0 ? points.size() - 1 : i - 1], points[i]));
	//Checking for right traversal direction
	double d_angle = 0;
	for (int i = 0; i < new_lines.size(); i++) {
		d_angle += (double)new_lines[i - 1 < 0 ? new_lines.size() - 1 : i - 1]->GetAngle().Dist(new_lines[i]->GetAngle());
	}
	if (d_angle > 0) for (int i = 0; i < (new_lines.size() - 1) / 2; i++) {
		new_lines[i]->GetSecond()->Replace(new_lines[new_lines.size() - i - 1]->GetFirst());
	}
	//Finishing
	lines.insert(new_lines.begin(), new_lines.end());
	for (Line2 *line : lines) {
		this->points.insert(line->GetFirst());
		line->AddPolygon(this);
	}
	all_polygons.insert(this);
}
Polygon2::~Polygon2() {
	for (Shape2 *shape : shapes) shape->GetPolygons().erase(this);
	for (Line2 *line : lines) {
		line->GetPolygons().erase(this);
		delete line;
	}
	for (Point2 *point : points) point->GetPolygons().erase(this);
	all_polygons.erase(this);
}
void Polygon2::Draw(Graphics &gfx, Color color) {
	std::set<Point2*>::iterator points_it = points.begin();
	gfx.DrawTriangle((*(points_it))->GetPos(), (*(points_it++))->GetPos(), (*(points_it++))->GetPos(), this->color, SpriteEffect::Ghost());
}
void Polygon2::Replace(Point2 *old_point, Point2 *new_point) {
	std::set<Point2*>::iterator point_it;
	for (Line2 *line : lines) if ((point_it = line->GetPoints().find(old_point)) != line->GetPoints().end())
		line->Replace(old_point, new_point);
}
std::set<Shape2*>& Polygon2::GetShapes() { return shapes; }
std::set<Line2*>& Polygon2::GetLines() { return lines; }
std::set<Point2*>& Polygon2::GetPoints() { return points; }
std::set<Polygon2*>& Polygon2::GetSet() { return all_polygons; }