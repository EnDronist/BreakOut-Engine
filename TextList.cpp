#include "TextList.h"
TextList::TextList(Mouse &mouse, HitboxD pos):
	mouse(mouse),
	mouse_last(mouse.GetPos()),
	mouse_now(mouse.GetPos()),
	pos(pos) {
}
TextList::~TextList() {}
void TextList::Update() {
	mouse_last = mouse_now;
	mouse_now = mouse.GetPos();
	if (mouse_hold || GetHitbox().IsContain(mouse.GetPos()) && mouse.Read().GetType() == Mouse::Event::LPress) {
		if (!mouse_hold) mouse_hold = true;
		if (mouse.Read().GetType() == Mouse::Event::LRelease) {
			mouse_hold = false;
		}
		else {
			offset += mouse_last - mouse_now;
			
			if (offset.y < 0) offset.y = 0;
			else if (labels.size() > 0) {
				double bottoms_difference = labels_pos.back().bottom - this->GetHitbox().bottom;
				if (bottoms_difference > 0 && offset.y > bottoms_difference) offset.y = bottoms_difference;
			}
			else if (offset.y > 0) offset.y = 0;
		}
	}
}
void TextList::Draw() const {
	for (int i = 0; i < labels.size(); i++) {
		labels[i]->GetHitbox().SetPos(labels_pos[i].GetPos() - Vec2{ 0, offset.y });
		if (labels[i]->GetHitbox().IsTouch(Graphics::GetGameRectD())) labels[i]->Draw(&pos);
	}
}
void TextList::Add(Label *label) {
	if (labels.size() > 0) 
		label->SetPos(pos.GetPos() + Vec2{ 0, labels.back()->GetHitbox().bottom - pos.GetY() + 8.0 });
	else label->SetPos(pos.GetPos());
	labels.push_back(label);
	labels_pos.push_back(label->GetHitbox());
}
void TextList::Delete(size_t i) {
	if (i >= 0 && i < labels.size()) { delete labels[i]; labels.erase(labels.begin() + i); }
}
HitboxD TextList::GetHitbox() const {
	return pos;
}
