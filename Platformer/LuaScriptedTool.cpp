#include "LuaScriptedTool.h"
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include "EditorUI.h"
#include "Engine.h"

LuaScriptedTool::LuaScriptedTool(EditorUserInterface* ui, Engine* engine, std::string luaName, unsigned int inpt, lua_State* L)
	:_LuaName(luaName), _L(L)
{
	name = luaName;
	InputRequirement = inpt;
	_UI = ui;
}

static void dumpstack(lua_State* L) {
	int top = lua_gettop(L);
	for (int i = 1; i <= top; i++) {
		printf("%d\t%s\t", i, luaL_typename(L, i));
		switch (lua_type(L, i)) {
		case LUA_TNUMBER:
			printf("%g\n", lua_tonumber(L, i));
			break;
		case LUA_TSTRING:
			printf("%s\n", lua_tostring(L, i));
			break;
		case LUA_TBOOLEAN:
			printf("%s\n", (lua_toboolean(L, i) ? "true" : "false"));
			break;
		case LUA_TNIL:
			printf("%s\n", "nil");
			break;
		default:
			printf("%p\n", lua_topointer(L, i));
			break;
		}
	}
}


void LuaScriptedTool::handleMouseButton(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera)
{
	if (InputRequirement & MouseButton) {
		lua_getglobal(_L, "EditorTools");
		//dumpstack(_L);
		int size = luaL_len(_L, -1);
		for (int i = 0; i < size; i++) {
			std::cout << "before " << lua_gettop(_L) << std::endl;
			lua_geti(_L, -1, i + 1);

			lua_getfield(_L, -1, "name");
			std::string toolname = luaL_checkstring(_L, -1);
			lua_pop(_L, 1);
			if (name == toolname) {
				lua_getfield(_L, -1, "handlers");
				lua_geti(_L, -1, (int)MouseButton);
				lua_pushinteger(_L, button);
				lua_pushinteger(_L, action);
				lua_pushinteger(_L, mods);
				lua_pushboolean(_L, imGuiWantsMouse);
				lua_pushlightuserdata(_L, (void*)&camera);
				lua_newtable(_L);
				lua_pushnumber(_L, _UI->_LastMouseWorld.x);
				lua_setfield(_L, -2, "x");
				lua_pushnumber(_L, _UI->_LastMouseWorld.y);
				lua_setfield(_L, -2, "y");
				lua_pcall(_L, 6, 0, 0);
				lua_pop(_L, 1);
			}
			lua_pop(_L, 1);
			std::cout << "after " << lua_gettop(_L) << std::endl;
		}
		lua_pop(_L, 1);
	}
}

void LuaScriptedTool::handleKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput)
{
	if (InputRequirement & KeyboardButton) {
		lua_getglobal(_L, "EditorTools");
		int size = luaL_len(_L, -1);
		for (int i = 0; i < size; i++) {
			lua_geti(_L, -1, i + 1);
			lua_getfield(_L, -1, "name");
			std::string toolname = luaL_checkstring(_L, -1);
			lua_pop(_L, 1);
			if (name == toolname) {
				lua_getfield(_L, -1, "handlers");
				lua_geti(_L, -1, (int)KeyboardButton);
				lua_pushinteger(_L, key);
				lua_pushinteger(_L, scancode);
				lua_pushinteger(_L, action);
				lua_pushinteger(_L, mods);
				lua_pushboolean(_L, wantKeyboardInput);
				lua_pcall(_L, 5, 0, 0);
				lua_pop(_L, 1);
			}
			lua_pop(_L, 1);
		}
		lua_pop(_L, 1);
	}
}

void LuaScriptedTool::drawOverlay(const IRenderer2D* renderer, const Camera2D& camera)
{

}

void LuaScriptedTool::handleMouseMove(double xpos, double ypos, bool imGuiWantsMouse, Camera2D& camera)
{
	if (InputRequirement & CursorPositionMove) {
		lua_getglobal(_L, "EditorTools");
		int size = luaL_len(_L, -1);
		for (int i = 0; i < size; i++) {
			lua_geti(_L, -1, i + 1);
			lua_getfield(_L, -1, "name");
			std::string toolname = luaL_checkstring(_L, -1);
			lua_pop(_L, 1);
			if (name == toolname) {
				lua_getfield(_L, -1, "handlers");
				lua_geti(_L, -1, (int)CursorPositionMove);
				lua_pushnumber(_L, xpos);
				lua_pushnumber(_L, ypos);
				lua_pushboolean(_L, imGuiWantsMouse);
				lua_pushlightuserdata(_L, &camera);
				lua_pcall(_L, 4, 0, 0);
				lua_pop(_L, 1);
			}
			lua_pop(_L, 1);
		}
		lua_pop(_L, 1);
	}
}
