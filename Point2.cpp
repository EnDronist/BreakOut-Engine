#include "Point2.h"
std::set<Point2*> Point2::all_points = std::set<Point2*>();
Point2::Point2(Vec2 pos, bool test):
	pos(pos), test(test) {
	if (!test) all_points.insert(this);
}
Point2::~Point2() {
	std::vector<Shape2*> shapes_vector(shapes.begin(), shapes.end());
	for (; shapes_vector.size() > 0;) {
		if (shapes_vector[0]->GetPoints().size() == 3) delete shapes_vector[0];
		else {
			try { shapes_vector[0]->RemovePoint(this); }
			catch (GameException&) {}
		}
		shapes_vector.erase(shapes_vector.begin());
	}
	std::vector<Polygon2*> polygons_vector(polygons.begin(), polygons.end());
	for (; polygons_vector.size() > 0;) {
		delete polygons_vector[0];
		polygons_vector.erase(polygons_vector.begin());
	}
	for (Line2 *line : lines) {
		line->GetShapes().clear();
		line->Replace(this, nullptr);
		delete line;
	}
	if (!test) all_points.erase(this);
}
void Point2::Draw(Graphics &gfx, Color color) {
	Vei2 draw_pos(pos);
	draw_pos *= Vec2(Settings::SizeMultiplierX(), Settings::SizeMultiplierY());
	if (draw_pos.x < 0 || draw_pos.x > Settings::ScreenWidth() - 1 ||
		draw_pos.y < 0 || draw_pos.y > Settings::ScreenHeight() - 1)
		return;
	gfx.PutPixel(draw_pos, color);
}
Vec2 Point2::GetPos() const { return pos; }
void Point2::SetPos(Vec2 pos) { this->pos = pos; }
void Point2::AddPos(Vec2 pos) { this->pos += pos; }
void Point2::Replace(Point2 *new_point) {
	//Swap line sets
	std::set<Line2*> lines_buffer = lines;
	lines = new_point->lines;
	new_point->lines = lines_buffer;
	//Find common lines
	lines_buffer.clear();
	std::set<Line2*> common_lines;
	std::set<Line2*> &less_lines = (lines.size() < new_point->lines.size() ? this->lines : new_point->lines);
	std::set<Line2*> &more_lines = (&less_lines == &this->lines ? new_point->lines : this->lines);
	std::set<Line2*>::iterator line_it;
	for (Line2 *line : less_lines) {
		if ((line_it = more_lines.find(line)) == more_lines.end())
			common_lines.insert(line);
	}
	//Swap points in lines
	/*for (int i = 0; i < 2; i++) {
		std::set<Line2*> &point_lines = (i == 0 ? lines : new_point->lines);
		for (Line2 *line : point_lines) {
			if ((line_it = common_lines.find(line)) != common_lines.end() ||
				(line_it = lines_buffer.find(line)) == lines_buffer.end()) {
				if (i == 0) line->Replace(this, new_point);
				else line->Replace(new_point, this);
				lines_buffer.insert(line);
			}
		}
	}*/
	for (int i = 0; i < 2; i++) {
		std::set<Line2*> &point_lines = (i == 0 ? lines : new_point->lines);
		for (Line2 *line : point_lines) {
			if (i == 0) line->Replace(new_point, this);
			else line->Replace(this, new_point);
		}
	}
	//Swap affiliation to polygons and shapes
	for (Polygon2 *polygon : polygons) polygon->Replace(this, new_point);
	for (Shape2 *shape : shapes) shape->Replace(this, new_point);
}
std::set<Shape2*>& Point2::GetShapes() { return shapes; }
std::set<Polygon2*>& Point2::GetPolygons() { return polygons; }
std::set<Line2*>& Point2::GetLines() { return lines; }
std::set<Point2*>& Point2::GetSet() { return all_points; }