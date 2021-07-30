#include "ChiliWin.h"
#include "Surface.h"
#include <cassert>
#include <fstream>
#include <algorithm>
Surface::Surface(const std::string &filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) { pixels.resize(0); }
	else {
		BITMAPFILEHEADER bmFileHeader;
		file.read(reinterpret_cast<char*>(&bmFileHeader), sizeof(bmFileHeader));
		BITMAPINFOHEADER bmInfoHeader;
		file.read(reinterpret_cast<char*>(&bmInfoHeader), sizeof(bmInfoHeader));
		//assert(bmInfoHeader.biBitCount == 24 || bmInfoHeader.biBitCount == 32);
		//assert(bmInfoHeader.biCompression == BI_RGB);
		const bool is32b = bmInfoHeader.biBitCount == 32;
		size.x = bmInfoHeader.biWidth;
		int yStart;
		int yEnd;
		int dy;
		if (bmInfoHeader.biHeight < 0) {
			size.y = -bmInfoHeader.biHeight;
			yStart = 0;
			yEnd = size.y;
			dy = 1;
		}
		else {
			size.y = bmInfoHeader.biHeight;
			yStart = size.y - 1;
			yEnd = -1;
			dy = -1;
		}
		pixels.resize(size.x * size.y);
		file.seekg(bmFileHeader.bfOffBits);
		const int padding = (4 - (size.x * 3) % 4) % 4;
		for (int y = yStart; y != yEnd; y += dy) {
			for (int x = 0; x < size.x; x++) {
				PutPixel(x, y, Color(file.get(), file.get(), file.get()));
				if (is32b) {
					file.seekg(1, std::ios::cur);
				}
			}
			if (!is32b) {
				file.seekg(padding, std::ios::cur);
			}
		}
	}
}
Surface::Surface(int width, int height):
	size(width, height),
	pixels(this->size.x * this->size.y)
{}
Surface::Surface(Vei2 size):
	size(size),
	pixels(this->size.x * this->size.y)
{}
Surface::Surface(Surface &&donor) {
	*this = std::move(donor);
}
Surface& Surface::operator=(Surface &&rhs) {
	size = rhs.size;
	pixels = std::move(rhs.pixels);
	return *this;
}
void Surface::PutPixel(int x, int y, Color c) {
	pixels[y * size.x + x] = c;
}
Color Surface::GetPixel(int x, int y) const {
	assert(pixels.size() > y * size.x + x);
	return pixels[y * size.x + x];
}
Vei2 Surface::GetSize() const {
	return size;
}
int Surface::GetWidth() const {
	return size.x;
}
int Surface::GetHeight() const {
	return size.y;
}
HitboxI Surface::GetRect() const{
	return { 0, size.x, 0, size.y, HitboxI::Type::Rect };
}
const Color* Surface::Data() const {
	return pixels.data();
}
void Surface::ToString(std::string &object_text) {
	// std::vector<Color> pixels
	object_text += std::to_string(pixels.size());
	object_text += "{";
	for (auto pixel : pixels) pixel.ToString(object_text);
	object_text += "}";
}
void Surface::FromString(std::string &object_text) {
	// std::vector<Color> pixels
	int vector_size = atoi(object_text.substr(size_t(0), object_text.find('{')).c_str());
	object_text.erase(0, object_text.find('{') + 1);
	for (int i = 0; i < vector_size; i++) this->pixels[i].FromString(object_text);
	object_text.erase(0, object_text.find('}') + 1);
}