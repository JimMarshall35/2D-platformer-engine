#pragma once
#include "EditorToolBase.h"
struct lua_State;
class LuaScriptedTool : public EditorToolBase {
public:
	LuaScriptedTool(EditorUserInterface* ui, Engine* engine, std::string luaName, unsigned int input_requirements, lua_State* L);
	void handleMouseButton(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera) override;
	void handleKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput) override;
	void drawOverlay(const IRenderer2D* renderer, const Camera2D& camera) override;
	void handleMouseMove(double xpos, double ypos, bool imGuiWantsMouse, Camera2D& camera) override;
private:
	std::string _LuaName;
	lua_State* _L;
};

