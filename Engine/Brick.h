#pragma once
#include "GUI.h"
#include <assert.h>
class Brick {
public:
	enum Type {
		Common,//	Original
		Expand,//	Dark Blue - Player width x2
		Divide,//	Blue - Balls x3
		Laser,//	Red - Vertical laser destroying bricks
		Slow,//		Pink - Slow balls x0.5
		Break,//	Purple - Next level
		Catch,//	Green - Balls sticks to player
		Player,//	Gray - Extra life
	};
	enum InfoType {
		Init,
		Process
	};
	Brick(Graphics &gfx, Vec2 pos, Vec2 size_multiplier, Ball &ball, Type type, bool multiplayer, Sound *ball_kick = nullptr);
	~Brick();
	void Update();
	void Draw() const;
	void Effect();
	bool IsUsed() const;
	bool IsCounted() const;
	void SetCounted(bool counted);
	HitboxDCluster& GetHitbox();
	Vec2 GetSize() const;
	void SetSize(Vec2 size);
	void ToString(std::string &object_text, InfoType type);
	void FromString(std::string &object_text, InfoType type);
	static constexpr double StartWidth = 64.0;
	static constexpr double StartHeight = 16.0;
private:
	Graphics &gfx;
	Surface surface;
	HitboxDCluster pos;
	Vec2 size_multiplier;
	Ball &ball;
	Type type;
	bool used, counted, physical;
	// Sound
	Sound *ball_kick_sound;
};
