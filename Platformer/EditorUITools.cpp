#include "EditorUI.h"
#include "Engine.h"
#include <queue>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include <cstdio>
#include "IRenderer2D.h"
#include "Tileset.h"
#include "AABB.h"
#pragma region single tile

EditorUserInterface::DrawSingleTileTool::DrawSingleTileTool(EditorUserInterface* ui, Engine* engine)
{
	InputRequirement |= CursorPositionMove;
	InputRequirement |= MouseButton;
	name = "Draw Single Tile";
	_Engine = engine;
	_UI = ui;
}

void EditorUserInterface::DrawSingleTileTool::handleMouseButton(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera)
{
	if (!imGuiWantsMouse) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			_Engine->TileLayers[_UI->_SelectedTileLayer].Tiles[_UI->_TileIndexHovvered] = _UI->_SelectedTile->ID;
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
			_Engine->TileLayers[_UI->_SelectedTileLayer].Tiles[_UI->_TileIndexHovvered] = 0;
		}
	}

}

void EditorUserInterface::DrawSingleTileTool::handleMouseMove(double xpos, double ypos, bool imGuiWantsMouse, Camera2D& camera)
{
	if (!imGuiWantsMouse) {
		if (_UI->_LeftMouseDragging) {
			_Engine->TileLayers[_UI->_SelectedTileLayer].Tiles[_UI->_TileIndexHovvered] = _UI->_SelectedTile->ID;
		}
		else if (_UI->_RightMouseDragging) {
			_Engine->TileLayers[_UI->_SelectedTileLayer].Tiles[_UI->_TileIndexHovvered] = 0;
		}
	}
}

void EditorUserInterface::DrawSingleTileTool::drawOverlay(const IRenderer2D* renderer, const Camera2D& camera)
{
	TileLayer& tl = _Engine->TileLayers[_UI->_SelectedTileLayer];//_SelectedTileLayer == _Engine.TileLayers.size() ? _SelectedTileLayer-1 : _SelectedTileLayer];
	auto width = tl.GetWidth();
	auto height = tl.GetHeight();
	glm::vec2 worldPos;
	auto xCoord = _UI->_TileIndexHovvered % width;
	auto yCoord = _UI->_TileIndexHovvered / width;
	worldPos.x = (float)xCoord;
	worldPos.y = (float)yCoord;
	ITileset* tileset = _Engine->Renderer->GetTileset();
	worldPos *= glm::vec2(tileset->TileWidthAndHeightPx);
	worldPos += glm::vec2(tileset->TileWidthAndHeightPx) * 0.5f;
	renderer->DrawWireframeRect(worldPos, glm::vec2(tileset->TileWidthAndHeightPx), 0.0, glm::vec4(1.0, 1.0, 1.0, 1.0), camera);
}

#pragma endregion

#pragma region select

EditorUserInterface::SelectTool::SelectTool(EditorUserInterface* ui, Engine* engine)
{
	InputRequirement |= MouseButton;
	InputRequirement |= KeyboardButton;
	name = "Select";
	_Engine = engine;
	_UI = ui;
}

void EditorUserInterface::SelectTool::handleMouseButton(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera)
{
	if (!imGuiWantsMouse) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
			GetNewSelection();
		}
	}

}



void EditorUserInterface::SelectTool::handleKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput)
{
	if (!wantKeyboardInput) {
		if (key == GLFW_KEY_C && action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
			CopyTiles();
		}
		if (key == GLFW_KEY_X && action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
			CutTiles();
		}
		if (key == GLFW_KEY_V && action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
			PasteTiles();
		}
		if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) {
			DeleteSelection();
		}
	}

}

