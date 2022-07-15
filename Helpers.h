#ifndef HELPERS_H
#define HELPERS_H

#define BIND_IENUM(L, value, name) lua_pushinteger(L, value); lua_setfield(L, -2, name);
#define BIND_FENUM(L, value, name) lua_pushnumber(L, value); lua_setfield(L, -2, name);

#define LUA_ASSERT(EXP, MSG) if (!(EXP)) { lua_pushstring(L, MSG); lua_error(L); }
#define LUA_ASSERTF(EXP, FMT, ...) if (!(EXP)) { lua_pushfstring(L, FMT, __VA_ARGS__); lua_error(L); }
#define LUA_THROW_ERROR(MSG) lua_pushstring(L, MSG); lua_error(L);
#define LUA_THROW_ERRORF(FMT, ...) lua_pushfstring(L, FMT, __VA_ARGS__); lua_error(L);
#define LUA_PRINTF(FMT, ...) lua_getglobal(L, "print"); lua_pushfstring(L, FMT, __VA_ARGS__); lua_call(L, 1, 0);
#define LUA_PRINT(MSG) lua_getglobal(L, "print"); lua_pushstring(L, MSG); lua_call(L, 1, 0);

#include "gplugin.h"
#include "imgui_src/imgui.h"
#include "imgui_src/imgui_internal.h"
#include "lualib.h"

////////////////////////////////////////////////////////////////////////////////
///
/// DEBUG TOOL
///
////////////////////////////////////////////////////////////////////////////////

static int DUMP_INDEX = 0;

static void stackDump(lua_State* L, const char* prefix = "")
{
	int i = lua_gettop(L);
	LUA_PRINTF("----------------      %d      ----------------\n>%s\n----------------  Stack Dump ----------------", DUMP_INDEX, prefix);
	while (i)
	{
		int t = lua_type(L, i);
		switch (t)
		{
		case LUA_TSTRING:
			{
				LUA_PRINTF("[S] %d:'%s'", i, lua_tostring(L, i));
			}
			break;
		case LUA_TBOOLEAN:
			{
				LUA_PRINTF("[B] %d: %s", i, lua_toboolean(L, i) ? "true" : "false");
			}
			break;
		case LUA_TNUMBER:
			{
				LUA_PRINTF("[N] %d: %f", i, lua_tonumber(L, i));
			}
			break;
#ifdef LUA_IS_LUAU
		case LUA_TVECTOR:
			{
				const float* v = lua_tovector(L, i);
				LUA_PRINTF("[V] %d: [%f; %f; %f; %f]", i, v[0], v[1], v[2], v[3]);
			}
			break;
#endif
		default:
			{
				LUA_PRINTF("[D] %d: %s", i, lua_typename(L, t));
			}
			break;
		}
		i--;
	}
	LUA_PRINT("------------ Stack Dump Finished ------------\n");

	DUMP_INDEX++;
}

////////////////////////////////////////////////////////////////////////////////
///
/// TEMPLATES
///
////////////////////////////////////////////////////////////////////////////////

template<class T>
inline T* getPtr(lua_State* L, const char* name, int idx = 1)
{
	return static_cast<T*>(g_getInstance(L, name, idx));
}

