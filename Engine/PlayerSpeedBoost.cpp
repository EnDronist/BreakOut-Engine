#include "PlayerSpeedBoost.h"
PlayerSpeedBoost::PlayerSpeedBoost(Graphics &gfx, Vec2 pos, Vec2 size, double max_energy, double energy, double drain_speed, double replenish_speed):
	gfx(gfx),
	pos(pos, pos + size, HitboxD::Type::Rect),
	surface((Vei2)size),
	max_energy(max_energy),
	energy(energy),
	drain_speed(drain_speed),
	replenish_speed(replenish_speed)
{}
PlayerSpeedBoost::~PlayerSpeedBoost() {}
void PlayerSpeedBoost::Update() {
	if (enabled) {
		double a = timer.GetTimeElapsed();
		energy -= drain_speed * timer.GetTimeElapsed();
		if (energy < 0) energy = 0;
	}
	else {
		double a = timer.GetTimeElapsed();
		energy += replenish_speed * timer.GetTimeElapsed();
		if (energy > max_energy) energy = max_energy;
	}
	timer.ResetTimer();
}
void PlayerSpeedBoost::Redraw() {
	Color energy_min_color = { 255, 0, 0 };
	Color energy_max_color = { 0, 255, 255 };
	unsigned char new_r = energy_min_color.GetR() + (unsigned char)((double)(energy_max_color.GetR() - energy_min_color.GetR()) * (energy / max_energy));
	unsigned char new_g = energy_min_color.GetG() + (unsigned char)((double)(energy_max_color.GetG() - energy_min_color.GetG()) * (energy / max_energy));
	unsigned char new_b = energy_min_color.GetB() + (unsigned char)((double)(energy_max_color.GetB() - energy_min_color.GetB()) * (energy / max_energy));
	for (double y = 0; y < pos.GetHeight(); y++) {
		for (double x = 0; x < pos.GetWidth(); x++) {
			// Border
			if (x < border || x >= (pos.GetWidth() - border) || y < border || y >= (pos.GetHeight() - border)) {
				if (surface.GetPixel((int)x, (int)y) != Color{ 34, 177, 76 }) surface.PutPixel((int)x, (int)y, Color{ 34, 177, 76 });
			}
			// Inside border
			else {
				// Energy
				if (((x - border) / (pos.GetWidth() - (2.0 * border))) < (energy / max_energy) && x >= border && x < (pos.GetWidth() - border)) {
					if (surface.GetPixel((int)x, (int)y) != Color{ new_r, new_g, new_b }) surface.PutPixel((int)x, (int)y, Color{ new_r, new_g, new_b });
				}
				// Empty
				else if (surface.GetPixel((int)x, (int)y) != Color{ 0, 0, 0 }) surface.PutPixel((int)x, (int)y, Color{ 0, 0, 0 });
			}
		}
	}
}
void PlayerSpeedBoost::Draw() {
	SpriteEffect::Copy effect;
	gfx.DrawSprite((Vei2)pos.GetPos(), surface, effect);
}
void PlayerSpeedBoost::Enable(bool enabled) {
	this->enabled = enabled;
}
double PlayerSpeedBoost::GetMaxValue() {
	return max_energy;
}
double PlayerSpeedBoost::GetValue() {
	return energy;
}
void PlayerSpeedBoost::ToString(std::string &object_text) const {
	// double energy
	object_text += std::to_string(energy); object_text += " ";
	// double max_energy
	object_text += std::to_string(max_energy); object_text += " ";
}
void PlayerSpeedBoost::FromString(std::string &object_text) {
	// int lives_second
	this->energy = atof(object_text.substr(size_t(0), object_text.find(' ')).c_str());
	object_text.erase(0, object_text.find(' ') + 1);
	// bool game_over
	this->max_energy = atof(object_text.substr(size_t(0), object_text.find(' ')).c_str());
	object_text.erase(0, object_text.find(' ') + 1);
}
