/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.h																				  *
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
#pragma once
#include "GUI.h"
class Game {
public:
	Game(class MainWindow &wnd);
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	~Game();
	void Go();
private:
	void ComposeFrame();
	void UpdateModel();
public:
private:
	//Main
	MainWindow &wnd;
	HitboxI wnd_rect{ 0, 0, 0, 0, HitboxI::Type::Rect };
	bool wnd_max = false;
	Graphics gfx;
	//Current GameStage
	GameStage *game_stage;
	//Common Variables
	int high_score = 0;
	GameStageList current_stage, last_stage;
	Time time;
	double updates_counter = 0;
#ifdef _DEBUG
	double updates_per_second = 100;
#else
	double updates_per_second = 1000;
#endif
};