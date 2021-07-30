#pragma once
#include "GUI.h"
class Stage::NewHighScore : public GameStage {
public:
	NewHighScore(const GameStageModel base_class, int &high_score);
	~NewHighScore();
	void Update();
	void Draw();
private:
	Button *new_high_score_panel;
	Label *new_high_score_label;
	Button *new_high_score_button;
};

