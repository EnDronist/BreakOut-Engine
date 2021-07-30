#pragma once
#include "GUI.h"
class Ball {
public: // pl1 required, pl2 could be
	Ball(Graphics &gfx, std::vector<Player*> players, Keyboard *kbd_second, Player *attach,
		Vec2 pos, Vec2 size_multiplier, double start_horizontal_speed, double start_vertical_speed,
		const std::string &filename = "ball.bmp");
	~Ball();
	bool OutWindow();
	Player* PlayerCollision();
	void Update();
	void Draw() const;
	HitboxDCluster& GetHitbox();
	bool IsLaunched() const;
	bool IsFailedUp() const;
	bool IsFailedDown() const;
	bool IsTouched() const;
	void Restart(Player *attach);
	void Touch();
	void ToString(std::string &object_text);
	void FromString(std::string &object_text);
	Ball &Ball::operator=(const Ball&);
	static constexpr double StartWidth = 16.0;
	static constexpr double StartHeight = 16.0;
	friend Brick;
private:
	Graphics &gfx;
	Surface surface;
	HitboxDCluster pos;
	Vec2 size_multiplier;
	std::vector<Player*> players;
	Player *last, *attach;
	Keyboard *kbd_second;
	double speed_h;
	double speed_v;
	double start_speed_v = 100.0 / 1000.0;
	double speed_impact_h = 0.0;
	double speed_impact_v = 0.0;
	double speed_increace = 0.0001 / 1000.0;
	bool failed_up = false, failed_down = false;
	// Momentary
	bool launched = false;
	bool touched = false;
};