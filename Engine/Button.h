#pragma once
#include "GUI.h"
class Button {
public:
	Button(Graphics &gfx, Mouse &mouse, Vec2 pos, Vec2 size, double border = 0);
	~Button();
	void CreateLabel(const std::string &text, Color &color, double size_miltiplier, Font &font);
	void DeleteLabel();
	void UpdateLabel(const std::string &text);
	void Update();
	void Redraw();
	bool MouseCollision(Mouse::Event::Type type);
	void Draw() const;
	double GetWidth() const;
	double GetHeight() const;
	Vec2 GetPos() const;
private:
	Graphics &gfx;
	Mouse &mouse;
	HitboxD pos;
	double border;
	Surface surface;
	Color chroma;
	bool mouse_focus;
	// Label
	Label *label;
};