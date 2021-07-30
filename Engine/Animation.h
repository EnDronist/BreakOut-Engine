#pragma once
#include "GUI.h"
class Animation {
public:
	enum Type {
		Color,
	};
	Animation();
	~Animation();
private:
	//Surface &source;
	Type type;
};

