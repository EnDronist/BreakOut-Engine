#include "GameProcess.h"
#include <iostream>
#include <random>
#include <ctime>
Stage::GameProcess::GameProcess(const GameStageModel base_class, bool multiplayer, int player_id, bool host, int &high_score):
	GameStage(base_class.wnd_, base_class.gfx_, base_class.stage_, base_class.time_),
	multiplayer(multiplayer),
	host(host),
	server(nullptr),
	client(nullptr),
	player_id(player_id),
	player_second(nullptr),
	kbd_second(nullptr),
	update_client_time(1.0/5.0),// per second
	lost_updates_to_disconnect((int)(5.0 / update_client_time)),// 5 seconds
	lost_updates(0),
	fps(0),
	bricks_used(0),
	score(0),
	high_score(high_score),
	game_over(false),
	game_over_new_high_score(false),
	livesf("live.bmp"),
	lives(lives_max),
	lives_second(lives_max),
	new_high_score_window(nullptr),
	ball_kick_sound({ ball_kick_soundname }) {
	game_init.resize(2);
	game_full_init.resize(2);
	std::vector<Player*> players;
	if (multiplayer) kbd_second = new Keyboard();
	player_boost = new PlayerSpeedBoost(gfx, { (Settings::GameScreenWidth / 2) - (158 / 2), Settings::GameScreenHeight - 30 - 8 }, { 158, 30 }, 100, 100, 20, 4);
	player = new Player(gfx, {(((double)Settings::GameScreenWidth - Player::StartWidth)*0.50),
		(((double)Settings::GameScreenHeight - Player::StartHeight)*0.93)}, *player_boost, host ? true : false, host ? wnd.kbd : *kbd_second);
	players.push_back(player);
	if (multiplayer) {
		player_second_boost = new PlayerSpeedBoost(gfx, { (Settings::GameScreenWidth / 2) - (158 / 2), 10 }, { 158, 30 }, 100, 100, 20, 4);
		player_second = new Player(gfx, {(((double)Settings::GameScreenWidth - Player::StartWidth)*0.50),
			(((double)Settings::GameScreenHeight - Player::StartHeight)*0.07)}, *player_second_boost, host ? true : false, host ? *kbd_second : wnd.kbd);
		players.push_back(player_second);
	}
	ball = new Ball(gfx, players, kbd_second, players[0],
		{ (((double)Settings::GameScreenWidth - Player::StartWidth)*0.5), (((double)Settings::GameScreenHeight - Player::StartHeight)*0.5) },
		{ 1.0, 1.0 }, 5.0, 5.0);
	srand((int)std::time((time_t)0));
	double brick_x_start = 30.0, brick_y_start = 150.0;
	for (int i = 0, y = 0; y < bricks_row_count; y++) {
		double brick_y = y * (int)Brick::StartHeight + (y * 4.0) + brick_y_start;
		for (int brick_x, x = 0; x < bricks_column_count; x++, i++) {
			bool chance = (rand() % 100) < 10;
			brick_x = (int)(x * (int)Brick::StartWidth + (x * 4.0) + brick_x_start);
			if (chance) brick[i] = new Brick(gfx, { (double)brick_x, (double)brick_y }, { 1.0, 1.0 }, *ball, Brick::Type::Expand, multiplayer, &ball_kick_sound);
			else brick[i] = new Brick(gfx, { (double)brick_x, (double)brick_y }, {1.0, 1.0}, *ball, Brick::Type::Common, multiplayer, &ball_kick_sound);
		}
	}
	score_label = new Label(gfx, "Score: " + std::to_string(score), { 20, Settings::GameScreenHeight - 40 }, Color{ 255,255,255 }, 1.0, Font::Find("pixel"));
	live_label = new Label(gfx, "Lives:", { Settings::GameScreenWidth - 180, Settings::GameScreenHeight - 40 }, Color{ 255, 255, 255 }, 1.0, Font::Find("pixel"));
	fps_label = new Label(gfx, "FPS: " + std::to_string(fps), { 10, 10 }, Color{ 255,255,255 }, 1.0, Font::Find("pixel"));
	if (multiplayer) {
		//score_label_second = new Label("Score: " + std::to_string(score), Vei2{ 20, 0 + 40 }, Color{ 255,255,255 }, font);
		updates_lost_label = new Label(gfx, "Lost packages: " + std::to_string(lost_updates), { 220, Settings::GameScreenHeight - 40 }, Color{ 255, 255, 255 }, 1.0, Font::Find("pixel"));
		live_label_second = new Label(gfx, "Lives:", { Settings::GameScreenWidth - 180, 10 }, Color{ 255, 255, 255 }, 1.0, Font::Find("pixel"));
		lives_second = lives_max;
	}
}
Stage::GameProcess::~GameProcess() {
	if (player != nullptr) { delete player; player = nullptr; }
	if (player_boost != nullptr) { delete player_boost; player_boost = nullptr; }
	if (ball != nullptr) { delete ball; ball = nullptr; }
	for (int i = 0; i < bricks_count; i++) { if (brick[i] != nullptr) { delete brick[i]; brick[i] = nullptr; } }
	if (score_label != nullptr) { delete score_label; score_label = nullptr; }
	if (live_label != nullptr) { delete live_label; live_label = nullptr; }
	if (fps_label != nullptr) { delete fps_label; fps_label = nullptr; }
	if (multiplayer) {
		if (player_second != nullptr) { delete player_second; player_second = nullptr; }
		if (player_second_boost != nullptr) { delete player_second_boost; player_second_boost = nullptr; }
		if (kbd_second != nullptr) { delete kbd_second; kbd_second = nullptr; }
		//if (score_label_second != nullptr) { delete score_label_second; score_label_second = nullptr; }
		if (updates_lost_label != nullptr) { delete updates_lost_label; updates_lost_label = nullptr; }
		if (live_label_second != nullptr) { delete live_label_second; live_label_second = nullptr; }
		if (server != nullptr) { delete server; server = nullptr; }
		if (client != nullptr) { delete client; client = nullptr; }
	}
	//Other windows
	if (new_high_score_window != nullptr) { delete new_high_score_window; new_high_score_window = nullptr; }
}
void Stage::GameProcess::Update() {
	//Connecting
	if (multiplayer && server == nullptr && client == nullptr && !game_over) {
		if (host) {
			server = new Server();
			std::string ip_address = fgetstr("ip_address.txt", 2);
			server->Start(IPPROTO_TCP, ip_address);//wnd.ShowMessageBox(L"Yes", L"Yes");
			for (int t = 0; t < 5; t++)
			{
				if (server->Find(1.0)) break;
				else if (t + 1 == 5) {
					delete server; server = nullptr;
					stage = GameStageList::StageName::MainMenu;
					return;
				}
			}
		}
		else {
			client = new Client();
			client->Start(IPPROTO_TCP);//wnd.ShowMessageBox(L"Yes", L"Yes");
			std::string ip_address = fgetstr("ip_address.txt", 4);
			for (int t = 0; t < 5; t++)
			{
				if (client->Connect(1.0, ip_address)) break;
				else if (t + 1 == 5) {
					delete client; client = nullptr;
					stage = GameStageList::StageName::MainMenu;
					return;
				}
			}
		}
	}
	// Init game
	if (!game_start) {
		if (!multiplayer) { game_start = true; return; }
		if (client_update_time.GetTimeElapsed() >= update_client_time) {
			client_update_time.ResetTimer();
			std::string send_text, recv_text_type, recv_text;
			if (host) {
				this->ToString(send_text, InfoType::Init, host);
				// Recv control info from client
				if (server->Recv(*server->GetClient(), recv_text_type, recv_text) != SOCKET_ERROR) {
					// GameProcess::Init type is need
					if (recv_text_type == "GameProcess::Init") {
						lost_updates = 0;
						this->FromString(recv_text, InfoType::Init, host);
					}
					else if (recv_text_type == "GameProcess::Process") game_start = true;
					else lost_updates++;
				}
				else lost_updates++;
				// Sending info to client
				if (server->Send(*server->GetClient(), "GameProcess::Init", send_text) != SOCKET_ERROR) {}
				else lost_updates++;
				if (lost_updates == lost_updates_to_disconnect) game_over = true;
			}
			else {
				this->ToString(send_text, InfoType::Init, host);
				// Recv info from server
				if (client->Recv(*client->GetSocket(), recv_text_type, recv_text) != SOCKET_ERROR) {
					// GameProcess::Init type is need
					if (recv_text_type == "GameProcess::Init") {
						lost_updates = 0;
						this->FromString(recv_text, InfoType::Init, host);
					}
					else lost_updates++;
				}
				else lost_updates++;
				// Sending control info to server
				if (client->Send(*client->GetSocket(), "GameProcess::Init", send_text) != SOCKET_ERROR) {}
				else lost_updates++;
				if (lost_updates == lost_updates_to_disconnect) game_over = true;
			}
		}
	}
	// Game is active
	if (!game_over && game_start) {
		// Update objects
		// Common updates
		player->Update(); player_boost->Update();
		if (multiplayer) { player_second->Update(); player_second_boost->Update(); }
		ball->Update();
		if (ball->IsFailedUp() && multiplayer) { ball->Restart(player_second); lives_second--; }
		if (ball->IsFailedDown()) { ball->Restart(player); lives--; }
		for (int i = 0; i < bricks_count; i++) {
			if (!brick[i]->IsUsed()) brick[i]->Update();
		}
		for (int i = 0; i < bricks_count; i++) {
			if (brick[i]->IsUsed()) if (!brick[i]->IsCounted()) {
				bricks_used++;
				score += 1;
				score_label->Update("Score: " + std::to_string(score));
				brick[i]->Effect();
				brick[i]->SetCounted(true);
			}
		}
		// Only host updates (multiplayer host and singleplayer)
		if (host && (bricks_used == bricks_count || lives <= 0 || lives_second <= 0)) { game_over = true; }
		//Info transference
		if (multiplayer && !game_over && client_update_time.GetTimeElapsed() >= update_client_time) {
			client_update_time.ResetTimer();
			std::string send_text, recv_text_type, recv_text;
			if (host) {
				this->ToString(send_text, InfoType::Process, host);
				//Recv control info from client
				if (server->Recv(*server->GetClient(), recv_text_type, recv_text) != SOCKET_ERROR) {
					if (recv_text_type == "GameProcess::Process") {
						lost_updates = 0;
						this->FromString(recv_text, InfoType::Process, host);
					}
				}
				else lost_updates++;
				//Sending info to client
				if (server->Send(*server->GetClient(), "GameProcess::Process", send_text) != SOCKET_ERROR) {}
				else lost_updates++;
				if (lost_updates == lost_updates_to_disconnect) game_over = true;
			}
			else {
				this->ToString(send_text, InfoType::Process, host);
				//Recv info from server
				if (client->Recv(*client->GetSocket(), recv_text_type, recv_text) != SOCKET_ERROR) {
					if (recv_text_type == "GameProcess::Process") {
						lost_updates = 0;
						this->FromString(recv_text, InfoType::Process, host);
					}
					else if (recv_text_type == "GameProcess::GameOver") {
						this->FromString(recv_text, InfoType::GameOver, host);
					}
				}
				else lost_updates++;
				//Sending control info to server
				if (client->Send(*client->GetSocket(), "GameProcess::Process", send_text) != SOCKET_ERROR) {}
				else lost_updates++;
				if (lost_updates == lost_updates_to_disconnect) game_over = true;
			}
		}
	}
	// Only multiplayer updates
	if (multiplayer) {
		updates_lost_label->Update("Lost packages: " + std::to_string(lost_updates));
	}
	// Game over
	if (game_over) {
		if (multiplayer) {
			if (client != nullptr) { delete client; client = nullptr; }
			if (host && server != nullptr && client_update_time.GetTimeElapsed() >= update_client_time) {
				client_update_time.ResetTimer();
				std::string send_text;
				this->ToString(send_text, InfoType::GameOver, host);
				if (server->Send(*server->GetClient(), "GameProcess::GameOver", send_text) != SOCKET_ERROR) {}
				else { delete server; server = nullptr; }
			}
		}
		if (high_score < score && new_high_score_window == nullptr) {
			high_score = score;
			game_over_new_high_score = true;
			new_high_score_window = new Stage::NewHighScore({ wnd, gfx, stage, time }, high_score);
		}
		if (new_high_score_window == nullptr) stage = GameStageList::StageName::MainMenu;
		if (new_high_score_window != nullptr) new_high_score_window->Update();
	}
}
void Stage::GameProcess::Draw() {
	for (int i = 0; i < bricks_count; i++) {
		if (!brick[i]->IsUsed()) brick[i]->Draw();
	}
	ball->Draw();
	player->Draw();
	player_boost->Redraw(); player_boost->Draw();
	score_label->Draw();
	live_label->Draw();
	// FPS label
	fps = (int)ceil(1.0 / fps_timer.GetTimeElapsed());
	fps_timer.ResetTimer();
	fps_label->Update("FPS: " + std::to_string(fps));
	fps_label->Draw();
	// -----
	for (int i = 0; i < lives; i++) {
		gfx.DrawSprite(
			{ (int)(live_label->GetHitbox().GetX() + live_label->GetWidth() + (i * (livesf.GetWidth()) + ((i + 1) * 8.0))),
			(int)(live_label->GetHitbox().GetY() + (live_label->GetHeight() / 2) - (livesf.GetHeight() / 2)) },
			livesf,
			SpriteEffect::Chroma{ Color{ 255,255,255 } }
		);
	}
	if (multiplayer) {
		player_second->Draw();
		player_second_boost->Redraw(); player_second_boost->Draw();
		live_label_second->Draw();
		updates_lost_label->Draw();
		for (int i = 0; i < lives_second; i++) {
			gfx.DrawSprite(
				{ (int)(live_label_second->GetHitbox().GetX() + live_label_second->GetWidth() + (i * (livesf.GetWidth()) + ((i + 1) * 8))),
				(int)(live_label_second->GetHitbox().GetY() + (live_label_second->GetHeight() / 2) - (livesf.GetHeight() / 2)) },
				livesf,
				SpriteEffect::Chroma{ Color{ 255,255,255 } }
			);
		}
	}
	if (new_high_score_window != nullptr) new_high_score_window->Draw();
}
std::string Stage::GameProcess::GetInfo() const {
	std::string info;
	// int score
	info += std::to_string(high_score);
	return info;
}
void Stage::GameProcess::ToString(std::string &object_text, InfoType type, bool host) const {
	if (type == InfoType::Init) {
		if (host) {
			// Player player
			player->ToString(object_text, Player::InfoType::Init);
			// Brick brick[bricks_count]
			for (int i = 0; i < bricks_count; i++) { brick[i]->ToString(object_text, Brick::InfoType::Init); }
			// std::vector<bool> game_init
			object_text += std::to_string(game_init.size());
			object_text += "{";
			for (int i = 0; i < game_init.size(); i++) { object_text += std::to_string(game_init[i]); object_text += " "; }
			object_text += "}";
		}
		else {
			// Player player_second
			player_second->ToString(object_text, Player::InfoType::Init);
			// int player_id
			object_text += std::to_string(player_id); object_text += " ";
			// std::vector<bool> game_init[player_id]
			// Will be set to true by server
		}
	}
	else if (type == InfoType::Process) {
		if (host) {
			// Player player
			player->ToString(object_text, Player::InfoType::Process);
			// PlayerSpeedBoost player_boost
			player_boost->ToString(object_text);
			// Player player_second
			player_second->ToString(object_text, Player::InfoType::Process);
			// PlayerSpeedBoost player_second_boost
			player_second_boost->ToString(object_text);
			// Ball ball
			ball->ToString(object_text);
			// Brick brick[bricks_count]
			for (int i = 0; i < bricks_count; i++) brick[i]->ToString(object_text, Brick::InfoType::Process);
			// Label score_label
			score_label->ToString(object_text);
			// int score
			object_text += std::to_string(score); object_text += " ";
			// int lives
			object_text += std::to_string(lives); object_text += " ";
			// int lives_second
			object_text += std::to_string(lives_second); object_text += " ";
		}
		else {
			// Keyboard kbd_second
			wnd.kbd.ToString(object_text);
		}
	}
	else if (type == InfoType::GameOver) {
		// bool game_over
		object_text += std::to_string(game_over); object_text += " ";
	}
}
void Stage::GameProcess::FromString(std::string &object_text, InfoType type, bool host) {
	if (type == InfoType::Init) {
		if (host) {
			// Player player_second
			player_second->FromString(object_text, Player::InfoType::Init);
			// int client_id(player_id)
			int client_id = atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
			object_text.erase(0, object_text.find(' ') + 1);
			// std::vector<bool> game_init[client_id]
			this->game_init[client_id] = true;
			// Init may be done
			bool all_players_init = true;
			for (int i = 0; i < game_init.size(); i++) if (!game_init[i] && i != player_id) all_players_init = false;
			this->game_init[player_id] = all_players_init;
			// If all client init then server init
			//if (game_full_init) game_init[player_id] = true;
		}
		else {
			// Player player
			player->FromString(object_text, Player::InfoType::Init);
			// Brick brick[bricks_count]
			for (int i = 0; i < bricks_count; i++) brick[i]->FromString(object_text, Brick::InfoType::Init);
			// std::vector<bool> game_init
			int vector_size = atoi(object_text.substr(size_t(0), object_text.find('{')).c_str());
			object_text.erase(0, object_text.find('{') + 1);
			for (int i = 0; i < vector_size; i++) {
				//if (player_id == i) { this->game_init[i] = true; continue; }
				this->game_init[i] = atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
				object_text.erase(0, object_text.find(' ') + 1);
			}
			object_text.erase(0, object_text.find('}') + 1);
			// Init may be done
			bool all_players_init = true;
			for (auto init : game_init) if (!init) all_players_init = false;
			this->game_start = all_players_init;
		}
	}
	else if (type == InfoType::Process) {
		if (host) {
			// Keyboard kbd_second
			kbd_second->FromString(object_text);
		}
		else {
			// Player player
			player->FromString(object_text, Player::InfoType::Process);
			// PlayerSpeedBoost player_boost
			player_boost->FromString(object_text);
			// Player player_second
			player_second->FromString(object_text, Player::InfoType::Process);
			// PlayerSpeedBoost player_second_boost
			player_second_boost->FromString(object_text);
			// Ball ball
			ball->FromString(object_text);
			// Brick brick[bricks_count]
			for (int i = 0; i < bricks_count; i++) brick[i]->FromString(object_text, Brick::InfoType::Process);
			// Label score_label
			score_label->FromString(object_text);
			// int score
			this->score = atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
			object_text.erase(0, object_text.find(' ') + 1);
			// int lives
			this->lives = atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
			object_text.erase(0, object_text.find(' ') + 1);
			// int lives_second
			this->lives_second = atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
			object_text.erase(0, object_text.find(' ') + 1);
		}
	}
	else if (type == InfoType::GameOver) {
		// bool game_over
		this->game_over = atoi(object_text.substr(size_t(0), object_text.find(' ')).c_str());
		object_text.erase(0, object_text.find(' ') + 1);
	}
}