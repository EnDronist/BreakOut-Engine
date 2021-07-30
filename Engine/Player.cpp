#include "Player.h"
Player::Player(Graphics &gfx, Vec2 pos, PlayerSpeedBoost &boost, bool host, Keyboard &kbd, const std::string surface):
	gfx(gfx),
	kbd(kbd),
	pos(&gfx),
	boost(boost),
	host(host),
	acceleration(0.3 / 1000.0),// +0.3 pixels/second^2
	speed_reduction_multiplier(pow(0.60, 1.0 / 1000.0)),// -40% speed per second
	speed_h(0.0),
	speed_v(0.0),
	speed_impact_h(0.0),
	speed_impact_v(0.0),
	speed_impact_h_now(0),
	speed_impact_v_now(0),
	key_shift_acceleration_multiplier(4.0),
	key_control_acceleration_multiplier(0.25),
	surface(surface) {
	this->pos.Add(new HitboxD{ pos, StartHeight, StartHeight, HitboxD::Type::Ellipse, &gfx });
	this->pos.Add(new HitboxD{ pos + Vec2(StartHeight / 2.0, 0.0), StartWidth - StartHeight, StartHeight, HitboxD::Type::Rect, &gfx });
	this->pos.Add(new HitboxD{ pos + Vec2(StartWidth - StartHeight, 0.0), StartHeight, StartHeight, HitboxD::Type::Ellipse, &gfx });
}
Player::~Player(){}
void Player::Update() {
	//Проверка нажатия клавиш
	double acceleration_addition = acceleration;
	if (host) {
		// Move
		if (kbd.KeyIsPressed(VK_CONTROL)) {
			acceleration_addition *= key_control_acceleration_multiplier;
		}
		if (kbd.KeyIsPressed(VK_SHIFT)) {
			if (boost.GetValue() > 0) acceleration_addition *= key_shift_acceleration_multiplier;
		}
		/*if ((kbd.KeyIsPressed('D') && !kbd.KeyIsPressed('A')) || (kbd.KeyIsPressed('A') && !kbd.KeyIsPressed('D')))
			if ((kbd.KeyIsPressed('W') && !kbd.KeyIsPressed('S')) || (kbd.KeyIsPressed('S') && !kbd.KeyIsPressed('W')))
				acceleration_addition *= (sqrt(2) / 2);*/
		bool left_pressed = kbd.KeyIsPressed('A') || kbd.KeyIsPressed(VK_LEFT);
		if (left_pressed) {
			speed_h -= acceleration_addition;
		}
		bool right_pressed = kbd.KeyIsPressed('D') || kbd.KeyIsPressed(VK_RIGHT);
		if (right_pressed) {
			speed_h += acceleration_addition;
		}
		// Boost
		if ((left_pressed || right_pressed) && kbd.KeyIsPressed(VK_SHIFT)) boost.Enable(true);
		else boost.Enable(false);
	}
	//Постоянное уменьшение скорости
	double speed_reduction_h, speed_reduction_v;
	if (abs(speed_v) > 0) speed_reduction_h = abs(speed_h) * (1 - speed_reduction_multiplier) * (abs(speed_h) / (abs(speed_h) + abs(speed_v)));
	else speed_reduction_h = abs(speed_h) * (1 - speed_reduction_multiplier);
	if (abs(speed_h) > 0) speed_reduction_v = abs(speed_v) * (1 - speed_reduction_multiplier) * (abs(speed_v) / (abs(speed_h) + abs(speed_v)));
	else speed_reduction_v = abs(speed_v) * (1 - speed_reduction_multiplier);
	
	if (speed_h > speed_reduction_h) speed_h -= speed_reduction_h;
	else if (speed_h < speed_reduction_h) speed_h += speed_reduction_h;
	else speed_h = 0;
	if (speed_v > speed_reduction_v) speed_v -= speed_reduction_v;
	else if (speed_v < speed_reduction_v) speed_v += speed_reduction_v;
	else speed_v = 0;
	// Подсчёт влияния скорости на передвижение
	if (speed_h != 0.0 || speed_v != 0.0) pos.AddPos({ speed_h, speed_v });
	/*
	if (speed_h != 0) {
		speed_impact_h += speed_h;
		if (abs(speed_impact_h) >= 1) {
			double speed_impact_h_floor = 0;
			if (speed_impact_h >= 0) speed_impact_h_floor = floor(speed_impact_h);
			else speed_impact_h_floor = ceil(speed_impact_h);
			speed_impact_h -= speed_impact_h_floor;
			x += static_cast<int>(speed_impact_h_floor);
		}
	}
	if (speed_v != 0) {
		speed_impact_v += speed_v;
		if (abs(speed_impact_v) >= 1) {
			double speed_impact_v_floor = 0;
			if (speed_impact_v >= 0) speed_impact_v_floor = floor(speed_impact_v);
			else speed_impact_v_floor = ceil(speed_impact_v);
			speed_impact_v -= speed_impact_v_floor;
			y += static_cast<int>(speed_impact_v_floor);
		}
	}
	*/
	//Проверка координат игрока на границы окна
	if (pos.GetX() < 0.0) {
		pos.SetX(0.0);
		if (speed_h != 0.0) speed_h *= -1;
	}
	else if (pos.GetX() > (Settings::GameScreenWidth - pos.GetWidth())) {
		pos.SetX(Settings::GameScreenWidth - pos.GetWidth());
		if (speed_h != 0.0) speed_h *= -1;
	}
	else if (pos.GetY() < 0.0) {
		pos.SetY(0.0);
		if (speed_v != 0.0) speed_v *= -1;
	}
	else if (pos.GetY() > (Settings::GameScreenHeight - pos.GetHeight())) {
		pos.SetX(Settings::GameScreenHeight - pos.GetHeight());
		if (speed_v != 0.0) speed_v *= -1;
	}
	//Обновление баффов
	for (int i = 0; i < buffs.size(); i++) {
		buffs[i]->Update();
		if (buffs[i]->GetTime() == 0.0) {
			delete buffs[i];
			buffs.erase(buffs.begin() + i);
			i--;
		}
	}
}
void Player::Draw() {
	if (HasBuff("BuffExpand")) {
		HitboxI surface_rect = surface.GetRect();
		HitboxI surface_rect_left = { 0, surface_rect.GetWidth() / 4, 0, surface_rect.GetHeight(), HitboxI::Type::Rect };
		int surface_rect_center_left = surface_rect.GetCenter().x - (int)floor((pos.GetWidth() - (double)StartWidth) / 2.0);
		int surface_rect_center_right = surface_rect.GetCenter().x + 1 + (int)ceil((pos.GetWidth() - (double)StartWidth) / 2.0);
		HitboxI surface_rect_center = { surface_rect_center_left, surface_rect_center_right, 0, surface_rect.GetHeight(), HitboxI::Type::Rect };
		HitboxI surface_rect_right = { surface_rect.GetWidth() - surface_rect.GetWidth() / 4, surface_rect.GetWidth(), 0, surface_rect.GetHeight(), HitboxI::Type::Rect };
		gfx.DrawSprite((Vei2)pos.GetPos(), surface_rect_left, surface, SpriteEffect::Chroma{ Color{ 255, 255, 255 } });
		gfx.DrawSprite({ (int)pos.GetPos().x + surface_rect.GetWidth() / 4, (int)pos.GetPos().y }, surface_rect_center, surface, SpriteEffect::Chroma{ Color{ 255, 255, 255 } });
		gfx.DrawSprite({ (int)(pos.GetPos().x + pos.GetSize().x - StartWidth / 2.0), (int)pos.GetPos().y }, surface_rect_right, surface, SpriteEffect::Chroma{ Color{ 255, 255, 255 } });
	}
	else {
		HitboxI surface_rect = surface.GetRect();
		HitboxI surface_rect_left = { 0, surface_rect.GetWidth() / 4, 0, surface_rect.GetHeight(), HitboxI::Type::Rect };
		HitboxI surface_rect_right = { surface_rect.GetWidth() - surface_rect.GetWidth() / 4, surface_rect.GetWidth(), 0, surface_rect.GetHeight(), HitboxI::Type::Rect };
		gfx.DrawSprite((Vei2)pos.GetPos(), surface_rect_left, surface, SpriteEffect::Chroma{ Color{ 255, 255, 255 } });
		gfx.DrawSprite({ (int)(pos.GetCenter().x), (int)pos.GetY() }, surface_rect_right, surface, SpriteEffect::Chroma{ Color{ 255, 255, 255 } });
	}
	for (int i = 0; i < buffs.size(); i++) {
		if (host) {
			//Vei2 sprite_pos = { (int)(pos.GetCenter().x - (((16 + 4) / 2) * (buffs.size() - 1))) + ((16 + 4) * i), (int)pos.bottom + 10 };
			//gfx.DrawSprite(sprite_pos.x, sprite_pos.y, Surface(16, 16), SpriteEffect::Chess{ Color{ 252, 15, 192 } });
		}
		else {
			//Vei2 sprite_pos = { (int)(pos.GetCenter().x - (((16 + 4) / 2) * (buffs.size() - 1))) + ((16 + 4) * i), (int)pos.top - 10 };
			//gfx.DrawSprite(sprite_pos.x, sprite_pos.y, Surface(16, 16), SpriteEffect::Chess{ Color{ 252, 15, 192 } });
		}
	}
	if (Settings::DrawCollisionRects()) {
		//gfx.DrawSprite((Vei2)pos.GetPos(), Surface((int)pos.GetWidth(), (int)pos.GetHeight()), SpriteEffect::Chess{ Color{ 252, 15, 192 } });
		gfx.DrawSprite((Vei2)pos[0]->GetPos(), Surface((int)pos[0]->GetWidth(), (int)pos[0]->GetHeight()), SpriteEffect::Chess{ Color{ 252, 15, 192 } });
		//gfx.DrawSprite((Vei2)pos[1]->GetPos(), Surface((int)pos[1]->GetWidth(), (int)pos[1]->GetHeight()), SpriteEffect::Chess{ Color{ 252, 115, 192 } });
		gfx.DrawSprite((Vei2)pos[2]->GetPos(), Surface((int)pos[2]->GetWidth(), (int)pos[2]->GetHeight()), SpriteEffect::Chess{ Color{ 252, 15, 192 } });
	}
}
HitboxDCluster& Player::GetHitbox() {
	return pos;
}
double Player::GetSpeedH() const {
	return speed_h;
}
double Player::GetSpeedV() const {
	return speed_v;
}
bool Player::IsHost() const {
	return host;
}
Player &Player::operator=(const Player &player) {
	this->pos = player.pos;
	this->acceleration = player.acceleration;
	this->speed_reduction_multiplier = player.speed_reduction_multiplier;
	this->speed_h = player.speed_h;
	this->speed_v = player.speed_v;
	this->speed_impact_h = player.speed_impact_h;
	this->speed_impact_v = player.speed_impact_v;
	this->speed_impact_h_now = player.speed_impact_h_now;
	this->speed_impact_v_now = player.speed_impact_v_now;
	this->key_shift_acceleration_multiplier = player.key_shift_acceleration_multiplier;
	this->key_control_acceleration_multiplier = player.key_control_acceleration_multiplier;
	return *this;
}
void Player::AddBuff(Buff *buff) {
	buffs.push_back(buff);
}
bool Player::RemoveBuff(Buff *buff) {
	for (int i = 0; i < buffs.size(); i++) {
		if (typeid(*(buffs[i])) == typeid(*buff)) {
			buffs.erase(buffs.begin() + i);
			return true;// Buff removed
		}
	}
	return false;// Buff not exists
}
void Player::UpdateBuff(Buff *buff_info) {
	for (int i = 0; i < buffs.size(); i++) {
		if (typeid(*(buffs[i])) == typeid(*buff_info)) {
			*buffs[i] = *buff_info;
			return;
		}
	}
}
int Player::HasBuff(std::string buff_name) const {
	int buff_count = 0;
	buff_name = std::string("class ") + buff_name;
	for (int i = 0; i < buffs.size(); i++) {
		std::string text = typeid(*buffs[i]).name();
		if (typeid(*buffs[i]).name() == buff_name) buff_count++;
	}
	return buff_count;
}
void Player::ToString(std::string &object_text, InfoType type) {
	if (type == InfoType::Init) {
		// Surface surface
		surface.ToString(object_text);
	}
	else if (type == InfoType::Process) {
		// HitboxDCluster pos
		pos.ToString(object_text);
		// double speed_h
		object_text += std::to_string(speed_h); object_text += " ";
		// double speed_v
		object_text += std::to_string(speed_v); object_text += " ";
		// std::vector<Buff*> buffs
		object_text += std::to_string(buffs.size());
		object_text += "{";
		for (auto buff : buffs) buff->ToString(object_text);
		object_text += "}";
	}
}
void Player::FromString(std::string &object_text, InfoType type) {
	if (type == InfoType::Init) {
		// Surface surface
		surface.FromString(object_text);
	}
	else if (type == InfoType::Process) {
		// HitboxDCluster pos
		pos.FromString(object_text);
		// double speed_h
		this->speed_h = atof(object_text.substr(size_t(0), object_text.find(' ')).c_str());
		object_text.erase(0, object_text.find(' ') + 1);
		// double speed_v
		this->speed_v = atof(object_text.substr(size_t(0), object_text.find(' ')).c_str());
		object_text.erase(0, object_text.find(' ') + 1);
		// std::vector<Buff*> buffs
		int vector_size = atoi(object_text.substr(size_t(0), object_text.find('{')).c_str());
		object_text.erase(0, object_text.find('{') + 1);
		for (int i = 0; i < vector_size; i++) {
			Buff* new_buff = Buff::FromString(object_text, *this);
			if (buffs.size() == i) buffs.push_back(new_buff);
			else {
				if (buffs[i]->GetType() == new_buff->GetType()) { *buffs[i] = *new_buff; delete new_buff; }
				else { delete buffs[i]; buffs[i] = new_buff; }
			}
		}
		while (vector_size < buffs.size()) buffs.pop_back();
		object_text.erase(0, object_text.find('}') + 1);
	}
}