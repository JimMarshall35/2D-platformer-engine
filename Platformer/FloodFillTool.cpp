#include "FloodFillTool.h"
#include "Engine.h"
#include "EditorUI.h"

FloodFillTool::FloodFillTool(EditorUserInterface* ui, Engine* engine)
{
	InputRequirement |= MouseButton;
	name = "FloodFill";
	_Engine = engine;
	_UI = ui;
}

void FloodFillTool::handleMouseButton(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera)
{
	if (!imGuiWantsMouse)
		FloodFill();
}

void FloodFillTool::FloodFill()
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