template <typename T>
T* getTableValues(lua_State* L, int idx, unsigned int len)
{
	T* values = new T[len];
	lua_pushvalue(L, idx);
	for (unsigned int i = 0; i < len; i++)
	{
		lua_rawgeti(L, idx, i+1);

		T v = luaL_checknumber(L, -1);
		values[i] = v;
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
	return values;
}

template <typename T>
T* getTableValues(lua_State* L, int idx)
{
	unsigned int len = lua_objlen(L, idx);
	T* values = getTableValues<T>(L, idx, len);
	return values;
}

template<typename T>
inline void destroyObject(void* p)
{
	void* data = GIDEROS_DTOR_UDATA(p);
	T* ptr = static_cast<T*>(data);
	delete ptr;
}

template<typename T>
int LuaDragScalarN(lua_State* L, ImGuiDataType data_type, const int n, const char* defFormat, int argsOffset = 0)
{
	int valueIndex = 3 + argsOffset;
	int stackOffset = lua_type(L, valueIndex) == LUA_TVECTOR ? 1 : n;
	stackOffset += argsOffset;

	const char* label = luaL_checkstring(L, 2 + argsOffset);
	T v_speed = luaL_optnumber(L, 3 + stackOffset, 1.0f);
	T v_min = luaL_optnumber(L, 4 + stackOffset, 0.0f);
	T v_max = luaL_optnumber(L, 5 + stackOffset, 0.0f);
	const char* format = luaL_optstring(L, 6 + stackOffset, defFormat);
	ImGuiSliderFlags sliderFlags = luaL_optinteger(L, 7 + stackOffset, 0);

	if (lua_type(L, valueIndex) == LUA_TVECTOR && n <= LUA_VECTOR_SIZE)
	{
		const float* fvec = lua_tovector(L, valueIndex);
		T vec[LUA_VECTOR_SIZE];
		for (int i = 0; i < LUA_VECTOR_SIZE; i++)
		{
			vec[i] = (T)fvec[i];
		}

		bool result = ImGui::DragScalarN(label, data_type, vec, n, v_speed, &v_min, &v_max, format, sliderFlags);
#if LUA_VECTOR_SIZE == 4
		lua_pushvector(L, (float)(vec[0]), (float)(vec[1]), (float)(vec[2]), (float)(vec[3]));
#else
		lua_pushvector(L, (float)(vec[0]), (float)(vec[1]), (float)(vec[2]));
#endif
		lua_pushboolean(L, result);
		return 2;
	}
	else
	{
		T vec[n];
		for (int i = 0; i < n; i++)
		{
			vec[i] = luaL_checknumber(L, valueIndex + i);
		}

		bool result = ImGui::DragScalarN(label, data_type, vec, n, v_speed, &v_min, &v_max, format, sliderFlags);

		for (int i = 0; i < n; i++)
		{
			lua_pushnumber(L, vec[i]);
		}

		lua_pushboolean(L, result);
		return n + 1;
	}
}

template<typename T>
int LuaSliderScalarN(lua_State* L, ImGuiDataType data_type, const int n, const char* defFormat, int argsOffset = 0)
{
	int valueIndex = 3 + argsOffset;
	int stackOffset = lua_type(L, valueIndex) == LUA_TVECTOR ? 1 : n;
	stackOffset += argsOffset;

	const char* label = luaL_checkstring(L, 2 + argsOffset);
	T v_min = luaL_checknumber(L, 3 + stackOffset);
	T v_max = luaL_checknumber(L, 4 + stackOffset);
	const char* format = luaL_optstring(L, 5 + stackOffset, defFormat);
	ImGuiSliderFlags sliderFlag = luaL_optinteger(L, 6 + stackOffset, 0);

	if (lua_type(L, valueIndex) == LUA_TVECTOR && n <= LUA_VECTOR_SIZE)
	{
		const float* fvec = lua_tovector(L, valueIndex);
		T vec[LUA_VECTOR_SIZE];
		for (int i = 0; i < LUA_VECTOR_SIZE; i++)
		{
			vec[i] = (T)fvec[i];
		}

		bool result = ImGui::SliderScalarN(label, data_type, vec, n, &v_min, &v_max, format, sliderFlag);
#if LUA_VECTOR_SIZE == 4
		lua_pushvector(L, (float)(vec[0]), (float)(vec[1]), (float)(vec[2]), (float)(vec[3]));
#else
		lua_pushvector(L, (float)(vec[0]), (float)(vec[1]), (float)(vec[2]));
#endif
		lua_pushboolean(L, result);
		return 2;
	}
	else
	{
		T vec[n];
		for (int i = 0; i < n; i++)
		{
			vec[i] = luaL_checknumber(L, valueIndex + i);
		}
		bool result = ImGui::SliderScalarN(label, data_type, vec, n, &v_min, &v_max, format, sliderFlag);

		for (int i = 0; i < n; i++)
		{
			lua_pushnumber(L, vec[i]);
		}
		lua_pushboolean(L, result);
		return n + 1;
	}
}

template<typename T>
int LuaFilledSliderScalarN(lua_State* L, ImGuiDataType data_type, const int n, const char* defFormat, int argsOffset = 0)
{
	int valueIndex = 4 + argsOffset;
	int stackOffset = lua_type(L, valueIndex) == LUA_TVECTOR ? 1 : n;
	stackOffset += argsOffset;

	const char* label = luaL_checkstring(L, 2 + argsOffset);
	bool mirror = lua_toboolean(L, 3 + argsOffset) > 0;
	T v_min = luaL_checknumber(L, 4 + stackOffset);
	T v_max = luaL_checknumber(L, 5 + stackOffset);
	const char* format = luaL_optstring(L, 6 + stackOffset, defFormat);
	ImGuiSliderFlags sliderFlag = luaL_optinteger(L, 7 + stackOffset, 0);


	if (lua_type(L, valueIndex) == LUA_TVECTOR && n <= LUA_VECTOR_SIZE)
	{
		const float* fvec = lua_tovector(L, valueIndex);
		T vec[LUA_VECTOR_SIZE];
		for (int i = 0; i < LUA_VECTOR_SIZE; i++)
		{
			vec[i] = (T)fvec[i];
		}
		bool result = ImGui::FilledSliderScalarN(label, mirror, data_type, vec, n, &v_min, &v_max, format, sliderFlag);
#if LUA_VECTOR_SIZE == 4
		lua_pushvector(L, (float)(vec[0]), (float)(vec[1]), (float)(vec[2]), (float)(vec[3]));
#else
		lua_pushvector(L, (float)(vec[0]), (float)(vec[1]), (float)(vec[2]));
#endif
		lua_pushboolean(L, result);
		return 2;
	}
	else
	{
		T vec[n];
		for (int i = 0; i < n; i++)
		{
			vec[i] = luaL_checknumber(L, valueIndex + i);
		}

		bool result = ImGui::FilledSliderScalarN(label, mirror, data_type, vec, n, &v_min, &v_max, format, sliderFlag);

		for (int i = 0; i < n; i++)
		{
			lua_pushnumber(L, vec[i]);
		}
		lua_pushboolean(L, result);
		return n + 1;
	}

}

template<typename T>
int LuaInputScalarN(lua_State* L, ImGuiDataType data_type, const int n, const char* defFormat, int argsOffset = 0)
{
	int valueIndex = 3 + argsOffset;
	int stackOffset = lua_type(L, valueIndex) == LUA_TVECTOR ? 1 : n;
	stackOffset += argsOffset;

	const char* label = luaL_checkstring(L, 2 + argsOffset);
	const char* format = luaL_optstring(L, 3 + stackOffset, defFormat);
	T step = luaL_optnumber(L, 4 + stackOffset, 0);
	T step_fast = luaL_optnumber(L, 5 + stackOffset, 0);
	ImGuiInputTextFlags flags = luaL_optinteger(L, 6 + stackOffset, 0);

	if (lua_type(L, valueIndex) == LUA_TVECTOR && n <= LUA_VECTOR_SIZE)
	{
		const float* fvec = lua_tovector(L, valueIndex);
		T vec[LUA_VECTOR_SIZE];
		for (int i = 0; i < LUA_VECTOR_SIZE; i++)
		{
			vec[i] = (T)fvec[i];
		}
		bool result = ImGui::InputScalarN(label, data_type, vec, n, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step : NULL), format, flags);
#if LUA_VECTOR_SIZE == 4
		lua_pushvector(L, (float)(vec[0]), (float)(vec[1]), (float)(vec[2]), (float)(vec[3]));
#else
		lua_pushvector(L, (float)(vec[0]), (float)(vec[1]), (float)(vec[2]));
#endif
		lua_pushboolean(L, result);
		return 2;
	}
	else
	{
		T vec[n];
		for (int i = 0; i < n; i++)
		{
			vec[i] = luaL_checknumber(L, valueIndex + i);
		}

		bool result = ImGui::InputScalarN(label, data_type, vec, n, &step, &step_fast, format, flags);

		for (int i = 0; i < n; i++)
		{
			lua_pushnumber(L, vec[i]);
		}
		lua_pushboolean(L, result);
		return n + 1;
	}
}

