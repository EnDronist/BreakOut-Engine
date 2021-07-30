#pragma once
#include "Util.h"
class Settings {
public:
	static void Update() {
		screen_width = (atoi(fgetstr("settings.txt", 2).c_str()) > 0) ? atoi(fgetstr("settings.txt", 2).c_str()) : 1;
		screen_height = (atoi(fgetstr("settings.txt", 3).c_str()) > 0) ? atoi(fgetstr("settings.txt", 3).c_str()) : 1;
		size_multiplier_x = (double)Settings::ScreenWidth() / (double)Settings::GameScreenWidth;
		size_multiplier_y = (double)Settings::ScreenHeight() / (double)Settings::GameScreenHeight;
		draw_collision_rects = (fgetstr("settings.txt", 5) == "true" || fgetstr("settings.txt", 5) == "1") ? true : false;
		draw_collision_lines = (fgetstr("settings.txt", 7) == "true" || fgetstr("settings.txt", 7) == "1") ? true : false;
	}
	static int& ScreenWidth() { return screen_width; }
	static int& ScreenHeight() { return screen_height; }
	static double& SizeMultiplierX() { return size_multiplier_x; }
	static double& SizeMultiplierY() { return size_multiplier_y; }
	static bool& DrawCollisionRects() { return draw_collision_rects; }
	static bool& DrawCollisionLines() { return draw_collision_lines; }
public:
	static constexpr int GameScreenWidth = 1280;
	static constexpr int GameScreenHeight = 720;
	static constexpr int Fps = 60;
private:
	static int screen_width;
	static int screen_height;
	static double size_multiplier_x;
	static double size_multiplier_y;
	static bool draw_collision_rects;
	static bool draw_collision_lines;
};