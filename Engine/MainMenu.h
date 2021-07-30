#pragma once
#include "GUI.h"
#include <vector>
class Stage::MainMenu : public GameStage {
public:
	MainMenu(const GameStageModel base_class, int &high_score);
	~MainMenu();
	void Update();
	void Draw();
	std::string GetInfo() const;
private:
	Surface game_title;
	static constexpr int menu_button_count = 6;
	std::vector<Button*> menu_button;
	bool menu_button_mp_active = false;
	Label *high_score_label = nullptr;
	Label *debug_label = nullptr;
	TextList *updates_textlist = nullptr;
	Label *last_update_description_label = nullptr;
	Label *game_version_label = nullptr;
	// info variables
	bool host, multiplayer;
};

