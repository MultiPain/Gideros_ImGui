#ifndef GCOLOR_H
#define GCOLOR_H

#include "imgui_src/imgui.h"

#define CC 0.0039215686274509803921568627451f

struct GColor {
	int hex; // 0xffffff
	double alpha; // [0..1]

	GColor()
	{
		hex = 0;
		alpha = 0;
	}

	GColor(ImU32 color)
	{
		GColor converted = GColor::toHex(color);
		hex = converted.hex;
		alpha = converted.alpha;
	}

	GColor(ImVec4 color)
	{
		GColor converted = GColor::toHex(color);
		hex = converted.hex;
		alpha = converted.alpha;
	}

	GColor(int _hex, double _alpha = 1.0)
	{
		hex = _hex;
		alpha = _alpha;
	}

	GColor(double _r, double _g, double _b, double _a = 1.0)
	{
		GColor converted = GColor::toHex(_r, _g, _b, _a);
		hex = converted.hex;
		alpha = converted.alpha;
	}

	static ImVec4 toVec4(int hex, double alpha = 1.0)
	{
		return ImVec4(
					((hex >> IM_COL32_B_SHIFT) & 0xFF) * CC,
					((hex >> IM_COL32_G_SHIFT) & 0xFF) * CC,
					((hex >> IM_COL32_R_SHIFT) & 0xFF) * CC,
					alpha);
	}

	static ImVec4 toVec4(GColor color)
	{
		return GColor::toVec4(color.hex, color.alpha);
	}

	static ImVec4 toVec4(ImU32 color)
	{
		GColor converted(color);
		return toVec4(converted);
	}

	static ImVec4 toVec4(lua_State* L, int idx, float defAlpha = 1.0f)
	{
		int hex = luaL_checkinteger(L, idx);
		float alpha = luaL_optnumber(L, idx + 1, defAlpha);
		return toVec4(hex, alpha);
	}

	static ImVec4 toVec4opt(lua_State* L, int idx, int defColor = 0xffffff, float defAlpha = 1.0f)
	{
		int hex = luaL_optinteger(L, idx, defColor);
		float alpha = luaL_optnumber(L, idx + 1, defAlpha);
		return toVec4(hex, alpha);
	}

	static GColor toHex(double _r, double _g, double _b, double _a = 1.0)
	{
		int r = _r * 255;
		int g = _g * 255;
		int b = _b * 255;

		int hex = (r << IM_COL32_B_SHIFT) + (g << IM_COL32_G_SHIFT) + (b << IM_COL32_R_SHIFT);

		return GColor(hex, _a);
	}

	static GColor toHex(ImVec4 color)
	{
		return GColor::toHex(color.x, color.y, color.z, color.w);
	}

	static GColor toHex(ImU32 color)
	{
		int a = color >> IM_COL32_A_SHIFT;
		int hex = color & ~IM_COL32_A_MASK;
		float alpha = a / 255.0f;
		return GColor(hex, alpha);
	}

	static ImU32 toU32(double _r, double _g, double _b, double _a = 1.0)
	{
		ImU32 r = _r * 255;
		ImU32 g = _g * 255;
		ImU32 b = _b * 255;
		ImU32 a = _a * 255;
		return ((a << IM_COL32_A_SHIFT) | (b << IM_COL32_B_SHIFT) | (g << IM_COL32_G_SHIFT) | (r << IM_COL32_R_SHIFT));
	}

	static ImU32 toU32(ImVec4 color)
	{
		return GColor::toU32(color.x, color.y, color.y, color.w);
	}

	static ImU32 toU32(int hex, double alpha = 1.0)
	{
		alpha *= 255.0f;
		ImU32 ghex = (int)alpha | hex << 8;

		ImU32 out =
				(((ghex << IM_COL32_R_SHIFT) & IM_COL32_A_MASK) >> IM_COL32_A_SHIFT) |
				(((ghex << IM_COL32_G_SHIFT) & IM_COL32_A_MASK) >> IM_COL32_B_SHIFT) |
				(((ghex << IM_COL32_B_SHIFT) & IM_COL32_A_MASK) >> IM_COL32_G_SHIFT) |
				(((ghex << IM_COL32_A_SHIFT) & IM_COL32_A_MASK) >> IM_COL32_R_SHIFT);
		return out;
	}

	static ImU32 toU32(GColor color)
	{
		return GColor::toU32(color.hex, color.alpha);
	}

	static ImU32 toU32(lua_State* L, int idx, float defAlpha = 1.0f)
	{
		int hex = luaL_checkinteger(L, idx);
		float alpha = luaL_optnumber(L, idx + 1, defAlpha);
		return toU32(hex, alpha);
	}

	static ImU32 toU32opt(lua_State* L, int idx, int defColor = 0xffffff, float defAlpha = 1.0f)
	{
		int hex = luaL_optinteger(L, idx, defColor);
		float alpha = luaL_optnumber(L, idx + 1, defAlpha);
		return toU32(hex, alpha);
	}
};


#endif // GCOLOR_H
