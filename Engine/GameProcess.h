#pragma once
#include "GUI.h"
#include "NewHighScore.h"
class Stage::GameProcess : public GameStage {
public:
	enum InfoType {
		Init,
		Process,
		GameOver,
	};
	GameProcess(const GameStageModel base_class, bool multiplayer, int player_id, bool host, int &high_score);
	~GameProcess();
	void Update();
	void Draw();
	std::string GetInfo() const;
	void ToString(std::string &object_text, InfoType type, bool host) const;
	void FromString(std::string &object_text, InfoType type, bool host);
	friend Server;
	friend Client;
private:
	std::string live_filename;
	Server *server;
	Client *client;
	bool multiplayer, host;
	Player *player, *player_second;
	int player_id;
	PlayerSpeedBoost *player_boost, *player_second_boost;
	Keyboard *kbd_second;
	Ball *ball;
	double update_client_time;
	int lost_updates_to_disconnect, lost_updates, fps;
	Time client_update_time, fps_timer;
	static constexpr int bricks_column_count = 18;
	static constexpr int bricks_row_count = 20;
	static constexpr int bricks_count = bricks_row_count * bricks_column_count;
	int bricks_used;
	Brick *brick[bricks_count];
	Label *score_label; //Label *score_label_second;
	int score, &high_score;
	bool game_over, game_over_new_high_score;
	std::vector<bool> game_init, game_full_init; bool game_start = false;
	Label *live_label, *live_label_second, *updates_lost_label, *fps_label;
	Surface livesf;
	static constexpr int lives_max = 3;
	int lives, lives_second;
	// Other windows
	NewHighScore *new_high_score_window;
	// Sounds
	const std::wstring ball_kick_soundname = L"fart0.wav";
	Sound ball_kick_sound;
};