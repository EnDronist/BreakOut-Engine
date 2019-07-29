#include "Settings.h"
int Settings::screen_width = (atoi(Util::fgetstr("settings.txt", 2).c_str()) > 0) ? atoi(Util::fgetstr("settings.txt", 2).c_str()) : 1;
int Settings::screen_height = (atoi(Util::fgetstr("settings.txt", 3).c_str()) > 0) ? atoi(Util::fgetstr("settings.txt", 3).c_str()) : 1;
double Settings::size_multiplier_x = (double)Settings::ScreenWidth() / (double)Settings::GameScreenWidth;
double Settings::size_multiplier_y = (double)Settings::ScreenHeight() / (double)Settings::GameScreenHeight;
bool Settings::draw_collision_rects = (Util::fgetstr("settings.txt", 5) == "true" || Util::fgetstr("settings.txt", 5) == "1") ? true : false;
bool Settings::draw_collision_lines = (Util::fgetstr("settings.txt", 7) == "true" || Util::fgetstr("settings.txt", 7) == "1") ? true : false;