void EditorUserInterface::SelectTool::drawOverlay(const IRenderer2D* renderer, const Camera2D& camera)
{
	glm::vec2 worldPos;

	ITileset* tileSet = _Engine->Renderer->GetTileset();
	TileLayer& tl = _Engine->TileLayers[_UI->_SelectedTileLayer];
	auto width = tl.GetWidth();
	auto height = tl.GetHeight();
	if (_UI->_LeftMouseDragging) {
		glm::vec2 scale;
		scale.x = _UI->_LeftMouseDragStart.x - _UI->_LastMouseWorld.x;
		scale.y = _UI->_LeftMouseDragStart.y - _UI->_LastMouseWorld.y;
		renderer->DrawWireframeRect(_UI->_LeftMouseDragStart + scale * -0.5f, scale, 0.0, glm::vec4(1.0, 1.0, 1.0, 1.0), camera);
	}
	for (unsigned int tile : _SelectedTiles) {
		auto xCoord = tile % width;
		auto yCoord = tile / width;
		worldPos.x = (float)xCoord;
		worldPos.y = (float)yCoord;
		worldPos *= glm::vec2(tileSet->TileWidthAndHeightPx);
		worldPos += glm::vec2(tileSet->TileWidthAndHeightPx) * 0.5f;
		renderer->DrawSolidRect(worldPos, glm::vec2(tileSet->TileWidthAndHeightPx), 0, glm::vec4(0.0f, 0.5f, 0.5f, 0.4f), camera);
	}
}

void EditorUserInterface::SelectTool::GetNewSelection()
{
	using namespace glm;
	_SelectedTiles.clear();
	ITileset* tileSet = _Engine->Renderer->GetTileset();
	TileLayer& tl = _Engine->TileLayers[_UI->_SelectedTileLayer];
	auto width = tl.GetWidth();
	auto height = tl.GetHeight();
	float lowestX, lowestY, highestX, highestY;
	if (_UI->_LeftMouseDragStart.x <= _UI->_LastMouseWorld.x) {
		lowestX = _UI->_LeftMouseDragStart.x;
		highestX = _UI->_LastMouseWorld.x;
	}
	else {
		lowestX = _UI->_LastMouseWorld.x;
		highestX = _UI->_LeftMouseDragStart.x;
	}

	if (_UI->_LeftMouseDragStart.y <= _UI->_LastMouseWorld.y) {
		lowestY = _UI->_LeftMouseDragStart.y;
		highestY = _UI->_LastMouseWorld.y;
	}
	else {
		lowestY = _UI->_LastMouseWorld.y;
		highestY = _UI->_LeftMouseDragStart.y;
	}
	vec4 selectionTLBR = vec4(lowestY, lowestX, highestY, highestX);
	int last_i = 0;
	_SelectionWidth = 1;
	for (int i = 0; i < _Engine->TileLayers[_UI->_SelectedTileLayer].Tiles.size(); i++) {
		vec2 worldPos;
		auto xCoord = i % width;
		auto yCoord = i / width;
		worldPos.x = (float)xCoord;
		worldPos.y = (float)yCoord;
		worldPos *= vec2(tileSet->TileWidthAndHeightPx);
		vec4 tileTLBR = vec4(
			worldPos.y,
			worldPos.x,
			worldPos.y + (float)tileSet->TileWidthAndHeightPx.y,
			worldPos.x + (float)tileSet->TileWidthAndHeightPx.x
		);
		if (AABBCollision(tileTLBR, selectionTLBR)) {
			if (i == last_i + 1) {
				_SelectionWidth++;
				if (_SelectionWidth >= width)
					_SelectionWidth = width;
			}
			else {
				_SelectionWidth = 1;
			}
			_SelectedTiles.push_back(i);
			last_i = i;
		}
	}
}

void EditorUserInterface::SelectTool::CopyTiles()
{
	_ClipBoardWidth = _SelectionWidth;
	_ClipBoardLayer = _UI->_SelectedTileLayer;
	_ClipBoard.clear();
	for (int tile : _SelectedTiles) {
		_ClipBoard.push_back(_Engine->TileLayers[_UI->_SelectedTileLayer].Tiles[tile]);
	}
}

