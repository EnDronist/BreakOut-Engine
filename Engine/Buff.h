#pragma once
#include "GUI.h"
class Buff {
public:
	Buff(Player &owner, double duration, double time_remains = 0.0);
	virtual ~Buff();
	virtual void Update();
	double GetTime();
	virtual std::string GetType();
	virtual void ToString(std::string &object_text);
	static Buff* FromString(std::string &object_text, Player &owner);
	virtual Buff &Buff::operator=(const Buff &buff);
protected:
	Player &owner;
	Time time;
	double time_remains;
	double duration;
	// Momentary
	bool activate = false;
};
#include "BuffExpand.h"
