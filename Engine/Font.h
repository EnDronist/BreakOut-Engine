#pragma once
#include "GUI.h"
#include <map>
class Font {
public:
	Font(Graphics &gfx, std::string filename, Color chroma = Colors::White);
	void Update(std::string filename, Color chroma = Colors::White);
	void DrawTxt(const std::string &text, Vec2 pos, Color color, double size_multiplier, const HitboxD *clip = nullptr) const;
	double GetGlyphWidth() const;
	double GetGlyphHeight() const;
	static void UpdateFonts(Graphics &gfx);
	static Font& Find(std::string font_name);
private:
	HitboxI MapGlyphRect(char c) const;
private:
	Graphics &gfx;
	Surface surface;
	Color chroma;
	const int n_columns = 32;
	const int n_rows = 3;
	double glyph_width;
	double glyph_height;
	const char first_char = ' ';
	const char last_char = '~';
};