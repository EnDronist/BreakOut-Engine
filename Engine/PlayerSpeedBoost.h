#pragma once
#include "GUI.h"
class PlayerSpeedBoost {
public:
	PlayerSpeedBoost(Graphics &gfx, Vec2 pos, Vec2 size, double max_energy, double energy, double drain_speed, double replenish_speed);
	~PlayerSpeedBoost();
	void Update();
	void Redraw();
	void Draw();
	void Enable(bool enabled);
	double GetMaxValue();
	double GetValue();
	void ToString(std::string &object_text) const;
	void FromString(std::string &object_text);
private:
	Graphics &gfx;
	HitboxD pos;
	Surface surface;
	static constexpr double border = 4.0;
	double drain_speed, replenish_speed;
	double max_energy, energy;
	bool enabled = false;
	Time timer;
};