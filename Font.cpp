#include "Font.h"
#include <cassert>
namespace Fonts {
	std::vector<Font> vector;
	std::map<std::string, Font*> map;
}
Font::Font(Graphics &gfx, std::string filename, Color chroma):
	gfx(gfx),
	surface(filename),
	chroma(Colors::White),
	glyph_width((double)surface.GetWidth() / (double)n_columns),
	glyph_height((double)surface.GetHeight() / (double)n_rows) {
	assert(glyph_width * n_columns == surface.GetWidth());
	assert(glyph_height * n_rows == surface.GetHeight());
}
void Font::Update(std::string filename, Color chroma) {
	this->surface = Surface(filename);
	this->chroma = chroma;
	this->glyph_width = (double)surface.GetWidth() / (double)n_columns;
	this->glyph_height = (double)surface.GetHeight() / (double)n_rows;
}
void Font::DrawTxt(const std::string &text, Vec2 pos, Color color, double size_multiplier, const HitboxD *clip) const {
	SpriteEffect::Substitution effect{ chroma, color };
	Vec2 current_pos = pos;
	HitboxD *draw_clip = new HitboxD((clip != nullptr) ? *clip : Graphics::GetGameRectD());
	for (char c : text) {
		if (c == '\n') {
			current_pos.x = pos.x;
			current_pos.y += GetGlyphHeight() * size_multiplier;
			continue;
		}
		else if (c >= first_char + 1 && c <= last_char) {
			gfx.DrawSprite((Vei2)current_pos, MapGlyphRect(c), *draw_clip, surface, effect, { size_multiplier, size_multiplier });
		}
		current_pos.x += GetGlyphWidth() * size_multiplier;
	}
	delete draw_clip;
}
double Font::GetGlyphWidth() const {
	return glyph_width;
}
double Font::GetGlyphHeight() const {
	return glyph_height;
}
HitboxI Font::MapGlyphRect(char c) const {
	assert(c >= first_char && c <= last_char);
	const int glyphIndex = c - ' ';
	const int yGlyph = glyphIndex / n_columns;
	const int xGlyph = glyphIndex % n_columns;
	return HitboxI({ (int)(xGlyph * glyph_width), (int)(yGlyph * glyph_height) },
		(int)glyph_width, (int)glyph_height, HitboxI::Type::Rect);
}
// Static methods
void Font::UpdateFonts(Graphics &gfx) {
	Fonts::vector.clear();
	Fonts::map.clear();
	// Default
	Fonts::vector.push_back(Font(gfx, "font-16x28.bmp", Colors::White));
	Fonts::map.insert(std::pair<std::string, Font*>(std::string("default"), &Fonts::vector.back()));
	// font-16x28.bmp
	Fonts::vector.push_back(Font(gfx, "font-16x28.bmp", Colors::White));
	Fonts::map.insert(std::pair<std::string, Font*>(std::string("pixel"), &Fonts::vector.back()));
}
Font& Font::Find(std::string font_name) {
	assert(!Fonts::vector.empty());
	return ((Fonts::map.find(font_name) != Fonts::map.end()) ? *Fonts::map[font_name] : *Fonts::map["default"]);
}