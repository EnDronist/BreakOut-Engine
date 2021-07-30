#include "Settings.h"
int Settings::screen_width = (atoi(fgetstr("settings.txt", 2).c_str()) > 0) ? atoi(fgetstr("settings.txt", 2).c_str()) : 1;
int Settings::screen_height = (atoi(fgetstr("settings.txt", 3).c_str()) > 0) ? atoi(fgetstr("settings.txt", 3).c_str()) : 1;
double Settings::size_multiplier_x = (double)Settings::ScreenWidth() / (double)Settings::GameScreenWidth;
double Settings::size_multiplier_y = (double)Settings::ScreenHeight() / (double)Settings::GameScreenHeight;
bool Settings::draw_collision_rects = (fgetstr("settings.txt", 5) == "true" || fgetstr("settings.txt", 5) == "1") ? true : false;
bool Settings::draw_collision_lines = (fgetstr("settings.txt", 7) == "true" || fgetstr("settings.txt", 7) == "1") ? true : false;