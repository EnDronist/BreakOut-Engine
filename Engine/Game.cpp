/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "Game.h"
#include <assert.h>
Game::Game(MainWindow &wnd):
	wnd(wnd),
	gfx(wnd),
	current_stage(GameStageList::StageName::MainMenu),
	last_stage(GameStageList::StageName::MainMenu) {
	Font::UpdateFonts(gfx);
	game_stage = new Stage::MainMenu({ wnd, gfx, current_stage, time }, high_score);
	RECT rect;
	GetClientRect(wnd.GetHandle(), &rect);
	wnd_rect = { rect.left, rect.right, rect.top, rect.bottom, HitboxI::Type::Rect };
}
Game::~Game() {
	if (game_stage != nullptr) { delete game_stage; game_stage = nullptr; }
}
void Game::Go() {
	// Update model
	if (time.GetTimeElapsed() < 1.0 / Settings::Fps) Sleep((DWORD)(((1.0 / Settings::Fps) - time.GetTimeElapsed()) * 1000));
	double updates_need = time.GetTimeElapsed() * updates_per_second;
	while (updates_counter < updates_need) UpdateModel();
	// Draw frame
	updates_counter -= updates_need;
	time.ResetTimer();
	gfx.BeginFrame();
	ComposeFrame();
	gfx.EndFrame();
	if (!wnd_max && wnd.IsMaximized()) {
		RECT rect;
		GetClientRect(wnd.GetHandle(), &rect);
		gfx.SetSize(rect.right - rect.left, rect.bottom - rect.top);
		wnd_max = true;
	}
	else if (wnd_max && !wnd.IsMaximized()) {
		gfx.SetSize(wnd_rect.GetWidth(), wnd_rect.GetHeight());
		wnd_max = false;
	}
}
void Game::UpdateModel() {
	updates_counter++;
	//Обновление объектов
	game_stage->Update();
	//Проверка этапа
	if (current_stage.GetStage() != last_stage.GetStage()) {
		// Разовые переменные --
		bool multiplayer = false, host = true;
		// Getting data from stage
		if (last_stage.GetStage() == GameStageList::StageName::GameProcess) {
			high_score = atoi(game_stage->GetInfo().c_str());
		}
		else if (last_stage.GetStage() == GameStageList::StageName::MainMenu) {
			std::string info = game_stage->GetInfo();
			// bool multiplayer
			multiplayer = atoi(info.substr(size_t(0), info.find(' ')).c_str());
			info.erase(0, info.find(' ') + 1);
			// bool host
			host = atoi(info.substr(size_t(0), info.find(' ')).c_str());
			info.erase(0, info.find(' ') + 1);
		}
		// Creating new stage
		delete game_stage; game_stage = nullptr;
		if (current_stage.GetStage() == GameStageList::StageName::MainMenu) {
			if (game_stage == nullptr) game_stage = new Stage::MainMenu({ wnd, gfx, current_stage, time }, high_score);
		}
		else if (current_stage.GetStage() == GameStageList::StageName::GameProcess) {
			if (game_stage == nullptr) game_stage = new Stage::GameProcess({ wnd, gfx, current_stage, time }, multiplayer, (host ? 0 : 1), host, high_score);
		}
		else if (current_stage.GetStage() == GameStageList::StageName::TestRoom) {
			if (game_stage == nullptr) game_stage = new Stage::TestRoom({ wnd, gfx, current_stage, time });
		}
		//Обновление последнего этапа
		last_stage = current_stage;
	}
}
void Game::ComposeFrame() {
	//Отрисовка персонажа
	game_stage->Draw();
}