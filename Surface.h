#pragma once
#include "Colors.h"
#include "Hitbox.h"
#include <string>
#include <vector>
class Surface {
public:
	Surface(const std::string &filename);
	Surface(int width, int height);
	Surface(Vei2 size);
	Surface(Surface &&donor);
	Surface(const Surface&) = default;
	Surface& operator=(Surface&&);
	Surface& operator=(const Surface&) = default;
	Surface() = default;
	void PutPixel(int x, int y, Color c);
	Color GetPixel(int x, int y) const;
	Vei2 GetSize() const;
	int GetWidth() const;
	int GetHeight() const;
	HitboxI GetRect() const;
	const Color *Data() const;
	void ToString(std::string &object_text);
	void FromString(std::string &object_text);
private:
	Vei2 size = { 0, 0 };
	std::vector<Color> pixels;
};

