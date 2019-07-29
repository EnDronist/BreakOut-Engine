#pragma once
#include "GUI.h"
class Button {
public:
	Button(Graphics &gfx, Mouse &mouse, Vec2 pos, Vec2 size, double border = 0);
	~Button();
	void Update();
	void Redraw();
	void Draw() const;
	void CreateLabel(const std::string &text, Color &color, double size_miltiplier, Font &font);
	void DeleteLabel();
	void ChangeText(const std::string &text);
	bool MouseCollision(Mouse::Event::Type type);
	bool IsMouseFocus() const;
	Label* GetLabel();
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