static const ImVec2 luaL_checkvec2(lua_State* L, int idx)
{
	float x = luaL_checknumber(L, idx);
	float y = luaL_checknumber(L, idx + 1);

	return ImVec2(x, y);
}

static const ImVec2 luaL_optvec2(lua_State* L, int idx, float defX = 0.0f, float defY = 0.0f)
{
	float x = luaL_optnumber(L, idx, defX);
	float y = luaL_optnumber(L, idx + 1, defY);

	return ImVec2(x, y);
}

static const ImVec4 luaL_checkvec4(lua_State* L, int idx)
{
	float x = luaL_checknumber(L, idx);
	float y = luaL_checknumber(L, idx + 1);
	float z = luaL_checknumber(L, idx + 2);
	float w = luaL_checknumber(L, idx + 3);

	return ImVec4(x, y, z, w);
}

static const ImVec4 luaL_optvec4(lua_State* L, int idx, float defX = 0.0f, float defY = 0.0f, float defZ = 0.0f, float defW = 0.0f)
{
	float x = luaL_optnumber(L, idx, defX);
	float y = luaL_optnumber(L, idx + 1, defY);
	float z = luaL_optnumber(L, idx + 2, defZ);
	float w = luaL_optnumber(L, idx + 3, defW);

	return ImVec4(x, y, z, w);
}

static ImRect luaL_checkrect(lua_State* L, int idx)
{
	float x1 = luaL_checknumber(L, idx + 0);
	float y1 = luaL_checknumber(L, idx + 1);
	float x2 = luaL_checknumber(L, idx + 2);
	float y2 = luaL_checknumber(L, idx + 3);

	return ImRect(x1, y1, x2, y2);
}

static ImRect luaL_optrect(lua_State* L, int idx, float defX1 = 0.0f, float defY1 = 0.0f, float defX2 = 0.0f, float defY2 = 0.0f)
{
	float x1 = luaL_optnumber(L, idx + 0, defX1);
	float y1 = luaL_optnumber(L, idx + 1, defY1);
	float x2 = luaL_optnumber(L, idx + 2, defX2);
	float y2 = luaL_optnumber(L, idx + 3, defY2);

	return ImRect(x1, y1, x2, y2);
}

static inline void lua_pushvec2(lua_State* L, ImVec2 vec)
{
	lua_pushnumber(L, vec.x);
	lua_pushnumber(L, vec.y);
}

static inline void lua_pushvec4(lua_State* L, ImVec4 vec)
{
	lua_pushnumber(L, vec.x);
	lua_pushnumber(L, vec.y);
	lua_pushnumber(L, vec.z);
	lua_pushnumber(L, vec.w);
}

static inline void lua_pushrect(lua_State* L, ImRect& rect)
{
	lua_pushnumber(L, rect.Min.x);
	lua_pushnumber(L, rect.Min.y);
	lua_pushnumber(L, rect.Max.x);
	lua_pushnumber(L, rect.Max.y);
}

#endif // HELPERS_H
