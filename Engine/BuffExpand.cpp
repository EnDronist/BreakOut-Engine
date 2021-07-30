#pragma once
#include "BuffExpand.h"
BuffExpand::BuffExpand(Buff buff, double progress, double expand_multiplier):
	Buff(buff),
	progress(progress),
	expand_multiplier(expand_multiplier) {
}
BuffExpand::~BuffExpand() {
	Buff::~Buff();
	// Buff effect end
	if (activate) {
		Vec2 owner_center = owner.GetHitbox().GetCenter();
		owner.GetHitbox()[1]->right = owner.GetHitbox()[1]->left + owner.StartWidth - owner.StartHeight;
		owner.GetHitbox()[2]->SetX(owner.GetHitbox()[1]->right - owner.StartHeight / 2);
		owner.GetHitbox().GetGeneralHitbox().right = owner.GetHitbox()[2]->right;
		owner.GetHitbox().AddX(owner_center.x - owner.GetHitbox().GetWidth() / 2 - owner.GetHitbox().GetX());
	}
}
void BuffExpand::Update() {
	Buff::Update();
	// Buff effect start
	if (time.GetTimeElapsed() < activate_time) progress = time.GetTimeElapsed() / activate_time;
	else if (time.GetTimeElapsed() > duration - activate_time) progress = (duration - time.GetTimeElapsed()) / activate_time;
	else progress = 1.0;
	Vec2 owner_center = owner.GetHitbox().GetCenter();
	owner.GetHitbox()[1]->right = owner.GetHitbox()[1]->left + owner.StartWidth * (1 + (expand_multiplier - 1) * progress) - owner.StartHeight;
	owner.GetHitbox()[2]->SetX(owner.GetHitbox()[1]->right - owner.StartHeight / 2);
	owner.GetHitbox().GetGeneralHitbox().right = owner.GetHitbox()[2]->right;
	owner.GetHitbox().AddX(owner_center.x - owner.GetHitbox().GetWidth() / 2 - owner.GetHitbox().GetX());
	if (!activate) activate = true;
}
void BuffExpand::ToString(std::string &object_text) {
	// Start
	object_text += "{";
	// typename this
	object_text += std::to_string(std::string("BuffExpand").size()) + std::string("{") + std::string("BuffExpand") + std::string("}");
	// Buff this
	Buff::ToString(object_text);
	// double progress
	object_text += std::to_string(progress); object_text += " ";
	// double expand_multiplier
	object_text += std::to_string(expand_multiplier); object_text += " ";
	// End
	object_text += "}";
}
Buff* BuffExpand::FromString(std::string &object_text, Player &owner) {
	// Continuation of the Buff::FromString method
	// double time_remains
	double time_remains = atof(object_text.substr(size_t(0), object_text.find(' ')).c_str());
	object_text.erase(0, object_text.find(' ') + 1);
	// double duration
	double duration = atof(object_text.substr(size_t(0), object_text.find(' ')).c_str());
	object_text.erase(0, object_text.find(' ') + 1);
	// double progress
	double progress = atof(object_text.substr(size_t(0), object_text.find(' ')).c_str());
	object_text.erase(0, object_text.find(' ') + 1);
	// double expand_multiplier
	double expand_multiplier = atof(object_text.substr(size_t(0), object_text.find(' ')).c_str());
	object_text.erase(0, object_text.find(' ') + 1);
	// End
	object_text.erase(0, object_text.find('}') + 1);
	Buff* new_buff = new BuffExpand({ owner, duration, time_remains }, progress, expand_multiplier);
	return new_buff;// {10{BuffExpand}5.0 } 
}
BuffExpand& BuffExpand::operator=(const BuffExpand &buff) {
	Buff::operator=(buff);
	this->expand_multiplier = buff.expand_multiplier;
	return *this;
}

