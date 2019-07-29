#pragma once
#include "GUI.h"
class Label {
public:
	Label(Graphics &gfx, const std::string &text, Vec2 pos, Color color, double size_miltiplier, Font &font);
	~Label();
	void Draw(const HitboxD *clip = nullptr) const;
	void SetText(const std::string &text);
	void SetColor(const Color &color);
	void SetPos(const Vec2 &pos);
	HitboxD& GetHitbox();
	double GetWidth() const;
	double GetHeight() const;
	void ToString(std::string &object_text);
	void FromString(std::string &object_text);
private:
	Graphics &gfx;
	Font &font;
	std::string text;
	HitboxD pos;
	Color color;
	double size_miltiplier;
};