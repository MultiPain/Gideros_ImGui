// TODO
// tables instead of vectors?
// Nav settings
// Remove "CallbackData" vecotr?

#define _UNUSED(n)
//#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"

#include "lua.hpp"
#include "luautil.h"

#include "gplugin.h"
#include "gfile.h"
#include "gstdio.h"
#include "ginput.h"
#include "application.h"
#include "luaapplication.h"

#include "mouseevent.h"
#include "keyboardevent.h"
#include "touchevent.h"

#include "sprite.h"
#include "texturebase.h"
#include "bitmapdata.h"
#include "bitmap.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// To compile for old version :)
#ifndef LUA_IS_LUAU
#define LUA_STATE lua_State
#else
#define LUA_STATE void
#define luaL_getn lua_objlen
#endif

#ifndef GIDEROS_DTOR_UDATA
#define GIDEROS_DTOR_UDATA(p) (*(void**)LUA_DTOR_UDATA(p))
#endif
#include "imgui_src/imgui.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "imgui_src/imgui_internal.h"
#include "TextEditor.h" // https://github.com/BalazsJako/ImGuiColorTextEdit

#include "GColor.h"
#include "Helpers.h"
#include "impl_ImGui_nodes.h"


#ifdef IS_DOCKING_BUILD
#define PLUGIN_NAME "ImGui_docking"
#elif defined(IS_BETA_BUILD)
#define PLUGIN_NAME "ImGui_beta"
#include "stackchecker.h"
#define STACK_CHECKER(L, pre, delta) StackChecker checker(L, pre, delta);
#else
#define PLUGIN_NAME "ImGui"
#define STACK_CHECKER(L, pre, delta) ((void)0)
#endif

static lua_State* L;
static Application* application;
static char keyWeak = ' ';
static const char* touchTypes[4]{"finger","pen","mouse","penTablet"};

////////////////////////////////////////////////////////////////////////////////
///
/// TEXTURES / COLORS
///
////////////////////////////////////////////////////////////////////////////////

struct GTextureData
{
	void* texture;
	ImVec2 texture_size;
	ImVec2 ex_size;
	ImVec2 uv0;
	ImVec2 uv1;

	GTextureData(lua_State* L, int idx = 1)
	{
		if (g_isInstanceOf(L, "TextureBase", idx))
		{
			TextureBase* textureBase = getPtr<TextureBase>(L, "TextureBase", idx);

			TextureData* gdata = textureBase->data;

			texture_size.x = (float)gdata->width;
			texture_size.y = (float)gdata->height;
			ex_size = ImVec2((float)gdata->exwidth, (float)gdata->exheight);
			texture = (void*)gdata->gid;
			uv0.x = 0.0f;
			uv0.y = 0.0f;
			uv1 = texture_size / ex_size;
		}
		else if (g_isInstanceOf(L, "TextureRegion", idx))
		{
			BitmapData* bitmapData = getPtr<BitmapData>(L, "TextureRegion", idx);

			TextureData* gdata = bitmapData->texture()->data;

			int x, y, w, h;
			bitmapData->getRegion(&x, &y, &w, &h, 0, 0, 0, 0);
			texture_size.x = (float)w;
			texture_size.y = (float)h;
			float fx = (float)x;
			float fy = (float)y;
			ex_size = ImVec2((float)gdata->exwidth, (float)gdata->exheight);
			uv0.x = fx / ex_size.x;
			uv0.y = fy / ex_size.y;
			uv1.x = (fx + texture_size.x) / ex_size.x;
			uv1.y = (fy + texture_size.y) / ex_size.y;
			texture = (void*)gdata->gid;
		}
		else
		{
			luaL_typerror(L, idx, "TextureBase or TextureRegion");
		}
	}
};

struct VColor
{
	uint8_t r,g,b,a;
};

////////////////////////////////////////////////////////////////////////////////
///
/// CallbackData
///
////////////////////////////////////////////////////////////////////////////////

class CallbackData
{
public:
	int functionIndex = -1;
	int argumentIndex = -1;
	int flags = 0;

	lua_State* L;

	CallbackData(lua_State* p_L, int index)
	{
		L = p_L;

		if (lua_gettop(L) == index + 1)
		{
			argumentIndex = luaL_ref(L, LUA_REGISTRYINDEX);
		}

		functionIndex = luaL_ref(L, LUA_REGISTRYINDEX);
	}

	~CallbackData()
	{
		if (functionIndex != -1)
		{
			luaL_unref(L, LUA_REGISTRYINDEX, functionIndex);
		}

		if (argumentIndex != -1)
		{
			luaL_unref(L, LUA_REGISTRYINDEX, argumentIndex);
		}
	}
};

////////////////////////////////////////////////////////////////////////////////
///
/// HELPERS
///
////////////////////////////////////////////////////////////////////////////////

static void localToGlobal(SpriteProxy* proxy, float x, float y, float* tx, float* ty)
{
	const Sprite* curr = proxy;

	float z;
	while (curr) {
		curr->matrix().transformPoint(x, y, 0, &x, &y, &z);
		curr = curr->parent();
	}

	if (tx)
		*tx = x;

	if (ty)
		*ty = y;
}

static ImGuiMouseButton giderosMouseToImGui(const int button)
{
	switch (button)
	{
	case GINPUT_LEFT_BUTTON:
		return ImGuiMouseButton_Left;
	case GINPUT_RIGHT_BUTTON:
		return ImGuiMouseButton_Right;
	case GINPUT_MIDDLE_BUTTON:
		return ImGuiMouseButton_Middle;
	case 8:
		return 3;
	case 16:
		return 4;
	default:
		return -1;
	}
}

static ImGuiKey giderosKeyToImGuiKey(const int key)
{
	switch (key)
	{
		case GINPUT_KEY_SHIFT:		return ImGuiKey_ModShift;
		case GINPUT_KEY_CTRL:		return ImGuiKey_ModCtrl;
		case GINPUT_KEY_ALT:		return ImGuiKey_ModAlt;
		case GINPUT_KEY_BACKSPACE:	return ImGuiKey_Backspace;
		case GINPUT_KEY_TAB:		return ImGuiKey_Tab;
		case GINPUT_KEY_ENTER:		return ImGuiKey_Enter;
		case GINPUT_KEY_ESC:		return ImGuiKey_Escape;
		case GINPUT_KEY_SPACE:		return ImGuiKey_Space;
		case GINPUT_KEY_LEFT:		return ImGuiKey_LeftArrow;
		case GINPUT_KEY_UP:		return ImGuiKey_UpArrow;
		case GINPUT_KEY_RIGHT:		return ImGuiKey_RightArrow;
		case GINPUT_KEY_DOWN:		return ImGuiKey_DownArrow;
		case GINPUT_KEY_0:		return ImGuiKey_0;
		case GINPUT_KEY_1:		return ImGuiKey_1;
		case GINPUT_KEY_2:		return ImGuiKey_2;
		case GINPUT_KEY_3:		return ImGuiKey_3;
		case GINPUT_KEY_4:		return ImGuiKey_4;
		case GINPUT_KEY_5:		return ImGuiKey_5;
		case GINPUT_KEY_6:		return ImGuiKey_6;
		case GINPUT_KEY_7:		return ImGuiKey_7;
		case GINPUT_KEY_8:		return ImGuiKey_8;
		case GINPUT_KEY_9:		return ImGuiKey_9;
		case GINPUT_KEY_A:		return ImGuiKey_A;
		case GINPUT_KEY_B:		return ImGuiKey_B;
		case GINPUT_KEY_C:		return ImGuiKey_C;
		case GINPUT_KEY_D:		return ImGuiKey_D;
		case GINPUT_KEY_E:		return ImGuiKey_E;
		case GINPUT_KEY_F:		return ImGuiKey_F;
		case GINPUT_KEY_G:		return ImGuiKey_G;
		case GINPUT_KEY_H:		return ImGuiKey_H;
		case GINPUT_KEY_I:		return ImGuiKey_I;
		case GINPUT_KEY_J:		return ImGuiKey_J;
		case GINPUT_KEY_K:		return ImGuiKey_K;
		case GINPUT_KEY_L:		return ImGuiKey_L;
		case GINPUT_KEY_M:		return ImGuiKey_M;
		case GINPUT_KEY_N:		return ImGuiKey_N;
		case GINPUT_KEY_O:		return ImGuiKey_O;
		case GINPUT_KEY_P:		return ImGuiKey_P;
		case GINPUT_KEY_Q:		return ImGuiKey_Q;
		case GINPUT_KEY_R:		return ImGuiKey_R;
		case GINPUT_KEY_S:		return ImGuiKey_S;
		case GINPUT_KEY_T:		return ImGuiKey_T;
		case GINPUT_KEY_U:		return ImGuiKey_U;
		case GINPUT_KEY_V:		return ImGuiKey_V;
		case GINPUT_KEY_W:		return ImGuiKey_W;
		case GINPUT_KEY_X:		return ImGuiKey_X;
		case GINPUT_KEY_Y:		return ImGuiKey_Y;
		case GINPUT_KEY_Z:		return ImGuiKey_Z;
//		case GINPUT_KEY_BACK:		return ImGuiKey_;
//		case GINPUT_KEY_SEARCH:		return ImGuiKey_;
//		case GINPUT_KEY_MENU:		return ImGuiKey_;
//		case GINPUT_KEY_CENTER:		return ImGuiKey_;
//		case GINPUT_KEY_SELECT:		return ImGuiKey_;
//		case GINPUT_KEY_START:		return ImGuiKey_;
//		case GINPUT_KEY_L1:		return ImGuiKey_;
//		case GINPUT_KEY_R1:		return ImGuiKey_;
		case GINPUT_KEY_HOME:		return ImGuiKey_Home;
		case GINPUT_KEY_END:		return ImGuiKey_End;
		case GINPUT_KEY_INSERT:		return ImGuiKey_Insert;
		case GINPUT_KEY_DELETE:		return ImGuiKey_Delete;
		case GINPUT_KEY_PAGEUP:		return ImGuiKey_PageUp;
		case GINPUT_KEY_PAGEDOWN:	return ImGuiKey_PageDown;
		case GINPUT_KEY_F1:		return ImGuiKey_F1;
		case GINPUT_KEY_F2:		return ImGuiKey_F2;
		case GINPUT_KEY_F3:		return ImGuiKey_F3;
		case GINPUT_KEY_F4:		return ImGuiKey_F4;
		case GINPUT_KEY_F5:		return ImGuiKey_F5;
		case GINPUT_KEY_F6:		return ImGuiKey_F6;
		case GINPUT_KEY_F7:		return ImGuiKey_F7;
		case GINPUT_KEY_F8:		return ImGuiKey_F8;
		case GINPUT_KEY_F9:		return ImGuiKey_F9;
		case GINPUT_KEY_F10:		return ImGuiKey_F10;
		case GINPUT_KEY_F11:		return ImGuiKey_F11;
		case GINPUT_KEY_F12:		return ImGuiKey_F12;
		default:			return ImGuiKey_None;
	}
}

static ImGuiKey giderosModKeyToImGuiMod(const int key)
{
	switch (key)
	{
		case GINPUT_ALT_MODIFIER:	return ImGuiKey_ModAlt;
		case GINPUT_SHIFT_MODIFIER:	return ImGuiKey_ModShift;
		case GINPUT_CTRL_MODIFIER:	return ImGuiKey_ModCtrl;
		case GINPUT_META_MODIFIER:	return ImGuiKey_ModSuper;
		default:					return ImGuiKey_None;
	}
}

static int imGuiKeyToGideros(const int key)
{
	switch (key)
	{
		case ImGuiKey_ModShift:		return GINPUT_KEY_SHIFT;
		case ImGuiKey_ModCtrl:		return GINPUT_KEY_CTRL;
		case ImGuiKey_ModAlt:		return GINPUT_KEY_ALT;
		case ImGuiKey_Backspace:	return GINPUT_KEY_BACKSPACE;
		case ImGuiKey_Tab:		return GINPUT_KEY_TAB;
		case ImGuiKey_Enter:		return GINPUT_KEY_ENTER;
		case ImGuiKey_Escape:		return GINPUT_KEY_ESC;
		case ImGuiKey_Space:		return GINPUT_KEY_SPACE;
		case ImGuiKey_LeftArrow:	return GINPUT_KEY_LEFT;
		case ImGuiKey_UpArrow:		return GINPUT_KEY_UP;
		case ImGuiKey_RightArrow:	return GINPUT_KEY_RIGHT;
		case ImGuiKey_DownArrow:	return GINPUT_KEY_DOWN;
		case ImGuiKey_0:		return GINPUT_KEY_0;
		case ImGuiKey_1:		return GINPUT_KEY_1;
		case ImGuiKey_2:		return GINPUT_KEY_2;
		case ImGuiKey_3:		return GINPUT_KEY_3;
		case ImGuiKey_4:		return GINPUT_KEY_4;
		case ImGuiKey_5:		return GINPUT_KEY_5;
		case ImGuiKey_6:		return GINPUT_KEY_6;
		case ImGuiKey_7:		return GINPUT_KEY_7;
		case ImGuiKey_8:		return GINPUT_KEY_8;
		case ImGuiKey_9:		return GINPUT_KEY_9;
		case ImGuiKey_A:		return GINPUT_KEY_A;
		case ImGuiKey_B:		return GINPUT_KEY_B;
		case ImGuiKey_C:		return GINPUT_KEY_C;
		case ImGuiKey_D:		return GINPUT_KEY_D;
		case ImGuiKey_E:		return GINPUT_KEY_E;
		case ImGuiKey_F:		return GINPUT_KEY_F;
		case ImGuiKey_G:		return GINPUT_KEY_G;
		case ImGuiKey_H:		return GINPUT_KEY_H;
		case ImGuiKey_I:		return GINPUT_KEY_I;
		case ImGuiKey_J:		return GINPUT_KEY_J;
		case ImGuiKey_K:		return GINPUT_KEY_K;
		case ImGuiKey_L:		return GINPUT_KEY_L;
		case ImGuiKey_M:		return GINPUT_KEY_M;
		case ImGuiKey_N:		return GINPUT_KEY_N;
		case ImGuiKey_O:		return GINPUT_KEY_O;
		case ImGuiKey_P:		return GINPUT_KEY_P;
		case ImGuiKey_Q:		return GINPUT_KEY_Q;
		case ImGuiKey_R:		return GINPUT_KEY_R;
		case ImGuiKey_S:		return GINPUT_KEY_S;
		case ImGuiKey_T:		return GINPUT_KEY_T;
		case ImGuiKey_U:		return GINPUT_KEY_U;
		case ImGuiKey_V:		return GINPUT_KEY_V;
		case ImGuiKey_W:		return GINPUT_KEY_W;
		case ImGuiKey_X:		return GINPUT_KEY_X;
		case ImGuiKey_Y:		return GINPUT_KEY_Y;
		case ImGuiKey_Z:		return GINPUT_KEY_Z;
		//case ImGuiKey_:		return GINPUT_KEY_BACK;
		//case ImGuiKey_:		return GINPUT_KEY_SEARCH;
		//case ImGuiKey_:		return GINPUT_KEY_MENU;
		//case ImGuiKey_:		return GINPUT_KEY_CENTER;
		//case ImGuiKey_:		return GINPUT_KEY_SELECT;
		//case ImGuiKey_:		return GINPUT_KEY_START;
		//case ImGuiKey_:		return GINPUT_KEY_L1;
		//case ImGuiKey_:		return GINPUT_KEY_R1;
		case ImGuiKey_Home:		return GINPUT_KEY_HOME;
		case ImGuiKey_End:		return GINPUT_KEY_END;
		case ImGuiKey_Insert:		return GINPUT_KEY_INSERT;
		case ImGuiKey_Delete:		return GINPUT_KEY_DELETE;
		case ImGuiKey_PageUp:		return GINPUT_KEY_PAGEUP;
		case ImGuiKey_PageDown:		return GINPUT_KEY_PAGEDOWN;
		case ImGuiKey_F1:		return GINPUT_KEY_F1;
		case ImGuiKey_F2:		return GINPUT_KEY_F2;
		case ImGuiKey_F3:		return GINPUT_KEY_F3;
		case ImGuiKey_F4:		return GINPUT_KEY_F4;
		case ImGuiKey_F5:		return GINPUT_KEY_F5;
		case ImGuiKey_F6:		return GINPUT_KEY_F6;
		case ImGuiKey_F7:		return GINPUT_KEY_F7;
		case ImGuiKey_F8:		return GINPUT_KEY_F8;
		case ImGuiKey_F9:		return GINPUT_KEY_F9;
		case ImGuiKey_F10:		return GINPUT_KEY_F10;
		case ImGuiKey_F11:		return GINPUT_KEY_F11;
		case ImGuiKey_F12:		return GINPUT_KEY_F12;
		default: 			return 0;
	}
}

static lua_Number getAppProperty(lua_State *L, const char* name)
{
	lua_getglobal(L, "application");
	lua_getfield(L, -1, name);
	lua_pushvalue(L, -2);
	lua_call(L, 1, 1);
	lua_Number value = lua_tonumber(L, -1);
	lua_remove(L, -1);
	return value;
}

static void setApplicationCursor(lua_State* L, const char* name)
{
	lua_getglobal(L, "application");
	lua_getfield(L, -1, "set");
	lua_pushvalue(L, -2);
	lua_pushstring(L, "cursor");
	lua_pushstring(L, name);
	lua_call(L, 3, 0);
	lua_pop(L, 2);
}


#ifndef LUA_IS_LUAU
static int luaL_optboolean(lua_State* L, int narg, int def)
{
	return lua_isboolean(L, narg) ? lua_toboolean(L, narg) : def;
}
#endif

static ginput_Touch getTouchInfo(lua_State* L)
{
	ginput_Touch info;

	lua_getfield(L, -1, "touch");

	lua_getfield(L, -1, "x");
	info.x = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "y");
	info.y = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "modifiers");
	info.modifiers = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "mouseButton");
	info.mouseButton = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "pressure");
	info.pressure = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "type");
	const char* luaTouchType = lua_tostring(L, -1);
	for (int i = 0; i < 4; i++) {
		if (!strcmp(luaTouchType, touchTypes[i]))
		{
			info.touchType = i;
			break;
		}
	}
	lua_pop(L, 1);

	lua_pop(L, 1); // pop "touch" table

	return info;
}

static ginput_MouseEvent getMouseInfo(lua_State* L)
{
	ginput_MouseEvent info;

	lua_getfield(L, -1, "x");
	info.x = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "y");
	info.y = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "button");
	info.button = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "wheel");
	info.wheel = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "modifiers");
	info.modifiers = lua_tointeger(L, -1);
	lua_pop(L, 1);

	return info;
}

ImGuiID checkID(lua_State* L, int idx = 2)
{
	double id = luaL_checknumber(L, idx);
	LUA_ASSERT(id > 0, "ID must be > 0!");
	return (ImGuiID)id;
}

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
	CallbackData* callbackData = (CallbackData*)data->UserData;
	lua_State* L = callbackData->L;
	STACK_CHECKER(L, "InputTextCallback", 0);

	lua_rawgeti(L, LUA_REGISTRYINDEX, callbackData->functionIndex);
	g_pushInstance(L, "ImGuiInputTextCallbackData", data);

	if (callbackData->argumentIndex != -1)
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, callbackData->argumentIndex);

		if (callbackData->flags & ImGuiInputTextFlags_CallbackCharFilter)
		{
			lua_call(L, 2, 1);
			int res = lua_toboolean(L, -1);
			return res;
		}
		else
		{
			lua_call(L, 2, 0);
		}
	}
	else
	{
		if (callbackData->flags & ImGuiInputTextFlags_CallbackCharFilter)
		{
			lua_call(L, 1, 1);
			int res = lua_toboolean(L, -1);
			return res;
		}
		else
		{
			lua_call(L, 1, 0);
		}
	}
	return 0;
}

static void NextWindowSizeConstraintCallback(ImGuiSizeCallbackData* data)
{
	CallbackData* callbackData = static_cast<CallbackData*>(data->UserData);
	lua_State* L = callbackData->L;
	STACK_CHECKER(L, "NextWindowSizeConstraintCallback", 0);

	lua_rawgeti(L, LUA_REGISTRYINDEX, callbackData->functionIndex);
	g_pushInstance(L, "ImGuiSizeCallbackData", data);
	if (callbackData->argumentIndex != -1)
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, callbackData->argumentIndex);
		lua_call(L, 2, 2);
	}
	else
	{
		lua_call(L, 1, 2);
	}
	data->DesiredSize = luaL_checkvec2(L, -2);
	lua_pop(L, 2);
}

lua_Number getTableValue(lua_State* L, int idx, int tid, lua_Number def)
{
	lua_rawgeti(L, idx, tid);
	lua_Number value = luaL_optnumber(L, -1, def);
	lua_pop(L, 1);
	return value;
}

void setupUVs(lua_State* L, GTextureData& data, int idx)
{
	if (lua_gettop(L) > idx - 1)
	{
		float x = 0.0f;
		float y = 0.0f;
		float w = data.texture_size.x;
		float h = data.texture_size.y;
		bool clamp_area;

		switch (lua_type(L, idx))
		{
			case LUA_TTABLE:
				{
					x = getTableValue(L, idx, 1, 0.0f);
					y = getTableValue(L, idx, 2, 0.0f);
					w = getTableValue(L, idx, 3, data.texture_size.x);
					h = getTableValue(L, idx, 4, data.texture_size.y);
					clamp_area = lua_toboolean(L, idx + 1);
				}
				break;
			case LUA_TVECTOR:
				{
					const float *cvec=lua_tovector(L, idx);
					x = cvec[0];
					y = cvec[1];
					w = cvec[2];
					h = cvec[3];
					clamp_area = lua_toboolean(L, idx + 1);
				}
				break;
			default:
				{
					x = luaL_optnumber(L, idx + 0, 0.0f);
					y = luaL_optnumber(L, idx + 1, 0.0f);
					w = luaL_optnumber(L, idx + 2, data.texture_size.x);
					h = luaL_optnumber(L, idx + 3, data.texture_size.y);
					clamp_area = lua_toboolean(L, idx + 4);
				}
				break;
		}

		float uv0x = x / data.ex_size.x;
		float uv0y = y / data.ex_size.y;
		float uv1x = (x + w) / data.ex_size.x;
		float uv1y = (y + h) / data.ex_size.y;

		if (clamp_area)
		{
			data.uv0.x = ImClamp(uv0x, 0.0f, 1.0f);
			data.uv0.y = ImClamp(uv0y, 0.0f, 1.0f);
			data.uv1.x = ImClamp(uv1x, 0.0f, 1.0f);
			data.uv1.y = ImClamp(uv1y, 0.0f, 1.0f);
		}
		else
		{
			data.uv0.x = uv0x;
			data.uv0.y = uv0y;
			data.uv1.x = uv1x;
			data.uv1.y = uv1y;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////
///
/// ENUMS
///
/////////////////////////////////////////////////////////////////////////////////////////////

void bindEnums(lua_State* L)
{
	lua_getglobal(L, "ImGui");
	//BackendFlags
	BIND_IENUM(L, ImGuiBackendFlags_None, "BackendFlags_None");
	BIND_IENUM(L, ImGuiBackendFlags_HasGamepad, "BackendFlags_HasGamepad");
	BIND_IENUM(L, ImGuiBackendFlags_HasMouseCursors, "BackendFlags_HasMouseCursors");
	BIND_IENUM(L, ImGuiBackendFlags_HasSetMousePos, "BackendFlags_HasSetMousePos");
	BIND_IENUM(L, ImGuiBackendFlags_RendererHasVtxOffset, "BackendFlags_RendererHasVtxOffset");

	//ImGuiFocusedFlags
	BIND_IENUM(L, ImGuiFocusedFlags_ChildWindows, "FocusedFlags_ChildWindows");
	BIND_IENUM(L, ImGuiFocusedFlags_AnyWindow, "FocusedFlags_AnyWindow");
	BIND_IENUM(L, ImGuiFocusedFlags_RootWindow, "FocusedFlags_RootWindow");
	BIND_IENUM(L, ImGuiFocusedFlags_RootAndChildWindows, "FocusedFlags_RootAndChildWindows");
	BIND_IENUM(L, ImGuiFocusedFlags_None, "FocusedFlags_None");
	BIND_IENUM(L, ImGuiFocusedFlags_NoPopupHierarchy, "FocusedFlags_NoPopupHierarchy");

	//ImGuiPopupFlags
	BIND_IENUM(L, ImGuiPopupFlags_NoOpenOverExistingPopup, "PopupFlags_NoOpenOverExistingPopup");
	BIND_IENUM(L, ImGuiPopupFlags_MouseButtonLeft, "PopupFlags_MouseButtonLeft");
	BIND_IENUM(L, ImGuiPopupFlags_MouseButtonMask_, "PopupFlags_MouseButtonMask");
	BIND_IENUM(L, ImGuiPopupFlags_MouseButtonRight, "PopupFlags_MouseButtonRight");
	BIND_IENUM(L, ImGuiPopupFlags_AnyPopupId, "PopupFlags_AnyPopupId");
	BIND_IENUM(L, ImGuiPopupFlags_MouseButtonDefault_, "PopupFlags_MouseButtonDefault");
	BIND_IENUM(L, ImGuiPopupFlags_MouseButtonMiddle, "PopupFlags_MouseButtonMiddle");
	BIND_IENUM(L, ImGuiPopupFlags_None, "PopupFlags_None");
	BIND_IENUM(L, ImGuiPopupFlags_AnyPopup, "PopupFlags_AnyPopup");
	BIND_IENUM(L, ImGuiPopupFlags_AnyPopupLevel, "PopupFlags_AnyPopupLevel");
	BIND_IENUM(L, ImGuiPopupFlags_NoOpenOverItems, "PopupFlags_NoOpenOverItems");

	//ImGuiHoveredFlags
	BIND_IENUM(L, ImGuiHoveredFlags_None, "HoveredFlags_None");
	BIND_IENUM(L, ImGuiHoveredFlags_RootAndChildWindows, "HoveredFlags_RootAndChildWindows");
	BIND_IENUM(L, ImGuiHoveredFlags_AllowWhenBlockedByPopup, "HoveredFlags_AllowWhenBlockedByPopup");
	BIND_IENUM(L, ImGuiHoveredFlags_AllowWhenBlockedByActiveItem, "HoveredFlags_AllowWhenBlockedByActiveItem");
	BIND_IENUM(L, ImGuiHoveredFlags_ChildWindows, "HoveredFlags_ChildWindows");
	BIND_IENUM(L, ImGuiHoveredFlags_RectOnly, "HoveredFlags_RectOnly");
	BIND_IENUM(L, ImGuiHoveredFlags_AllowWhenDisabled, "HoveredFlags_AllowWhenDisabled");
	BIND_IENUM(L, ImGuiHoveredFlags_AllowWhenOverlapped, "HoveredFlags_AllowWhenOverlapped");
	BIND_IENUM(L, ImGuiHoveredFlags_AnyWindow, "HoveredFlags_AnyWindow");
	BIND_IENUM(L, ImGuiHoveredFlags_RootWindow, "HoveredFlags_RootWindow");

	//ImGuiInputTextFlags
	BIND_IENUM(L, ImGuiInputTextFlags_EnterReturnsTrue, "InputTextFlags_EnterReturnsTrue");
	BIND_IENUM(L, ImGuiInputTextFlags_CallbackCompletion, "InputTextFlags_CallbackCompletion");
	BIND_IENUM(L, ImGuiInputTextFlags_CallbackEdit, "InputTextFlags_CallbackEdit");
	BIND_IENUM(L, ImGuiInputTextFlags_None, "InputTextFlags_None");
	BIND_IENUM(L, ImGuiInputTextFlags_CallbackResize, "InputTextFlags_CallbackResize");
	BIND_IENUM(L, ImGuiInputTextFlags_ReadOnly, "InputTextFlags_ReadOnly");
	BIND_IENUM(L, ImGuiInputTextFlags_AutoSelectAll, "InputTextFlags_AutoSelectAll");
	BIND_IENUM(L, ImGuiInputTextFlags_AllowTabInput, "InputTextFlags_AllowTabInput");
	BIND_IENUM(L, ImGuiInputTextFlags_CharsScientific, "InputTextFlags_CharsScientific");
	BIND_IENUM(L, ImGuiInputTextFlags_CallbackAlways, "InputTextFlags_CallbackAlways");
	BIND_IENUM(L, ImGuiInputTextFlags_CharsDecimal, "InputTextFlags_CharsDecimal");
	BIND_IENUM(L, ImGuiInputTextFlags_NoUndoRedo, "InputTextFlags_NoUndoRedo");
	BIND_IENUM(L, ImGuiInputTextFlags_CallbackHistory, "InputTextFlags_CallbackHistory");
	BIND_IENUM(L, ImGuiInputTextFlags_CtrlEnterForNewLine, "InputTextFlags_CtrlEnterForNewLine");
	BIND_IENUM(L, ImGuiInputTextFlags_CharsHexadecimal, "InputTextFlags_CharsHexadecimal");
	BIND_IENUM(L, ImGuiInputTextFlags_CharsNoBlank, "InputTextFlags_CharsNoBlank");
	BIND_IENUM(L, ImGuiInputTextFlags_Password, "InputTextFlags_Password");
	BIND_IENUM(L, ImGuiInputTextFlags_CallbackCharFilter, "InputTextFlags_CallbackCharFilter");
	BIND_IENUM(L, ImGuiInputTextFlags_NoHorizontalScroll, "InputTextFlags_NoHorizontalScroll");
	BIND_IENUM(L, ImGuiInputTextFlags_AlwaysOverwrite, "InputTextFlags_AlwaysOverwrite");
	BIND_IENUM(L, ImGuiInputTextFlags_CharsUppercase, "InputTextFlags_CharsUppercase");
	BIND_IENUM(L, ImGuiInputTextFlags_NoBackground, "InputTextFlags_NoBackground");

	//ImGuiTabBarFlags
	BIND_IENUM(L, ImGuiTabBarFlags_AutoSelectNewTabs, "TabBarFlags_AutoSelectNewTabs");
	BIND_IENUM(L, ImGuiTabBarFlags_NoCloseWithMiddleMouseButton, "TabBarFlags_NoCloseWithMiddleMouseButton");
	BIND_IENUM(L, ImGuiTabBarFlags_TabListPopupButton, "TabBarFlags_TabListPopupButton");
	BIND_IENUM(L, ImGuiTabBarFlags_NoTooltip, "TabBarFlags_NoTooltip");
	BIND_IENUM(L, ImGuiTabBarFlags_FittingPolicyMask_, "TabBarFlags_FittingPolicyMask");
	BIND_IENUM(L, ImGuiTabBarFlags_Reorderable, "TabBarFlags_Reorderable");
	BIND_IENUM(L, ImGuiTabBarFlags_FittingPolicyDefault_, "TabBarFlags_FittingPolicyDefault");
	BIND_IENUM(L, ImGuiTabBarFlags_FittingPolicyScroll, "TabBarFlags_FittingPolicyScroll");
	BIND_IENUM(L, ImGuiTabBarFlags_FittingPolicyResizeDown, "TabBarFlags_FittingPolicyResizeDown");
	BIND_IENUM(L, ImGuiTabBarFlags_None, "TabBarFlags_None");
	BIND_IENUM(L, ImGuiTabBarFlags_NoTabListScrollingButtons, "TabBarFlags_NoTabListScrollingButtons");

	//ImGuiTreeNodeFlags
	BIND_IENUM(L, ImGuiTreeNodeFlags_Bullet, "TreeNodeFlags_Bullet");
	BIND_IENUM(L, ImGuiTreeNodeFlags_None, "TreeNodeFlags_None");
	BIND_IENUM(L, ImGuiTreeNodeFlags_CollapsingHeader, "TreeNodeFlags_CollapsingHeader");
	BIND_IENUM(L, ImGuiTreeNodeFlags_NavLeftJumpsBackHere, "TreeNodeFlags_NavLeftJumpsBackHere");
	BIND_IENUM(L, ImGuiTreeNodeFlags_Framed, "TreeNodeFlags_Framed");
	BIND_IENUM(L, ImGuiTreeNodeFlags_FramePadding, "TreeNodeFlags_FramePadding");
	BIND_IENUM(L, ImGuiTreeNodeFlags_AllowItemOverlap, "TreeNodeFlags_AllowItemOverlap");
	BIND_IENUM(L, ImGuiTreeNodeFlags_OpenOnArrow, "TreeNodeFlags_OpenOnArrow");
	BIND_IENUM(L, ImGuiTreeNodeFlags_SpanFullWidth, "TreeNodeFlags_SpanFullWidth");
	BIND_IENUM(L, ImGuiTreeNodeFlags_NoAutoOpenOnLog, "TreeNodeFlags_NoAutoOpenOnLog");
	BIND_IENUM(L, ImGuiTreeNodeFlags_Leaf, "TreeNodeFlags_Leaf");
	BIND_IENUM(L, ImGuiTreeNodeFlags_NoTreePushOnOpen, "TreeNodeFlags_NoTreePushOnOpen");
	BIND_IENUM(L, ImGuiTreeNodeFlags_Selected, "TreeNodeFlags_Selected");
	BIND_IENUM(L, ImGuiTreeNodeFlags_SpanAvailWidth, "TreeNodeFlags_SpanAvailWidth");
	BIND_IENUM(L, ImGuiTreeNodeFlags_OpenOnDoubleClick, "TreeNodeFlags_OpenOnDoubleClick");
	BIND_IENUM(L, ImGuiTreeNodeFlags_DefaultOpen, "TreeNodeFlags_DefaultOpen");

	//ImGuiStyleVar
	BIND_IENUM(L, ImGuiStyleVar_GrabRounding, "StyleVar_GrabRounding");
	BIND_IENUM(L, ImGuiStyleVar_Alpha, "StyleVar_Alpha");
	BIND_IENUM(L, ImGuiStyleVar_WindowMinSize, "StyleVar_WindowMinSize");
	BIND_IENUM(L, ImGuiStyleVar_PopupBorderSize, "StyleVar_PopupBorderSize");
	BIND_IENUM(L, ImGuiStyleVar_WindowBorderSize, "StyleVar_WindowBorderSize");
	BIND_IENUM(L, ImGuiStyleVar_FrameBorderSize, "StyleVar_FrameBorderSize");
	BIND_IENUM(L, ImGuiStyleVar_ItemSpacing, "StyleVar_ItemSpacing");
	BIND_IENUM(L, ImGuiStyleVar_IndentSpacing, "StyleVar_IndentSpacing");
	BIND_IENUM(L, ImGuiStyleVar_FramePadding, "StyleVar_FramePadding");
	BIND_IENUM(L, ImGuiStyleVar_WindowPadding, "StyleVar_WindowPadding");
	BIND_IENUM(L, ImGuiStyleVar_ChildRounding, "StyleVar_ChildRounding");
	BIND_IENUM(L, ImGuiStyleVar_ItemInnerSpacing, "StyleVar_ItemInnerSpacing");
	BIND_IENUM(L, ImGuiStyleVar_WindowRounding, "StyleVar_WindowRounding");
	BIND_IENUM(L, ImGuiStyleVar_FrameRounding, "StyleVar_FrameRounding");
	BIND_IENUM(L, ImGuiStyleVar_TabRounding, "StyleVar_TabRounding");
	BIND_IENUM(L, ImGuiStyleVar_ChildBorderSize, "StyleVar_ChildBorderSize");
	BIND_IENUM(L, ImGuiStyleVar_GrabMinSize, "StyleVar_GrabMinSize");
	BIND_IENUM(L, ImGuiStyleVar_ScrollbarRounding, "StyleVar_ScrollbarRounding");
	BIND_IENUM(L, ImGuiStyleVar_ScrollbarSize, "StyleVar_ScrollbarSize");
	BIND_IENUM(L, ImGuiStyleVar_WindowTitleAlign, "StyleVar_WindowTitleAlign");
	BIND_IENUM(L, ImGuiStyleVar_SelectableTextAlign, "StyleVar_SelectableTextAlign");
	BIND_IENUM(L, ImGuiStyleVar_PopupRounding, "StyleVar_PopupRounding");
	BIND_IENUM(L, ImGuiStyleVar_ButtonTextAlign, "StyleVar_ButtonTextAlign");
	BIND_IENUM(L, ImGuiStyleVar_CellPadding, "StyleVar_CellPadding");
	BIND_IENUM(L, ImGuiStyleVar_DisabledAlpha, "StyleVar_DisabledAlpha");


	//ImGuiCol
	BIND_IENUM(L, ImGuiCol_PlotHistogram, "Col_PlotHistogram");
	BIND_IENUM(L, ImGuiCol_TitleBg, "Col_TitleBg");
	BIND_IENUM(L, ImGuiCol_Separator, "Col_Separator");
	BIND_IENUM(L, ImGuiCol_HeaderActive, "Col_HeaderActive");
	BIND_IENUM(L, ImGuiCol_HeaderHovered, "Col_HeaderHovered");
	BIND_IENUM(L, ImGuiCol_ButtonHovered, "Col_ButtonHovered");
	BIND_IENUM(L, ImGuiCol_NavWindowingHighlight, "Col_NavWindowingHighlight");
	BIND_IENUM(L, ImGuiCol_ScrollbarGrab, "Col_ScrollbarGrab");
	BIND_IENUM(L, ImGuiCol_FrameBg, "Col_FrameBg");
	BIND_IENUM(L, ImGuiCol_TextSelectedBg, "Col_TextSelectedBg");
	BIND_IENUM(L, ImGuiCol_ScrollbarGrabActive, "Col_ScrollbarGrabActive");
	BIND_IENUM(L, ImGuiCol_TitleBgCollapsed, "Col_TitleBgCollapsed");
	BIND_IENUM(L, ImGuiCol_ModalWindowDimBg, "Col_ModalWindowDimBg");
	BIND_IENUM(L, ImGuiCol_ResizeGripActive, "Col_ResizeGripActive");
	BIND_IENUM(L, ImGuiCol_SeparatorHovered, "Col_SeparatorHovered");
	BIND_IENUM(L, ImGuiCol_ScrollbarGrabHovered, "Col_ScrollbarGrabHovered");
	BIND_IENUM(L, ImGuiCol_TabUnfocused, "Col_TabUnfocused");
	BIND_IENUM(L, ImGuiCol_ScrollbarBg, "Col_ScrollbarBg");
	BIND_IENUM(L, ImGuiCol_ChildBg, "Col_ChildBg");
	BIND_IENUM(L, ImGuiCol_Header, "Col_Header");
	BIND_IENUM(L, ImGuiCol_NavWindowingDimBg, "Col_NavWindowingDimBg");
	BIND_IENUM(L, ImGuiCol_CheckMark, "Col_CheckMark");
	BIND_IENUM(L, ImGuiCol_Button, "Col_Button");
	BIND_IENUM(L, ImGuiCol_BorderShadow, "Col_BorderShadow");
	BIND_IENUM(L, ImGuiCol_DragDropTarget, "Col_DragDropTarget");
	BIND_IENUM(L, ImGuiCol_MenuBarBg, "Col_MenuBarBg");
	BIND_IENUM(L, ImGuiCol_TitleBgActive, "Col_TitleBgActive");
	BIND_IENUM(L, ImGuiCol_SeparatorActive, "Col_SeparatorActive");
	BIND_IENUM(L, ImGuiCol_Text, "Col_Text");
	BIND_IENUM(L, ImGuiCol_PlotLinesHovered, "Col_PlotLinesHovered");
	BIND_IENUM(L, ImGuiCol_Border, "Col_Border");
	BIND_IENUM(L, ImGuiCol_TabUnfocusedActive, "Col_TabUnfocusedActive");
	BIND_IENUM(L, ImGuiCol_PlotLines, "Col_PlotLines");
	BIND_IENUM(L, ImGuiCol_PlotHistogramHovered, "Col_PlotHistogramHovered");
	BIND_IENUM(L, ImGuiCol_ResizeGripHovered, "Col_ResizeGripHovered");
	BIND_IENUM(L, ImGuiCol_Tab, "Col_Tab");
	BIND_IENUM(L, ImGuiCol_TabHovered, "Col_TabHovered");
	BIND_IENUM(L, ImGuiCol_PopupBg, "Col_PopupBg");
	BIND_IENUM(L, ImGuiCol_TabActive, "Col_TabActive");
	BIND_IENUM(L, ImGuiCol_FrameBgActive, "Col_FrameBgActive");
	BIND_IENUM(L, ImGuiCol_ButtonActive, "Col_ButtonActive");
	BIND_IENUM(L, ImGuiCol_WindowBg, "Col_WindowBg");
	BIND_IENUM(L, ImGuiCol_SliderGrabActive, "Col_SliderGrabActive");
	BIND_IENUM(L, ImGuiCol_SliderGrab, "Col_SliderGrab");
	BIND_IENUM(L, ImGuiCol_NavHighlight, "Col_NavHighlight");
	BIND_IENUM(L, ImGuiCol_FrameBgHovered, "Col_FrameBgHovered");
	BIND_IENUM(L, ImGuiCol_TextDisabled, "Col_TextDisabled");
	BIND_IENUM(L, ImGuiCol_ResizeGrip, "Col_ResizeGrip");
	BIND_IENUM(L, ImGuiCol_TableHeaderBg, "Col_TableHeaderBg");
	BIND_IENUM(L, ImGuiCol_TableBorderStrong, "Col_TableBorderStrong");
	BIND_IENUM(L, ImGuiCol_TableBorderLight, "Col_TableBorderLight");
	BIND_IENUM(L, ImGuiCol_TableRowBg, "Col_TableRowBg");
	BIND_IENUM(L, ImGuiCol_TableRowBgAlt, "Col_TableRowBgAlt");

#ifdef IS_DOCKING_BUILD
	BIND_IENUM(L, ImGuiCol_DockingPreview, "Col_DockingPreview");
	BIND_IENUM(L, ImGuiCol_DockingEmptyBg, "Col_DockingEmptyBg");
#endif

	// TextEditor
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::Default, "TE_Default");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::Keyword, "TE_Keyword");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::Number, "TE_Number");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::String, "TE_String");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::CharLiteral, "TE_CharLiteral");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::Punctuation, "TE_Punctuation");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::Preprocessor, "TE_Preprocessor");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::Identifier, "TE_Identifier");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::KnownIdentifier, "TE_KnownIdentifier");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::PreprocIdentifier, "TE_PreprocIdentifier");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::Comment, "TE_Comment");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::MultiLineComment, "TE_MultiLineComment");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::Background, "TE_Background");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::Cursor, "TE_Cursor");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::Selection, "TE_Selection");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::ErrorMarker, "TE_ErrorMarker");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::Breakpoint, "TE_Breakpoint");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::LineNumber, "TE_LineNumber");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::CurrentLineFill, "TE_CurrentLineFill");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::CurrentLineFillInactive, "TE_CurrentLineFillInactive");
	BIND_IENUM(L, (int)TextEditor::PaletteIndex::CurrentLineEdge, "TE_CurrentLineEdge");

	//ImGuiDataType
	BIND_IENUM(L, ImGuiDataType_U8, "DataType_U8");
	BIND_IENUM(L, ImGuiDataType_S64, "DataType_S64");
	BIND_IENUM(L, ImGuiDataType_Float, "DataType_Float");
	BIND_IENUM(L, ImGuiDataType_S16, "DataType_S16");
	BIND_IENUM(L, ImGuiDataType_U16, "DataType_U16");
	BIND_IENUM(L, ImGuiDataType_Double, "DataType_Double");
	BIND_IENUM(L, ImGuiDataType_S8, "DataType_S8");
	BIND_IENUM(L, ImGuiDataType_U32, "DataType_U32");
	BIND_IENUM(L, ImGuiDataType_S32, "DataType_S32");
	BIND_IENUM(L, ImGuiDataType_U64, "DataType_U64");

	//ImGuiDir
	BIND_IENUM(L, ImGuiDir_None, "Dir_None");
	BIND_IENUM(L, ImGuiDir_Left, "Dir_Left");
	BIND_IENUM(L, ImGuiDir_Up, "Dir_Up");
	BIND_IENUM(L, ImGuiDir_Down, "Dir_Down");
	BIND_IENUM(L, ImGuiDir_Right, "Dir_Right");

	//ImGuiWindowFlags
	BIND_IENUM(L, ImGuiWindowFlags_NoScrollWithMouse, "WindowFlags_NoScrollWithMouse");
	BIND_IENUM(L, ImGuiWindowFlags_None, "WindowFlags_None");
	BIND_IENUM(L, ImGuiWindowFlags_NoScrollbar, "WindowFlags_NoScrollbar");
	BIND_IENUM(L, ImGuiWindowFlags_HorizontalScrollbar, "WindowFlags_HorizontalScrollbar");
	BIND_IENUM(L, ImGuiWindowFlags_NoFocusOnAppearing, "WindowFlags_NoFocusOnAppearing");
	BIND_IENUM(L, ImGuiWindowFlags_NoBringToFrontOnFocus, "WindowFlags_NoBringToFrontOnFocus");
	BIND_IENUM(L, ImGuiWindowFlags_NoDecoration, "WindowFlags_NoDecoration");
	BIND_IENUM(L, ImGuiWindowFlags_NoCollapse, "WindowFlags_NoCollapse");
	BIND_IENUM(L, ImGuiWindowFlags_NoTitleBar, "WindowFlags_NoTitleBar");
	BIND_IENUM(L, ImGuiWindowFlags_NoMove, "WindowFlags_NoMove");
	BIND_IENUM(L, ImGuiWindowFlags_NoInputs, "WindowFlags_NoInputs");
	BIND_IENUM(L, ImGuiWindowFlags_NoMouseInputs, "WindowFlags_NoMouseInputs");
	BIND_IENUM(L, ImGuiWindowFlags_NoSavedSettings, "WindowFlags_NoSavedSettings");
	BIND_IENUM(L, ImGuiWindowFlags_NoNav, "WindowFlags_NoNav");
	BIND_IENUM(L, ImGuiWindowFlags_UnsavedDocument, "WindowFlags_UnsavedDocument");
	BIND_IENUM(L, ImGuiWindowFlags_NoNavFocus, "WindowFlags_NoNavFocus");
	BIND_IENUM(L, ImGuiWindowFlags_AlwaysHorizontalScrollbar, "WindowFlags_AlwaysHorizontalScrollbar");
	BIND_IENUM(L, ImGuiWindowFlags_AlwaysUseWindowPadding, "WindowFlags_AlwaysUseWindowPadding");
	BIND_IENUM(L, ImGuiWindowFlags_NoNavInputs, "WindowFlags_NoNavInputs");
	BIND_IENUM(L, ImGuiWindowFlags_NoResize, "WindowFlags_NoResize");
	BIND_IENUM(L, ImGuiWindowFlags_AlwaysVerticalScrollbar, "WindowFlags_AlwaysVerticalScrollbar");
	BIND_IENUM(L, ImGuiWindowFlags_MenuBar, "WindowFlags_MenuBar");
	BIND_IENUM(L, ImGuiWindowFlags_NoBackground, "WindowFlags_NoBackground");
	BIND_IENUM(L, ImGuiWindowFlags_AlwaysAutoResize, "WindowFlags_AlwaysAutoResize");
#ifdef IS_DOCKING_BUILD
	BIND_IENUM(L, ImGuiWindowFlags_NoDocking, "WindowFlags_NoDocking");
#endif
	//@MultiPain
	BIND_IENUM(L, ImGuiWindowFlags_FullScreen, "WindowFlags_FullScreen");

	//ImGuiTabItemFlags
	BIND_IENUM(L, ImGuiTabItemFlags_SetSelected, "TabItemFlags_SetSelected");
	BIND_IENUM(L, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton, "TabItemFlags_NoCloseWithMiddleMouseButton");
	BIND_IENUM(L, ImGuiTabItemFlags_NoTooltip, "TabItemFlags_NoTooltip");
	BIND_IENUM(L, ImGuiTabItemFlags_None, "TabItemFlags_None");
	BIND_IENUM(L, ImGuiTabItemFlags_NoPushId, "TabItemFlags_NoPushId");
	BIND_IENUM(L, ImGuiTabItemFlags_UnsavedDocument, "TabItemFlags_UnsavedDocument");
	BIND_IENUM(L, ImGuiTabItemFlags_Leading, "TabItemFlags_Leading");
	BIND_IENUM(L, ImGuiTabItemFlags_Trailing, "TabItemFlags_Trailing");
	BIND_IENUM(L, ImGuiTabItemFlags_NoReorder, "TabItemFlags_NoReorder");

	//ImGuiComboFlags
	BIND_IENUM(L, ImGuiComboFlags_HeightSmall, "ComboFlags_HeightSmall");
	BIND_IENUM(L, ImGuiComboFlags_HeightLarge, "ComboFlags_HeightLarge");
	BIND_IENUM(L, ImGuiComboFlags_PopupAlignLeft, "ComboFlags_PopupAlignLeft");
	BIND_IENUM(L, ImGuiComboFlags_None, "ComboFlags_None");
	BIND_IENUM(L, ImGuiComboFlags_NoPreview, "ComboFlags_NoPreview");
	BIND_IENUM(L, ImGuiComboFlags_HeightRegular, "ComboFlags_HeightRegular");
	BIND_IENUM(L, ImGuiComboFlags_HeightMask_, "ComboFlags_HeightMask");
	BIND_IENUM(L, ImGuiComboFlags_NoArrowButton, "ComboFlags_NoArrowButton");
	BIND_IENUM(L, ImGuiComboFlags_HeightLargest, "ComboFlags_HeightLargest");

	//ImGuiCond
	BIND_IENUM(L, ImGuiCond_Appearing, "Cond_Appearing");
	BIND_IENUM(L, ImGuiCond_None, "Cond_None");
	BIND_IENUM(L, ImGuiCond_Always, "Cond_Always");
	BIND_IENUM(L, ImGuiCond_FirstUseEver, "Cond_FirstUseEver");
	BIND_IENUM(L, ImGuiCond_Once, "Cond_Once");

	//ImGuiSelectableFlags
	BIND_IENUM(L, ImGuiSelectableFlags_None, "SelectableFlags_None");
	BIND_IENUM(L, ImGuiSelectableFlags_SpanAllColumns, "SelectableFlags_SpanAllColumns");
	BIND_IENUM(L, ImGuiSelectableFlags_AllowItemOverlap, "SelectableFlags_AllowItemOverlap");
	BIND_IENUM(L, ImGuiSelectableFlags_DontClosePopups, "SelectableFlags_DontClosePopups");
	BIND_IENUM(L, ImGuiSelectableFlags_AllowDoubleClick, "SelectableFlags_AllowDoubleClick");
	BIND_IENUM(L, ImGuiSelectableFlags_Disabled, "SelectableFlags_Disabled");

	//ImGuiMouseCursor
	BIND_IENUM(L, ImGuiMouseCursor_None, "MouseCursor_None");
	BIND_IENUM(L, ImGuiMouseCursor_Arrow, "MouseCursor_Arrow");
	BIND_IENUM(L, ImGuiMouseCursor_TextInput, "MouseCursor_TextInput");
	BIND_IENUM(L, ImGuiMouseCursor_ResizeAll, "MouseCursor_ResizeAll");
	BIND_IENUM(L, ImGuiMouseCursor_ResizeNS, "MouseCursor_ResizeNS");
	BIND_IENUM(L, ImGuiMouseCursor_ResizeEW, "MouseCursor_ResizeEW");
	BIND_IENUM(L, ImGuiMouseCursor_ResizeNESW, "MouseCursor_ResizeNESW");
	BIND_IENUM(L, ImGuiMouseCursor_ResizeNWSE, "MouseCursor_ResizeNWSE");
	BIND_IENUM(L, ImGuiMouseCursor_Hand, "MouseCursor_Hand");
	BIND_IENUM(L, ImGuiMouseCursor_NotAllowed, "MouseCursor_NotAllowed");

	//ImGuiColorEditFlags
	BIND_IENUM(L, ImGuiColorEditFlags_AlphaPreview, "ColorEditFlags_AlphaPreview");
	BIND_IENUM(L, ImGuiColorEditFlags_DisplayRGB, "ColorEditFlags_DisplayRGB");
	BIND_IENUM(L, ImGuiColorEditFlags_DisplayHex, "ColorEditFlags_DisplayHex");
	BIND_IENUM(L, ImGuiColorEditFlags_InputHSV, "ColorEditFlags_InputHSV");
	BIND_IENUM(L, ImGuiColorEditFlags_NoSidePreview, "ColorEditFlags_NoSidePreview");
	BIND_IENUM(L, ImGuiColorEditFlags_Uint8, "ColorEditFlags_Uint8");
	BIND_IENUM(L, ImGuiColorEditFlags_AlphaPreviewHalf, "ColorEditFlags_AlphaPreviewHalf");
	BIND_IENUM(L, ImGuiColorEditFlags_Float, "ColorEditFlags_Float");
	BIND_IENUM(L, ImGuiColorEditFlags_PickerHueWheel, "ColorEditFlags_PickerHueWheel");
	BIND_IENUM(L, ImGuiColorEditFlags_DefaultOptions_, "ColorEditFlags_OptionsDefault");
	BIND_IENUM(L, ImGuiColorEditFlags_InputRGB, "ColorEditFlags_InputRGB");
	BIND_IENUM(L, ImGuiColorEditFlags_HDR, "ColorEditFlags_HDR");
	BIND_IENUM(L, ImGuiColorEditFlags_NoPicker, "ColorEditFlags_NoPicker");
	BIND_IENUM(L, ImGuiColorEditFlags_AlphaBar, "ColorEditFlags_AlphaBar");
	BIND_IENUM(L, ImGuiColorEditFlags_DisplayHSV, "ColorEditFlags_DisplayHSV");
	BIND_IENUM(L, ImGuiColorEditFlags_PickerHueBar, "ColorEditFlags_PickerHueBar");
	BIND_IENUM(L, ImGuiColorEditFlags_NoAlpha, "ColorEditFlags_NoAlpha");
	BIND_IENUM(L, ImGuiColorEditFlags_NoOptions, "ColorEditFlags_NoOptions");
	BIND_IENUM(L, ImGuiColorEditFlags_NoDragDrop, "ColorEditFlags_NoDragDrop");
	BIND_IENUM(L, ImGuiColorEditFlags_NoInputs, "ColorEditFlags_NoInputs");
	BIND_IENUM(L, ImGuiColorEditFlags_None, "ColorEditFlags_None");
	BIND_IENUM(L, ImGuiColorEditFlags_NoSmallPreview, "ColorEditFlags_NoSmallPreview");
	BIND_IENUM(L, ImGuiColorEditFlags_NoBorder, "ColorEditFlags_NoBorder");
	BIND_IENUM(L, ImGuiColorEditFlags_NoLabel, "ColorEditFlags_NoLabel");
	BIND_IENUM(L, ImGuiColorEditFlags_NoTooltip, "ColorEditFlags_NoTooltip");

	//ImGuiDragDropFlags
	BIND_IENUM(L, ImGuiDragDropFlags_SourceNoPreviewTooltip, "DragDropFlags_SourceNoPreviewTooltip");
	BIND_IENUM(L, ImGuiDragDropFlags_SourceAllowNullID, "DragDropFlags_SourceAllowNullID");
	BIND_IENUM(L, ImGuiDragDropFlags_AcceptNoDrawDefaultRect, "DragDropFlags_AcceptNoDrawDefaultRect");
	BIND_IENUM(L, ImGuiDragDropFlags_AcceptPeekOnly, "DragDropFlags_AcceptPeekOnly");
	BIND_IENUM(L, ImGuiDragDropFlags_AcceptBeforeDelivery, "DragDropFlags_AcceptBeforeDelivery");
	BIND_IENUM(L, ImGuiDragDropFlags_SourceNoHoldToOpenOthers, "DragDropFlags_SourceNoHoldToOpenOthers");
	BIND_IENUM(L, ImGuiDragDropFlags_AcceptNoPreviewTooltip, "DragDropFlags_AcceptNoPreviewTooltip");
	BIND_IENUM(L, ImGuiDragDropFlags_SourceAutoExpirePayload, "DragDropFlags_SourceAutoExpirePayload");
	BIND_IENUM(L, ImGuiDragDropFlags_SourceExtern, "DragDropFlags_SourceExtern");
	BIND_IENUM(L, ImGuiDragDropFlags_None, "DragDropFlags_None");
	BIND_IENUM(L, ImGuiDragDropFlags_SourceNoDisableHover, "DragDropFlags_SourceNoDisableHover");

	//ImDrawFlags
	BIND_IENUM(L, ImDrawFlags_None, "DrawFlags_None");
	BIND_IENUM(L, ImDrawFlags_Closed, "DrawFlags_Closed");
	BIND_IENUM(L, ImDrawFlags_RoundCornersTopLeft, "DrawFlags_RoundCornersTopLeft");
	BIND_IENUM(L, ImDrawFlags_RoundCornersTopRight, "DrawFlags_RoundCornersTopRight");
	BIND_IENUM(L, ImDrawFlags_RoundCornersBottomLeft, "DrawFlags_RoundCornersBottomLeft");
	BIND_IENUM(L, ImDrawFlags_RoundCornersBottomRight, "DrawFlags_RoundCornersBottomRight");
	BIND_IENUM(L, ImDrawFlags_RoundCornersNone, "DrawFlags_RoundCornersNone");
	BIND_IENUM(L, ImDrawFlags_RoundCornersTop, "DrawFlags_RoundCornersTop");
	BIND_IENUM(L, ImDrawFlags_RoundCornersBottom, "DrawFlags_RoundCornersBottom");
	BIND_IENUM(L, ImDrawFlags_RoundCornersLeft, "DrawFlags_RoundCornersLeft");
	BIND_IENUM(L, ImDrawFlags_RoundCornersRight, "DrawFlags_RoundCornersRight");
	BIND_IENUM(L, ImDrawFlags_RoundCornersAll, "DrawFlags_RoundCornersAll");

	//ImGuiSliderFlags
	BIND_IENUM(L, ImGuiSliderFlags_None, "SliderFlags_None");
	BIND_IENUM(L, ImGuiSliderFlags_AlwaysClamp, "SliderFlags_ClampOnInput");
	BIND_IENUM(L, ImGuiSliderFlags_AlwaysClamp, "SliderFlags_AlwaysClamp");
	BIND_IENUM(L, ImGuiSliderFlags_Logarithmic, "SliderFlags_Logarithmic");
	BIND_IENUM(L, ImGuiSliderFlags_NoRoundToFormat, "SliderFlags_NoRoundToFormat");
	BIND_IENUM(L, ImGuiSliderFlags_NoInput, "SliderFlags_NoInput");

	//ImGuiConfigFlags
	BIND_IENUM(L, ImGuiConfigFlags_None, "ConfigFlags_None");
	BIND_IENUM(L, ImGuiConfigFlags_NavEnableKeyboard, "ConfigFlags_NavEnableKeyboard");
	BIND_IENUM(L, ImGuiConfigFlags_NavEnableGamepad, "ConfigFlags_NavEnableGamepad");
	BIND_IENUM(L, ImGuiConfigFlags_NavEnableSetMousePos, "ConfigFlags_NavEnableSetMousePos");
	BIND_IENUM(L, ImGuiConfigFlags_NavNoCaptureKeyboard, "ConfigFlags_NavNoCaptureKeyboard");
	BIND_IENUM(L, ImGuiConfigFlags_NoMouse, "ConfigFlags_NoMouse");
	BIND_IENUM(L, ImGuiConfigFlags_NoMouseCursorChange, "ConfigFlags_NoMouseCursorChange");
	BIND_IENUM(L, ImGuiConfigFlags_IsSRGB, "ConfigFlags_IsSRGB");
	BIND_IENUM(L, ImGuiConfigFlags_IsTouchScreen, "ConfigFlags_IsTouchScreen");
#ifdef IS_DOCKING_BUILD
	BIND_IENUM(L, ImGuiConfigFlags_DockingEnable, "ConfigFlags_DockingEnable");

	//ImGuiDockNodeFlags
	BIND_IENUM(L, ImGuiDockNodeFlags_None, "DockNodeFlags_None");
	BIND_IENUM(L, ImGuiDockNodeFlags_KeepAliveOnly, "DockNodeFlags_KeepAliveOnly");
	BIND_IENUM(L, ImGuiDockNodeFlags_NoDockingInCentralNode, "DockNodeFlags_NoDockingInCentralNode");
	BIND_IENUM(L, ImGuiDockNodeFlags_PassthruCentralNode, "DockNodeFlags_PassthruCentralNode");
	BIND_IENUM(L, ImGuiDockNodeFlags_NoSplit, "DockNodeFlags_NoSplit");
	BIND_IENUM(L, ImGuiDockNodeFlags_NoResize, "DockNodeFlags_NoResize");
	BIND_IENUM(L, ImGuiDockNodeFlags_AutoHideTabBar, "DockNodeFlags_AutoHideTabBar");
	BIND_IENUM(L, ImGuiDockNodeFlags_NoWindowMenuButton, "DockNodeFlags_NoWindowMenuButton");
	BIND_IENUM(L, ImGuiDockNodeFlags_NoCloseButton, "DockNodeFlags_NoCloseButton");
#endif

	//@MultiPain
	//ImGuiGlyphRanges
	BIND_IENUM(L, ImGuiGlyphRanges_Default, "GlyphRanges_Default");
	BIND_IENUM(L, ImGuiGlyphRanges_Korean, "GlyphRanges_Korean");
	BIND_IENUM(L, ImGuiGlyphRanges_ChineseFull, "GlyphRanges_ChineseFull");
	BIND_IENUM(L, ImGuiGlyphRanges_ChineseSimplifiedCommon, "GlyphRanges_ChineseSimplifiedCommon");
	BIND_IENUM(L, ImGuiGlyphRanges_Japanese, "GlyphRanges_Japanese");
	BIND_IENUM(L, ImGuiGlyphRanges_Cyrillic, "GlyphRanges_Cyrillic");
	BIND_IENUM(L, ImGuiGlyphRanges_Thai, "GlyphRanges_Thai");
	BIND_IENUM(L, ImGuiGlyphRanges_Vietnamese, "GlyphRanges_Vietnamese");

	//ImGuiItemFlags
	BIND_IENUM(L, ImGuiItemFlags_Disabled, "ItemFlags_Disabled");
	BIND_IENUM(L, ImGuiItemFlags_ButtonRepeat, "ItemFlags_ButtonRepeat");

	//ImGuiNavInput
	BIND_IENUM(L, ImGuiNavInput_FocusNext, "NavInput_FocusNext");
	BIND_IENUM(L, ImGuiNavInput_TweakFast, "NavInput_TweakFast");
	BIND_IENUM(L, ImGuiNavInput_Input, "NavInput_Input");
	BIND_IENUM(L, ImGuiNavInput_DpadRight, "NavInput_DpadRight");
	BIND_IENUM(L, ImGuiNavInput_FocusPrev, "NavInput_FocusPrev");
	BIND_IENUM(L, ImGuiNavInput_LStickDown, "NavInput_LStickDown");
	BIND_IENUM(L, ImGuiNavInput_LStickUp, "NavInput_LStickUp");
	BIND_IENUM(L, ImGuiNavInput_Activate, "NavInput_Activate");
	BIND_IENUM(L, ImGuiNavInput_LStickLeft, "NavInput_LStickLeft");
	BIND_IENUM(L, ImGuiNavInput_LStickRight, "NavInput_LStickRight");
	BIND_IENUM(L, ImGuiNavInput_DpadLeft, "NavInput_DpadLeft");
	BIND_IENUM(L, ImGuiNavInput_DpadDown, "NavInput_DpadDown");
	BIND_IENUM(L, ImGuiNavInput_TweakSlow, "NavInput_TweakSlow");
	BIND_IENUM(L, ImGuiNavInput_DpadUp, "NavInput_DpadUp");
	BIND_IENUM(L, ImGuiNavInput_Menu, "NavInput_Menu");
	BIND_IENUM(L, ImGuiNavInput_Cancel, "NavInput_Cancel");

	// ImGuiTableBgTarget
	BIND_IENUM(L, ImGuiTableBgTarget_None, "TableBgTarget_None");
	BIND_IENUM(L, ImGuiTableBgTarget_RowBg0, "TableBgTarget_RowBg0");
	BIND_IENUM(L, ImGuiTableBgTarget_RowBg1, "TableBgTarget_RowBg1");
	BIND_IENUM(L, ImGuiTableBgTarget_CellBg, "TableBgTarget_CellBg");

	// ImGuiTableColumnFlags
	BIND_IENUM(L, ImGuiTableColumnFlags_None, "TableColumnFlags_None");
	BIND_IENUM(L, ImGuiTableColumnFlags_DefaultHide, "TableColumnFlags_DefaultHide");
	BIND_IENUM(L, ImGuiTableColumnFlags_DefaultSort, "TableColumnFlags_DefaultSort");
	BIND_IENUM(L, ImGuiTableColumnFlags_WidthStretch, "TableColumnFlags_WidthStretch");
	BIND_IENUM(L, ImGuiTableColumnFlags_WidthFixed, "TableColumnFlags_WidthFixed");
	BIND_IENUM(L, ImGuiTableColumnFlags_NoResize, "TableColumnFlags_NoResize");
	BIND_IENUM(L, ImGuiTableColumnFlags_NoReorder, "TableColumnFlags_NoReorder");
	BIND_IENUM(L, ImGuiTableColumnFlags_NoHide, "TableColumnFlags_NoHide");
	BIND_IENUM(L, ImGuiTableColumnFlags_NoClip, "TableColumnFlags_NoClip");
	BIND_IENUM(L, ImGuiTableColumnFlags_NoSort, "TableColumnFlags_NoSort");
	BIND_IENUM(L, ImGuiTableColumnFlags_NoSortAscending, "TableColumnFlags_NoSortAscending");
	BIND_IENUM(L, ImGuiTableColumnFlags_NoSortDescending, "TableColumnFlags_NoSortDescending");
	BIND_IENUM(L, ImGuiTableColumnFlags_NoHeaderWidth, "TableColumnFlags_NoHeaderWidth");
	BIND_IENUM(L, ImGuiTableColumnFlags_PreferSortAscending, "TableColumnFlags_PreferSortAscending");
	BIND_IENUM(L, ImGuiTableColumnFlags_PreferSortDescending, "TableColumnFlags_PreferSortDescending");
	BIND_IENUM(L, ImGuiTableColumnFlags_IndentEnable, "TableColumnFlags_IndentEnable");
	BIND_IENUM(L, ImGuiTableColumnFlags_IndentDisable, "TableColumnFlags_IndentDisable");
	BIND_IENUM(L, ImGuiTableColumnFlags_IsEnabled, "TableColumnFlags_IsEnabled");
	BIND_IENUM(L, ImGuiTableColumnFlags_IsVisible, "TableColumnFlags_IsVisible");
	BIND_IENUM(L, ImGuiTableColumnFlags_IsSorted, "TableColumnFlags_IsSorted");
	BIND_IENUM(L, ImGuiTableColumnFlags_IsHovered, "TableColumnFlags_IsHovered");
	BIND_IENUM(L, ImGuiTableColumnFlags_Disabled, "TableColumnFlags_Disabled");
	BIND_IENUM(L, ImGuiTableColumnFlags_NoHeaderLabel, "TableColumnFlags_NoHeaderLabel");

	// ImGuiTableFlags
	BIND_IENUM(L, ImGuiTableFlags_None, "TableFlags_None");
	BIND_IENUM(L, ImGuiTableFlags_Resizable, "TableFlags_Resizable");
	BIND_IENUM(L, ImGuiTableFlags_Reorderable, "TableFlags_Reorderable");
	BIND_IENUM(L, ImGuiTableFlags_Hideable, "TableFlags_Hideable");
	BIND_IENUM(L, ImGuiTableFlags_Sortable, "TableFlags_Sortable");
	BIND_IENUM(L, ImGuiTableFlags_NoSavedSettings, "TableFlags_NoSavedSettings");
	BIND_IENUM(L, ImGuiTableFlags_ContextMenuInBody, "TableFlags_ContextMenuInBody");
	BIND_IENUM(L, ImGuiTableFlags_RowBg, "TableFlags_RowBg");
	BIND_IENUM(L, ImGuiTableFlags_BordersInnerH, "TableFlags_BordersInnerH");
	BIND_IENUM(L, ImGuiTableFlags_BordersOuterH, "TableFlags_BordersOuterH");
	BIND_IENUM(L, ImGuiTableFlags_BordersInnerV, "TableFlags_BordersInnerV");
	BIND_IENUM(L, ImGuiTableFlags_BordersOuterV, "TableFlags_BordersOuterV");
	BIND_IENUM(L, ImGuiTableFlags_BordersH, "TableFlags_BordersH");
	BIND_IENUM(L, ImGuiTableFlags_BordersV, "TableFlags_BordersV");
	BIND_IENUM(L, ImGuiTableFlags_BordersInner, "TableFlags_BordersInner");
	BIND_IENUM(L, ImGuiTableFlags_BordersOuter, "TableFlags_BordersOuter");
	BIND_IENUM(L, ImGuiTableFlags_Borders, "TableFlags_Borders");
	BIND_IENUM(L, ImGuiTableFlags_NoBordersInBody, "TableFlags_NoBordersInBody");
	BIND_IENUM(L, ImGuiTableFlags_NoBordersInBodyUntilResize, "TableFlags_NoBordersInBodyUntilResize");
	BIND_IENUM(L, ImGuiTableFlags_SizingFixedFit, "TableFlags_SizingFixedFit");
	BIND_IENUM(L, ImGuiTableFlags_SizingFixedSame, "TableFlags_SizingFixedSame");
	BIND_IENUM(L, ImGuiTableFlags_SizingStretchProp, "TableFlags_SizingStretchProp");
	BIND_IENUM(L, ImGuiTableFlags_SizingStretchSame, "TableFlags_SizingStretchSame");
	BIND_IENUM(L, ImGuiTableFlags_NoHostExtendX, "TableFlags_NoHostExtendX");
	BIND_IENUM(L, ImGuiTableFlags_NoHostExtendY, "TableFlags_NoHostExtendY");
	BIND_IENUM(L, ImGuiTableFlags_NoKeepColumnsVisible, "TableFlags_NoKeepColumnsVisible");
	BIND_IENUM(L, ImGuiTableFlags_PreciseWidths, "TableFlags_PreciseWidths");
	BIND_IENUM(L, ImGuiTableFlags_NoClip, "TableFlags_NoClip");
	BIND_IENUM(L, ImGuiTableFlags_PadOuterX, "TableFlags_PadOuterX");
	BIND_IENUM(L, ImGuiTableFlags_NoPadOuterX, "TableFlags_NoPadOuterX");
	BIND_IENUM(L, ImGuiTableFlags_NoPadInnerX, "TableFlags_NoPadInnerX");
	BIND_IENUM(L, ImGuiTableFlags_ScrollX, "TableFlags_ScrollX");
	BIND_IENUM(L, ImGuiTableFlags_ScrollY, "TableFlags_ScrollY");
	BIND_IENUM(L, ImGuiTableFlags_SortMulti, "TableFlags_SortMulti");
	BIND_IENUM(L, ImGuiTableFlags_SortTristate, "TableFlags_SortTristate");

	// ImGuiTableColumnFlags
	BIND_IENUM(L, ImGuiTableColumnFlags_None, "TableColumnFlags_None");
	BIND_IENUM(L, ImGuiTableColumnFlags_DefaultHide, "TableColumnFlags_DefaultHide");
	BIND_IENUM(L, ImGuiTableColumnFlags_DefaultSort, "TableColumnFlags_DefaultSort");
	BIND_IENUM(L, ImGuiTableColumnFlags_WidthStretch, "TableColumnFlags_WidthStretch");
	BIND_IENUM(L, ImGuiTableColumnFlags_WidthFixed, "TableColumnFlags_WidthFixed");
	BIND_IENUM(L, ImGuiTableColumnFlags_NoResize, "TableColumnFlags_NoResize");
	BIND_IENUM(L, ImGuiTableColumnFlags_NoReorder, "TableColumnFlags_NoReorder");
	BIND_IENUM(L, ImGuiTableColumnFlags_NoHide, "TableColumnFlags_NoHide");
	BIND_IENUM(L, ImGuiTableColumnFlags_NoClip, "TableColumnFlags_NoClip");
	BIND_IENUM(L, ImGuiTableColumnFlags_NoSort, "TableColumnFlags_NoSort");
	BIND_IENUM(L, ImGuiTableColumnFlags_NoSortAscending, "TableColumnFlags_NoSortAscending");
	BIND_IENUM(L, ImGuiTableColumnFlags_NoSortDescending, "TableColumnFlags_NoSortDescending");
	BIND_IENUM(L, ImGuiTableColumnFlags_NoHeaderWidth, "TableColumnFlags_NoHeaderWidth");
	BIND_IENUM(L, ImGuiTableColumnFlags_PreferSortAscending, "TableColumnFlags_PreferSortAscending");
	BIND_IENUM(L, ImGuiTableColumnFlags_PreferSortDescending, "TableColumnFlags_PreferSortDescending");
	BIND_IENUM(L, ImGuiTableColumnFlags_IndentEnable, "TableColumnFlags_IndentEnable");
	BIND_IENUM(L, ImGuiTableColumnFlags_IndentDisable, "TableColumnFlags_IndentDisable");
	BIND_IENUM(L, ImGuiTableColumnFlags_IsEnabled, "TableColumnFlags_IsEnabled");
	BIND_IENUM(L, ImGuiTableColumnFlags_IsVisible, "TableColumnFlags_IsVisible");
	BIND_IENUM(L, ImGuiTableColumnFlags_IsSorted, "TableColumnFlags_IsSorted");
	BIND_IENUM(L, ImGuiTableColumnFlags_IsHovered, "TableColumnFlags_IsHovered");

	// ImGuiTableRowFlags
	BIND_IENUM(L, ImGuiTableRowFlags_None, "TableRowFlags_None");
	BIND_IENUM(L, ImGuiTableRowFlags_Headers, "TableRowFlags_Headers");

	// ImGuiSortDirection
	BIND_IENUM(L, ImGuiSortDirection_None, "SortDirection_None");
	BIND_IENUM(L, ImGuiSortDirection_Ascending, "SortDirection_Ascending");
	BIND_IENUM(L, ImGuiSortDirection_Descending, "SortDirection_Descending");

	BIND_IENUM(L, ImGuiImageScaleMode_LetterBox, "ImageScaleMode_LetterBox");
	BIND_IENUM(L, ImGuiImageScaleMode_FitWidth, "ImageScaleMode_FitWidth");
	BIND_IENUM(L, ImGuiImageScaleMode_FitHeight, "ImageScaleMode_FitHeight");
	BIND_IENUM(L, ImGuiImageScaleMode_Stretch, "ImageScaleMode_Stretch");

	// ImGuiButtonFlags
	BIND_IENUM(L, ImGuiButtonFlags_None, "ButtonFlags_None");
	BIND_IENUM(L, ImGuiButtonFlags_MouseButtonLeft, "ButtonFlags_MouseButtonLeft");
	BIND_IENUM(L, ImGuiButtonFlags_MouseButtonRight, "ButtonFlags_MouseButtonRight");
	BIND_IENUM(L, ImGuiButtonFlags_MouseButtonMiddle, "ButtonFlags_MouseButtonMiddle");

	// Keyboard

	BIND_IENUM(L, ImGuiKey_None, "Key_None");
	BIND_IENUM(L, ImGuiKey_Tab, "Key_Tab");
	BIND_IENUM(L, ImGuiKey_LeftArrow, "Key_LeftArrow");
	BIND_IENUM(L, ImGuiKey_RightArrow, "Key_RightArrow");
	BIND_IENUM(L, ImGuiKey_UpArrow, "Key_UpArrow");
	BIND_IENUM(L, ImGuiKey_DownArrow, "Key_DownArrow");
	BIND_IENUM(L, ImGuiKey_PageUp, "Key_PageUp");
	BIND_IENUM(L, ImGuiKey_PageDown, "Key_PageDown");
	BIND_IENUM(L, ImGuiKey_Home, "Key_Home");
	BIND_IENUM(L, ImGuiKey_End, "Key_End");
	BIND_IENUM(L, ImGuiKey_Insert, "Key_Insert");
	BIND_IENUM(L, ImGuiKey_Delete, "Key_Delete");
	BIND_IENUM(L, ImGuiKey_Backspace, "Key_Backspace");
	BIND_IENUM(L, ImGuiKey_Space, "Key_Space");
	BIND_IENUM(L, ImGuiKey_Enter, "Key_Enter");
	BIND_IENUM(L, ImGuiKey_Escape, "Key_Escape");
	BIND_IENUM(L, ImGuiKey_LeftCtrl, "Key_LeftCtrl");
	BIND_IENUM(L, ImGuiKey_LeftShift, "Key_LeftShift");
	BIND_IENUM(L, ImGuiKey_LeftAlt, "Key_LeftAlt");
	BIND_IENUM(L, ImGuiKey_LeftSuper, "Key_LeftSuper");
	BIND_IENUM(L, ImGuiKey_RightCtrl, "Key_RightCtrl");
	BIND_IENUM(L, ImGuiKey_RightShift, "Key_RightShift");
	BIND_IENUM(L, ImGuiKey_RightAlt, "Key_RightAlt");
	BIND_IENUM(L, ImGuiKey_RightSuper, "Key_RightSuper");
	BIND_IENUM(L, ImGuiKey_Menu, "Key_Menu");
	BIND_IENUM(L, ImGuiKey_0, "Key_0");
	BIND_IENUM(L, ImGuiKey_1, "Key_1");
	BIND_IENUM(L, ImGuiKey_2, "Key_2");
	BIND_IENUM(L, ImGuiKey_3, "Key_3");
	BIND_IENUM(L, ImGuiKey_4, "Key_4");
	BIND_IENUM(L, ImGuiKey_5, "Key_5");
	BIND_IENUM(L, ImGuiKey_6, "Key_6");
	BIND_IENUM(L, ImGuiKey_7, "Key_7");
	BIND_IENUM(L, ImGuiKey_8, "Key_8");
	BIND_IENUM(L, ImGuiKey_9, "Key_9");
	BIND_IENUM(L, ImGuiKey_A, "Key_A");
	BIND_IENUM(L, ImGuiKey_B, "Key_B");
	BIND_IENUM(L, ImGuiKey_C, "Key_C");
	BIND_IENUM(L, ImGuiKey_D, "Key_D");
	BIND_IENUM(L, ImGuiKey_E, "Key_E");
	BIND_IENUM(L, ImGuiKey_F, "Key_F");
	BIND_IENUM(L, ImGuiKey_G, "Key_G");
	BIND_IENUM(L, ImGuiKey_H, "Key_H");
	BIND_IENUM(L, ImGuiKey_I, "Key_I");
	BIND_IENUM(L, ImGuiKey_J, "Key_J");
	BIND_IENUM(L, ImGuiKey_K, "Key_K");
	BIND_IENUM(L, ImGuiKey_L, "Key_L");
	BIND_IENUM(L, ImGuiKey_M, "Key_M");
	BIND_IENUM(L, ImGuiKey_N, "Key_N");
	BIND_IENUM(L, ImGuiKey_O, "Key_O");
	BIND_IENUM(L, ImGuiKey_P, "Key_P");
	BIND_IENUM(L, ImGuiKey_Q, "Key_Q");
	BIND_IENUM(L, ImGuiKey_R, "Key_R");
	BIND_IENUM(L, ImGuiKey_S, "Key_S");
	BIND_IENUM(L, ImGuiKey_T, "Key_T");
	BIND_IENUM(L, ImGuiKey_U, "Key_U");
	BIND_IENUM(L, ImGuiKey_V, "Key_V");
	BIND_IENUM(L, ImGuiKey_W, "Key_W");
	BIND_IENUM(L, ImGuiKey_X, "Key_X");
	BIND_IENUM(L, ImGuiKey_Y, "Key_Y");
	BIND_IENUM(L, ImGuiKey_Z, "Key_Z");
	BIND_IENUM(L, ImGuiKey_F1, "Key_F1");
	BIND_IENUM(L, ImGuiKey_F2, "Key_F2");
	BIND_IENUM(L, ImGuiKey_F3, "Key_F3");
	BIND_IENUM(L, ImGuiKey_F4, "Key_F4");
	BIND_IENUM(L, ImGuiKey_F5, "Key_F5");
	BIND_IENUM(L, ImGuiKey_F6, "Key_F6");
	BIND_IENUM(L, ImGuiKey_F7, "Key_F7");
	BIND_IENUM(L, ImGuiKey_F8, "Key_F8");
	BIND_IENUM(L, ImGuiKey_F9, "Key_F9");
	BIND_IENUM(L, ImGuiKey_F10, "Key_F10");
	BIND_IENUM(L, ImGuiKey_F11, "Key_F11");
	BIND_IENUM(L, ImGuiKey_F12, "Key_F12");
	BIND_IENUM(L, ImGuiKey_Apostrophe, "Key_Apostrophe");
	BIND_IENUM(L, ImGuiKey_Comma, "Key_Comma");
	BIND_IENUM(L, ImGuiKey_Minus, "Key_Minus");
	BIND_IENUM(L, ImGuiKey_Period, "Key_Period");
	BIND_IENUM(L, ImGuiKey_Slash, "Key_Slash");
	BIND_IENUM(L, ImGuiKey_Semicolon, "Key_Semicolon");
	BIND_IENUM(L, ImGuiKey_Equal, "Key_Equal");
	BIND_IENUM(L, ImGuiKey_LeftBracket, "Key_LeftBracket");
	BIND_IENUM(L, ImGuiKey_Backslash, "Key_Backslash");
	BIND_IENUM(L, ImGuiKey_RightBracket, "Key_RightBracket");
	BIND_IENUM(L, ImGuiKey_GraveAccent, "Key_GraveAccent");
	BIND_IENUM(L, ImGuiKey_CapsLock, "Key_CapsLock");
	BIND_IENUM(L, ImGuiKey_ScrollLock, "Key_ScrollLock");
	BIND_IENUM(L, ImGuiKey_NumLock, "Key_NumLock");
	BIND_IENUM(L, ImGuiKey_PrintScreen, "Key_PrintScreen");
	BIND_IENUM(L, ImGuiKey_Pause, "Key_Pause");
	BIND_IENUM(L, ImGuiKey_Keypad0, "Key_Keypad0");
	BIND_IENUM(L, ImGuiKey_Keypad1, "Key_Keypad1");
	BIND_IENUM(L, ImGuiKey_Keypad2, "Key_Keypad2");
	BIND_IENUM(L, ImGuiKey_Keypad3, "Key_Keypad3");
	BIND_IENUM(L, ImGuiKey_Keypad4, "Key_Keypad4");
	BIND_IENUM(L, ImGuiKey_Keypad5, "Key_Keypad5");
	BIND_IENUM(L, ImGuiKey_Keypad6, "Key_Keypad6");
	BIND_IENUM(L, ImGuiKey_Keypad7, "Key_Keypad7");
	BIND_IENUM(L, ImGuiKey_Keypad8, "Key_Keypad8");
	BIND_IENUM(L, ImGuiKey_Keypad9, "Key_Keypad9");
	BIND_IENUM(L, ImGuiKey_KeypadDecimal, "Key_KeypadDecimal");
	BIND_IENUM(L, ImGuiKey_KeypadDivide, "Key_KeypadDivide");
	BIND_IENUM(L, ImGuiKey_KeypadMultiply, "Key_KeypadMultiply");
	BIND_IENUM(L, ImGuiKey_KeypadSubtract, "Key_KeypadSubtract");
	BIND_IENUM(L, ImGuiKey_KeypadAdd, "Key_KeypadAdd");
	BIND_IENUM(L, ImGuiKey_KeypadEnter, "Key_KeypadEnter");
	BIND_IENUM(L, ImGuiKey_KeypadEqual, "Key_KeypadEqual");

	// Gamepad
	BIND_IENUM(L, ImGuiKey_GamepadStart, "Key_GamepadStart");
	BIND_IENUM(L, ImGuiKey_GamepadBack, "Key_GamepadBack");
	BIND_IENUM(L, ImGuiKey_GamepadFaceUp, "Key_GamepadFaceUp");
	BIND_IENUM(L, ImGuiKey_GamepadFaceDown, "Key_GamepadFaceDown");
	BIND_IENUM(L, ImGuiKey_GamepadFaceLeft, "Key_GamepadFaceLeft");
	BIND_IENUM(L, ImGuiKey_GamepadFaceRight, "Key_GamepadFaceRight");
	BIND_IENUM(L, ImGuiKey_GamepadDpadUp, "Key_GamepadDpadUp");
	BIND_IENUM(L, ImGuiKey_GamepadDpadDown, "Key_GamepadDpadDown");
	BIND_IENUM(L, ImGuiKey_GamepadDpadLeft, "Key_GamepadDpadLeft");
	BIND_IENUM(L, ImGuiKey_GamepadDpadRight, "Key_GamepadDpadRight");
	BIND_IENUM(L, ImGuiKey_GamepadL1, "Key_GamepadL1");
	BIND_IENUM(L, ImGuiKey_GamepadR1, "Key_GamepadR1");
	BIND_IENUM(L, ImGuiKey_GamepadL2, "Key_GamepadL2");
	BIND_IENUM(L, ImGuiKey_GamepadR2, "Key_GamepadR2");
	BIND_IENUM(L, ImGuiKey_GamepadL3, "Key_GamepadL3");
	BIND_IENUM(L, ImGuiKey_GamepadR3, "Key_GamepadR3");
	BIND_IENUM(L, ImGuiKey_GamepadLStickUp, "Key_GamepadLStickUp");
	BIND_IENUM(L, ImGuiKey_GamepadLStickDown, "Key_GamepadLStickDown");
	BIND_IENUM(L, ImGuiKey_GamepadLStickLeft, "Key_GamepadLStickLeft");
	BIND_IENUM(L, ImGuiKey_GamepadLStickRight, "Key_GamepadLStickRight");
	BIND_IENUM(L, ImGuiKey_GamepadRStickUp, "Key_GamepadRStickUp");
	BIND_IENUM(L, ImGuiKey_GamepadRStickDown, "Key_GamepadRStickDown");
	BIND_IENUM(L, ImGuiKey_GamepadRStickLeft, "Key_GamepadRStickLeft");
	BIND_IENUM(L, ImGuiKey_GamepadRStickRight, "Key_GamepadRStickRight");

	// Modifiers
	BIND_IENUM(L, ImGuiKey_ModCtrl, "Key_ModCtrl");
	BIND_IENUM(L, ImGuiKey_ModShift, "Key_ModShift");
	BIND_IENUM(L, ImGuiKey_ModAlt, "Key_ModAlt");
	BIND_IENUM(L, ImGuiKey_ModSuper, "Key_ModSuper");

	BIND_FENUM(L, FLT_MAX, "FLT_MAX");
	BIND_FENUM(L, DBL_MAX, "DBL_MAX");

	lua_pop(L, 1);
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// GidImGui
///
/////////////////////////////////////////////////////////////////////////////////////////////

class EventListener;

class GidImGui
{
public:
	GidImGui(LuaApplication* application, ImFontAtlas* atlas,
			 bool addMouseListeners, bool addKeyboardListeners, bool addTouchListeners);
	~GidImGui();

	EventListener* eventListener;
	ImGuiContext* ctx;
	SpriteProxy* proxy;
	std::vector<CallbackData*> callbacks;

	bool resetTouchPosOnEnd;

	void doDraw(const CurrentTransform&, float sx, float sy, float ex, float ey);
	void clearCallbacks();
private:
	VertexBuffer<Point2f> vertices;
	VertexBuffer<Point2f> texcoords;
	VertexBuffer<VColor> colors;
};

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// EVENT LISTENER
///
/////////////////////////////////////////////////////////////////////////////////////////////

class EventListener : public EventDispatcher
{
private:
	GidImGui* gidImGui;

	void updateModifiers(int modifiers)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddKeyEvent(ImGuiKey_ModAlt, modifiers & GINPUT_ALT_MODIFIER);
		io.AddKeyEvent(ImGuiKey_ModCtrl, modifiers & GINPUT_CTRL_MODIFIER);
		io.AddKeyEvent(ImGuiKey_ModShift, modifiers & GINPUT_SHIFT_MODIFIER);
		io.AddKeyEvent(ImGuiKey_ModSuper, modifiers & GINPUT_META_MODIFIER);
	}

	void keyUpOrDown(int giderosKeyCode, bool state)
	{
		ImGuiIO& io = ImGui::GetIO();
		const ImGuiKey imguiKey = giderosKeyToImGuiKey(giderosKeyCode);
		io.AddKeyEvent(imguiKey, state);
	}

	void mouseUpOrDown(float x, float y, int giderosButton, bool state, int modifiers, float pressure = 0.0f)
	{
		ImGuiKey imguiKey = giderosMouseToImGui(giderosButton);
		ImGuiIO& io = ImGui::GetIO();
		const ImVec2 mpos = translateMousePos(gidImGui->proxy, x, y);
		io.PenPressure = pressure;
		io.AddMouseButtonEvent(imguiKey, state);
		io.AddMousePosEvent(mpos.x, mpos.y);
		updateModifiers(modifiers);
	}

	void scaleMouseCoords(float& x, float& y)
	{
		x = x * r_app_scale.x + app_bounds.x;
		y = y * r_app_scale.y + app_bounds.y;
	}

public:
	ImVec2 r_app_scale;
	ImVec2 app_bounds;

	EventListener(GidImGui* p_gidImGui)
	{
		this->gidImGui = p_gidImGui;
		applicationResize(nullptr);
	}

	~EventListener() { }

	static ImVec2 translateMousePos(Sprite* sprite, float x, float y)
	{
		std::stack<const Sprite*> stack;
		float z;

		const Sprite* curr = sprite;
		while (curr)
		{
			stack.push(curr);
			curr = curr->parent();
		}

		float ox = x;
		float oy = y;
		while (!stack.empty())
		{
			stack.top()->matrix().inverseTransformPoint(x, y, 0, &x, &y, &z);
			// Avoid NaN
			if (x != x)
			{
				x = ox;
				y = oy;
				break;
			}
			stack.pop();
		}
		return ImVec2(x, y);
	}

	void mouseUpOrDown(ginput_Touch touch, bool state)
	{
		//LUA_PRINTF("POS: (%f; %f)\nBTN:%d\nSTATE: %d\nMODS: %d\nPRESSURE: %f\n=================", touch.x, touch.y, touch.mouseButton, state?1:0, touch.modifiers, touch.pressure);
		mouseUpOrDown(touch.x, touch.y, touch.mouseButton, state, touch.modifiers, touch.pressure);
	}

	void mouseUpOrDown(ginput_MouseEvent mouse, bool state)
	{
		mouseUpOrDown(mouse.x, mouse.y, mouse.button, state, mouse.modifiers);
	}

	///////////////////////////////////////////////////
	///
	/// MOUSE
	///
	///////////////////////////////////////////////////

	void mouseDown(MouseEvent* event)
	{
		float x = (float)event->x;
		float y = (float)event->y;
		scaleMouseCoords(x, y);
		mouseUpOrDown(x, y, event->button, true, event->modifiers);
	}

	void mouseUp(MouseEvent* event)
	{
		float x = (float)event->x;
		float y = (float)event->y;
		scaleMouseCoords(x, y);
		mouseUpOrDown(x, y, event->button, false, event->modifiers);
	}

	void mouseMove(float x, float y, int button, int modifiers)
	{
		mouseUpOrDown(x, y, button, true, modifiers);
	}

	void mouseHover(float x, float y, int modifiers)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImVec2 mpos = translateMousePos(gidImGui->proxy, x, y);
		io.AddMousePosEvent(mpos.x, mpos.y);
		updateModifiers(modifiers);
	}

	void mouseHover(MouseEvent* event)
	{
		float x = (float)event->x;
		float y = (float)event->y;
		scaleMouseCoords(x, y);
		mouseHover(x, y, event->modifiers);
	}

	void mouseWheel(float x, float y, int wheel, int modifiers)
	{
		ImGuiIO& io = ImGui::GetIO();
		const ImVec2 mpos = translateMousePos(gidImGui->proxy, x, y);
		io.AddMouseWheelEvent(0.0f, wheel < 0 ? -1.0f : 1.0f);
		io.AddMousePosEvent(mpos.x, mpos.y);
		updateModifiers(modifiers);
	}

	void mouseWheel(MouseEvent* event)
	{
		float x = (float)event->x;
		float y = (float)event->y;
		scaleMouseCoords(x, y);
		mouseWheel(x, y, event->wheel, event->modifiers);
	}

	///////////////////////////////////////////////////
	///
	/// TOUCH
	///
	///////////////////////////////////////////////////

	void touchesBegin(TouchEvent* event)
	{
		ginput_Touch touch = event->event->touch;
		float x = touch.x;
		float y = touch.y;
		scaleMouseCoords(x, y);
		mouseUpOrDown(x, y, touch.mouseButton, true, event->event->touch.modifiers, touch.pressure);
	}

	void touchesEnd(TouchEvent* event)
	{
		ginput_Touch touch = event->event->touch;
		float x;
		float y;
		if (gidImGui->resetTouchPosOnEnd)
		{
			x = FLT_MAX;
			y = FLT_MAX;
		}
		else
		{
			x = touch.x;
			y = touch.y;
		}
		scaleMouseCoords(x, y);
		mouseUpOrDown(x, y, touch.mouseButton, false, touch.modifiers, touch.pressure);
	}

	void touchesMove(TouchEvent* event)
	{
		ginput_Touch touch = event->event->touch;
		float x = touch.x;
		float y = touch.y;
		scaleMouseCoords(x, y);
		mouseUpOrDown(x, y, touch.mouseButton, true, touch.modifiers, touch.pressure);
	}

	void touchesCancel(TouchEvent* event)
	{
		ginput_Touch touch = event->event->touch;
		float x;
		float y;
		if (gidImGui->resetTouchPosOnEnd)
		{
			x = FLT_MAX;
			y = FLT_MAX;
		}
		else
		{
			x = touch.x;
			y = touch.y;
		}
		scaleMouseCoords(x, y);
		mouseUpOrDown(x, y, touch.mouseButton, false, touch.modifiers, touch.pressure);
	}

	///////////////////////////////////////////////////
	///
	/// KEYBAORD
	///
	///////////////////////////////////////////////////

	void keyDown(KeyboardEvent* event)
	{
		keyDown(event->keyCode);
	}

	void keyDown(int keyCode)
	{
		keyUpOrDown(keyCode, true);
	}

	void keyUp(KeyboardEvent* event)
	{
		keyUpOrDown(event->keyCode, false);
	}

	void keyUp(int keyCode)
	{
		keyUpOrDown(keyCode, false);
	}

	void keyChar(KeyboardEvent* event)
	{
		keyChar(event->charCode);
	}

	void keyChar(std::string text)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddInputCharactersUTF8(text.c_str());
	}

	void keyChar2(const char* text) // error when adding event listener to a proxy in GidImGui constructor
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddInputCharactersUTF8(text);
	}

	void applicationResize(Event *)
	{
		lua_getglobal(L, "application");

		lua_getfield(L, -1, "getLogicalScaleX");
		lua_pushvalue(L, -2);
		lua_call(L, 1, 1);
		float sx = lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "getLogicalScaleY");
		lua_pushvalue(L, -2);
		lua_call(L, 1, 1);
		float sy = lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "getLogicalBounds");
		lua_pushvalue(L, -2);
		lua_call(L, 1, 4);
		app_bounds.x = luaL_checknumber(L, -4);
		app_bounds.y = luaL_checknumber(L, -3);
		lua_pop(L, 5);

		r_app_scale.x = 1.0f / sx;
		r_app_scale.y = 1.0f / sy;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////

//static

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// SPRITE PROXY
///
/////////////////////////////////////////////////////////////////////////////////////////////

static void _Draw(void* c, const CurrentTransform&t, float sx, float sy, float ex, float ey)
{
	((GidImGui* ) c)->doDraw(t, sx, sy, ex, ey);
}

static void _Destroy(void* c)
{
	delete ((GidImGui* ) c);
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// CONSREUCTOR / DESTRUCTOR / DRAW
///
/////////////////////////////////////////////////////////////////////////////////////////////

GidImGui::GidImGui(LuaApplication* application, ImFontAtlas* atlas,
				   bool addMouseListeners = true, bool addKeyboardListeners = true, bool addTouchListeners = false)
{
	ctx = ImGui::CreateContext(atlas);
	ImGui::SetCurrentContext(ctx);
	ImGui::SetLuaState(L);
	resetTouchPosOnEnd = false;
	ImGuiIO& io = ImGui::GetIO();
	io.BackendRendererUserData = (void*)this;

	// Setup display size
	io.DisplaySize.x = getAppProperty(L, "getContentWidth");
	io.DisplaySize.y = getAppProperty(L, "getContentHeight");

	io.BackendPlatformName = "Gideros Studio";
	io.BackendRendererName = "Gideros Studio";

	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
	g_id tex = gtexture_create(width, height, GTEXTURE_RGBA, GTEXTURE_UNSIGNED_BYTE, GTEXTURE_CLAMP, GTEXTURE_LINEAR, pixels, NULL, 0);
	io.Fonts->SetTexID((ImTextureID)(uintptr_t)tex);

	proxy = gtexture_get_spritefactory()->createProxy(application->getApplication(), this, _Draw, _Destroy);

	eventListener = new EventListener(this);

	if (addMouseListeners)
	{
		proxy->addEventListener(MouseEvent::MOUSE_DOWN,     eventListener, &EventListener::mouseDown);
		proxy->addEventListener(MouseEvent::MOUSE_UP,       eventListener, &EventListener::mouseUp);
		proxy->addEventListener(MouseEvent::MOUSE_MOVE,     eventListener, &EventListener::mouseDown);
		proxy->addEventListener(MouseEvent::MOUSE_HOVER,    eventListener, &EventListener::mouseHover);
		proxy->addEventListener(MouseEvent::MOUSE_WHEEL,    eventListener, &EventListener::mouseWheel);
	}

	if (addTouchListeners)
	{
		proxy->addEventListener(TouchEvent::TOUCHES_BEGIN,  eventListener, &EventListener::touchesBegin);
		proxy->addEventListener(TouchEvent::TOUCHES_END,    eventListener, &EventListener::touchesEnd);
		proxy->addEventListener(TouchEvent::TOUCHES_MOVE,   eventListener, &EventListener::touchesMove);
		proxy->addEventListener(TouchEvent::TOUCHES_CANCEL, eventListener, &EventListener::touchesCancel);
	}

	if (addKeyboardListeners)
	{
		proxy->addEventListener(KeyboardEvent::KEY_DOWN,    eventListener, &EventListener::keyDown);
		proxy->addEventListener(KeyboardEvent::KEY_UP,      eventListener, &EventListener::keyUp);
		proxy->addEventListener(KeyboardEvent::KEY_CHAR,    eventListener, &EventListener::keyChar);
	}

	proxy->addEventListener(Event::APPLICATION_RESIZE,  eventListener, &EventListener::applicationResize);
}

GidImGui::~GidImGui()
{
}

void GidImGui::doDraw(const CurrentTransform&, float _UNUSED(sx), float _UNUSED(sy), float _UNUSED(ex), float _UNUSED(ey))
{
	ImGuiViewportP* viewport = ctx->Viewports[0];
	if (!viewport->DrawDataP.Valid) return;
	ImDrawData* draw_data = &viewport->DrawDataP;

	ShaderEngine* engine = gtexture_get_engine();
	ShaderProgram* shp = engine->getDefault(ShaderEngine::STDP_TEXTURECOLOR);
	ImVec2 pos = draw_data->DisplayPos;

	int offset_pos = offsetof(ImDrawVert, pos);
	int offset_uv = offsetof(ImDrawVert, uv);
	int offset_col = offsetof(ImDrawVert, col);

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;  // vertex buffer generated by Dear ImGui
		const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;   // index buffer generated by Dear ImGui

		size_t vtx_size = cmd_list->VtxBuffer.Size;

		vertices.resize(vtx_size);
		texcoords.resize(vtx_size);
		colors.resize(vtx_size);

		for (size_t i = 0; i < vtx_size; i++)
		{
			vertices[i].x = vtx_buffer[i].pos.x;
			vertices[i].y = vtx_buffer[i].pos.y;
			texcoords[i].x = vtx_buffer[i].uv.x;
			texcoords[i].y = vtx_buffer[i].uv.y;

			uint32_t c = vtx_buffer[i].col;

			uint32_t r = (c >> IM_COL32_R_SHIFT) & 0xFF;
			uint32_t g = (c >> IM_COL32_G_SHIFT) & 0xFF;
			uint32_t b = (c >> IM_COL32_B_SHIFT) & 0xFF;
			uint32_t a = (c >> IM_COL32_A_SHIFT) & 0xFF;

			colors[i].r = (r * a) >> 8;
			colors[i].g = (g * a) >> 8;
			colors[i].b = (b * a) >> 8;
			colors[i].a = a;
		}
		vertices.Update();
		texcoords.Update();
		colors.Update();

		shp->setData(ShaderProgram::DataVertex, ShaderProgram::DFLOAT,2, &vertices[0], vtx_size, true, NULL);
		shp->setData(ShaderProgram::DataTexture, ShaderProgram::DFLOAT,2, &texcoords[0], vtx_size, true, NULL);
		shp->setData(ShaderProgram::DataColor, ShaderProgram::DUBYTE,4, &colors[0], vtx_size, true, NULL);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				g_id textureId = (g_id)(uintptr_t)pcmd->GetTexID();

				engine->bindTexture(0, gtexture_getInternalTexture(textureId));

				engine->pushClip(
							(int)(pcmd->ClipRect.x - pos.x),
							(int)(pcmd->ClipRect.y - pos.y),
							(int)(pcmd->ClipRect.z - pcmd->ClipRect.x),
							(int)(pcmd->ClipRect.w - pcmd->ClipRect.y)
							);
				shp->drawElements(ShaderProgram::Triangles, pcmd->ElemCount, ShaderProgram::DINT, idx_buffer + pcmd->IdxOffset, true, NULL);
				engine->popClip();
			}
		}

	}

}

void GidImGui::clearCallbacks()
{
	for (CallbackData* data : callbacks)
	{
		delete data;
	}
	callbacks.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////

inline GidImGui* getImgui(lua_State* L, int index = 1)
{
	SpriteProxy* sprite = getPtr<SpriteProxy>(L, "ImGui", index);
	return (GidImGui*)sprite->getContext();
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// ImGui create / destroy
///
/////////////////////////////////////////////////////////////////////////////////////////////

int initImGui(lua_State* L) // ImGui.new() call
{
	LuaApplication* application = static_cast<LuaApplication*>(luaL_getdata(L));
	::application = application->getApplication();

	ImFontAtlas* atlas = NULL;
	if (g_isInstanceOf(L, "ImFontAtlas", 1))
	{
		atlas = getPtr<ImFontAtlas>(L, "ImFontAtlas");
	}

	GidImGui* imgui = new GidImGui(application, atlas, luaL_optboolean(L, 2, 1), luaL_optboolean(L, 3, 1), luaL_optboolean(L, 4, 0));
	g_pushInstance(L, "ImGui", imgui->proxy);

	luaL_rawgetptr(L, LUA_REGISTRYINDEX, &keyWeak);
	lua_pushvalue(L, -2);
	luaL_rawsetptr(L, -2, imgui);
	lua_pop(L, 1);

	return 1;
}

int destroyImGui(LUA_STATE* p)
{
	void* ptr = GIDEROS_DTOR_UDATA(p);
	GidImGui* imgui = static_cast<GidImGui*>(static_cast<SpriteProxy *>(ptr)->getContext());

	if (imgui->ctx->IO.Fonts->TexID)
	{
		gtexture_delete((g_id)(uintptr_t)imgui->ctx->IO.Fonts->TexID);
	}

	ImGui::DestroyContext(imgui->ctx);

	imgui->proxy->removeEventListeners(imgui->eventListener);
	imgui->eventListener->removeEventListeners();
	imgui->clearCallbacks();
	delete imgui->eventListener;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// BINDINGS.
///
/////////////////////////////////////////////////////////////////////////////////////////////

/// MOUSE INPUTS

int MouseHover(lua_State* L)
{
	STACK_CHECKER(L, "onMouseHover", 0);

	GidImGui* imgui = getImgui(L);
	ginput_MouseEvent mouse = getMouseInfo(L);
	imgui->eventListener->mouseHover(mouse.x, mouse.y, mouse.modifiers);
	return 0;
}

int MouseMove(lua_State* L)
{
	STACK_CHECKER(L, "onMouseMove", 0);

	GidImGui* imgui = getImgui(L);
	ginput_MouseEvent mouse = getMouseInfo(L);
	imgui->eventListener->mouseUpOrDown(mouse, true);
	return 0;
}

int MouseDown(lua_State* L)
{
	STACK_CHECKER(L, "onMouseDown", 0);

	GidImGui* imgui = getImgui(L);
	ginput_MouseEvent mouse = getMouseInfo(L);
	imgui->eventListener->mouseUpOrDown(mouse, true);
	return 0;
}

int MouseUp(lua_State* L)
{
	STACK_CHECKER(L, "onMouseUp", 0);

	GidImGui* imgui = getImgui(L);
	ginput_MouseEvent mouse = getMouseInfo(L);
	imgui->eventListener->mouseUpOrDown(mouse, false);
	return 0;
}

int MouseWheel(lua_State* L)
{
	STACK_CHECKER(L, "onMouseWheel", 0);

	GidImGui* imgui = getImgui(L);
	ginput_MouseEvent mouse = getMouseInfo(L);
	imgui->eventListener->mouseWheel(mouse.x, mouse.y, mouse.wheel, mouse.modifiers);
	return 0;
}

/// TOUCH INPUT

int TouchCancel(lua_State* L)
{
	STACK_CHECKER(L, "onTouchCancel", 0);

	GidImGui* imgui = getImgui(L);
	ginput_Touch touch = getTouchInfo(L);
	imgui->eventListener->mouseUpOrDown(touch, false);
	return 0;
}

int TouchMove(lua_State* L)
{
	STACK_CHECKER(L, "onTouchMove", 0);

	GidImGui* imgui = getImgui(L);
	ginput_Touch touch = getTouchInfo(L);
	imgui->eventListener->mouseUpOrDown(touch, true);
	return 0;
}

int TouchBegin(lua_State* L)
{
	STACK_CHECKER(L, "onTouchBegin", 0);

	GidImGui* imgui = getImgui(L);
	ginput_Touch touch = getTouchInfo(L);
	imgui->eventListener->mouseUpOrDown(touch, true);
	return 0;
}

int TouchEnd(lua_State* L)
{
	STACK_CHECKER(L, "onTouchEnd", 0);

	GidImGui* imgui = getImgui(L);
	ginput_Touch touch = getTouchInfo(L);
	imgui->eventListener->mouseUpOrDown(touch, false);
	return 0;
}

/// KEYBOARD INPUTS

int KeyUp(lua_State* L)
{
	STACK_CHECKER(L, "onKeyUp", 0);

	GidImGui* imgui = getImgui(L);

	lua_getfield(L, -1, "keyCode");
	int keyCode = lua_tointeger(L, -1);
	lua_pop(L, 1);

	imgui->eventListener->keyUp(keyCode);

	return 0;
}

int KeyDown(lua_State* L)
{
	STACK_CHECKER(L, "onKeyDown", 0);

	GidImGui* imgui = getImgui(L);

	lua_getfield(L, -1, "keyCode");
	int keyCode = lua_tointeger(L, -1);
	lua_pop(L, 1);

	imgui->eventListener->keyDown(keyCode);

	return 0;
}

int KeyChar(lua_State* L)
{
	STACK_CHECKER(L, "onKeyChar", 0);

	GidImGui* imgui = getImgui(L);

	lua_pushstring(L, "text");
	lua_gettable(L, -2);
	const char* text = lua_tostring(L, -1);
	lua_pop(L, 1);

	imgui->eventListener->keyChar2(text);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// DRAWING STUFF
///
/////////////////////////////////////////////////////////////////////////////////////////////

int NewFrame(lua_State* L)
{
	STACK_CHECKER(L, "newFrame", 0);

	GidImGui* imgui = getImgui(L);

	double deltaTime = luaL_checknumber(L, 2);
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	ImGui::NewFrame();
	return 0;
}

int Render(lua_State* L)
{
	STACK_CHECKER(L, "render", 0);

	ImGui::Render();
	return 0;
}

int EndFrame(lua_State* L)
{
	STACK_CHECKER(L, "endFrame", 0);

	GidImGui* imgui = getImgui(L);
	imgui->clearCallbacks();
	ImGui::EndFrame();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Windows
///
/////////////////////////////////////////////////////////////////////////////////////////////

int Begin(lua_State* L)
{

	const char* name = luaL_checkstring(L, 2);
	ImGuiWindowFlags flags = luaL_optinteger(L, 4, 0);
	if (lua_isnoneornil(L, 3))
	{
		STACK_CHECKER(L, "beginWindow", 1);
		lua_pushboolean(L, ImGui::Begin(name, NULL, flags));
		return 1;
	}
	STACK_CHECKER(L, "beginWindow", 2);
	bool p_open = lua_toboolean(L, 3) > 0;
	bool draw_flag = ImGui::Begin(name, &p_open, flags);
	lua_pushboolean(L, p_open);
	lua_pushboolean(L, draw_flag);
	return 2;
}

int End(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "endWindow", 0);

	ImGui::End();
	return 0;
}

// @MultiPain
int BeginFullScreenWindow(lua_State* L)
{

	GidImGui* imgui = getImgui(L);
	const char* name = luaL_checkstring(L, 2);
	ImGuiWindowFlags flags = luaL_optinteger(L, 4, 0);
	flags |= ImGuiWindowFlags_FullScreen;

	ImGuiIO& IO = ImGui::GetIO();

	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(IO.DisplaySize);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);


	if (lua_isnoneornil(L, 3))
	{
		STACK_CHECKER(L, "beginFullScreenWindow", 1);
		bool draw_flag = ImGui::Begin(name, NULL, flags);
		ImGui::PopStyleVar(2);
		lua_pushboolean(L, draw_flag);
		return 1;
	}
	else
	{
		STACK_CHECKER(L, "beginFullScreenWindow", 2);
		bool p_open = lua_toboolean(L, 3);
		bool draw_flag = ImGui::Begin(name, &p_open, flags);
		ImGui::PopStyleVar(2);
		lua_pushboolean(L, p_open);
		lua_pushboolean(L, draw_flag);
		return 2;
	}
}

int BeginDisabled(lua_State* L)
{
	STACK_CHECKER(L, "beginDisabled", 1);

	int disabled = lua_toboolean(L, 2);
	ImGui::BeginDisabled(disabled);
	return 0;
}

int EndDisabled(lua_State* L)
{
	STACK_CHECKER(L, "endDisabled", 1);

	ImGui::EndDisabled();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Child Windows
///
/////////////////////////////////////////////////////////////////////////////////////////////

int BeginChild(lua_State* L)
{
	STACK_CHECKER(L, "beginChild", 1);

	ImVec2 size = luaL_optvec2(L, 3);
	bool border = luaL_optboolean(L, 5, 0);
	ImGuiWindowFlags flags = luaL_optinteger(L, 6, 0);
	bool result;

	if (lua_type(L, 2) == LUA_TSTRING)
	{
		const char* str_id = luaL_checkstring(L, 2);
		result = ImGui::BeginChild(str_id, size, border, flags);
	}
	else
	{
		ImGuiID id = checkID(L);
		result = ImGui::BeginChild(id, size, border, flags);
	}

	lua_pushboolean(L, result);
	return 1;
}

int EndChild(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "endChild", 0);

	ImGui::EndChild();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Windows Utilities
///
/////////////////////////////////////////////////////////////////////////////////////////////

int IsWindowAppearing(lua_State* L)
{
	STACK_CHECKER(L, "isWindowAppearing", 1);

	lua_pushboolean(L, ImGui::IsWindowAppearing());
	return 1;
}

int IsWindowCollapsed(lua_State* L)
{
	STACK_CHECKER(L, "isWindowCollapsed", 1);

	lua_pushboolean(L, ImGui::IsWindowCollapsed());
	return 1;
}

int IsWindowFocused(lua_State* L)
{
	STACK_CHECKER(L, "isWindowFocused", 1);

	ImGuiFocusedFlags flag = luaL_optinteger(L, 2, 0);
	lua_pushboolean(L, ImGui::IsWindowFocused(flag));
	return 1;
}

int IsWindowHovered(lua_State* L)
{
	STACK_CHECKER(L, "isWindowHovered", 1);

	ImGuiHoveredFlags flag = luaL_optinteger(L, 2, 0);
	lua_pushboolean(L, ImGui::IsWindowHovered(flag));
	return 1;
}

int GetWindowPos(lua_State* L)
{
	STACK_CHECKER(L, "getWindowPos", 2);

	ImVec2 pos = ImGui::GetWindowPos();
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	return 2;
}

int GetWindowSize(lua_State* L)
{
	STACK_CHECKER(L, "getWindowSize", 2);

	ImVec2 size = ImGui::GetWindowSize();
	lua_pushnumber(L, size.x);
	lua_pushnumber(L, size.y);
	return  2;
}

int GetWindowWidth(lua_State* L)
{
	STACK_CHECKER(L, "getWindowWidth", 1);

	lua_pushnumber(L, ImGui::GetWindowWidth());
	return  1;
}

int GetWindowHeight(lua_State* L)
{
	STACK_CHECKER(L, "getWindowHeight", 1);

	lua_pushnumber(L, ImGui::GetWindowHeight());
	return  1;
}

// @MultiPain

int GetWindowBounds(lua_State* L)
{
	STACK_CHECKER(L, "getWindowBounds", 4);

	GidImGui* gidImgui = getImgui(L);

	ImVec2 vMin = ImGui::GetWindowContentRegionMin();
	ImVec2 vMax = ImGui::GetWindowContentRegionMax();
	ImVec2 pos = ImGui::GetWindowPos();
	vMin += pos;
	vMax += pos;

	float x1, y1, x2, y2;

	localToGlobal(gidImgui->proxy, vMin.x, vMin.y, &x1, &y1);
	localToGlobal(gidImgui->proxy, vMax.x, vMax.y, &x2, &y2);

	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	lua_pushnumber(L, x2);
	lua_pushnumber(L, y2);

	return 4;
}


int SetNextWindowPos(lua_State* L)
{
	STACK_CHECKER(L, "setNextWindowPos", 0);

	ImVec2 pos = luaL_checkvec2(L, 2);
	ImGuiCond cond = luaL_optinteger(L, 4, 0);
	ImVec2 pivot = luaL_optvec2(L, 5);

	ImGui::SetNextWindowPos(pos, cond, pivot);

	return 0;
}

int SetNextWindowSize(lua_State* L)
{
	STACK_CHECKER(L, "setNextWindowSize", 0);

	ImVec2 size = luaL_checkvec2(L, 2);
	ImGuiCond cond = luaL_optinteger(L, 4, 0);

	ImGui::SetNextWindowSize(size, cond);

	return 0;
}

int SetNextWindowSizeConstraints(lua_State* L)
{
	GidImGui* imgui = getImgui(L);
	ImVec2 size_min = luaL_checkvec2(L, 2);
	ImVec2 size_max = luaL_checkvec2(L, 4);

	if (lua_type(L, 6) == LUA_TFUNCTION)
	{
		CallbackData* callback = new CallbackData(L, 6);
		imgui->callbacks.push_back(callback);

		ImGui::SetNextWindowSizeConstraints(size_min, size_max, NextWindowSizeConstraintCallback, (void *)callback);
	}
	else
	{
		ImGui::SetNextWindowSizeConstraints(size_min, size_max);
	}
	return 0;
}

int SetNextWindowContentSize(lua_State* L)
{
	STACK_CHECKER(L, "setNextWindowContentSize", 0);

	const ImVec2 size = luaL_checkvec2(L, 2);

	ImGui::SetNextWindowContentSize(size);

	return 0;
}

int SetNextWindowCollapsed(lua_State* L)
{
	STACK_CHECKER(L, "setNextWindowCollapsed", 0);

	bool collapsed = lua_toboolean(L, 2) > 0;
	ImGuiCond cond = luaL_optinteger(L, 3, 0);

	ImGui::SetNextWindowCollapsed(collapsed, cond);
	return 0;
}

int SetNextWindowFocus(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "setNextWindowFocus", 0);

	ImGui::SetNextWindowFocus();
	return 0;
}

int SetNextWindowBgAlpha(lua_State* L)
{
	STACK_CHECKER(L, "setNextWindowBgAlpha", 0);

	double alpha = luaL_checknumber(L, 2);
	ImGui::SetNextWindowBgAlpha(alpha);
	return 0;
}

int SetWindowPos(lua_State* L)
{
	STACK_CHECKER(L, "setWindowPos", 0);

	if (lua_type(L, 2) == LUA_TSTRING)
	{
		const char* name = luaL_checkstring(L, 2);
		const ImVec2 pos = luaL_checkvec2(L, 3);
		ImGuiCond cond = luaL_optinteger(L, 5, 0);

		ImGui::SetWindowPos(name, pos, cond);
	}
	else
	{
		const ImVec2 pos = luaL_checkvec2(L, 2);
		ImGuiCond cond = luaL_optinteger(L, 4, 0);

		ImGui::SetWindowPos(pos, cond);
	}

	return  0;
}

int SetWindowSize(lua_State* L)
{
	STACK_CHECKER(L, "setWindowSize", 0);

	if (lua_type(L, 2) == LUA_TSTRING)
	{
		const char* name = luaL_checkstring(L, 2);
		const ImVec2 size = luaL_checkvec2(L, 3);
		ImGuiCond cond = luaL_optinteger(L, 5, 0);

		ImGui::SetWindowSize(name, size, cond);
	}
	else
	{
		const ImVec2 size = luaL_checkvec2(L, 2);
		ImGuiCond cond = luaL_optinteger(L, 4, 0);

		ImGui::SetWindowSize(size, cond);
	}

	return 0;
}

int SetWindowCollapsed(lua_State* L)
{
	STACK_CHECKER(L, "setWindowCollapsed", 0);

	if (lua_type(L, 2) == LUA_TSTRING)
	{
		const char* name = luaL_checkstring(L, 2);
		bool collapsed = lua_toboolean(L, 3);
		ImGuiCond cond = luaL_optinteger(L, 4, 0);

		ImGui::SetWindowCollapsed(name, collapsed, cond);
	}
	else
	{
		bool collapsed = lua_toboolean(L, 2);
		ImGuiCond cond = luaL_optinteger(L, 3, 0);

		ImGui::SetWindowCollapsed(collapsed, cond);
	}

	return 0;
}

int SetWindowFocus(lua_State* L)
{
	STACK_CHECKER(L, "setWindowFocus", 0);

	if (lua_type(L, 2) == LUA_TSTRING)
	{
		const char* name = luaL_checkstring(L, 2);
		ImGui::SetWindowFocus(name);
	}
	else
		ImGui::SetWindowFocus();

	return 0;
}

int SetWindowFontScale(lua_State* L)
{
	STACK_CHECKER(L, "setWindowFontScale", 0);

	double scale = luaL_checknumber(L, 2);
	ImGui::SetWindowFontScale(scale);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Content region
/// Those functions are bound to be redesigned soon (they are confusing, incomplete and return values in local window coordinates which increases confusion)
///
/////////////////////////////////////////////////////////////////////////////////////////////
int GetContentRegionMax(lua_State* L)
{
	STACK_CHECKER(L, "getContentRegionMax", 2);

	ImVec2 max = ImGui::GetContentRegionMax();
	lua_pushnumber(L, max.x);
	lua_pushnumber(L, max.y);
	return 2;
}

int GetContentRegionAvail(lua_State* L)
{
	STACK_CHECKER(L, "getContentRegionAvail", 2);

	ImVec2 avail = ImGui::GetContentRegionAvail();
	lua_pushnumber(L, avail.x);
	lua_pushnumber(L, avail.y);
	return 2;
}

int GetWindowContentRegionMin(lua_State* L)
{
	STACK_CHECKER(L, "getWindowContentRegionMin", 2);

	ImVec2 min = ImGui::GetWindowContentRegionMin();
	lua_pushnumber(L, min.x);
	lua_pushnumber(L, min.y);
	return 2;
}

int GetWindowContentRegionMax(lua_State* L)
{
	STACK_CHECKER(L, "getWindowContentRegionMax", 2);

	ImVec2 max = ImGui::GetWindowContentRegionMax();
	lua_pushnumber(L, max.x);
	lua_pushnumber(L, max.y);
	return 2;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Windows Scrolling
///
/////////////////////////////////////////////////////////////////////////////////////////////
int GetScrollX(lua_State* L)
{
	STACK_CHECKER(L, "getScrollX", 1);

	lua_pushnumber(L, ImGui::GetScrollX());
	return 1;
}

int GetScrollY(lua_State* L)
{
	STACK_CHECKER(L, "getScrollY", 1);

	lua_pushnumber(L, ImGui::GetScrollY());
	return 1;
}

int GetScrollMaxX(lua_State* L)
{
	STACK_CHECKER(L, "getScrollMaxX", 1);

	lua_pushnumber(L, ImGui::GetScrollMaxX());
	return 1;
}

int GetScrollMaxY(lua_State* L)
{
	STACK_CHECKER(L, "getScrollMaxY", 1);

	lua_pushnumber(L, ImGui::GetScrollMaxY());
	return 1;
}

int SetScrollX(lua_State* L)
{
	STACK_CHECKER(L, "setScrollX", 0);

	double scroll_x = luaL_checknumber(L, 2);
	ImGui::SetScrollX(scroll_x);
	return 0;
}

int SetScrollY(lua_State* L)
{
	STACK_CHECKER(L, "setScrollY", 0);

	double scroll_y = luaL_checknumber(L, 2);
	ImGui::SetScrollY(scroll_y);
	return 0;
}

int SetScrollHereX(lua_State* L)
{
	STACK_CHECKER(L, "setScrollHereX", 0);

	double center_x_ratio = luaL_optnumber(L, 2, 0.5f);
	ImGui::SetScrollHereX(center_x_ratio);
	return 0;
}

int SetScrollHereY(lua_State* L)
{
	STACK_CHECKER(L, "setScrollHereY", 0);

	double center_y_ratio = luaL_optnumber(L, 2, 0.5f);
	ImGui::SetScrollHereY(center_y_ratio);
	return 0;
}

int SetScrollFromPosX(lua_State* L)
{
	STACK_CHECKER(L, "setScrollFromPosX", 0);

	double local_x = luaL_checknumber(L, 2);
	double center_x_ratio = luaL_optnumber(L, 3, 0.5f);
	ImGui::SetScrollFromPosX(local_x, center_x_ratio);
	return 0;
}

int SetScrollFromPosY(lua_State* L)
{
	STACK_CHECKER(L, "setScrollFromPosY", 0);

	double local_y = luaL_checknumber(L, 2);
	double center_y_ratio = luaL_optnumber(L, 3, 0.5f);
	ImGui::SetScrollFromPosY(local_y, center_y_ratio);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Parameters stacks (shared)
///
/////////////////////////////////////////////////////////////////////////////////////////////

int PushStyleColor(lua_State* L)
{
	STACK_CHECKER(L, "pushStyleColor", 0);

	ImGuiCol idx = luaL_checkinteger(L, 2);
	int hex = luaL_checkinteger(L, 3);
	float alpha = luaL_optnumber(L, 4, 1.0f);

	ImGui::PushStyleColor(idx, GColor::toU32(hex, alpha));

	return 0;
}

int PopStyleColor(lua_State* L)
{
	STACK_CHECKER(L, "popStyleColor", 0);

	int count = luaL_optinteger(L, 2, 1);
	ImGui::PopStyleColor(count);
	return 0;
}

int PushStyleVar(lua_State* L)
{
	STACK_CHECKER(L, "pushStyleVar", 0);

	ImGuiStyleVar idx = luaL_checkinteger(L, 2);

	if (lua_gettop(L) > 3)
	{
		double vx = luaL_checknumber(L, 3);
		double vy = luaL_checknumber(L, 4);
		ImGui::PushStyleVar(idx, ImVec2(vx, vy));
	}
	else
	{
		double val = luaL_checknumber(L, 3);
		ImGui::PushStyleVar(idx, val);
	}

	return 0;
}

int PopStyleVar(lua_State* L)
{
	STACK_CHECKER(L, "popStyleVar", 0);

	int count = luaL_optinteger(L, 2, 1);
	ImGui::PopStyleVar(count);
	return 0;
}

int GetFont(lua_State* L)
{
	STACK_CHECKER(L, "getFont", 1);

	g_pushInstance(L, "ImFont", ImGui::GetFont());
	return 1;
}

int GetFontSize(lua_State* L)
{
	STACK_CHECKER(L, "getFontSize", 1);

	lua_pushnumber(L, ImGui::GetFontSize());
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Parameters stacks (current window)
///
/////////////////////////////////////////////////////////////////////////////////////////////

int PushItemWidth(lua_State* L)
{
	STACK_CHECKER(L, "pushItemWidth", 0);

	double item_width = luaL_checknumber(L, 2);
	ImGui::PushItemWidth(item_width);
	return 0;
}

int PopItemWidth(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "popItemWidth", 0);

	ImGui::PopItemWidth();
	return 0;
}

int PushItemFlag(lua_State* L)
{
	STACK_CHECKER(L, "pushItemFlag", 0);

	int option = luaL_checkinteger(L, 2);
	ImGui::PushItemFlag(option, lua_toboolean(L, 3));
	return 0;
}

int PopItemFlag(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "popItemFlag", 0);

	ImGui::PopItemFlag();
	return 0;
}

int SetNextItemWidth(lua_State* L)
{
	STACK_CHECKER(L, "setNextItemWidth", 0);

	double item_width = luaL_checknumber(L, 2);
	ImGui::SetNextItemWidth(item_width);
	return 0;
}

int CalcItemWidth(lua_State* L)
{
	STACK_CHECKER(L, "calcItemWidth", 1);

	lua_pushnumber(L, ImGui::CalcItemWidth());
	return 1;
}

int PushTextWrapPos(lua_State* L)
{
	STACK_CHECKER(L, "pushTextWrapPos", 0);

	double wrap_local_pos_x = luaL_optnumber(L, 2, 0.0f);
	ImGui::PushTextWrapPos(wrap_local_pos_x);
	return 0;
}

int PopTextWrapPos(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "popTextWrapPos", 0);

	ImGui::PopTextWrapPos();
	return 0;
}

int PushAllowKeyboardFocus(lua_State* L)
{
	STACK_CHECKER(L, "pushAllowKeyboardFocus", 0);

	bool allow_keyboard_focus = lua_toboolean(L, 2);
	ImGui::PushAllowKeyboardFocus(allow_keyboard_focus);
	return 0;
}

int PopAllowKeyboardFocus(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "popAllowKeyboardFocus", 0);

	ImGui::PopAllowKeyboardFocus();
	return 0;
}

int PushButtonRepeat(lua_State* L)
{
	STACK_CHECKER(L, "pushButtonRepeat", 0);

	bool repeat = lua_toboolean(L, 2);
	ImGui::PushButtonRepeat(repeat);
	return 0;
}

int PopButtonRepeat(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "popButtonRepeat", 0);

	ImGui::PopButtonRepeat();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Cursor / Layout
///
/////////////////////////////////////////////////////////////////////////////////////////////

int Separator(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "separator", 0);

	ImGui::Separator();
	return 0;
}

int SameLine(lua_State* L)
{
	STACK_CHECKER(L, "sameLine", 0);

	double offset_from_start_x = luaL_optnumber(L, 2, 0.0f);
	double spacing = luaL_optnumber(L, 3, -1.0f);
	ImGui::SameLine(offset_from_start_x, spacing);
	return 0;
}

int NewLine(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "newLine", 0);

	ImGui::NewLine();
	return 0;
}

int Spacing(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "spacing", 0);

	ImGui::Spacing();
	return 0;
}

int Dummy(lua_State* L)
{
	STACK_CHECKER(L, "dummy", 0);

	double w = luaL_checknumber(L, 2);
	double h = luaL_checknumber(L, 3);

	ImGui::Dummy(ImVec2(w, h));
	return 0;
}

int Indent(lua_State* L)
{
	STACK_CHECKER(L, "indent", 0);

	double indent_w = luaL_optnumber(L, 2, 0.0f);
	ImGui::Indent(indent_w);
	return 0;
}

int Unindent(lua_State* L)
{
	STACK_CHECKER(L, "unindent", 0);

	double indent_w = luaL_optnumber(L, 2, 0.0f);
	ImGui::Unindent(indent_w);
	return 0;
}

int BeginGroup(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "beginGroup", 0);

	ImGui::BeginGroup();
	return 0;
}

int EndGroup(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "endGroup", 0);

	ImGui::EndGroup();
	return 0;
}

int GetCursorPos(lua_State* L)
{
	STACK_CHECKER(L, "getCursorPos", 2);

	ImVec2 pos = ImGui::GetCursorPos();
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	return 2;
}

int GetCursorPosX(lua_State* L)
{
	STACK_CHECKER(L, "getCursorPosX", 1);

	lua_pushnumber(L, ImGui::GetCursorPosX());
	return 1;
}

int GetCursorPosY(lua_State* L)
{
	STACK_CHECKER(L, "getCursorPosY", 1);

	lua_pushnumber(L, ImGui::GetCursorPosY());
	return 1;
}

int SetCursorPos(lua_State* L)
{
	STACK_CHECKER(L, "setCursorPos", 0);

	ImVec2 local_pos = luaL_checkvec2(L, 2);
	ImGui::SetCursorPos(local_pos);
	return 0;
}

int SetCursorPosX(lua_State* L)
{
	STACK_CHECKER(L, "setCursorPosX", 0);

	double local_x = luaL_checknumber(L, 2);
	ImGui::SetCursorPosX(local_x);
	return 0;
}

int SetCursorPosY(lua_State* L)
{
	STACK_CHECKER(L, "setCursorPosY", 0);

	double local_y = luaL_checknumber(L, 2);
	ImGui::SetCursorPosY(local_y);
	return 0;
}

int GetCursorStartPos(lua_State* L)
{
	STACK_CHECKER(L, "getCursorStartPos", 2);

	ImVec2 pos = ImGui::GetCursorStartPos();
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	return 2;
}

int GetCursorScreenPos(lua_State* L)
{
	STACK_CHECKER(L, "getCursorScreenPos", 2);

	ImVec2 pos = ImGui::GetCursorScreenPos();
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	return 2;
}

int SetCursorScreenPos(lua_State* L)
{
	STACK_CHECKER(L, "setCursorScreenPos", 0);

	ImVec2 pos = luaL_checkvec2(L, 2);
	ImGui::SetCursorScreenPos(pos);
	return 0;
}

int AlignTextToFramePadding(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "alignTextToFramePadding", 0);

	ImGui::AlignTextToFramePadding();
	return 0;
}

int GetTextLineHeight(lua_State* L)
{
	STACK_CHECKER(L, "getTextLineHeight", 1);

	lua_pushnumber(L, ImGui::GetTextLineHeight());
	return 1;
}

int GetTextLineHeightWithSpacing(lua_State* L)
{
	STACK_CHECKER(L, "getTextLineHeightWithSpacing", 1);

	lua_pushnumber(L, ImGui::GetTextLineHeightWithSpacing());
	return 1;
}

int GetFrameHeight(lua_State* L)
{
	STACK_CHECKER(L, "getFrameHeight", 1);

	lua_pushnumber(L, ImGui::GetFrameHeight());
	return 1;
}

int GetFrameHeightWithSpacing(lua_State* L)
{
	STACK_CHECKER(L, "getFrameHeightWithSpacing", 1);

	lua_pushnumber(L, ImGui::GetFrameHeightWithSpacing());
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// ID
///
/////////////////////////////////////////////////////////////////////////////////////////////

int PushID(lua_State* L)
{
	STACK_CHECKER(L, "pushID", 0);

	if (lua_gettop(L) == 2)
	{
		const int arg_type = lua_type(L, 2);
		switch(arg_type)
		{
		case(LUA_TNIL):
		{
			LUA_THROW_ERROR("bad argument #2 to 'pushID' (string/number/table/function expected, got nil)");
		}
			break;
		case(LUA_TSTRING):
			ImGui::PushID(luaL_checkstring(L, 2));
			break;
		case(LUA_TNUMBER):
			ImGui::PushID(luaL_checknumber(L, 2));
			break;
		default:
			ImGui::PushID(lua_topointer(L, 2));
			break;
		}
	}
	else
	{
		LUA_THROW_ERROR("bar argument #2 to 'pushID'");
	}
	return 0;
}

int PopID(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "popID", 0);

	ImGui::PopID();
	return 0;
}

int GetID(lua_State* L)
{
	STACK_CHECKER(L, "getID", 1);

	switch(lua_type(L, 2))
	{
	case LUA_TSTRING:
	{
		const char* str_id = luaL_checkstring(L, 2);
		ImGuiID id = ImGui::GetID(str_id);
		lua_pushnumber(L, static_cast<double>(id));
	}
		break;
	default:
	{
		ImGuiID id = ImGui::GetID(lua_topointer(L, 2));
		lua_pushnumber(L, static_cast<double>(id));
	}
		break;
	}

	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Widgets: Text
///
/////////////////////////////////////////////////////////////////////////////////////////////

int Text(lua_State* L)
{
	STACK_CHECKER(L, "text", 0);

	const char* text = luaL_checkstring(L, 2);
	ImGui::TextUnformatted(text);
	return 0;
}

int TextColored(lua_State* L)
{
	STACK_CHECKER(L, "textColored", 0);

	const char* text = luaL_checkstring(L, 2);
	ImVec4 color = GColor::toVec4(L, 3);
	ImGui::TextColored(color, "%s", text);
	return 0;
}

int TextDisabled(lua_State* L)
{
	STACK_CHECKER(L, "textDisabled", 0);

	const char* text = luaL_checkstring(L, 2);
	ImGui::TextDisabled("%s", text);
	return 0;
}

int TextWrapped(lua_State* L)
{
	STACK_CHECKER(L, "textWrapped", 0);

	const char* text = luaL_checkstring(L, 2);
	ImGui::TextWrapped("%s", text);
	return 0;
}

int LabelText(lua_State* L)
{
	STACK_CHECKER(L, "labelText", 0);

	const char* text = luaL_checkstring(L, 2);
	const char* label = luaL_checkstring(L, 3);
	ImGui::LabelText(label, "%s", text);
	return 0;
}

int BulletText(lua_State* L)
{
	STACK_CHECKER(L, "bulletText", 0);

	const char* text = luaL_checkstring(L, 2);
	ImGui::BulletText("%s", text);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Widgets: Main
///
/////////////////////////////////////////////////////////////////////////////////////////////

int Button(lua_State* L)
{
	STACK_CHECKER(L, "button", 1);

	const char* label = luaL_checkstring(L, 2);
	const ImVec2 size = luaL_optvec2(L, 3);
	lua_pushboolean(L, ImGui::Button(label, size));
	return 1;
}

int SmallButton(lua_State* L)
{
	STACK_CHECKER(L, "smallButton", 1);

	const char* label = luaL_checkstring(L, 2);
	lua_pushboolean(L, ImGui::SmallButton(label));
	return 1;
}

int InvisibleButton(lua_State* L)
{
	STACK_CHECKER(L, "invisibleButton", 1);

	const char* str_id = luaL_checkstring(L, 2);
	const ImVec2 size = luaL_optvec2(L, 3);
	lua_pushboolean(L, ImGui::InvisibleButton(str_id, size));
	return 1;
}

int ArrowButton(lua_State* L)
{
	STACK_CHECKER(L, "arrowButton", 1);

	const char* str_id = luaL_checkstring(L, 2);
	ImGuiDir dir = luaL_checknumber(L, 3);
	lua_pushboolean(L, ImGui::ArrowButton(str_id, dir));
	return 1;
}

int Image(lua_State* L)
{
	STACK_CHECKER(L, "image", 0);

	GTextureData data(L, 2);
	const ImVec2 size = luaL_checkvec2(L, 3);
	const ImVec4 tint = GColor::toVec4opt(L, 5);
	const ImVec4 border = GColor::toVec4opt(L, 7, 0xffffff, 0.0f);

	setupUVs(L, data, 9);

	ImGui::Image(data.texture, size, data.uv0, data.uv1, tint, border);
	return 0;
}

int ImageUV(lua_State* L)
{
	STACK_CHECKER(L, "image", 0);

	GTextureData data(L, 2);
	const ImVec2 size = luaL_checkvec2(L, 3);
	const ImVec2 uv0 = luaL_checkvec2(L, 5);
	const ImVec2 uv1 = luaL_checkvec2(L, 7);
	const ImVec4 tint = GColor::toVec4opt(L, 9);
	const ImVec4 border = GColor::toVec4opt(L, 11, 0xffffff, 0.0f);

	ImGui::Image(data.texture, size, uv0, uv1, tint, border);
	return 0;
}

int ImageButton(lua_State* L)
{
	STACK_CHECKER(L, "imageButton", 1);

	GTextureData data(L, 2);
	const ImVec2 size = luaL_checkvec2(L, 3);
	int frame_padding = luaL_optinteger(L, 5, -1);
	const ImVec4 tint = GColor::toVec4opt(L, 6);
	const ImVec4 bg_col = GColor::toVec4opt(L, 8, 0xffffff, 0.0f);

	setupUVs(L, data, 10);

	lua_pushboolean(L, ImGui::ImageButton(data.texture, size, data.uv0, data.uv1, frame_padding, bg_col, tint));
	return 1;
}

int ImageButtonUV(lua_State* L)
{
	STACK_CHECKER(L, "imageButton", 1);

	GTextureData data(L, 2);
	const ImVec2 size = luaL_checkvec2(L, 3);
	const ImVec2 uv0 = luaL_checkvec2(L, 5);
	const ImVec2 uv1 = luaL_checkvec2(L, 7);
	int frame_padding = luaL_optinteger(L, 9, -1);
	const ImVec4 tint = GColor::toVec4opt(L, 10);
	const ImVec4 bg_col = GColor::toVec4opt(L, 12, 0xffffff, 0.0f);

	lua_pushboolean(L, ImGui::ImageButton(data.texture, size, uv0, uv1, frame_padding, bg_col, tint));
	return 1;
}

int ScaledImage(lua_State* L)
{
	STACK_CHECKER(L, "scaledImage", 0);

	GTextureData data(L, 2);
	const ImVec2 size = luaL_checkvec2(L, 3);
	ImGuiImageScaleMode fit_mode = luaL_optinteger(L, 5, ImGuiImageScaleMode_LetterBox);
	bool keep_size = lua_toboolean(L, 6);
	const ImVec2 anchor = luaL_optvec2(L, 7, 0.5f, 0.5f);
	const ImVec4 tint_col = GColor::toVec4opt(L, 9);
	const ImVec4 border_col = GColor::toVec4opt(L, 11, 0, 0.0f);
	const ImVec4 bg_col = GColor::toVec4opt(L, 13, 0, 0.0f);

	setupUVs(L, data, 15);

	ImGui::ScaledImage(data.texture_size, data.texture, size, fit_mode, keep_size, anchor, tint_col, border_col, bg_col, data.uv0, data.uv1);

	return 0;
}

int ScaledImageButton(lua_State* L)
{
	STACK_CHECKER(L, "scaledImageButton", 1);

	GTextureData data(L, 2);
	const ImVec2 size = luaL_checkvec2(L, 3);
	int fit_mode = luaL_optinteger(L, 5, 0);
	bool keep_size = lua_toboolean(L, 6);
	int flags = luaL_optinteger(L, 7, 0);
	const ImVec2 anchor = luaL_optvec2(L, 8, 0.5f, 0.5f);
	const ImVec2 clip_offset = luaL_optvec2(L, 10);
	const ImVec4 tint_col = GColor::toVec4opt(L, 12);
	const ImVec4 border_col = GColor::toVec4opt(L, 14, 0, 0.0f);
	const ImVec4 bg_col = GColor::toVec4opt(L, 16, 0, 0.0f);

	setupUVs(L, data, 18);

	lua_pushboolean(L, ImGui::ScaledImageButton(data.texture_size, data.texture, size, fit_mode, keep_size, flags, anchor, clip_offset, tint_col, border_col, bg_col, data.uv0, data.uv1));
	return 1;
}

int ScaledImageButtonWithText(lua_State* L)
{
	STACK_CHECKER(L, "scaledImageButtonWithText", 1);

	GTextureData data(L, 2);
	const char* label = luaL_checkstring(L, 3);
	const ImVec2 size = luaL_checkvec2(L, 4);
	const ImVec2 button_size = luaL_optvec2(L, 6);
	ImGuiButtonFlags flags = luaL_optinteger(L, 8, 0);
	ImGuiImageScaleMode fit_mode = luaL_optinteger(L, 9, 0);
	bool keep_size = lua_toboolean(L, 10);
	const ImVec2 anchor = luaL_optvec2(L, 11, 0.5f, 0.5f);
	int image_side = luaL_optinteger(L, 13, ImGuiDir_Left);
	const ImVec2 clip_offset = luaL_optvec2(L, 14);
	const ImVec4 tint_col = GColor::toVec4opt(L, 16);
	const ImVec4 boreder_col = GColor::toVec4opt(L, 18, 0, 0.0f);
	const ImVec4 bg_col = GColor::toVec4opt(L, 20, 0, 0.0f);

	setupUVs(L, data, 22);

	lua_pushboolean(L, ImGui::ScaledImageButtonWithText(data.texture_size, data.texture, label, size, button_size, flags, fit_mode, keep_size, anchor, image_side, clip_offset, tint_col, boreder_col, bg_col, data.uv0, data.uv1));
	return 1;
}

int Checkbox(lua_State* L)
{
	STACK_CHECKER(L, "checkbox", 2);

	const char* label = luaL_checkstring(L, 2);
	bool v = lua_toboolean2(L, 3);
	bool result = ImGui::Checkbox(label, &v);
	lua_pushboolean(L, v);
	lua_pushboolean(L, result);
	return 2;
}

int CheckboxFlags(lua_State* L)
{
	STACK_CHECKER(L, "checkboxFlags", 2);

	const char* label = luaL_checkstring(L, 2);
	int flags = luaL_optinteger(L, 3, 0);
	int flags_value = luaL_optinteger(L, 4, 0);

	bool pressed = ImGui::CheckboxFlags(label, &flags, flags_value);

	lua_pushinteger(L, flags);
	lua_pushboolean(L, pressed);
	return 2;
}

int RadioButton(lua_State* L)
{
	STACK_CHECKER(L, "radioButton", 1);

	const char* label = luaL_checkstring(L, 2);
	if (lua_gettop(L) < 4)
	{
		bool active = lua_toboolean2(L, 3) > 0;
		lua_pushboolean(L, ImGui::RadioButton(label, active));
		return 1;
	}
	else
	{
		int v = luaL_checkinteger(L, 3);
		int v_button = luaL_checkinteger(L, 4);
		bool flag = ImGui::RadioButton(label, &v, v_button);
		lua_pushinteger(L, v);
		lua_pushboolean(L, flag);
		return 2;
	}
}

int ProgressBar(lua_State* L)
{
	STACK_CHECKER(L, "progressBar", 0);

	double fraction = luaL_checknumber(L, 2);
	ImVec2 size = luaL_optvec2(L, 3, -1.0f, 0.0f);
	const char* overlay = luaL_optstring(L, 5, "");
	ImGui::ProgressBar(fraction, size, overlay);
	return  0;
}

int Bullet(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "bullet", 0);

	ImGui::Bullet();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Widgets: Combo Box
///
/////////////////////////////////////////////////////////////////////////////////////////////

int BeginCombo(lua_State* L)
{
	STACK_CHECKER(L, "beginCombo", 1);

	const char* label = luaL_checkstring(L, 2);
	const char* preview_value = luaL_checkstring(L, 3);
	ImGuiComboFlags flags = luaL_optinteger(L, 4, 0);
	lua_pushboolean(L, ImGui::BeginCombo(label, preview_value, flags));
	return 1;
}

int EndCombo(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "endCombo", 0);

	ImGui::EndCombo();
	return 0;
}

int Combo(lua_State* L)
{
	STACK_CHECKER(L, "combo", 2);

	const char* label = luaL_checkstring(L, 2);
	int item_current = luaL_checkinteger(L, 3);
	if (item_current < 0)
	{
		lua_pushnumber(L, -1);
		lua_pushboolean(L, false);
		return 2;
	}

	int maxItems = luaL_optinteger(L, 5, -1);
	bool result = false;
	const int arg_type = lua_type(L, 4);

	switch (arg_type)
	{
	case LUA_TTABLE:
	{
		int len = luaL_getn(L, 4);
		if (!len)
		{
			lua_pushnumber(L, -1);
			return 1;
		}

		const char** items = new const char*[len];
		lua_pushvalue(L, 4);
		for (int i = 0; i < len; i++)
		{
			lua_rawgeti(L, 4, i + 1);
			const char* str = lua_tostring(L, -1);
			items[i] = str;
			lua_pop(L, 1);
		}
		lua_pop(L, 1);

		result = ImGui::Combo(label, &item_current, items, len, maxItems);

		delete[] items;
	} break;
	case LUA_TSTRING:
	{
		const char* items = luaL_checkstring(L, 4);

		result = ImGui::Combo(label, &item_current, items, maxItems);
	} break;
	default:
	{
		LUA_THROW_ERRORF("bad argument #3 to 'combo' (table/string expected, got %s)", lua_typename(L, arg_type));
		return 0;
	}
	}

	lua_pushinteger(L, item_current);
	lua_pushboolean(L, result);
	return 2;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Widgets: Drags
///
/////////////////////////////////////////////////////////////////////////////////////////////

int DragFloat(lua_State* L)
{
	return LuaDragScalarN<float>(L, ImGuiDataType_Float, 1, "%.3f");
}

int DragFloat2(lua_State* L)
{
	return LuaDragScalarN<float>(L, ImGuiDataType_Float, 2, "%.3f");
}

int DragFloat3(lua_State* L)
{
	return LuaDragScalarN<float>(L, ImGuiDataType_Float, 3, "%.3f");
}

int DragFloat4(lua_State* L)
{
	return LuaDragScalarN<float>(L, ImGuiDataType_Float, 4, "%.3f");
}

int DragFloatN(lua_State* L)
{
	int data_size = luaL_checkinteger(L, 2);
	return LuaDragScalarN<float>(L, ImGuiDataType_Float, data_size, "%.3f", 1);
}

int DragFloatT(lua_State* L)
{
	STACK_CHECKER(L, "dragFloatT", 1);

	const char* label = luaL_checkstring(L, 2);
	luaL_checktype(L, 3, LUA_TTABLE);
	const int size = luaL_getn(L, 3);
	float* data = getTableValues<float>(L, 3, size);
	float v_speed = luaL_optnumber(L, 4, 1.0f);
	float v_min = luaL_optnumber(L, 5, 0.0f);
	float v_max = luaL_optnumber(L, 6, 0.0f);
	const char* format = luaL_optstring(L, 7, "%.3f");
	ImGuiSliderFlags flags = luaL_optinteger(L, 8, 0);
	bool pressed = ImGui::DragScalarN(label, ImGuiDataType_Float, data, size, v_speed, &v_min, &v_max, format, flags);
	lua_pushvalue(L, 3);
	for (int i = 0; i < size; i++) {
		lua_pushnumber(L, data[i]);
		lua_rawseti(L, -2, i + 1);
	}
	lua_pop(L, 1);
	delete[] data;
	lua_pushboolean(L, pressed);
	return 1;
}

int DragFloatRange2(lua_State* L)
{
	//STACK_CHECKER(L, "dragFloatRange2", 3);
	int stackOffset = lua_type(L, 3) == LUA_TVECTOR ? 1 : 2;

	const char* label = luaL_checkstring(L, 2);
	float v_speed = luaL_optnumber(L, 3 + stackOffset, 1.0f);
	float v_min = luaL_optnumber(L, 4 + stackOffset, 0.0f);
	float v_max = luaL_optnumber(L, 5 + stackOffset, 0.0f);
	const char* format = luaL_optstring(L, 6 + stackOffset, "%.3f");
	const char* format_max = luaL_optstring(L, 7 + stackOffset, "%.3f");
	ImGuiSliderFlags sliderFlag = luaL_optinteger(L, 8 + stackOffset, 0);

	if (lua_type(L, 3) == LUA_TVECTOR)
	{
		float* vec = const_cast<float*>(lua_tovector(L, 3));
		bool result = ImGui::DragFloatRange2(label, &vec[0], &vec[1], v_speed, v_min, v_max, format, format_max, sliderFlag);

		lua_pushvector(L, vec[0], vec[1], vec[2], vec[3]);
		lua_pushboolean(L, result);
		return 2;
	}
	else
	{
		float v_current_min = luaL_checknumber(L, 3);
		float v_current_max = luaL_checknumber(L, 4);
		bool result = ImGui::DragFloatRange2(label, &v_current_min, &v_current_max, v_speed, v_min, v_max, format, format_max, sliderFlag);

		lua_pushnumber(L, v_current_min);
		lua_pushnumber(L, v_current_max);
		lua_pushboolean(L, result);
		return 3;
	}
}

int DragInt(lua_State* L)
{
	return LuaDragScalarN<int>(L, ImGuiDataType_S32, 1, "%d");
}

int DragInt2(lua_State* L)
{
	return LuaDragScalarN<int>(L, ImGuiDataType_S32, 2, "%d");
}

int DragInt3(lua_State* L)
{
	return LuaDragScalarN<int>(L, ImGuiDataType_S32, 3, "%d");
}

int DragInt4(lua_State* L)
{
	return LuaDragScalarN<int>(L, ImGuiDataType_S32, 4, "%d");
}

int DragIntN(lua_State* L)
{
	int data_size = luaL_checkinteger(L, 2);
	return LuaDragScalarN<int>(L, ImGuiDataType_S32, data_size, "%d", 1);
}

int DragIntT(lua_State* L)
{
	STACK_CHECKER(L, "dragIntT", 1);

	const char* label = luaL_checkstring(L, 2);
	luaL_checktype(L, 3, LUA_TTABLE);
	const int size = luaL_getn(L, 3);
	int* data = getTableValues<int>(L, 3, size);
	float v_speed = luaL_optnumber(L, 4, 1.0f);
	int v_min = luaL_optinteger(L, 5, 0);
	int v_max = luaL_optinteger(L, 6, 0);
	const char* format = luaL_optstring(L, 7, "%d");
	ImGuiSliderFlags flags = luaL_optinteger(L, 8, 0);
	bool pressed = ImGui::DragScalarN(label, ImGuiDataType_S32, data, size, v_speed, &v_min, &v_max, format, flags);
	lua_pushvalue(L, 3);
	for (int i = 0; i < size; i++) {
		lua_pushinteger(L, data[i]);
		lua_rawseti(L, -2, i + 1);
	}
	lua_pop(L, 1);
	delete[] data;
	lua_pushboolean(L, pressed);
	return 1;
}

int DragIntRange2(lua_State* L)
{
	//STACK_CHECKER(L, "dragIntRange2", 3);
	int stackOffset = lua_type(L, 3) == LUA_TVECTOR ? 1 : 2;

	const char* label = luaL_checkstring(L, 2);
	float v_speed = luaL_optnumber(L, 3 + stackOffset, 1.0f);
	int v_min = luaL_optinteger(L, 4 + stackOffset, 0);
	int v_max = luaL_optinteger(L, 5 + stackOffset, 0);
	const char* format = luaL_optstring(L, 6 + stackOffset, "%d");
	const char* format_max = luaL_optstring(L, 7 + stackOffset, "%d");
	ImGuiSliderFlags sliderFlag = luaL_optinteger(L, 8 + stackOffset, 0);

	if (lua_type(L, 3) == LUA_TVECTOR)
	{
		const float* vec = lua_tovector(L, 3);
		int v_current_min = (int)vec[0];
		int v_current_max = (int)vec[1];

		bool result = ImGui::DragIntRange2(label, &v_current_min, &v_current_max, v_speed, v_min, v_max, format, format_max, sliderFlag);

		lua_pushvector(L, (float)v_current_min, (float)v_current_max, vec[2], vec[3]);
		lua_pushboolean(L, result);
		return 2;
	}
	else
	{
		int v_current_min = luaL_checkinteger(L, 3);
		int v_current_max = luaL_checkinteger(L, 4);
		bool result = ImGui::DragIntRange2(label, &v_current_min, &v_current_max, v_speed, v_min, v_max, format, format_max, sliderFlag);

		lua_pushinteger(L, v_current_min);
		lua_pushinteger(L, v_current_max);
		lua_pushboolean(L, result);
		return 3;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Widgets: Sliders
///
/////////////////////////////////////////////////////////////////////////////////////////////

int SliderFloat(lua_State* L)
{
	return LuaSliderScalarN<float>(L, ImGuiDataType_Float, 1, "%.3f");
}

int SliderFloat2(lua_State* L)
{
	return LuaSliderScalarN<float>(L, ImGuiDataType_Float, 2, "%.3f");
}

int SliderFloat3(lua_State* L)
{
	return LuaSliderScalarN<float>(L, ImGuiDataType_Float, 3, "%.3f");
}

int SliderFloat4(lua_State* L)
{
	return LuaSliderScalarN<float>(L, ImGuiDataType_Float, 4, "%.3f");
}

int SliderFloatN(lua_State* L)
{
	int data_size = luaL_checkinteger(L, 2);
	return LuaSliderScalarN<float>(L, ImGuiDataType_Float, data_size, "%.3f", 1);
}

int SliderFloatT(lua_State* L)
{
	STACK_CHECKER(L, "sliderFloatT", 1);

	const char* label = luaL_checkstring(L, 2);
	luaL_checktype(L, 3, LUA_TTABLE);
	const int size = luaL_getn(L, 3);
	float* data = getTableValues<float>(L, 3, size);
	float v_min = luaL_checknumber(L, 4);
	float v_max = luaL_checknumber(L, 5);
	const char* format = luaL_optstring(L, 6, "%.3f");
	ImGuiSliderFlags flags = luaL_optinteger(L, 7, 0);
	bool pressed = ImGui::SliderScalarN(label, ImGuiDataType_Float, data, size, &v_min, &v_max, format, flags);
	lua_pushvalue(L, 3);
	for (int i = 0; i < size; i++) {
		lua_pushnumber(L, data[i]);
		lua_rawseti(L, -2, i + 1);
	}
	lua_pop(L, 1);
	delete[] data;
	lua_pushboolean(L, pressed);
	return 1;
}

int SliderAngle(lua_State* L)
{
	STACK_CHECKER(L, "sliderAngle", 2);

	const char* label = luaL_checkstring(L, 2);
	float v_rad = luaL_checknumber(L, 3);
	float v_degrees_min = luaL_optnumber(L, 4, -360.0f);
	float v_degrees_max = luaL_optnumber(L, 5,  360.0f);
	const char* format = luaL_optstring(L, 6, "%.0f deg");

	bool result = ImGui::SliderAngle(label, &v_rad, v_degrees_min, v_degrees_max, format);

	lua_pushnumber(L, v_rad);
	lua_pushboolean(L, result);
	return 2;
}

int SliderInt(lua_State* L)
{
	return LuaSliderScalarN<int>(L, ImGuiDataType_S32, 1, "%d");
}

int SliderInt2(lua_State* L)
{
	return LuaSliderScalarN<int>(L, ImGuiDataType_S32, 2, "%d");
}

int SliderInt3(lua_State* L)
{
	return LuaSliderScalarN<int>(L, ImGuiDataType_S32, 3, "%d");
}

int SliderInt4(lua_State* L)
{
	return LuaSliderScalarN<int>(L, ImGuiDataType_S32, 4, "%d");
}

int SliderIntN(lua_State* L)
{
	int data_size = luaL_checkinteger(L, 2);
	return LuaSliderScalarN<int>(L, ImGuiDataType_S32, data_size, "%d", 1);
}

int SliderIntT(lua_State* L)
{
	STACK_CHECKER(L, "sliderIntT", 1);

	const char* label = luaL_checkstring(L, 2);
	luaL_checktype(L, 3, LUA_TTABLE);
	const int size = luaL_getn(L, 3);
	int* data = getTableValues<int>(L, 3, size);
	int v_min = luaL_checkinteger(L, 4);
	int v_max = luaL_checkinteger(L, 5);
	const char* format = luaL_optstring(L, 6, "%d");
	ImGuiSliderFlags flags = luaL_optinteger(L, 7, 0);
	bool pressed = ImGui::SliderScalarN(label, ImGuiDataType_S32, data, size, &v_min, &v_max, format, flags);
	lua_pushvalue(L, 3);
	for (int i = 0; i < size; i++) {
		lua_pushinteger(L, data[i]);
		lua_rawseti(L, -2, i + 1);
	}
	lua_pop(L, 1);
	delete[] data;
	lua_pushboolean(L, pressed);
	return 1;
}

int VSliderFloat(lua_State* L)
{
	STACK_CHECKER(L, "vSliderFloat", 2);

	const char* label = luaL_checkstring(L, 2);
	const ImVec2 size = luaL_checkvec2(L, 3);
	float v = luaL_checknumber(L, 5);
	float v_min = luaL_checknumber(L, 6);
	float v_max = luaL_checknumber(L, 7);
	const char* format = luaL_optstring(L, 8, "%.3f");
	ImGuiSliderFlags sliderFlag = luaL_optinteger(L, 9, 0);

	bool result = ImGui::VSliderFloat(label, size, &v, v_min, v_max, format, sliderFlag);

	lua_pushnumber(L, v);
	lua_pushboolean(L, result);
	return 2;
}

int VSliderInt(lua_State* L)
{
	STACK_CHECKER(L, "vSliderInt", 2);

	const char* label = luaL_checkstring(L, 2);
	const ImVec2 size = luaL_checkvec2(L, 3);
	int v = luaL_checkinteger(L, 5);
	int v_min = luaL_checkinteger(L, 6);
	int v_max = luaL_checkinteger(L, 7);
	const char* format = luaL_optstring(L, 8, "%d");
	ImGuiSliderFlags sliderFlag = luaL_optinteger(L, 9, 0);

	bool result = ImGui::VSliderInt(label, size, &v, v_min, v_max, format, sliderFlag);

	lua_pushinteger(L, v);
	lua_pushboolean(L, result);
	return 2;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Custom filled sliders
///
/////////////////////////////////////////////////////////////////////////////////////////////

int FilledSliderFloat(lua_State* L)
{
	return LuaFilledSliderScalarN<float>(L, ImGuiDataType_Float, 1, "%.3f");
}

int FilledSliderFloat2(lua_State* L)
{
	return LuaFilledSliderScalarN<float>(L, ImGuiDataType_Float, 2, "%.3f");
}

int FilledSliderFloat3(lua_State* L)
{
	return LuaFilledSliderScalarN<float>(L, ImGuiDataType_Float, 3, "%.3f");
}

int FilledSliderFloat4(lua_State* L)
{
	return LuaFilledSliderScalarN<float>(L, ImGuiDataType_Float, 4, "%.3f");
}

int FilledSliderFloatN(lua_State* L)
{
	int data_size = luaL_checkinteger(L, 2);
	return LuaFilledSliderScalarN<float>(L, ImGuiDataType_Float, data_size, "%.3f", 1);
}

int FilledSliderFloatT(lua_State* L)
{
	STACK_CHECKER(L, "filledSliderFloatT", 1);

	const char* label = luaL_checkstring(L, 2);
	bool mirror = lua_toboolean(L, 3);
	luaL_checktype(L, 4, LUA_TTABLE);
	const int size = luaL_getn(L, 4);
	float* data = getTableValues<float>(L, 4, size);
	float v_min = luaL_checknumber(L, 5);
	float v_max = luaL_checknumber(L, 6);
	const char* format = luaL_optstring(L, 7, "%.3f");
	ImGuiSliderFlags flags = luaL_optinteger(L, 8, 0);
	bool pressed = ImGui::FilledSliderScalarN(label, mirror, ImGuiDataType_Float, data, size, &v_min, &v_max, format, flags);
	lua_pushvalue(L, 4);
	for (int i = 0; i < size; i++) {
		lua_pushnumber(L, data[i]);
		lua_rawseti(L, -2, i + 1);
	}
	lua_pop(L, 1);
	delete[] data;
	lua_pushboolean(L, pressed);
	return 1;
}

int FilledSliderAngle(lua_State* L)
{
	STACK_CHECKER(L, "filledSliderAngle", 2);

	const char* label = luaL_checkstring(L, 2);
	bool mirror = lua_toboolean(L, 3) > 0;
	float v_rad = luaL_checknumber(L, 4);
	float v_degrees_min = luaL_optnumber(L, 5, -360.0f);
	float v_degrees_max = luaL_optnumber(L, 6,  360.0f);
	const char* format = luaL_optstring(L, 7, "%.0f deg");
	ImGuiSliderFlags sliderFlag = luaL_optinteger(L, 8, 0);

	bool result = ImGui::FilledSliderAngle(label, mirror, &v_rad, v_degrees_min, v_degrees_max, format, sliderFlag);

	lua_pushnumber(L, v_rad);
	lua_pushboolean(L, result);
	return 2;
}

int FilledSliderInt(lua_State* L)
{
	return LuaFilledSliderScalarN<int>(L, ImGuiDataType_S32, 1, "%d");
}

int FilledSliderInt2(lua_State* L)
{
	return LuaFilledSliderScalarN<int>(L, ImGuiDataType_S32, 2, "%d");
}

int FilledSliderInt3(lua_State* L)
{
	return LuaFilledSliderScalarN<int>(L, ImGuiDataType_S32, 3, "%d");
}

int FilledSliderInt4(lua_State* L)
{
	return LuaFilledSliderScalarN<int>(L, ImGuiDataType_S32, 4, "%d");
}

int FilledSliderIntN(lua_State* L)
{
	int data_size = luaL_checkinteger(L, 2);
	return LuaFilledSliderScalarN<int>(L, ImGuiDataType_S32, data_size, "%d", 1);
}

int FilledSliderIntT(lua_State* L)
{
	STACK_CHECKER(L, "filledSliderIntT", 1);

	const char* label = luaL_checkstring(L, 2);
	bool mirror = lua_toboolean(L, 3);
	luaL_checktype(L, 4, LUA_TTABLE);
	const int size = luaL_getn(L, 4);
	int* data = getTableValues<int>(L, 4, size);
	int v_min = luaL_checkinteger(L, 5);
	int v_max = luaL_checkinteger(L, 6);
	const char* format = luaL_optstring(L, 7, "%d");
	ImGuiSliderFlags flags = luaL_optinteger(L, 8, 0);
	bool pressed = ImGui::FilledSliderScalarN(label, mirror, ImGuiDataType_S32, data, size, &v_min, &v_max, format, flags);
	lua_pushvalue(L, 4);
	for (int i = 0; i < size; i++) {
		lua_pushinteger(L, data[i]);
		lua_rawseti(L, -2, i + 1);
	}
	lua_pop(L, 1);
	delete[] data;
	lua_pushboolean(L, pressed);
	return 1;
}

int VFilledSliderFloat(lua_State* L)
{
	STACK_CHECKER(L, "vFilledSliderFloat", 2);

	const char* label = luaL_checkstring(L, 2);
	bool mirror = lua_toboolean(L, 3) > 0;
	const ImVec2 size = luaL_checkvec2(L, 4);
	float v = luaL_checknumber(L, 6);
	float v_min = luaL_checknumber(L, 7);
	float v_max = luaL_checknumber(L, 8);
	const char* format = luaL_optstring(L, 9, "%.3f");
	ImGuiSliderFlags sliderFlag = luaL_optinteger(L, 10, 0);

	bool result = ImGui::VFilledSliderFloat(label, mirror, size, &v, v_min, v_max, format, sliderFlag);

	lua_pushnumber(L, v);
	lua_pushboolean(L, result);
	return 2;
}

int VFilledSliderInt(lua_State* L)
{
	STACK_CHECKER(L, "vFilledSliderInt", 2);

	const char* label = luaL_checkstring(L, 2);
	bool mirror = lua_toboolean(L, 3) > 0;
	const ImVec2 size = luaL_checkvec2(L, 4);
	int v = luaL_checkinteger(L, 6);
	int v_min = luaL_checkinteger(L, 7);
	int v_max = luaL_checkinteger(L, 8);
	const char* format = luaL_optstring(L, 9, "%d");
	ImGuiSliderFlags sliderFlag = luaL_optinteger(L, 10, 0);

	bool result = ImGui::VFilledSliderInt(label, mirror, size, &v, v_min, v_max, format, sliderFlag);

	lua_pushinteger(L, v);
	lua_pushboolean(L, result);
	return 2;
}

// Widgets: Input with Keyboard
int InputText(lua_State* L)
{
	STACK_CHECKER(L, "inputText", 2);
	GidImGui* imgui = getImgui(L);
	const char* label = luaL_checkstring(L, 2);
	const char* text = luaL_checkstring(L, 3);
	int buffer_size = luaL_checkinteger(L, 4);
	ImGuiInputTextFlags flags = luaL_optinteger(L, 5, 0);
	char* buffer = new char[buffer_size];
	sprintf(buffer, "%s", text);
	bool result = false;

	if (lua_gettop(L) > 5)
	{
		CallbackData* callback = new CallbackData(L, 6);
		callback->flags = flags;
		imgui->callbacks.push_back(callback);

		result = ImGui::InputText(label, buffer, buffer_size, flags, InputTextCallback, (void*)callback);
	}
	else
	{
		result = ImGui::InputText(label, buffer, buffer_size, flags);
	}

	lua_pushstring(L, &(*buffer));
	lua_pushboolean(L, result);
	delete[] buffer;
	return 2;
}

int InputTextMultiline(lua_State* L)
{
	STACK_CHECKER(L, "inputTextMultiline", 2);

	const char* label = luaL_checkstring(L, 2);
	const char* text = luaL_checkstring(L, 3);
	int buffer_size = luaL_checkinteger(L, 4);
	ImVec2 size = luaL_optvec2(L, 5);
	ImGuiInputTextFlags flags = luaL_optinteger(L, 7, 0);
	char* buffer = new char[buffer_size];
	sprintf(buffer, "%s", text);
	bool result = false;

	if (lua_gettop(L) > 7)
	{
		GidImGui* imgui = getImgui(L);
		CallbackData* callback = new CallbackData(L, 8);
		callback->flags = flags;
		imgui->callbacks.push_back(callback);

		result = ImGui::InputTextMultiline(label, buffer, buffer_size, size, flags, InputTextCallback, (void*)callback);
	}
	else
	{
		result = ImGui::InputTextMultiline(label, buffer, buffer_size, size, flags);
	}

	lua_pushstring(L, &(*buffer));
	lua_pushboolean(L, result);
	delete[] buffer;
	return 2;

}

int InputTextWithHint(lua_State* L)
{
	STACK_CHECKER(L, "inputTextWithHint", 2);


	const char* label = luaL_checkstring(L, 2);
	const char* text = luaL_checkstring(L, 3);
	const char* hint = luaL_checkstring(L, 4);
	size_t buf_size = luaL_checkinteger(L, 5);
	ImGuiInputTextFlags flags = luaL_optinteger(L, 6, 0);
	char* buffer = new char[buf_size];
	sprintf(buffer, "%s", text);
	bool result = false;

	if (lua_gettop(L) > 6)
	{
		GidImGui* imgui = getImgui(L);
		CallbackData* callback = new CallbackData(L, 7);
		callback->flags = flags;
		imgui->callbacks.push_back(callback);

		result = ImGui::InputTextWithHint(label, hint, buffer, buf_size, flags, InputTextCallback, (void *)callback);
	}
	else
	{
		result = ImGui::InputTextWithHint(label, hint, buffer, buf_size, flags);
	}

	lua_pushstring(L, &(*buffer));
	lua_pushboolean(L, result);
	delete[] buffer;
	return 2;
}

int InputFloat(lua_State* L)
{
	return LuaInputScalarN<float>(L, ImGuiDataType_Float, 1, "%.3f");
}

int InputFloat2(lua_State* L)
{
	return LuaInputScalarN<float>(L, ImGuiDataType_Float, 2, "%.3f");
}

int InputFloat3(lua_State* L)
{
	return LuaInputScalarN<float>(L, ImGuiDataType_Float, 3, "%.3f");
}

int InputFloat4(lua_State* L)
{
	return LuaInputScalarN<float>(L, ImGuiDataType_Float, 4, "%.3f");
}

int InputFloatN(lua_State* L)
{
	int data_size = luaL_checkinteger(L, 2);
	return LuaInputScalarN<int>(L, ImGuiDataType_Float, data_size, "%.3", 1);
}

int InputFloatT(lua_State* L)
{
	STACK_CHECKER(L, "inputFloatT", 1);

	const char* label = luaL_checkstring(L, 2);
	luaL_checktype(L, 3, LUA_TTABLE);
	const int size = luaL_getn(L, 3);
	float* data = getTableValues<float>(L, 3, size);
	const char* format = luaL_optstring(L, 4, "%.3f");
	ImGuiSliderFlags flags = luaL_optinteger(L, 5, 0);
	bool pressed = ImGui::InputScalarN(label, ImGuiDataType_Float, data, size, NULL, NULL, format, flags);
	lua_pushvalue(L, 3);
	for (int i = 0; i < size; i++) {
		lua_pushnumber(L, data[i]);
		lua_rawseti(L, -2, i + 1);
	}
	lua_pop(L, 1);
	delete[] data;
	lua_pushboolean(L, pressed);
	return 1;
}

int InputInt(lua_State* L)
{
	return LuaInputScalarN<int>(L, ImGuiDataType_S32, 1, "%d");
}

int InputInt2(lua_State* L)
{
	return LuaInputScalarN<int>(L, ImGuiDataType_S32, 2, "%d");
}

int InputInt3(lua_State* L)
{
	return LuaInputScalarN<int>(L, ImGuiDataType_S32, 3, "%d");
}

int InputInt4(lua_State* L)
{
	return LuaInputScalarN<int>(L, ImGuiDataType_S32, 4, "%d");
}

int InputIntN(lua_State* L)
{
	int data_size = luaL_checkinteger(L, 2);
	return LuaInputScalarN<int>(L, ImGuiDataType_S32, data_size, "%d", 1);
}

int InputIntT(lua_State* L)
{
	STACK_CHECKER(L, "inputIntT", 1);

	const char* label = luaL_checkstring(L, 2);
	luaL_checktype(L, 3, LUA_TTABLE);
	const int size = luaL_getn(L, 3);
	int* data = getTableValues<int>(L, 3, size);
	const char* format = luaL_optstring(L, 4, "%d");
	ImGuiSliderFlags flags = luaL_optinteger(L, 5, 0);
	bool pressed = ImGui::InputScalarN(label, ImGuiDataType_S32, data, size, NULL, NULL, format, flags);
	lua_pushvalue(L, 3);
	for (int i = 0; i < size; i++) {
		lua_pushinteger(L, data[i]);
		lua_rawseti(L, -2, i + 1);
	}
	lua_pop(L, 1);
	delete[] data;
	lua_pushboolean(L, pressed);
	return 1;
}

int InputDouble(lua_State* L)
{
	STACK_CHECKER(L, "inputDouble", 2);

	const char* label = luaL_checkstring(L, 2);
	double value = luaL_checknumber(L, 3);
	double step = luaL_optnumber(L, 4, 0.0);
	double step_fast = luaL_optnumber(L, 5, 0.0);
	const char* format = luaL_optstring(L, 6, "%.6f");
	ImGuiInputTextFlags flags = luaL_optinteger(L, 7, 0);

	bool result = ImGui::InputDouble(label, &value, step, step_fast, format, flags);
	lua_pushnumber(L, value);
	lua_pushboolean(L, result);
	return 2;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Widgets: Color Editor/Picker (tip: the ColorEdit* functions have a little colored preview square that can be left-clicked to open a picker, and right-clicked to open an option menu.)
///
/////////////////////////////////////////////////////////////////////////////////////////////

int ColorEdit3(lua_State* L)
{
	STACK_CHECKER(L, "colorEdit3", 2);

	const char* label = luaL_checkstring(L, 2);
	ImVec4 col = GColor::toVec4(luaL_checkinteger(L, 3), 1.0f);
	ImGuiColorEditFlags flags = luaL_optinteger(L, 4, 0);

	bool result = ImGui::ColorEdit3(label, (float*)&col, flags);

	GColor conv = GColor::toHex(col);

	lua_pushnumber(L, conv.hex);
	lua_pushboolean(L, result);
	return 2;
}

int ColorEdit4(lua_State* L)
{
	STACK_CHECKER(L, "colorEdit4", 3);

	const char* label = luaL_checkstring(L, 2);
	ImVec4 col = GColor::toVec4(L, 3);
	ImGuiColorEditFlags flags = luaL_optinteger(L, 5, 0);

	bool result = ImGui::ColorEdit4(label, (float*)&col, flags);

	GColor conv = GColor::toHex(col);
	lua_pushnumber(L, conv.hex);
	lua_pushnumber(L, conv.alpha);
	lua_pushboolean(L, result);
	return 3;
}

int ColorPicker3(lua_State* L)
{
	STACK_CHECKER(L, "colorPicker3", 2);

	const char* label = luaL_checkstring(L, 2);
	ImVec4 col = GColor::toVec4(luaL_checkinteger(L, 3), 1.0f);
	ImGuiColorEditFlags flags = luaL_optinteger(L, 4, 0);

	bool result = ImGui::ColorPicker3(label, (float*)&col, flags);

	GColor conv = GColor::toHex(col);
	lua_pushnumber(L, conv.hex);
	lua_pushboolean(L, result);
	return 2;
}

int ColorPicker4(lua_State* L)
{
	STACK_CHECKER(L, "colorPicker4", 5);

	const char* label = luaL_checkstring(L, 2);
	ImVec4 col = GColor::toVec4(L, 3);
	ImGuiColorEditFlags flags = luaL_optinteger(L, 5, 0);
	ImVec4 refCol = GColor::toVec4opt(L, 6);

	bool result = ImGui::ColorPicker4(label, (float*)&col, flags, (float*)&refCol);

	GColor conv1 = GColor::toHex(col);
	GColor conv2 = GColor::toHex(refCol);
	lua_pushnumber(L, conv1.hex);
	lua_pushnumber(L, conv1.alpha);
	lua_pushnumber(L, conv2.hex);
	lua_pushnumber(L, conv2.alpha);
	lua_pushboolean(L, result);
	return 5;
}

int ColorButton(lua_State* L)
{
	STACK_CHECKER(L, "colorButton", 1);

	const char* desc_id = luaL_checkstring(L, 2);
	ImVec4 col = GColor::toVec4(L, 3);
	ImVec2 size = luaL_optvec2(L, 5);
	ImGuiColorEditFlags flags = luaL_optinteger(L, 7, 0);

	lua_pushboolean(L, ImGui::ColorButton(desc_id, col, flags, size));
	return 1;
}

int SetColorEditOptions(lua_State* L)
{
	STACK_CHECKER(L, "setColorEditOptions", 0);

	ImGuiColorEditFlags flags = luaL_checkinteger(L, 2);
	ImGui::SetColorEditOptions(flags);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Widgets: Trees
///
/////////////////////////////////////////////////////////////////////////////////////////////

int TreeNode(lua_State* L)
{
	STACK_CHECKER(L, "treeNode", 1);

	const char* label = luaL_checkstring(L, 2);
	bool result = ImGui::TreeNode(label);
	lua_pushboolean(L, result);
	return 1;
}

int TreeNodeID(lua_State* L)
{
	STACK_CHECKER(L, "treeNodeID", 1);

	const char* str_id = luaL_checkstring(L, 2);
	const char* label = luaL_checkstring(L, 3);
	bool result = ImGui::TreeNode(str_id, "%s", label);
	lua_pushboolean(L, result);
	return 1;
}

int TreeNodeEx(lua_State* L)
{
	STACK_CHECKER(L, "treeNodeEx", 1);

	const char* label = luaL_checkstring(L, 2);
	ImGuiTreeNodeFlags flags = luaL_checkinteger(L, 3);
	bool result = ImGui::TreeNodeEx(label, flags);
	lua_pushboolean(L, result);
	return 1;
}

int TreePush(lua_State* L)
{
	STACK_CHECKER(L, "treePush", 0);

	const char* str_id = luaL_checkstring(L, 2);
	ImGui::TreePush(str_id);
	return 0;
}

int TreePop(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "treePop", 0);

	ImGui::TreePop();
	return 0;
}

int GetTreeNodeToLabelSpacing(lua_State* L)
{
	STACK_CHECKER(L, "getTreeNodeToLabelSpacing", 1);

	lua_pushnumber(L, ImGui::GetTreeNodeToLabelSpacing());
	return 1;
}

int CollapsingHeader(lua_State* L)
{

	const char* label = luaL_checkstring(L, 2);
	ImGuiTreeNodeFlags flags = luaL_optinteger(L, 4, 0);

	if (lua_isnoneornil(L, 3))
	{
		STACK_CHECKER(L, "collapsingHeader", 1);
		lua_pushboolean(L, ImGui::CollapsingHeader(label, NULL, flags));
		return 1;
	}
	STACK_CHECKER(L, "collapsingHeader", 2);

	bool p_open = lua_toboolean(L, 3);
	bool result = ImGui::CollapsingHeader(label, &p_open, flags);
	lua_pushboolean(L, p_open);
	lua_pushboolean(L, result);
	return 2;
}

int SetNextItemOpen(lua_State* L)
{
	STACK_CHECKER(L, "setNextItemOpen", 0);

	bool is_open = lua_toboolean(L, 2);
	ImGuiCond cond = luaL_optinteger(L, 3, 0);
	ImGui::SetNextItemOpen(is_open, cond);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Widgets: Selectables
///
/////////////////////////////////////////////////////////////////////////////////////////////

int Selectable(lua_State* L)
{
	STACK_CHECKER(L, "selectable", 2);

	const char* label = luaL_checkstring(L, 2);
	bool selected = lua_toboolean(L, 3) > 0;
	ImGuiSelectableFlags flags = luaL_optinteger(L, 4, 0);
	ImVec2 size = luaL_optvec2(L, 5);

	bool result = ImGui::Selectable(label, &selected, flags, size);

	lua_pushboolean(L, result);
	lua_pushboolean(L, selected);
	return 2;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Widgets: List Boxes
///
/////////////////////////////////////////////////////////////////////////////////////////////

int ListBox(lua_State* L)
{
	STACK_CHECKER(L, "listBox", 2);

	const char* label = luaL_checkstring(L, 2);
	int current_item = luaL_checkinteger(L, 3);
	luaL_checktype(L, 4, LUA_TTABLE);
	int maxItems = luaL_optinteger(L, 5, -1);

	int len = luaL_getn(L, 4);
	const char** items = new const char*[len];
	lua_pushvalue(L, 4);
	for (int i = 0; i < len; i++)
	{
		lua_rawgeti(L, 4, i+1);

		const char* str = lua_tostring(L,-1);
		items[i] = str;
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	bool result = ImGui::ListBox(label, &current_item, items, len, maxItems);

	lua_pushinteger(L, current_item);
	lua_pushboolean(L, result);
	delete[] items;
	return 2;
}

int BeginListBox(lua_State* L)
{
	STACK_CHECKER(L, "listBoxHeader", 1);

	const char* label = luaL_checkstring(L, 2);
	ImVec2 size = luaL_checkvec2(L, 3);
	lua_pushboolean(L, ImGui::BeginListBox(label, size));
	return 1;
}

int EndListBox(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "listBoxFooter", 0);

	ImGui::EndListBox();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Widgets: Data Plotting
///
/////////////////////////////////////////////////////////////////////////////////////////////

int PlotLines(lua_State* L)
{
	STACK_CHECKER(L, "plotLines", 0);

	const char* label = luaL_checkstring(L, 2);

	luaL_checktype(L, 3, LUA_TTABLE);
	size_t len = luaL_getn(L, 3);
	float* values = getTableValues<float>(L, 3, len);
	int values_offset = luaL_optinteger(L, 4, 0);
	const char* overlay_text = luaL_optstring(L, 5, NULL);
	float scale_min = luaL_optnumber(L, 6, FLT_MAX);
	float scale_max = luaL_optnumber(L, 7, FLT_MAX);
	ImVec2 graph_size = luaL_optvec2(L, 8);
	int stride = sizeof(float);

	ImGui::PlotLines(label, values, len, values_offset, overlay_text, scale_min, scale_max, graph_size, stride);
	delete[] values;
	return 0;
}

int PlotHistogram(lua_State* L)
{
	STACK_CHECKER(L, "plotHistogram", 0);

	const char* label = luaL_checkstring(L, 2);

	luaL_checktype(L, 3, LUA_TTABLE);
	int len = luaL_getn(L, 3);
	float* values = getTableValues<float>(L, 3, len);
	int values_offset = luaL_optinteger(L, 4, 0);
	const char* overlay_text = luaL_optstring(L, 5, NULL);
	float scale_min = luaL_optnumber(L, 6, FLT_MAX);
	float scale_max = luaL_optnumber(L, 7, FLT_MAX);
	ImVec2 graph_size = luaL_optvec2(L, 8);
	int stride = sizeof(float);

	ImGui::PlotHistogram(label, values, len, values_offset, overlay_text, scale_min, scale_max, graph_size, stride);
	delete[] values;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Widgets: Value() Helpers.
///
/////////////////////////////////////////////////////////////////////////////////////////////

int Value(lua_State* L)
{
	STACK_CHECKER(L, "value", 0);

	const char* prefix = luaL_checkstring(L, 2);
	const int valueType = lua_type(L, 3);
	switch(valueType)
	{
	case LUA_TBOOLEAN:
	{
		ImGui::Value(prefix, lua_toboolean(L, 3));
		break;
	}
	case LUA_TNUMBER:
	{
		float n = luaL_checknumber(L, 3);
		if (lua_gettop(L) > 3)
		{
			ImGui::Value(prefix, n, luaL_optstring(L, 4, ""));
		}
		else
		{
			ImGui::Value(prefix, n);
		}
		break;
	}
	default:
	{
		luaL_typerror(L, 3, "'number' or 'boolean'");
		break;
	}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Widgets: Menus
///
/////////////////////////////////////////////////////////////////////////////////////////////

int BeginMenuBar(lua_State* L)
{
	STACK_CHECKER(L, "beginMenuBar", 1);

	lua_pushboolean(L, ImGui::BeginMenuBar());
	return 1;
}

int EndMenuBar(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "endMenuBar", 0);

	ImGui::EndMenuBar();
	return 0;
}

int BeginMainMenuBar(lua_State* L)
{
	STACK_CHECKER(L, "beginMainMenuBar", 1);

	lua_pushboolean(L, ImGui::BeginMainMenuBar());
	return 1;
}

int EndMainMenuBar(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "endMainMenuBar", 0);

	ImGui::EndMainMenuBar();
	return 0;
}

int BeginMenu(lua_State* L)
{
	STACK_CHECKER(L, "beginMenu", 1);

	const char* label = luaL_checkstring(L, 2);
	bool enabled = luaL_optboolean(L, 3, 1);
	lua_pushboolean(L, ImGui::BeginMenu(label, enabled));
	return 1;
}

int BeginMenuEx(lua_State* L)
{
	STACK_CHECKER(L, "beginMenuEx", 1);

	const char* label = luaL_checkstring(L, 2);
	const char* icon = luaL_optstring(L, 3, NULL);
	bool enabled = luaL_optboolean(L, 4, 1);
	lua_pushboolean(L, ImGui::BeginMenuEx(label, icon, enabled));
	return 1;
}

int EndMenu(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "endMenu", 0);

	ImGui::EndMenu();
	return 0;
}

int MenuItem(lua_State* L)
{
	STACK_CHECKER(L, "menuItem", 1);

	const char* label = luaL_checkstring(L, 2);
	const char* shortcut = luaL_optstring(L, 3, NULL);
	int selected = luaL_optboolean(L, 4, 0);
	int enabled = luaL_optboolean(L, 5, 1);

	bool flag = ImGui::MenuItem(label, shortcut, selected, enabled);
	lua_pushboolean(L, flag);

	return 1;
}

int MenuItemEx(lua_State* L)
{
	STACK_CHECKER(L, "menuItemEx", 1);

	const char* label = luaL_checkstring(L, 2);
	const char* icon = luaL_optstring(L, 3, NULL);
	const char* shortcut = luaL_optstring(L, 4, NULL);
	int selected = luaL_optboolean(L, 5, 0);
	int enabled = luaL_optboolean(L, 6, 1);

	bool flag = ImGui::MenuItemEx(label, icon, shortcut, selected, enabled);
	lua_pushboolean(L, flag);

	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tooltips
///
/////////////////////////////////////////////////////////////////////////////////////////////

int BeginTooltip(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "beginTooltip", 0);

	ImGui::BeginTooltip();
	return 0;
}

int EndTooltip(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "endTooltip", 0);

	ImGui::EndTooltip();
	return 0;
}

int SetTooltip(lua_State* L)
{
	STACK_CHECKER(L, "setTooltip", 0);

	const char* text = luaL_checkstring(L, 2);
	ImGui::SetTooltip("%s", text);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Popups, Modals
///
/////////////////////////////////////////////////////////////////////////////////////////////

int BeginPopup(lua_State* L)
{
	STACK_CHECKER(L, "beginPopup", 1);

	const char* str_id = luaL_checkstring(L, 2);
	ImGuiWindowFlags flags = luaL_optinteger(L, 3, 0);
	lua_pushboolean(L, ImGui::BeginPopup(str_id, flags));
	return 1;
}

int BeginPopupModal(lua_State* L)
{
	STACK_CHECKER(L, "beginPopupModal", 1);

	const char* name = luaL_checkstring(L, 2);
	ImGuiWindowFlags flags = luaL_optinteger(L, 4, 0);
	if (lua_isnoneornil(L, 3))
	{
		lua_pushboolean(L, ImGui::BeginPopupModal(name, NULL, flags));
		return 1;
	}
	bool p_open = lua_toboolean(L, 3);
	bool result = ImGui::BeginPopupModal(name, &p_open, flags);
	lua_pushboolean(L, result);
	return 1;
}

int EndPopup(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "endPopup", 0);

	ImGui::EndPopup();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Popups: open/close functions
///
/////////////////////////////////////////////////////////////////////////////////////////////

int OpenPopup(lua_State* L)
{
	STACK_CHECKER(L, "openPopup", 0);

	ImGuiPopupFlags popup_flags = luaL_optinteger(L, 3, 0);
	if (lua_type(L, 2) == LUA_TSTRING)
	{
		const char* str_id = lua_tostring(L, 2);
		ImGui::OpenPopup(str_id, popup_flags);
	}
	else
	{
		ImGuiID id = checkID(L, 2);
		ImGui::OpenPopup(id, popup_flags);
	}
	return 0;
}

int OpenPopupOnItemClick(lua_State* L)
{
	STACK_CHECKER(L, "openPopupOnItemClick", 0);

	const char* str_id = luaL_optstring(L, 2, NULL);
	ImGuiPopupFlags popup_flags = luaL_optinteger(L, 3, 1);
	ImGui::OpenPopupOnItemClick(str_id, popup_flags);
	return 0;
}

int OpenPopupContextItem(lua_State* L)
{
	STACK_CHECKER(L, "openPopupContextItem", 0);

	LUA_THROW_ERROR("\"ImGui:openPopupContextItem()\" is deprecated, use \"ImGui:openPopupOnItemClick()\" instead");
	return 0;
}

int CloseCurrentPopup(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "closeCurrentPopup", 0);

	ImGui::CloseCurrentPopup();
	return 0;
}

// Popups: open+begin combined functions helpers
int BeginPopupContextItem(lua_State* L)
{
	STACK_CHECKER(L, "beginPopupContextItem", 1);

	const char* str_id = luaL_optstring(L, 2, NULL);
	ImGuiPopupFlags popup_flags = luaL_optinteger(L, 3, 1);

	lua_pushboolean(L, ImGui::BeginPopupContextItem(str_id, popup_flags));
	return 1;
}

int BeginPopupContextWindow(lua_State* L)
{
	STACK_CHECKER(L, "beginPopupContextWindow", 1);

	const char* str_id = luaL_optstring(L, 2, NULL);
	ImGuiPopupFlags popup_flags = luaL_optinteger(L, 3, 1);

	lua_pushboolean(L, ImGui::BeginPopupContextWindow(str_id, popup_flags));
	return 1;
}

int BeginPopupContextVoid(lua_State* L)
{
	STACK_CHECKER(L, "beginPopupContextVoid", 1);

	const char* str_id = luaL_optstring(L, 2, NULL);
	ImGuiPopupFlags popup_flags = luaL_optinteger(L, 3, 1);

	lua_pushboolean(L, ImGui::BeginPopupContextVoid(str_id, popup_flags));
	return 1;
}

// Popups: test function
int IsPopupOpen(lua_State* L)
{
	STACK_CHECKER(L, "isPopupOpen", 1);

	const char* str_id = luaL_optstring(L, 2, NULL);
	ImGuiPopupFlags popup_flags = luaL_optinteger(L, 3, 1);

	lua_pushboolean(L, ImGui::IsPopupOpen(str_id, popup_flags));
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// NEW TABLES
///
/////////////////////////////////////////////////////////////////////////////////////////////

int BeginTable(lua_State* L)
{
	STACK_CHECKER(L, "beginTable", 1);

	const char* str_id = luaL_checkstring(L, 2);
	int column = luaL_checkinteger(L, 3);
	ImGuiTableFlags flags = luaL_optinteger(L, 4, 0);
	ImVec2 outer_size = luaL_optvec2(L, 5);
	float inner_width = luaL_optnumber(L, 7, 0.0f);
	bool flag = ImGui::BeginTable(str_id, column, flags, outer_size, inner_width);
	lua_pushboolean(L, flag);
	return 1;
}

int EndTable(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "endTable", 0);

	ImGui::EndTable();
	return 0;
}

int TableNextRow(lua_State* L)
{
	STACK_CHECKER(L, "tableNextRow", 0);

	ImGuiTableRowFlags row_flags = luaL_optinteger(L, 2, 0);
	float min_row_height = luaL_optnumber(L, 3, 0.0f);
	ImGui::TableNextRow(row_flags, min_row_height);
	return 0;
}

int TableNextColumn(lua_State* L)
{
	STACK_CHECKER(L, "tableNextColumn", 1);

	bool flag = ImGui::TableNextColumn();
	lua_pushboolean(L, flag);
	return 1;
}

int TableSetColumnIndex(lua_State* L)
{
	STACK_CHECKER(L, "tableSetColumnIndex", 1);

	int column_n = luaL_checkinteger(L, 2);
	bool flag = ImGui::TableSetColumnIndex(column_n);
	lua_pushboolean(L, flag);
	return 1;
}

int TableSetupColumn(lua_State* L)
{
	STACK_CHECKER(L, "tableSetupColumn", 0);

	const char* label = luaL_checkstring(L, 2);
	ImGuiTableColumnFlags flags = luaL_optinteger(L, 3, 0);
	float init_width_or_weight = luaL_optnumber(L, 4, 0.0f);
	ImU32 user_id = luaL_optinteger(L, 5, 0);
	ImGui::TableSetupColumn(label, flags, init_width_or_weight, user_id);
	return 0;
}

int TableSetupScrollFreeze(lua_State* L)
{
	STACK_CHECKER(L, "tableSetupScrollFreeze", 0);

	int cols = luaL_checkinteger(L, 2);
	int rows = luaL_checkinteger(L, 3);
	ImGui::TableSetupScrollFreeze(cols, rows);
	return 0;
}

int TableSetColumnEnabled(lua_State* L)
{
	STACK_CHECKER(L, "tableSetColumnEnabled", 0);

	int col = luaL_checkinteger(L, 2);
	luaL_checktype(L, 3, LUA_TBOOLEAN);
	bool v = lua_toboolean(L, 3);
	ImGui::TableSetColumnEnabled(col, v);
	return 0;
}

int TableHeadersRow(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "tableHeadersRow", 0);

	ImGui::TableHeadersRow();
	return 0;
}

int TableHeader(lua_State* L)
{
	STACK_CHECKER(L, "tableHeader", 0);

	const char* label = luaL_checkstring(L, 2);
	ImGui::TableHeader(label);
	return 0;
}

int TableGetSortSpecs(lua_State* L)
{
	STACK_CHECKER(L, "tableGetSortSpecs", 1);

	ImGuiTableSortSpecs* specs = ImGui::TableGetSortSpecs();
	g_pushInstance(L, "ImGuiTableSortSpecs", specs);
	return 1;
}

int TableGetColumnCount(lua_State* L)
{
	STACK_CHECKER(L, "tableGetColumnCount", 1);

	int n = ImGui::TableGetColumnCount();
	lua_pushinteger(L, n);
	return 1;
}

int TableGetColumnIndex(lua_State* L)
{
	STACK_CHECKER(L, "tableGetColumnIndex", 1);

	int n = ImGui::TableGetColumnIndex();
	lua_pushinteger(L, n);
	return 1;
}

int TableGetRowIndex(lua_State* L)
{
	STACK_CHECKER(L, "tableGetRowIndex", 1);

	int i = ImGui::TableGetRowIndex();
	lua_pushinteger(L, i);
	return 1;
}

int TableGetColumnName(lua_State* L)
{
	STACK_CHECKER(L, "tableGetColumnName", 1);

	int column_n = luaL_optinteger(L, 2, -1);
	const char* name = ImGui::TableGetColumnName(column_n);
	lua_pushstring(L, name);
	return 1;
}

int TableGetColumnFlags(lua_State* L)
{
	STACK_CHECKER(L, "tableGetColumnFlags", 1);

	int column_n = luaL_optinteger(L, 2, -1);
	ImGuiTableColumnFlags flags = ImGui::TableGetColumnFlags(column_n);
	lua_pushinteger(L, flags);
	return 1;
}

int TableSetBgColor(lua_State* L)
{
	STACK_CHECKER(L, "tableSetBgColor", 0);

	ImGuiTableBgTarget target = luaL_checkinteger(L, 2);
	ImU32 color = GColor::toU32(L, 3);
	int column_n = luaL_optinteger(L, 5, -1);
	ImGui::TableSetBgColor(target, color, column_n);
	return 0;
}

int TableSortSpecs_GetColumnSortSpecs(lua_State* L)
{
	STACK_CHECKER(L, "getColumnSortSpecs", 1);

	ImGuiTableSortSpecs* specs = getPtr<ImGuiTableSortSpecs>(L, "ImGuiTableSortSpecs");
	lua_createtable(L, 0, specs->SpecsCount);

	for (int i = 0; i < specs->SpecsCount; i++)
	{
		const ImGuiTableColumnSortSpecs* sort_spec = &specs->Specs[i];

		lua_pushnumber(L, i + 1);
		g_pushInstance(L, "ImGuiTableColumnSortSpecs", const_cast<ImGuiTableColumnSortSpecs*>(sort_spec));
		lua_settable(L, -3);
	}

	return 1;
}

int TableSortSpecs_GetSpecsCount(lua_State* L)
{
	STACK_CHECKER(L, "getSpecsCount", 1);

	ImGuiTableSortSpecs* specs = getPtr<ImGuiTableSortSpecs>(L, "ImGuiTableSortSpecs");
	lua_pushinteger(L, specs->SpecsCount);
	return 1;
}

int TableSortSpecs_GetSpecsDirty(lua_State* L)
{
	STACK_CHECKER(L, "isSpecsDirty", 1);

	ImGuiTableSortSpecs* specs = getPtr<ImGuiTableSortSpecs>(L, "ImGuiTableSortSpecs");
	lua_pushboolean(L, specs->SpecsDirty);
	return 1;
}

int TableSortSpecs_SetSpecsDirty(lua_State* L)
{
	STACK_CHECKER(L, "setSpecsDirty", 0);

	ImGuiTableSortSpecs* specs = getPtr<ImGuiTableSortSpecs>(L, "ImGuiTableSortSpecs");
	specs->SpecsDirty = lua_toboolean(L, 2);
	return 0;
}

int TableColumnSortSpecs_GetColumnUserID(lua_State* L)
{
	STACK_CHECKER(L, "getColumnUserID", 1);

	ImGuiTableColumnSortSpecs* sort_spec = getPtr<ImGuiTableColumnSortSpecs>(L, "ImGuiTableColumnSortSpecs");
	lua_pushinteger(L, sort_spec->ColumnUserID);
	return 1;
}

int TableColumnSortSpecs_GetColumnIndex(lua_State* L)
{
	STACK_CHECKER(L, "getColumnIndex", 1);

	ImGuiTableColumnSortSpecs* sort_spec = getPtr<ImGuiTableColumnSortSpecs>(L, "ImGuiTableColumnSortSpecs");
	lua_pushinteger(L, sort_spec->ColumnIndex);
	return 1;
}

int TableColumnSortSpecs_GetSortOrder(lua_State* L)
{
	STACK_CHECKER(L, "getSortOrder", 1);

	ImGuiTableColumnSortSpecs* sort_spec = getPtr<ImGuiTableColumnSortSpecs>(L, "ImGuiTableColumnSortSpecs");
	lua_pushinteger(L, sort_spec->SortOrder);
	return 1;
}

int TableColumnSortSpecs_GetSortDirection(lua_State* L)
{
	STACK_CHECKER(L, "getSortDirection", 1);

	ImGuiTableColumnSortSpecs* sort_spec = getPtr<ImGuiTableColumnSortSpecs>(L, "ImGuiTableColumnSortSpecs");
	lua_pushinteger(L, sort_spec->SortDirection);
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// ImGuiTextInputCallbackData functions
///
/////////////////////////////////////////////////////////////////////////////////////////////

int ITCD_GetEventFlag(lua_State* L)
{
	STACK_CHECKER(L, "getEventFlag", 1);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	lua_pushinteger(L, data->EventFlag);
	return 1;
}

int ITCD_GetFlags(lua_State* L)
{
	STACK_CHECKER(L, "getFlags", 1);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	lua_pushinteger(L, data->Flags);
	return 1;
}

int ITCD_GetEventChar(lua_State* L)
{
	STACK_CHECKER(L, "getEventChar", 1);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	lua_pushinteger(L, data->EventChar);
	return 1;
}

int ITCD_SetEventChar(lua_State* L)
{
	STACK_CHECKER(L, "setEventChar", 0);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	int eChar = luaL_checkinteger(L, 2);
	data->EventChar = eChar;
	return 0;
}

int ITCD_GetEventKey(lua_State* L)
{
	STACK_CHECKER(L, "getEventKey", 1);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	int giderosKeyCode = imGuiKeyToGideros(data->EventKey);
	lua_pushinteger(L, giderosKeyCode);
	return 1;
}

int ITCD_GetBuf(lua_State* L)
{
	STACK_CHECKER(L, "getBuf", 1);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	lua_pushstring(L, data->Buf);
	return 1;
}

int ITCD_SetBuf(lua_State* L)
{
	STACK_CHECKER(L, "setBuf", 0);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	const char* buf = luaL_checkstring(L, 2);
	sprintf(data->Buf, "%s", buf);
	return 0;
}

int ITCD_GetBufTextLen(lua_State* L)
{
	STACK_CHECKER(L, "getBufTextLen", 1);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	lua_pushinteger(L, data->BufTextLen);
	return 1;
}

int ITCD_SetBufTextLen(lua_State* L)
{
	STACK_CHECKER(L, "setBufTextLen", 0);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	data->BufTextLen = luaL_checkinteger(L, 2);
	return 0;
}

int ITCD_GetBufSize(lua_State* L)
{
	STACK_CHECKER(L, "getBufSize", 1);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	lua_pushinteger(L, data->BufSize);
	return 1;
}

int ITCD_IsBufDirty(lua_State* L)
{
	STACK_CHECKER(L, "isBufDirty", 1);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	lua_pushboolean(L, data->BufDirty);
	return 1;
}

int ITCD_SetBufDirty(lua_State* L)
{
	STACK_CHECKER(L, "setBufDirty", 0);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	data->BufDirty = lua_toboolean(L, 2);
	return 0;
}

int ITCD_GetCursorPos(lua_State* L)
{
	STACK_CHECKER(L, "getCursorPos", 1);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	lua_pushinteger(L, data->CursorPos);
	return 1;
}

int ITCD_SetCursorPos(lua_State* L)
{
	STACK_CHECKER(L, "setCursorPos", 0);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	data->CursorPos = luaL_checkinteger(L, 2);
	return 0;
}

int ITCD_GetSelectionStart(lua_State* L)
{
	STACK_CHECKER(L, "getSelectionStart", 1);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	lua_pushinteger(L, data->SelectionStart);
	return 1;
}

int ITCD_SetSelectionStart(lua_State* L)
{
	STACK_CHECKER(L, "setSelectionStart", 0);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	data->SelectionStart = luaL_checkinteger(L, 2);
	return 0;
}

int ITCD_GetSelectionEnd(lua_State* L)
{
	STACK_CHECKER(L, "getSelectionEnd", 1);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	lua_pushinteger(L, data->SelectionEnd);
	return 1;
}

int ITCD_SetSelectionEnd(lua_State* L)
{
	STACK_CHECKER(L, "setSelectionEnd", 0);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	data->SelectionEnd = luaL_checkinteger(L, 2);
	return 0;
}

int ITCD_GetSelection(lua_State* L)
{
	STACK_CHECKER(L, "getSelection", 1);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	lua_pushinteger(L, data->SelectionStart);
	lua_pushinteger(L, data->SelectionEnd);
	return 2;
}

int ITCD_SetSelection(lua_State* L)
{
	STACK_CHECKER(L, "setSelection", 0);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	data->SelectionStart = luaL_checkinteger(L, 2);
	data->SelectionEnd = luaL_checkinteger(L, 3);
	return 0;
}


int ITCD_DeleteChars(lua_State* L)
{
	STACK_CHECKER(L, "deleteChars", 0);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	int pos = luaL_checkinteger(L, 2);
	int count = luaL_checkinteger(L, 3);
	data->DeleteChars(pos, count);
	return 0;
}

int ITCD_InsertChars(lua_State* L)
{
	STACK_CHECKER(L, "insertChars", 0);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	int pos = luaL_checkinteger(L, 2);
	const char* text = luaL_checkstring(L, 3);
	data->InsertChars(pos, text);
	return 0;
}

int ITCD_SelectAll(lua_State* L)
{
	STACK_CHECKER(L, "selectAll", 0);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	data->SelectAll();
	return 0;
}

int ITCD_ClearSelection(lua_State* L)
{
	STACK_CHECKER(L, "clearSelection", 0);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	data->ClearSelection();
	return 0;
}

int ITCD_HasSelection(lua_State* L)
{
	STACK_CHECKER(L, "hasSelection", 1);

	ImGuiInputTextCallbackData* data = getPtr<ImGuiInputTextCallbackData>(L, "ImGuiInputTextCallbackData");
	lua_pushboolean(L, data->HasSelection());
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// ImGuiSizeCallbackData
///
/////////////////////////////////////////////////////////////////////////////////////////////

int SCD_GetPos(lua_State* L)
{
	STACK_CHECKER(L, "getPos", 2);

	ImGuiSizeCallbackData* data = getPtr<ImGuiSizeCallbackData>(L, "ImGuiSizeCallbackData");
	lua_pushnumber(L, data->Pos.x);
	lua_pushnumber(L, data->Pos.y);
	return 2;
}

int SCD_GetPosX(lua_State* L)
{
	STACK_CHECKER(L, "getPosX", 2);

	ImGuiSizeCallbackData* data = getPtr<ImGuiSizeCallbackData>(L, "ImGuiSizeCallbackData");
	lua_pushnumber(L, data->Pos.x);
	return 1;
}

int SCD_GetPosY(lua_State* L)
{
	STACK_CHECKER(L, "getPosY", 2);

	ImGuiSizeCallbackData* data = getPtr<ImGuiSizeCallbackData>(L, "ImGuiSizeCallbackData");
	lua_pushnumber(L, data->Pos.y);
	return 1;
}

int SCD_GetCurrentSize(lua_State* L)
{
	STACK_CHECKER(L, "getCurrentSize", 2);

	ImGuiSizeCallbackData* data = getPtr<ImGuiSizeCallbackData>(L, "ImGuiSizeCallbackData");
	lua_pushnumber(L, data->CurrentSize.x);
	lua_pushnumber(L, data->CurrentSize.y);
	return 2;
}

int SCD_GetCurrentWidth(lua_State* L)
{
	STACK_CHECKER(L, "getCurrentWidth", 2);

	ImGuiSizeCallbackData* data = getPtr<ImGuiSizeCallbackData>(L, "ImGuiSizeCallbackData");
	lua_pushnumber(L, data->CurrentSize.x);
	return 1;
}

int SCD_GetCurrentHeight(lua_State* L)
{
	STACK_CHECKER(L, "getCurrentHeight", 2);

	ImGuiSizeCallbackData* data = getPtr<ImGuiSizeCallbackData>(L, "ImGuiSizeCallbackData");
	lua_pushnumber(L, data->CurrentSize.y);
	return 1;
}

int SCD_GetDesiredSize(lua_State* L)
{
	STACK_CHECKER(L, "getDesiredSize", 2);

	ImGuiSizeCallbackData* data = getPtr<ImGuiSizeCallbackData>(L, "ImGuiSizeCallbackData");
	lua_pushnumber(L, data->DesiredSize.x);
	lua_pushnumber(L, data->DesiredSize.y);
	return 2;
}

int SCD_GetDesiredWidth(lua_State* L)
{
	STACK_CHECKER(L, "getDesiredWidth", 2);

	ImGuiSizeCallbackData* data = getPtr<ImGuiSizeCallbackData>(L, "ImGuiSizeCallbackData");
	lua_pushnumber(L, data->DesiredSize.x);
	return 1;
}

int SCD_GetDesiredHeight(lua_State* L)
{
	STACK_CHECKER(L, "getDesiredHeight", 2);

	ImGuiSizeCallbackData* data = getPtr<ImGuiSizeCallbackData>(L, "ImGuiSizeCallbackData");
	lua_pushnumber(L, data->DesiredSize.y);
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// ListClipper
///
/////////////////////////////////////////////////////////////////////////////////////////////

int initImGuiListClipper(lua_State* L)
{
	ImGuiListClipper* clipper = new ImGuiListClipper();
	g_pushInstance(L, "ImGuiListClipper", clipper);

	luaL_rawgetptr(L, LUA_REGISTRYINDEX, &keyWeak);
	lua_pushvalue(L, -2);
	luaL_rawsetptr(L, -2, clipper);
	lua_pop(L, 1);

	return 1;
}

int destroyImGuiListClipper(LUA_STATE* p)
{
	destroyObject<ImGuiListClipper>(p);
	return 0;
}

int Clipper_Begin(lua_State* L)
{
	STACK_CHECKER(L, "beginClip", 0);

	ImGuiListClipper* clipper = getPtr<ImGuiListClipper>(L, "ImGuiListClipper");
	int items_count = luaL_checkinteger(L, 2);
	float items_height = luaL_optnumber(L, 3, -1.0f);
	clipper->Begin(items_count, items_height);
	return 0;
}

int Clipper_End(lua_State* L)
{
	STACK_CHECKER(L, "endClip", 0);

	ImGuiListClipper* clipper = getPtr<ImGuiListClipper>(L, "ImGuiListClipper");
	clipper->End();
	return 0;
}

int Clipper_Step(lua_State* L)
{
	STACK_CHECKER(L, "step", 1);

	ImGuiListClipper* clipper = getPtr<ImGuiListClipper>(L, "ImGuiListClipper");
	lua_pushboolean(L, clipper->Step());
	return 1;
}

int Clipper_GetDisplayStart(lua_State* L)
{
	STACK_CHECKER(L, "getDisplayStart", 1);

	ImGuiListClipper* clipper = getPtr<ImGuiListClipper>(L, "ImGuiListClipper");
	lua_pushinteger(L, clipper->DisplayStart + 1);
	return 1;
}

int Clipper_GetDisplayEnd(lua_State* L)
{
	STACK_CHECKER(L, "getDisplayEnd", 1);

	ImGuiListClipper* clipper = getPtr<ImGuiListClipper>(L, "ImGuiListClipper");
	lua_pushinteger(L, clipper->DisplayEnd);
	return 1;
}

int Clipper_GetStartPosY(lua_State* L)
{
	STACK_CHECKER(L, "getStartPosY", 1);

	ImGuiListClipper* clipper = getPtr<ImGuiListClipper>(L, "ImGuiListClipper");
	lua_pushnumber(L, clipper->StartPosY);
	return 1;
}

int Clipper_GetItemsCount(lua_State* L)
{
	STACK_CHECKER(L, "getItemsCount", 1);

	ImGuiListClipper* clipper = getPtr<ImGuiListClipper>(L, "ImGuiListClipper");
	lua_pushinteger(L, clipper->ItemsCount);
	return 1;
}

int Clipper_GetItemsHeight(lua_State* L)
{
	STACK_CHECKER(L, "getItemsHeight", 1);

	ImGuiListClipper* clipper = getPtr<ImGuiListClipper>(L, "ImGuiListClipper");
	lua_pushnumber(L, clipper->ItemsHeight);
	return 1;
}

int Clipper_ForceDisplayRangeByIndices(lua_State* L)
{
	STACK_CHECKER(L, "forceDisplayRangeByIndices", 0);

	ImGuiListClipper* clipper = getPtr<ImGuiListClipper>(L, "ImGuiListClipper");
	int item_min = luaL_checkinteger(L, 2);
	int item_max = luaL_checkinteger(L, 3);
	clipper->ForceDisplayRangeByIndices(item_min, item_max);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// TextFilter
///
/////////////////////////////////////////////////////////////////////////////////////////////

int initImGuiTextFilter(lua_State* L)
{
	ImGuiTextFilter* filter = new ImGuiTextFilter();
	g_pushInstance(L, "ImGuiTextFilter", filter);
	return 1;
}

int destroyImGuiTextFilter(LUA_STATE* p)
{
	destroyObject<ImGuiTextFilter>(p);
	return 0;
}

int TextFilter_PassFilter(lua_State* L)
{
	ImGuiTextFilter* filter = getPtr<ImGuiTextFilter>(L, "ImGuiTextFilter");
	const char* text = luaL_checkstring(L, 2);
	lua_pushboolean(L, filter->PassFilter(text));
	return 1;
}

int TextFilter_Clear(lua_State* L)
{
	ImGuiTextFilter* filter = getPtr<ImGuiTextFilter>(L, "ImGuiTextFilter");
	filter->Clear();
	return 0;
}

int TextFilter_Draw(lua_State* L)
{
	ImGuiTextFilter* filter = getPtr<ImGuiTextFilter>(L, "ImGuiTextFilter");
	const char* label = luaL_checkstring(L, 2);
	float width = luaL_optnumber(L, 3, 0.0f);
	lua_pushboolean(L, filter->Draw(label, width));
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Columns (LEGACY API)
///
/////////////////////////////////////////////////////////////////////////////////////////////

int Columns(lua_State* L)
{
	STACK_CHECKER(L, "columns", 0);

	int count = luaL_optinteger(L, 2, 1);
	const char* id = luaL_optstring(L, 3, NULL);
	bool border = luaL_optboolean(L, 4, 1);

	ImGui::Columns(count, id, border);

	return 0;
}

int NextColumn(lua_State* L)
{
	STACK_CHECKER(L, "nextColumn", 0);

	ImGui::NextColumn();
	return 0;
}

int GetColumnIndex(lua_State* L)
{
	STACK_CHECKER(L, "getColumnIndex", 1);

	lua_pushinteger(L, ImGui::GetColumnIndex());
	return 1;
}

int GetColumnWidth(lua_State* L)
{
	STACK_CHECKER(L, "getColumnWidth", 1);

	int column_index = luaL_optinteger(L, 2, -1);
	lua_pushnumber(L, ImGui::GetColumnWidth(column_index));
	return 1;
}

int SetColumnWidth(lua_State* L)
{
	STACK_CHECKER(L, "setColumnWidth", 0);

	int column_index = luaL_checkinteger(L, 2);
	double width = luaL_checknumber(L, 3);
	ImGui::SetColumnWidth(column_index, width);
	return 0;
}

int GetColumnOffset(lua_State* L)
{
	STACK_CHECKER(L, "getColumnOffset", 1);

	int column_index = luaL_optinteger(L, 2, -1);
	lua_pushnumber(L, ImGui::GetColumnOffset(column_index));
	return 1;
}

int SetColumnOffset(lua_State* L)
{
	STACK_CHECKER(L, "setColumnOffset", 0);

	int column_index = luaL_checkinteger(L, 2);
	double offset = luaL_checknumber(L, 3);
	ImGui::SetColumnOffset(column_index, offset);
	return 0;
}

int GetColumnsCount(lua_State* L)
{
	STACK_CHECKER(L, "getColumnsCount", 1);

	lua_pushinteger(L, ImGui::GetColumnsCount());
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tab Bars, Tabs
///
/////////////////////////////////////////////////////////////////////////////////////////////

int BeginTabBar(lua_State* L)
{
	STACK_CHECKER(L, "beginTabBar", 1);

	const char* str_id = luaL_checkstring(L, 2);
	ImGuiTabBarFlags flags = luaL_optinteger(L, 3, 0);

	lua_pushboolean(L, ImGui::BeginTabBar(str_id, flags));
	return 1;
}

int EndTabBar(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "endTabBar", 0);

	ImGui::EndTabBar();
	return  0;
}

int BeginTabItem(lua_State* L)
{

	const char* label = luaL_checkstring(L, 2);
	ImGuiTabItemFlags flags = luaL_optinteger(L, 4, 0);

	if (lua_isnoneornil(L, 3))
	{
		STACK_CHECKER(L, "beginTabItem", 1);
		lua_pushboolean(L, ImGui::BeginTabItem(label, NULL, flags));
		return 1;
	}
	STACK_CHECKER(L, "beginTabItem", 2);
	bool p_open = lua_toboolean(L, 3);
	bool result = ImGui::BeginTabItem(label, &p_open, flags);
	lua_pushboolean(L, p_open);
	lua_pushboolean(L, result);
	return 2;
}

int EndTabItem(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "endTabItem", 0);

	ImGui::EndTabItem();
	return 0;
}

int TabItemButton(lua_State* L)
{
	STACK_CHECKER(L, "tabItemButton", 1);

	const char* label = luaL_checkstring(L, 2);
	ImGuiTabItemFlags flags = luaL_optinteger(L, 3, 0);
	lua_pushboolean(L, ImGui::TabItemButton(label, flags));
	return 1;
}

int SetTabItemClosed(lua_State* L)
{
	STACK_CHECKER(L, "setTabItemClosed", 0);

	const char* tab_or_docked_window_label = luaL_checkstring(L, 2);
	ImGui::SetTabItemClosed(tab_or_docked_window_label);
	return 0;
}

#ifdef IS_DOCKING_BUILD

/// TODO list:
/// windows api?

int DockSpace(lua_State* L)
{
	STACK_CHECKER(L, "dockSpace", 0);

	ImGuiID id = checkID(L);
	ImVec2 size = luaL_checkvec2(L, 3);
	ImGuiDockNodeFlags flags = luaL_optinteger(L, 5, 0);
	ImGui::DockSpace(id, size, flags);
	return 0;
}

int DockSpaceOverViewport(lua_State* L)
{
	STACK_CHECKER(L, "dockSpaceOverViewport", 1);

	ImGuiDockNodeFlags flags = luaL_optinteger(L, 2, 0);
	lua_pushinteger(L, ImGui::DockSpaceOverViewport(NULL, flags));
	return 1;
}

int SetNextWindowDockID(lua_State* L)
{
	STACK_CHECKER(L, "setNextWindowDockID", 0);

	ImGuiID dock_id = checkID(L);

	ImGuiCond cond = luaL_optinteger(L, 3, 0);
	ImGui::SetNextWindowDockID(dock_id, cond);
	return 0;
}

int GetWindowDockID(lua_State* L)
{
	STACK_CHECKER(L, "getWindowDockID", 1);

	lua_pushinteger(L, ImGui::GetWindowDockID());
	return 1;
}

int IsWindowDocked(lua_State* L)
{
	STACK_CHECKER(L, "isWindowDocked", 1);

	lua_pushboolean(L, ImGui::IsWindowDocked());
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// DockBuilder [BETA API]
///
/////////////////////////////////////////////////////////////////////////////////////////////

int DockBuilderDockWindow(lua_State* L)
{
	STACK_CHECKER(L, "dockBuilderDockWindow", 0);

	const char* window_name = luaL_checkstring(L, 2);
	ImGuiID node_id = checkID(L, 3);
	ImGui::DockBuilderDockWindow(window_name, node_id);
	return 0;
}

int DockBuilderCheckNode(lua_State* L)
{
	STACK_CHECKER(L, "dockBuilderCheckNode", 1);

	ImGuiID node_id = checkID(L);
	ImGuiDockNode* node = ImGui::DockBuilderGetNode(node_id);
	lua_pushboolean(L, node != nullptr);
	return 1;
}

int DockBuilderSetNodePos(lua_State* L)
{
	STACK_CHECKER(L, "dockBuilderSetNodePos", 0);

	ImGuiID node_id = checkID(L);
	ImVec2 pos = luaL_checkvec2(L, 3);
	ImGui::DockBuilderSetNodePos((ImGuiID)node_id, pos);
	return 0;
}

int DockBuilderSetNodeSize(lua_State* L)
{
	STACK_CHECKER(L, "dockBuilderSetNodeSize", 0);

	ImGuiID node_id = checkID(L);
	ImVec2 size = luaL_checkvec2(L, 3);
	ImGui::DockBuilderSetNodeSize((ImGuiID)node_id, size);
	return 0;
}

int DockBuilderAddNode(lua_State* L)
{
	STACK_CHECKER(L, "dockBuilderAddNode", 1);

	ImGuiID node_id = checkID(L);
	ImGuiDockNodeFlags flags = luaL_optinteger(L, 3, 0);
	lua_pushnumber(L, ImGui::DockBuilderAddNode(node_id, flags));
	return 1;
}

int DockBuilderRemoveNode(lua_State* L)
{
	STACK_CHECKER(L, "dockBuilderRemoveNode", 0);

	ImGuiID node_id = checkID(L);
	ImGui::DockBuilderRemoveNode(node_id);
	return 0;
}

int DockBuilderRemoveNodeChildNodes(lua_State* L)
{
	STACK_CHECKER(L, "dockBuilderRemoveNodeChildNodes", 0);

	ImGuiID node_id = checkID(L, 2);
	ImGui::DockBuilderRemoveNodeChildNodes(node_id);
	return 0;
}

int DockBuilderRemoveNodeDockedWindows(lua_State* L)
{
	STACK_CHECKER(L, "dockBuilderRemoveNodeDockedWindows", 0);

	lua_Number node_id = checkID(L);
	bool clear_settings_refs = lua_toboolean(L, 3);
	ImGui::DockBuilderRemoveNodeDockedWindows(node_id, clear_settings_refs);
	return 0;
}

int DockBuilderSplitNode(lua_State* L)
{
	STACK_CHECKER(L, "dockBuilderSplitNode", 3);

	ImGuiID id = checkID(L);
	ImGuiDir split_dir = luaL_checkinteger(L, 3);
	float size_ratio_for_node_at_dir = luaL_checknumber(L, 4);
	ImGuiID* out_id_at_dir;
	ImGuiID* out_id_at_opposite_dir;

	if (lua_isnil(L, 5))
		out_id_at_dir = NULL;
	else
	{
		ImGuiID id = checkID(L, 5);
		out_id_at_dir = &id;
	}

	if (lua_isnil(L, 6))
		out_id_at_opposite_dir = NULL;
	else
	{
		ImGuiID id = checkID(L, 6);
		out_id_at_opposite_dir = &id;
	}

	lua_pushinteger(L, ImGui::DockBuilderSplitNode(id, split_dir, size_ratio_for_node_at_dir, out_id_at_dir, out_id_at_opposite_dir));
	if (out_id_at_dir == nullptr)
		lua_pushnil(L);
	else
		lua_pushnumber(L, (lua_Number)(*out_id_at_dir));

	if (out_id_at_opposite_dir == nullptr)
		lua_pushnil(L);
	else
		lua_pushnumber(L, (lua_Number)(*out_id_at_opposite_dir));
	return 3;
}

// NOT TESTED
int DockBuilderCopyNode(lua_State* L)
{
	ImGuiID src_node_id = checkID(L);
	ImGuiID dst_node_id = checkID(L, 3);
	ImVector<ImGuiID>* out_node_remap_pairs;

	ImGui::DockBuilderCopyNode(src_node_id, dst_node_id, out_node_remap_pairs);

	int count = out_node_remap_pairs->Size;

	lua_createtable(L, count, 0);
	for (int i = 0; i < count; i++)
	{
		lua_pushnumber(L, (*out_node_remap_pairs)[i]);
		lua_rawgeti(L, -2, i + 1);
	}
	return 1;
}

int DockBuilderCopyWindowSettings(lua_State* L)
{
	STACK_CHECKER(L, "dockBuilderCopyWindowSettings", 0);

	const char* src_name = luaL_checkstring(L, 2);
	const char* dst_name = luaL_checkstring(L, 3);
	ImGui::DockBuilderCopyWindowSettings(src_name, dst_name);
	return 0;
}

int DockBuilderCopyDockSpace(lua_State* L)
{
	STACK_CHECKER(L, "dockBuilderCopyDockSpace", 0);

	ImGuiID src_dockspace_id = checkID(L);
	ImGuiID dst_dockspace_id = checkID(L, 3);
	ImVector<const char*>* in_window_remap_pairs;
	ImGui::DockBuilderCopyDockSpace(src_dockspace_id, dst_dockspace_id, in_window_remap_pairs);
	return 0;
}

int DockBuilderFinish(lua_State* L)
{
	STACK_CHECKER(L, "dockBuilderFinish", 0);

	ImGuiID node_id = checkID(L);
	ImGui::DockBuilderFinish(node_id);
	return 0;
}

int DockBuilderGetNode(lua_State* L)
{
	STACK_CHECKER(L, "dockBuilderGetNode", 1);

	ImGuiID node_id = checkID(L, 2);
	ImGuiDockNode* node = ImGui::DockBuilderGetNode(node_id);
	if (node == nullptr)
	{
		lua_pushnil(L);
		return 1;
	}

	g_pushInstance(L, "ImGuiDockNode", node);
	return 1;
}

int DockBuilder_Node_GetID(lua_State* L)
{
	STACK_CHECKER(L, "getID", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->ID);
	return 1;
}

int DockBuilder_Node_GetSharedFlags(lua_State* L)
{
	STACK_CHECKER(L, "getSharedFlags", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->SharedFlags);
	return 1;
}

int DockBuilder_Node_GetLocalFlags(lua_State* L)
{
	STACK_CHECKER(L, "getLocalFlags", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->LocalFlags);
	return 1;
}

int DockBuilder_Node_GetParentNode(lua_State* L)
{
	STACK_CHECKER(L, "getParentNode", 1);


	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	if (node == nullptr)
	{
		lua_pushnil(L);
		return 1;
	}
	g_pushInstance(L, "ImGuiDockNode", node->ParentNode);
	return 1;
}

int DockBuilder_Node_GetChildNodes(lua_State* L)
{
	STACK_CHECKER(L, "getChildNodes", 2);


	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	if (node->ChildNodes[0] == nullptr)
		lua_pushnil(L);
	else
		g_pushInstance(L, "ImGuiDockNode", node->ChildNodes[0]);

	if (node->ChildNodes[1] == nullptr)
		lua_pushnil(L);
	else
		g_pushInstance(L, "ImGuiDockNode", node->ChildNodes[1]);
	return 2;
}

/*
int DockBuilder_Node_GetWindows(lua_State* L)
{
	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->Windows);
	return 1;
}
*/

int DockBuilder_Node_GetTabBar(lua_State* L)
{
	STACK_CHECKER(L, "getTabBar", 1);


	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");

	if (node->TabBar == nullptr)
	{
		lua_pushnil(L);
		return 1;
	}

	g_pushInstance(L, "ImGuiTabBar", node->TabBar);
	return 1;
}

int DockBuilder_Node_GetPos(lua_State* L)
{
	STACK_CHECKER(L, "getPos", 2);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->Pos.x);
	lua_pushnumber(L, node->Pos.y);
	return 2;
}

int DockBuilder_Node_GetSize(lua_State* L)
{
	STACK_CHECKER(L, "getSize", 2);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->Size.x);
	lua_pushnumber(L, node->Size.y);
	return 2;
}

int DockBuilder_Node_GetSizeRef(lua_State* L)
{
	STACK_CHECKER(L, "getSizeRef", 2);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->SizeRef.x);
	lua_pushnumber(L, node->SizeRef.y);
	return 2;
}

int DockBuilder_Node_GetSplitAxis(lua_State* L)
{
	STACK_CHECKER(L, "getSplitAxis", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->SplitAxis);
	return 1;
}

/*
int DockBuilder_Node_GetWindowClass(lua_State* L)
{
	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->WindowClass);
	return 1;
}
*/

int DockBuilder_Node_GetState(lua_State* L)
{
	STACK_CHECKER(L, "getState", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->State);
	return 1;
}

/*
int DockBuilder_Node_GetHostWindow(lua_State* L)
{
	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->HostWindow);
	return 1;
}
int DockBuilder_Node_GetVisibleWindow(lua_State* L)
{
	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->VisibleWindow);
	return 1;
}
*/

int DockBuilder_Node_GetCentralNode(lua_State* L)
{
	STACK_CHECKER(L, "getCentralNode", 1);


	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	g_pushInstance(L, "ImGuiDockNode", node->CentralNode);
	return 1;
}

int DockBuilder_Node_GetOnlyNodeWithWindows(lua_State* L)
{
	STACK_CHECKER(L, "getOnlyNodeWithWindows", 1);


	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	g_pushInstance(L, "ImGuiDockNode", node->OnlyNodeWithWindows);
	return 1;
}

int DockBuilder_Node_GetLastFrameAlive(lua_State* L)
{
	STACK_CHECKER(L, "getLastFrameAlive", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->LastFrameAlive);
	return 1;
}

int DockBuilder_Node_GetLastFrameActive(lua_State* L)
{
	STACK_CHECKER(L, "getLastFrameActive", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->LastFrameActive);
	return 1;
}

int DockBuilder_Node_GetLastFrameFocused(lua_State* L)
{
	STACK_CHECKER(L, "getLastFrameFocused", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->LastFrameFocused);
	return 1;
}

int DockBuilder_Node_GetLastFocusedNodeId(lua_State* L)
{
	STACK_CHECKER(L, "getLastFocusedNodeId", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->LastFocusedNodeId);
	return 1;
}

int DockBuilder_Node_GetSelectedTabId(lua_State* L)
{
	STACK_CHECKER(L, "getSelectedTabId", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->SelectedTabId);
	return 1;
}

int DockBuilder_Node_WantCloseTabId(lua_State* L)
{
	STACK_CHECKER(L, "getWantCloseTabId", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->WantCloseTabId);
	return 1;
}

int DockBuilder_Node_GetAuthorityForPos(lua_State* L)
{
	STACK_CHECKER(L, "getAuthorityForPos", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->AuthorityForPos);
	return 1;
}

int DockBuilder_Node_GetAuthorityForSize(lua_State* L)
{
	STACK_CHECKER(L, "getAuthorityForSize", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->AuthorityForSize);
	return 1;
}

int DockBuilder_Node_GetAuthorityForViewport(lua_State* L)
{
	STACK_CHECKER(L, "getAuthorityForViewport", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->AuthorityForViewport);
	return 1;
}

int DockBuilder_Node_IsVisible(lua_State* L)
{
	STACK_CHECKER(L, "isVisible", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->IsVisible);
	return 1;
}

int DockBuilder_Node_IsFocused(lua_State* L)
{
	STACK_CHECKER(L, "isFocused", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->IsFocused);
	return 1;
}

int DockBuilder_Node_HasCloseButton(lua_State* L)
{
	STACK_CHECKER(L, "hasCloseButton", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->HasCloseButton);
	return 1;
}

int DockBuilder_Node_HasWindowMenuButton(lua_State* L)
{
	STACK_CHECKER(L, "hasWindowMenuButton", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->HasWindowMenuButton);
	return 1;
}

int DockBuilder_Node_EnableCloseButton(lua_State* L)
{
	STACK_CHECKER(L, "enableCloseButton", 0);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	node->EnableCloseButton = lua_toboolean(L, 2);
	return 0;
}

int DockBuilder_Node_IsCloseButtonEnable(lua_State* L)
{
	STACK_CHECKER(L, "isCloseButtonEnable", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->EnableCloseButton);
	return 1;
}

int DockBuilder_Node_WantCloseAll(lua_State* L)
{
	STACK_CHECKER(L, "wantCloseAll", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->WantCloseAll);
	return 1;
}

int DockBuilder_Node_WantLockSizeOnce(lua_State* L)
{
	STACK_CHECKER(L, "wantLockSizeOnce", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->WantLockSizeOnce);
	return 1;
}

int DockBuilder_Node_WantMouseMove(lua_State* L)
{
	STACK_CHECKER(L, "wantMouseMove", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->WantMouseMove);
	return 1;
}

int DockBuilder_Node_WantHiddenTabBarUpdate(lua_State* L)
{
	STACK_CHECKER(L, "wantHiddenTabBarUpdate", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->WantHiddenTabBarUpdate);
	return 1;
}

int DockBuilder_Node_WantHiddenTabBarToggle(lua_State* L)
{
	STACK_CHECKER(L, "wantHiddenTabBarToggle", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->WantHiddenTabBarToggle);
	return 1;
}

int DockBuilder_Node_MarkedForPosSizeWrite(lua_State* L)
{
	STACK_CHECKER(L, "isMarkedForPosSizeWrite", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->MarkedForPosSizeWrite);
	return 1;
}

int DockBuilder_Node_IsRootNode(lua_State* L)
{
	STACK_CHECKER(L, "isRootNode", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->IsRootNode());
	return 1;
}

int DockBuilder_Node_IsDockSpace(lua_State* L)
{
	STACK_CHECKER(L, "isDockSpace", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->IsDockSpace());
	return 1;
}

int DockBuilder_Node_IsFloatingNode(lua_State* L)
{
	STACK_CHECKER(L, "isFloatingNode", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->IsFloatingNode());
	return 1;
}

int DockBuilder_Node_IsCentralNode(lua_State* L)
{
	STACK_CHECKER(L, "isCentralNode", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->IsCentralNode());
	return 1;
}

int DockBuilder_Node_IsHiddenTabBar(lua_State* L)
{
	STACK_CHECKER(L, "isHiddenTabBar", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->IsHiddenTabBar());
	return 1;
}

int DockBuilder_Node_IsNoTabBar(lua_State* L)
{
	STACK_CHECKER(L, "isNoTabBar", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->IsNoTabBar());
	return 1;
}

int DockBuilder_Node_IsSplitNode(lua_State* L)
{
	STACK_CHECKER(L, "isSplitNode", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->IsSplitNode());
	return 1;
}

int DockBuilder_Node_IsLeafNode(lua_State* L)
{
	STACK_CHECKER(L, "isLeafNode", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->IsLeafNode());
	return 1;
}

int DockBuilder_Node_IsEmpty(lua_State* L)
{
	STACK_CHECKER(L, "isEmpty", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushboolean(L, node->IsEmpty());
	return 1;
}

int DockBuilder_Node_GetMergedFlags(lua_State* L)
{
	STACK_CHECKER(L, "getMergedFlags", 1);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	lua_pushnumber(L, node->GetMergedFlags());
	return 1;
}

int DockBuilder_Node_Rect(lua_State* L)
{
	STACK_CHECKER(L, "rect", 4);

	ImGuiDockNode* node = getPtr<ImGuiDockNode>(L, "ImGuiDockNode");
	ImRect rect = node->Rect();
	lua_pushnumber(L, rect.Min.x);
	lua_pushnumber(L, rect.Min.y);
	lua_pushnumber(L, rect.Max.x);
	lua_pushnumber(L, rect.Max.y);
	return 4;
}

/// TabItem +

int TabItem_GetID(lua_State* L)
{
	STACK_CHECKER(L, "getID", 1);

	ImGuiTabItem* tabItem = getPtr<ImGuiTabItem>(L, "ImGuiTabItem");
	lua_pushnumber(L, tabItem->ID);
	return 1;
}

int TabItem_GetFlags(lua_State* L)
{
	STACK_CHECKER(L, "getFlags", 1);

	ImGuiTabItem* tabItem = getPtr<ImGuiTabItem>(L, "ImGuiTabItem");
	lua_pushnumber(L, tabItem->Flags);
	return 1;
}

int TabItem_GetLastFrameVisible(lua_State* L)
{
	STACK_CHECKER(L, "getLastFrameVisible", 1);

	ImGuiTabItem* tabItem = getPtr<ImGuiTabItem>(L, "ImGuiTabItem");
	lua_pushnumber(L, tabItem->LastFrameVisible);
	return 1;
}

int TabItem_GetLastFrameSelected(lua_State* L)
{
	STACK_CHECKER(L, "getLastFrameSelected", 1);

	ImGuiTabItem* tabItem = getPtr<ImGuiTabItem>(L, "ImGuiTabItem");
	lua_pushnumber(L, tabItem->LastFrameSelected);
	return 1;
}

int TabItem_GetOffset(lua_State* L)
{
	STACK_CHECKER(L, "getOffset", 1);

	ImGuiTabItem* tabItem = getPtr<ImGuiTabItem>(L, "ImGuiTabItem");
	lua_pushnumber(L, tabItem->Offset);
	return 1;
}

int TabItem_GetWidth(lua_State* L)
{
	STACK_CHECKER(L, "getWidth", 1);

	ImGuiTabItem* tabItem = getPtr<ImGuiTabItem>(L, "ImGuiTabItem");
	lua_pushnumber(L, tabItem->Width);
	return 1;
}

int TabItem_GetContentWidth(lua_State* L)
{
	STACK_CHECKER(L, "getContentWidth", 1);

	ImGuiTabItem* tabItem = getPtr<ImGuiTabItem>(L, "ImGuiTabItem");
	lua_pushnumber(L, tabItem->ContentWidth);
	return 1;
}

int TabItem_GetNameOffset(lua_State* L)
{
	STACK_CHECKER(L, "getNameOffset", 1);

	ImGuiTabItem* tabItem = getPtr<ImGuiTabItem>(L, "ImGuiTabItem");
	lua_pushnumber(L, tabItem->NameOffset);
	return 1;
}

int TabItem_GetBeginOrder(lua_State* L)
{
	STACK_CHECKER(L, "getBeginOrder", 1);

	ImGuiTabItem* tabItem = getPtr<ImGuiTabItem>(L, "ImGuiTabItem");
	lua_pushnumber(L, tabItem->BeginOrder);
	return 1;
}

int TabItem_GetIndexDuringLayout(lua_State* L)
{
	STACK_CHECKER(L, "getIndexDuringLayout", 1);

	ImGuiTabItem* tabItem = getPtr<ImGuiTabItem>(L, "ImGuiTabItem");
	lua_pushnumber(L, tabItem->IndexDuringLayout);
	return 1;
}

int TabItem_WantClose(lua_State* L)
{
	STACK_CHECKER(L, "wantClose", 1);

	ImGuiTabItem* tabItem = getPtr<ImGuiTabItem>(L, "ImGuiTabItem");
	lua_pushboolean(L, tabItem->WantClose);
	return 1;
}

/// TabItem -

/// TabBar +
int TabBar_GetTabs(lua_State* L)
{
	STACK_CHECKER(L, "getTabs", 1);


	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	int count = tabBar->Tabs.Size;
	lua_createtable(L, count, 0);
	for (int i = 0; i < count; i++)
	{
		g_pushInstance(L, "ImGuiTabItem", &tabBar->Tabs[i]);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}

int TabBar_GetTab(lua_State* L)
{
	STACK_CHECKER(L, "getTab", 1);


	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	int count = tabBar->Tabs.Size;
	int index = luaL_checkinteger(L, 2);
	LUA_ASSERTF(index >= 0 && index <= count, "Tab index is out of bounds. Must be: [1; %d]", count);
	g_pushInstance(L, "ImGuiTabItem", &tabBar->Tabs[index]);
	return 1;
}

int TabBar_GetTabCount(lua_State* L)
{
	STACK_CHECKER(L, "getTabCount", 1);


	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushinteger(L, tabBar->Tabs.Size);
	return 1;
}

int TabBar_GetFlags(lua_State* L)
{
	STACK_CHECKER(L, "getFlags", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushinteger(L, tabBar->Flags);
	return 1;
}

int TabBar_GetID(lua_State* L)
{
	STACK_CHECKER(L, "getID", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->ID);
	return 1;
}

int TabBar_GetSelectedTabId(lua_State* L)
{
	STACK_CHECKER(L, "getSelectedTabId", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->SelectedTabId);
	return 1;
}

int TabBar_GetNextSelectedTabId(lua_State* L)
{
	STACK_CHECKER(L, "getNextSelectedTabId", 0);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->NextSelectedTabId);
	return 0;
}

int TabBar_GetVisibleTabId(lua_State* L)
{
	STACK_CHECKER(L, "getVisibleTabId", 0);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->VisibleTabId);
	return 0;
}

int TabBar_GetCurrFrameVisible(lua_State* L)
{
	STACK_CHECKER(L, "getCurrFrameVisible", 0);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushinteger(L, tabBar->CurrFrameVisible);
	return 0;
}

int TabBar_GetPrevFrameVisible(lua_State* L)
{
	STACK_CHECKER(L, "getPrevFrameVisible", 0);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushinteger(L, tabBar->PrevFrameVisible);
	return 0;
}

int TabBar_GetBarRect(lua_State* L)
{
	STACK_CHECKER(L, "getBarRect", 4);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->BarRect.Min.x);
	lua_pushnumber(L, tabBar->BarRect.Min.y);
	lua_pushnumber(L, tabBar->BarRect.Max.x);
	lua_pushnumber(L, tabBar->BarRect.Max.y);
	return 4;
}

int TabBar_GetCurrTabsContentsHeight(lua_State* L)
{
	STACK_CHECKER(L, "getCurrTabsContentsHeight", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->CurrTabsContentsHeight);
	return 1;
}

int TabBar_GetPrevTabsContentsHeight(lua_State* L)
{
	STACK_CHECKER(L, "getPrevTabsContentsHeight", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->PrevTabsContentsHeight);
	return 1;
}

int TabBar_GetWidthAllTabs(lua_State* L)
{
	STACK_CHECKER(L, "getWidthAllTabs", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->WidthAllTabs);
	return 1;
}

int TabBar_GetWidthAllTabsIdeal(lua_State* L)
{
	STACK_CHECKER(L, "getWidthAllTabsIdeal", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->WidthAllTabsIdeal);
	return 1;
}

int TabBar_GetScrollingAnim(lua_State* L)
{
	STACK_CHECKER(L, "getScrollingAnim", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->ScrollingAnim);
	return 1;
}

int TabBar_GetScrollingTarget(lua_State* L)
{
	STACK_CHECKER(L, "getScrollingTarget", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->ScrollingTarget);
	return 1;
}

int TabBar_GetScrollingTargetDistToVisibility(lua_State* L)
{
	STACK_CHECKER(L, "getScrollingTargetDistToVisibility", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->ScrollingTargetDistToVisibility);
	return 1;
}

int TabBar_GetScrollingSpeed(lua_State* L)
{
	STACK_CHECKER(L, "getScrollingSpeed", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->ScrollingSpeed);
	return 1;
}

int TabBar_GetScrollingRectMinX(lua_State* L)
{
	STACK_CHECKER(L, "getScrollingRectMinX", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->ScrollingRectMinX);
	return 1;
}

int TabBar_GetScrollingRectMaxX(lua_State* L)
{
	STACK_CHECKER(L, "getScrollingRectMaxX", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->ScrollingRectMaxX);
	return 1;
}

int TabBar_GetReorderRequestTabId(lua_State* L)
{
	STACK_CHECKER(L, "getReorderRequestTabId", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->ReorderRequestTabId);
	return 1;
}

int TabBar_GetReorderRequestDir(lua_State* L)
{
	STACK_CHECKER(L, "getReorderRequestDir", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->ReorderRequestDir);
	return 1;
}

int TabBar_GetBeginCount(lua_State* L)
{
	STACK_CHECKER(L, "getBeginCount", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->BeginCount);
	return 1;
}

int TabBar_WantLayout(lua_State* L)
{
	STACK_CHECKER(L, "wantLayout", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushboolean(L, tabBar->WantLayout);
	return 1;
}

int TabBar_VisibleTabWasSubmitted(lua_State* L)
{
	STACK_CHECKER(L, "visibleTabWasSubmitted", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushboolean(L, tabBar->VisibleTabWasSubmitted);
	return 1;
}

int TabBar_TabsAddedNew(lua_State* L)
{
	STACK_CHECKER(L, "getTabsAddedNew", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushboolean(L, tabBar->TabsAddedNew);
	return 1;
}

int TabBar_GetTabsActiveCount(lua_State* L)
{
	STACK_CHECKER(L, "getTabsActiveCount", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->TabsActiveCount);
	return 1;
}

int TabBar_GetLastTabItemIdx(lua_State* L)
{
	STACK_CHECKER(L, "getLastTabItemIdx", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->LastTabItemIdx);
	return 1;
}

int TabBar_GetItemSpacingY(lua_State* L)
{
	STACK_CHECKER(L, "getItemSpacingY", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->ItemSpacingY);
	return 1;
}

int TabBar_GetFramePadding(lua_State* L)
{
	STACK_CHECKER(L, "getFramePadding", 2);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->FramePadding.x);
	lua_pushnumber(L, tabBar->FramePadding.y);
	return 2;
}

int TabBar_GetBackupCursorPos(lua_State* L)
{
	STACK_CHECKER(L, "getBackupCursorPos", 2);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushnumber(L, tabBar->BackupCursorPos.x);
	lua_pushnumber(L, tabBar->BackupCursorPos.y);
	return 2;
}

int TabBar_GetTabsNames(lua_State* L)
{
	STACK_CHECKER(L, "getTabsNames", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	lua_pushstring(L, tabBar->TabsNames.c_str());
	return 1;
}

int TabBar_GetTabOrder(lua_State* L)
{
	STACK_CHECKER(L, "getTabOrder", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	ImGuiTabItem* tab = getPtr<ImGuiTabItem>(L, "ImGuiTabItem");

	lua_pushnumber(L, tabBar->GetTabOrder(tab));
	return 1;
}

int TabBar_GetTabName(lua_State* L)
{
	STACK_CHECKER(L, "getTabName", 1);

	ImGuiTabBar* tabBar = getPtr<ImGuiTabBar>(L, "ImGuiTabBar");
	ImGuiTabItem* tab = getPtr<ImGuiTabItem>(L, "ImGuiTabItem");

	lua_pushstring(L, tabBar->GetTabName(tab));
	return 1;
}

/// TabBar -

#endif // IS_DOCKING_BUILD

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Logging/Capture
///
/////////////////////////////////////////////////////////////////////////////////////////////

int LogToTTY(lua_State* L)
{
	STACK_CHECKER(L, "logToTTY", 0);

	int auto_open_depth = luaL_optinteger(L, 2, -1);
	ImGui::LogToTTY(auto_open_depth);
	return 0;
}

int LogToFile(lua_State* L)
{
	STACK_CHECKER(L, "logToFile", 0);

	ImGuiIO& io = ImGui::GetIO();

	LUA_ASSERT(io.LogFilename != NULL, "Log to file is disabled! Use ImGui:setLogFilename(filename) first.");

	int auto_open_depth = luaL_optinteger(L, 2, -1);

	if (lua_gettop(L) < 3)
		ImGui::LogToFile(auto_open_depth, NULL);
	else
	{
		const char* filename = luaL_checkstring(L, 3);
		ImGui::LogToFile(auto_open_depth, filename);
	}

	return 0;
}

int LogToClipboard(lua_State* L)
{
	STACK_CHECKER(L, "logToClipboard", 0);

	int auto_open_depth = luaL_optinteger(L, 2, -1);
	ImGui::LogToClipboard(auto_open_depth);
	return 0;
}

int LogFinish(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "logFinish", 0);

	ImGui::LogFinish();
	return 0;
}

int LogButtons(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "logButtons", 0);

	ImGui::LogButtons();
	return 0;
}

int LogText(lua_State* L)
{
	STACK_CHECKER(L, "logText", 0);

	const char* text = luaL_checkstring(L, 2);
	ImGui::LogText("%s", text);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Drag and Drop
///
/////////////////////////////////////////////////////////////////////////////////////////////

int BeginDragDropSource(lua_State* L)
{
	STACK_CHECKER(L, "beginDragDropSource", 1);

	ImGuiDragDropFlags flags = luaL_optinteger(L, 2, 0);
	lua_pushboolean(L, ImGui::BeginDragDropSource(flags));
	return 1;
}

int SetNumberDragDropPayload(lua_State* L)
{
	STACK_CHECKER(L, "setNumDragDropPayload", 1);

	const char* type = luaL_checkstring(L, 2);
	double v = luaL_checknumber(L, 3);
	ImGuiCond cond = luaL_optinteger(L, 4, 0);
	lua_pushboolean(L, ImGui::SetDragDropPayload(type, (const void*)&v, sizeof(double), cond));
	return 1;
}

int SetStringDragDropPayload(lua_State* L)
{
	STACK_CHECKER(L, "setStrDragDropPayload", 1);

	const char* type = luaL_checkstring(L, 2);
	const char* str = luaL_checkstring(L, 3);

	ImGuiCond cond = luaL_optinteger(L, 4, 0);
	lua_pushboolean(L, ImGui::SetDragDropPayload(type, str, strlen(str), cond));
	return 1;
}

int SetArrDragDropPayload(lua_State* L)
{
	GidImGui* imgui = getImgui(L);
	STACK_CHECKER(L, "setArrDragDropPayload", 1);

	const char* type = luaL_checkstring(L, 2);
	luaL_checktype(L, 3, LUA_TTABLE);
	ImGuiCond cond = luaL_optinteger(L, 4, 0);

	void* data = imgui->ctx->DragDropPayload.Data;

	lua_pushvalue(L, 3);
	int idx = luaL_ref(L, LUA_REGISTRYINDEX);

	lua_pushboolean(L, ImGui::SetDragDropPayload(type, (const void*)&idx, sizeof(int), cond));
	return 1;
}

int EndDragDropSource(lua_State* L)
{
	STACK_CHECKER(L, "endDragDropSource", 0);

	ImGui::EndDragDropSource();
	return 0;
}

int BeginDragDropTarget(lua_State* L)
{
	STACK_CHECKER(L, "beginDragDropTarget", 1);

	lua_pushboolean(L, ImGui::BeginDragDropTarget());
	return 1;
}

int AcceptDragDropPayload(lua_State* L)
{
	STACK_CHECKER(L, "acceptDragDropPayload", 1);

	const char* type = luaL_checkstring(L, 2);
	ImGuiDragDropFlags flags = luaL_optinteger(L, 3, 0);

	const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type, flags);
	if (payload == nullptr)
	{
		lua_pushnil(L);
	}
	else
	{
		g_pushInstance(L, "ImGuiPayload", const_cast<ImGuiPayload*>(reinterpret_cast<const ImGuiPayload*>(payload)));
	}
	return 1;
}

int EndDragDropTarget(lua_State* L)
{
	STACK_CHECKER(L, "endDragDropTarget", 0);

	ImGui::EndDragDropTarget();
	return 0;
}

int GetDragDropPayload(lua_State* L)
{
	STACK_CHECKER(L, "getDragDropPayload", 1);

	const ImGuiPayload* payload = ImGui::GetDragDropPayload();
	if (payload == nullptr)
	{
		lua_pushnil(L);
	}
	else
	{
		g_pushInstance(L, "ImGuiPayload", const_cast<ImGuiPayload*>(reinterpret_cast<const ImGuiPayload*>(payload)));
	}

	return 1;
}

int Payload_GetNumberData(lua_State* L)
{
	STACK_CHECKER(L, "getNumData", 1);

	ImGuiPayload* payload = getPtr<ImGuiPayload>(L, "ImGuiPayload");
	double* v = (double*)(payload->Data);
	lua_pushnumber(L, *v);
	return 1;
}

int Payload_GetStringData(lua_State* L)
{
	STACK_CHECKER(L, "getStrData", 1);

	ImGuiPayload* payload = getPtr<ImGuiPayload>(L, "ImGuiPayload");
	const char* str = static_cast<const char*>(payload->Data);
	lua_pushlstring(L, str, payload->DataSize);
	return 1;
}

int Payload_GetArrData(lua_State* L)
{
	STACK_CHECKER(L, "getArrData", 1);

	ImGuiPayload* payload = getPtr<ImGuiPayload>(L, "ImGuiPayload");
	int* idx = (int*)(payload->Data);
	lua_rawgeti(L, LUA_REGISTRYINDEX, *idx);
	luaL_unref(L, LUA_REGISTRYINDEX, *idx);
	return 1;
}

int Payload_GetColor3Data(lua_State* L)
{
	STACK_CHECKER(L, "getColor3Data", 1);

	ImGuiPayload* payload = getPtr<ImGuiPayload>(L, "ImGuiPayload");
	float* v = (float*)(payload->Data);
	GColor color = GColor::toHex(v[0], v[1], v[2], 1.0);
	lua_pushinteger(L, color.hex);
	return 1;
}

int Payload_GetColor4Data(lua_State* L)
{
	STACK_CHECKER(L, "getColor4Data", 2);

	ImGuiPayload* payload = getPtr<ImGuiPayload>(L, "ImGuiPayload");
	float* v = (float*)(payload->Data);
	GColor color = GColor::toHex(v[0], v[1], v[2], v[3]);
	lua_pushinteger(L, color.hex);
	lua_pushnumber(L, color.alpha);
	return 2;
}

// TODO add tables ?

int Payload_Clear(lua_State* L)
{
	STACK_CHECKER(L, "clear", 0);

	ImGuiPayload* payload = getPtr<ImGuiPayload>(L, "ImGuiPayload");
	payload->Clear();
	return 0;
}

int Payload_GetDataSize(lua_State* L)
{
	STACK_CHECKER(L, "getDataSize", 1);

	ImGuiPayload* payload = getPtr<ImGuiPayload>(L, "ImGuiPayload");
	lua_pushinteger(L, payload->DataSize);
	return 1;
}

int Payload_IsDataType(lua_State* L)
{
	STACK_CHECKER(L, "isDataType", 1);

	const char* datatype = luaL_checkstring(L, 2);

	ImGuiPayload* payload = getPtr<ImGuiPayload>(L, "ImGuiPayload");
	lua_pushboolean(L, payload->IsDataType(datatype));
	return 1;
}

int Payload_IsPreview(lua_State* L)
{
	STACK_CHECKER(L, "isPreview", 1);

	ImGuiPayload* payload = getPtr<ImGuiPayload>(L, "ImGuiPayload");
	lua_pushboolean(L, payload->IsPreview());
	return 1;
}

int Payload_IsDelivery(lua_State* L)
{
	STACK_CHECKER(L, "isDelivery", 1);

	ImGuiPayload* payload = getPtr<ImGuiPayload>(L, "ImGuiPayload");
	lua_pushboolean(L, payload->IsDelivery());
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Clipping
///
/////////////////////////////////////////////////////////////////////////////////////////////

int PushClipRect(lua_State* L)
{
	STACK_CHECKER(L, "pushClipRect", 0);

	const ImVec2 clip_rect_min = luaL_checkvec2(L, 2);
	const ImVec2 clip_rect_max = luaL_checkvec2(L, 4);
	bool intersect_with_current_clip_rect = lua_toboolean(L, 6);
	ImGui::PushClipRect(clip_rect_min, clip_rect_max, intersect_with_current_clip_rect);
	return 0;
}

int PopClipRect(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "popClipRect", 0);

	ImGui::PopClipRect();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Focus, Activation
///
/////////////////////////////////////////////////////////////////////////////////////////////

int SetItemDefaultFocus(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "setItemDefaultFocus", 0);

	ImGui::SetItemDefaultFocus();
	return 0;
}

int SetKeyboardFocusHere(lua_State* L)
{
	STACK_CHECKER(L, "setKeyboardFocusHere", 0);

	int offset = luaL_optinteger(L, 2, 0);
	ImGui::SetKeyboardFocusHere(offset);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Item/Widgets Utilities
///
/////////////////////////////////////////////////////////////////////////////////////////////

int IsItemHovered(lua_State* L)
{
	STACK_CHECKER(L, "isItemHovered", 1);

	ImGuiHoveredFlags flags = luaL_optinteger(L, 2, 0);
	lua_pushboolean(L, ImGui::IsItemHovered(flags));
	return 1;
}

int IsItemActive(lua_State* L)
{
	STACK_CHECKER(L, "isItemActive", 1);

	lua_pushboolean(L, ImGui::IsItemActive());
	return 1;
}

int IsItemFocused(lua_State* L)
{
	STACK_CHECKER(L, "isItemFocused", 1);

	lua_pushboolean(L, ImGui::IsItemFocused());
	return 1;
}

int IsItemClicked(lua_State* L)
{
	STACK_CHECKER(L, "isItemClicked", 1);

	ImGuiMouseButton mouse_button = giderosMouseToImGui(luaL_optinteger(L, 2, GINPUT_LEFT_BUTTON));
	lua_pushboolean(L, ImGui::IsItemClicked(mouse_button));
	return 1;
}

int IsItemVisible(lua_State* L)
{
	STACK_CHECKER(L, "isItemVisible", 1);

	lua_pushboolean(L, ImGui::IsItemVisible());
	return 1;
}

int IsItemEdited(lua_State* L)
{
	STACK_CHECKER(L, "isItemEdited", 1);

	lua_pushboolean(L, ImGui::IsItemEdited());
	return 1;
}

int IsItemActivated(lua_State* L)
{
	STACK_CHECKER(L, "isItemActivated", 1);

	lua_pushboolean(L, ImGui::IsItemActivated());
	return 1;
}

int IsItemDeactivated(lua_State* L)
{
	STACK_CHECKER(L, "isItemDeactivated", 1);

	lua_pushboolean(L, ImGui::IsItemDeactivated());
	return 1;
}

int IsItemDeactivatedAfterEdit(lua_State* L)
{
	STACK_CHECKER(L, "isItemDeactivatedAfterEdit", 1);

	lua_pushboolean(L, ImGui::IsItemDeactivatedAfterEdit());
	return 1;
}

int IsItemToggledOpen(lua_State* L)
{
	STACK_CHECKER(L, "isItemToggledOpen", 1);

	lua_pushboolean(L, ImGui::IsItemToggledOpen());
	return 1;
}

int IsAnyItemHovered(lua_State* L)
{
	STACK_CHECKER(L, "isAnyItemHovered", 1);

	lua_pushboolean(L, ImGui::IsAnyItemHovered());
	return 1;
}

int IsAnyItemActive(lua_State* L)
{
	STACK_CHECKER(L, "isAnyItemActive", 1);

	lua_pushboolean(L, ImGui::IsAnyItemActive());
	return 1;
}

int IsAnyItemFocused(lua_State* L)
{
	STACK_CHECKER(L, "isAnyItemFocused", 1);

	lua_pushboolean(L, ImGui::IsAnyItemFocused());
	return 1;
}

int GetItemRect(lua_State* L)
{
	STACK_CHECKER(L, "getItemRect", 4);

	ImVec2 min = ImGui::GetItemRectMin();
	ImVec2 max = ImGui::GetItemRectMax();
	lua_pushnumber(L, min.x);
	lua_pushnumber(L, min.y);
	lua_pushnumber(L, max.x);
	lua_pushnumber(L, max.y);
	return 4;
}

int GetItemRectMin(lua_State* L)
{
	STACK_CHECKER(L, "getItemRectMin", 2);

	ImVec2 min = ImGui::GetItemRectMin();
	lua_pushnumber(L, min.x);
	lua_pushnumber(L, min.y);
	return 2;
}

int GetItemRectMax(lua_State* L)
{
	STACK_CHECKER(L, "getItemRectMax", 2);

	ImVec2 max = ImGui::GetItemRectMax();
	lua_pushnumber(L, max.x);
	lua_pushnumber(L, max.y);
	return 2;
}

int GetItemRectSize(lua_State* L)
{
	STACK_CHECKER(L, "getItemRectSize", 2);

	ImVec2 size = ImGui::GetItemRectSize();
	lua_pushnumber(L, size.x);
	lua_pushnumber(L, size.y);
	return 2;
}

int SetItemAllowOverlap(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "setItemAllowOverlap", 0);

	ImGui::SetItemAllowOverlap();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Miscellaneous Utilities
///
/////////////////////////////////////////////////////////////////////////////////////////////

int IsRectVisible(lua_State* L)
{
	STACK_CHECKER(L, "isRectVisible", 1);

	ImVec2 size = luaL_checkvec2(L, 2);
	if (lua_gettop(L) > 3)
	{
		ImVec2 rect_max = luaL_checkvec2(L, 4);
		lua_pushboolean(L, ImGui::IsRectVisible(size, rect_max));
	}
	else
	{
		lua_pushboolean(L, ImGui::IsRectVisible(size));
	}
	return 1;
}

int GetTime(lua_State* L)
{
	STACK_CHECKER(L, "getTime", 1);

	lua_pushnumber(L, ImGui::GetTime());
	return 1;
}

int GetFrameCount(lua_State* L)
{
	STACK_CHECKER(L, "getFrameCount", 1);

	lua_pushinteger(L, ImGui::GetFrameCount());
	return 1;
}

int GetStyleColorName(lua_State* L)
{
	STACK_CHECKER(L, "getStyleColorName", 1);

	ImGuiCol idx = luaL_checkinteger(L, 2);
	lua_pushstring(L, ImGui::GetStyleColorName(idx));
	return 1;
}

int GetStyleColor(lua_State* L)
{
	STACK_CHECKER(L, "getStyleColor", 2);

	int idx = luaL_checkinteger(L, 2);
	GColor color(ImGui::GetStyleColorVec4(idx));
	lua_pushinteger(L, color.hex);
	lua_pushnumber(L, color.alpha);
	return 2;
}

int BeginChildFrame(lua_State* L)
{
	STACK_CHECKER(L, "beginChildFrame", 1);


	ImGuiID id = checkID(L);
	ImVec2 size = luaL_checkvec2(L, 3);
	ImGuiWindowFlags flags = luaL_optinteger(L, 5, 0);

	lua_pushboolean(L, ImGui::BeginChildFrame(id, size, flags));
	return 1;
}

int EndChildFrame(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "endChildFrame", 0);

	ImGui::EndChildFrame();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Text Utilities
///
/////////////////////////////////////////////////////////////////////////////////////////////

int CalcTextSize(lua_State* L)
{
	STACK_CHECKER(L, "calcTextSize", 2);

	const char* text = luaL_checkstring(L, 2);
	bool hide_text_after_double_hash = luaL_optboolean(L, 3, 0);
	float wrap_width = luaL_optnumber(L, 4, -1.0);

	ImVec2 size = ImGui::CalcTextSize(text, NULL, hide_text_after_double_hash, wrap_width);

	lua_pushnumber(L, size.x);
	lua_pushnumber(L, size.y);

	return 2;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Inputs Utilities: Keyboard
///
/////////////////////////////////////////////////////////////////////////////////////////////

int GetKeyName(lua_State* L)
{
	STACK_CHECKER(L, "GetKeyName", 1);

	int imguiKey = giderosKeyToImGuiKey(luaL_checkinteger(L, 2));
	const char* keyName = ImGui::GetKeyName(imguiKey);
	lua_pushstring(L, keyName);
	return 1;
}

int IsKeyDown(lua_State* L)
{
	STACK_CHECKER(L, "isKeyDown", 1);

	int imguiKey = giderosKeyToImGuiKey(luaL_checkinteger(L, 2));
	lua_pushboolean(L, ImGui::IsKeyDown(imguiKey));
	return 1;
}

int IsKeyPressed(lua_State* L)
{
	STACK_CHECKER(L, "isKeyPressed", 1);

	int imguiKey = giderosKeyToImGuiKey(luaL_checkinteger(L, 2));
	bool repeat = luaL_optboolean(L, 3, 1);

	lua_pushboolean(L, ImGui::IsKeyPressed(imguiKey, repeat));
	return 1;
}

int IsKeyReleased(lua_State* L)
{
	STACK_CHECKER(L, "isKeyReleased", 1);

	int imguiKey = giderosKeyToImGuiKey(luaL_checkinteger(L, 2));
	lua_pushboolean(L, ImGui::IsKeyReleased(imguiKey));
	return 1;
}

int GetKeyPressedAmount(lua_State* L)
{
	STACK_CHECKER(L, "getKeyPressedAmount", 1);

	int imguiKey = giderosKeyToImGuiKey(luaL_checkinteger(L, 2));
	double repeat_delay = luaL_checknumber(L, 3);
	double rate = luaL_checknumber(L, 4);
	lua_pushinteger(L, ImGui::GetKeyPressedAmount(imguiKey, repeat_delay, rate));
	return 1;
}

int SetNextFrameWantCaptureKeyboard(lua_State* L)
{
	STACK_CHECKER(L, "setNextFrameWantCaptureKeyboard", 0);

	bool want_capture_keyboard_value = luaL_optboolean(L, 2, 1);
	ImGui::SetNextFrameWantCaptureKeyboard(want_capture_keyboard_value);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Inputs Utilities: Mouse
///
/////////////////////////////////////////////////////////////////////////////////////////////

int IsMouseDown(lua_State* L)
{
	STACK_CHECKER(L, "isMouseDown", 1);

	ImGuiMouseButton button = giderosMouseToImGui(lua_tointeger(L, 2));
	lua_pushboolean(L, ImGui::IsMouseDown(button));
	return 1;
}

int IsMouseClicked(lua_State* L)
{
	STACK_CHECKER(L, "isMouseClicked", 1);

	ImGuiMouseButton button = giderosMouseToImGui(lua_tointeger(L, 2));
	bool repeat = luaL_optboolean(L, 3, 0);
	lua_pushboolean(L, ImGui::IsMouseClicked(button, repeat));
	return 1;
}

int IsMouseReleased(lua_State* L)
{
	STACK_CHECKER(L, "isMouseReleased", 1);

	ImGuiMouseButton button = giderosMouseToImGui(lua_tointeger(L, 2));
	lua_pushboolean(L, ImGui::IsMouseReleased(button));
	return 1;
}

int IsMouseDoubleClicked(lua_State* L)
{
	STACK_CHECKER(L, "isMouseDoubleClicked", 1);

	ImGuiMouseButton button = giderosMouseToImGui(lua_tointeger(L, 2));
	lua_pushboolean(L, ImGui::IsMouseDoubleClicked(button));
	return 1;
}

int GetMouseClickedCount(lua_State* L)
{
	STACK_CHECKER(L, "getMouseClickedCount", 1);

	ImGuiMouseButton button = giderosMouseToImGui(lua_tointeger(L, 2));
	lua_pushnumber(L, ImGui::GetMouseClickedCount(button));
	return 1;
}

int IsMouseHoveringRect(lua_State* L)
{
	STACK_CHECKER(L, "isMouseHoveringRect", 1);

	ImVec2 r_min = luaL_checkvec2(L, 2);
	ImVec2 r_max = luaL_checkvec2(L, 4);
	bool clip = luaL_optboolean(L, 6, 1);
	lua_pushboolean(L, ImGui::IsMouseHoveringRect(r_min, r_max, clip));
	return 1;
}

int IsMousePosValid(lua_State* L)
{
	STACK_CHECKER(L, "isMousePosValid", 1);

	ImVec2 mouse_pos = ImVec2(luaL_optnumber(L, 2, -FLT_MAX), luaL_optnumber(L, 3, -FLT_MAX));
	lua_pushboolean(L, ImGui::IsMousePosValid(&mouse_pos));
	return 1;
}

int IsAnyMouseDown(lua_State* L)
{
	STACK_CHECKER(L, "isAnyMouseDown", 1);

	lua_pushboolean(L, ImGui::IsAnyMouseDown());
	return 1;
}

int GetMousePos(lua_State* L)
{
	STACK_CHECKER(L, "getMousePos", 2);

	ImVec2 pos = ImGui::GetMousePos();
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	return 2;
}

int GetMousePosOnOpeningCurrentPopup(lua_State* L)
{
	STACK_CHECKER(L, "getMousePosOnOpeningCurrentPopup", 2);

	ImVec2 pos = ImGui::GetMousePosOnOpeningCurrentPopup();
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	return 2;
}

int IsMouseDragging(lua_State* L)
{
	STACK_CHECKER(L, "isMouseDragging", 1);

	ImGuiMouseButton button = giderosMouseToImGui(lua_tointeger(L, 2));
	float lock_threshold = luaL_optnumber(L, 3, -1.0f);
	lua_pushboolean(L, ImGui::IsMouseDragging(button, lock_threshold));
	return 1;
}

int GetMouseDragDelta(lua_State* L)
{
	STACK_CHECKER(L, "getMouseDragDelta", 2);

	ImGuiMouseButton button = giderosMouseToImGui(lua_tointeger(L, 2));
	float lock_threshold = luaL_optnumber(L, 3, -1.0f);
	ImVec2 pos = ImGui::GetMouseDragDelta(button, lock_threshold);
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	return 2;
}

int ResetMouseDragDelta(lua_State* L)
{
	STACK_CHECKER(L, "resetMouseDragDelta", 0);

	ImGuiMouseButton button = giderosMouseToImGui(lua_tointeger(L, 2));
	ImGui::ResetMouseDragDelta(button);
	return 0;
}

int GetMouseCursor(lua_State* L)
{
	STACK_CHECKER(L, "getMouseCursor", 1);

	lua_pushinteger(L, ImGui::GetMouseCursor());
	return 1;
}

int SetMouseCursor(lua_State* L)
{
	STACK_CHECKER(L, "setMouseCursor", 0);

	ImGuiMouseCursor cursor_type = luaL_checkinteger(L, 2);
	ImGui::SetMouseCursor(cursor_type);
	return 0;
}

int SetNextFrameWantCaptureMouse(lua_State* L)
{
	STACK_CHECKER(L, "setNextFrameWantCaptureMouse", 0);

	bool want_capture_mouse_value = luaL_optboolean(L, 2, 1) > 0;
	ImGui::SetNextFrameWantCaptureMouse(want_capture_mouse_value);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// STYLES
///
/////////////////////////////////////////////////////////////////////////////////////////////

int StyleDark(lua_State* L)
{
	STACK_CHECKER(L, "setDarkStyle", 0);

	GidImGui* imgui = getImgui(L);
	ImGui::StyleColorsDark();
	return 0;
}

int StyleLight(lua_State* L)
{
	STACK_CHECKER(L, "setLightStyle", 0);

	GidImGui* imgui = getImgui(L);
	ImGui::StyleColorsLight();
	return 0;
}

int StyleClassic(lua_State* L)
{
	STACK_CHECKER(L, "setClassicStyle", 0);

	GidImGui* imgui = getImgui(L);
	ImGui::StyleColorsClassic();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Color Utilities
///
/////////////////////////////////////////////////////////////////////////////////////////////

int ColorConvertHEXtoRGB(lua_State* L)
{
	STACK_CHECKER(L, "colorConvertHEXtoRGB", 4);

	int hex = luaL_checkinteger(L, 1);
	float alpha = luaL_optnumber(L, 2, 1.0f);

	ImVec4 vec = GColor::toVec4(GColor(hex, alpha));

	lua_pushnumber(L, vec.x);
	lua_pushnumber(L, vec.y);
	lua_pushnumber(L, vec.z);
	lua_pushnumber(L, vec.w);

	return 4;
}

int ColorConvertRGBtoHEX(lua_State* L)
{
	STACK_CHECKER(L, "colorConvertRGBtoHEX", 1);

	float r = luaL_checknumber(L, 1);
	float g = luaL_checknumber(L, 2);
	float b = luaL_checknumber(L, 3);

	GColor color = GColor::toHex(r, g, b, 1.0f);

	lua_pushinteger(L, color.hex);
	return 1;
}

int ColorConvertRGBtoHSV(lua_State* L)
{
	STACK_CHECKER(L, "colorConvertRGBtoHSV", 3);

	float r = luaL_checknumber(L, 1);
	float g = luaL_checknumber(L, 2);
	float b = luaL_checknumber(L, 3);

	float h = 0;
	float s = 0;
	float v = 0;

	ImGui::ColorConvertRGBtoHSV(r, g, b, h, s, v);

	lua_pushnumber(L, h);
	lua_pushnumber(L, s);
	lua_pushnumber(L, v);

	return 3;
}

int ColorConvertHSVtoRGB(lua_State* L)
{
	STACK_CHECKER(L, "colorConvertHSVtoRGB", 3);

	float h = luaL_checknumber(L, 1);
	float s = luaL_checknumber(L, 2);
	float v = luaL_checknumber(L, 3);

	float r = 0;
	float g = 0;
	float b = 0;

	ImGui::ColorConvertHSVtoRGB(h, s, v, r, g, b);

	lua_pushnumber(L, r);
	lua_pushnumber(L, g);
	lua_pushnumber(L, b);

	return 3;
}

int ColorConvertHEXtoHSV(lua_State* L)
{
	STACK_CHECKER(L, "colorConvertHEXtoHSV", 3);

	int hex = luaL_checkinteger(L, 1);
	float alpha = luaL_optnumber(L, 2, 1.0f);

	float h = 0;
	float s = 0;
	float v = 0;

	ImVec4 in = GColor::toVec4(hex, alpha);

	ImGui::ColorConvertRGBtoHSV(in.x, in.y, in.z, h, s, v);

	lua_pushnumber(L, h);
	lua_pushnumber(L, s);
	lua_pushnumber(L, v);

	return 3;
}

int ColorConvertHSVtoHEX(lua_State* L)
{
	STACK_CHECKER(L, "colorConvertHSVtoHEX", 1);

	float h = luaL_checknumber(L, 1);
	float s = luaL_checknumber(L, 2);
	float v = luaL_checknumber(L, 3);

	float r = 0;
	float g = 0;
	float b = 0;

	ImGui::ColorConvertHSVtoRGB(h, s, v, r, g, b);

	GColor out(r, g, b);

	lua_pushinteger(L, out.hex);

	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// DEMOS
///
/////////////////////////////////////////////////////////////////////////////////////////////

int ShowUserGuide(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "showUserGuide", 0);

	ImGui::ShowUserGuide();
	return 0;
}

int ShowDemoWindow(lua_State* L)
{

	if (lua_isnoneornil(L, 2))
	{
		STACK_CHECKER(L, "showDemoWindow", 0);
		ImGui::ShowDemoWindow();
		return 0;
	}
	STACK_CHECKER(L, "showDemoWindow", 1);
	bool p_open = lua_toboolean(L, 2);
	ImGui::ShowDemoWindow(&p_open);
	lua_pushboolean(L, p_open);
	return 1;
}

int ShowAboutWindow(lua_State* L)
{

	if (lua_isnoneornil(L, 2))
	{
		STACK_CHECKER(L, "showAboutWindow", 0);
		ImGui::ShowAboutWindow();
		return 0;
	}
	STACK_CHECKER(L, "showAboutWindow", 1);
	bool p_open = lua_toboolean(L, 2);
	ImGui::ShowAboutWindow(&p_open);
	lua_pushboolean(L, p_open);
	return 1;
}

int ShowStyleEditor(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "showStyleEditor", 0);

	ImGui::ShowStyleEditor();
	return 0;
}

int ShowFontSelector(lua_State* L)
{
	STACK_CHECKER(L, "showFontSelector", 0);

	const char* label = luaL_checkstring(L, 2);
	ImGui::ShowFontSelector(label);
	return 0;
}

int ShowMetricsWindow(lua_State* L)
{

	if (lua_isnoneornil(L, 2))
	{
		STACK_CHECKER(L, "showMetricsWindow", 0);
		ImGui::ShowMetricsWindow();
		return 0;
	}
	STACK_CHECKER(L, "showMetricsWindow", 1);
	bool p_open = lua_toboolean(L, 2);
	ImGui::ShowMetricsWindow(&p_open);
	lua_pushboolean(L, p_open);
	return 1;
}

int ShowStyleSelector(lua_State* L)
{
	STACK_CHECKER(L, "showStyleSelector", 1);

	const char* label = luaL_checkstring(L, 2);
	bool open = ImGui::ShowStyleSelector(label);
	lua_pushboolean(L, open);
	return 1;
}

int ShowStackToolWindow(lua_State* L)
{
	if (lua_isnoneornil(L, 2))
	{
		STACK_CHECKER(L, "showMetricsWindow", 0);
		ImGui::ShowStackToolWindow();
		return 0;
	}
	STACK_CHECKER(L, "showMetricsWindow", 1);
	bool p_open = lua_toboolean(L, 2);
	ImGui::ShowStackToolWindow(&p_open);
	lua_pushboolean(L, p_open);
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Style class
///
/////////////////////////////////////////////////////////////////////////////////////////////

int GetStyle(lua_State* L)
{
	STACK_CHECKER(L, "getStyle", 1);

	GidImGui* imgui = getImgui(L);
	ImGuiStyle* style = &ImGui::GetStyle();
	g_pushInstance(L, "ImGuiStyle", style);
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// AUTO GENERATED STYLE METHODS ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

int Style_old_SetColor(lua_State* L)
{
	STACK_CHECKER(L, "setStyleColor", 0);

	int idx = luaL_checkinteger(L, 2);
	LUA_ASSERT(idx >= 0 && idx <= ImGuiCol_COUNT, "Color index is out of bounds.");

	GidImGui* imgui = getImgui(L);
	ImGuiStyle &style = ImGui::GetStyle();
	style.Colors[idx] = GColor::toVec4(L, 3);
	return 0;
}

int Style_SetColor(lua_State* L)
{
	STACK_CHECKER(L, "setColor", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	int idx = luaL_checkinteger(L, 2);
	LUA_ASSERT(idx >= 0 && idx <= ImGuiCol_COUNT, "Color index is out of bounds.");
	style.Colors[idx] = GColor::toVec4(L, 3);
	return 0;
}

int Style_GetColor(lua_State* L)
{
	STACK_CHECKER(L, "getColor", 2);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	int idx = luaL_checkinteger(L, 2);
	LUA_ASSERT(idx >= 0 && idx <= ImGuiCol_COUNT, "Color index is out of bounds.");
	GColor color = GColor::toHex(style.Colors[idx]);
	lua_pushinteger(L, color.hex);
	lua_pushnumber(L, color.alpha);
	return 2;
}

int Style_SetAlpha(lua_State* L)
{
	STACK_CHECKER(L, "setAlpha", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.Alpha = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetAlpha(lua_State* L)
{
	STACK_CHECKER(L, "getAlpha", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.Alpha);
	return 1;
}

int Style_SetWindowRounding(lua_State* L)
{
	STACK_CHECKER(L, "setWindowRounding", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.WindowRounding = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetWindowRounding(lua_State* L)
{
	STACK_CHECKER(L, "getWindowRounding", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.WindowRounding);
	return 1;
}

int Style_SetWindowBorderSize(lua_State* L)
{
	STACK_CHECKER(L, "setWindowBorderSize", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.WindowBorderSize = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetWindowBorderSize(lua_State* L)
{
	STACK_CHECKER(L, "getWindowBorderSize", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.WindowBorderSize);
	return 1;
}

int Style_SetChildRounding(lua_State* L)
{
	STACK_CHECKER(L, "setChildRounding", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.ChildRounding = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetChildRounding(lua_State* L)
{
	STACK_CHECKER(L, "getChildRounding", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.ChildRounding);
	return 1;
}

int Style_SetChildBorderSize(lua_State* L)
{
	STACK_CHECKER(L, "setChildBorderSize", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.ChildBorderSize = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetChildBorderSize(lua_State* L)
{
	STACK_CHECKER(L, "getChildBorderSize", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.ChildBorderSize);
	return 1;
}

int Style_SetPopupRounding(lua_State* L)
{
	STACK_CHECKER(L, "setPopupRounding", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.PopupRounding = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetPopupRounding(lua_State* L)
{
	STACK_CHECKER(L, "getPopupRounding", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.PopupRounding);
	return 1;
}

int Style_SetPopupBorderSize(lua_State* L)
{
	STACK_CHECKER(L, "setPopupBorderSize", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.PopupBorderSize = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetPopupBorderSize(lua_State* L)
{
	STACK_CHECKER(L, "getPopupBorderSize", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.PopupBorderSize);
	return 1;
}

int Style_SetFrameRounding(lua_State* L)
{
	STACK_CHECKER(L, "setFrameRounding", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.FrameRounding = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetFrameRounding(lua_State* L)
{
	STACK_CHECKER(L, "getFrameRounding", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.FrameRounding);
	return 1;
}

int Style_SetFrameBorderSize(lua_State* L)
{
	STACK_CHECKER(L, "setFrameBorderSize", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.FrameBorderSize = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetFrameBorderSize(lua_State* L)
{
	STACK_CHECKER(L, "getFrameBorderSize", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.FrameBorderSize);
	return 1;
}

int Style_SetIndentSpacing(lua_State* L)
{
	STACK_CHECKER(L, "setIndentSpacing", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.IndentSpacing = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetIndentSpacing(lua_State* L)
{
	STACK_CHECKER(L, "getIndentSpacing", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.IndentSpacing);
	return 1;
}

int Style_SetColumnsMinSpacing(lua_State* L)
{
	STACK_CHECKER(L, "setColumnsMinSpacing", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.ColumnsMinSpacing = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetColumnsMinSpacing(lua_State* L)
{
	STACK_CHECKER(L, "getColumnsMinSpacing", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.ColumnsMinSpacing);
	return 1;
}

int Style_SetScrollbarSize(lua_State* L)
{
	STACK_CHECKER(L, "setScrollbarSize", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.ScrollbarSize = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetScrollbarSize(lua_State* L)
{
	STACK_CHECKER(L, "getScrollbarSize", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.ScrollbarSize);
	return 1;
}

int Style_SetScrollbarRounding(lua_State* L)
{
	STACK_CHECKER(L, "setScrollbarRounding", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.ScrollbarRounding = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetScrollbarRounding(lua_State* L)
{
	STACK_CHECKER(L, "getScrollbarRounding", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.ScrollbarRounding);
	return 1;
}

int Style_SetGrabMinSize(lua_State* L)
{
	STACK_CHECKER(L, "setGrabMinSize", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.GrabMinSize = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetGrabMinSize(lua_State* L)
{
	STACK_CHECKER(L, "getGrabMinSize", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.GrabMinSize);
	return 1;
}

int Style_SetGrabRounding(lua_State* L)
{
	STACK_CHECKER(L, "setGrabRounding", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.GrabRounding = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetGrabRounding(lua_State* L)
{
	STACK_CHECKER(L, "getGrabRounding", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.GrabRounding);
	return 1;
}

int Style_SetLogSliderDeadzone(lua_State* L)
{
	STACK_CHECKER(L, "setLogSliderDeadzone", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.LogSliderDeadzone = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetLogSliderDeadzone(lua_State* L)
{
	STACK_CHECKER(L, "getLogSliderDeadzone", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.LogSliderDeadzone);
	return 1;
}

int Style_SetTabRounding(lua_State* L)
{
	STACK_CHECKER(L, "setTabRounding", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.TabRounding = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetTabRounding(lua_State* L)
{
	STACK_CHECKER(L, "getTabRounding", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.TabRounding);
	return 1;
}

int Style_SetTabBorderSize(lua_State* L)
{
	STACK_CHECKER(L, "setTabBorderSize", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.TabBorderSize = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetTabBorderSize(lua_State* L)
{
	STACK_CHECKER(L, "getTabBorderSize", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.TabBorderSize);
	return 1;
}

int Style_SetTabMinWidthForCloseButton(lua_State* L)
{
	STACK_CHECKER(L, "setTabMinWidthForCloseButton", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.TabMinWidthForCloseButton = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetTabMinWidthForCloseButton(lua_State* L)
{
	STACK_CHECKER(L, "getTabMinWidthForCloseButton", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.TabMinWidthForCloseButton);
	return 1;
}

int Style_SetMouseCursorScale(lua_State* L)
{
	STACK_CHECKER(L, "setMouseCursorScale", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.MouseCursorScale = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetMouseCursorScale(lua_State* L)
{
	STACK_CHECKER(L, "getMouseCursorScale", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.MouseCursorScale);
	return 1;
}

int Style_SetCurveTessellationTol(lua_State* L)
{
	STACK_CHECKER(L, "setCurveTessellationTol", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.CurveTessellationTol = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetCurveTessellationTol(lua_State* L)
{
	STACK_CHECKER(L, "getCurveTessellationTol", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.CurveTessellationTol);
	return 1;
}

int Style_SetCircleTessellationMaxError(lua_State* L)
{
	STACK_CHECKER(L, "setCircleTessellationMaxError", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.CircleTessellationMaxError = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetCircleTessellationMaxError(lua_State* L)
{
	STACK_CHECKER(L, "getCircleTessellationMaxError", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.CircleTessellationMaxError);
	return 1;
}

int Style_SetWindowPadding(lua_State* L)
{
	STACK_CHECKER(L, "setWindowPadding", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.WindowPadding = luaL_checkvec2(L, 2);
	return 0;
}

int Style_GetWindowPadding(lua_State* L)
{
	STACK_CHECKER(L, "getWindowPadding", 2);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.WindowPadding.x);
	lua_pushnumber(L, style.WindowPadding.y);
	return 2;
}

int Style_SetWindowMinSize(lua_State* L)
{
	STACK_CHECKER(L, "setWindowMinSize", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.WindowMinSize = luaL_checkvec2(L, 2);
	return 0;
}

int Style_GetWindowMinSize(lua_State* L)
{
	STACK_CHECKER(L, "getWindowMinSize", 2);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.WindowMinSize.x);
	lua_pushnumber(L, style.WindowMinSize.y);
	return 2;
}

int Style_SetWindowTitleAlign(lua_State* L)
{
	STACK_CHECKER(L, "setWindowTitleAlign", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.WindowTitleAlign = luaL_checkvec2(L, 2);
	return 0;
}

int Style_GetWindowTitleAlign(lua_State* L)
{
	STACK_CHECKER(L, "getWindowTitleAlign", 2);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.WindowTitleAlign.x);
	lua_pushnumber(L, style.WindowTitleAlign.y);
	return 2;
}

int Style_SetFramePadding(lua_State* L)
{
	STACK_CHECKER(L, "setFramePadding", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.FramePadding = luaL_checkvec2(L, 2);
	return 0;
}

int Style_GetFramePadding(lua_State* L)
{
	STACK_CHECKER(L, "getFramePadding", 2);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.FramePadding.x);
	lua_pushnumber(L, style.FramePadding.y);
	return 2;
}

int Style_SetCellPadding(lua_State* L)
{
	STACK_CHECKER(L, "setCellPadding", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.CellPadding = luaL_checkvec2(L, 2);
	return 0;
}

int Style_GetCellPadding(lua_State* L)
{
	STACK_CHECKER(L, "getCellPadding", 2);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.CellPadding.x);
	lua_pushnumber(L, style.CellPadding.y);
	return 2;
}

int Style_SetDisabledAlpha(lua_State* L)
{
	STACK_CHECKER(L, "setDisabledAlpha", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.DisabledAlpha = luaL_checknumber(L, 2);
	return 0;
}

int Style_GetDisabledAlpha(lua_State* L)
{
	STACK_CHECKER(L, "getDisabledAlpha", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.DisabledAlpha);
	return 1;
}

int Style_SetItemSpacing(lua_State* L)
{
	STACK_CHECKER(L, "setItemSpacing", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.ItemSpacing = luaL_checkvec2(L, 2);
	return 0;
}

int Style_GetItemSpacing(lua_State* L)
{
	STACK_CHECKER(L, "getItemSpacing", 2);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.ItemSpacing.x);
	lua_pushnumber(L, style.ItemSpacing.y);
	return 2;
}

int Style_SetItemInnerSpacing(lua_State* L)
{
	STACK_CHECKER(L, "setItemInnerSpacing", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.ItemInnerSpacing = luaL_checkvec2(L, 2);
	return 0;
}

int Style_GetItemInnerSpacing(lua_State* L)
{
	STACK_CHECKER(L, "getItemInnerSpacing", 2);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.ItemInnerSpacing.x);
	lua_pushnumber(L, style.ItemInnerSpacing.y);
	return 2;
}

int Style_SetTouchExtraPadding(lua_State* L)
{
	STACK_CHECKER(L, "setTouchExtraPadding", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.TouchExtraPadding = luaL_checkvec2(L, 2);
	return 0;
}

int Style_GetTouchExtraPadding(lua_State* L)
{
	STACK_CHECKER(L, "getTouchExtraPadding", 2);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.TouchExtraPadding.x);
	lua_pushnumber(L, style.TouchExtraPadding.y);
	return 2;
}

int Style_SetButtonTextAlign(lua_State* L)
{
	STACK_CHECKER(L, "setButtonTextAlign", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.ButtonTextAlign = luaL_checkvec2(L, 2);
	return 0;
}

int Style_GetButtonTextAlign(lua_State* L)
{
	STACK_CHECKER(L, "getButtonTextAlign", 2);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.ButtonTextAlign.x);
	lua_pushnumber(L, style.ButtonTextAlign.y);
	return 2;
}

int Style_SetSelectableTextAlign(lua_State* L)
{
	STACK_CHECKER(L, "setSelectableTextAlign", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.SelectableTextAlign = luaL_checkvec2(L, 2);
	return 0;
}

int Style_GetSelectableTextAlign(lua_State* L)
{
	STACK_CHECKER(L, "getSelectableTextAlign", 2);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.SelectableTextAlign.x);
	lua_pushnumber(L, style.SelectableTextAlign.y);
	return 2;
}

int Style_SetDisplayWindowPadding(lua_State* L)
{
	STACK_CHECKER(L, "setDisplayWindowPadding", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.DisplayWindowPadding = luaL_checkvec2(L, 2);
	return 0;
}

int Style_GetDisplayWindowPadding(lua_State* L)
{
	STACK_CHECKER(L, "getDisplayWindowPadding", 2);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.DisplayWindowPadding.x);
	lua_pushnumber(L, style.DisplayWindowPadding.y);
	return 2;
}

int Style_SetDisplaySafeAreaPadding(lua_State* L)
{
	STACK_CHECKER(L, "setDisplaySafeAreaPadding", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.DisplaySafeAreaPadding = luaL_checkvec2(L, 2);
	return 0;
}

int Style_GetDisplaySafeAreaPadding(lua_State* L)
{
	STACK_CHECKER(L, "getDisplaySafeAreaPadding", 2);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushnumber(L, style.DisplaySafeAreaPadding.x);
	lua_pushnumber(L, style.DisplaySafeAreaPadding.y);
	return 2;
}

int Style_SetWindowMenuButtonPosition(lua_State* L)
{
	STACK_CHECKER(L, "setWindowMenuButtonPosition", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.WindowMenuButtonPosition = luaL_checkinteger(L, 2);
	return 0;
}

int Style_GetWindowMenuButtonPosition(lua_State* L)
{
	STACK_CHECKER(L, "getWindowMenuButtonPosition", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushinteger(L, style.WindowMenuButtonPosition);
	return 1;
}

int Style_SetColorButtonPosition(lua_State* L)
{
	STACK_CHECKER(L, "setColorButtonPosition", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.ColorButtonPosition = luaL_checkinteger(L, 2);
	return 0;
}

int Style_GetColorButtonPosition(lua_State* L)
{
	STACK_CHECKER(L, "getColorButtonPosition", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushinteger(L, style.ColorButtonPosition);
	return 1;
}

int Style_SetAntiAliasedLines(lua_State* L)
{
	STACK_CHECKER(L, "setAntiAliasedLines", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.AntiAliasedLines = lua_toboolean(L, 2) > 0;
	return 0;
}

int Style_GetAntiAliasedLines(lua_State* L)
{
	STACK_CHECKER(L, "getAntiAliasedLines", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushboolean(L, style.AntiAliasedLines);
	return 1;
}

int Style_SetAntiAliasedLinesUseTex(lua_State* L)
{
	STACK_CHECKER(L, "setAntiAliasedLinesUseTex", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.AntiAliasedLinesUseTex = lua_toboolean(L, 2) > 0;
	return 0;
}

int Style_GetAntiAliasedLinesUseTex(lua_State* L)
{
	STACK_CHECKER(L, "getAntiAliasedLinesUseTex", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushboolean(L, style.AntiAliasedLinesUseTex);
	return 1;
}

int Style_SetAntiAliasedFill(lua_State* L)
{
	STACK_CHECKER(L, "setAntiAliasedFill", 0);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	style.AntiAliasedFill = lua_toboolean(L, 2) > 0;
	return 0;
}

int Style_GetAntiAliasedFill(lua_State* L)
{
	STACK_CHECKER(L, "getAntiAliasedFill", 1);

	ImGuiStyle &style = *getPtr<ImGuiStyle>(L, "ImGuiStyle");
	lua_pushboolean(L, style.AntiAliasedFill);
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

int GetIO(lua_State* L)
{
	STACK_CHECKER(L, "getIO", 1);

	GidImGui* imgui = getImgui(L);
	ImGuiIO* io = &ImGui::GetIO();
	g_pushInstance(L, "ImGuiIO", io);
	return 1;
}

#ifdef IS_DOCKING_BUILD
int IO_GetConfigDockingNoSplit(lua_State* L)
{
	STACK_CHECKER(L, "setConfigDockingNoSplit", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushboolean(L, io.ConfigDockingNoSplit);
	return 0;
}

int IO_SetConfigDockingNoSplit(lua_State* L)
{
	STACK_CHECKER(L, "setConfigDockingNoSplit", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.ConfigDockingNoSplit = lua_toboolean(L, 2) > 0;
	return 0;
}

int IO_GetConfigDockingWithShift(lua_State* L)
{
	STACK_CHECKER(L, "setConfigDockingWithShift", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushboolean(L, io.ConfigDockingWithShift);
	return 0;
}

int IO_SetConfigDockingWithShift(lua_State* L)
{
	STACK_CHECKER(L, "setConfigDockingWithShift", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.ConfigDockingWithShift = lua_toboolean(L, 2) > 0;
	return 0;
}

int IO_GetConfigDockingAlwaysTabBar(lua_State* L)
{
	STACK_CHECKER(L, "setConfigDockingAlwaysTabBar", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushboolean(L, io.ConfigDockingAlwaysTabBar);
	return 0;
}

int IO_SetConfigDockingAlwaysTabBar(lua_State* L)
{
	STACK_CHECKER(L, "setConfigDockingAlwaysTabBar", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.ConfigDockingAlwaysTabBar = lua_toboolean(L, 2) > 0;
	return 0;
}

int IO_GetConfigDockingTransparentPayload(lua_State* L)
{
	STACK_CHECKER(L, "setConfigDockingTransparentPayload", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushboolean(L, io.ConfigDockingTransparentPayload);
	return 0;
}

int IO_SetConfigDockingTransparentPayload(lua_State* L)
{
	STACK_CHECKER(L, "setConfigDockingTransparentPayload", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.ConfigDockingTransparentPayload = lua_toboolean(L, 2) > 0;
	return 0;
}
#endif

int IO_SetFontDefault(lua_State* L)
{
	STACK_CHECKER(L, "setFontDefault", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	ImFont* font = getPtr<ImFont>(L, "ImFont", 2);
	if (font)
		io.FontDefault = font;
	return 0;
}

int IO_GetDefaultFont(lua_State* L)
{
	STACK_CHECKER(L, "getDefaultFont", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	g_pushInstance(L, "ImFont", io.FontDefault);
	return 1;
}

int IO_GetFonts(lua_State* L)
{
	STACK_CHECKER(L, "getFonts", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");

	g_pushInstance(L, "ImFontAtlas", io.Fonts);
	return 1;
}

int IO_GetDeltaTime(lua_State* L)
{
	STACK_CHECKER(L, "getDeltaTime", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushnumber(L, io.DeltaTime);
	return 1;
}

int IO_GetMouseWheel(lua_State* L)
{

	STACK_CHECKER(L, "getMouseWheel", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushnumber(L, io.MouseWheel);
	return 1;
}

int IO_GetMouseWheelH(lua_State* L)
{
	STACK_CHECKER(L, "getMouseWheelH", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushnumber(L, io.MouseWheel);
	return 1;
}

int IO_isMouseDown(lua_State* L)
{
	STACK_CHECKER(L, "isMouseDown", 1);

	ImGuiKey button = giderosMouseToImGui(luaL_checkinteger(L, 2));
	lua_pushboolean(L, ImGui::IsMouseDown(button));
	return  1;
}

int IO_WantCaptureMouse(lua_State* L)
{
	STACK_CHECKER(L, "wantCaptureMouse", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");

	lua_pushboolean(L, io.WantCaptureMouse);
	return 1;
}

int IO_WantCaptureMouseUnlessPopupClose(lua_State* L)
{
	STACK_CHECKER(L, "wantCaptureMouse", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");

	lua_pushboolean(L, io.WantCaptureMouseUnlessPopupClose);
	return 1;
}

int IO_WantCaptureKeyboard(lua_State* L)
{
	STACK_CHECKER(L, "wantCaptureKeyboard", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");

	lua_pushboolean(L, io.WantCaptureKeyboard);
	return 1;
}

int IO_WantTextInput(lua_State* L)
{
	STACK_CHECKER(L, "wantTextInput", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");

	lua_pushboolean(L, io.WantTextInput);
	return 1;
}

int IO_WantSetMousePos(lua_State* L)
{
	STACK_CHECKER(L, "wantSetMousePos", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");

	lua_pushboolean(L, io.WantSetMousePos);
	return 1;
}

int IO_WantSaveIniSettings(lua_State* L)
{
	STACK_CHECKER(L, "wantSaveIniSettings", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");

	lua_pushboolean(L, io.WantSaveIniSettings);
	return 1;
}

int IO_GetFramerate(lua_State* L)
{
	STACK_CHECKER(L, "getFramerate", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");

	lua_pushnumber(L, io.Framerate);
	return 1;
}

int IO_GetMetricsRenderVertices(lua_State* L)
{
	STACK_CHECKER(L, "getMetricsRenderVertices", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");

	lua_pushinteger(L, io.MetricsRenderVertices);
	return 1;
}

int IO_GetMetricsRenderIndices(lua_State* L)
{
	STACK_CHECKER(L, "getMetricsRenderIndices", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");

	lua_pushinteger(L, io.MetricsRenderIndices);
	return 1;
}

int IO_GetMetricsRenderWindows(lua_State* L)
{
	STACK_CHECKER(L, "getMetricsRenderWindows", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");

	lua_pushinteger(L, io.MetricsRenderWindows);
	return 1;
}

int IO_GetMetricsActiveWindows(lua_State* L)
{
	STACK_CHECKER(L, "getMetricsActiveWindows", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");

	lua_pushinteger(L, io.MetricsActiveWindows);
	return 1;
}

int IO_GetMetricsActiveAllocations(lua_State* L)
{
	STACK_CHECKER(L, "getMetricsActiveAllocations", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");

	lua_pushinteger(L, io.MetricsActiveAllocations);
	return 1;
}

int IO_GetMouseDelta(lua_State* L)
{
	STACK_CHECKER(L, "getMouseDelta", 2);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushnumber(L, io.MouseDelta.x);
	lua_pushnumber(L, io.MouseDelta.y);
	return 2;
}

int IO_GetMouseDownSec(lua_State* L)
{
	STACK_CHECKER(L, "getMouseDownSec", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	int button = giderosMouseToImGui(lua_tointeger(L, 2));

	lua_pushnumber(L, io.MouseDownDuration[button]);
	return 1;
}

int IO_SetDisplaySize(lua_State* L)
{
	STACK_CHECKER(L, "setDisplaySize", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.DisplaySize.x = luaL_checknumber(L, 2);
	io.DisplaySize.y = luaL_checknumber(L, 3);

	return 0;
}

int IO_GetDisplaySize(lua_State* L)
{
	STACK_CHECKER(L, "getDisplaySize", 2);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushnumber(L, io.DisplaySize.x);
	lua_pushnumber(L, io.DisplaySize.y);

	return 2;
}


int IO_GetConfigFlags(lua_State* L)
{
	STACK_CHECKER(L, "getConfigFlags", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushinteger(L, io.ConfigFlags);
	return 1;
}

int IO_SetConfigFlags(lua_State* L)
{
	STACK_CHECKER(L, "setConfigFlags", 0);

	ImGuiConfigFlags flags = luaL_checkinteger(L, 2);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.ConfigFlags = flags;
	return 0;
}

int IO_AddConfigFlags(lua_State* L)
{
	STACK_CHECKER(L, "addConfigFlags", 0);

	ImGuiConfigFlags flags = luaL_checkinteger(L, 2);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.ConfigFlags |= flags;
	return 0;
}

int IO_GetBackendFlags(lua_State* L)
{
	STACK_CHECKER(L, "getBackendFlags", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushinteger(L, io.BackendFlags);
	return 1;
}

int IO_SetBackendFlags(lua_State* L)
{
	STACK_CHECKER(L, "setBackendFlags", 0);

	ImGuiBackendFlags flags = luaL_checkinteger(L, 2);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.BackendFlags = flags;
	return 0;
}

int IO_AddBackendFlags(lua_State* L)
{
	STACK_CHECKER(L, "addBackendFlags", 0);

	ImGuiBackendFlags flags = luaL_checkinteger(L, 2);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.BackendFlags |= flags;
	return 0;
}

int IO_GetIniSavingRate(lua_State* L)
{
	STACK_CHECKER(L, "getIniSavingRate", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushnumber(L, io.IniSavingRate);
	return 1;
}

int IO_SetIniSavingRate(lua_State* L)
{
	STACK_CHECKER(L, "setIniSavingRate", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.IniSavingRate = luaL_optnumber(L, 2, 5.0f);
	return 0;
}

int IO_GetIniFilename(lua_State* L)
{
	STACK_CHECKER(L, "getIniFilename", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushstring(L, io.IniFilename);
	return 1;
}

int IO_SaveIniSettingsToDisk(lua_State* L)
{
	STACK_CHECKER(L, "saveIniSettings", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	if (lua_type(L, 2) == LUA_TSTRING)
	{
		ImGui::SaveIniSettingsToDisk(lua_tostring(L, 2));
		return 0;
	}
	ImGui::SaveIniSettingsToDisk(io.IniFilename);
	return 0;
}

int IO_LoadIniSettingsFromDisk(lua_State* L)
{
	STACK_CHECKER(L, "loadIniSettings", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	if (lua_type(L, 2) == LUA_TSTRING)
	{
		ImGui::LoadIniSettingsFromDisk(lua_tostring(L, 2));
		return 0;
	}
	ImGui::LoadIniSettingsFromDisk(io.IniFilename);
	return 0;
}

int IO_SetIniFilename(lua_State* L)
{
	STACK_CHECKER(L, "setIniFilename", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	if (lua_gettop(L) == 2 && lua_isnil(L, 2))
		io.IniFilename = NULL;
	else
		io.IniFilename = luaL_checkstring(L, 2);
	return 0;
}

int IO_GetLogFilename(lua_State* L)
{
	STACK_CHECKER(L, "getLogFilename", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushstring(L, io.LogFilename);
	return 1;
}

int IO_SetLogFilename(lua_State* L)
{
	STACK_CHECKER(L, "setLogFilename", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	if (lua_gettop(L) == 2 && lua_isnil(L, 2))
		io.LogFilename = NULL;
	else
		io.LogFilename = luaL_checkstring(L, 2);
	return 0;
}

int IO_GetMouseDoubleClickTime(lua_State* L)
{
	STACK_CHECKER(L, "getMouseDoubleClickTime", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushnumber(L, io.MouseDoubleClickTime);
	return 1;
}

int IO_SetMouseDoubleClickTime(lua_State* L)
{
	STACK_CHECKER(L, "setMouseDoubleClickTime", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.MouseDoubleClickTime = luaL_optnumber(L, 2, 0.30f);
	return 0;
}

int IO_GetMouseDragThreshold(lua_State* L)
{
	STACK_CHECKER(L, "getMouseDragThreshold", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushnumber(L, io.MouseDragThreshold);
	return 1;
}

int IO_SetMouseDragThreshold(lua_State* L)
{
	STACK_CHECKER(L, "setMouseDragThreshold", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.MouseDragThreshold = luaL_optnumber(L, 2, 6.0f);
	return 0;
}

int IO_GetMouseDrawCursor(lua_State* L)
{
	STACK_CHECKER(L, "getMouseDrawCursor", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushboolean(L, io.MouseDrawCursor);
	return 1;
}

int IO_SetMouseDrawCursor(lua_State* L)
{
	STACK_CHECKER(L, "setMouseDrawCursor", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.MouseDrawCursor = lua_toboolean(L, 2) > 0;
	return 0;
}

int IO_GetMouseDoubleClickMaxDist(lua_State* L)
{
	STACK_CHECKER(L, "getMouseDoubleClickMaxDist", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushnumber(L, io.MouseDoubleClickMaxDist);
	return 1;
}

int IO_SetMouseDoubleClickMaxDist(lua_State* L)
{
	STACK_CHECKER(L, "setMouseDoubleClickMaxDist", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.MouseDoubleClickMaxDist = luaL_optnumber(L, 2, 6.0f);
	return 0;
}

int IO_GetKeyRepeatDelay(lua_State* L)
{
	STACK_CHECKER(L, "getKeyRepeatDelay", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushnumber(L, io.KeyRepeatDelay);
	return 1;
}

int IO_SetKeyRepeatDelay(lua_State* L)
{
	STACK_CHECKER(L, "setKeyRepeatDelay", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.KeyRepeatDelay = luaL_optnumber(L, 2, 0.25f);
	return 0;
}

int IO_GetKeyRepeatRate(lua_State* L)
{
	STACK_CHECKER(L, "getKeyRepeatRate", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushnumber(L, io.KeyRepeatRate);
	return 1;
}

int IO_SetKeyRepeatRate(lua_State* L)
{
	STACK_CHECKER(L, "setKeyRepeatRate", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.KeyRepeatRate = luaL_optnumber(L, 2, 0.05f);
	return 0;
}

int IO_GetFontGlobalScale(lua_State* L)
{
	STACK_CHECKER(L, "getFontGlobalScale", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushnumber(L, io.FontGlobalScale);
	return 1;
}

int IO_SetFontGlobalScale(lua_State* L)
{
	STACK_CHECKER(L, "setFontGlobalScale", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.FontGlobalScale = luaL_optnumber(L, 2, 1.0f);
	return 0;
}

int IO_GetFontAllowUserScaling(lua_State* L)
{
	STACK_CHECKER(L, "getFontAllowUserScaling", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushboolean(L, io.FontAllowUserScaling);
	return 1;
}

int IO_SetFontAllowUserScaling(lua_State* L)
{
	STACK_CHECKER(L, "setFontAllowUserScaling", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.FontAllowUserScaling = lua_toboolean(L, 2) > 0;
	return 0;
}

int IO_GetDisplayFramebufferScale(lua_State* L)
{
	STACK_CHECKER(L, "getDisplayFramebufferScale", 2);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushnumber(L, io.DisplayFramebufferScale.x);
	lua_pushnumber(L, io.DisplayFramebufferScale.y);
	return 2;
}

int IO_SetDisplayFramebufferScale(lua_State* L)
{
	STACK_CHECKER(L, "setDisplayFramebufferScale", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	ImVec2 scale = luaL_checkvec2(L, 2);
	io.DisplayFramebufferScale = scale;
	return 0;
}

int IO_GetConfigMacOSXBehaviors(lua_State* L)
{
	STACK_CHECKER(L, "getConfigMacOSXBehaviors", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushboolean(L, io.ConfigMacOSXBehaviors);
	return 1;
}

int IO_SetConfigMacOSXBehaviors(lua_State* L)
{
	STACK_CHECKER(L, "setConfigMacOSXBehaviors", 0);

	bool flag = lua_toboolean(L, 2) > 0;

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.ConfigMacOSXBehaviors = flag;
	return 0;
}

int IO_GetConfigDragClickToInputText(lua_State* L)
{
	STACK_CHECKER(L, "getConfigDragClickToInputText", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushboolean(L, io.ConfigDragClickToInputText);
	return 1;
}

int IO_SetConfigDragClickToInputText(lua_State* L)
{
	STACK_CHECKER(L, "setConfigDragClickToInputText", 0);

	bool flag = lua_toboolean(L, 2) > 0;

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.ConfigDragClickToInputText = flag;
	return 0;
}

int IO_GetConfigInputTextCursorBlink(lua_State* L)
{
	STACK_CHECKER(L, "getConfigInputTextCursorBlink", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushboolean(L, io.ConfigInputTextCursorBlink);
	return 1;
}

int IO_SetConfigInputTextCursorBlink(lua_State* L)
{
	STACK_CHECKER(L, "setConfigInputTextCursorBlink", 0);

	bool flag = lua_toboolean(L, 2) > 0;

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.ConfigInputTextCursorBlink = flag;
	return 0;
}

int IO_GetConfigWindowsResizeFromEdges(lua_State* L)
{
	STACK_CHECKER(L, "getConfigWindowsResizeFromEdges", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushboolean(L, io.ConfigInputTextCursorBlink);
	return 1;
}

int IO_SetConfigWindowsResizeFromEdges(lua_State* L)
{
	STACK_CHECKER(L, "setConfigWindowsResizeFromEdges", 0);

	bool flag = lua_toboolean(L, 2) > 0;

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.ConfigWindowsResizeFromEdges = flag;
	return 0;
}

int IO_GetConfigWindowsMoveFromTitleBarOnly(lua_State* L)
{
	STACK_CHECKER(L, "getConfigWindowsMoveFromTitleBarOnly", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushboolean(L, io.ConfigWindowsMoveFromTitleBarOnly);
	return 1;
}

int IO_SetConfigWindowsMoveFromTitleBarOnly(lua_State* L)
{
	STACK_CHECKER(L, "setConfigWindowsMoveFromTitleBarOnly", 0);

	bool flag = lua_toboolean(L, 2) > 0;

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.ConfigWindowsMoveFromTitleBarOnly = flag;
	return 0;
}

int IO_GetConfigMemoryCompactTimer(lua_State* L)
{
	STACK_CHECKER(L, "getConfigWindowsMemoryCompactTimer", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushnumber(L, io.ConfigMemoryCompactTimer);
	return 1;
}

int IO_SetConfigMemoryCompactTimer(lua_State* L)
{
	STACK_CHECKER(L, "setConfigWindowsMemoryCompactTimer", 0);

	double t = luaL_optnumber(L, 2, -1.0f);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.ConfigMemoryCompactTimer = t;
	return 0;
}

int IO_GetBackendPlatformName(lua_State* L)
{
	STACK_CHECKER(L, "getBackendPlatformName", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushstring(L, io.BackendPlatformName);
	return 1;
}

int IO_GetBackendRendererName(lua_State* L)
{
	STACK_CHECKER(L, "getBackendRendererName", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushstring(L, io.BackendRendererName);
	return 1;
}

int IO_SetMouseDown(lua_State* L)
{
	STACK_CHECKER(L, "setMouseDown", 0);

	ImGuiMouseButton mouseButton = giderosMouseToImGui(luaL_checkinteger(L, 2));
	bool state = lua_toboolean(L, 3);
	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.AddMouseButtonEvent(mouseButton, state);
	return 0;
}

int IO_ResetMouseDown(lua_State* L)
{
	STACK_CHECKER(L, "resetMouseDown", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	for (int i = 0; i < ImGuiMouseButton_COUNT; ++i) {
		io.AddMouseButtonEvent(i, false);
	}
	return 0;
}

int IO_SetMousePos(lua_State* L)
{
	STACK_CHECKER(L, "setMousePos", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	io.AddMousePosEvent(x, y);
	return 0;
}

int IO_SetMouseWheel(lua_State* L)
{
	STACK_CHECKER(L, "setMouseWheel", 0);

	float wheel = luaL_checknumber(L, 2);
	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.AddMouseWheelEvent(0.0f, wheel);
	return 0;
}

int IO_AddInputCharactersUTF8(lua_State* L)
{
	STACK_CHECKER(L, "addInputCharactersUTF8", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	const char* text = luaL_checkstring(L, 2);
	io.AddInputCharactersUTF8(text);
	return 0;
}

int IO_SetKeysDown(lua_State* L)
{
	STACK_CHECKER(L, "setKeysDown", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	int giderosKey = luaL_checkinteger(L, 2);
	bool down = lua_toboolean(L, 3);
	ImGuiKey imguiKey = giderosKeyToImGuiKey(giderosKey);
	io.AddKeyEvent(imguiKey, down);
	return 0;
}

int IO_SetModKeyDown(lua_State* L)
{
	STACK_CHECKER(L, "setModKeysDown", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	const int mod = luaL_checkinteger(L, 2);
	bool state = lua_toboolean(L, 3);
	ImGuiKey imguiKey = giderosModKeyToImGuiMod(mod);
	io.AddKeyEvent(imguiKey, state);
	return 0;
}

int IO_ResetKeysDown(lua_State* L)
{
	STACK_CHECKER(L, "resetKeysDown", 0);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.ClearInputKeys();
	return 0;
}

int IO_GetPenPressure(lua_State* L)
{
	STACK_CHECKER(L, "getPenPressure", 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	lua_pushnumber(L, io.PenPressure);
	return 1;
}

int IO_SetAppAcceptingEvents(lua_State* L)
{
	STACK_CHECKER(L, "SetAppAcceptingEvents", 0);
	bool accepting_events = luaL_optboolean(L, 2, 1);

	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	io.SetAppAcceptingEvents(accepting_events);
	return 0;
}

int SetMousePos(lua_State* L)
{
	STACK_CHECKER(L, "setMousePos", 0);

	GidImGui* imgui = getImgui(L);
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);

	ImVec2 mpos = imgui->eventListener->translateMousePos(imgui->proxy, x, y);
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent(mpos.x, mpos.y);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// 1.87 new input functions
///
/////////////////////////////////////////////////////////////////////////////////////////////

int IO_AddKeyEvent(lua_State* L)
{
	STACK_CHECKER(L, "addKeyEvent", 0);
	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	int key = luaL_checkinteger(L, 2);
	bool down = lua_toboolean(L, 3);
	io.AddKeyEvent(key, down);
	return 0;
}

int IO_AddKeyAnalogEvent(lua_State* L)
{
	STACK_CHECKER(L, "addKeyAnalogEvent", 0);
	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	int key = luaL_checkinteger(L, 2);
	bool down = lua_toboolean(L, 3);
	float v = luaL_checknumber(L, 4);
	io.AddKeyAnalogEvent(key, down, v);
	return 0;
}

int IO_AddMousePosEvent(lua_State* L)
{
	STACK_CHECKER(L, "addMousePosEvent", 0);
	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	io.AddMousePosEvent(x, y);
	return 0;
}

int IO_AddMouseButtonEvent(lua_State* L)
{
	STACK_CHECKER(L, "addMouseButtonEvent", 0);
	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	int button = luaL_checkinteger(L, 2);
	bool down = lua_toboolean(L, 3);
	io.AddMouseButtonEvent(button, down);
	return 0;
}

int IO_AddMouseWheelEvent(lua_State* L)
{
	STACK_CHECKER(L, "addMouseWheelEvent", 0);
	ImGuiIO& io = *getPtr<ImGuiIO>(L, "ImGuiIO");
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	io.AddMouseWheelEvent(x, y);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// FONTS API
///
/////////////////////////////////////////////////////////////////////////////////////////////

const ImWchar* getRanges(ImFontAtlas* atlas, const int ranges)
{
	switch(ranges)
	{
	case ImGuiGlyphRanges_Korean:
		return atlas->GetGlyphRangesKorean();
	case ImGuiGlyphRanges_ChineseFull:
		return atlas->GetGlyphRangesChineseFull();
	case ImGuiGlyphRanges_ChineseSimplifiedCommon:
		return atlas->GetGlyphRangesChineseSimplifiedCommon();
	case ImGuiGlyphRanges_Japanese:
		return atlas->GetGlyphRangesJapanese();
	case ImGuiGlyphRanges_Cyrillic:
		return atlas->GetGlyphRangesCyrillic();
	case ImGuiGlyphRanges_Thai:
		return atlas->GetGlyphRangesThai();
	case ImGuiGlyphRanges_Vietnamese:
		return atlas->GetGlyphRangesVietnamese();
	default:
		return atlas->GetGlyphRangesDefault();
	}
}

void loadCharsConf(lua_State* L, ImFontGlyphRangesBuilder &builder)
{
	luaL_checktype(L, -1, LUA_TTABLE);
	int len = luaL_getn(L, -1);

	if (!lua_isnil(L, -1) && len > 0)
	{
		for (int i = 0; i < len; i++)
		{
			lua_rawgeti(L, -1, i + 1);
			int value = luaL_checkinteger(L, -1);
			builder.AddChar((ImWchar)value);
			lua_pop(L, 1);
		}
	}
}

void loadRangesConf(lua_State* L, ImFontGlyphRangesBuilder &builder, ImFontAtlas* atlas)
{
	luaL_checktype(L, -1, LUA_TTABLE);
	int len = luaL_getn(L, -1);

	if (!lua_isnil(L, -1) && len > 0)
	{
		for (int i = 0; i < len; i++)
		{
			lua_rawgeti(L, -1, i + 1);
			if (lua_type(L, -1) == LUA_TTABLE)
			{
				int ranges_len = luaL_getn(L, -1);
				if (ranges_len > 0)
				{
					// Get last element
					lua_rawgeti(L, -1, ranges_len);
					int last = luaL_checkinteger(L, -1);
					lua_pop(L, 1);

					// Check if array is not zero terminated
					int offset = 0;
					if (last != 0)
					{
						offset = 1;
						ranges_len++;
					}

					ImWchar* ranges = new ImWchar[ranges_len];
					ranges[ranges_len - 1] = 0;

					for (int j = 0; j < ranges_len - offset; j++)
					{
						lua_rawgeti(L, -1, j + 1);
						int v = luaL_checkinteger(L, -1);
						ranges[j] = v;
						lua_pop(L, 1);
					}

					builder.AddRanges(ranges);

					delete[] ranges;
				}
			}
			else if (lua_type(L, -1) == LUA_TNUMBER)
			{
				int value = luaL_checkinteger(L, -1);
				builder.AddRanges(getRanges(atlas, value));
			}
			else
			{
				LUA_THROW_ERRORF("Expected \"number\" or \"table\" to \"ranges\" table, but got: %s", lua_typename(L, lua_type(L, -1)));
			}
			lua_pop(L, 1);
		}
	}
}

void loadFontConfig(lua_State* L, int index, ImFontConfig &config, ImFontAtlas* atlas, ImVector<ImWchar>& ranges)
{
	luaL_checktype(L, index, LUA_TTABLE);

	lua_getfield(L, index, "glyphExtraSpacingX");
	if (!lua_isnil(L, -1)) config.GlyphExtraSpacing.x = luaL_checknumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "glyphExtraSpacingY");
	if (!lua_isnil(L, -1)) config.GlyphExtraSpacing.y = luaL_checknumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "glyphOffsetX");
	if (!lua_isnil(L, -1)) config.GlyphOffset.x = luaL_checknumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "glyphOffsetY");
	if (!lua_isnil(L, -1)) config.GlyphOffset.y = luaL_checknumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "fontDataOwnedByAtlas");
	if (!lua_isnil(L, -1)) config.FontDataOwnedByAtlas = lua_toboolean(L, -1) > 0;
	lua_pop(L, 1);

	lua_getfield(L, index, "pixelSnapH");
	if (!lua_isnil(L, -1)) config.PixelSnapH = lua_toboolean(L, -1) > 0;
	lua_pop(L, 1);

	lua_getfield(L, index, "fontNo");
	if (!lua_isnil(L, -1)) config.FontNo = luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "oversampleH");
	if (!lua_isnil(L, -1)) config.OversampleH = luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "oversampleV");
	if (!lua_isnil(L, -1)) config.OversampleV = luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "sizePixels");
	if (!lua_isnil(L, -1)) config.SizePixels = luaL_checknumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "glyphMinAdvanceX");
	if (!lua_isnil(L, -1)) config.GlyphMinAdvanceX = luaL_checknumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "glyphMaxAdvanceX");
	if (!lua_isnil(L, -1)) config.GlyphMaxAdvanceX = luaL_checknumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "mergeMode");
	if (!lua_isnil(L, -1)) config.MergeMode = lua_toboolean(L, -1) > 0;
	lua_pop(L, 1);

	lua_getfield(L, index, "fontBuilderFlags");
	if (!lua_isnil(L, -1)) config.FontBuilderFlags = luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "rasterizerMultiply");
	if (!lua_isnil(L, -1)) config.RasterizerMultiply = luaL_checknumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "ellipsisChar");
	if (!lua_isnil(L, -1)) config.EllipsisChar = (ImWchar)luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "glyphs");
	if (!lua_isnil(L, -1))
	{
		luaL_checktype(L, -1, LUA_TTABLE);

		ImFontGlyphRangesBuilder builder;

		lua_getfield(L, -1, "text");
		if (!lua_isnil(L, -1)) builder.AddText(luaL_checkstring(L, -1));
		lua_pop(L, 1);

		lua_getfield(L, -1, "ranges");
		if (!lua_isnil(L, -1)) loadRangesConf(L, builder, atlas);
		lua_pop(L, 1);

		lua_getfield(L, -1, "chars");
		if (!lua_isnil(L, -1)) loadCharsConf(L, builder);
		lua_pop(L, 1);

		builder.BuildRanges(&ranges);
	}
	lua_pop(L, 1);
}

int PushFont(lua_State* L)
{
	STACK_CHECKER(L, "pushFont", 0);

	ImFont* font = getPtr<ImFont>(L, "ImFont", 2);
	LUA_ASSERT(font, "Font is nil");
	ImGui::PushFont(font);
	return 0;
}

int PopFont(lua_State* _UNUSED(L))
{
	STACK_CHECKER(L, "popFont", 0);

	ImGui::PopFont();
	return 0;
}

ImFont* addFont(lua_State *L, ImFontAtlas* atlas, const char* file_name, double size_pixels, bool setupConfig = false, int idx = -1)
{
	ImFontConfig cfg = ImFontConfig();
	ImVector<ImWchar> ranges;

	if (setupConfig)
	{
		loadFontConfig(L, idx, cfg, atlas, ranges);
		cfg.GlyphRanges = ranges.Data;
	}

	ImFont* font = atlas->AddFontFromFileTTF(file_name, size_pixels, &cfg);
	return font;
}

int FontAtlas_AddFont(lua_State *L)
{
	STACK_CHECKER(L, "addFont", 1);

	ImFontAtlas* atlas = getPtr<ImFontAtlas>(L, "ImFontAtlas");

	const char* file_name = luaL_checkstring(L, 2);
	double size_pixels = luaL_checknumber(L, 3);

	ImFont* font = addFont(L, atlas, file_name, size_pixels, lua_gettop(L) > 3, 4);

	g_pushInstance(L, "ImFont", font);

	return 1;
}

int FontAtlas_AddFonts(lua_State *L)
{
	STACK_CHECKER(L, "addFonts", 1);

	ImFontAtlas* atlas = getPtr<ImFontAtlas>(L, "ImFontAtlas");

	luaL_checktype(L, 2, LUA_TTABLE);
	int len = luaL_getn(L, 2);

	lua_createtable(L, len, 0);
	for (int i = 0; i < len; i++)
	{
		lua_rawgeti(L, 2, i + 1);

		lua_rawgeti(L, -1, 1);
		const char* file_name = luaL_checkstring(L, -1);
		lua_pop(L, 1);

		lua_rawgeti(L, -1, 2);
		double size_pixels = luaL_checknumber(L, -1);
		lua_pop(L, 1);

		// options table
		lua_rawgeti(L, -1, 3);
		ImFont* font = addFont(L, atlas, file_name, size_pixels, !lua_isnil(L, -1), -1);
		lua_pop(L, 1);

		lua_pop(L, 1);

		g_pushInstance(L, "ImFont", font);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}

int FontAtlas_GetFonts(lua_State* L)
{
	STACK_CHECKER(L, "getFonts", 1);

	ImFontAtlas* atlas = getPtr<ImFontAtlas>(L, "ImFontAtlas");
	int count = atlas->Fonts.Size;
	if (count == 0)
	{
		lua_pushnil(L);
		return 1;
	}

	lua_createtable(L, count, 0);
	for (int i = 0; i < count; i++)
	{
		g_pushInstance(L, "ImFont", atlas->Fonts[i]);
		lua_rawseti(L, -2, i + 1);
	}

	return 1;
}

int FontAtlas_Build(lua_State* L)
{
	STACK_CHECKER(L, "build", 0);

	ImFontAtlas* atlas = getPtr<ImFontAtlas>(L, "ImFontAtlas");
	// TexID will be cleared after "GetTexDataAsRGBA32" call, but we need it
	// to updated already existing texture
	void* id = atlas->TexID;

	LUA_ASSERT(id != NULL, "No texture generated!");

	unsigned char* pixels;
	int width, height;
	atlas->GetTexDataAsRGBA32(&pixels, &width, &height);

	gtexture_update((g_id)(uintptr_t)id, width, height, GTEXTURE_RGBA, GTEXTURE_UNSIGNED_BYTE, GTEXTURE_CLAMP,  GTEXTURE_LINEAR, pixels);
	atlas->SetTexID(id);

	return 0;
}

int FontAtlas_GetFontByIndex(lua_State* L)
{
	STACK_CHECKER(L, "getFont", 1);

	ImFontAtlas* atlas = getPtr<ImFontAtlas>(L, "ImFontAtlas");
	int index = 0;
	if (lua_gettop(L) > 1 && !lua_isnil(L, 2))
	{
		index = luaL_checkinteger(L, 2) - 1;
	}
	int fonts_count = atlas->Fonts.Size;
	LUA_ASSERTF(index >= 0 && index < fonts_count, "Font index is out of bounds! Must be [1..%d]", fonts_count);
	ImFont* font = atlas->Fonts[index];
	LUA_ASSERT(font, "Font is nil");

	g_pushInstance(L, "ImFont", font);
	return 1;
}

int FontAtlas_GetFontsSize(lua_State* L)
{
	STACK_CHECKER(L, "getFontsCount", 1);

	ImFontAtlas* atlas = getPtr<ImFontAtlas>(L, "ImFontAtlas");
	int fonts_count = atlas->Fonts.Size;
	lua_pushinteger(L, fonts_count);
	return 1;
}

int FontAtlas_GetCurrentFont(lua_State* L)
{
	STACK_CHECKER(L, "getCurrentFont", 1);

	g_pushInstance(L, "ImFont", ImGui::GetFont());
	return 1;
}

int FontAtlas_AddDefaultFont(lua_State* L)
{
	STACK_CHECKER(L, "addDefaultFont", 0);

	ImFontAtlas* atlas = getPtr<ImFontAtlas>(L, "ImFontAtlas");
	atlas->AddFontDefault();
	return 0;
}

int FontAtlas_ClearInputData(lua_State* L)
{
	STACK_CHECKER(L, "clearInputData", 0);

	ImFontAtlas* atlas = getPtr<ImFontAtlas>(L, "ImFontAtlas");
	atlas->ClearInputData();
	return 0;
}

int FontAtlas_ClearTexData(lua_State* L)
{
	STACK_CHECKER(L, "clearTexData", 0);

	ImFontAtlas* atlas = getPtr<ImFontAtlas>(L, "ImFontAtlas");
	atlas->ClearTexData();
	return 0;
}

int FontAtlas_ClearFonts(lua_State* L)
{
	STACK_CHECKER(L, "clearFonts", 0);

	ImFontAtlas* atlas = getPtr<ImFontAtlas>(L, "ImFontAtlas");
	atlas->ClearFonts();
	return 0;
}

int FontAtlas_Clear(lua_State* L)
{
	STACK_CHECKER(L, "clear", 0);

	ImFontAtlas* atlas = getPtr<ImFontAtlas>(L, "ImFontAtlas");
	atlas->Clear();
	return 0;
}

int FontAtlas_IsBuilt(lua_State* L)
{
	STACK_CHECKER(L, "isBuilt", 1);

	ImFontAtlas* atlas = getPtr<ImFontAtlas>(L, "ImFontAtlas");
	lua_pushboolean(L, atlas->IsBuilt());
	return 1;
}

int FontAtlas_AddCustomRectRegular(lua_State* L)
{
	STACK_CHECKER(L, "addCustomRectRegular", 1);

	ImFontAtlas* atlas = getPtr<ImFontAtlas>(L, "ImFontAtlas");
	int width  = luaL_checkinteger(L, 2);
	int height = luaL_checkinteger(L, 3);
	lua_pushinteger(L, atlas->AddCustomRectRegular(width, height));
	return 1;
}

int FontAtlas_AddCustomRectFontGlyph(lua_State* L)
{
	STACK_CHECKER(L, "addCustomRectFontGlyph", 1);

	ImFontAtlas* atlas = getPtr<ImFontAtlas>(L, "ImFontAtlas");
	ImFont* font = getPtr<ImFont>(L, "ImFont", 2);
	ImWchar id = (ImWchar)luaL_checkinteger(L, 3);
	int width = luaL_checkinteger(L, 4);
	int height = luaL_checkinteger(L, 5);
	float advance_x = luaL_checkinteger(L, 6);
	const ImVec2 offset = luaL_optvec2(L, 7);

	lua_pushinteger(L, atlas->AddCustomRectFontGlyph(font, id, width, height, advance_x, offset));
	return 1;
}

int FontAtlas_GetCustomRectByIndex(lua_State* L)
{
	STACK_CHECKER(L, "getCustomRectByIndex", 10);

	int index = luaL_checkinteger(L, 2);
	ImFontAtlas* atlas = getPtr<ImFontAtlas>(L, "ImFontAtlas");
	ImFontAtlasCustomRect* rect = atlas->GetCustomRectByIndex(index);
	lua_pushinteger(L, rect->Width);
	lua_pushinteger(L, rect->Height);
	lua_pushinteger(L, rect->X);
	lua_pushinteger(L, rect->Y);
	lua_pushinteger(L, rect->X);
	lua_pushinteger(L, rect->GlyphID);
	lua_pushnumber(L, rect->GlyphOffset.x);
	lua_pushnumber(L, rect->GlyphOffset.y);

	g_pushInstance(L, "ImFont", rect->Font);
	lua_pushboolean(L, rect->IsPacked());
	return 10;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// ImFont
///
/////////////////////////////////////////////////////////////////////////////////////////////

int ImFont_GetFontSize(lua_State* L)
{
	STACK_CHECKER(L, "getFontSize", 1);

	ImFont* font = getPtr<ImFont>(L, "ImFont");
	lua_pushnumber(L, font->FontSize);
	return 1;
}

int ImFont_GetContainerAtlas(lua_State* L)
{
	STACK_CHECKER(L, "getContainerAtlas", 1);

	ImFont* font = getPtr<ImFont>(L, "ImFont");
	g_pushInstance(L, "ImFontAtlas", font->ContainerAtlas);
	return 1;
}

int ImFont_SetScale(lua_State* L)
{
	STACK_CHECKER(L, "setScale", 0);

	ImFont* font = getPtr<ImFont>(L, "ImFont");
	font->Scale = luaL_checknumber(L, 2);
	return 0;
}

int ImFont_GetScale(lua_State* L)
{
	STACK_CHECKER(L, "getScale", 1);

	ImFont* font = getPtr<ImFont>(L, "ImFont");
	lua_pushnumber(L, font->Scale);
	return 1;
}

int ImFont_GetAscent(lua_State* L)
{
	STACK_CHECKER(L, "getAscent", 1);

	ImFont* font = getPtr<ImFont>(L, "ImFont");
	lua_pushnumber(L, font->Ascent);
	return 1;
}

int ImFont_GetDescent(lua_State* L)
{
	STACK_CHECKER(L, "getDescent", 1);

	ImFont* font = getPtr<ImFont>(L, "ImFont");
	lua_pushnumber(L, font->Descent);
	return 1;
}

int ImFont_IsLoaded(lua_State* L)
{
	STACK_CHECKER(L, "isLoaded", 1);

	ImFont* font = getPtr<ImFont>(L, "ImFont");
	lua_pushboolean(L, font->IsLoaded());
	return 1;
}

int ImFont_GetDebugName(lua_State* L)
{
	STACK_CHECKER(L, "getDebugName", 1);

	ImFont* font = getPtr<ImFont>(L, "ImFont");
	lua_pushstring(L, font->GetDebugName());
	return 1;
}

int ImFont_CalcTextSizeA(lua_State* L)
{
	STACK_CHECKER(L, "calcTextSizeA", 2);

	ImFont* font = getPtr<ImFont>(L, "ImFont");
	float size = luaL_checknumber(L, 2);
	float max_width = luaL_checknumber(L, 3);
	float wrap_width = luaL_checknumber(L, 4);
	const char* text = luaL_checkstring(L, 5);
	ImVec2 tsize = font->CalcTextSizeA(size, max_width, wrap_width, text);
	lua_pushnumber(L, tsize.x);
	lua_pushnumber(L, tsize.y);
	return 2;
}

int ImFont_CalcWordWrapPositionA(lua_State* L)
{
	STACK_CHECKER(L, "calcWordWrapPositionA", 1);

	ImFont* font = getPtr<ImFont>(L, "ImFont");
	float scale = luaL_checknumber(L, 2);
	const char* text = luaL_checkstring(L, 3);
	float wrap_width = luaL_checknumber(L, 4);
	const char* t = font->CalcWordWrapPositionA(scale, text, NULL, wrap_width);
	lua_pushstring(L, t);
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// DRAW LIST
///
/////////////////////////////////////////////////////////////////////////////////////////////

int GetWindowDrawList(lua_State* L)
{
	STACK_CHECKER(L, "getWindowDrawList", 1);

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	g_pushInstance(L, "ImDrawList", draw_list);
	return 1;
}

int GetBackgroundDrawList(lua_State* L)
{
	STACK_CHECKER(L, "getBackgroundDrawList", 1);

	ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
	g_pushInstance(L, "ImDrawList", draw_list);
	return 1;
}

int GetForegroundDrawList(lua_State* L)
{
	STACK_CHECKER(L, "getForegroundDrawList", 1);

	ImDrawList* draw_list = ImGui::GetForegroundDrawList();
	g_pushInstance(L, "ImDrawList", draw_list);
	return 1;
}

int DrawList_GetFont(lua_State* L)
{
	STACK_CHECKER(L, "getFont", 1);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	ImFont* font = list->_Data->Font;
	g_pushInstance(L, "ImFont", font);
	return 1;
}

int DrawList_GetFontSize(lua_State* L)
{
	STACK_CHECKER(L, "getFontSize", 1);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	float font_size = list->_Data->FontSize;
	lua_pushnumber(L, font_size);
	return 1;
}

int DrawList_GetCurveTessellationTol(lua_State* L)
{
	STACK_CHECKER(L, "getCurveTessellationTol", 1);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	float tessellation = list->_Data->CurveTessellationTol;
	lua_pushnumber(L, tessellation);
	return 1;
}

int DrawList_GetCircleSegmentMaxError(lua_State* L)
{
	STACK_CHECKER(L, "getCircleSegmentMaxError", 1);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	float maxError = list->_Data->CircleSegmentMaxError;
	lua_pushnumber(L, maxError);
	return 1;
}

int DrawList_PushClipRect(lua_State* L)
{
	STACK_CHECKER(L, "pushClipRect", 0);

	ImVec2 clip_rect_min = luaL_checkvec2(L, 2);
	ImVec2 clip_rect_max = luaL_checkvec2(L, 4);
	bool intersect_with_current_clip_rect = luaL_optboolean(L, 6, 0) > 0;

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->PushClipRect(clip_rect_min, clip_rect_max, intersect_with_current_clip_rect);
	return 0;
}

int DrawList_PushClipRectFullScreen(lua_State* L)
{
	STACK_CHECKER(L, "pushClipRectFullScreen", 0);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->PushClipRectFullScreen();
	return 0;
}

int DrawList_PopClipRect(lua_State* L)
{
	STACK_CHECKER(L, "popClipRect", 0);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->PopClipRect();
	return 0;
}

int DrawList_PushTextureID(lua_State* L)
{
	STACK_CHECKER(L, "pushTextureID", 0);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	GTextureData data(L, 2);
	list->PushTextureID(data.texture);
	return 0;
}

int DrawList_PopTextureID(lua_State* L)
{
	STACK_CHECKER(L, "popTextureID", 0);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->PopTextureID();
	return 0;
}

int DrawList_GetClipRectMin(lua_State* L)
{
	STACK_CHECKER(L, "getClipRectMin", 2);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	ImVec2 min = list->GetClipRectMin();
	lua_pushnumber(L, min.x);
	lua_pushnumber(L, min.y);
	return 2;
}

int DrawList_GetClipRectMax(lua_State* L)
{
	STACK_CHECKER(L, "getClipRectMax", 2);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	ImVec2 max = list->GetClipRectMax();
	lua_pushnumber(L, max.x);
	lua_pushnumber(L, max.y);
	return 2;
}

int DrawList_AddLine(lua_State* L)
{
	STACK_CHECKER(L, "addLine", 0);

	ImVec2 p1 = luaL_checkvec2(L, 2);
	ImVec2 p2 = luaL_checkvec2(L, 4);
	ImU32 col = GColor::toU32(L, 6);
	double thickness = luaL_optnumber(L, 8, 1.0f);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->AddLine(p1, p2, col, thickness);
	return 0;
}

int DrawList_AddRect(lua_State* L)
{
	STACK_CHECKER(L, "addRect", 0);

	ImVec2 p_min = luaL_checkvec2(L, 2);
	ImVec2 p_max = luaL_checkvec2(L, 4);
	ImU32 col = GColor::toU32(L, 6);
	double rounding = luaL_optnumber(L, 8, 0.0f);
	ImDrawFlags rounding_corners = luaL_optinteger(L, 9, ImDrawFlags_RoundCornersAll);
	double thickness = luaL_optnumber(L, 10, 1.0f);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->AddRect(p_min, p_max, col, rounding, rounding_corners, thickness);

	return 0;
}

int DrawList_AddRectFilled(lua_State* L)
{
	STACK_CHECKER(L, "addRectFilled", 0);

	ImVec2 p_min = luaL_checkvec2(L, 2);
	ImVec2 p_max = luaL_checkvec2(L, 4);
	ImU32 col = GColor::toU32(L, 6);
	double rounding = luaL_optnumber(L, 8, 0.0f);
	ImDrawFlags rounding_corners = luaL_optinteger(L, 9, ImDrawFlags_RoundCornersAll);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->AddRectFilled(p_min, p_max, col, rounding, rounding_corners);

	return 0;
}

int DrawList_AddRectFilledMultiColor(lua_State* L)
{
	STACK_CHECKER(L, "addRectFilledMultiColor", 0);

	ImVec2 p_min = luaL_checkvec2(L, 2);
	ImVec2 p_max = luaL_checkvec2(L, 4);
	ImU32 col_upr_left  = GColor::toU32(L, 6);
	ImU32 col_upr_right = GColor::toU32(L, 8);
	ImU32 col_bot_right = GColor::toU32(L, 10);
	ImU32 col_bot_left  = GColor::toU32(L, 12);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->AddRectFilledMultiColor(p_min, p_max, col_upr_left, col_upr_right, col_bot_right, col_bot_left);

	return 0;
}

int DrawList_AddQuad(lua_State* L)
{
	STACK_CHECKER(L, "addQuad", 0);

	ImVec2 p1 = luaL_checkvec2(L, 2);
	ImVec2 p2 = luaL_checkvec2(L, 4);
	ImVec2 p3 = luaL_checkvec2(L, 6);
	ImVec2 p4 = luaL_checkvec2(L, 8);
	ImU32 col = GColor::toU32(L, 10);
	double thickness = luaL_optnumber(L, 12, 1.0f);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->AddQuad(p1, p2, p3, p4, col, thickness);

	return  0;
}

int DrawList_AddQuadFilled(lua_State* L)
{
	STACK_CHECKER(L, "addQuadFilled", 0);

	ImVec2 p1 = luaL_checkvec2(L, 2);
	ImVec2 p2 = luaL_checkvec2(L, 4);
	ImVec2 p3 = luaL_checkvec2(L, 6);
	ImVec2 p4 = luaL_checkvec2(L, 8);
	ImU32 col = GColor::toU32(L, 10);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->AddQuadFilled(p1, p2, p3, p4, col);

	return  0;
}

int DrawList_AddTriangle(lua_State* L)
{
	STACK_CHECKER(L, "addTriangle", 0);

	ImVec2 p1 = luaL_checkvec2(L, 2);
	ImVec2 p2 = luaL_checkvec2(L, 4);
	ImVec2 p3 = luaL_checkvec2(L, 6);
	ImU32 col = GColor::toU32(L, 8);
	double thickness = luaL_optnumber(L, 10, 1.0f);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->AddTriangle(p1, p2, p3, col, thickness);

	return  0;
}

int DrawList_AddTriangleFilled(lua_State* L)
{
	STACK_CHECKER(L, "addTriangleFilled", 0);

	ImVec2 p1 = luaL_checkvec2(L, 2);
	ImVec2 p2 = luaL_checkvec2(L, 4);
	ImVec2 p3 = luaL_checkvec2(L, 6);
	ImU32 col = GColor::toU32(L, 8);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->AddTriangleFilled(p1, p2, p3, col);

	return  0;
}

int DrawList_AddCircle(lua_State* L)
{
	STACK_CHECKER(L, "addCircle", 0);

	ImVec2 center = luaL_checkvec2(L, 2);
	double radius = luaL_checknumber(L, 4);
	ImU32 col = GColor::toU32(L, 5);
	int num_segments = luaL_optinteger(L, 7, 0);
	double thickness = luaL_optnumber(L, 8, 1.0f);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->AddCircle(center, radius, col, num_segments, thickness);

	return 0;
}

int DrawList_AddCircleFilled(lua_State* L)
{
	STACK_CHECKER(L, "addCircleFilled", 0);

	ImVec2 center = luaL_checkvec2(L, 2);
	double radius = luaL_checknumber(L, 4);
	ImU32 col = GColor::toU32(L, 5);
	int num_segments = luaL_optinteger(L, 7, 0);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->AddCircleFilled(center, radius, col, num_segments);

	return 0;
}

int DrawList_AddNgon(lua_State* L)
{
	STACK_CHECKER(L, "addNgon", 0);

	ImVec2 center = luaL_checkvec2(L, 2);
	double radius = luaL_checknumber(L, 4);
	ImU32 col = GColor::toU32(L, 5);
	int num_segments = luaL_optinteger(L, 7, 0);
	double thickness = luaL_optnumber(L, 8, 1.0f);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->AddNgon(center, radius, col, num_segments, thickness);

	return 0;
}

int DrawList_AddNgonFilled(lua_State* L)
{
	STACK_CHECKER(L, "addNgonFilled", 0);

	ImVec2 center = luaL_checkvec2(L, 2);
	double radius = luaL_checknumber(L, 4);
	ImU32 col = GColor::toU32(L, 5);
	int num_segments = luaL_optinteger(L, 7, 0);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->AddNgonFilled(center, radius, col, num_segments);

	return 0;
}

int DrawList_AddText(lua_State* L)
{
	STACK_CHECKER(L, "addText", 0);

	ImVec2 pos = luaL_checkvec2(L, 2);
	ImU32 col = GColor::toU32(L, 4);
	const char* text = luaL_checkstring(L, 6);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->AddText(pos, col, text);

	return 0;
}

int DrawList_AddFontText(lua_State* L)
{
	STACK_CHECKER(L, "addFontText", 0);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	ImFont* font = getPtr<ImFont>(L, "ImFont", 2);
	double font_size = luaL_checknumber(L, 3);
	ImVec2 pos = luaL_checkvec2(L, 4);
	ImU32 col = GColor::toU32(L, 6);
	const char* text = luaL_checkstring(L, 8);
	double wrap_width = luaL_optnumber(L, 9, 0.0f);
	ImVec4* cpu_fine_clip_rect = NULL;
	if (lua_gettop(L) > 9)
	{
		ImVec4 rect = luaL_checkvec4(L, 10);
		cpu_fine_clip_rect = &rect;
	}
	list->AddText(font, font_size, pos, col, text, NULL, wrap_width, cpu_fine_clip_rect);
	return 0;
}

int DrawList_AddPolyline(lua_State* L)
{
	STACK_CHECKER(L, "addPolyline", 0);


	luaL_checktype(L, 2, LUA_TTABLE);
	int index = 0;
	int num_points = luaL_getn(L, 2);
	ImVec2* points = new ImVec2[num_points];
	lua_pushvalue(L, 2);
	for (int i = 0; i < num_points; i+=2)
	{
		lua_rawgeti(L, 2, i+1);
		double x = luaL_checknumber(L, -1);
		lua_pop(L, 1);

		lua_rawgeti(L, 2, i+2);
		double y = luaL_checknumber(L, -1);
		points[index] = ImVec2(x,y);

		index ++;

		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	ImU32 col = GColor::toU32(L, 3);
	bool closed = lua_toboolean(L, 5) > 0;
	double thickness = luaL_checknumber(L, 6);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->AddPolyline(points, index, col, closed, thickness);
	delete[] points;
	return  0;
}

int DrawList_AddConvexPolyFilled(lua_State* L)
{
	STACK_CHECKER(L, "addConvexPolyFilled", 0);


	luaL_checktype(L, 2, LUA_TTABLE);
	int index = 0;
	int num_points = luaL_getn(L, 2);
	ImVec2* points = new ImVec2[num_points];
	lua_pushvalue(L, 2);
	for (int i = 0; i < num_points; i+=2)
	{
		lua_rawgeti(L, 2, i + 1);
		double x = luaL_checknumber(L, -1);
		lua_pop(L, 1);

		lua_rawgeti(L, 2, i + 2);
		double y = luaL_checknumber(L, -1);
		points[index] = ImVec2(x, y);

		index++;

		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	ImU32 col = GColor::toU32(L, 3);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->AddConvexPolyFilled(points, index, col);
	delete[] points;
	return  0;
}

int DrawList_AddBezierCubic(lua_State* L)
{
	STACK_CHECKER(L, "addBezierCubic", 0);

	ImVec2 p1 = luaL_checkvec2(L, 2);
	ImVec2 p2 = luaL_checkvec2(L, 4);
	ImVec2 p3 = luaL_checkvec2(L, 6);
	ImVec2 p4 = luaL_checkvec2(L, 8);
	ImU32 col = GColor::toU32(L, 10);
	double thickness = luaL_checknumber(L, 12);
	int num_segments = luaL_optinteger(L, 13, 0);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->AddBezierCubic(p1, p2, p3, p4, col, thickness, num_segments);
	return 0;
}

int DrawList_AddBezierQuadratic(lua_State* L)
{
	STACK_CHECKER(L, "addBezierQuadratic", 0);

	ImVec2 p1 = luaL_checkvec2(L, 2);
	ImVec2 p2 = luaL_checkvec2(L, 4);
	ImVec2 p3 = luaL_checkvec2(L, 6);
	ImU32 col = GColor::toU32(L, 8);
	double thickness = luaL_checknumber(L, 10);
	int num_segments = luaL_optinteger(L, 11, 0);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->AddBezierQuadratic(p1, p2, p3, col, thickness, num_segments);
	return 0;
}

int DrawList_AddImage(lua_State* L)
{
	STACK_CHECKER(L, "addImage", 0);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	GTextureData data(L, 2);
	ImVec2 p_min = luaL_checkvec2(L, 3);
	ImVec2 p_max = luaL_checkvec2(L, 5);
	ImU32 col = GColor::toU32opt(L, 7);

	list->AddImage(data.texture, p_min, p_max, data.uv0, data.uv1, col);
	return 0;
}


int DrawList_AddImageUV(lua_State* L)
{
	STACK_CHECKER(L, "addImage", 0);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	GTextureData data(L, 2);
	ImVec2 p_min = luaL_checkvec2(L, 3);
	ImVec2 p_max = luaL_checkvec2(L, 5);
	ImVec2 uv0 = luaL_checkvec2(L, 7);
	ImVec2 uv1 = luaL_checkvec2(L, 9);
	ImU32 col = GColor::toU32opt(L, 11);

	list->AddImage(data.texture, p_min, p_max, uv0, uv1, col);
	return 0;
}

int DrawList_AddImageQuad(lua_State* L)
{
	STACK_CHECKER(L, "addImageQuad", 0);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	GTextureData data(L, 2);
	ImVec2 p1 = luaL_checkvec2(L, 3);
	ImVec2 p2 = luaL_checkvec2(L, 5);
	ImVec2 p3 = luaL_checkvec2(L, 7);
	ImVec2 p4 = luaL_checkvec2(L, 9);
	ImU32 col = GColor::toU32opt(L, 11);
	ImVec2 uv1 = luaL_optvec2(L, 13, 0.0f, 0.0f);
	ImVec2 uv2 = luaL_optvec2(L, 15, 1.0f, 0.0f);
	ImVec2 uv3 = luaL_optvec2(L, 17, 1.0f, 1.0f);
	ImVec2 uv4 = luaL_optvec2(L, 19, 0.0f, 1.0f);

	list->AddImageQuad(data.texture, p1, p2, p3, p4, uv1, uv2, uv3, uv4, col);
	return 0;
}

int DrawList_AddImageRounded(lua_State* L)
{
	STACK_CHECKER(L, "addImageRounded", 0);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	GTextureData data(L, 2);
	ImVec2 p_min = luaL_checkvec2(L, 3);
	ImVec2 p_max = luaL_checkvec2(L, 5);
	ImU32 col = GColor::toU32(L, 7);
	double rounding = luaL_checknumber(L, 9);
	ImDrawFlags rounding_corners = luaL_optinteger(L, 10, ImDrawFlags_RoundCornersAll);
	list->AddImageRounded(data.texture, p_min, p_max, data.uv0, data.uv1, col, rounding, rounding_corners);
	return 0;
}

int DrawList_AddImageRoundedUV(lua_State* L)
{
	STACK_CHECKER(L, "addImageRounded", 0);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	GTextureData data(L, 2);
	ImVec2 p_min = luaL_checkvec2(L, 3);
	ImVec2 p_max = luaL_checkvec2(L, 5);
	ImVec2 uv0 = luaL_checkvec2(L, 7);
	ImVec2 uv1 = luaL_checkvec2(L, 9);
	ImU32 col = GColor::toU32(L, 11);
	double rounding = luaL_checknumber(L, 13);
	ImDrawFlags rounding_corners = luaL_optinteger(L, 14, ImDrawFlags_RoundCornersAll);
	list->AddImageRounded(data.texture, p_min, p_max, uv0, uv1, col, rounding, rounding_corners);
	return 0;
}

int DrawList_AddScaledImage(lua_State* L)
{
	STACK_CHECKER(L, "addScaledImage", 0);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	GTextureData data(L, 2);
	ImVec2 p_min = luaL_checkvec2(L, 3);
	ImVec2 p_max = luaL_checkvec2(L, 5);
	ImU32 col = GColor::toU32opt(L, 7);
	ImVec2 anchor = luaL_optvec2(L, 9, 0.5f, 0.5f);
	int scale_mode = luaL_optinteger(L, 11, 0);
	bool keep_size = lua_toboolean(L, 12);
	ImRect bb(p_min, p_max);
	ImGui::FitImage(bb.Min, bb.Max, p_max - p_min, data.texture_size, anchor, scale_mode, keep_size);
	list->AddImage(data.texture, p_min, p_max, data.uv0, data.uv1, col);
	return 0;
}

int DrawList_AddScaledImageRounded(lua_State* L)
{
	STACK_CHECKER(L, "addScaledImageRounded", 0);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	GTextureData data(L, 2);
	ImVec2 p_min = luaL_checkvec2(L, 3);
	ImVec2 p_max = luaL_checkvec2(L, 5);
	ImU32 col = GColor::toU32(L, 7);
	double rounding = luaL_checknumber(L, 9);
	ImDrawFlags rounding_corners = luaL_optinteger(L, 10, ImDrawFlags_RoundCornersAll);
	ImVec2 anchor = luaL_optvec2(L, 11, 0.5f, 0.5f);
	int scale_mode = luaL_optinteger(L, 13, 0);
	bool keep_size = lua_toboolean(L, 14);

	ImRect bb(p_min, p_max);
	ImGui::FitImage(bb.Min, bb.Max, p_max - p_min, data.texture_size, anchor, scale_mode, keep_size);
	list->AddImageRounded(data.texture, bb.Min, bb.Max, data.uv0, data.uv1, col, rounding, rounding_corners);
	return 0;
}

// TODO: add scaled images to draw lists

int DrawList_PathClear(lua_State* L)
{
	STACK_CHECKER(L, "pathClear", 0);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->PathClear();
	return 0;
}

int DrawList_PathLineTo(lua_State* L)
{
	STACK_CHECKER(L, "pathLineTo", 0);

	ImVec2 pos = luaL_checkvec2(L, 2);
	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->PathLineTo(pos);
	return 0;
}

int DrawList_PathLineToMergeDuplicate(lua_State* L)
{
	STACK_CHECKER(L, "pathLineToMergeDuplicate", 0);

	ImVec2 pos = luaL_checkvec2(L, 2);
	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->PathLineToMergeDuplicate(pos);
	return 0;
}

int DrawList_PathFillConvex(lua_State* L)
{
	STACK_CHECKER(L, "pathFillConvex", 0);

	ImU32 color = GColor::toU32(L, 2);
	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->PathFillConvex(color);
	return 0;

}

int DrawList_PathStroke(lua_State* L)
{
	STACK_CHECKER(L, "pathStroke", 0);

	ImU32 color = GColor::toU32(L, 2);
	bool closed = lua_toboolean(L, 4) > 0;
	float thickness = luaL_optnumber(L, 5, 1.0f);
	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->PathStroke(color, closed, thickness);
	return 0;
}

int DrawList_PathArcTo(lua_State* L)
{
	STACK_CHECKER(L, "pathArcTo", 0);

	ImVec2 center = luaL_checkvec2(L, 2);
	double radius = luaL_checknumber(L, 4);
	double a_min = luaL_checknumber(L, 5);
	double a_max = luaL_checknumber(L, 6);
	int num_segments = luaL_optinteger(L, 7, 0);
	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->PathArcTo(center, radius, a_min, a_max, num_segments);
	return 0;

}

int DrawList_PathArcToFast(lua_State* L)
{
	STACK_CHECKER(L, "pathArcToFast", 0);

	ImVec2 center = luaL_checkvec2(L, 2);
	double radius = luaL_checknumber(L, 4);
	int a_min = luaL_checkinteger(L, 5);
	int a_max = luaL_checkinteger(L, 6);
	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->PathArcToFast(center, radius, a_min, a_max);
	return 0;

}

int DrawList_PathBezierCubicCurveTo(lua_State* L)
{
	STACK_CHECKER(L, "pathBezierCubicCurveTo", 0);

	ImVec2 p2 = luaL_checkvec2(L, 2);
	ImVec2 p3 = luaL_checkvec2(L, 4);
	ImVec2 p4 = luaL_checkvec2(L, 6);
	int num_segments = luaL_optinteger(L, 8, 0);
	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->PathBezierCubicCurveTo(p2, p3, p4, num_segments);
	return 0;
}

int DrawList_PathBezierQuadraticCurveTo(lua_State* L)
{
	STACK_CHECKER(L, "pathBezierQuadraticCurveTo", 0);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	ImVec2 p2 = luaL_checkvec2(L, 2);
	ImVec2 p3 = luaL_checkvec2(L, 4);
	int num_segments = luaL_optinteger(L, 6, 0);
	list->PathBezierQuadraticCurveTo(p2, p3, num_segments);
	return 0;
}

int DrawList_PathRect(lua_State* L)
{
	STACK_CHECKER(L, "pathRect", 0);

	ImVec2 rect_min = luaL_checkvec2(L, 2);
	ImVec2 rect_max = luaL_checkvec2(L, 4);
	double rounding = luaL_optnumber(L, 6, 0.0f);
	ImDrawFlags rounding_corners = luaL_optinteger(L, 7, ImDrawFlags_RoundCornersAll);
	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	list->PathRect(rect_min, rect_max, rounding, rounding_corners);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// https://gist.github.com/carasuca/e72aacadcf6cf8139de46f97158f790f
// https://github.com/ocornut/imgui/issues/1286

int rotation_start_index;

int DrawList_RotateStart(lua_State* L)
{
	STACK_CHECKER(L, "rotateBegin", 0);

	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	rotation_start_index = list->VtxBuffer.Size;
	return 0;
}

ImVec2 DrawList_RotationCenter(ImDrawList* list)
{
	ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

	const ImVector<ImDrawVert>& buf = list->VtxBuffer;
	for (int i = rotation_start_index; i < buf.Size; i++)
		l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

	return ImVec2((l.x+u.x)/2, (l.y+u.y)/2); // or use _ClipRectStack?
}

int DrawList_RotateEnd(lua_State* L)
{
	STACK_CHECKER(L, "rotateEnd", 0);

	float rad = luaL_checknumber(L, 2);
	ImDrawList* list = getPtr<ImDrawList>(L, "ImDrawList");
	ImVec2 center = DrawList_RotationCenter(list);

	float s = sin(rad), c = cos(rad);
	center = ImRotate(center, s, c) - center;

	ImVector<ImDrawVert>& buf = list->VtxBuffer;
	for (int i = rotation_start_index; i < buf.Size; i++)
		buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

int UpdateMouseCursor(lua_State* L)
{
	STACK_CHECKER(L, "updateCursor", 0);

#if defined(QT_CORE_LIB) || defined(WINSTORE)
	ImGuiIO& io = ImGui::GetIO();
	if (io.MouseDrawCursor)
	{
		setApplicationCursor(L, "blank");
		return 0;
	}

	GidImGui* imgui = getImgui(L);
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	ImGuiMouseCursor cursor = ctx->MouseCursor;
	const char* giderosCursor = "arrow";

	switch (cursor)
	{
		case ImGuiMouseCursor_Hand:       giderosCursor = "pointingHand"; break;
		case ImGuiMouseCursor_None:       giderosCursor = "blank";		  break;
		case ImGuiMouseCursor_ResizeEW:   giderosCursor = "sizeHor";	  break;
		case ImGuiMouseCursor_ResizeNS:   giderosCursor = "sizeVer";	  break;
		case ImGuiMouseCursor_ResizeAll:  giderosCursor = "sizeAll";	  break;
		case ImGuiMouseCursor_TextInput:  giderosCursor = "IBeam";		  break;
		case ImGuiMouseCursor_NotAllowed: giderosCursor = "forbidden";	  break;
		case ImGuiMouseCursor_ResizeNESW: giderosCursor = "sizeBDiag";	  break;
		case ImGuiMouseCursor_ResizeNWSE: giderosCursor = "sizeFDiag";	  break;
	}

	setApplicationCursor(L, giderosCursor);
#endif
	return 0;
}

int SetResetTouchPosOnEnd(lua_State* L)
{
	STACK_CHECKER(L, "setResetTouchPosOnEnd", 0);

	GidImGui* imgui = getImgui(L);
	imgui->resetTouchPosOnEnd = lua_toboolean(L, 2);
	return 0;
}

int GetResetTouchPosOnEnd(lua_State* L)
{
	STACK_CHECKER(L, "getResetTouchPosOnEnd", 1);

	GidImGui* imgui = getImgui(L);
	lua_pushboolean(L, imgui->resetTouchPosOnEnd);
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////
///
/// ImGuiContext
///
//////////////////////////////////////////////////////////////////////////////////////////////

int CTX_GetHoveredWindow(lua_State* L)
{
	STACK_CHECKER(L, "getHoveredWindow", 1);

	GidImGui* ui = getImgui(L);
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	if (ctx->HoveredWindow)
		lua_pushstring(L, ctx->HoveredWindow->Name);
	else
		lua_pushnil(L);
	return 1;
}

int CTX_GetHoveredWindowRoot(lua_State* L)
{
	STACK_CHECKER(L, "getHoveredWindowRoot", 1);

	GidImGui* ui = getImgui(L);
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	if (ctx->HoveredWindow)
		lua_pushstring(L, ctx->HoveredWindow->RootWindow->Name);
	else
		lua_pushnil(L);
	return 1;
}

int CTX_GetHoveredWindowUnderMovingWindow(lua_State* L)
{
	STACK_CHECKER(L, "getHoveredWindowUnderMovingWindow", 1);

	GidImGui* ui = getImgui(L);
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	if (ctx->HoveredWindowUnderMovingWindow)
		lua_pushstring(L, ctx->HoveredWindowUnderMovingWindow->Name);
	else
		lua_pushnil(L);
	return 1;
}

int CTX_GetMovingWindow(lua_State* L)
{
	STACK_CHECKER(L, "getMovingWindow", 1);

	GidImGui* ui = getImgui(L);
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	if (ctx->MovingWindow)
		lua_pushstring(L, ctx->MovingWindow->Name);
	else
		lua_pushnil(L);
	return 1;
}

int CTX_GetActiveIdWindow(lua_State* L)
{
	STACK_CHECKER(L, "getActiveIdWindow", 1);

	GidImGui* ui = getImgui(L);
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	if (ctx->ActiveIdWindow)
		lua_pushstring(L, ctx->ActiveIdWindow->Name);
	else
		lua_pushnil(L);
	return 1;
}

int CTX_GetActiveId(lua_State* L)
{
	STACK_CHECKER(L, "getActiveId", 1);

	GidImGui* ui = getImgui(L);
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	lua_pushnumber(L, ctx->ActiveId);
	return 1;
}

int CTX_GetActiveIdPreviousFrame(lua_State* L)
{
	STACK_CHECKER(L, "getActiveIdPreviousFrame", 1);

	GidImGui* ui = getImgui(L);
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	lua_pushnumber(L, ctx->ActiveIdPreviousFrame);
	return 1;
}

int CTX_GetActiveIdTimer(lua_State* L)
{
	STACK_CHECKER(L, "getActiveIdTimer", 1);

	GidImGui* ui = getImgui(L);
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	lua_pushnumber(L, ctx->ActiveIdTimer);
	return 1;
}

int CTX_GetActiveIdAllowOverlap(lua_State* L)
{
	STACK_CHECKER(L, "getActiveIdAllowOverlap", 1);

	GidImGui* ui = getImgui(L);
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	lua_pushnumber(L, ctx->ActiveIdAllowOverlap);
	return 1;
}

int CTX_GetHoveredId(lua_State* L)
{
	STACK_CHECKER(L, "getHoveredId", 1);

	GidImGui* ui = getImgui(L);
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	lua_pushnumber(L, ctx->HoveredId);
	return 1;
}

int CTX_GetHoveredIdPreviousFrame(lua_State* L)
{
	STACK_CHECKER(L, "getHoveredIdPreviousFrame", 1);

	GidImGui* ui = getImgui(L);
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	lua_pushnumber(L, ctx->HoveredIdPreviousFrame);
	return 1;
}

int CTX_GetHoveredIdTimer(lua_State* L)
{
	STACK_CHECKER(L, "getHoveredIdTimer", 1);

	GidImGui* ui = getImgui(L);
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	lua_pushnumber(L, ctx->HoveredIdTimer);
	return 1;
}

int CTX_GetHoveredIdAllowOverlap(lua_State* L)
{
	STACK_CHECKER(L, "getHoveredIdAllowOverlap", 1);

	GidImGui* ui = getImgui(L);
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	lua_pushnumber(L, ctx->HoveredIdAllowOverlap);
	return 1;
}

int CTX_GetDragDropActive(lua_State* L)
{
	STACK_CHECKER(L, "getDragDropActive", 1);

	GidImGui* ui = getImgui(L);
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	lua_pushboolean(L, ctx->DragDropActive);
	return 1;
}

int CTX_GetDragDropPayloadSourceId(lua_State* L)
{
	STACK_CHECKER(L, "getDragDropPayloadSourceId", 1);

	GidImGui* ui = getImgui(L);
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	lua_pushnumber(L, ctx->DragDropPayload.SourceId);
	return 1;
}

int CTX_GetDragDropPayloadDataType(lua_State* L)
{
	STACK_CHECKER(L, "getDragDropPayloadDataType", 1);

	GidImGui* ui = getImgui(L);
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	lua_pushstring(L, ctx->DragDropPayload.DataType);
	return 1;
}

int CTX_GetDragDropPayloadDataSize(lua_State* L)
{
	STACK_CHECKER(L, "getDragDropPayloadDataSize", 1);

	GidImGui* ui = getImgui(L);
	ImGuiContext* ctx = ImGui::GetCurrentContext();

	lua_pushinteger(L, ctx->DragDropPayload.DataSize);
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////
///
/// TextEditor
///
//////////////////////////////////////////////////////////////////////////////////////////////

int initTextEditor(lua_State* L)
{
	TextEditor* editor;
	if (lua_gettop(L) > 0)
	{
		TextEditor* other = getPtr<TextEditor>(L, "ImGuiTextEditor");
		editor = new TextEditor(*other);
	}
	else
		editor = new TextEditor();

	g_pushInstance(L, "ImGuiTextEditor", editor);

	luaL_rawgetptr(L, LUA_REGISTRYINDEX, &keyWeak);
	lua_pushvalue(L, -2);
	luaL_rawsetptr(L, -2, editor);
	lua_pop(L, 1);

	return 1;
}

int destroyTextEditor(LUA_STATE* p)
{
	destroyObject<TextEditor>(p);
	return 0;
}

int TE_LoadPalette(lua_State* L)
{
	STACK_CHECKER(L, "loadPalette", 0);

	luaL_checktype(L, 2, LUA_TTABLE);
	int count = luaL_getn(L, 2);
	const int MAX = (int)TextEditor::PaletteIndex::Max;
	LUA_ASSERTF(count / 2 == MAX, "Incorrect number of colors. Expected: %d, but got: %d", MAX, count / 2);
	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	for (int i = 0; i < count; i+=2)
	{
		lua_rawgeti(L, 2, i + 1);
		lua_rawgeti(L, 2, i + 2);

		int hex = luaL_checkinteger(L, -2);
		float alpha = luaL_checknumber(L, -1);

		ImU32 color = GColor::toU32(hex, alpha);
		lua_pop(L, 2);

		editor->SetPaletteColor(i / 2, color);
	}
	return 0;
}

int TE_SetLanguageDefinition(lua_State* L)
{
	STACK_CHECKER(L, "setLanguageDefinition", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");

	TextEditor::LanguageDefinition& lang = *getPtr<TextEditor::LanguageDefinition>(L,"ImGuiTextEditorLanguage", 2);
	editor->SetLanguageDefinition(lang);
	return 0;
}

int TE_GetLanguageDefinition_CPP(lua_State* L)
{
	STACK_CHECKER(L, "getLanguageCPP", 1);

	TextEditor::LanguageDefinition* lang = const_cast<TextEditor::LanguageDefinition*>(&(TextEditor::LanguageDefinition::CPlusPlus()));
	g_pushInstance(L, "ImGuiTextEditorLanguage", lang);
	return 1;
}

int TE_GetLanguageDefinition_GLSL(lua_State* L)
{
	STACK_CHECKER(L, "getLanguageGLSL", 1);

	TextEditor::LanguageDefinition* lang = const_cast<TextEditor::LanguageDefinition*>(&(TextEditor::LanguageDefinition::GLSL()));
	g_pushInstance(L, "ImGuiTextEditorLanguage", lang);
	return 1;
}

int TE_GetLanguageDefinition_HLSL(lua_State* L)
{
	STACK_CHECKER(L, "getLanguageHLSL", 1);

	TextEditor::LanguageDefinition* lang = const_cast<TextEditor::LanguageDefinition*>(&(TextEditor::LanguageDefinition::HLSL()));
	g_pushInstance(L, "ImGuiTextEditorLanguage", lang);
	return 1;
}

int TE_GetLanguageDefinition_C(lua_State* L)
{
	STACK_CHECKER(L, "getLanguageC", 1);

	TextEditor::LanguageDefinition* lang = const_cast<TextEditor::LanguageDefinition*>(&(TextEditor::LanguageDefinition::C()));
	g_pushInstance(L, "ImGuiTextEditorLanguage", lang);
	return 1;
}

int TE_GetLanguageDefinition_SQL(lua_State* L)
{
	STACK_CHECKER(L, "getLanguageSQL", 1);

	TextEditor::LanguageDefinition* lang = const_cast<TextEditor::LanguageDefinition*>(&(TextEditor::LanguageDefinition::SQL()));
	g_pushInstance(L, "ImGuiTextEditorLanguage", lang);
	return 1;
}

int TE_GetLanguageDefinition_AngelScript(lua_State* L)
{
	STACK_CHECKER(L, "getLanguageAngelScript", 1);

	TextEditor::LanguageDefinition* lang = const_cast<TextEditor::LanguageDefinition*>(&(TextEditor::LanguageDefinition::AngelScript()));
	g_pushInstance(L, "ImGuiTextEditorLanguage", lang);
	return 1;
}

int TE_GetLanguageDefinition_Lua(lua_State* L)
{
	STACK_CHECKER(L, "getLanguageLua", 1);

	TextEditor::LanguageDefinition* lang = const_cast<TextEditor::LanguageDefinition*>(&(TextEditor::LanguageDefinition::Lua()));
	g_pushInstance(L, "ImGuiTextEditorLanguage", lang);
	return 1;
}

int TE_GetName(lua_State* L)
{
	STACK_CHECKER(L, "getName", 1);

	TextEditor::LanguageDefinition* lang = getPtr<TextEditor::LanguageDefinition>(L, "ImGuiTextEditorLanguage");
	lua_pushstring(L, lang->mName.c_str());
	return 1;
}

int TE_GetLanguageDefinition(lua_State* L)
{
	STACK_CHECKER(L, "getLanguageDefinition", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	g_pushInstance(L, "ImGuiTextEditorLanguage", const_cast<TextEditor::LanguageDefinition*>(&(editor->GetLanguageDefinition())));
	return 1;
}

int TE_GetPalette_Dark(lua_State* L)
{
	STACK_CHECKER(L, "getPaletteDark", 1);

	const TextEditor::Palette& palette = TextEditor::GetDarkPalette();
	TextEditor::Palette* ptr = const_cast<TextEditor::Palette*>(&palette);
	g_pushInstance(L, "ImGuiTextEditorPalette", ptr);
	return 1;
}

int TE_GetPalette_Light(lua_State* L)
{
	STACK_CHECKER(L, "getPaletteLight", 1);

	TextEditor::Palette* palette = const_cast<TextEditor::Palette*>(&(TextEditor::GetLightPalette()));
	g_pushInstance(L, "ImGuiTextEditorPalette", palette);
	return 1;
}

int TE_GetPalette_Retro(lua_State* L)
{
	STACK_CHECKER(L, "getPaletteRetro", 1);

	TextEditor::Palette* palette = const_cast<TextEditor::Palette*>(&(TextEditor::GetRetroBluePalette()));
	g_pushInstance(L, "ImGuiTextEditorPalette", palette);
	return 1;
}

int TE_SetPalette(lua_State* L)
{
	STACK_CHECKER(L, "setPalette", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	TextEditor::Palette& palette = *getPtr<TextEditor::Palette>(L, "ImGuiTextEditorPalette", 2);
	editor->SetPalette(palette);
	return 0;
}

int TE_SetPaletteColor(lua_State* L)
{
	STACK_CHECKER(L, "setPaletteColor", 0);

	int i = luaL_checkinteger(L, 2);
	ImU32 color = GColor::toU32(L, 3);
	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	editor->SetPaletteColor(i, color);
	return 0;
}

int TE_GetPaletteColor(lua_State* L)
{
	STACK_CHECKER(L, "getPaletteColor", 2);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	int i = luaL_checkinteger(L, 2);
	ImU32 color = editor->GetPaletteColor(i);
	GColor converted(color);
	lua_pushinteger(L, converted.hex);
	lua_pushnumber(L, converted.alpha);
	return 2;
}

int TE_GetPalette(lua_State* L)
{
	STACK_CHECKER(L, "getPalette", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	g_pushInstance(L, "ImGuiTextEditorPalette", const_cast<TextEditor::Palette*>(&(editor->GetPalette())));

	return 1;
}

int TE_SetErrorMarkers(lua_State* L)
{
	STACK_CHECKER(L, "setErrorMarkers", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	TextEditor::ErrorMarkers& markers = *getPtr<TextEditor::ErrorMarkers>(L, "ImGuiErrorMarkers", 2);
	editor->SetErrorMarkers(markers);
	return 0;
}

int TE_SetBreakpoints(lua_State* L)
{
	STACK_CHECKER(L, "setBreakpoints", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	TextEditor::Breakpoints& points = *getPtr<TextEditor::Breakpoints>(L, "ImGuiBreakpoints", 2);
	editor->SetBreakpoints(points);
	return 0;
}

int TE_Render(lua_State* L)
{
	STACK_CHECKER(L, "render", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	const char* title = luaL_checkstring(L, 2);
	ImVec2 size = luaL_optvec2(L, 3);
	bool border = luaL_optboolean(L, 5, 0);
	editor->Render(title, size, border);
	return 0;
}

int TE_SetText(lua_State* L)
{
	STACK_CHECKER(L, "setText", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	const char* buf = luaL_checkstring(L, 2);
	std::string text(buf);
	editor->SetText(text);
	return 0;
}

int TE_GetText(lua_State* L)
{
	STACK_CHECKER(L, "getText", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	std::string text = editor->GetText();
	lua_pushlstring(L, text.c_str(), text.size());
	return 1;
}

int TE_SetTextLines(lua_State* L)
{
	STACK_CHECKER(L, "setTextLines", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	luaL_checktype(L, 2, LUA_TTABLE);
	int len = luaL_getn(L, 2);
	std::vector<std::string> lines;

	for (int i = 0; i < len; i++)
	{
		lua_rawgeti(L, 2, i + 1);
		std::string line(luaL_checkstring(L, -1));
		lua_pop(L, 1);
		lines.push_back(line);
	}

	editor->SetTextLines(lines);
	return 0;
}

int TE_GetTextLines(lua_State* L)
{
	STACK_CHECKER(L, "getTextLines", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	std::vector<std::string> lines = editor->GetTextLines();

	lua_createtable(L, lines.size(), 0);

	for (size_t i = 0; i < lines.size(); i++)
	{
		lua_pushstring(L, lines[i].c_str());
		lua_rawseti(L, -2, i + 1);
	}

	return 1;
}

int TE_GetSelectedText(lua_State* L)
{
	STACK_CHECKER(L, "getSelectedText", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	std::string text = editor->GetSelectedText();
	lua_pushlstring(L, text.c_str(), text.size());
	return 1;
}

int TE_GetCurrentLineText(lua_State* L)
{
	STACK_CHECKER(L, "getCurrentLineText", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	std::string text = editor->GetCurrentLineText();
	lua_pushlstring(L, text.c_str(), text.size());
	return 1;
}

int TE_GetTotalLines(lua_State* L)
{
	STACK_CHECKER(L, "getTotalLines", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	lua_pushinteger(L, editor->GetTotalLines());
	return 1;
}

int TE_IsOverwrite(lua_State* L)
{
	STACK_CHECKER(L, "isOverwrite", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	lua_pushboolean(L, editor->IsOverwrite());
	return 1;
}

int TE_SetReadOnly(lua_State* L)
{
	STACK_CHECKER(L, "setReadOnly", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	editor->SetReadOnly(lua_toboolean(L, 2));
	return 0;
}

int TE_IsReadOnly(lua_State* L)
{
	STACK_CHECKER(L, "isReadOnly", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	lua_pushboolean(L, editor->IsReadOnly());
	return 1;
}

int TE_IsTextChanged(lua_State* L)
{
	STACK_CHECKER(L, "isTextChanged", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	lua_pushboolean(L, editor->IsTextChanged());
	return 1;
}

int TE_IsCursorPositionChanged(lua_State* L)
{
	STACK_CHECKER(L, "isCursorPositionChanged", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	lua_pushboolean(L, editor->IsCursorPositionChanged());
	return 1;
}

int TE_IsColorizerEnabled(lua_State* L)
{
	STACK_CHECKER(L, "isColorizerEnabled", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	lua_pushboolean(L, editor->IsColorizerEnabled());
	return 1;
}

int TE_SetColorizerEnable(lua_State* L)
{
	STACK_CHECKER(L, "setColorizerEnable", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	editor->SetColorizerEnable(lua_toboolean(L, 2));
	return 0;
}

int TE_GetCursorPosition(lua_State* L)
{
	STACK_CHECKER(L, "getCursorPosition", 2);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	TextEditor::Coordinates coord = editor->GetCursorPosition();
	lua_pushinteger(L, coord.mLine);
	lua_pushinteger(L, coord.mColumn);
	return 2;
}

int TE_SetCursorPosition(lua_State* L)
{
	STACK_CHECKER(L, "setCursorPosition", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	int line = luaL_checkinteger(L, 2);
	int column = luaL_checkinteger(L, 3);
	TextEditor::Coordinates coord(line, column);
	editor->SetCursorPosition(coord);
	return 0;
}

int TE_SetHandleMouseInputs(lua_State* L)
{
	STACK_CHECKER(L, "setHandleMouseInputs", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	editor->SetHandleMouseInputs(lua_toboolean(L, 2));
	return 0;
}

int TE_IsHandleMouseInputsEnabled(lua_State* L)
{
	STACK_CHECKER(L, "isHandleMouseInputsEnabled", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	lua_pushboolean(L, editor->IsHandleMouseInputsEnabled());
	return 1;
}

int TE_SetHandleKeyboardInputs(lua_State* L)
{
	STACK_CHECKER(L, "setHandleKeyboardInputs", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	editor->SetHandleKeyboardInputs(lua_toboolean(L, 2));
	return 0;
}

int TE_IsHandleKeyboardInputsEnabled(lua_State* L)
{
	STACK_CHECKER(L, "isHandleKeyboardInputsEnabled", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	lua_pushboolean(L, editor->IsHandleKeyboardInputsEnabled());
	return 1;
}

int TE_SetImGuiChildIgnored(lua_State* L)
{
	STACK_CHECKER(L, "setImGuiChildIgnored", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	editor->SetImGuiChildIgnored(lua_toboolean(L, 2));
	return 0;
}

int TE_IsImGuiChildIgnored(lua_State* L)
{
	STACK_CHECKER(L, "isImGuiChildIgnored", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	lua_pushboolean(L, editor->IsImGuiChildIgnored());
	return 1;
}

int TE_SetShowWhitespaces(lua_State* L)
{
	STACK_CHECKER(L, "setShowWhitespaces", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	editor->SetShowWhitespaces(lua_toboolean(L, 2));
	return 0;
}

int TE_IsShowingWhitespaces(lua_State* L)
{
	STACK_CHECKER(L, "isShowingWhitespaces", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	lua_pushboolean(L, editor->IsShowingWhitespaces());
	return 1;
}

int TE_SetTabSize(lua_State* L)
{
	STACK_CHECKER(L, "setTabSize", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	int size = luaL_checknumber(L, 2);
	editor->SetTabSize(size);
	return 0;
}

int TE_GetTabSize(lua_State* L)
{
	STACK_CHECKER(L, "getTabSize", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	lua_pushnumber(L, editor->GetTabSize());
	return 1;
}

int TE_InsertText(lua_State* L)
{
	STACK_CHECKER(L, "insertText", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	const char* text = luaL_checkstring(L, 2);
	editor->InsertText(text);
	return 0;
}

int TE_MoveUp(lua_State* L)
{
	STACK_CHECKER(L, "moveUp", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	int amount = luaL_optinteger(L, 2, 1);
	bool select = luaL_optboolean(L, 3, 0);
	editor->MoveUp(amount, select);
	return 0;
}

int TE_MoveDown(lua_State* L)
{
	STACK_CHECKER(L, "moveDown", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	int amount = luaL_optinteger(L, 2, 1);
	bool select = luaL_optboolean(L, 3, 0);
	editor->MoveDown(amount, select);
	return 0;
}

int TE_MoveLeft(lua_State* L)
{
	STACK_CHECKER(L, "moveLeft", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	int amount = luaL_optinteger(L, 2, 1);
	bool select = luaL_optboolean(L, 3, 0);
	editor->MoveLeft(amount, select);
	return 0;
}

int TE_MoveRight(lua_State* L)
{
	STACK_CHECKER(L, "moveRight", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	int amount = luaL_optinteger(L, 2, 1);
	bool select = luaL_optboolean(L, 3, 0);
	editor->MoveRight(amount, select);
	return 0;
}

int TE_MoveTop(lua_State* L)
{
	STACK_CHECKER(L, "moveTop", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	bool select = luaL_optboolean(L, 2, 0);
	editor->MoveTop(select);
	return 0;
}

int TE_MoveBottom(lua_State* L)
{
	STACK_CHECKER(L, "moveBottom", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	bool select = luaL_optboolean(L, 2, 0);
	editor->MoveBottom(select);
	return 0;
}

int TE_MoveHome(lua_State* L)
{
	STACK_CHECKER(L, "moveHome", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	bool select = luaL_optboolean(L, 2, 0);
	editor->MoveHome(select);
	return 0;
}

int TE_MoveEnd(lua_State* L)
{
	STACK_CHECKER(L, "moveEnd", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	bool select = luaL_optboolean(L, 2, 0);
	editor->MoveEnd(select);
	return 0;
}

int TE_SetSelectionStart(lua_State* L)
{
	STACK_CHECKER(L, "setSelectionStart", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	int line = luaL_checkinteger(L, 2);
	int column = luaL_checkinteger(L, 3);
	TextEditor::Coordinates pos(line, column);
	editor->SetSelectionStart(pos);
	return 0;
}

int TE_SetSelectionEnd(lua_State* L)
{
	STACK_CHECKER(L, "setSelectionEnd", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	int line = luaL_checkinteger(L, 2);
	int column = luaL_checkinteger(L, 3);
	TextEditor::Coordinates pos(line, column);
	editor->SetSelectionEnd(pos);
	return 0;
}

int TE_SetSelection(lua_State* L)
{
	STACK_CHECKER(L, "setSelection", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	TextEditor::Coordinates posStart(luaL_checkinteger(L, 2), luaL_checkinteger(L, 3));
	TextEditor::Coordinates posEnd(luaL_checkinteger(L, 4), luaL_checkinteger(L, 5));
	TextEditor::SelectionMode mode = (TextEditor::SelectionMode)luaL_optinteger(L, 6, 0);
	editor->SetSelection(posStart, posEnd, mode);
	return 0;
}

int TE_SelectWordUnderCursor(lua_State* L)
{
	STACK_CHECKER(L, "selectWordUnderCursor", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	editor->SelectWordUnderCursor();
	return 0;
}

int TE_SelectAll(lua_State* L)
{
	STACK_CHECKER(L, "selectAll", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	editor->SelectAll();
	return 0;
}

int TE_HasSelection(lua_State* L)
{
	STACK_CHECKER(L, "hasSelection", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	lua_pushboolean(L, editor->HasSelection());
	return 1;
}

int TE_Copy(lua_State* L)
{
	STACK_CHECKER(L, "copy", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	editor->Copy();
	return 0;
}

int TE_Cut(lua_State* L)
{
	STACK_CHECKER(L, "cut", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	editor->Cut();
	return 0;
}

int TE_Paste(lua_State* L)
{
	STACK_CHECKER(L, "paste", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	editor->Paste();
	return 0;
}

int TE_Delete(lua_State* L)
{
	STACK_CHECKER(L, "delete", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	editor->Delete();
	return 0;
}

int TE_CanUndo(lua_State* L)
{
	STACK_CHECKER(L, "canUndo", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	lua_pushboolean(L, editor->CanUndo());
	return 1;
}

int TE_CanRedo(lua_State* L)
{
	STACK_CHECKER(L, "canRedo", 1);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	lua_pushboolean(L, editor->CanRedo());
	return 1;
}

int TE_Undo(lua_State* L)
{
	STACK_CHECKER(L, "undo", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	int steps = luaL_optinteger(L, 2, 1);
	editor->Undo(steps);
	return 0;
}

int TE_Redo(lua_State* L)
{
	STACK_CHECKER(L, "redo", 0);

	TextEditor* editor = getPtr<TextEditor>(L, "ImGuiTextEditor");
	int steps = luaL_optinteger(L, 2, 1);
	editor->Redo(steps);
	return 0;
}


int initErrorMarkers(lua_State* L)
{
	TextEditor::ErrorMarkers* markers = new TextEditor::ErrorMarkers();
	g_pushInstance(L, "ImGuiErrorMarkers", markers);

	luaL_rawgetptr(L, LUA_REGISTRYINDEX, &keyWeak);
	lua_pushvalue(L, -2);
	luaL_rawsetptr(L, -2, markers);
	lua_pop(L, 1);

	return 1;
}

int destroyErrorMarkers(LUA_STATE* p)
{
	destroyObject<TextEditor::ErrorMarkers>(p);
	return 0;
}

int EM_MAdd(lua_State* L)
{
	STACK_CHECKER(L, "add", 0);

	TextEditor::ErrorMarkers* markers = getPtr<TextEditor::ErrorMarkers>(L, "ImGuiErrorMarkers");
	int lineNumber = luaL_checkinteger(L, 2);
	std::string message(luaL_checkstring(L, 3));
	(*markers)[lineNumber] = message;
	return 0;
}

int EM_MRemove(lua_State* L)
{
	STACK_CHECKER(L, "remove", 0);

	TextEditor::ErrorMarkers* markers = getPtr<TextEditor::ErrorMarkers>(L, "ImGuiErrorMarkers");
	int lineNumber = luaL_checkinteger(L, 2);
	markers->erase(lineNumber);
	return 0;
}

int EM_MGet(lua_State* L)
{
	STACK_CHECKER(L, "get", 1);

	TextEditor::ErrorMarkers* markers = getPtr<TextEditor::ErrorMarkers>(L, "ImGuiErrorMarkers");
	int lineNumber = luaL_checkinteger(L, 2);
	TextEditor::ErrorMarkers::iterator it = markers->find(lineNumber);
	if (it == markers->end())
	{
		lua_pushnil(L);
		return 1;
	}
	else
	{
		lua_pushnumber(L, (*it).first);
		lua_pushstring(L, (*it).second.c_str());
		return 2;
	}
}

int EM_MSize(lua_State* L)
{
	STACK_CHECKER(L, "getSize", 1);

	TextEditor::ErrorMarkers* markers = getPtr<TextEditor::ErrorMarkers>(L, "ImGuiErrorMarkers");
	lua_pushnumber(L, markers->size());
	return 1;
}


int initBreakpoints(lua_State* L)
{
	TextEditor::Breakpoints* points = new TextEditor::Breakpoints();
	g_pushInstance(L, "ImGuiBreakpoints", points);

	luaL_rawgetptr(L, LUA_REGISTRYINDEX, &keyWeak);
	lua_pushvalue(L, -2);
	luaL_rawsetptr(L, -2, points);
	lua_pop(L, 1);

	return 1;
}

int destroyBreakpoints(LUA_STATE* p)
{
	destroyObject<TextEditor::Breakpoints>(p);
	return 0;
}

int EM_BAdd(lua_State* L)
{
	STACK_CHECKER(L, "add", 0);

	TextEditor::Breakpoints* points = getPtr<TextEditor::Breakpoints>(L, "ImGuiBreakpoints");
	int lineNumber = luaL_checkinteger(L, 2);
	points->insert(lineNumber);
	return 0;
}

int EM_BRemove(lua_State* L)
{
	STACK_CHECKER(L, "remove", 0);

	TextEditor::Breakpoints* points = getPtr<TextEditor::Breakpoints>(L, "ImGuiBreakpoints");
	int lineNumber = luaL_checkinteger(L, 2);
	points->erase(lineNumber);
	return 0;
}

int EM_BGet(lua_State* L)
{
	STACK_CHECKER(L, "get", 1);

	TextEditor::Breakpoints* points = getPtr<TextEditor::Breakpoints>(L, "ImGuiBreakpoints");
	int lineNumber = luaL_checkinteger(L, 2);
	TextEditor::Breakpoints::iterator it = points->find(lineNumber);
	it == points->end() ? lua_pushnil(L) : lua_pushinteger(L, *it);
	return 1;
}

int EM_BSize(lua_State* L)
{
	STACK_CHECKER(L, "getSize", 1);

	TextEditor::Breakpoints* points = getPtr<TextEditor::Breakpoints>(L, "ImGuiBreakpoints");
	lua_pushnumber(L, points->size());
	return 1;
}

////////////////////////////////////////////////////////////////////

int LyaoutBeginHorizontal(lua_State* L)
{	ImVec2 size = luaL_optvec2(L, 3);
	float align = luaL_optnumber(L, 5, -1.0f);

	if (lua_type(L, 2) == LUA_TNUMBER)
	{
		ImGuiID id = checkID(L, 2);
		int uid = (int)id;
		ImGui::BeginHorizontal(uid, size, align);
	}
	else
	{
		const char* str_id = luaL_checkstring(L, 2);
		ImGui::BeginHorizontal(str_id, size, align);
	}
	return 0;
}

int LyaoutEndHorizontal(lua_State* L)
{
	ImGui::EndHorizontal();
	return 0;
}

int LyaoutBeginVertical(lua_State* L)
{
	ImVec2 size = luaL_optvec2(L, 3);
	float align = luaL_optnumber(L, 5, -1.0f);

	if (lua_type(L, 2) == LUA_TNUMBER)
	{
		ImGuiID id = checkID(L, 2);
		int uid = (int)id;
		ImGui::BeginVertical(uid, size, align);
	}
	else
	{
		const char* str_id = luaL_checkstring(L, 2);
		ImGui::BeginVertical(str_id, size, align);
	}

	return 0;
}

int LyaoutEndVertical(lua_State* L)
{
	ImGui::EndVertical();
	return 0;
}

int LyaoutSpring(lua_State* L)
{
	float weight = luaL_optnumber(L, 2, 1.0f);
	float spacing = luaL_optnumber(L, 3, -1.0f);
	ImGui::Spring(weight, spacing);
	return 0;
}

int LyaoutSuspendLayout(lua_State* L)
{
	ImGui::SuspendLayout();
	return 0;
}

int LyaoutResumeLayout(lua_State* L)
{
	ImGui::ResumeLayout();
	return 0;
}


////////////////////////////////////////////////////////////////////

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void DrawLuaStyleEditor(const char* title, bool* p_open = NULL, ImGuiWindowFlags flags = ImGuiWindowFlags_None)
{
	if (!ImGui::Begin(title, p_open, flags))
	{
		ImGui::End();
		return;
	}

	ImGuiStyle& style = ImGui::GetStyle();
	static ImGuiStyle ref_saved_style;
	static ImGuiStyle* ref;

	// Default to using internal storage as reference
	static bool init = true;
	if (init && ref == NULL)
		ref_saved_style = style;
	init = false;
	if (ref == NULL)
		ref = &ref_saved_style;

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

	if (ImGui::ShowStyleSelector("Colors##Selector"))
		ref_saved_style = style;
	ImGui::ShowFontSelector("Fonts##Selector");

	// Simplified Settings (expose floating-pointer border sizes as boolean representing 0.0f or 1.0f)
	if (ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
		style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
	{ bool border = (style.WindowBorderSize > 0.0f); if (ImGui::Checkbox("WindowBorder", &border)) { style.WindowBorderSize = border ? 1.0f : 0.0f; } }
	ImGui::SameLine();
	{ bool border = (style.FrameBorderSize > 0.0f);  if (ImGui::Checkbox("FrameBorder",  &border)) { style.FrameBorderSize  = border ? 1.0f : 0.0f; } }
	ImGui::SameLine();
	{ bool border = (style.PopupBorderSize > 0.0f);  if (ImGui::Checkbox("PopupBorder",  &border)) { style.PopupBorderSize  = border ? 1.0f : 0.0f; } }

	static int output_dest = 0;

	// Save/Revert button
	if (ImGui::Button("Save Ref"))
		*ref = ref_saved_style = style;
	ImGui::SameLine();
	if (ImGui::Button("Revert Ref"))
		style =* ref;

	static bool output_only_modified = true;

	if (ImGui::Button("Export"))
	{
		if (output_dest == 0)
			ImGui::LogToClipboard();
		else
			ImGui::LogToTTY();
		ImGui::LogText("%s", "local style = imgui:getStyle()\r\n");
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			const ImVec4& col = style.Colors[i];
			const char* name = ImGui::GetStyleColorName(i);
			GColor gcolor = GColor::toHex(col);
			if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0)
				ImGui::LogText("style:setColor(ImGui.Col_%s, 0x%06X, %.2f)\r\n", name, gcolor.hex, gcolor.alpha);
		}
		ImGui::LogFinish();
	}


	ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
	ImGui::SameLine(); ImGui::Checkbox("Only Modified Colors", &output_only_modified);

	static ImGuiTextFilter filter;
	filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

	static ImGuiColorEditFlags alpha_flags = 0;
	if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None))             { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
	if (ImGui::RadioButton("Alpha",  alpha_flags == ImGuiColorEditFlags_AlphaPreview))     { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
	if (ImGui::RadioButton("Both",   alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
	HelpMarker(
				"In the color list:\n"
				"Left-click on colored square to open color picker,\n"
				"Right-click to open edit options menu.");

	ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
	ImGui::PushItemWidth(-160);
	for (int i = 0; i < ImGuiCol_COUNT; i++)
	{
		const char* name = ImGui::GetStyleColorName(i);
		if (!filter.PassFilter(name))
			continue;
		ImGui::PushID(i);
		ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
		if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
		{
			// Tips: in a real user application, you may want to merge and use an icon font into the main font,
			// so instead of "Save"/"Revert" you'd use icons!
			// Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
			ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) { ref->Colors[i] = style.Colors[i]; }
			ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { style.Colors[i] = ref->Colors[i]; }
		}
		ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
		ImGui::TextUnformatted(name);
		ImGui::PopID();
	}
	ImGui::PopItemWidth();
	ImGui::EndChild();

	ImGui::End();
}

int ShowLuaStyleEditor(lua_State* L)
{
	STACK_CHECKER(L, "showLuaStyleEditor", 1);

	const char* title = luaL_checkstring(L, 2);

	ImGuiWindowFlags window_flags = luaL_optinteger(L, 4, ImGuiWindowFlags_None);

	int type = lua_type(L, 3);
	if (type == LUA_TBOOLEAN)
	{
		bool p_open = lua_toboolean(L, 3);
		DrawLuaStyleEditor(title, &p_open, window_flags);
		lua_pushboolean(L, p_open);
		return 1;
	}
	else
	{
		DrawLuaStyleEditor(title, NULL, window_flags);
		return 0;
	}
}

struct ExampleAppLog
{
	ImGuiTextBuffer     Buf;
	ImGuiTextFilter     Filter;
	ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
	bool                AutoScroll;  // Keep scrolling if already at the bottom.
	bool                Shown;

	ExampleAppLog()
	{
		AutoScroll = true;
		Clear();
	}

	void    Clear()
	{
		Buf.clear();
		LineOffsets.clear();
		LineOffsets.push_back(0);
	}

	void    AddLog(const char* fmt, ...) //IM_FMTARGS(2)
	{
		int old_size = Buf.size();
		va_list args;
		va_start(args, fmt);
		Buf.appendfv(fmt, args);
		va_end(args);
		for (int new_size = Buf.size(); old_size < new_size; old_size++)
			if (Buf[old_size] == '\n')
				LineOffsets.push_back(old_size + 1);
	}

	void    Draw(const char* title, bool* p_open = NULL, ImGuiWindowFlags flags = ImGuiWindowFlags_None)
	{
		if (!ImGui::Begin(title, p_open, flags))
		{
			ImGui::End();
			return;
		}

		// Options menu
		if (ImGui::BeginPopup("Options"))
		{
			ImGui::Checkbox("Auto-scroll", &AutoScroll);
			ImGui::EndPopup();
		}

		// Main window
		if (ImGui::Button("Options"))
			ImGui::OpenPopup("Options");
		ImGui::SameLine();
		bool clear = ImGui::Button("Clear");
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		Filter.Draw("Filter", -100.0f);
		ImGui::SameLine();
		if (ImGui::Button("X"))
			Filter.Clear();

		ImGui::Separator();
		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		if (clear)
			Clear();
		if (copy)
			ImGui::LogToClipboard();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		const char* buf = Buf.begin();
		const char* buf_end = Buf.end();
		if (Filter.IsActive())
		{
			// In this example we don't use the clipper when Filter is enabled.
			// This is because we don't have a random access on the result on our filter.
			// A real application processing logs with ten of thousands of entries may want to store the result of
			// search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
			for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
			{
				const char* line_start = buf + LineOffsets[line_no];
				const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
				if (Filter.PassFilter(line_start, line_end))
					ImGui::TextUnformatted(line_start, line_end);
			}
		}
		else
		{
			// The simplest and easy way to display the entire buffer:
			//   ImGui::TextUnformatted(buf_begin, buf_end);
			// And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
			// to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
			// within the visible area.
			// If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
			// on your side is recommended. Using ImGuiListClipper requires
			// - A) random access into your data
			// - B) items all being the  same height,
			// both of which we can handle since we an array pointing to the beginning of each line of text.
			// When using the filter (in the block of code above) we don't have random access into the data to display
			// anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
			// it possible (and would be recommended if you want to search through tens of thousands of entries).
			ImGuiListClipper clipper;
			clipper.Begin(LineOffsets.Size);
			while (clipper.Step())
			{
				for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
				{
					const char* line_start = buf + LineOffsets[line_no];
					const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
					ImGui::TextUnformatted(line_start, line_end);
				}
			}
			clipper.End();
		}
		ImGui::PopStyleVar();

		if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();
		ImGui::End();
	}
};

static ExampleAppLog logapp;

int ShowLog(lua_State* L)
{
	const char* title = luaL_checkstring(L, 2);
	ImGuiWindowFlags window_flags = luaL_optinteger(L, 4, ImGuiWindowFlags_None);

	if (lua_isnoneornil(L, 3))
	{
		STACK_CHECKER(L, "showLog", 0);
		logapp.Draw(title, NULL, window_flags);
		logapp.Shown = true;
		return 0;
	}
	STACK_CHECKER(L, "showLog", 1);
	bool p_open = lua_toboolean(L, 3);
	logapp.Draw(title, &p_open, window_flags);
	logapp.Shown = p_open;
	lua_pushboolean(L, p_open);
	return 1;
}

int WriteLog(lua_State* L)
{
	STACK_CHECKER(L, "writeLog", 0);

	if (!logapp.Shown)
		return 0;

	const char* text = luaL_checkstring(L, 2);
	logapp.AddLog("%s\n", text);

	return 0;
}

int HelpMarker(lua_State* L)
{
	STACK_CHECKER(L, "helpMarker", 0);

	const char* message = luaL_checkstring(L, 2);
	HelpMarker(message);
	return 0;
}

int loader(lua_State* L)
{
	const luaL_Reg imguiTextFilterFunctionsList[] = {
		{"passFilter", TextFilter_PassFilter},
		{"clear", TextFilter_Clear},
		{"draw", TextFilter_Draw},
		{NULL, NULL}
	};

	g_createClass(L, "ImGuiTextFilter", NULL, initImGuiTextFilter, destroyImGuiTextFilter, imguiTextFilterFunctionsList);

	const luaL_Reg imguiEmptyFunctionsList[] = {
		{NULL, NULL}
	};

	g_createClass(L, "ImGuiCallbackData", NULL, NULL, NULL, imguiEmptyFunctionsList);

	const luaL_Reg imguiStylesFunctionList[] =
	{
		{"setColor", Style_SetColor},
		{"getColor", Style_GetColor},
		{"setAlpha", Style_SetAlpha},
		{"getAlpha", Style_GetAlpha},
		{"setWindowRounding", Style_SetWindowRounding},
		{"getWindowRounding", Style_GetWindowRounding},
		{"setWindowBorderSize", Style_SetWindowBorderSize},
		{"getWindowBorderSize", Style_GetWindowBorderSize},
		{"setChildRounding", Style_SetChildRounding},
		{"getChildRounding", Style_GetChildRounding},
		{"setChildBorderSize", Style_SetChildBorderSize},
		{"getChildBorderSize", Style_GetChildBorderSize},
		{"setPopupRounding", Style_SetPopupRounding},
		{"getPopupRounding", Style_GetPopupRounding},
		{"setPopupBorderSize", Style_SetPopupBorderSize},
		{"getPopupBorderSize", Style_GetPopupBorderSize},
		{"setFrameRounding", Style_SetFrameRounding},
		{"getFrameRounding", Style_GetFrameRounding},
		{"setFrameBorderSize", Style_SetFrameBorderSize},
		{"getFrameBorderSize", Style_GetFrameBorderSize},
		{"setIndentSpacing", Style_SetIndentSpacing},
		{"getIndentSpacing", Style_GetIndentSpacing},
		{"setColumnsMinSpacing", Style_SetColumnsMinSpacing},
		{"getColumnsMinSpacing", Style_GetColumnsMinSpacing},
		{"setScrollbarSize", Style_SetScrollbarSize},
		{"getScrollbarSize", Style_GetScrollbarSize},
		{"setScrollbarRounding", Style_SetScrollbarRounding},
		{"getScrollbarRounding", Style_GetScrollbarRounding},
		{"setGrabMinSize", Style_SetGrabMinSize},
		{"getGrabMinSize", Style_GetGrabMinSize},
		{"setGrabRounding", Style_SetGrabRounding},
		{"getGrabRounding", Style_GetGrabRounding},
		{"setLogSliderDeadzone", Style_SetLogSliderDeadzone},
		{"getLogSliderDeadzone", Style_GetLogSliderDeadzone},
		{"setTabRounding", Style_SetTabRounding},
		{"getTabRounding", Style_GetTabRounding},
		{"setTabBorderSize", Style_SetTabBorderSize},
		{"getTabBorderSize", Style_GetTabBorderSize},
		{"setTabMinWidthForCloseButton", Style_SetTabMinWidthForCloseButton},
		{"getTabMinWidthForCloseButton", Style_GetTabMinWidthForCloseButton},
		{"setMouseCursorScale", Style_SetMouseCursorScale},
		{"getMouseCursorScale", Style_GetMouseCursorScale},
		{"setCurveTessellationTol", Style_SetCurveTessellationTol},
		{"getCurveTessellationTol", Style_GetCurveTessellationTol},
		{"setCircleTessellationMaxError", Style_SetCircleTessellationMaxError},
		{"getCircleTessellationMaxError", Style_GetCircleTessellationMaxError},
		{"setWindowPadding", Style_SetWindowPadding},
		{"getWindowPadding", Style_GetWindowPadding},
		{"setWindowMinSize", Style_SetWindowMinSize},
		{"getWindowMinSize", Style_GetWindowMinSize},
		{"setWindowTitleAlign", Style_SetWindowTitleAlign},
		{"getWindowTitleAlign", Style_GetWindowTitleAlign},
		{"setFramePadding", Style_SetFramePadding},
		{"getFramePadding", Style_GetFramePadding},
		{"setItemSpacing", Style_SetItemSpacing},
		{"getItemSpacing", Style_GetItemSpacing},
		{"setItemInnerSpacing", Style_SetItemInnerSpacing},
		{"getItemInnerSpacing", Style_GetItemInnerSpacing},
		{"setTouchExtraPadding", Style_SetTouchExtraPadding},
		{"getTouchExtraPadding", Style_GetTouchExtraPadding},
		{"setButtonTextAlign", Style_SetButtonTextAlign},
		{"getButtonTextAlign", Style_GetButtonTextAlign},
		{"setSelectableTextAlign", Style_SetSelectableTextAlign},
		{"getSelectableTextAlign", Style_GetSelectableTextAlign},
		{"setDisplayWindowPadding", Style_SetDisplayWindowPadding},
		{"getDisplayWindowPadding", Style_GetDisplayWindowPadding},
		{"setDisplaySafeAreaPadding", Style_SetDisplaySafeAreaPadding},
		{"getDisplaySafeAreaPadding", Style_GetDisplaySafeAreaPadding},
		{"setWindowMenuButtonPosition", Style_SetWindowMenuButtonPosition},
		{"getWindowMenuButtonPosition", Style_GetWindowMenuButtonPosition},
		{"setColorButtonPosition", Style_SetColorButtonPosition},
		{"getColorButtonPosition", Style_GetColorButtonPosition},
		{"setAntiAliasedLines", Style_SetAntiAliasedLines},
		{"getAntiAliasedLines", Style_GetAntiAliasedLines},
		{"setAntiAliasedLinesUseTex", Style_SetAntiAliasedLinesUseTex},
		{"getAntiAliasedLinesUseTex", Style_GetAntiAliasedLinesUseTex},
		{"setAntiAliasedFill", Style_SetAntiAliasedFill},
		{"getAntiAliasedFill", Style_GetAntiAliasedFill},
		{"setDisabledAlpha", Style_SetDisabledAlpha},
		{"getDisabledAlpha", Style_GetDisabledAlpha},
		{"setCellPadding", Style_SetCellPadding},
		{"getCellPadding", Style_GetCellPadding},

		{NULL, NULL},
	};

	g_createClass(L, "ImGuiStyle", 0, NULL, NULL, imguiStylesFunctionList);

	const luaL_Reg imguiDrawListFunctionList[] =
	{
		{"getFont", DrawList_GetFont},
		{"getFontSize", DrawList_GetFontSize},
		{"getCurveTessellationTol", DrawList_GetCurveTessellationTol},
		{"getCircleSegmentMaxError", DrawList_GetCircleSegmentMaxError},

		{"pushClipRect", DrawList_PushClipRect},
		{"pushClipRectFullScreen", DrawList_PushClipRectFullScreen},
		{"popClipRect", DrawList_PopClipRect},
		{"pushTextureID", DrawList_PushTextureID},
		{"popTextureID", DrawList_PopTextureID},
		{"getClipRectMin", DrawList_GetClipRectMin},
		{"getClipRectMax", DrawList_GetClipRectMax},
		{"addLine", DrawList_AddLine},
		{"addRect", DrawList_AddRect},
		{"addRectFilled", DrawList_AddRectFilled},
		{"addRectFilledMultiColor", DrawList_AddRectFilledMultiColor},
		{"addQuad", DrawList_AddQuad},
		{"addQuadFilled", DrawList_AddQuadFilled},
		{"addTriangle", DrawList_AddTriangle},
		{"addTriangleFilled", DrawList_AddTriangleFilled},
		{"addCircle", DrawList_AddCircle},
		{"addCircleFilled", DrawList_AddCircleFilled},
		{"addNgon", DrawList_AddNgon},
		{"addNgonFilled", DrawList_AddNgonFilled},
		{"addText", DrawList_AddText},
		{"addFontText", DrawList_AddFontText},
		{"addPolyline", DrawList_AddPolyline},
		{"addConvexPolyFilled", DrawList_AddConvexPolyFilled},
		{"addBezierCubic", DrawList_AddBezierCubic},
		{"addBezierQuadratic", DrawList_AddBezierQuadratic},

		{"addImageQuad", DrawList_AddImageQuad},
		{"addImage", DrawList_AddImage},
		{"addImageUV", DrawList_AddImageUV},
		{"addImageRounded", DrawList_AddImageRounded},
		{"addImageRoundedUV", DrawList_AddImageRoundedUV},
		{"addScaledImage", DrawList_AddScaledImage},
		{"addScaledImageRounded", DrawList_AddScaledImageRounded},
		{"pathClear", DrawList_PathClear},
		{"pathLineTo", DrawList_PathLineTo},
		{"pathLineToMergeDuplicate", DrawList_PathLineToMergeDuplicate},
		{"pathFillConvex", DrawList_PathFillConvex},
		{"pathStroke", DrawList_PathStroke},
		{"pathArcTo", DrawList_PathArcTo},
		{"pathArcToFast", DrawList_PathArcToFast},
		{"pathBezierCubicCurveTo", DrawList_PathBezierCubicCurveTo},
		{"pathBezierQuadraticCurveTo", DrawList_PathBezierQuadraticCurveTo},
		{"pathRect", DrawList_PathRect},

		{"rotateBegin", DrawList_RotateStart},
		{"rotateEnd", DrawList_RotateEnd},
		{NULL, NULL}
	};
	g_createClass(L, "ImDrawList", 0, NULL, NULL, imguiDrawListFunctionList);

	const luaL_Reg imguiIoFunctionList[] =
	{
		{"setFontDefault", IO_SetFontDefault},
		{"getDefaultFont", IO_GetDefaultFont},
		{"getFonts", IO_GetFonts},

		{"getDeltaTime", IO_GetDeltaTime},
		{"isMouseDown", IO_isMouseDown},
		{"getMouseWheel", IO_GetMouseWheel},
		{"getMouseWheelH", IO_GetMouseWheelH},
		{"setKeysDown", IO_SetKeysDown},
		{"setModKeysDown", IO_SetModKeyDown},
		{"resetKeysDown", IO_ResetKeysDown},

		{"wantCaptureMouse", IO_WantCaptureMouse},
		{"wantCaptureMouseUnlessPopupClose", IO_WantCaptureMouseUnlessPopupClose},
		{"wantCaptureKeyboard", IO_WantCaptureKeyboard},
		{"wantTextInput", IO_WantTextInput},
		{"wantSetMousePos", IO_WantSetMousePos},
		{"wantSaveIniSettings", IO_WantSaveIniSettings},

		/// 1.87 inputs
		{"addKeyEvent", IO_AddKeyEvent},
		{"addKeyAnalogEvent", IO_AddKeyAnalogEvent},
		{"addMousePosEvent", IO_AddMousePosEvent},
		{"addMouseButtonEvent", IO_AddMouseButtonEvent},
		{"addMouseWheelEvent", IO_AddMouseWheelEvent},

		/// NAVIGATION +
//		{"setNavInput", IO_SetNavInput},
//		{"getNavInput", IO_GetNavInput},
//		{"setNavInputsDownDuration", IO_SetNavInputsDownDuration},
//		{"getNavInputsDownDuration", IO_GetNavInputsDownDuration},
//		{"setNavInputsDownDurationPrev", IO_SetNavInputsDownDurationPrev},
//		{"getNavInputsDownDurationPrev", IO_GetNavInputsDownDurationPrev},
//		{"isNavActive", IO_IsNavActive},
//		{"isNavVisible", IO_IsNavVisible},
		/// NAVIGATION -
		{"getFramerate", IO_GetFramerate},
		{"getMetricsRenderVertices", IO_GetMetricsRenderVertices},
		{"getMetricsRenderIndices", IO_GetMetricsRenderIndices},
		{"getMetricsRenderWindows", IO_GetMetricsRenderWindows},
		{"getMetricsActiveWindows", IO_GetMetricsActiveWindows},
		{"getMetricsActiveAllocations", IO_GetMetricsActiveAllocations},
		{"getMouseDelta", IO_GetMouseDelta},
		{"getMouseDownSec", IO_GetMouseDownSec},
		{"setDisplaySize", IO_SetDisplaySize},
		{"getDisplaySize", IO_GetDisplaySize},

	#ifdef IS_DOCKING_BUILD
		{"setConfigDockingNoSplit", IO_GetConfigDockingNoSplit},
		{"setConfigDockingNoSplit", IO_SetConfigDockingNoSplit},
		{"setConfigDockingWithShift", IO_GetConfigDockingWithShift},
		{"setConfigDockingWithShift", IO_SetConfigDockingWithShift},
		{"setConfigDockingAlwaysTabBar", IO_GetConfigDockingAlwaysTabBar},
		{"setConfigDockingAlwaysTabBar", IO_SetConfigDockingAlwaysTabBar},
		{"setConfigDockingTransparentPayload", IO_GetConfigDockingTransparentPayload},
		{"setConfigDockingTransparentPayload", IO_SetConfigDockingTransparentPayload},
	#endif
		{"getConfigFlags", IO_GetConfigFlags},
		{"setConfigFlags", IO_SetConfigFlags},
		{"addConfigFlags", IO_AddConfigFlags},
		{"getBackendFlags", IO_GetBackendFlags},
		{"setBackendFlags", IO_SetBackendFlags},
		{"addBackendFlags", IO_AddBackendFlags},
		{"getIniSavingRate", IO_GetIniSavingRate},
		{"setIniSavingRate", IO_SetIniSavingRate},
		{"getIniFilename", IO_GetIniFilename},
		{"setIniFilename", IO_SetIniFilename},
		{"saveIniSettings", IO_SaveIniSettingsToDisk},
		{"loadIniSettings", IO_LoadIniSettingsFromDisk},
		{"getLogFilename", IO_GetLogFilename},
		{"setLogFilename", IO_SetLogFilename},
		{"getMouseDoubleClickTime", IO_GetMouseDoubleClickTime},
		{"setMouseDoubleClickTime", IO_SetMouseDoubleClickTime},
		{"getMouseDragThreshold", IO_GetMouseDragThreshold},
		{"setMouseDragThreshold", IO_SetMouseDragThreshold},
		{"getMouseDrawCursor", IO_GetMouseDrawCursor},
		{"setMouseDrawCursor", IO_SetMouseDrawCursor},
		{"getMouseDoubleClickMaxDist", IO_GetMouseDoubleClickMaxDist},
		{"setMouseDoubleClickMaxDist", IO_SetMouseDoubleClickMaxDist},
		{"setMouseDown", IO_SetMouseDown},
		{"resetMouseDown", IO_ResetMouseDown},
		{"setMousePos", IO_SetMousePos},
		{"setMouseWheel", IO_SetMouseWheel},
		{"getKeyRepeatDelay", IO_GetKeyRepeatDelay},
		{"setKeyRepeatDelay", IO_SetKeyRepeatDelay},
		{"getKeyRepeatRate", IO_GetKeyRepeatRate},
		{"setKeyRepeatRate", IO_SetKeyRepeatRate},
		{"getFontGlobalScale", IO_GetFontGlobalScale},
		{"setFontGlobalScale", IO_SetFontGlobalScale},
		{"getFontAllowUserScaling", IO_GetFontAllowUserScaling},
		{"setFontAllowUserScaling", IO_SetFontAllowUserScaling},
		{"getDisplayFramebufferScale", IO_GetDisplayFramebufferScale},
		{"setDisplayFramebufferScale", IO_SetDisplayFramebufferScale},
		{"getConfigMacOSXBehaviors", IO_GetConfigMacOSXBehaviors},
		{"setConfigMacOSXBehaviors", IO_SetConfigMacOSXBehaviors},
		{"getConfigInputTextCursorBlink", IO_GetConfigInputTextCursorBlink},
		{"setConfigInputTextCursorBlink", IO_SetConfigInputTextCursorBlink},
		{"getConfigDragClickToInputText", IO_GetConfigDragClickToInputText},
		{"setConfigDragClickToInputText", IO_SetConfigDragClickToInputText},
		{"getConfigWindowsResizeFromEdges", IO_GetConfigWindowsResizeFromEdges},
		{"setConfigWindowsResizeFromEdges", IO_SetConfigWindowsResizeFromEdges},
		{"getConfigWindowsMoveFromTitleBarOnly", IO_GetConfigWindowsMoveFromTitleBarOnly},
		{"setConfigWindowsMoveFromTitleBarOnly", IO_SetConfigWindowsMoveFromTitleBarOnly},
		{"getConfigWindowsMemoryCompactTimer", IO_GetConfigMemoryCompactTimer},
		{"setConfigWindowsMemoryCompactTimer", IO_SetConfigMemoryCompactTimer},

		{"addInputCharactersUTF8", IO_AddInputCharactersUTF8},

		{"getBackendPlatformName", IO_GetBackendPlatformName},
		{"getBackendRendererName", IO_GetBackendRendererName},

		{"getPenPressure", IO_GetPenPressure},

		{NULL, NULL}
	};
	g_createClass(L, "ImGuiIO", NULL, NULL, NULL, imguiIoFunctionList);

	const luaL_Reg imguiFontAtlasFunctionList[] =
	{
		{"addFont", FontAtlas_AddFont},
		{"addFonts", FontAtlas_AddFonts},
		{"getFont", FontAtlas_GetFontByIndex},
		{"getFonts", FontAtlas_GetFonts},
		{"getFontsCount", FontAtlas_GetFontsSize},
		{"getCurrentFont", FontAtlas_GetCurrentFont},
		{"addDefaultFont", FontAtlas_AddDefaultFont},
		{"build", FontAtlas_Build},
		//{"bake", FontAtlas_Bake},
		{"clearInputData", FontAtlas_ClearInputData},
		{"clearTexData", FontAtlas_ClearTexData},
		{"clearFonts", FontAtlas_ClearFonts},
		{"clear", FontAtlas_Clear},
		{"isBuilt", FontAtlas_IsBuilt},
		{"addCustomRectRegular", FontAtlas_AddCustomRectRegular},
		{"addCustomRectFontGlyph", FontAtlas_AddCustomRectFontGlyph},
		{"getCustomRectByIndex", FontAtlas_GetCustomRectByIndex},
		{NULL, NULL}
	};
	g_createClass(L, "ImFontAtlas", NULL, NULL, NULL, imguiFontAtlasFunctionList);

	const luaL_Reg imguiFontFunctionsList[] = {
		{"getSize", ImFont_GetFontSize },
		{"getContainerAtlas", ImFont_GetContainerAtlas },
		{"setScale", ImFont_SetScale },
		{"getScale", ImFont_GetScale },
		{"getAscent", ImFont_GetAscent },
		{"getDescent", ImFont_GetDescent },
		{"isLoaded", ImFont_IsLoaded },
		{"getDebugName", ImFont_GetDebugName },
		{"calcTextSizeA", ImFont_CalcTextSizeA },
		{"calcWordWrapPositionA", ImFont_CalcWordWrapPositionA},
		{NULL, NULL}
	};
	g_createClass(L, "ImFont", NULL, NULL, NULL, imguiFontFunctionsList);

#ifdef IS_DOCKING_BUILD

	const luaL_Reg imguiDockNodeFunctionList[] = {
		{"getID", DockBuilder_Node_GetID},
		{"getSharedFlags", DockBuilder_Node_GetSharedFlags},
		{"getLocalFlags", DockBuilder_Node_GetLocalFlags},
		{"getParentNode", DockBuilder_Node_GetParentNode},
		{"getChildNodes", DockBuilder_Node_GetChildNodes},
		//{"getWindows", DockBuilder_Node_GetWindows},
		{"getTabBar", DockBuilder_Node_GetTabBar},
		{"getPos", DockBuilder_Node_GetPos},
		{"getSize", DockBuilder_Node_GetSize},
		{"getSizeRef", DockBuilder_Node_GetSizeRef},
		{"getSplitAxis", DockBuilder_Node_GetSplitAxis},
		//{"getWindowClass", DockBuilder_Node_GetWindowClass},
		{"getState", DockBuilder_Node_GetState},
		//{"getHostWindow", DockBuilder_Node_GetHostWindow},
		//{"getVisibleWindow", DockBuilder_Node_GetVisibleWindow},
		{"getCentralNode", DockBuilder_Node_GetCentralNode},
		{"getOnlyNodeWithWindows", DockBuilder_Node_GetOnlyNodeWithWindows},
		{"getLastFrameAlive", DockBuilder_Node_GetLastFrameAlive},
		{"getLastFrameActive", DockBuilder_Node_GetLastFrameActive},
		{"getLastFrameFocused", DockBuilder_Node_GetLastFrameFocused},
		{"getLastFocusedNodeId", DockBuilder_Node_GetLastFocusedNodeId},
		{"getSelectedTabId", DockBuilder_Node_GetSelectedTabId},
		{"getWantCloseTabId", DockBuilder_Node_WantCloseTabId},
		{"getAuthorityForPos", DockBuilder_Node_GetAuthorityForPos},
		{"getAuthorityForSize", DockBuilder_Node_GetAuthorityForSize},
		{"getAuthorityForViewport", DockBuilder_Node_GetAuthorityForViewport},
		{"isVisible", DockBuilder_Node_IsVisible},
		{"isFocused", DockBuilder_Node_IsFocused},
		{"hasCloseButton", DockBuilder_Node_HasCloseButton},
		{"hasWindowMenuButton", DockBuilder_Node_HasWindowMenuButton},
		{"enableCloseButton", DockBuilder_Node_EnableCloseButton},
		{"isCloseButtonEnable", DockBuilder_Node_IsCloseButtonEnable},
		{"wantCloseAll", DockBuilder_Node_WantCloseAll},
		{"wantLockSizeOnce", DockBuilder_Node_WantLockSizeOnce},
		{"wantMouseMove", DockBuilder_Node_WantMouseMove},
		{"wantHiddenTabBarUpdate", DockBuilder_Node_WantHiddenTabBarUpdate},
		{"wantHiddenTabBarToggle", DockBuilder_Node_WantHiddenTabBarToggle},
		{"isMarkedForPosSizeWrite", DockBuilder_Node_MarkedForPosSizeWrite},

		{"isRootNode", DockBuilder_Node_IsRootNode},
		{"isDockSpace", DockBuilder_Node_IsDockSpace},
		{"isFloatingNode", DockBuilder_Node_IsFloatingNode},
		{"isCentralNode", DockBuilder_Node_IsCentralNode},
		{"isHiddenTabBar", DockBuilder_Node_IsHiddenTabBar},
		{"isNoTabBar", DockBuilder_Node_IsNoTabBar},
		{"isSplitNode", DockBuilder_Node_IsSplitNode},
		{"isLeafNode", DockBuilder_Node_IsLeafNode},
		{"isEmpty", DockBuilder_Node_IsEmpty},
		{"getMergedFlags", DockBuilder_Node_GetMergedFlags},
		{"rect", DockBuilder_Node_Rect},
		{NULL, NULL}
	};
	g_createClass(L, "ImGuiDockNode", 0, NULL, NULL, imguiDockNodeFunctionList);

	const luaL_Reg imguiTabBarFunctionList[] = {
		{"getTabs", TabBar_GetTabs},
		{"getTab", TabBar_GetTab},
		{"getTabCount", TabBar_GetTabCount},
		{"getFlags", TabBar_GetFlags},
		{"getID", TabBar_GetID},
		{"getSelectedTabId", TabBar_GetSelectedTabId},
		{"getNextSelectedTabId", TabBar_GetNextSelectedTabId},
		{"getVisibleTabId", TabBar_GetVisibleTabId},
		{"getCurrFrameVisible", TabBar_GetCurrFrameVisible},
		{"getPrevFrameVisible", TabBar_GetPrevFrameVisible},
		{"getBarRect", TabBar_GetBarRect},
		{"getCurrTabsContentsHeight", TabBar_GetCurrTabsContentsHeight},
		{"getPrevTabsContentsHeight", TabBar_GetPrevTabsContentsHeight},
		{"getWidthAllTabs", TabBar_GetWidthAllTabs},
		{"getWidthAllTabsIdeal", TabBar_GetWidthAllTabsIdeal},
		{"getScrollingAnim", TabBar_GetScrollingAnim},
		{"getScrollingTarget", TabBar_GetScrollingTarget},
		{"getScrollingTargetDistToVisibility", TabBar_GetScrollingTargetDistToVisibility},
		{"getScrollingSpeed", TabBar_GetScrollingSpeed},
		{"getScrollingRectMinX", TabBar_GetScrollingRectMinX},
		{"getScrollingRectMaxX", TabBar_GetScrollingRectMaxX},
		{"getReorderRequestTabId", TabBar_GetReorderRequestTabId},
		{"getReorderRequestDir", TabBar_GetReorderRequestDir},
		{"getBeginCount", TabBar_GetBeginCount},
		{"wantLayout", TabBar_WantLayout},
		{"visibleTabWasSubmitted", TabBar_VisibleTabWasSubmitted},
		{"getTabsAddedNew", TabBar_TabsAddedNew},
		{"getTabsActiveCount", TabBar_GetTabsActiveCount},
		{"getLastTabItemIdx", TabBar_GetLastTabItemIdx},
		{"getItemSpacingY", TabBar_GetItemSpacingY},
		{"getFramePadding", TabBar_GetFramePadding},
		{"getBackupCursorPos", TabBar_GetBackupCursorPos},
		{"getTabsNames", TabBar_GetTabsNames},
		{"getTabOrder", TabBar_GetTabOrder},
		{"getTabName", TabBar_GetTabName},
		{NULL, NULL}
	};
	g_createClass(L, "ImGuiTabBar", 0, NULL, NULL, imguiTabBarFunctionList);

	const luaL_Reg imguiTabItemFunctionList[] = {
		{"getID", TabItem_GetID},
		{"getFlags", TabItem_GetFlags},
		{"getLastFrameVisible", TabItem_GetLastFrameVisible},
		{"getLastFrameSelected", TabItem_GetLastFrameSelected},
		{"getOffset", TabItem_GetOffset},
		{"getWidth", TabItem_GetWidth},
		{"getContentWidth", TabItem_GetContentWidth},
		{"getNameOffset", TabItem_GetNameOffset},
		{"getBeginOrder", TabItem_GetBeginOrder},
		{"getIndexDuringLayout", TabItem_GetIndexDuringLayout},
		{"wantClose", TabItem_WantClose},
		{NULL, NULL}
	};
	g_createClass(L, "ImGuiTabItem", 0, NULL, NULL, imguiTabItemFunctionList);

#endif

	const luaL_Reg imguiTextEditorFunctionsList[] = {

		{"setLanguageDefinition", TE_SetLanguageDefinition},
		{"getLanguageDefinition", TE_GetLanguageDefinition},

		{"getLanguageCPP", TE_GetLanguageDefinition_CPP},
		{"getLanguageGLSL", TE_GetLanguageDefinition_GLSL},
		{"getLanguageHLSL", TE_GetLanguageDefinition_HLSL},
		{"getLanguageC", TE_GetLanguageDefinition_C},
		{"getLanguageSQL", TE_GetLanguageDefinition_SQL},
		{"getLanguageAngelScript", TE_GetLanguageDefinition_AngelScript},
		{"getLanguageLua", TE_GetLanguageDefinition_Lua},

		{"getPaletteDark", TE_GetPalette_Dark},
		{"getPaletteLight", TE_GetPalette_Light},
		{"getPaletteRetro", TE_GetPalette_Retro},

		{"setPalette", TE_SetPalette},
		{"getPalette", TE_GetPalette},

		{"setPaletteColor", TE_SetPaletteColor},
		{"getPaletteColor", TE_GetPaletteColor},

		{"loadPalette", TE_LoadPalette},

		{"setErrorMarkers", TE_SetErrorMarkers},
		{"setBreakpoints", TE_SetBreakpoints},

		{"render", TE_Render},

		{"setText", TE_SetText},
		{"getText", TE_GetText},
		{"setTextLines", TE_SetTextLines},
		{"getTextLines", TE_GetTextLines},

		{"getSelectedText", TE_GetSelectedText},
		{"getCurrentLineText", TE_GetCurrentLineText},

		{"getTotalLines", TE_GetTotalLines},
		{"isOverwrite", TE_IsOverwrite},

		{"setReadOnly", TE_SetReadOnly},
		{"isReadOnly", TE_IsReadOnly},
		{"isTextChanged", TE_IsTextChanged},
		{"isCursorPositionChanged", TE_IsCursorPositionChanged},

		{"setColorizerEnable", TE_SetColorizerEnable},
		{"isColorizerEnabled", TE_IsColorizerEnabled},

		{"getCursorPosition", TE_GetCursorPosition},
		{"setCursorPosition", TE_SetCursorPosition},

		{"setHandleMouseInputs", TE_SetHandleMouseInputs},
		{"isHandleMouseInputsEnabled", TE_IsHandleMouseInputsEnabled},

		{"setHandleKeyboardInputs", TE_SetHandleKeyboardInputs},
		{"isHandleKeyboardInputsEnabled", TE_IsHandleKeyboardInputsEnabled},

		{"setImGuiChildIgnored", TE_SetImGuiChildIgnored},
		{"isImGuiChildIgnored", TE_IsImGuiChildIgnored},

		{"setShowWhitespaces", TE_SetShowWhitespaces},
		{"isShowingWhitespaces", TE_IsShowingWhitespaces},

		{"setTabSize", TE_SetTabSize},
		{"getTabSize", TE_GetTabSize},

		{"insertText", TE_InsertText},

		{"moveUp", TE_MoveUp},
		{"moveDown", TE_MoveDown},
		{"moveLeft", TE_MoveLeft},
		{"moveRight", TE_MoveRight},
		{"moveTop", TE_MoveTop},
		{"moveBottom", TE_MoveBottom},
		{"moveHome", TE_MoveHome},
		{"moveEnd", TE_MoveEnd},

		{"setSelectionStart", TE_SetSelectionStart},
		{"setSelectionEnd", TE_SetSelectionEnd},
		{"setSelection", TE_SetSelection},
		{"selectWordUnderCursor", TE_SelectWordUnderCursor},
		{"selectAll", TE_SelectAll},
		{"hasSelection", TE_HasSelection},

		{"copy", TE_Copy},
		{"cut", TE_Cut},
		{"paste", TE_Paste},
		{"delete", TE_Delete},

		{"canUndo", TE_CanUndo},
		{"canRedo", TE_CanRedo},
		{"undo", TE_Undo},
		{"redo", TE_Redo},

		{NULL, NULL}
	};
	g_createClass(L, "ImGuiTextEditor", 0, initTextEditor, destroyTextEditor, imguiTextEditorFunctionsList);

	g_createClass(L, "ImGuiTextEditorPalette", 0, NULL, NULL, imguiEmptyFunctionsList);

	const luaL_Reg imguiLanguageDefenitionFunctionsList[] = {
		{"getName", TE_GetName},
		{NULL, NULL}
	};
	g_createClass(L, "ImGuiTextEditorLanguage", 0, NULL, NULL, imguiLanguageDefenitionFunctionsList);

	const luaL_Reg imguiErrorMarkersFunctionsList[] = {
		{"add", EM_MAdd},
		{"remove", EM_MRemove},
		{"get", EM_MGet},
		{"getSize", EM_MSize},
		{NULL, NULL}
	};
	g_createClass(L, "ImGuiErrorMarkers", 0, initErrorMarkers, destroyErrorMarkers, imguiErrorMarkersFunctionsList);

	const luaL_Reg imguiBreakpointsFunctionsList[] = {
		{"add", EM_BAdd},
		{"remove", EM_BRemove},
		{"get", EM_BGet},
		{"getSize", EM_BSize},

		{NULL, NULL}
	};
	g_createClass(L, "ImGuiBreakpoints", 0, initBreakpoints, destroyBreakpoints, imguiBreakpointsFunctionsList);

	const luaL_Reg imguiPayloadFunctionsList[] = {
		{"getNumData", Payload_GetNumberData},
		{"getStrData", Payload_GetStringData},
		{"getArrData", Payload_GetArrData},
		{"getColor3Data", Payload_GetColor3Data},
		{"getColor4Data", Payload_GetColor4Data},
		{"clear", Payload_Clear},
		{"getDataSize", Payload_GetDataSize},
		{"isDataType", Payload_IsDataType},
		{"isPreview", Payload_IsPreview},
		{"isDelivery", Payload_IsDelivery},
		{NULL, NULL}
	};
	g_createClass(L, "ImGuiPayload", 0, NULL, NULL, imguiPayloadFunctionsList);

	const luaL_Reg clipperFunctionList[] = {
		{"beginClip", Clipper_Begin},
		{"endClip", Clipper_End},
		{"step", Clipper_Step},
		{"getDisplayStart", Clipper_GetDisplayStart},
		{"getDisplayEnd", Clipper_GetDisplayEnd},
		{"forceDisplayRangeByIndices", Clipper_ForceDisplayRangeByIndices},
		{"getStartPosY", Clipper_GetStartPosY},
		{"getItemsCount", Clipper_GetItemsCount},
		{"getItemsHeight", Clipper_GetItemsHeight},
		{NULL, NULL}
	};
	g_createClass(L, "ImGuiListClipper", 0, initImGuiListClipper, destroyImGuiListClipper, clipperFunctionList);

	const luaL_Reg imguiTableSortSpecsFunctionList[] = {
		{"getColumnSortSpecs", TableSortSpecs_GetColumnSortSpecs},
		{"getSpecsCount", TableSortSpecs_GetSpecsCount},
		{"isSpecsDirty", TableSortSpecs_GetSpecsDirty},
		{"setSpecsDirty", TableSortSpecs_SetSpecsDirty},
		{NULL, NULL}
	};
	g_createClass(L, "ImGuiTableSortSpecs", NULL, NULL, NULL, imguiTableSortSpecsFunctionList);

	const luaL_Reg imguiTableColumnSortSpecsFunctionList[] = {
		{"getColumnUserID", TableColumnSortSpecs_GetColumnUserID},
		{"getColumnIndex", TableColumnSortSpecs_GetColumnIndex},
		{"getSortOrder", TableColumnSortSpecs_GetSortOrder},
		{"getSortDirection", TableColumnSortSpecs_GetSortDirection},
		{NULL, NULL}
	};
	g_createClass(L, "ImGuiTableColumnSortSpecs", NULL, NULL, NULL, imguiTableColumnSortSpecsFunctionList);

	const luaL_Reg imguiInputTextCallbackDataFunctionList[] = {
		{"getEventFlag", ITCD_GetEventFlag},
		{"getFlags", ITCD_GetFlags},
		{"getEventChar", ITCD_GetEventChar},
		{"setEventChar", ITCD_SetEventChar},
		{"getEventKey", ITCD_GetEventKey},
		{"getBuf", ITCD_GetBuf},
		{"setBuf", ITCD_SetBuf},
		{"getBufTextLen", ITCD_GetBufTextLen},
		{"setBufTextLen", ITCD_SetBufTextLen},
		{"getBufSize", ITCD_GetBufSize},
		{"isBufDirty", ITCD_IsBufDirty},
		{"setBufDirty", ITCD_SetBufDirty},
		{"getCursorPos", ITCD_GetCursorPos},
		{"setCursorPos", ITCD_SetCursorPos},
		{"getSelectionStart", ITCD_GetSelectionStart},
		{"setSelectionStart", ITCD_SetSelectionStart},
		{"getSelectionEnd", ITCD_GetSelectionEnd},
		{"setSelectionEnd", ITCD_SetSelectionEnd},
		{"setSelection", ITCD_SetSelection},
		{"getSelection", ITCD_GetSelection},
		{"deleteChars", ITCD_DeleteChars},
		{"insertChars", ITCD_InsertChars},
		{"selectAll", ITCD_SelectAll},
		{"clearSelection", ITCD_ClearSelection},
		{"hasSelection", ITCD_HasSelection},
		{NULL, NULL}
	};
	g_createClass(L, "ImGuiInputTextCallbackData", NULL, NULL, NULL, imguiInputTextCallbackDataFunctionList);

	const luaL_Reg imguiSizeCallbackDataFunctionList[] = {
		{"getPos", SCD_GetPos},
		{"getX", SCD_GetPosX},
		{"getY", SCD_GetPosY},
		{"getCurrentSize", SCD_GetCurrentSize},
		{"getCurrentWidth", SCD_GetCurrentWidth},
		{"getCurrentHeight", SCD_GetCurrentHeight},

		{"getDesiredSize", SCD_GetDesiredSize},
		{"getDesiredWidth", SCD_GetDesiredWidth},
		{"getDesiredHeight", SCD_GetDesiredHeight},
		{NULL, NULL}
	};
	g_createClass(L, "ImGuiSizeCallbackData", NULL, NULL, NULL, imguiSizeCallbackDataFunctionList);

	const luaL_Reg imguiFunctionList[] =
	{
		{"beginHorizontal", LyaoutBeginHorizontal},
		{"endHorizontal", LyaoutEndHorizontal},
		{"beginVertical", LyaoutBeginVertical},
		{"endVertical", LyaoutEndVertical},
		{"spring", LyaoutSpring},
		{"suspendLayout", LyaoutSuspendLayout},
		{"resumeLayout", LyaoutResumeLayout},

		{"beginDisabled", BeginDisabled},
		{"endDisabled", EndDisabled},

		{"updateCursor", UpdateMouseCursor},
		{"setResetTouchPosOnEnd", SetResetTouchPosOnEnd},
		{"getResetTouchPosOnEnd", GetResetTouchPosOnEnd},

		{"helpMarker", HelpMarker},

		// Fonts API
		{"pushFont", PushFont},
		{"popFont", PopFont},

		{"setStyleColor", Style_old_SetColor}, // Backward capability

		// Draw list
		{"getStyle", GetStyle},
		{"getWindowDrawList", GetWindowDrawList},
		{"getBackgroundDrawList", GetBackgroundDrawList},
		{"getForegroundDrawList", GetForegroundDrawList},
		{"getIO", GetIO},

		/////////////////////////////////////////////////////////////////////////////// Inputs +

		/// Mouse
		{"onMouseHover", MouseHover},
		{"onMouseMove", MouseMove},
		{"onMouseDown", MouseDown},
		{"onMouseUp", MouseUp},
		{"onMouseWheel", MouseWheel},

		/// Touch
		{"onTouchCancel", TouchCancel},
		{"onTouchMove", TouchMove},
		{"onTouchBegin", TouchBegin},
		{"onTouchEnd", TouchEnd},

		/// Keyboard
		{"onKeyUp", KeyUp},
		{"onKeyDown", KeyDown},
		{"onKeyChar", KeyChar},

		/////////////////////////////////////////////////////////////////////////////// Inputs -

		// Colors
		{"colorConvertHEXtoRGB", ColorConvertHEXtoRGB},
		{"colorConvertRGBtoHEX", ColorConvertRGBtoHEX},
		{"colorConvertRGBtoHSV", ColorConvertRGBtoHSV},
		{"colorConvertHSVtoRGB", ColorConvertHSVtoRGB},
		{"colorConvertHEXtoHSV", ColorConvertHEXtoHSV},
		{"colorConvertHSVtoHEX", ColorConvertHSVtoHEX},

		// Style themes
		{"setDarkStyle", StyleDark},
		{"setLightStyle", StyleLight},
		{"setClassicStyle", StyleClassic},

		// Childs
		{"beginChild", BeginChild},
		{"endChild", EndChild},

		{"isWindowAppearing", IsWindowAppearing},
		{"isWindowCollapsed", IsWindowCollapsed},
		{"isWindowFocused", IsWindowFocused},
		{"isWindowHovered", IsWindowHovered},
		{"getWindowPos", GetWindowPos},
		{"getWindowSize", GetWindowSize},
		{"getWindowWidth", GetWindowWidth},
		{"getWindowHeight", GetWindowHeight},
		{"getWindowBounds", GetWindowBounds},

		{"setNextWindowPos", SetNextWindowPos},
		{"setNextWindowSize", SetNextWindowSize},
		{"setNextWindowSizeConstraints", SetNextWindowSizeConstraints},
		{"setNextWindowContentSize", SetNextWindowContentSize},
		{"setNextWindowCollapsed", SetNextWindowCollapsed},
		{"setNextWindowFocus", SetNextWindowFocus},
		{"setNextWindowBgAlpha", SetNextWindowBgAlpha},
		{"setWindowPos", SetWindowPos},
		{"setWindowSize", SetWindowSize},
		{"setWindowCollapsed", SetWindowCollapsed},
		{"setWindowFocus", SetWindowFocus},
		{"setWindowFontScale", SetWindowFontScale},

		{"getContentRegionMax", GetContentRegionMax},
		{"getContentRegionAvail", GetContentRegionAvail},
		{"getWindowContentRegionMin", GetWindowContentRegionMin},
		{"getWindowContentRegionMax", GetWindowContentRegionMax},

		{"getScrollX", GetScrollX},
		{"getScrollY", GetScrollY},
		{"getScrollMaxX", GetScrollMaxX},
		{"getScrollMaxY", GetScrollMaxY},
		{"setScrollX", SetScrollX},
		{"setScrollY", SetScrollY},
		{"setScrollHereX", SetScrollHereX},
		{"setScrollHereY", SetScrollHereY},
		{"setScrollFromPosX", SetScrollFromPosX},
		{"setScrollFromPosY", SetScrollFromPosY},

		{"pushStyleColor", PushStyleColor},
		{"popStyleColor", PopStyleColor},
		{"pushStyleVar", PushStyleVar},
		{"popStyleVar", PopStyleVar},
		{"getFont", GetFont},
		{"getFontSize", GetFontSize},

		{"pushItemWidth", PushItemWidth},
		{"popItemWidth", PopItemWidth},
		{"pushItemFlag", PushItemFlag},
		{"popItemFlag", PopItemFlag},
		{"setNextItemWidth", SetNextItemWidth},
		{"calcItemWidth", CalcItemWidth},
		{"pushTextWrapPos", PushTextWrapPos},
		{"popTextWrapPos", PopTextWrapPos},
		{"pushAllowKeyboardFocus", PushAllowKeyboardFocus},
		{"popAllowKeyboardFocus", PopAllowKeyboardFocus},
		{"pushButtonRepeat", PushButtonRepeat},
		{"popButtonRepeat", PopButtonRepeat},

		{"separator", Separator},
		{"sameLine", SameLine},
		{"newLine", NewLine},
		{"spacing", Spacing},
		{"dummy", Dummy},
		{"indent", Indent},
		{"unindent", Unindent},
		{"beginGroup", BeginGroup},
		{"endGroup", EndGroup},

		{"getCursorPos", GetCursorPos},
		{"getCursorPosX", GetCursorPosX},
		{"getCursorPosY", GetCursorPosY},
		{"setCursorPos", SetCursorPos},
		{"setCursorPosX", SetCursorPosX},
		{"setCursorPosY", SetCursorPosY},
		{"getCursorStartPos", GetCursorStartPos},
		{"getCursorScreenPos", GetCursorScreenPos},
		{"setCursorScreenPos", SetCursorScreenPos},
		{"alignTextToFramePadding", AlignTextToFramePadding},
		{"getTextLineHeight", GetTextLineHeight},
		{"getTextLineHeightWithSpacing", GetTextLineHeightWithSpacing},
		{"getFrameHeight", GetFrameHeight},
		{"getFrameHeightWithSpacing", GetFrameHeightWithSpacing},

		{"pushID", PushID},
		{"popID", PopID},
		{"getID", GetID},

		{"text", Text},
		{"textColored", TextColored},
		{"textDisabled", TextDisabled},
		{"textWrapped", TextWrapped},
		{"labelText", LabelText},
		{"bulletText", BulletText},

		{"button", Button},
		{"smallButton", SmallButton},
		{"invisibleButton", InvisibleButton},
		{"arrowButton", ArrowButton},

		/// Images +

		{"image", Image},
		{"imageUV", ImageUV},
		{"imageButton", ImageButton},
		{"imageButtonUV", ImageButtonUV},

		{"scaledImage", ScaledImage},
		{"scaledImageButton", ScaledImageButton},
		{"scaledImageButtonWithText", ScaledImageButtonWithText},

		/// Images -

		{"checkbox", Checkbox},
		{"checkboxFlags", CheckboxFlags},
		{"radioButton", RadioButton},
		{"progressBar", ProgressBar},
		{"bullet", Bullet},
		{"beginCombo", BeginCombo},
		{"endCombo", EndCombo},
		{"combo", Combo},

		{"dragFloat", DragFloat},
		{"dragFloat2", DragFloat2},
		{"dragFloat3", DragFloat3},
		{"dragFloat4", DragFloat4},
		{"dragFloatN", DragFloatN},
		{"dragFloatT", DragFloatT},
		{"dragFloatRange2", DragFloatRange2},

		{"dragInt", DragInt},
		{"dragInt2", DragInt2},
		{"dragInt3", DragInt3},
		{"dragInt4", DragInt4},
		{"dragIntN", DragIntN},
		{"dragIntT", DragIntT},
		{"dragIntRange2", DragIntRange2},

		{"sliderFloat", SliderFloat},
		{"sliderFloat2", SliderFloat2},
		{"sliderFloat3", SliderFloat3},
		{"sliderFloat4", SliderFloat4},
		{"sliderFloatN", SliderFloatN},
		{"sliderFloatT", SliderFloatT},
		{"sliderAngle", SliderAngle},
		{"sliderInt", SliderInt},
		{"sliderInt2", SliderInt2},
		{"sliderInt3", SliderInt3},
		{"sliderInt4", SliderInt4},
		{"sliderIntN", SliderIntN},
		{"sliderIntT", SliderIntT},
		{"vSliderFloat", VSliderFloat},
		{"vSliderInt", VSliderInt},

		{"filledSliderFloat", FilledSliderFloat},
		{"filledSliderFloat2", FilledSliderFloat2},
		{"filledSliderFloat3", FilledSliderFloat3},
		{"filledSliderFloat4", FilledSliderFloat4},
		{"filledSliderFloatN", FilledSliderFloatN},
		{"filledSliderFloatT", FilledSliderFloatT},
		{"filledSliderAngle", FilledSliderAngle},
		{"filledSliderInt", FilledSliderInt},
		{"filledSliderInt2", FilledSliderInt2},
		{"filledSliderInt3", FilledSliderInt3},
		{"filledSliderInt4", FilledSliderInt4},
		{"filledSliderIntN", FilledSliderIntN},
		{"filledSliderIntT", FilledSliderIntT},
		{"vFilledSliderFloat", VFilledSliderFloat},
		{"vFilledSliderInt", VFilledSliderInt},

		{"inputText", InputText},
		{"inputTextMultiline", InputTextMultiline},
		{"inputTextWithHint", InputTextWithHint},
		{"inputFloat", InputFloat},
		{"inputFloat2", InputFloat2},
		{"inputFloat3", InputFloat3},
		{"inputFloat4", InputFloat4},
		{"inputFloatN", InputFloatN},
		{"inputFloatT", InputFloatT},
		{"inputInt", InputInt},
		{"inputInt2", InputInt2},
		{"inputInt3", InputInt3},
		{"inputInt4", InputInt4},
		{"inputIntN", InputIntN},
		{"inputIntT", InputIntT},
		{"inputDouble", InputDouble},

		{"colorEdit3", ColorEdit3},
		{"colorEdit4", ColorEdit4},
		{"colorPicker3", ColorPicker3},
		{"colorPicker4", ColorPicker4},
		{"colorButton", ColorButton},
		{"setColorEditOptions", SetColorEditOptions},

		{"treeNode", TreeNode},
		{"treeNodeID", TreeNodeID},
		{"treeNodeEx", TreeNodeEx},
		{"treePush", TreePush},
		{"treePop", TreePop},
		{"getTreeNodeToLabelSpacing", GetTreeNodeToLabelSpacing},
		{"collapsingHeader", CollapsingHeader},
		{"setNextItemOpen", SetNextItemOpen},
		{"selectable", Selectable},

		{"listBox", ListBox},
		{"listBoxHeader", BeginListBox},
		{"listBoxFooter", EndListBox},
		{"plotLines", PlotLines},
		{"plotHistogram", PlotHistogram},
		{"value", Value},

		{"beginMenuBar", BeginMenuBar },
		{"endMenuBar", EndMenuBar },
		{"beginMainMenuBar", BeginMainMenuBar },
		{"endMainMenuBar", EndMainMenuBar },
		{"beginMenu", BeginMenu },
		{"beginMenuEx", BeginMenuEx },
		{"endMenu", EndMenu },
		{"menuItem", MenuItem },
		{"menuItemEx", MenuItemEx },
		{"beginTooltip", BeginTooltip },
		{"endTooltip", EndTooltip },
		{"setTooltip", SetTooltip },
		{"beginPopup", BeginPopup},
		{"beginPopupModal", BeginPopupModal },
		{"endPopup", EndPopup },
		{"openPopup", OpenPopup },
		{"openPopupOnItemClick", OpenPopupOnItemClick},
		{"openPopupContextItem", OpenPopupContextItem},
		{"closeCurrentPopup", CloseCurrentPopup },
		{"beginPopupContextItem", BeginPopupContextItem },
		{"beginPopupContextWindow", BeginPopupContextWindow },
		{"beginPopupContextVoid", BeginPopupContextVoid },
		{"isPopupOpen", IsPopupOpen },

		{"columns", Columns},
		{"nextColumn", NextColumn},
		{"getColumnIndex", GetColumnIndex},
		{"getColumnWidth", GetColumnWidth},
		{"setColumnWidth", SetColumnWidth},
		{"getColumnOffset", GetColumnOffset},
		{"setColumnOffset", SetColumnOffset},
		{"getColumnsCount", GetColumnsCount},

		{"beginTabBar", BeginTabBar},
		{"endTabBar", EndTabBar},
		{"beginTabItem", BeginTabItem},
		{"endTabItem", EndTabItem},
		{"tabItemButton", TabItemButton},
		{"setTabItemClosed", SetTabItemClosed},

		{"logToTTY", LogToTTY},
		{"logToFile", LogToFile},
		{"logToClipboard", LogToClipboard},
		{"logFinish", LogFinish},
		{"logButtons", LogButtons},
		{"logText", LogText},

		{"pushClipRect", PushClipRect},
		{"popClipRect", PopClipRect},

		{"setItemDefaultFocus", SetItemDefaultFocus},
		{"setKeyboardFocusHere", SetKeyboardFocusHere},

		{"isItemHovered", IsItemHovered},
		{"isItemActive", IsItemActive},
		{"isItemFocused", IsItemFocused},
		{"isItemClicked", IsItemClicked},
		{"isItemVisible", IsItemVisible},
		{"isItemEdited", IsItemEdited},
		{"isItemActivated", IsItemActivated},
		{"isItemDeactivated", IsItemDeactivated},
		{"isItemDeactivatedAfterEdit", IsItemDeactivatedAfterEdit},
		{"isItemToggledOpen", IsItemToggledOpen},
		{"isAnyItemHovered", IsAnyItemHovered},
		{"isAnyItemActive", IsAnyItemActive},
		{"isAnyItemFocused", IsAnyItemFocused},
		{"getItemRect", GetItemRect},
		{"getItemRectMin", GetItemRectMin},
		{"getItemRectMax", GetItemRectMax},
		{"getItemRectSize", GetItemRectSize},
		{"setItemAllowOverlap", SetItemAllowOverlap},

		// Miscellaneous Utilities
		{"isRectVisible", IsRectVisible},
		{"getTime", GetTime},
		{"getFrameCount", GetFrameCount},
		{"getStyleColorName", GetStyleColorName},
		{"getStyleColor", GetStyleColor},
		{"beginChildFrame", BeginChildFrame},
		{"endChildFrame", EndChildFrame},

		// Text Utilities
		{"calcTextSize", CalcTextSize},

		// Inputs Utilities: Keyboard
		{"getKeyName", GetKeyName},
		{"isKeyDown", IsKeyDown},
		{"isKeyPressed", IsKeyPressed},
		{"isKeyReleased", IsKeyReleased},
		{"getKeyPressedAmount", GetKeyPressedAmount},
		{"setNextFrameWantCaptureKeyboard", SetNextFrameWantCaptureKeyboard},

		// Inputs Utilities: Mouse
		{"isMouseDown", IsMouseDown},
		{"isMouseClicked", IsMouseClicked},
		{"isMouseReleased", IsMouseReleased},
		{"isMouseDoubleClicked", IsMouseDoubleClicked},
		{"getMouseClickedCount", GetMouseClickedCount},
		{"isMouseHoveringRect", IsMouseHoveringRect},
		{"isMousePosValid", IsMousePosValid},
		{"isAnyMouseDown", IsAnyMouseDown},
		{"getMousePos", GetMousePos},
		{"getMousePosOnOpeningCurrentPopup", GetMousePosOnOpeningCurrentPopup},
		{"isMouseDragging", IsMouseDragging},
		{"getMouseDragDelta", GetMouseDragDelta},
		{"resetMouseDragDelta", ResetMouseDragDelta},
		{"getMouseCursor", GetMouseCursor},
		{"setMouseCursor", SetMouseCursor},
		{"setNextFrameWantCaptureMouse", SetNextFrameWantCaptureMouse},

		// Windows
		{"beginWindow", Begin},
		{"endWindow", End},
		{"beginFullScreenWindow", BeginFullScreenWindow},

		// Render
		{"newFrame", NewFrame},
		{"render", Render},
		{"endFrame", EndFrame},

		// Demos
		{"showUserGuide", ShowUserGuide},
		{"showDemoWindow", ShowDemoWindow},
		{"showAboutWindow", ShowAboutWindow},
		{"showStyleEditor", ShowStyleEditor},
		{"showFontSelector", ShowFontSelector},
		{"showMetricsWindow", ShowMetricsWindow},
		{"showStyleSelector", ShowStyleSelector},
		{"showLuaStyleEditor", ShowLuaStyleEditor},
		{"showStackToolWindow", ShowStackToolWindow},

		// Logs
		{"showLog", ShowLog},
		{"writeLog", WriteLog},

		// Drag & Drop
		{"beginDragDropSource", BeginDragDropSource},
		{"setNumDragDropPayload", SetNumberDragDropPayload},
		{"setStrDragDropPayload", SetStringDragDropPayload},
		{"setArrDragDropPayload", SetArrDragDropPayload},
		{"endDragDropSource", EndDragDropSource},
		{"beginDragDropTarget", BeginDragDropTarget},
		{"acceptDragDropPayload", AcceptDragDropPayload},
		{"endDragDropTarget", EndDragDropTarget},
		{"getDragDropPayload", GetDragDropPayload},


		// TABLES

		{"beginTable", BeginTable},
		{"endTable", EndTable},
		{"tableNextRow", TableNextRow},
		{"tableNextColumn", TableNextColumn},
		{"tableSetColumnIndex", TableSetColumnIndex},

		{"tableSetupColumn", TableSetupColumn},
		{"tableSetupScrollFreeze", TableSetupScrollFreeze},
		{"tableSetColumnEnabled", TableSetColumnEnabled},
		{"tableHeadersRow", TableHeadersRow},
		{"tableHeader", TableHeader},

		{"tableGetSortSpecs", TableGetSortSpecs},

		{"tableGetColumnCount", TableGetColumnCount},
		{"tableGetColumnIndex", TableGetColumnIndex},
		{"tableGetRowIndex", TableGetRowIndex},
		{"tableGetColumnName", TableGetColumnName},
		{"tableGetColumnFlags", TableGetColumnFlags},
		{"tableSetBgColor", TableSetBgColor},

		{"getHoveredWindow", CTX_GetHoveredWindow},
		{"getHoveredWindowRoot", CTX_GetHoveredWindowRoot},
		{"getHoveredWindowUnderMovingWindow", CTX_GetHoveredWindowUnderMovingWindow},
		{"getMovingWindow", CTX_GetMovingWindow},
		{"getActiveIdWindow", CTX_GetActiveIdWindow},
		{"getActiveId", CTX_GetActiveId},
		{"getActiveIdPreviousFrame", CTX_GetActiveIdPreviousFrame},
		{"getActiveIdTimer", CTX_GetActiveIdTimer},
		{"getActiveIdAllowOverlap", CTX_GetActiveIdAllowOverlap},
		{"getHoveredId", CTX_GetHoveredId},
		{"getHoveredIdPreviousFrame", CTX_GetHoveredIdPreviousFrame},
		{"getHoveredIdTimer", CTX_GetHoveredIdTimer},
		{"getHoveredIdAllowOverlap", CTX_GetHoveredIdAllowOverlap},
		{"getDragDropActive", CTX_GetDragDropActive},
		{"getDragDropPayloadSourceId", CTX_GetDragDropPayloadSourceId},
		{"getDragDropPayloadDataType", CTX_GetDragDropPayloadDataType},
		{"getDragDropPayloadDataSize", CTX_GetDragDropPayloadDataSize},

		{"setMousePos", SetMousePos},

	#ifdef IS_DOCKING_BUILD
		{"dockSpace", DockSpace},
		{"dockSpaceOverViewport", DockSpaceOverViewport},
		{"setNextWindowDockID", SetNextWindowDockID},
		{"getWindowDockID", GetWindowDockID},
		{"isWindowDocked", IsWindowDocked},

		{"dockBuilderDockWindow", DockBuilderDockWindow},
		{"dockBuilderGetNode", DockBuilderGetNode},
		{"dockBuilderCheckNode", DockBuilderCheckNode},
		{"dockBuilderSetNodePos", DockBuilderSetNodePos},
		{"dockBuilderSetNodeSize", DockBuilderSetNodeSize},
		{"dockBuilderAddNode", DockBuilderAddNode},
		{"dockBuilderRemoveNode", DockBuilderRemoveNode},
		{"dockBuilderRemoveNodeChildNodes", DockBuilderRemoveNodeChildNodes},
		{"dockBuilderRemoveNodeDockedWindows", DockBuilderRemoveNodeDockedWindows},
		{"dockBuilderSplitNode", DockBuilderSplitNode},
		//{"dockBuilderCopyNode", DockBuilderCopyNode},
		{"dockBuilderCopyWindowSettings", DockBuilderCopyWindowSettings},
		{"dockBuilderCopyDockSpace", DockBuilderCopyDockSpace},
		{"dockBuilderFinish", DockBuilderFinish},
	#endif
		{NULL, NULL}
	};
	g_createClass(L, "ImGui", "Sprite", initImGui, destroyImGui, imguiFunctionList);

	luaL_newweaktable(L);
	luaL_rawsetptr(L, LUA_REGISTRYINDEX, &keyWeak);
	
	bindEnums(L);

	ImNodes_impl::nodes_loader(L);

	lua_getglobal(L, "ImGui");
	lua_pushstring(L, ImGui::GetVersion());
	lua_setfield(L, -2, "_VERSION");
	lua_pop(L, 1);

	return 1;
}

static void g_initializePlugin(lua_State* L)
{
	::L = L;
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");

	lua_pushcnfunction(L, loader, "plugin_init_imgui");
	lua_setfield(L, -2, PLUGIN_NAME);

	lua_pop(L, 2);
}

static void g_deinitializePlugin(lua_State* _UNUSED(L)) { }

#if defined(IS_DOCKING_BUILD) || defined(IS_BETA_BUILD)
REGISTER_PLUGIN_NAMED(PLUGIN_NAME, "1.0.0", imgui_beta)
#else
#ifdef QT_NO_DEBUG
REGISTER_PLUGIN_NAMED(PLUGIN_NAME, "1.0.0", Imgui)
#elif defined(TARGET_OS_MAC) || defined(_MSC_VER)
REGISTER_PLUGIN_STATICNAMED_CPP(PLUGIN_NAME, "1.0.0", Imgui)
#else
REGISTER_PLUGIN_NAMED(PLUGIN_NAME, "1.0.0", Imgui)
#endif
#endif
