#pragma once
#include "GUI.h"
class Player {
public:
	enum InfoType {
		Init,
		Process
	};
	Player(Graphics &gfx, Vec2 pos, PlayerSpeedBoost &boost, bool host, Keyboard &kbd, const std::string surface = "player.bmp");
	virtual ~Player();
	void Update();
	void Draw();
	HitboxDCluster& GetHitbox();
	double GetSpeedH() const;
	double GetSpeedV() const;
	bool IsHost() const;
	Player &Player::operator=(const Player&);
	void AddBuff(Buff *buff);
	bool RemoveBuff(Buff *buff);
	void UpdateBuff(Buff *buff_info);
	int HasBuff(std::string buff_name) const;
	void ToString(std::string &object_text, InfoType type);
	void FromString(std::string &object_text, InfoType type);
	friend Ball;
	friend Buff;
	static constexpr double StartWidth = 96.0;
	static constexpr double StartHeight = 16.0;
private:
	Graphics &gfx;
	Keyboard &kbd;
	PlayerSpeedBoost &boost;
	Surface surface;
	HitboxDCluster pos;
	std::vector<Buff*> buffs;
	bool host;
	double acceleration;
	double speed_reduction_multiplier;
	double speed_h;
	double speed_v;
	double speed_impact_h;
	double speed_impact_v;
	int speed_impact_h_now;
	int speed_impact_v_now;
	double key_shift_acceleration_multiplier;
	double key_control_acceleration_multiplier;
};
