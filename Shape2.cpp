#include "Shape2.h"
#include <vector>
std::set<Shape2*> Shape2::all_shapes = std::set<Shape2*>();
Shape2::Shape2(std::vector<Line2*> &lines, bool is_traversal_shape):
	is_traversal_shape(is_traversal_shape) {
	//Start checks
	if (lines.size() < 3) {
		throw Exception(L"Not enough lines to create a polygon (creating shape).", _CRT_WIDE(__FILE__), __LINE__);
	}
	//Checking for right traversal direction
	double d_angle = 0;
	for (int i = 0; i < lines.size(); i++) {
		if (lines[i]->GetFirst() != lines[i == 0 ? lines.size() - 1 : i - 1]->GetSecond()) {
			throw Exception(L"Lines not connected (creating shape).", _CRT_WIDE(__FILE__), __LINE__);
		}
		for (Line2 *line : lines) {
			if (line->IsIntersect(lines[i]))
				throw Exception(L"Lines intersected (creating shape).", _CRT_WIDE(__FILE__), __LINE__);
		}
		d_angle += (double)lines[i - 1 < 0 ? lines.size() - 1 : i - 1]->GetAngle().Dist(lines[i]->GetAngle());
	}
	if (d_angle > 0) for (int i = 0; i < (lines.size() - 1) / 2; i++) {
		lines[i]->GetSecond()->Replace(lines[lines.size() - i - 1]->GetFirst());
	}
	//Finishing
	this->lines = lines;
	for (Line2 *line : this->lines) {
		points.insert(line->GetFirst());
		line->AddShape(this);
	}
	if (!is_traversal_shape) Triangulate();
	all_shapes.insert(this);
}
Shape2::~Shape2() {
	for (Polygon2 *polygon : polygons) {
		polygon->GetShapes().erase(this);
		delete polygon;
	}
	for (Point2 *point : points) point->GetShapes().erase(this);
	for (Line2 *line : lines) line->GetShapes().erase(this);
	all_shapes.erase(this);
}
void Shape2::AddPoint(Point2 *point, Line2 *where) {
	//Creating new lines
	Line2 *new_first(new Line2(where->GetFirst(), point)),
		*new_second(new Line2(point, where->GetSecond()));
	//Finding adjacent lines
	Line2 *prev_line = nullptr, *next_line = nullptr;
	for (int i = 0; i < 2; i++) {
		Line2 *&finding_line = (i == 0 ? prev_line : next_line);
		Point2 *point_to_find = (i == 0 ? where->GetFirst() : where->GetSecond());
		for (Line2 *line : point_to_find->GetLines()) {
			if (line == where) continue;
			std::vector<Line2*>::iterator line_it;
			if ((line_it = std::find(lines.begin(), lines.end(), line)) != lines.end()) {
				finding_line = *line_it; break;
			}
		}
	}
	//Checking new lines for collision
	bool first_condition = where->GetAngle() > prev_line->GetAngle() ?
		new_first->GetAngle().Between(where->GetAngle().Reverse(), prev_line->GetAngle().Reverse()):
		new_first->GetAngle().Between(prev_line->GetAngle().Reverse(), where->GetAngle().Reverse()),
		second_condition = next_line->GetAngle().Reverse() > where->GetAngle().Reverse() ?
		new_second->GetAngle().Between(next_line->GetAngle().Reverse(), where->GetAngle().Reverse()):
		new_second->GetAngle().Between(where->GetAngle().Reverse(), next_line->GetAngle().Reverse());
	if (!first_condition || !second_condition) {
		delete new_first; delete new_second;
		throw Exception(L"Incorrect angle of new line (adding point).", _CRT_WIDE(__FILE__), __LINE__);
	}
	for (Line2 *line : lines) {
		if (line == where) continue;
		if (new_first->IsIntersect(line) || new_second->IsIntersect(line)) {
			delete new_first; delete new_second;
			throw Exception(L"Lines intersected (adding point).", _CRT_WIDE(__FILE__), __LINE__);
		}
	}
	//Finishing
	bool is_new_polygon = new_first->GetAngle() > where->GetAngle() && new_second->GetAngle() < where->GetAngle();//Is it possible to create a new polygon
	std::vector<Line2*>::iterator replace_line_it = std::find(lines.begin(), lines.end(), where);
	replace_line_it = lines.erase(replace_line_it);
	where->GetShapes().erase(this);
	delete where;
	points.insert(point);
	point->GetShapes().insert(this);
	for (int i = 0; i < 2; i++) {
		Line2 *current_line = i == 0 ? new_second : new_first;
		current_line->GetShapes().insert(this);
		replace_line_it = lines.insert(replace_line_it, current_line);
		point->GetLines().insert(current_line);
	}
	if (!is_traversal_shape) {
		if (is_new_polygon) {
			//Adding new polygon
			polygons.insert(new Polygon2(std::vector<Point2*>{
				new_first->GetFirst(),
				new_second->GetFirst(),
				new_second->GetSecond()
			}));
		}
		else Triangulate();
	}
}
void Shape2::RemovePoint(Point2 *point, bool test) {
	//Check points count
	if (points.size() == 3) {
		throw Exception(L"Points can't be less than 3 (removing point).", _CRT_WIDE(__FILE__), __LINE__);
	}
	//Finding point to be deleted in shape
	std::vector<Line2*> deleting_lines;
	std::vector<Line2*>::iterator line_it;
	for (Line2 *line : point->GetLines())
		if ((line_it = std::find(lines.begin(), lines.end(), line)) != lines.end()) deleting_lines.push_back(line);
	if (deleting_lines.size() < 2) {
		throw Exception(L"Point not connected (removing point).", _CRT_WIDE(__FILE__), __LINE__);
	}
	//Finding adjacent lines
	Line2 *prev_deleting_line, *next_deleting_line;
	Point2 *neighboring_points[2];
	if (deleting_lines[0]->GetSecond() == point) {
		prev_deleting_line = deleting_lines[0];
		neighboring_points[0] = prev_deleting_line->GetFirst();
		next_deleting_line = deleting_lines[1];
		neighboring_points[1] = next_deleting_line->GetSecond();
	}
	else {
		prev_deleting_line = deleting_lines[1];
		neighboring_points[0] = prev_deleting_line->GetFirst();
		next_deleting_line = deleting_lines[0];
		neighboring_points[1] = next_deleting_line->GetSecond();
	}
	if (prev_deleting_line == nullptr || next_deleting_line == nullptr) {
		throw Exception(L"Lines not connected (removing point).", _CRT_WIDE(__FILE__), __LINE__);
	}
	//Checking new line for collision
	Line2 *new_line = new Line2(neighboring_points[0], neighboring_points[1]);
	for (Line2 *line : lines) {
		if (new_line->IsIntersect(line)) {
			delete new_line;
			throw Exception(L"Lines intersected (removing point).", _CRT_WIDE(__FILE__), __LINE__);
		}
	}
	//Finding adjacent lines of adjacent lines
	Line2 *prev_line, *next_line;
	for (Line2 *line : neighboring_points[0]->GetLines()) {
		if ((line_it = std::find(lines.begin(), lines.end(), line)) != lines.end() && line != prev_deleting_line) {
			prev_line = line;
			break;
		}
	}
	for (Line2 *line : neighboring_points[1]->GetLines()) {
		if ((line_it = std::find(lines.begin(), lines.end(), line)) != lines.end() && line != next_deleting_line) {
			next_line = line;
			break;
		}
	}
	//Checking for right traversal direction
	double d_angle = 0, new_d_angle = 0;
	for (int i = 0; i < 3; i++) {
		Line2 *first_line = i == 1 ? prev_line : i == 2 ? prev_deleting_line : next_deleting_line,
			*second_line = i == 1 ? prev_deleting_line : i == 2 ? next_deleting_line : next_line;
		d_angle += first_line->GetAngle().Dist(second_line->GetAngle());
	}
	for (int i = 0; i < 2; i++) {
		Line2 *first_line = i == 1 ? prev_line : new_line,
			*second_line = i == 1 ? new_line : next_line;
		new_d_angle += first_line->GetAngle().Dist(second_line->GetAngle());
	}
	if (abs(new_d_angle - d_angle) > 0.000001) {
		delete new_line;
		throw Exception(L"Incorrect angle of new line (removing point).", _CRT_WIDE(__FILE__), __LINE__);
	}
	if (test) { delete new_line; return; }
	//Finishing
	std::vector<Line2*>::iterator new_line_it;
	for (int i = 0; i < deleting_lines.size(); i++) {
		new_line_it = lines.erase(std::find(lines.begin(), lines.end(), deleting_lines[i]));
		deleting_lines[i]->GetShapes().erase(this);
		delete deleting_lines[i];
	}
	lines.insert(new_line_it, new_line);
	new_line->GetShapes().insert(this);
	points.erase(point);
	point->GetShapes().erase(this);
	if (!is_traversal_shape) Triangulate();
}
void Shape2::Replace(Point2 *old_point, Point2 *new_point) {
	std::set<Point2*>::iterator point_it;
	if ((point_it = points.find(old_point)) != points.end()) {
		points.erase(old_point);
		points.insert(new_point);
	}
}
void Shape2::Triangulate() {
	//Deleting polygons
	for (Polygon2 *polygon : polygons) delete polygon;
	polygons.clear();
	//Creating new polygons
	std::vector<Line2*> created_lines_to_traversal;
	for (Line2 *line : lines) created_lines_to_traversal.push_back(new Line2(*line));
	Shape2 shape_to_traversal(created_lines_to_traversal, true);
	std::vector<Line2*> &lines_to_traversal = shape_to_traversal.GetLines();
	for (int i = 0; lines_to_traversal.size() > 3; i >= lines_to_traversal.size() - 1 ? i = 0 : i++) {
		Line2 *&next_lines_to_traversal = lines_to_traversal[i == lines_to_traversal.size() - 1 ? 0 : i + 1];
		std::vector<Point2*> removing_points{
			lines_to_traversal[i]->GetFirst(),
			lines_to_traversal[i]->GetSecond(),
			next_lines_to_traversal->GetSecond()
		};
		if (lines_to_traversal[i]->GetAngle().Dist(next_lines_to_traversal->GetAngle()) < 0) {
			try {
				shape_to_traversal.RemovePoint(lines_to_traversal[i]->GetSecond());
				polygons.insert(new Polygon2(removing_points));
			}
			catch (GameException&) {}
		}
	}
	polygons.insert(new Polygon2(std::vector<Point2*>{
		lines_to_traversal[0]->GetFirst(),
		lines_to_traversal[1]->GetFirst(),
		lines_to_traversal[2]->GetFirst()
	}));
	//Clearing shape for traversal
	for (; lines_to_traversal.size() > 0; ) {
		(*lines_to_traversal.begin())->GetShapes().erase(&shape_to_traversal);
		delete *lines_to_traversal.begin();
		shape_to_traversal.GetLines().erase(lines_to_traversal.begin());
	}
}
std::set<Polygon2*>& Shape2::GetPolygons() { return polygons; }
std::vector<Line2*>& Shape2::GetLines() { return lines; }
std::set<Point2*>& Shape2::GetPoints() { return points; }
std::set<Shape2*>& Shape2::GetSet() { return all_shapes; }