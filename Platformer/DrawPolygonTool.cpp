#include "DrawPolygonTool.h"
#include "Engine.h"
#include "EditorUI.h"
#include "IRenderer2D.h"

DrawPolygonTool::DrawPolygonTool(EditorUserInterface* ui, Engine* engine){
	InputRequirement |= MouseButton;
	InputRequirement |= CursorPositionMove;
	
	name = "Draw Polyon";
	_Engine = engine;
	_UI = ui;
}

void DrawPolygonTool::handleKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput)
{
}

void DrawPolygonTool::handleMouseButton(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera)
{
	if (!imGuiWantsMouse) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			glm::vec2 pos;
			if (_snappedToGrid) {
				pos = GetSnappedPos();
			}
			else {
				pos = glm::vec2(_currentMouse.x,_currentMouse.y);
			}
			_polyUnderConstruction.push_back(Line{
				pos,
				pos
				});
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
			OnPolygonComplete();
			_polyUnderConstruction.clear();
		}
	}
}

void DrawPolygonTool::handleMouseMove(double xpos, double ypos, bool imGuiWantsMouse, Camera2D& camera)
{
	_currentMouse = _UI->_LastMouseWorld;
	if (_polyUnderConstruction.size() > 0) {
		if (_snappedToGrid) {
			auto snapped = GetSnappedPos();
			_polyUnderConstruction.back().pt2 = snapped;
		}
		else {
			_polyUnderConstruction.back().pt2 = _currentMouse;
		}
		
	}
}

void DrawPolygonTool::drawOverlay(const IRenderer2D* renderer, const Camera2D& camera)
{
	auto snappedPos = GetSnappedPos();
	renderer->DrawSolidRect(snappedPos, glm::vec2(5, 5), 45, glm::vec4(1.0, 1.0, 0.0, 0.8), camera);
	for (auto& line : _polyUnderConstruction) {
		glm::vec2 point1pos(line.pt1.x, line.pt1.y);
		glm::vec2 point2pos(line.pt2.x, line.pt2.y);
		renderer->DrawLine(point1pos, point2pos, glm::vec4(1.0, 1.0, 0.0, 0.8),3, camera);
	}
}

glm::vec2 DrawPolygonTool::GetSnappedPos()
{
	glm::ivec2 tilewidthandWHeight = _Engine->Renderer->GetTileset()->TileWidthAndHeightPx;
	int xindex = ((int)_currentMouse.x / tilewidthandWHeight.x);
	if (((int)_currentMouse.x % tilewidthandWHeight.x) > (tilewidthandWHeight.x/2)) {
		xindex++;
	}

	int yindex = ((int)_currentMouse.y / tilewidthandWHeight.y);
	if (((int)_currentMouse.y % tilewidthandWHeight.y) > (tilewidthandWHeight.y/2)) {
		yindex++;
	}

	int snappedx = xindex * tilewidthandWHeight.x;
	int snappedy = yindex * tilewidthandWHeight.y;
	return glm::vec2(snappedx, snappedy);
}

void DrawPolygonTool::OnPolygonComplete()
{
	using namespace glm;
	std::vector<vec2> flattened;
	flattened.resize(_polyUnderConstruction.size() * 2);

	for (int i = 0; i < _polyUnderConstruction.size(); i++) {
		const Line& line = _polyUnderConstruction[i];
		flattened[i * 2] = line.pt1;
		flattened[(i * 2) + 1] = line.pt2;

	}
	auto entity = _Engine->CreateEntity(std::vector<ComponentType>{CT_BOX2DPHYSICS});
	auto bodyPtr = _Engine->Box2dContext.MakeStaticPolygon(flattened, entity);
	if (bodyPtr == nullptr) {
		std::cout << "not convex or too many vertices" << std::endl;
	}
	else {
		
		_Engine->_Components.box2d_physicses[entity].body = bodyPtr;
	}
}
