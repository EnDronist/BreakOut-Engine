#pragma once
class GameStageList {
public:
	enum StageName {
		MainMenu,
		GameProcess,
		TestRoom,
	};
	GameStageList(StageName current_stage): current_stage(current_stage) {};
	~GameStageList() {};
	StageName GetStage() { return current_stage; };
private:
	StageName current_stage;
};