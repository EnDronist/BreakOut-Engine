#pragma once
#include "Vec2.h"
#include <cmath>
#include <stdlib.h>
#include <vector>
class Graphics;
template<typename T>
class Hitbox_ {
public:
	enum Type {
		Rect,
		Ellipse,
		Line,//left-top - start, right-bottom - end
	};
	template<typename S>
	explicit Hitbox_(const Hitbox_<S> &rhs) :
		left((T)left_in),
		right((T)right_in),
		top((T)top_in),
		bottom((T)bottom_in),
		type(type),
		gfx(gfx) {
	}
	Hitbox_(T left_in, T right_in, T top_in, T bottom_in, Type type = Type::Rect, Graphics *gfx = nullptr):
		left(left_in),
		right(right_in),
		top(top_in),
		bottom(bottom_in),
		type(type),
		gfx(gfx) {
	}
	Hitbox_(const Vec2_<T> topLeft, const Vec2_<T> bottomRight, Type type = Type::Rect, Graphics *gfx = nullptr):
		Hitbox_(topLeft.x, bottomRight.x, topLeft.y, bottomRight.y, type, gfx) {
	}
	Hitbox_(const Vec2_<T> topLeft, const T width, const T height, Type type = Type::Rect, Graphics *gfx = nullptr):
		Hitbox_(topLeft, topLeft + Vec2_<T>(width, height), type, gfx) {
	}
	Hitbox_(const Hitbox_ &other, Type type):
		Hitbox_(other) {
		this->type = type;
		this->gfx = gfx;
	}
	// Operators
	Hitbox_ operator+(const Vec2_<T> &rhs) const {
		return Hitbox_(left + rhs.x, right + rhs.x, top + rhs.y, bottom + rhs.y, type, gfx);
	}
	Hitbox_& operator+=(const Vec2_<T> &rhs) {
		return *this = *this + rhs;
	}
	Hitbox_ operator-(const Vec2_<T> &rhs) const {
		return Hitbox_(left - rhs.x, right - rhs.x, top - rhs.y, bottom - rhs.y, type, gfx);
	}
	Hitbox_& operator-=(const Vec2_<T> &rhs) {
		return *this = *this - rhs;
	}
	Hitbox_& operator=(const Vec2_<T> &rhs) {
		return *this = Hitbox_(rhs.x, rhs.x + this->GetWidth(), rhs.y, rhs.y + this->GetHeight(), type, gfx);
	}
	// Is other full in this
	bool IsContain(const Vec2_<T> &point, bool draw_lines = false) const {
		draw_lines = (draw_lines && gfx != nullptr && Settings::DrawCollisionLines());
		// this Rect
		if (this->type == Type::Rect) {
			return left <= point.x && right >= point.x && top <= point.y && bottom >= point.y;
		}
		// this Circle
		if (this->type == Type::Ellipse) {
			Vec2 centers_distance = point - this->GetCenter();
			if (draw_lines) gfx->DrawLine(this->GetCenter(), point, { 150, 150, 0 }, SpriteEffect::Copy());
			double angle = Vec2::GetAngle(this->GetCenter(), point);
			double root_size = 1.0 / sqrt(pow((sin(angle) / this->GetHeight()), 2) + pow((cos(angle) / this->GetWidth()), 2));
			Vec2 this_distance{ (root_size / 2) * cos(angle), (root_size / 2) * sin(angle) };
			if (draw_lines) gfx->DrawLine(this->GetCenter(), this->GetCenter() + this_distance, { 150, 150, 0 }, SpriteEffect::Copy());
			return centers_distance.GetLength() <= this_distance.GetLength();
		}
		else return false;
	}
	bool IsContain(const Hitbox_ &other, bool draw_lines = false) const {
		draw_lines = (draw_lines && gfx != nullptr && Settings::DrawCollisionLines());
		// this Rect - other Rect
		if (this->type == Type::Rect && other.type == Type::Rect)
			return left <= other.left && right >= other.right && top <= other.top && bottom >= other.bottom;
		// this Rect - other Ellipse
		else if (this->type == Type::Rect && other.type == Type::Ellipse) {
			if (Hitbox_(*this, Type::Rect).IsTouch(Hitbox_(other, Type::Rect), draw_lines)) {
				if (Hitbox_(*this, Type::Rect).IsContain(Hitbox_(other, Type::Rect), draw_lines)) return true;
				else {
					Vec2 centers_distance = other.GetCenter() - this->GetCenter();
					if (draw_lines) gfx->DrawLine(this->GetCenter(), this->GetCenter() + centers_distance, { 150, 150, 0 }, SpriteEffect::Copy());
					/*double min_size = (this->GetWidth() < this->GetHeight()) ? this->GetWidth() / 2 : this->GetHeight() / 2;
					double angle = Vec2::GetAngle(this->GetCenter(), other.GetCenter());
					double real_angle = (4 * acos(0)) - angle;
					double pi_eighths = angle / (0.5 * acos(0));
					Vec2 rect_radius = {
					(fmod(pi_eighths + 1.0, 8) < 2) ? min_size :
					(fmod(pi_eighths + 3.0, 8) < 2) ? -min_size + fmod(pi_eighths + 3.0, 8) * min_size :
					(fmod(pi_eighths + 5.0, 8) < 2) ? -min_size :
					(fmod(pi_eighths + 7.0, 8) < 2) ? min_size + fmod(pi_eighths + 7.0, 8) * -min_size : 0.0,
					(fmod(pi_eighths + 1.0, 8) < 2) ? -min_size + fmod(pi_eighths + 1.0, 8) * min_size :
					(fmod(pi_eighths + 3.0, 8) < 2) ? -min_size :
					(fmod(pi_eighths + 5.0, 8) < 2) ? min_size + fmod(pi_eighths + 5.0, 8) * -min_size :
					(fmod(pi_eighths + 7.0, 8) < 2) ? min_size : 0.0
					};
					Vec2 rect_distance = (this->GetWidth() >= this->GetHeight()) ?
					Vec2{ rect_radius.x * this->GetWidth() / this->GetHeight(), rect_radius.y } :
					Vec2{ rect_radius.x, rect_radius.y * this->GetHeight() / this->GetWidth() };
					if (draw_lines) gfx->DrawLine(this->GetCenter(), this->GetCenter() + rect_distance, { 150, 150, 0 }, SpriteEffect::Copy());
					double root_size = 1.0 / sqrt(pow((sin(angle) / other.GetHeight()), 2) + pow((cos(angle) / other.GetWidth()), 2));
					Vec2 ellipse_distance{ -(root_size / 2) * cos(angle), -(root_size / 2) * sin(angle) };
					if (draw_lines) gfx->DrawLine(other.GetCenter(), other.GetCenter() + ellipse_distance, { 150, 150, 0 }, SpriteEffect::Copy());
					//return rect_distance.GetLength() + ellipse_distance.GetLength() <= centers_distance.GetLength();*/
					if (other.IsTouch({ this->left, this->top }, draw_lines) &&
						other.IsTouch({ this->left, this->bottom }, draw_lines) &&
						other.IsTouch({ this->right, this->top }, draw_lines) &&
						other.IsTouch({ this->right, this->bottom }, draw_lines) &&
						IsTouch({ other.GetCenter().x, other.top }, draw_lines) &&
						IsTouch({ other.GetCenter().x, other.bottom }, draw_lines) &&
						IsTouch({ other.left, other.GetCenter().y }, draw_lines) &&
						IsTouch({ other.right, other.GetCenter().y }, draw_lines)) return true;
					else return false;
				}
			}
		}
		// this Ellipse - other Rect
		else if (this->type == Type::Ellipse && other.type == Type::Rect) {
			return other.IsContain(*this, draw_lines);
		}
		// this Ellipse - other Ellipse
		else if (this->type == Type::Ellipse && other.type == Type::Ellipse) {
			if (Hitbox_(*this, Type::Rect).IsContain(Hitbox_(other, Type::Rect), draw_lines)) {
				Vec2 centers_distance = other.GetCenter() - this->GetCenter();
				double angle = Vec2::GetAngle(this->GetCenter(), other.GetCenter());
				Vec2 this_distance = { (this->GetWidth() / 2.0) * cos(angle), (this->GetHeight() / 2.0) * sin(angle) };
				Vec2 other_distance = { (other.GetWidth() / 2.0) * cos(angle), (other.GetHeight() / 2.0) * sin(angle) };
				Vec2 inside_distance = centers_distance + other_distance;
				if (draw_lines) gfx->DrawLine(other.GetCenter(), this->GetCenter(), { 150, 150, 0 }, SpriteEffect::Copy());
				if (draw_lines) gfx->DrawLine(this->GetCenter(), this->GetCenter() + this_distance, { 150, 150, 0 }, SpriteEffect::Copy());
				if (draw_lines) gfx->DrawLine(other.GetCenter(), other.GetCenter() + other_distance, { 150, 150, 0 }, SpriteEffect::Copy());
				if (draw_lines) gfx->DrawLine(this->GetCenter(), this->GetCenter() + inside_distance, { 150, 150, 0 }, SpriteEffect::Copy());
				if (draw_lines) gfx->DrawHitbox(*this, SpriteEffect::Chess({ 150, 150, 0 }));
				Vec2 other_center = other.GetCenter();
				if (!(IsContain({ other.left, other_center.y }, draw_lines) &&
					IsContain({ other.right, other_center.y }, draw_lines) &&
					IsContain({ other_center.x, other.top }, draw_lines) &&
					IsContain({ other_center.x, other.bottom }, draw_lines))) return false;
				else return inside_distance.GetLength() <= this_distance.GetLength();
			}
		}
		return false;
	}
	// Is this full in other
	bool IsInside(const Hitbox_ &other, bool draw_lines = false) const {
		return other.IsContain(*this, draw_lines);
	}
	// Is other partially or full in this
	bool IsTouch(const Vec2_<T> &point, bool draw_lines = false) const {
		return IsContain(point, draw_lines);
	}
	bool IsTouch(const Hitbox_ &other, bool draw_lines = false) const {
		draw_lines = (draw_lines && gfx != nullptr && Settings::DrawCollisionLines());
		// this Rect - other Rect
		if (this->type == Type::Rect && other.type == Type::Rect)
			return left <= other.right && right >= other.left && top <= other.bottom && bottom >= other.top;
		// this Rect - other Ellipse
		else if (this->type == Type::Rect && other.type == Type::Ellipse) {
			if (Hitbox_(*this, Type::Rect).IsTouch(Hitbox_(other, Type::Rect), draw_lines)) {
				if (Hitbox_(*this, Type::Rect).IsContain(Hitbox_(other, Type::Rect), draw_lines)) return true;
				Vec2 centers_distance = other.GetCenter() - this->GetCenter();
				if (draw_lines) gfx->DrawLine(this->GetCenter(), this->GetCenter() + centers_distance, { 150, 150, 0 }, SpriteEffect::Copy());
				/*double min_size = (this->GetWidth() < this->GetHeight()) ? this->GetWidth() / 2 : this->GetHeight() / 2;
				double angle = Vec2::GetAngle(this->GetCenter(), other.GetCenter());
				double real_angle = (4 * acos(0)) - angle;
				double pi_eighths = angle / (0.5 * acos(0));
				Vec2 rect_radius = {
					(fmod(pi_eighths + 1.0, 8) < 2) ? min_size :
					(fmod(pi_eighths + 3.0, 8) < 2) ? -min_size + fmod(pi_eighths + 3.0, 8) * min_size :
					(fmod(pi_eighths + 5.0, 8) < 2) ? -min_size :
					(fmod(pi_eighths + 7.0, 8) < 2) ? min_size + fmod(pi_eighths + 7.0, 8) * -min_size : 0.0,
					(fmod(pi_eighths + 1.0, 8) < 2) ? -min_size + fmod(pi_eighths + 1.0, 8) * min_size :
					(fmod(pi_eighths + 3.0, 8) < 2) ? -min_size :
					(fmod(pi_eighths + 5.0, 8) < 2) ? min_size + fmod(pi_eighths + 5.0, 8) * -min_size :
					(fmod(pi_eighths + 7.0, 8) < 2) ? min_size : 0.0
				};
				Vec2 rect_distance = (this->GetWidth() >= this->GetHeight()) ?
					Vec2{ rect_radius.x * this->GetWidth() / this->GetHeight(), rect_radius.y } :
					Vec2{ rect_radius.x, rect_radius.y * this->GetHeight() / this->GetWidth() };
				if (draw_lines) gfx->DrawLine(this->GetCenter(), this->GetCenter() + rect_distance, { 150, 150, 0 }, SpriteEffect::Copy());
				double root_size = 1.0 / sqrt(pow((sin(angle) / other.GetHeight()), 2) + pow((cos(angle) / other.GetWidth()), 2));
				Vec2 ellipse_distance{ -(root_size / 2) * cos(angle), -(root_size / 2) * sin(angle) };
				if (draw_lines) gfx->DrawLine(other.GetCenter(), other.GetCenter() + ellipse_distance, { 150, 150, 0 }, SpriteEffect::Copy());
				//return rect_distance.GetLength() + ellipse_distance.GetLength() <= centers_distance.GetLength();*/
				if (other.IsTouch({ this->left, this->top }, draw_lines) ||
					other.IsTouch({ this->left, this->bottom }, draw_lines) ||
					other.IsTouch({ this->right, this->top }, draw_lines) ||
					other.IsTouch({ this->right, this->bottom }, draw_lines) ||
					IsTouch({ other.GetCenter().x, other.top }, draw_lines) ||
					IsTouch({ other.GetCenter().x, other.bottom }, draw_lines) ||
					IsTouch({ other.left, other.GetCenter().y }, draw_lines) ||
					IsTouch({ other.right, other.GetCenter().y }, draw_lines)) return true;
				else return false;
			}
		}
		// this Ellipse - other Rect
		else if (this->type == Type::Ellipse && other.type == Type::Rect) {
			return other.IsTouch(*this, draw_lines);
		}
		// this Ellipse - other Ellipse
		else if (this->type == Type::Ellipse && other.type == Type::Ellipse) {
			if (Hitbox_(*this, Type::Rect).IsTouch(Hitbox_(other, Type::Rect), draw_lines)) {
				Vec2 centers_distance = other.GetCenter() - this->GetCenter();
				double angle = Vec2::GetAngle(this->GetCenter(), other.GetCenter());
				Vec2 this_distance = { (this->GetWidth() / 2.0) * cos(angle), (this->GetHeight() / 2.0) * sin(angle) };
				Vec2 other_distance = { -(other.GetWidth() / 2.0) * cos(angle), -(other.GetHeight() / 2.0) * sin(angle) };
				if (draw_lines) gfx->DrawLine(other.GetCenter(), this->GetCenter(), { 0, 150, 150 }, SpriteEffect::Copy());
				if (draw_lines) gfx->DrawLine(this->GetCenter(), this->GetCenter() + this_distance, { 0, 150, 150 }, SpriteEffect::Copy());
				if (draw_lines) gfx->DrawLine(other.GetCenter(), other.GetCenter() + other_distance, { 0, 150, 150 }, SpriteEffect::Copy());
				return this_distance.GetLength() + other_distance.GetLength() >= centers_distance.GetLength();
			}
		}
		return false;
	}
	Hitbox_ FromCenter(const Vec2_<T> &center, T half_width, T half_height) {
		const Vec2_<T> half(half_width, half_height);
		return Hitbox_(center - half, center + half, type, gfx);
	}
	// Gets
	Hitbox_ GetExpanded(T offset) const {
		return Hitbox_(left - offset, right + offset, top - offset, bottom + offset, type, gfx);
	}
	Vec2_<T> GetCenter() const {
		return Vec2_<T>((left + right) / (T)2, (top + bottom) / (T)2);
	}
	// Get position
	T GetX() const { return left; }
	T GetY() const { return top; }
	Vec2_<T> GetPos() const { return Vec2_<T>{ left, top }; }
	// Get size
	T GetWidth() const { return right - left; }
	T GetHeight() const { return bottom - top; }
	Vec2_<T> GetSize() const { return { GetWidth(), GetHeight() }; }
	// Get type
	Type GetType() const {
		return type;
	}
	// Sets
	// Set position
	void SetX(T value) {
		this->right = value + GetWidth();
		this->left = value;
	}
	void AddX(T value) {
		this->right += value;
		this->left += value;
	}
	void SetY(T value) {
		this->bottom = value + GetHeight();
		this->top = value;
	}
	void AddY(T value) {
		this->bottom += value;
		this->top += value;
	}
	void SetPos(Vec2_<T> pos) {
		this->right = pos.x + GetWidth();
		this->left = pos.x;
		this->bottom = pos.y + GetHeight();
		this->top = pos.y;
	}
	void AddPos(Vec2_<T> pos) {
		this->right += pos.x;
		this->left += pos.x;
		this->bottom += pos.y;
		this->top += pos.y;
	}
	// Set size
	void SetWidth(T width) {
		if (width > 0) {
			this->left = GetCenter().x - (width / (T)2);
			this->right = left + width;
		}
		else {
			this->left = GetCenter().x;
			this->right = left;
		}
	}
	void AddWidth(T width) {
		T local_width = GetWidth() + width;
		if (local_width > 0) {
			this->left = GetCenter().x - (local_width / (T)2);
			this->right = left + local_width;
		}
		else {
			this->left = GetCenter().x;
			this->right = left;
		}
	}
	void SetHeight(T height) {
		if (height > 0) {
			this->top = GetCenter().y - (height / (T)2);
			this->bottom = top + height;
		}
		else {
			this->top = GetCenter().y;
			this->bottom = top;
		}
	}
	void AddHeight(T height) {
		T local_height = GetHeight() + height;
		if (local_height > 0) {
			this->top = GetCenter().y - (local_height / (T)2);
			this->bottom = top + local_height;
		}
		else {
			this->top = GetCenter().y;
			this->bottom = top;
		}
	}
	void SetSize(Vec2_<T> size) {
		SetWidth(size.x);
		SetHeight(size.y);
	}
	void AddSize(Vec2_<T> size) {
		AddWidth(size.x);
		AddHeight(size.y);
	}
	// Info
	void ToString(std::string &object_text) {
		// Start
		object_text += "{";
		// T left
		object_text += std::to_string(left); object_text += " ";
		// T right
		object_text += std::to_string(right); object_text += " ";
		// T top
		object_text += std::to_string(top); object_text += " ";
		// T bottom
		object_text += std::to_string(bottom); object_text += " ";
		// Hitbox_::Type type
		object_text += std::to_string(type); object_text += " ";
		// End
		object_text += "}"; object_text += " ";
	}
	void FromString(std::string &object_text) {
		// Start
		object_text.erase(0, object_text.find('{') + 1);
		// T left
		if (typeid(int) == typeid(T)) left = (T)atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
		else if (typeid(double) == typeid(T)) left = (T)atof(object_text.substr(size_t(0), object_text.find(' ')).c_str());
		object_text.erase(0, object_text.find(' ') + 1);
		// T right
		if (typeid(int) == typeid(T)) right = (T)atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
		else if (typeid(double) == typeid(T)) right = (T)atof(object_text.substr(size_t(0), object_text.find(' ')).c_str());
		object_text.erase(0, object_text.find(' ') + 1);
		// T top
		if (typeid(int) == typeid(T)) top = (T)atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
		else if (typeid(double) == typeid(T)) top = (T)atof(object_text.substr(size_t(0), object_text.find(' ')).c_str());
		object_text.erase(0, object_text.find(' ') + 1);
		// T bottom
		if (typeid(int) == typeid(T)) bottom = (T)atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
		else if (typeid(double) == typeid(T)) bottom = (T)atof(object_text.substr(size_t(0), object_text.find(' ')).c_str());
		object_text.erase(0, object_text.find(' ') + 1);
		// Hitbox_::Type type
		type = (Type)atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
		object_text.erase(0, object_text.find(' ') + 1);
		// End
		object_text.erase(0, object_text.find('}') + 1);
		object_text.erase(0, object_text.find(' ') + 1);
	}
public:
	Graphics *gfx;
	T left;
	T right;
	T top;
	T bottom;
	Type type;
};
template<typename T>
class Hitbox_Cluster {
public:
	Hitbox_Cluster(Graphics *gfx = nullptr): cluster_hitbox((T)0, (T)0, (T)0, (T)0, Hitbox_<T>::Type::Rect, gfx) {}
	~Hitbox_Cluster() {
		for (Hitbox_<T> *hitbox : hitboxes) if (hitbox != nullptr) { delete hitbox; hitbox = nullptr; }
	}
	// Add hitbox
	void Add(Hitbox_<T> *hitbox) {
		hitboxes.push_back(hitbox);
		if (hitboxes.size() == 1) { cluster_hitbox = Hitbox_<T>(*hitbox, Hitbox_<T>::Type::Rect); return; }
		for (int i = 0; i < 4; i++) {
			T &this_direction = (i == 0) ? cluster_hitbox.left : (i == 1) ? cluster_hitbox.right :
				(i == 2) ? cluster_hitbox.top : cluster_hitbox.bottom;
			T &new_direction = (i == 0) ? hitbox->left : (i == 1) ? hitbox->right :
				(i == 2) ? hitbox->top : hitbox->bottom;
			if ((i == 0 || i == 2) ? (new_direction < this_direction) :
			(new_direction > this_direction)) this_direction = new_direction;
		}
	}
	// Clear all hitboxes
	void Clear() {
		for (Hitbox_<T> *hitbox : hitboxes) if (hitbox != nullptr) { delete hitbox; hitbox = nullptr; }
		hitboxes.clear();
		cluster_hitbox = { (T)0, (T)0, (T)0, (T)0 };
	}
	// Delete specific hitbox
	void Delete(size_t i) {
		if (hitboxes.size() > i) if (hitbox[i] != nullptr) { delete hitbox[i]; hitboxes.erase(i); }
	}
	// Get specific hitbox
	Hitbox_<T>* operator[](int i) const { return hitboxes[i]; }
	// Get hitboxes' count
	size_t Size() const { return hitboxes.size(); }
	// Is
	bool IsContain(const Vec2_<T> &point, bool draw_lines = false) const {
		if (cluster_hitbox.IsContain(point)) {
			bool is_contain = false;
			for (Hitbox_<T> *hitbox : hitboxes) if (hitbox->IsContain(point, draw_lines)) is_contain = true;
			return is_contain;
		}
		return false;
	}
	bool IsContain(const Hitbox_<T> &other, bool draw_lines = false) const {
		if (cluster_hitbox.IsContain(other)) {
			bool is_contain = false;
			for (Hitbox_<T> *hitbox : hitboxes) if (hitbox->IsContain(other, draw_lines)) is_contain = true;
			return is_contain;
		}
		return false;
	}
	bool IsContain(const Hitbox_Cluster &other, bool draw_lines = false) const {
		if (cluster_hitbox.IsContain(other.cluster_hitbox, draw_lines)) {
			bool is_contain = (other.hitboxes.size() > 0 && hitboxes.size() > 0) ? true : false;
			for (Hitbox_<T> *other_hitbox : other.hitboxes) {
				if (cluster_hitbox.IsContain(*other_hitbox, draw_lines)) {
					for (Hitbox_<T> *hitbox : hitboxes) {
						if (!hitbox->IsContain(*other_hitbox, draw_lines)) is_contain = false;
					}
				}
			}
			return is_contain;
		}
		return false;
	}
	bool IsInside(const Vec2_<T> &point, bool draw_lines = false) const {
		return IsContain(point, draw_lines);
	}
	bool IsInside(const Hitbox_<T> &other, bool draw_lines = false) const {
		//return other.IsContain(*this);
		// Для каждого хитбокса проверка нахождения внутри other
		if (cluster_hitbox.IsInside(other, draw_lines)) {
			bool is_inside = (hitboxes.size() > 0) ? true : false;
			for (Hitbox_<T> *hitbox : hitboxes) if (!hitbox->IsInside(other, draw_lines)) is_inside = false;
			return is_inside;
		}
		return false;
	}
	bool IsInside(const Hitbox_Cluster &other, bool draw_lines = false) const {
		return other.IsContain(*this, draw_lines);
	}
	bool IsTouch(const Vec2_<T> &point, bool draw_lines = false) const {
		if (cluster_hitbox.IsTouch(point, draw_lines)) {
			bool is_touch = false;
			for (Hitbox_<T> *hitbox : hitboxes) if (hitbox->IsTouch(point, draw_lines)) is_touch = true;
			return is_touch;
		}
		return false;
	}
	bool IsTouch(const Hitbox_<T> &other, bool draw_lines = false) const {
		if (cluster_hitbox.IsTouch(other, draw_lines)) {
			bool is_touch = false;
			for (Hitbox_<T> *hitbox : hitboxes) if (hitbox->IsTouch(other, draw_lines)) is_touch = true;
			return is_touch;
		}
		return false;
	}
	bool IsTouch(const Hitbox_Cluster &other, bool draw_lines = false) const {
		if (cluster_hitbox.IsTouch(other.cluster_hitbox, draw_lines)) {
			for (Hitbox_<T> *other_hitbox : other.hitboxes) {
				if (cluster_hitbox.IsTouch(*other_hitbox, draw_lines)) {
					for (Hitbox_<T> *hitbox : hitboxes) {
						if (hitbox->IsTouch(*other_hitbox, draw_lines)) return true;
					}
				}
			}
		}
		return false;
	}
	// Gets
	Hitbox_<T>& GetGeneralHitbox() { return cluster_hitbox; }
	// Get position
	T GetX() const { return cluster_hitbox.left; }
	T GetY() const { return cluster_hitbox.top; }
	Vec2_<T> GetPos() const { return Vec2_<T>{ GetX(), GetY() }; }
	// Get size
	Vec2_<T> GetCenter() const {
		return Vec2_<T>(((T)2 * GetX() + GetWidth()) / (T)2, ((T)2 * GetY() + GetHeight()) / (T)2);
	}
	T GetWidth() const { return cluster_hitbox.right - cluster_hitbox.left; }
	T GetHeight() const { return cluster_hitbox.bottom - cluster_hitbox.top; }
	Vec2_<T> GetSize() const { return { GetWidth(), GetHeight() }; }
	// Set position
	void SetX(T value) {
		cluster_hitbox.SetX(value);
		for (Hitbox_<T> *hitbox : hitboxes) hitbox->SetX(value);
	}
	void AddX(T value) {
		cluster_hitbox.AddX(value);
		for (Hitbox_<T> *hitbox : hitboxes) hitbox->AddX(value);
	}
	void SetY(T value) {
		cluster_hitbox.SetY(value);
		for (Hitbox_<T> *hitbox : hitboxes) hitbox->SetY(value);
	}
	void AddY(T value) {
		cluster_hitbox.AddY(value);
		for (Hitbox_<T> *hitbox : hitboxes) hitbox->AddY(value);
	}
	void SetPos(Vec2_<T> pos) {
		cluster_hitbox.SetPos(pos);
		for (Hitbox_<T> *hitbox : hitboxes) hitbox->SetPos(pos);
	}
	void AddPos(Vec2_<T> pos) {
		cluster_hitbox.AddPos(pos);
		for (Hitbox_<T> *hitbox : hitboxes) hitbox->AddPos(pos);
	}
	// Set size
	void SetWidth(T width) {
		for (Hitbox_<T> *hitbox : hitboxes) hitbox->SetWidth(hitbox->GetWidth() * width / GetWidth());
		cluster_hitbox.SetWidth(width);
	}
	void AddWidth(T width) {
		for (Hitbox_<T> *hitbox : hitboxes) hitbox->AddWidth(hitbox->GetWidth() * width / GetWidth());
		cluster_hitbox.AddWidth(width);
	}
	void SetHeight(T height) {
		for (Hitbox_<T> *hitbox : hitboxes) hitbox->SetHeight(hitbox->GetHeight() * height / GetHeight());
		cluster_hitbox.SetHeight(height);
	}
	void AddHeight(T height) {
		for (Hitbox_<T> *hitbox : hitboxes) hitbox->AddHeight(hitbox->GetHeight() * height / GetHeight());
		cluster_hitbox.AddHeight(height);
	}
	void SetSize(Vec2_<T> size) {
		cluster_hitbox.SetSize(size);
		for (Hitbox_<T> *hitbox : hitboxes) hitbox->SetSize(size);
	}
	void AddSize(Vec2_<T> size) {
		cluster_hitbox.AddSize(size);
		for (Hitbox_<T> *hitbox : hitboxes) hitbox->AddSize(size);
	}
	void ToString(std::string &object_text) const {
		// Start
		object_text += "{";
		// std::vector<Hitbox_<T>*>
		for (Hitbox_<T> *hitbox : hitboxes) hitbox->ToString(object_text);
		// End
		object_text += "}"; object_text += " ";
	}
	void FromString(std::string &object_text) {
		// Start
		object_text.erase(0, object_text.find('{') + 1);
		// std::vector<Hitbox_<T>*>
		for (Hitbox_<T> *hitbox : hitboxes) hitbox->FromString(object_text);
		// End
		object_text.erase(0, object_text.find('}') + 1);
		object_text.erase(0, object_text.find(' ') + 1);
	}
private:
	Hitbox_<T> cluster_hitbox;
	std::vector<Hitbox_<T>*> hitboxes;
};
typedef Hitbox_<int> HitboxI;
typedef Hitbox_<double> HitboxD;
typedef Hitbox_Cluster<int> HitboxICluster;
typedef Hitbox_Cluster<double> HitboxDCluster;