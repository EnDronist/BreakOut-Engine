#pragma once
#include "GUI.h"
class GameStage {
public:
	GameStage(MainWindow &wnd, Graphics &gfx, GameStageList &stage, Time &time):
		wnd(wnd),
		gfx(gfx), 
		stage(stage),
		time(time)
	{}
	virtual ~GameStage() {}
	virtual void Update() {}
	virtual void Draw() {}
	virtual std::string GetInfo() const { return std::string(); }
	virtual void ToString(std::string &object_text) const {}
	virtual void FromString(std::string &object_text) {}
protected:
	MainWindow &wnd;
	Graphics &gfx;
	GameStageList &stage;
	Time &time;
};
class GameStageModel : public GameStage {
public:
	GameStageModel(MainWindow &wnd, Graphics &gfx, GameStageList &stage, Time &time):
		GameStage(wnd, gfx, stage, time),
		wnd_(wnd),
		gfx_(gfx),
		stage_(stage),
		time_(time)
	{}
	MainWindow &wnd_;
	Graphics &gfx_;
	GameStageList &stage_;
	Time &time_;
};