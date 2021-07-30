#include "Brick.h"
Brick::Brick(Graphics &gfx, Vec2 pos, Vec2 size_multiplier, Ball &ball, Type type, bool multiplayer, Sound *ball_kick):
	gfx(gfx),
	surface("brick.bmp"),
	pos(&gfx),
	size_multiplier(size_multiplier),
	ball(ball),
	type(type),
	used(false),
	counted(false),
	physical(!multiplayer),
	ball_kick_sound(ball_kick) {
	this->pos.Add(new HitboxD(pos, StartWidth * size_multiplier.x, StartHeight * size_multiplier.y, HitboxD::Type::Rect, &gfx));
}
Brick::~Brick() {}
void Brick::Update() {
	if (pos.IsTouch(ball.pos)) {
		if (physical && !ball.IsTouched()) {
			ball.Touch();
			double ratio = ((pos.GetCenter().x - ball.pos.GetCenter().x) / ((StartWidth / 2.0) + (ball.StartWidth / 2.0))) /
				((pos.GetCenter().y - ball.pos.GetCenter().y) / ((StartHeight / 2.0) + (ball.StartHeight / 2.0)));
			if (abs(ratio) >= 1.0) ball.speed_h *= -1;
			else ball.speed_v *= -1;
		}
		used = true;
	}
}
void Brick::Draw() const {
	if (type == Type::Common) gfx.DrawSprite((Vei2)pos.GetPos(), surface, SpriteEffect::Chroma{ Color{ 0, 0, 0 } }, pos.GetSize() / (Vec2)surface.GetSize());
	else if (type == Type::Expand) gfx.DrawSprite((Vei2)pos.GetPos(), surface, SpriteEffect::Substitution{ Color{ 0, 0, 0 }, Colors::Blue }, pos.GetSize() / (Vec2)surface.GetSize());
}
void Brick::Effect() {
	if (ball_kick_sound != nullptr) ball_kick_sound->Play();
	switch (type) {
	case Type::Common: {
		break;
	}
	case Type::Expand: {
		Buff *buff = new BuffExpand({ *ball.last, 5.0 });
		if (ball.last != nullptr) {
			if (ball.last->HasBuff("BuffExpand")) {
				ball.last->UpdateBuff(buff); delete buff;
			}
			else ball.last->AddBuff(buff);
		}
		break;
	}
	case Type::Divide: {
		break;
	}
	case Type::Laser: {
		break;
	}
	case Type::Slow: {
		break;
	}
	case Type::Break: {
		break;
	}
	case Type::Catch: {
		break;
	}
	case Type::Player: {
		break;
	}
	}
}
bool Brick::IsUsed() const{ return used; }
bool Brick::IsCounted() const { return counted; }
void Brick::SetCounted(bool counted) { this->counted = counted; }
HitboxDCluster& Brick::GetHitbox() { return pos; }
Vec2 Brick::GetSize() const { return pos.GetSize(); }
void Brick::SetSize(Vec2 size) { this->pos.SetSize(size); }
void Brick::ToString(std::string &object_text, InfoType type){
	if (type == InfoType::Init) {
	// HitboxD pos
	pos.ToString(object_text);
	// Brick::Type type
	object_text += std::to_string(this->type); object_text += " ";
	}
	else if (type == InfoType::Process) {
		// bool used
		object_text += std::to_string(used); object_text += " ";
	}
}
void Brick::FromString(std::string &object_text, InfoType type){
	if (type == InfoType::Init) {
		// HitboxD pos
		pos.FromString(object_text);
		// Brick::Type type
		this->type = (Type)atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
		object_text.erase(0, object_text.find(' ') + 1);
	}
	else if (type == InfoType::Process) {
		// bool used
		this->used = atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
		object_text.erase(0, object_text.find(' ') + 1);
	}
}
