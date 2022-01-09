#pragma once


#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <map>

#include <memory>
#include "IEditorUI.h"

enum class EditorTool {
	DrawSingleTile,
	DrawSquareOfTile,
	FloodFillTile,
	Select
};
class Engine;
class Tile;
class Camera2D;
class Renderer2D;

class EditorUserInterface : public IEditorUserInterface
{
public:
	EditorUserInterface() {};
	~EditorUserInterface();
	
	
	const Tile* GetSelectedTile() { return _SelectedTile; }

#pragma region IEditorUserInterface implementation
	void DoGui();
	void DrawEngineOverlay(const Renderer2D& renderer, const Camera2D& camera);
	void cursorPositionCallbackHandler(double xpos, double ypos, bool imGuiWantsMouse, Camera2D& camera);
	void scrollCallbackHandler(GLFWwindow* window, double xoffset, double yoffset, bool imGuiWantsMouse, Camera2D& cam);
	void mouseButtonCallbackHandler(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& cam);
	void keyboardButtonCallbackHandler(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput);
	void frameBufferSizeChangeCallbackHandler(GLFWwindow* window, int newwidth, int newheight, Camera2D& camera);
	void SetEngine(Engine* engine) { _Engine = engine; }
#pragma endregion

		

	
public:
	int WindowH = 600;
	int WindowW = 800;
private:
	void IncrementTilePickerScale(float amount);
	void DeleteLastLayer();
	void PushNewTileLayer(unsigned int width_tiles, unsigned int height_tiles);
	void DoTileButtonsWindow();
	void DoLayersWindow();
	void DoTileSetSelectWindow();
	void DoToolSelectWindow();
	void GetNewSelection();
	void CopyTiles();
	void CutTiles();
	void PasteTiles();
	void DeleteSelection();
	void FloodFill();

	std::string _ChosenFilePath;
	Engine* _Engine;
	float _TilePickerScale = 2;
	const Tile* _SelectedTile = nullptr;
	int _SelectedTileLayer = -1;

	double _lastMouseRawX;
	double _lastMouseRawY;
	glm::vec2 _LastMouseWorld;

	int _TileIndexHovvered;
	bool* _LayerTabsOpen = nullptr; // using a C style array because std::vector<bool> isn't actually a vector of bools, each "bool" is one bit: incompatible with ImGui
	int _LayerTabsOpenSize = 0;

	EditorTool _CurrentEditorTool = EditorTool::DrawSingleTile;
	bool _LeftMouseDragging = false;
	bool _RightMouseDragging = false;
	glm::vec2 _LeftMouseDragStart;
	glm::vec2 _RightMouseDragStart;

	std::vector<unsigned int> _SelectedTiles;
	unsigned int _SelectionWidth;

	std::vector<unsigned int> _ClipBoard;
	unsigned int _ClipBoardWidth;
	unsigned int _ClipBoardLayer;

	const std::map<EditorTool, const char*> _EditorToolNameMap = {
		{EditorTool::DrawSingleTile, "Draw single tile"},
		{EditorTool::DrawSquareOfTile, "Draw square of tile"},
		{EditorTool::Select, "Select"},
		{EditorTool::FloodFillTile, "Flood fill"}

	};
private:
	bool FileChosen(std::string path);
};

