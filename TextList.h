#pragma once
#include "GUI.h"
class TextList {
public:
	TextList(Mouse &mouse, HitboxD pos);
	~TextList();
	void Update();
	void Draw() const;
	void Add(Label *label);
	void Delete(size_t i);
	HitboxD GetHitbox() const;
private:
	Mouse &mouse; Vec2 mouse_last, mouse_now; bool mouse_hold = false;
	HitboxD pos;
	Vec2 offset = {0, 0};
	std::vector<Label*> labels;
	std::vector<HitboxD> labels_pos;
};