#pragma once
#include "Buff.h"
Buff::Buff(Player &owner, double duration, double time_remains):
	owner(owner),
	duration(duration),
	time_remains((time_remains == 0.0) ? duration : time_remains) {
}
Buff::~Buff() {}
void Buff::Update() {
	time_remains = duration - time.GetTimeElapsed();
}
double Buff::GetTime() {
	time_remains = duration - time.GetTimeElapsed();
	if (time_remains < 0.0) return 0.0;
	else return time_remains;
}
std::string Buff::GetType() {
	std::string buff_name = typeid(this).name();
	buff_name.erase(0, buff_name.find(' ') + 1);
	return buff_name;
}
void Buff::ToString(std::string & object_text) {
	// double time_remains
	object_text += std::to_string(time_remains); object_text += " ";
	// double duration
	object_text += std::to_string(duration); object_text += " ";
}
Buff* Buff::FromString(std::string & object_text, Player &owner) {
	int text_size;
	// Start
	object_text.erase(0, object_text.find('{') + 1);
	// typename this
	text_size = atoi(object_text.substr(size_t(0), object_text.find('{')).c_str());
	object_text.erase(0, object_text.find('{') + 1);
	std::string buff_name = object_text.substr(size_t(0), size_t(text_size));
	object_text.erase(0, object_text.find('}') + 1);
	// Create Buff
	Buff* new_buff = nullptr;
	if (buff_name == "BuffExpand") new_buff = BuffExpand::FromString(object_text, owner);
	return new_buff;
}
Buff& Buff::operator=(const Buff &buff) {
	this->owner = buff.owner;
	// Если новое оставшееся время больше прошлого, то к длительности добавляется разница в оставшемся времени
	this->duration += ((buff.time_remains > this->time_remains) ? (buff.time_remains - this->time_remains) : 0.0);
	this->time_remains = buff.time_remains;
	return *this;
}