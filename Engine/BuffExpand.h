#pragma once
#include "GUI.h"
class BuffExpand : public Buff {
public:
	BuffExpand(Buff buff, double progress = 0, double expand_multiplier = 2.0);
	~BuffExpand();
	void Update();
	void ToString(std::string &object_text);
	static Buff* FromString(std::string &object_text, Player &owner);
	BuffExpand &BuffExpand::operator=(const BuffExpand &buff);
private:
	double progress = 0;
	const double activate_time = 1.0;
	double expand_multiplier = 2.0;
};

