#include "DrawSingleTileTool.h"
#include "Engine.h"
#include "EditorUI.h"
#include "IRenderer2D.h"

#pragma region single tile

DrawSingleTileTool::DrawSingleTileTool(EditorUserInterface* ui, Engine* engine)
{
	InputRequirement |= CursorPositionMove;
	InputRequirement |= MouseButton;
	name = "Draw Single Tile";
	_Engine = engine;
	_UI = ui;
}

void DrawSingleTileTool::handleMouseButton(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera)
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

void DrawSingleTileTool::handleMouseMove(double xpos, double ypos, bool imGuiWantsMouse, Camera2D& camera)
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

void DrawSingleTileTool::drawOverlay(const IRenderer2D* renderer, const Camera2D& camera)
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