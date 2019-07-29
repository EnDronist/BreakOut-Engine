/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.h																			  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
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
#include "Util.h"
#include "GameException.h"
#include "Surface.h"
#include <d3d11.h>
#include <wrl.h>
#include <assert.h>
class Graphics {
public:
	class Exception : public GameException {
	public:
		Exception(HRESULT hr, const std::wstring &note, const wchar_t *file, unsigned int line);
		std::wstring GetErrorName() const;
		std::wstring GetErrorDescription() const;
		virtual std::wstring GetFullMessage() const override;
		virtual std::wstring GetExceptionType() const override;
	private:
		HRESULT hr;
	};
private:
	// vertex format for the framebuffer fullscreen textured quad
	struct FSQVertex {
		float x, y, z;		// position
		float u, v;			// texcoords
	};
public:
	Graphics(class HWNDKey& key);
	Graphics(const Graphics&) = delete;
	~Graphics();
	Graphics& operator=(const Graphics&) = delete;
	void EndFrame();
	void BeginFrame();
	static HitboxI GetGameRectI();
	static HitboxD GetGameRectD();
	HitboxI GetScreenRect();
	Color GetPixel(Vei2 pos) const;
	void PutPixel(Vei2 pos, Color c);
	//template<typename E>
	//void DrawSprite(int x, int y, const Surface &surface, E effect);
	//template<typename E>
	//void DrawSprite(int x, int y, const HitboxI &surface_rect, const Surface &surface, E effect);
	//template<typename E>
	//void DrawSprite(int x, int y, HitboxI surface_rect, const HitboxI &clip, const Surface &surface, E effect);
	template<typename E>
	void DrawSprite(Vei2 pos, const Surface &surface, E effect, Vec2 size_multiplier = { 1.0, 1.0 }) {
		DrawSprite(pos, surface.GetRect(), surface, effect, size_multiplier);
	}
	template<typename E>
	void DrawSprite(Vei2 pos, const HitboxI &surface_rect, const Surface &surface, E effect, Vec2 size_multiplier = { 1.0, 1.0 }) {
		DrawSprite(pos, surface_rect, GetGameRectD(), surface, effect, size_multiplier);
	}
	template<typename E>
	void DrawSprite(Vei2 pos, HitboxI surface_rect, HitboxD &clip, const Surface &surface, E effect, Vec2 size_multiplier = { 1.0, 1.0 }) {
		assert(surface_rect.left >= 0);
		assert(surface_rect.right <= surface.GetWidth());
		assert(surface_rect.top >= 0);
		assert(surface_rect.bottom <= surface.GetHeight());
		//Check: clip & GameScreen
		if (clip.left >= Settings::GameScreenWidth) clip.left = Settings::GameScreenWidth - 1;
		else if (clip.left < 0) clip.left = 0;
		if (clip.top >= Settings::GameScreenHeight) clip.top = Settings::GameScreenHeight - 1;
		else if (clip.top < 0) clip.top = Settings::GameScreenHeight;
		if (clip.right >= Settings::GameScreenWidth) clip.right = Settings::GameScreenWidth - 1;
		else if (clip.right < 0) clip.right = 0;
		if (clip.bottom >= Settings::GameScreenHeight) clip.bottom = Settings::GameScreenHeight - 1;
		else if (clip.bottom < 0) clip.bottom = 0;
		//Check: clip & surface_rect
		/*if ((double)pos.x < clip.left) pos.x = (int)clip.left;
		if ((double)pos.y < clip.top) pos.y = (int)clip.top;
		if ((double)pos.x + (double)surface_rect.GetWidth() * size_multiplier.x > clip.right)
			surface_rect.right = surface_rect.left + (int)((clip.right - (double)pos.x) / size_multiplier.x);
		if ((double)pos.y + (double)surface_rect.GetHeight() * size_multiplier.y > clip.bottom)
			surface_rect.bottom = surface_rect.top + (int)((clip.bottom - (double)pos.y) / size_multiplier.y);*/
		//Check: pos & rect_draw
		HitboxD rect_draw = {
			(double)surface_rect.left * Settings::SizeMultiplierX() * size_multiplier.x,
			(double)surface_rect.right * Settings::SizeMultiplierX() * size_multiplier.x,
			(double)surface_rect.top * Settings::SizeMultiplierY() * size_multiplier.y,
			(double)surface_rect.bottom * Settings::SizeMultiplierY() * size_multiplier.y
		};
		HitboxD rect_clip = {
			(double)clip.left * Settings::SizeMultiplierX(),
			(double)clip.right * Settings::SizeMultiplierX(),
			(double)clip.top * Settings::SizeMultiplierY(),
			(double)clip.bottom * Settings::SizeMultiplierY()
		};
		int x_draw = (int)((double)pos.x * Settings::SizeMultiplierX());
		int y_draw = (int)((double)pos.y * Settings::SizeMultiplierY());
		if ((double)x_draw < rect_clip.left) {
			rect_draw.left += rect_clip.left - (double)x_draw; x_draw = (int)rect_clip.left;
		}
		if ((double)y_draw < rect_clip.top) {
			rect_draw.top += rect_clip.top - (double)y_draw; y_draw = (int)rect_clip.top;
		}
		if ((double)x_draw + rect_draw.GetWidth() > rect_clip.right)
			rect_draw.right -= (double)x_draw + rect_draw.GetWidth() - rect_clip.right;
		if ((double)y_draw + rect_draw.GetHeight() > rect_clip.bottom)
			rect_draw.bottom -= (double)y_draw + rect_draw.GetHeight() - rect_clip.bottom;
		for (int dy = 0; dy < rect_draw.GetHeight(); dy++) {
			if (y_draw + dy > Settings::ScreenHeight() || y_draw + dy >= rect_clip.bottom) break;
			for (int dx = 0; dx < rect_draw.GetWidth(); dx++) {
				if (x_draw + dx > Settings::ScreenWidth() || x_draw + dx >= rect_clip.right) break;
				int x_get = (int)((rect_draw.left + (double)dx) / (Settings::SizeMultiplierX() * size_multiplier.x));
				int y_get = (int)((rect_draw.top + (double)dy) / (Settings::SizeMultiplierY() * size_multiplier.y));
				effect(surface.GetPixel(x_get, y_get), { x_draw + dx, y_draw + dy }, *this);
			}
		}
	}
	template<typename T, typename E>
	void DrawHitbox(Hitbox_<T> hitbox, E effect, Vec2 size_multiplier = { 1.0, 1.0 }) {
		double x_mp = (double)hitbox.left / size_multiplier.x, y_mp = (double)hitbox.top / size_multiplier.y;
		HitboxI clip = GetScreenRect();
		if (hitbox.left < clip.left) { hitbox.left += clip.left - x_mp; hitbox.left = clip.left; }
		if (hitbox.top < clip.top) { hitbox.top += clip.top - y_mp; hitbox.top = clip.top; }
		HitboxD rect_draw = {
			hitbox.left * Settings::SizeMultiplierX() * size_multiplier.x,
			hitbox.right * Settings::SizeMultiplierX() * size_multiplier.x,
			hitbox.top * Settings::SizeMultiplierY() * size_multiplier.y,
			hitbox.bottom * Settings::SizeMultiplierY() * size_multiplier.y,
			hitbox.GetType()
		};
		Vei2 pos_draw = { (int)((double)hitbox.left * Settings::SizeMultiplierX()),
			(int)((double)hitbox.top * Settings::SizeMultiplierY()) };
		for (int dy = 0; dy < rect_draw.GetHeight(); dy++) {
			if (pos_draw.y < clip.top || pos_draw.y + dy >= clip.bottom) continue;
			for (int dx = 0; dx < rect_draw.GetWidth(); dx++) {
				if (pos_draw.x < clip.left || pos_draw.x + dx >= clip.right) continue;
				if (rect_draw.GetType() == HitboxD::Type::Rect) {
					effect(Color(252, 15, 192), { pos_draw.x + dx, pos_draw.y + dy }, *this);
				}
				else if (rect_draw.GetType() == HitboxD::Type::Ellipse) {
					if (pow(pos_draw.x, 2) / pow(hitbox.GetWidth(), 2) + pow(pos_draw.y, 2) / pow(hitbox.GetHeight(), 2) <= 1)
						effect(Color(252, 15, 192), { pos_draw.x + dx, pos_draw.y + dy }, *this);
				}
			}
		}
	}
	template<typename E>
	void DrawLine(Vec2 start, Vec2 end, Color color, E effect);
	template<typename E>
	void DrawTriangle(const Vec2 &first, const Vec2 &second, const Vec2 &third, Color color, E effect);
	template<typename E>
	void DrawCircle(const Vec2 &pos, double radius, Color color, E effect);
	void SetSize(int ScreenWidth, int ScreenHeight) {
		//screen_width = ScreenWidth;
		//screen_height = ScreenHeight;
		//size_multiplier_x = (double)screen_width / (double)GameScreenWidth;
		//size_multiplier_y = (double)screen_height / (double)GameScreenHeight;
		// 1. Clear the existing references to the backbuffer
		//ID3D11RenderTargetView* nullViews[] = { nullptr };
		//pImmediateContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
		//pRenderTargetView.Reset();
		//pImmediateContext->Flush();
		// 2. Resize the existing swapchain
		//_aligned_free(pSysBuffer); pSysBuffer = nullptr;
		//pSysBuffer = reinterpret_cast<Color*>(_aligned_malloc(sizeof(Color) * screen_width * screen_height, 16u));
		// 3. Get the new backbuffer texture to use as a render target
		//pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pSysBufferTexture);
		//pDevice->CreateRenderTargetView(pSysBufferTexture.Get(), nullptr, &pRenderTargetView);
	}
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain>				pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device>				pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			pImmediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				pSysBufferTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pSysBufferTextureView;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			pSamplerState;
	D3D11_MAPPED_SUBRESOURCE							mappedSysBufferTexture;
	Color*                                              pSysBuffer = nullptr;
};
template<typename E>
void Graphics::DrawLine(Vec2 start, Vec2 end, Color color, E effect) {
	Vec2 size_miltiplier = Vec2(Settings::SizeMultiplierX(), Settings::SizeMultiplierY());
	Vec2 draw_start = start * size_miltiplier,
		draw_end = end * size_miltiplier,
		draw_vec = draw_end - draw_start;
	HitboxI draw_clip = GetScreenRect();
	for (int i = 0; i < (int)draw_vec.GetLength(); i++) {
		Vei2 draw_pos(
			(int)draw_start.x + (int)((double)i * cos((float)draw_vec.GetAngle())),
			(int)draw_start.y + (int)((double)i * sin((float)draw_vec.GetAngle()))
		);
		if (draw_pos.x < draw_clip.left || draw_pos.x > draw_clip.right ||
			draw_pos.y < draw_clip.top || draw_pos.y > draw_clip.bottom)
			continue;
		effect(color, draw_pos, *this);
	}
}
template<typename E>
void Graphics::DrawTriangle(const Vec2 &first, const Vec2 &second, const Vec2 &third, Color color, E effect) {
	Vec2 size_miltiplier = Vec2(Settings::SizeMultiplierX(), Settings::SizeMultiplierY());
	std::vector<Vec2> points_pos{ first, second, third };
	std::sort(points_pos.begin(), points_pos.end(), [](const Vec2 &first, const Vec2 &second) -> bool {
		return first.y < second.y;
	});
	for (Vec2 &pos : points_pos) pos *= size_miltiplier;
	/*const Vec2 &top = first.y >= third.y ? first.y >= second.y ? first : second : third,
	&bottom = first.y < third.y ? first.y < second.y ? first : second : third,
	&middle = &first == &top || &first == &bottom ? &second == &top || &second == &bottom ? third : second : first;*/
	Vec2 &top = points_pos[0], &middle = points_pos[1], &bottom = points_pos[2];
	bool is_bottom_flat = bottom.y == middle.y, is_top_flat = top.y == middle.y;
	Vec2 auxiliary_pos(is_top_flat ? top : bottom);
	if (!is_bottom_flat && !is_top_flat) {
		auxiliary_pos = { top.x + (middle.y - top.y) / (bottom.y - top.y) * (bottom.x - top.x), middle.y };
		is_bottom_flat = true, is_top_flat = true;
	}
	HitboxI draw_clip = GetScreenRect();
	if (is_bottom_flat) {
		double left_side_narrowing = (middle.x - top.x) / (middle.y - top.y),
			right_side_narrowing = (auxiliary_pos.x - top.x) / (auxiliary_pos.y - top.y),
			current_left_x = top.x, current_right_x = top.x;
		for (int y_draw = (int)top.y; y_draw <= auxiliary_pos.y; y_draw++) {
			int start_x_draw = (int)(current_left_x < current_right_x ? current_left_x : current_right_x),
				end_x_draw = (int)(current_left_x > current_right_x ? current_left_x : current_right_x);
			for (int x_draw = start_x_draw; x_draw <= end_x_draw; x_draw++) {
				if (x_draw < draw_clip.left || x_draw > draw_clip.right ||
					y_draw < draw_clip.top || y_draw > draw_clip.bottom)
					continue;
				effect(color, { x_draw, y_draw }, *this);
			}
			current_left_x += left_side_narrowing;
			current_right_x += right_side_narrowing;
		}
	}
	if (is_top_flat) {
		double left_side_narrowing = (bottom.x - auxiliary_pos.x) / (bottom.y - auxiliary_pos.y),
			right_side_narrowing = (bottom.x - middle.x) / (bottom.y - middle.y),
			current_left_x = bottom.x, current_right_x = bottom.x;
		for (int y_draw = (int)bottom.y; y_draw > auxiliary_pos.y; y_draw--) {
			int start_x_draw = (int)(current_left_x < current_right_x ? current_left_x : current_right_x),
				end_x_draw = (int)(current_left_x > current_right_x ? current_left_x : current_right_x);
			for (int x_draw = start_x_draw; x_draw <= end_x_draw; x_draw++) {
				if (x_draw < draw_clip.left || x_draw > draw_clip.right ||
					y_draw < draw_clip.top || y_draw > draw_clip.bottom)
					continue;
				effect(color, { x_draw, y_draw }, *this);
			}
			current_left_x -= left_side_narrowing;
			current_right_x -= right_side_narrowing;
		}
	}
}
template<typename E>
void Graphics::DrawCircle(const Vec2 &pos, double radius, Color color, E effect) {
	if (radius == 0) return;
	Vec2 size_miltiplier = Vec2{ Settings::SizeMultiplierX(), Settings::SizeMultiplierY() };
	Vei2 draw_pos((int)pos.x, (int)pos.y);
	draw_pos *= size_miltiplier;
	Vec2 draw_radius = size_miltiplier * radius;
	double longest_radius = (size_miltiplier.x > size_miltiplier.y ? size_miltiplier.x : size_miltiplier.y) * radius;
	int draw_perimetr_length = (int)(2 * M_PI * longest_radius);
	HitboxI draw_clip = GetScreenRect();
	for (int i = 0; i < draw_perimetr_length; i++) {
		int x_draw = (int)draw_pos.x + (int)(draw_radius.x * cos(i / longest_radius)),
			y_draw = (int)draw_pos.y + (int)(draw_radius.y * sin(i / longest_radius));
		if (x_draw < draw_clip.left || x_draw > draw_clip.right
			|| y_draw < draw_clip.top || y_draw > draw_clip.bottom) continue;
		effect(color, { x_draw, y_draw }, *this);
		PutPixel({ x_draw, y_draw }, color);
	}
}