void EditorUserInterface::SelectTool::CutTiles()
{
	_ClipBoardWidth = _SelectionWidth;
	_ClipBoardLayer = _UI->_SelectedTileLayer;
	_ClipBoard.clear();
	for (int tile : _SelectedTiles) {
		_ClipBoard.push_back(_Engine->TileLayers[_UI->_SelectedTileLayer].Tiles[tile]);
		_Engine->TileLayers[_UI->_SelectedTileLayer].Tiles[tile] = 0;

	}
}

void EditorUserInterface::SelectTool::PasteTiles()
{
	int startindex = _SelectedTiles[0];
	int dst_tile = startindex;
	int rows = _ClipBoard.size() / _ClipBoardWidth;
	int copiedTilesOnRow = 0;
	int dst_layer_width = _Engine->TileLayers[_UI->_SelectedTileLayer].GetWidth();
	for (int i = 0; i < _ClipBoard.size(); i++) {
		_Engine->TileLayers[_UI->_SelectedTileLayer].Tiles[dst_tile] = _ClipBoard[i];
		copiedTilesOnRow++;
		if (copiedTilesOnRow >= _ClipBoardWidth) {
			dst_tile -= _ClipBoardWidth - 1;
			dst_tile += dst_layer_width;
			copiedTilesOnRow = 0;
		}
		else {
			dst_tile++;
		}
	}
}

void EditorUserInterface::SelectTool::DeleteSelection()
{
	for (int tile : _SelectedTiles) {
		_Engine->TileLayers[_UI->_SelectedTileLayer].Tiles[tile] = 0;
	}
}

#pragma endregion

#pragma region flood fill

EditorUserInterface::FloodFillTool::FloodFillTool(EditorUserInterface* ui, Engine* engine)
{
	InputRequirement |= MouseButton;
	name = "FloodFill";
	_Engine = engine;
	_UI = ui;
}

void EditorUserInterface::FloodFillTool::handleMouseButton(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera)
{
	if (!imGuiWantsMouse)
		FloodFill();
}

void EditorUserInterface::FloodFillTool::FloodFill()
{
	std::queue<unsigned int> q;
	q.push(_UI->_TileIndexHovvered);
	TileLayer& tl = _Engine->TileLayers[_UI->_SelectedTileLayer];
	auto tl_w = tl.GetWidth();
	auto tl_h = tl.GetHeight();
	while (!q.empty()) {
		unsigned int current = q.front();
		q.pop();
		unsigned int tile = tl.Tiles[current];
		if (tile != 0) continue;
		tl.Tiles[current] = _UI->_SelectedTile->ID;

		int topindex, bottomindex, leftindex, rightindex;
		topindex = current - tl_w;
		bottomindex = current + tl_w;
		leftindex = current - 1;
		rightindex = current + 1;
		if (topindex > 0) {
			q.push(topindex);
		}
		if (bottomindex < tl.Tiles.size()) {
			q.push(bottomindex);
		}
		//test
		if (leftindex > 0)
			q.push(leftindex);
		if (rightindex < tl.Tiles.size())
			q.push(rightindex);
	}
}

#pragma endregion

#pragma region lua scripted tool

EditorUserInterface::LuaScriptedTool::LuaScriptedTool(EditorUserInterface* ui, Engine* engine, std::string luaName, unsigned int inpt, lua_State* L)
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


void EditorUserInterface::LuaScriptedTool::handleMouseButton(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera)
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

void EditorUserInterface::LuaScriptedTool::handleKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput)
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

void EditorUserInterface::LuaScriptedTool::drawOverlay(const IRenderer2D* renderer, const Camera2D& camera)
{

}

void EditorUserInterface::LuaScriptedTool::handleMouseMove(double xpos, double ypos, bool imGuiWantsMouse, Camera2D& camera)
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

#pragma endregion

