#pragma once
#include "GUI.h"
class Stage::TestRoom : public GameStage {
public:
	enum Experiment {
		None,
		Hitbox,
	};
	TestRoom(const GameStageModel base_class);
	~TestRoom();
	void Update();
	void Draw();
	std::string GetInfo() const;
private:
	// Common variables
	Button *exit_button;
	const int experiment_button_count = 2;
	std::vector<Button*> experiment_button;
	std::vector<Button*> experiment_addition_button;
	bool experiment_addition_lines = Settings::DrawCollisionLines();
	Experiment experiment = Experiment::None,
		last_experiment = Experiment::None;
	Vec2 mouse_now, mouse_last, attach_pos/*poses ratio*/;
	bool mouse_hold = false;
	// Experiment::Hitbox
	Label *hitbox_label = nullptr;
	Ball *ball = nullptr;
	Ball *second_ball = nullptr;
	Brick *brick = nullptr;
};