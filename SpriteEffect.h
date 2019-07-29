#pragma once
#include "GUI.h"
namespace SpriteEffect {
	// Chroma
	class Chroma {
	public:
		Chroma(Color chroma): chroma(chroma) {}
		void operator()(Color cSrc, Vei2 pos, Graphics &gfx) const {
			if (cSrc != chroma) {
				gfx.PutPixel(pos, cSrc);
			}
		}
	private:
		Color chroma;
	};
	// Substitution
	class Substitution {
	public:
		Substitution(Color chroma, Color substitution): chroma(chroma), substitution(substitution) {}
		void operator()(Color cSrc, Vei2 pos, Graphics &gfx) const {
			if (cSrc != chroma) {
				gfx.PutPixel(pos, substitution);
			}
		}
	private:
		Color chroma;
		Color substitution;
	};
	// Copy
	class Copy {
	public:
		void operator()(Color cSrc, Vei2 pos, Graphics &gfx) const {
			gfx.PutPixel(pos, cSrc);
		}
	};
	// Ghost
	class Ghost {
	public:
		Ghost(Color *chroma = nullptr): chroma(chroma) {}
		void operator()(Color src, Vei2 pos, Graphics &gfx) const {
			if (chroma != nullptr && src == *chroma) return;
			const Color dest = gfx.GetPixel(pos);
			const Color blend = {
				unsigned char((src.GetR() + dest.GetR()) / 2),
				unsigned char((src.GetG() + dest.GetG()) / 2),
				unsigned char((src.GetB() + dest.GetB()) / 2)
			};
			gfx.PutPixel(pos, blend);
		}
	private:
		Color *chroma;
	};
	// Chess
	class Chess {
	public:
		Chess(Color color) : color(color) {}
		void operator()(Color src, Vei2 pos, Graphics &gfx) const {
			if ((pos.x + pos.y) % 2 == 0) gfx.PutPixel(pos, color);
		}
	private:
		Color color;
	};
}