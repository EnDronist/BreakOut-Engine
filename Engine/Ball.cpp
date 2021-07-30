#include "Ball.h"
Ball::Ball(Graphics &gfx, std::vector<Player*> players, Keyboard *kbd_second, Player *attach,
	Vec2 pos, Vec2 size_multiplier, double start_h_speed, double start_v_speed, const std::string &filename):
	gfx(gfx),
	surface(filename),
	pos(&gfx),
	size_multiplier(size_multiplier),
	speed_h(start_h_speed),
	speed_v(start_v_speed),
	players(players),
	last(attach),
	kbd_second(kbd_second),
	attach(attach) {
	this->pos.Add(new HitboxD(pos, StartWidth * size_multiplier.x, StartHeight * size_multiplier.y, HitboxD::Type::Ellipse, &gfx));
}
Ball::~Ball() {}
bool Ball::OutWindow() {
	bool out = false;
	if (pos.GetX() < 0.0) {
		pos.SetX(0.0);
		if (speed_h != 0.0) speed_h *= -1;
		out = true;
	}
	else if (pos.GetX() > (Settings::GameScreenWidth - StartWidth)) {
		pos.SetX(Settings::GameScreenWidth - StartWidth);
		if (speed_h != 0.0) speed_h *= -1;
		out = true;
	}
	else if (pos.GetY() < 0.0) {
		pos.SetY(0.0);
		if (speed_v != 0.0) speed_v *= -1;
		out = true;
		failed_up = true;
	}
	else if (pos.GetY() > (Settings::GameScreenHeight - StartHeight)) {
		pos.SetX(Settings::GameScreenHeight - StartHeight);
		if (speed_v != 0.0) speed_v *= -1;
		out = true;
		failed_down = true;
	}
	return out;
}
Player* Ball::PlayerCollision() {
	for (int i = 0; i < players.size(); i++) {
		if (pos.IsTouch(players[i]->pos)) {
			Vec2 ball_center = pos.GetCenter(),
				pl_center = players[i]->pos.GetCenter();
			double speed_impetus_h, speed_impetus_v;
			if (abs(pl_center.x - ball_center.x) == 0) speed_impetus_h = speed_h;
			else speed_impetus_h = sqrt(pow(speed_h, 2) + pow(speed_v, 2)) * abs(pl_center.x - ball_center.x) / sqrt(pow(abs(pl_center.x - ball_center.x), 2) + pow(abs(pl_center.y - ball_center.y), 2));
			if (abs(pl_center.y - ball_center.y) == 0) speed_impetus_v = speed_v;
			else speed_impetus_v = sqrt(pow(speed_h, 2) + pow(speed_v, 2)) * abs(pl_center.y - ball_center.y) / sqrt(pow(abs(pl_center.x - ball_center.x), 2) + pow(abs(pl_center.y - ball_center.y), 2));
			if (abs(speed_h) < 0.1 || abs(speed_v) < 0.1) speed_h = speed_h;
			if (sqrt(pow(speed_impetus_h, 2) + pow(speed_impetus_v, 2)) > sqrt(pow(speed_h, 2) + pow(speed_v, 2))) {
				speed_impetus_h /= sqrt(pow(speed_impetus_h, 2) + pow(speed_impetus_v, 2)) / sqrt(pow(speed_h, 2) + pow(speed_v, 2));
				speed_impetus_v /= sqrt(pow(speed_impetus_h, 2) + pow(speed_impetus_v, 2)) / sqrt(pow(speed_h, 2) + pow(speed_v, 2));
			}
			if ((pl_center.x - ball_center.x) >= 0) speed_h = -speed_impetus_h;
			else speed_h = speed_impetus_h;
			if ((pl_center.y - ball_center.y) >= 0) speed_v = -speed_impetus_v;
			else speed_v = speed_impetus_v;
			last = players[i];
			return players[i];
		}
	}
	return nullptr;
}
void Ball::Update() {
	if (launched) {
		if (touched) touched = false;
		// Speed increace
		speed_h += speed_increace * (speed_h / abs(speed_h + speed_v));
		speed_v += speed_increace * (speed_v / abs(speed_h + speed_v));
		// Подсчёт влияния скорости на передвижение
		if (speed_h != 0.0 || speed_v != 0.0) pos.AddPos({ speed_h, speed_v });
		//Проверка координат шара (игрок)
		PlayerCollision();
		/*
		if (abs(speed_impact_h) >= 1) {
		double speed_impact_h_floor = 0;
		if (speed_impact_h >= 0) speed_impact_h_floor = floor(speed_impact_h);
		else speed_impact_h_floor = ceil(speed_impact_h);
		speed_impact_h_now = static_cast<int>(speed_impact_h_floor);
		speed_impact_h -= speed_impact_h_floor;
		x += speed_impact_h_now;
		}
		speed_impact_v += speed_v;
		if (abs(speed_impact_v) >= 1) {
		double speed_impact_v_floor = 0;
		if (speed_impact_v >= 0) speed_impact_v_floor = floor(speed_impact_v);
		else speed_impact_v_floor = ceil(speed_impact_v);
		speed_impact_v_now = static_cast<int>(speed_impact_v_floor);
		speed_impact_v -= speed_impact_v_floor;
		y += speed_impact_v_now;
		}
		*/
	}
	else {
		if (attach != nullptr) {
			if (pos.GetX() != (attach->GetHitbox().GetCenter().x - (StartWidth / 2)))
				pos.SetX(attach->GetHitbox().GetCenter().x - (StartWidth / 2));
			if (attach == players[0]) {
				if (pos.GetY() != (attach->GetHitbox().GetY() - StartHeight))
					pos.SetY(attach->GetHitbox().GetY() - StartHeight);
				if (attach->kbd.KeyIsPressed(VK_SPACE)) {
					speed_h = attach->GetSpeedH();
					speed_v = attach->GetSpeedV() - start_speed_v;
					last = attach;
					launched = true;
				}
			}
			else if (attach == players[1]) {
				if (pos.GetY() != (attach->GetHitbox().GetY() + StartHeight))
					pos.SetY(attach->GetHitbox().GetY() + StartHeight);
				if (kbd_second->KeyIsPressed(VK_SPACE)) {
					speed_h = attach->GetSpeedH();
					speed_v = attach->GetSpeedV() + start_speed_v;
					last = attach;
					launched = true;
				}
			}
		}
		else launched = true;
	}
	//Проверка координат шара (границы окна)
	OutWindow();
}
void Ball::Draw() const {
	gfx.DrawSprite((Vei2)pos.GetPos(), surface, SpriteEffect::Chroma{ Color{ 255,255,255 } }, pos.GetSize() / (Vec2)surface.GetSize() );
	if (Settings::DrawCollisionRects()) gfx.DrawSprite((Vei2)pos.GetPos(), Surface((int)pos.GetWidth(), (int)pos.GetHeight()), SpriteEffect::Chess{ Color{ 252, 15, 192 } });
}
HitboxDCluster& Ball::GetHitbox() { return pos; }
bool Ball::IsLaunched() const { return launched; }
bool Ball::IsFailedUp() const { return failed_up; }
bool Ball::IsFailedDown() const { return failed_down; }
bool Ball::IsTouched() const { return touched; }
void Ball::Restart(Player *attach) {
	failed_up = false;
	failed_down = false;
	launched = false;
	this->attach = attach;
}
void Ball::Touch() { touched = true; }
void Ball::ToString(std::string & object_text) {
	int pointer_number;
	// RectD pos
	pos.ToString(object_text); object_text += " ";
	// double speed_h
	object_text += std::to_string(speed_h); object_text += " ";
	// double speed_v
	object_text += std::to_string(speed_v); object_text += " ";
	// bool failed_up
	object_text += std::to_string(failed_up); object_text += " ";
	// bool failed_down
	object_text += std::to_string(failed_down); object_text += " ";
	// bool launched
	object_text += std::to_string(launched); object_text += " ";
	// Player(int) last
	if (last == nullptr) pointer_number = -1;
	else for (int i = 0; i < players.size(); i++) {
		if (last == players[i]) { pointer_number = i; break; }
	}
	object_text += std::to_string(pointer_number); object_text += " ";
	// Player(int) attach
	if (attach == nullptr) pointer_number = -1;
	else for (int i = 0; i < players.size(); i++) {
		if (attach == players[i]) { pointer_number = i; break; }
	}
	object_text += std::to_string(pointer_number); object_text += " ";
}
void Ball::FromString(std::string & object_text) {
	int pointer_number;
	// RectD pos
	this->pos.FromString(object_text);
	object_text.erase(0, object_text.find(' ') + 1);
	// double speed_h
	this->speed_h = atof(object_text.substr(size_t(0), object_text.find(' ')).c_str());
	object_text.erase(0, object_text.find(' ') + 1);
	// double speed_v
	this->speed_v = atof(object_text.substr(size_t(0), object_text.find(' ')).c_str());
	object_text.erase(0, object_text.find(' ') + 1);
	// bool failed_up
	this->failed_up = atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
	object_text.erase(0, object_text.find(' ') + 1);
	// bool failed_down
	this->failed_down = atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
	object_text.erase(0, object_text.find(' ') + 1);
	// bool launched
	this->launched = atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
	object_text.erase(0, object_text.find(' ') + 1);
	// Player(int) last
	pointer_number = atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
	if (pointer_number == -1) last = nullptr;
	else if (players.size() > pointer_number && players[pointer_number] != nullptr) last = players[pointer_number];
	object_text.erase(0, object_text.find(' ') + 1);
	// Player(int) attach
	pointer_number = atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
	if (pointer_number == -1) attach = nullptr;
	else if (players.size() > pointer_number && players[pointer_number] != nullptr) attach = players[pointer_number];
	object_text.erase(0, object_text.find(' ') + 1);
}
Ball &Ball::operator=(const Ball &ball) {
	this->pos = ball.pos;
	this->players = ball.players;
	this->last = ball.last;
	this->attach = ball.attach;
	this->start_speed_v = ball.start_speed_v;
	this->speed_h = ball.speed_h;
	this->speed_v = ball.speed_v;
	this->launched = ball.launched;
	return *this;
}
