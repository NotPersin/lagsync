#pragma once

#include "macros.h"

class OverlayText_t;

class c_debug_overlay
{
protected:
	~c_debug_overlay() = default;
public:
	virtual void            AddEntityTextOverlay(int ent_index, int line_offset, float duration, int r, int g, int b, int a, const char* format, ...) = 0;
	virtual void            AddBoxOverlay(const c_vector3d& origin, const c_vector3d& mins, const c_vector3d& max, c_vector3d const& orientation, int r, int g, int b, int a, float duration) = 0;
	virtual void            AddSphereOverlay(const c_vector3d& vOrigin, float flRadius, int nTheta, int nPhi, int r, int g, int b, int a, float flDuration) = 0;
	virtual void            AddTriangleOverlay(const c_vector3d& p1, const c_vector3d& p2, const c_vector3d& p3, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void            AddLineOverlay(const c_vector3d& origin, const c_vector3d& dest, int r, int g, int b, bool noDepthTest, float duration) = 0;
	virtual void            AddTextOverlay(const c_vector3d& origin, float duration, const char* format, ...) = 0;
	virtual void            AddTextOverlay(const c_vector3d& origin, int line_offset, float duration, const char* format, ...) = 0;
	virtual void            AddScreenTextOverlay(float flXPos, float flYPos, float flDuration, int r, int g, int b, int a, const char* text) = 0;
	virtual void            AddSweptBoxOverlay(const c_vector3d& start, const c_vector3d& end, const c_vector3d& mins, const c_vector3d& max, const c_vector3d& angles, int r, int g, int b, int a, float flDuration) = 0;
	virtual void            AddGridOverlay(const c_vector3d& origin) = 0;
	virtual void            AddCoordFrameOverlay(const matrix3x4t& frame, float flScale, int vColorTable[3][3] = NULL) = 0;
	virtual int             ScreenPosition(const c_vector3d& point, c_vector3d& screen) = 0;
	virtual int             ScreenPosition(float flXPos, float flYPos, c_vector3d& screen) = 0;
	virtual OverlayText_t* GetFirst(void) = 0;
	virtual OverlayText_t* GetNext(OverlayText_t* current) = 0;
	virtual void            ClearDeadOverlays(void) = 0;
	virtual void            ClearAllOverlays(void) = 0;
	virtual void            AddTextOverlayRGB(const c_vector3d& origin, int line_offset, float duration, float r, float g, float b, float alpha, const char* format, ...) = 0;
	virtual void            AddTextOverlayRGB(const c_vector3d& origin, int line_offset, float duration, int r, int g, int b, int a, const char* format, ...) = 0;
	virtual void            AddLineOverlayAlpha(const c_vector3d& origin, const c_vector3d& dest, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void            AddBoxOverlay2(const c_vector3d& origin, const c_vector3d& mins, const c_vector3d& max, c_vector3d const& orientation, const uint8_t* faceColor, const uint8_t* edgeColor, float duration) = 0;
	virtual void			AddLineOverlay(const c_vector3d& origin, const c_vector3d& dest, int r, int g, int b, int a, float, float) = 0;
	virtual void            PurgeTextOverlays() = 0;

	vfunc(24, add_capsule_overlay(c_vector3d& mins, c_vector3d& maxs, float radius, const int r,
		const int g, const int b, const int a, const float duration), void(__thiscall*)(
		c_debug_overlay*, c_vector3d&, c_vector3d&, float&, int, int, int, int, float))(mins, maxs, radius, r, g, b, a, duration)
};

interface_var(c_debug_overlay, debug_overlay, "engine.dll", "VDebugOverlay")
