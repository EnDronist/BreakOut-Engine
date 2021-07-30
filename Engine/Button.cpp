#include "Button.h"
Button::Button(Graphics &gfx, Mouse &mouse, Vec2 pos, Vec2 size, double border):
	gfx(gfx),
	mouse(mouse),
	pos(pos, pos + size, HitboxD::Type::Rect),
	border(border),
	chroma(chroma),
	surface((Vei2)size),
	mouse_focus(false),
	label(nullptr)
{}
Button::~Button() {
	DeleteLabel();
}
void Button::CreateLabel(const std::string &text, Color &color, double size_miltiplier, Font &font){
	if (label != nullptr) { delete label; label = nullptr; }
	label = new Label(gfx, text, Vec2(0, 0), color, size_miltiplier, font);
	Vec2 label_pos = { pos.GetCenter().x - (label->GetWidth() / 2.0),
		pos.GetCenter().y - (label->GetHeight() / 2.0) };
	label->SetPos(label_pos);
}
void Button::DeleteLabel(){
	if (label != nullptr) { delete label; label = nullptr; }
}
void Button::UpdateLabel(const std::string &text) {
	if (label != nullptr) {
		label->Update(text);
		Vec2 label_pos = { pos.GetCenter().x - (label->GetWidth() / 2.0),
			pos.GetCenter().y - (label->GetHeight() / 2.0) };
		label->SetPos(label_pos);
	}
}
void Button::Update() {
	if (pos.IsContain(mouse.GetPos())) {
		if (!mouse_focus) {
			mouse_focus = true;
			if (border != 0.0) border *= 2.0;
		}
	}
	else if (mouse_focus) {
		mouse_focus = false;
		if (border != 0.0) border /= 2.0;
	}
}
void Button::Redraw() {
	if (border != 0.0) {
		for (int y = 0; y < pos.GetHeight(); y++) {
			for (int x = 0; x < pos.GetWidth(); x++) {
				if (x < border || x >= (pos.GetWidth() - border) || y < border || y >= (pos.GetHeight() - border)) {
					if (surface.GetPixel(x, y) != Color{ 0, 255, 255 }) surface.PutPixel(x, y, Color{ 0, 255, 255 });
				}
				else {
					if (surface.GetPixel(x, y) != Color{ 0, 0, 0 }) surface.PutPixel(x, y, Color{ 0, 0, 0 });
				}
			}
		}
	}
}
bool Button::MouseCollision(Mouse::Event::Type type) {
	if (mouse_focus && mouse.Read().GetType() == type) return true;
	else return false;
}
void Button::Draw() const {
	SpriteEffect::Copy effect;
	gfx.DrawSprite((Vei2)pos.GetPos(), surface, effect);
	if (label != nullptr) label->Draw();
}
double Button::GetWidth() const{
	return pos.GetWidth();
}
double Button::GetHeight() const{
	return pos.GetHeight();
}
Vec2 Button::GetPos() const{
	return pos.GetPos();
}
