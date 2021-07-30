#include "Label.h"
Label::Label(Graphics &gfx, const std::string &text, Vec2 pos, Color color, double size_miltiplier, Font &font):
	gfx(gfx),
	font(font),
	text(text),
	pos(pos, GetWidth(), GetHeight(), HitboxD::Type::Rect),
	color(color),
	size_miltiplier(size_miltiplier) {
}
Label::~Label() {}
void Label::Draw(const HitboxD *clip) const {
	font.DrawTxt(text, pos.GetPos(), color, size_miltiplier, clip);
}
void Label::Update(const std::string &text) {
	this->text = text;
	pos.right = pos.left + GetWidth();
	pos.bottom = pos.top + GetHeight();
}
void Label::SetPos(Vec2 &pos) {
	this->pos = { pos, GetWidth(), GetHeight(), HitboxD::Type::Rect };
}
HitboxD& Label::GetHitbox() {
	return pos;
}
double Label::GetWidth() const {
	int text_width = 0, current_text_width = 0;
	for (auto c : text) {
		if (c == '\n') { current_text_width = 0; }
		else current_text_width++;
		if (current_text_width > text_width) text_width = current_text_width;
	}
	return (double)text_width * size_miltiplier * font.GetGlyphWidth();
}
double Label::GetHeight() const {
	int text_height = 1;
	for (auto c : text) if (c == '\n') text_height++;
	return (double)text_height * size_miltiplier * font.GetGlyphHeight();
}
void Label::ToString(std::string &object_text) {
	// string text
	object_text += std::to_string(text.size()) + "{" + text + "}";
	// HitboxD pos
	pos.ToString(object_text);
}
void Label::FromString(std::string &object_text) {
	int text_size;
	// string text
	text_size = atoi(object_text.substr(size_t(0), object_text.find('{')).c_str());
	object_text.erase(0, object_text.find('{') + 1);
	this->text = object_text.substr(size_t(0), size_t(text_size));
	Update(text);
	object_text.erase(0, object_text.find('}') + 1);
	// RectD pos
	pos.FromString(object_text);
}