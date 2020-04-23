#include "c_surface.h"
#include "../utils/math.h"
#include "c_engine_client.h"
#include "../hacks/c_esp.h"
#include "../hooks/c_client.h"
#include "../fonts/profont.h"

vgui::HFont F::Menu;
vgui::HFont F::ESPInfo;
vgui::HFont F::ESP;
vgui::HFont F::ESPWeapon;
vgui::HFont F::Events;
vgui::HFont F::BombTimer;
vgui::HFont F::Indicators;
c_vector2d F::screen_size = c_vector2d(0, 0);
const char* Drawing::LastFontName = "";


void Drawing::CreateFonts()
{
	LastFontName = "";

	DWORD fnt;
	AddFontMemResourceEx(reinterpret_cast<void*>(shellcode_astriumwep), sizeof(shellcode_astriumwep), nullptr, &fnt);

	LastFontName = ("Tahoma");
	surface()->SetFontGlyphSet(F::Menu =surface()->CreateFont_(), _("Tahoma"), 12, 400, NULL, NULL, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
	LastFontName = ("Verdana");
	surface()->SetFontGlyphSet(F::ESP =surface()->CreateFont_(), _("Verdana"), 12, 700, NULL, NULL, 128);
	LastFontName = ("Smallest Pixel-7");
	surface()->SetFontGlyphSet(F::ESPInfo =surface()->CreateFont_(), _("Smallest Pixel-7"), 10, 100, NULL, NULL, FONTFLAG_OUTLINE);
	LastFontName = ("Lucida Console");
	surface()->SetFontGlyphSet(F::Events =surface()->CreateFont_(), _("Lucida Console"), 10, NULL, NULL, NULL, FONTFLAG_DROPSHADOW);
	LastFontName = ("astriumwep");
	surface()->SetFontGlyphSet(F::ESPWeapon =surface()->CreateFont_(), _("astriumwep"), 16, 500, NULL, NULL, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
	LastFontName = ("Verdana");
	surface()->SetFontGlyphSet(F::BombTimer =surface()->CreateFont_(), _("Verdana"), 14, 0, NULL, NULL, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
	LastFontName = ("Verdana");
	surface()->SetFontGlyphSet(F::Indicators = surface()->CreateFont_(), _("Verdana"), 25, 800, NULL, NULL, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
	LastFontName = "";
}

void Drawing::DrawString(vgui::HFont font, int x, int y, c_color color, DWORD alignment, const char* msg, ...)
{
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf_s(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int r = 255, g = 255, b = 255, a = 255;
	r = color.red, g = color.green, b = color.blue, a = color.alpha;

	int width, height;
	surface()->GetTextSize(font, wbuf, width, height);

	if (alignment & FONT_RIGHT)
		x -= width;
	if (alignment & FONT_CENTER)
		x -= width / 2;

	surface()->DrawSetTextFont(font);

	if (width > 65536)
		return;

	if (alignment & FONT_OUTLINE)
	{
		surface()->DrawSetTextColor(c_color(0, 0, 0, color.alpha));
		surface()->DrawSetTextPos(x + 1, y /*+ 1*/);
		surface()->DrawPrintText(wbuf, wcslen(wbuf));

		surface()->DrawSetTextColor(c_color(0, 0, 0, color.alpha));
		surface()->DrawSetTextPos(x - 1, y /*- 1*/);
		surface()->DrawPrintText(wbuf, wcslen(wbuf));
	}

	surface()->DrawSetTextColor(r, g, b, a);
	surface()->DrawSetTextPos(x, y /*- height / 2*/);
	surface()->DrawPrintText(wbuf, wcslen(wbuf));
}

void Drawing::DrawStringFont(vgui::HFont font, int x, int y, c_color clrc_color, bool bCenter, const char* szText, ...)
{
	if (!szText)
		return;

	va_list va_alist;
	char buf[1024];
	va_start(va_alist, szText);
	_vsnprintf_s(buf, sizeof(buf), szText, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	if (bCenter)
	{
		int Wide = 0, Tall = 0;

		surface()->GetTextSize(font, wbuf, Wide, Tall);

		x -= Wide / 2;
	}

	surface()->DrawSetTextFont(font);
	surface()->DrawSetTextPos(x, y);
	surface()->DrawSetTextColor(clrc_color);
	surface()->DrawPrintText(wbuf, wcslen(wbuf));
}

void Drawing::DrawStringUnicode(vgui::HFont font, int x, int y, c_color color, bool bCenter, const wchar_t* msg, ...)
{
	int r = 255, g = 255, b = 255, a = 255;
	r = color.red, g = color.green, b = color.blue, a = color.alpha;

	int iWidth, iHeight;

	surface()->GetTextSize(font, msg, iWidth, iHeight);
	surface()->DrawSetTextFont(font);
	surface()->DrawSetTextColor(r, g, b, a);
	surface()->DrawSetTextPos(!bCenter ? x : x - iWidth / 2, y - iHeight / 2);
	surface()->DrawPrintText(msg, wcslen(msg));
}

void Drawing::DrawRect(int x, int y, int w, int h, c_color col)
{
	surface()->DrawSetColor(col.red, col.green, col.blue, col.alpha);
	surface()->draw_filled_rect(x, y, x + w, y + h);
}

void Drawing::Rectangle(float x, float y, float w, float h, float px, c_color col)
{
	DrawRect(x, (y + h - px), w, px, col);
	DrawRect(x, y, px, h, col);
	DrawRect(x, y, w, px, col);
	DrawRect((x + w - px), y, px, h, col);
}

void Drawing::Border(int x, int y, int w, int h, int line, c_color col)
{
	DrawRect(x, y, w, line, col);
	DrawRect(x, y, line, h, col);
	DrawRect((x + w), y, line, h, col);
	DrawRect(x, (y + h), (w + line), line, col);
}

void Drawing::DrawRectRainbow(int x, int y, int width, int height, float flSpeed, float& flRainbow)
{
	c_color colc_color(0, 0, 0);

	flRainbow += flSpeed;
	if (flRainbow > 1.f) flRainbow = 0.f;

	for (int i = 0; i < width; i++)
	{
		float hue = (1.f / (float)width) * i;
		hue -= flRainbow;
		if (hue < 0.f) hue += 1.f;

		c_color colRainbow = colc_color.from_hsb(hue, 1.f, 1.f);
		Drawing::DrawRect(x + i, y, 1, height, colRainbow);
	}
}

void Drawing::DrawRectGradientVertical(int x, int y, int width, int height, c_color color1, c_color color2)
{
	float flDifferenceR = (float)(color2.red - color1.red) / (float)height;
	float flDifferenceG = (float)(color2.green - color1.green) / (float)height;
	float flDifferenceB = (float)(color2.blue - color1.blue) / (float)height;

	for (float i = 0.f; i < height; i++)
	{
		c_color colGradient = c_color(color1.red + (flDifferenceR * i), color1.green + (flDifferenceG * i), color1.blue + (flDifferenceB * i), color1.alpha);
		Drawing::DrawRect(x, y + i, width, 1, colGradient);
	}
}

void Drawing::DrawRectGradientHorizontal(int x, int y, int width, int height, c_color color1, c_color color2)
{
	float flDifferenceR = (float)(color2.red - color1.red) / (float)width;
	float flDifferenceG = (float)(color2.green - color1.green) / (float)width;
	float flDifferenceB = (float)(color2.blue - color1.blue) / (float)width;

	for (float i = 0.f; i < width; i++)
	{
		c_color colGradient = c_color(color1.red + (flDifferenceR * i), color1.green + (flDifferenceG * i), color1.blue + (flDifferenceB * i), color1.alpha);
		Drawing::DrawRect(x + i, y, 1, height, colGradient);
	}
}

void Drawing::DrawPixel(int x, int y, c_color col)
{
	surface()->DrawSetColor(col.red, col.green, col.blue, col.alpha);
	surface()->draw_filled_rect(x, y, x + 1, y + 1);
}

void Drawing::DrawOutlinedRect(int x, int y, int w, int h, c_color col)
{
	surface()->DrawSetColor(col.red, col.green, col.blue, col.alpha);
	surface()->DrawOutlinedRect(x, y, x + w, y + h);
}

void Drawing::DrawOutlinedCircle(int x, int y, int r, int s, c_color col)
{
	surface()->DrawSetColor(col.red, col.green, col.blue, col.alpha);
	surface()->DrawOutlinedCircle(x, y, r, s);
}

void Drawing::DrawLine(int x0, int y0, int x1, int y1, c_color col)
{
	surface()->DrawSetColor(col.red, col.green, col.blue, col.alpha);
	surface()->DrawLine(x0, y0, x1, y1);
}

void Drawing::DrawCorner(int iX, int iY, int iWidth, int iHeight, bool bRight, bool bDown, c_color colDraw)
{
	int iRealX = bRight ? iX - iWidth : iX;
	int iRealY = bDown ? iY - iHeight : iY;

	if (bDown && bRight)
		iWidth = iWidth + 1;

	Drawing::DrawRect(iRealX, iY, iWidth, 1, colDraw);
	Drawing::DrawRect(iX, iRealY, 1, iHeight, colDraw);

	Drawing::DrawRect(iRealX, bDown ? iY + 1 : iY - 1, !bDown && bRight ? iWidth + 1 : iWidth, 1, c_color(0, 0, 0, 255));
	Drawing::DrawRect(bRight ? iX + 1 : iX - 1, bDown ? iRealY : iRealY - 1, 1, bDown ? iHeight + 2 : iHeight + 1, c_color(0, 0, 0, 255));
}
//
//#define COL2DWORD(x) (D3DCOLOR_ARGB(x.alpha, x.red, x.green, x.blue))
//void Drawing::Triangle(c_vector2d pos1, c_vector2d pos2, c_vector2d pos3, c_color color) const
//{
//	const auto dwc_color = COL2DWORD(color);
//	Vertex vert[4] =
//	{
//		{ pos1.x, pos1.y, 0.0f, 1.0f, dwc_color },
//		{ pos2.x, pos2.y, 0.0f, 1.0f, dwc_color },
//		{ pos3.x, pos3.y, 0.0f, 1.0f, dwc_color },
//		{ pos1.x, pos1.y, 0.0f, 1.0f, dwc_color }
//	};
//
//	this->pDevice->SetTexture(0, nullptr);
//	this->pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 3, &vert, sizeof(Vertex));
//}
//
//
//void Drawing::TriangleFilled(c_vector2d pos1, c_vector2d pos2, c_vector2d pos3, c_color color) const
//{
//	const auto dwc_color = COL2DWORD(color);
//	Vertex vert[3] =
//	{
//		{ pos1.x, pos1.y, 0.0f, 1.0f, dwc_color },
//		{ pos2.x, pos2.y, 0.0f, 1.0f, dwc_color },
//		{ pos3.x, pos3.y, 0.0f, 1.0f, dwc_color }
//	};
//
//	this->pDevice->SetTexture(0, nullptr);
//	this->pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 1, &vert, sizeof(Vertex));
//}

void Drawing::Triangle(c_vector3d ldcorner, c_vector3d rucorner, c_color col)
{
	DrawLine(ldcorner.x, ldcorner.y, (rucorner.x / 2) - 1, rucorner.y, col); // left shit

	DrawLine(rucorner.x, ldcorner.y, (rucorner.x / 2) - 1, rucorner.y, col); // right shit

	DrawLine(ldcorner.x, ldcorner.y, rucorner.x, ldcorner.y/*(ldcorner.x - rucorner.x), (ldcorner.y - rucorner.y)*/, col); // down shit
}

void Drawing::DrawPolygon(int count, Vertex_t* Vertexs, c_color color)
{
	static int Texture =surface()->CreateNewTextureID(true);
	unsigned char buffer[4] = { 255, 255, 255, 255 };

	surface()->DrawSetTextureRGBA(Texture, buffer, 1, 1);
	surface()->DrawSetColor(color.red, color.green, color.blue, color.alpha);
	surface()->DrawSetTexture(Texture);

	surface()->DrawTexturedPolygon(count, Vertexs);
}

void Drawing::DrawBox(int x, int y, int w, int h, c_color color)
{
	// top
	DrawRect(x, y, w, 1, color);
	// right
	DrawRect(x, y + 1, 1, h - 1, color);
	// left
	DrawRect(x + w - 1, y + 1, 1, h - 1, color);
	// bottom
	DrawRect(x, y + h - 1, w - 1, 1, color);
}

void Drawing::DrawRoundedBox(int x, int y, int w, int h, int r, int v, c_color col)
{
	std::vector<Vertex_t> p;
	for (int _i = 0; _i < 3; _i++)
	{
		int _x = x + (_i < 2 && r || w - r);
		int _y = y + (_i % 3 > 0 && r || h - r);
		for (int i = 0; i < v; i++)
		{
			int a = rad2deg((i / v) * -90 - _i * 90);
			p.push_back(Vertex_t(c_vector2d(_x + sin(a) * r, _y + cos(a) * r)));
		}
	}

	Drawing::DrawPolygon(4 * (v + 1), &p[0], col);
	/*
	function DrawRoundedBox(x, y, w, h, r, v, col)
	local p = {};
	for _i = 0, 3 do
	local _x = x + (_i < 2 && r || w - r)
	local _y = y + (_i%3 > 0 && r || h - r)
	for i = 0, v do
	local a = math.rad((i / v) * - 90 - _i * 90)
	table.insert(p, {x = _x + math.sin(a) * r, y = _y + math.cos(a) * r})
	end
	end

	surface.SetDrawc_color(col.r, col.g, col.b, 255)
	draw.NoTexture()
	surface.DrawPoly(p)
	end
	*/

	// Notes: amount of vertexes is 4(v + 1) where v is the number of vertices on each corner bit.
	// I did it in lua cause I have no idea how the vertex_t struct works and i'm still aids at C++
}

bool Drawing::ScreenTransform(const c_vector3d& point, c_vector3d& screen) // tots not pasted
{
	float w;
	const VMatrix& worldToScreen = engine_client()->get_matrix();

	screen.x = worldToScreen[0][0] * point[0] + worldToScreen[0][1] * point[1] + worldToScreen[0][2] * point[2] + worldToScreen[0][3];
	screen.y = worldToScreen[1][0] * point[0] + worldToScreen[1][1] * point[1] + worldToScreen[1][2] * point[2] + worldToScreen[1][3];
	w = worldToScreen[3][0] * point[0] + worldToScreen[3][1] * point[1] + worldToScreen[3][2] * point[2] + worldToScreen[3][3];
	screen.z = 0.0f;

	bool behind = false;

	if (w < 0.001f)
	{
		behind = true;
		screen.x *= 100000;
		screen.y *= 100000;
	}
	else
	{
		behind = false;
		float invw = 1.0f / w;
		screen.x *= invw;
		screen.y *= invw;
	}

	return behind;
}

bool Drawing::WorldToScreen(const c_vector3d& origin, c_vector3d& screen)
{
	if (!ScreenTransform(origin, screen))
	{
		int ScreenWidth, ScreenHeight;
		engine_client()->get_screen_size(ScreenWidth, ScreenHeight);
		float x = ScreenWidth / 2;
		float y = ScreenHeight / 2;
		x += 0.5 * screen.x * ScreenWidth + 0.5;
		y -= 0.5 * screen.y * ScreenHeight + 0.5;
		screen.x = x;
		screen.y = y;
		return true;
	}

	return false;
}

RECT Drawing::GetViewport()
{
	RECT Viewport = { 0, 0, 0, 0 };
	int w, h;
	engine_client()->get_screen_size(w, h);
	Viewport.right = w; Viewport.bottom = h;
	return Viewport;
}

int Drawing::GetStringWidth(vgui::HFont font, const char* msg, ...)
{
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf_s(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int iWidth, iHeight;

	surface()->GetTextSize(font, wbuf, iWidth, iHeight);

	return iWidth;
}

RECT Drawing::GetTextSize(vgui::HFont font, const char* text)
{
	/*size_t origsize = strlen(text) + 1;
	const size_t newsize = 500;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	mbstowcs_s(&convertedChars, wcstring, origsize, text, _TRUNCATE);*/

	RECT rect;

	rect.left = rect.right = rect.bottom = rect.top = 0;

	wchar_t wbuf[1024];
	if (MultiByteToWideChar(CP_UTF8, 0, text, -1, wbuf, 256) > 0) {
		int x, y;
		surface()->GetTextSize(font, wbuf, x, y);
		rect.left = x; rect.bottom = y;
		rect.right = x; rect.top = y;
	}

	return rect;
}

void Drawing::Draw3DBox(c_vector3d* boxc_vector3ds, c_color color)
{
	c_vector3d boxc_vector3ds0, boxc_vector3ds1, boxc_vector3ds2, boxc_vector3ds3,
		boxc_vector3ds4, boxc_vector3ds5, boxc_vector3ds6, boxc_vector3ds7;


	if (Drawing::WorldToScreen(boxc_vector3ds[0], boxc_vector3ds0) &&
		Drawing::WorldToScreen(boxc_vector3ds[1], boxc_vector3ds1) &&
		Drawing::WorldToScreen(boxc_vector3ds[2], boxc_vector3ds2) &&
		Drawing::WorldToScreen(boxc_vector3ds[3], boxc_vector3ds3) &&
		Drawing::WorldToScreen(boxc_vector3ds[4], boxc_vector3ds4) &&
		Drawing::WorldToScreen(boxc_vector3ds[5], boxc_vector3ds5) &&
		Drawing::WorldToScreen(boxc_vector3ds[6], boxc_vector3ds6) &&
		Drawing::WorldToScreen(boxc_vector3ds[7], boxc_vector3ds7))
	{

		/*
		.+--5---+
		.8 4    6'|
		+--7---+'  11
		9   |  10  |
		|  ,+--|3--+
		|.0    | 2'
		+--1---+'
		*/

		c_vector2d lines[12][2];
		//bottom of box
		lines[0][0] = { boxc_vector3ds0.x, boxc_vector3ds0.y };
		lines[0][1] = { boxc_vector3ds1.x, boxc_vector3ds1.y };
		lines[1][0] = { boxc_vector3ds1.x, boxc_vector3ds1.y };
		lines[1][1] = { boxc_vector3ds2.x, boxc_vector3ds2.y };
		lines[2][0] = { boxc_vector3ds2.x, boxc_vector3ds2.y };
		lines[2][1] = { boxc_vector3ds3.x, boxc_vector3ds3.y };
		lines[3][0] = { boxc_vector3ds3.x, boxc_vector3ds3.y };
		lines[3][1] = { boxc_vector3ds0.x, boxc_vector3ds0.y };

		lines[4][0] = { boxc_vector3ds0.x, boxc_vector3ds0.y };
		lines[4][1] = { boxc_vector3ds6.x, boxc_vector3ds6.y };

		// top of box
		lines[5][0] = { boxc_vector3ds6.x, boxc_vector3ds6.y };
		lines[5][1] = { boxc_vector3ds5.x, boxc_vector3ds5.y };
		lines[6][0] = { boxc_vector3ds5.x, boxc_vector3ds5.y };
		lines[6][1] = { boxc_vector3ds4.x, boxc_vector3ds4.y };
		lines[7][0] = { boxc_vector3ds4.x, boxc_vector3ds4.y };
		lines[7][1] = { boxc_vector3ds7.x, boxc_vector3ds7.y };
		lines[8][0] = { boxc_vector3ds7.x, boxc_vector3ds7.y };
		lines[8][1] = { boxc_vector3ds6.x, boxc_vector3ds6.y };


		lines[9][0] = { boxc_vector3ds5.x, boxc_vector3ds5.y };
		lines[9][1] = { boxc_vector3ds1.x, boxc_vector3ds1.y };

		lines[10][0] = { boxc_vector3ds4.x, boxc_vector3ds4.y };
		lines[10][1] = { boxc_vector3ds2.x, boxc_vector3ds2.y };

		lines[11][0] = { boxc_vector3ds7.x, boxc_vector3ds7.y };
		lines[11][1] = { boxc_vector3ds3.x, boxc_vector3ds3.y };

		for (int i = 0; i < 12; i++)
		{
			Drawing::DrawLine(lines[i][0].x, lines[i][0].y, lines[i][1].x, lines[i][1].y, color);
		}
	}
}

void Drawing::rotate_point(c_vector2d& point, c_vector2d origin, bool clockwise, float angle) {
	c_vector2d delta = point - origin;
	c_vector2d rotated;

	if (clockwise) {
		rotated = c_vector2d(delta.x * cos(angle) - delta.y * sin(angle), delta.x * sin(angle) + delta.y * cos(angle));
	}
	else {
		rotated = c_vector2d(delta.x * sin(angle) - delta.y * cos(angle), delta.x * cos(angle) + delta.y * sin(angle));
	}

	point = rotated + origin;
}

void Drawing::DrawFilledCircle(int x, int y, int radius, int segments, c_color color) {
	std::vector< Vertex_t > vertices;
	float step = pi * 2.0f / segments;
	for (float a = 0; a < (pi * 2.0f); a += step)
		vertices.push_back(Vertex_t(c_vector2d(radius * cosf(a) + x, radius * sinf(a) + y)));

	TexturedPolygon(vertices.size(), vertices, color);
}

void Drawing::TexturedPolygon(int n, std::vector< Vertex_t > vertice, c_color color) {
	static int texture_id =surface()->CreateNewTextureID(true); // 
	static unsigned char buf[4] = { 255, 255, 255, 255 };
	surface()->DrawSetTextureRGBA(texture_id, buf, 1, 1); //
	surface()->DrawSetColor(color.red, color.green, color.blue, color.alpha); //
	surface()->DrawSetTexture(texture_id); //
	surface()->DrawTexturedPolygon(n, vertice.data()); //
}

void Drawing::TexturedPolygon(int n, Vertex_t* vertice, c_color color)
{
	static int texture_id = surface()->CreateNewTextureID(true); // 
	static unsigned char buf[4] = { 255, 255, 255, 255 };
	surface()->DrawSetTextureRGBA(texture_id, buf, 1, 1); //
	surface()->DrawSetColor(color.red, color.green, color.blue, color.alpha); //
	surface()->DrawSetTexture(texture_id); //
	surface()->DrawTexturedPolygon(n, vertice); //
}

void Drawing::filled_tilted_triangle(c_vector2d position, c_vector2d size, c_vector2d origin, bool clockwise, float angle, c_color color, bool rotate) {
	std::vector< Vertex_t > vertices =
	{
		Vertex_t{ c_vector2d(position.x - size.x, position.y + size.y), c_vector2d() },
		Vertex_t{ c_vector2d(position.x, position.y - size.y), c_vector2d() },
		Vertex_t{ position + size, c_vector2d() }
	};

	if (rotate) {
		for (unsigned int p = 0; p < vertices.size(); p++) {
			rotate_point(vertices[p].mPosition, origin, clockwise, angle);
		}
	}

	TexturedPolygon(vertices.size(), vertices, color);

	vertices.clear();
}


void Drawing::Triangle(c_vector2d one, c_vector2d two, c_vector2d three, c_color fill)
{
	static Vertex_t MouseVt[3];

	MouseVt[0].Init(one);
	MouseVt[1].Init(two);
	MouseVt[2].Init(three);

	TexturedPolygon(3, MouseVt, fill);
}


void Drawing::DrawCircle(float x, float y, float r, float s, c_color color)
{
	float Step = pi * 2.0 / s;
	for (float a = 0; a < (pi * 2.0); a += Step)
	{
		float x1 = r * cos(a) + x;
		float y1 = r * sin(a) + y;
		float x2 = r * cos(a + Step) + x;
		float y2 = r * sin(a + Step) + y;

		DrawLine(x1, y1, x2, y2, color);
	}
}

void c_vgui_panel::hook()
{
	static c_hook<IEngineVGui> hook(engine_vgui());
	_engine_vgui = hook.apply<engine_vgui_t>(14, EngineVGUI_Paint);
}

#define IS_IN_RANGE( value, max, min ) ( value >= max && value <= min )
#define GET_BITS( value ) ( IS_IN_RANGE( value, '0', '9' ) ? ( value - '0' ) : ( ( value & ( ~0x20 ) ) - 'A' + 0xA ) )
#define GET_BYTE( value ) ( GET_BITS( value[0] ) << 4 | GET_BITS( value[1] ) )

std::uintptr_t Scan(const std::string& image_name, const std::string& signature)
{
	auto image = GetModuleHandleA(image_name.c_str());

	if (!image)
	{
		return 0u;
	}

	auto image_base = (std::uintptr_t)(image);
	auto image_dos_hdr = (IMAGE_DOS_HEADER*)(image_base);

	if (image_dos_hdr->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return 0u;
	}

	auto image_nt_hdrs = (IMAGE_NT_HEADERS*)(image_base + image_dos_hdr->e_lfanew);

	if (image_nt_hdrs->Signature != IMAGE_NT_SIGNATURE)
	{
		return 0u;
	}

	auto scan_begin = (std::uint8_t*)(image_base);
	auto scan_end = (std::uint8_t*)(image_base + image_nt_hdrs->OptionalHeader.SizeOfImage);

	std::uint8_t* scan_result = nullptr;
	std::uint8_t* scan_data = (std::uint8_t*)(signature.c_str());

	for (auto current = scan_begin; current < scan_end; current++)
	{
		if (*(std::uint8_t*)scan_data == '\?' || *current == GET_BYTE(scan_data))
		{
			if (!scan_result)
				scan_result = current;

			if (!scan_data[2])
				return (std::uintptr_t)(scan_result);

			scan_data += (*(std::uint16_t*)scan_data == '\?\?' || *(std::uint8_t*)scan_data != '\?') ? 3 : 2;

			if (!*scan_data)
				return (std::uintptr_t)(scan_result);
		}
		else if (scan_result)
		{
			current = scan_result;
			scan_data = (std::uint8_t*)(signature.c_str());
			scan_result = nullptr;
		}
	}
	return 0u;
}


void __stdcall  c_vgui_panel::EngineVGUI_Paint(int mode)
{
	_engine_vgui(engine_vgui(), mode);

	typedef void(__thiscall* start_drawing)(void*);
	typedef void(__thiscall* finish_drawing)(void*);

	static start_drawing start_draw = (start_drawing)Scan("vguimatsurface.dll", "55 8B EC 83 E4 C0 83 EC 38");
	static finish_drawing end_draw = (finish_drawing)Scan("vguimatsurface.dll", "8B 0D ? ? ? ? 56 C6 05");

	int ScreenSize2W, ScreenSize2H;

	if (mode & 1)
	{
		start_draw(surface());

		static bool bResChange = false;

		engine_client()->get_screen_size(ScreenSize2W, ScreenSize2H);

		if (!bResChange && (ScreenSize2W != F::screen_size.x
			|| ScreenSize2H != F::screen_size.y))
		{
			F::screen_size.x = ScreenSize2W;
			F::screen_size.y = ScreenSize2H;
			bResChange = true;
		}

		if (bResChange)
			Drawing::CreateFonts();

		c_esp::render_player();

		if (bResChange)
			bResChange = false;

		end_draw(surface());
	}
}
