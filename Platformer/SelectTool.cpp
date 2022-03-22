#include "SelectTool.h"
#include "Engine.h"
#include "EditorUI.h"
#include "IRenderer2D.h"
#include "AABB.h"

SelectTool::SelectTool(EditorUserInterface* ui, Engine* engine)
{
	InputRequirement |= MouseButton;
	InputRequirement |= KeyboardButton;
	name = "Select";
	_Engine = engine;
	_UI = ui;
}

void SelectTool::handleMouseButton(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera)
{
	if (!imGuiWantsMouse) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
			GetNewSelection();
		}
	}

}



void SelectTool::handleKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput)
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

void SelectTool::drawOverlay(const IRenderer2D* renderer, const Camera2D& camera)
{
	glm::vec2 worldPos;

	TilesetBase* tileSet = _Engine->Renderer->GetTileset();
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

void SelectTool::GetNewSelection()
{
	using namespace glm;
	_SelectedTiles.clear();
	TilesetBase* tileSet = _Engine->Renderer->GetTileset();
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

void SelectTool::CopyTiles()
{
	_ClipBoardWidth = _SelectionWidth;
	_ClipBoardLayer = _UI->_SelectedTileLayer;
	_ClipBoard.clear();
	for (int tile : _SelectedTiles) {
		_ClipBoard.push_back(_Engine->TileLayers[_UI->_SelectedTileLayer].Tiles[tile]);
	}
}

void SelectTool::CutTiles()
{
	_ClipBoardWidth = _SelectionWidth;
	_ClipBoardLayer = _UI->_SelectedTileLayer;
	_ClipBoard.clear();
	for (int tile : _SelectedTiles) {
		_ClipBoard.push_back(_Engine->TileLayers[_UI->_SelectedTileLayer].Tiles[tile]);
		_Engine->TileLayers[_UI->_SelectedTileLayer].Tiles[tile] = 0;

	}
}

void SelectTool::PasteTiles()
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

void SelectTool::DeleteSelection()
{
	for (int tile : _SelectedTiles) {
		_Engine->TileLayers[_UI->_SelectedTileLayer].Tiles[tile] = 0;
	}
}