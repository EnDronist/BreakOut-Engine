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
	gfx(wnd), {
	Font::UpdateFonts(gfx);
}
Game::~Game() {}
void Game::Go() {
	// Update model
	if (time.GetTimeElapsed() < 1.0 / 60.0) Sleep((DWORD)(((1.0 / 60.0) - time.GetTimeElapsed()) * 1000));
	double updates_need = time.GetTimeElapsed() * updates_per_second;
	while (updates_counter < updates_need) { UpdateModel(); updates_counter++; }
	// Draw frame
	updates_counter -= updates_need;
	time.ResetTimer();
	gfx.BeginFrame();
	ComposeFrame();
	gfx.EndFrame();
}
void Game::UpdateModel() {
	//Objects update
	//Here it is necessary to update objects with Update() function.
}
void Game::ComposeFrame() {
	for (Polygon2 *polygon : Polygon2::GetSet()) polygon->Draw(gfx, Color{ 255, 127, 255 });
	for (Line2 *line : Line2::GetSet()) line->Draw(gfx, Color{ 255, 127, 255 });
	for (Point2 *point : Point2::GetSet()) point->Draw(gfx, Color{ 255, 127, 255 });
	//Objects draw
	//Here it is necessary to draw objects with Draw() function.
}
