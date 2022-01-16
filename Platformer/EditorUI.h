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
#include <iostream>

class EditorUserInterface;

enum EditorToolInputRequirement: unsigned int {
	None = 0,
	CursorPositionMove = 1,
	MouseButton = 2,
	KeyboardButton = 4
};


class EditorToolBase {
public:
	std::string name;
	unsigned int InputRequirement = 0;
	virtual void handleKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput) = 0;
	virtual void handleMouseButton(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera) = 0;
	virtual void handleMouseMove(double xpos, double ypos, bool imGuiWantsMouse, Camera2D& camera) = 0;
	virtual void drawOverlay(const Renderer2D& renderer, const Camera2D& camera) = 0;
protected:
	Engine* _Engine;
	EditorUserInterface* _UI;
};

class Engine;
class Tile;
class Camera2D;
class Renderer2D;
class LuaVMService;

class EditorUserInterface : public IEditorUserInterface
{
private:

#pragma region Editor tool internal classes

	class DrawSingleTileTool : public EditorToolBase {
	public:
		DrawSingleTileTool(EditorUserInterface* ui, Engine* engine);
		//used
		void handleMouseButton(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera) override;
		void handleMouseMove(double xpos, double ypos, bool imGuiWantsMouse, Camera2D& camera) override;
		void drawOverlay(const Renderer2D& renderer, const Camera2D& camera) override;
		//unused
		void handleKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput) override {};
	};

	class SelectTool : public EditorToolBase {
	public:
		SelectTool(EditorUserInterface* ui, Engine* engine);
		// used
		void handleMouseButton(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera) override;
		void handleKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput) override;
		void drawOverlay(const Renderer2D& renderer, const Camera2D& camera) override;
		//unused
		void handleMouseMove(double xpos, double ypos, bool imGuiWantsMouse, Camera2D& camera) override {};
	private:
		std::vector<unsigned int> _SelectedTiles;
		unsigned int _SelectionWidth;
		std::vector<unsigned int> _ClipBoard;
		unsigned int _ClipBoardWidth;
		unsigned int _ClipBoardLayer;
		void GetNewSelection();
	private:
		void CopyTiles();
		void CutTiles();
		void PasteTiles();
		void DeleteSelection();
	};

	class FloodFillTool : public EditorToolBase {
	public:
		FloodFillTool(EditorUserInterface* ui, Engine* engine);
		//used
		void handleMouseButton(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera) override;
		//unused
		void handleKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput) override {};
		void handleMouseMove(double xpos, double ypos, bool imGuiWantsMouse, Camera2D& camera) override {};
		void drawOverlay(const Renderer2D& renderer, const Camera2D& camera) override {};
	private:
		void FloodFill();
	};

	class LuaScriptedTool : public EditorToolBase {
	public:
		LuaScriptedTool(EditorUserInterface* ui, Engine* engine);
		void handleMouseButton(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera) override;
		void handleKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput) override;
		void drawOverlay(const Renderer2D& renderer, const Camera2D& camera) override;
		void handleMouseMove(double xpos, double ypos, bool imGuiWantsMouse, Camera2D& camera) override;
	};

#pragma endregion

public:
	EditorUserInterface(LuaVMService* vm);
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
	void SetEngine(Engine* engine);
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
	void DoEntitiesWindow();
	void DoTileSetSelectWindow();
	void DoToolSelectWindow();

	std::vector<EditorToolBase*> _EditorTools;
	EditorToolBase* _SelectedTool;


	LuaVMService* _VM;
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

	bool _LeftMouseDragging = false;
	bool _RightMouseDragging = false;
	glm::vec2 _LeftMouseDragStart;
	glm::vec2 _RightMouseDragStart;


private:
	bool FileChosen(std::string path);
};

