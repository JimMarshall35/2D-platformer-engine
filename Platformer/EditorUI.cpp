#include "EditorUI.h"
#include "ImGuiFileDialog.h"
#include "stb/stb_image.h"
#include <queue>
#include "Engine.h"
#include "ILevelSerializer.h"
#include "LuaVMService.h"
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "IRenderer2D.h"
#include "Tileset.h"
#include "AABB.h"

void EditorUserInterface::DeleteLastLayer()
{
	if (_Engine->TileLayers.size() > 1) {
		_Engine->TileLayers.pop_back();
		if (_SelectedTileLayer >= _Engine->TileLayers.size()) 
			_SelectedTileLayer--;
	}
	std::cout << "sdjoigsd" << std::endl;
}

void EditorUserInterface::PushNewTileLayer(unsigned int width_tiles, unsigned int height_tiles)
{
	TileLayer t;
	t.SetWidthAndHeight(width_tiles, height_tiles);
	_Engine->TileLayers.push_back(t);
}

void EditorUserInterface::DoTileButtonsWindow() {
	unsigned int oncol = 0;
	ITileset* tileset = _Engine->Renderer->GetTileset();
	if (_SelectedTile) {
		std::string selected_info = "selected texture: " + std::to_string(_SelectedTile->Texture) + " selected ID: " + std::to_string(_SelectedTile->ID);
		ImGui::Text(selected_info.c_str());
	}
	for (Tile& t : tileset->Tiles) {
		auto dims = tileset->GetTileDims(t.ID);
		int texWidth = dims.x;
		int texHeight = dims.y;
		ImVec2 uv0 = ImVec2(0.0f, 0.0f);
		ImVec2 uv1 = ImVec2(1.0,1.0);
		if (ImGui::ImageButton((void*)(intptr_t)t.Texture, ImVec2(texWidth * _TilePickerScale, texHeight * _TilePickerScale), uv0, uv1, 0)) {
			_SelectedTile = &t;
		}
		oncol++;
		if (oncol >= tileset->TileSetWidthAndHeightTiles.x) {
			oncol = 0;
		}
		else {
			ImGui::SameLine();
		}
	}
}

void EditorUserInterface::DoLayersWindow()
{
	int numLayers = _Engine->TileLayers.size();
	if (_LayerTabsOpenSize != numLayers) {
		if (_LayerTabsOpen != nullptr) {
			delete[] _LayerTabsOpen;
		}
		_LayerTabsOpen = new bool[numLayers];
	}
	int onLayer = 0;
	unsigned int tab_bar_flags = ImGuiTabBarFlags_Reorderable;
	bool delete_pressed = 0;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {
		for (TileLayer& t : _Engine->TileLayers) {
			
			int width = t.GetWidth();
			int height = t.GetHeight();
			std::stringstream ss;
			ss << "Layer " << onLayer;
			if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip)) {
				PushNewTileLayer(100, 50);
			}
			if (ImGui::TabItemButton("-", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip)) {
				delete_pressed = true;
			}
			if (_LayerTabsOpen[onLayer] && ImGui::BeginTabItem(ss.str().c_str(), &_LayerTabsOpen[onLayer], ImGuiTabItemFlags_None))
			{
				ImGui::Text(ss.str().c_str());
				if (ImGui::InputInt("width", &width)) {
					t.SetWidthAndHeight(width, height);
				}
				ImGui::SameLine();
				ImGui::Checkbox("visible", &t.Visible);
				if (ImGui::InputInt("height", &height)) {
					t.SetWidthAndHeight(width, height);
				}
				_SelectedTileLayer = onLayer;

				//
				const std::string items[] = {  "Decoration","Solid","Kills", "PickUp",  "Ladder", "OneWay"};
				unsigned int& item_current_idx = (unsigned int&)t.Type;
				
				const char* previewVal = items[item_current_idx].c_str();
				if (ImGui::BeginCombo("Layer type", previewVal)) {
					
					for (int i = 0; i < 6; i++) {
						const bool is_selected = (item_current_idx == i);
						if (ImGui::Selectable(items[i].c_str(), is_selected))
							item_current_idx = i;

						// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				ImGui::EndTabItem();
			}
			onLayer++;
		}
		ImGui::EndTabBar();
		if (delete_pressed) {
			DeleteLastLayer();
		}
	}
	
}

#define M_PI           3.14159265358979323846  /* pi */


void EditorUserInterface::DoEntitiesWindow(std::vector<unsigned int>& idsToDelete)
{
	Transform* transform = nullptr;
	Physics* phys = nullptr;
	Sprite* sprite = nullptr;
	Animation* anim = nullptr;
	MovingPlatform* mp = nullptr;
	PlayerBehavior* pb = nullptr;
	Collectable* co = nullptr;
	ExplodingSprite* es = nullptr;
	Health* h = nullptr;
	
	for (auto& [id, components] : _Engine->_Entities) {
		
		if (ImGui::CollapsingHeader(std::to_string(id).c_str(), ImGuiTreeNodeFlags_None))
		{
			for (auto& componentType : components) {
				switch (componentType) {
				case CT_TRANSFORM:
					transform = &_Engine->_Components.transforms[id];
					if (ImGui::TreeNode("Transform")) {
						ImGui::InputFloat2("position", &transform->pos[0]);
						ImGui::InputFloat2("scale", &transform->scale[0]);
						ImGui::InputFloat("rotation", &transform->rot);
						ImGui::TreePop();
					}
					break;
				case CT_PHYSICS:
					phys = &_Engine->_Components.physicses[id];
					if (ImGui::TreeNode("Physics")) {
						ImGui::InputFloat("collider top", &phys->collider.MinusPixelsTop);
						ImGui::InputFloat("collider bottom", &phys->collider.MinusPixelsBottom);
						ImGui::InputFloat("collider left", &phys->collider.MinusPixelsLeft);
						ImGui::InputFloat("collider right", &phys->collider.MinusPixelsRight);
						ImGui::InputFloat2("velocity", &phys->velocity[0]);
						ImGui::InputFloat2("lastpos", &phys->lastPos[0]);
						ImGui::Checkbox("bottom touching", &phys->bottomTouching);
						ImGui::Checkbox("top touching", &phys->topTouching);
						ImGui::Checkbox("left touching", &phys->leftTouching);
						ImGui::Checkbox("right touching", &phys->rightTouching);
						ImGui::Checkbox("collidable", &phys->collider.Collidable);
						ImGui::TreePop();
					}
					break;
				case CT_HEALTH:
					h = &_Engine->_Components.healths[id];
					if (ImGui::TreeNode("Healths")) {
						ImGui::InputInt("max", &h->max);
						ImGui::InputInt("current", &h->current);
						ImGui::TreePop();
					}
					break;
				case CT_SPRITE:
					sprite = &_Engine->_Components.sprites[id];
					
					if (_Engine->_Components.transforms.find(id) != _Engine->_Components.transforms.end()) {
						transform = &_Engine->_Components.transforms[id];
						if (ImGui::TreeNode("Sprite")) {

							ImGui::Image((ImTextureID)sprite->texture, ImVec2(transform->scale.x, transform->scale.y));
							ImGui::Checkbox("should draw", &sprite->shoulddraw);
							ImGui::TreePop();
						};
					}
					break;
				case CT_ANIMATION:
					anim = &_Engine->_Components.animations[id];
					static char name[200];
					strcpy_s(name, anim->animationName.c_str());
					if (ImGui::TreeNode("Animation")) {
						if (ImGui::InputText("Animation name", name, 200)) {
							anim->animationName = std::string((const char*)name);
						}
						ImGui::InputDouble("fps", &anim->fps);
						ImGui::InputInt("onframe", &anim->onframe);
						ImGui::InputInt("numframes", &anim->numframes);
						ImGui::InputDouble("timer", &anim->timer);
						ImGui::Checkbox("shouldloop", &anim->shouldLoop);
						ImGui::Checkbox("isAnimating", &anim->isAnimating);
						ImGui::TreePop();
					};
					break;
				case CT_PLAYERBEHAVIOR:
					if (ImGui::TreeNode("PlayerBehavior")) {
						pb = &_Engine->_Components.player_behaviors[id];
						ImGui::Checkbox("left pressed", &pb->leftPressed);
						ImGui::Checkbox("right pressed", &pb->rightPressed);
						ImGui::Checkbox("up pressed", &pb->upPressed);
						ImGui::Checkbox("up pressed", &pb->downPressed);
						ImGui::Checkbox("space pressed", &pb->spacePressed);
						ImGui::Checkbox("jumping", &pb->jumping);
						ImGui::InputInt("jump counter", &pb->jumpcounter);
						ImGui::InputInt("state", (int*)&pb->state);
						ImGui::InputInt("last state", (int*)&pb->laststate);
						ImGui::InputFloat("max x speed", &pb->MAX_X_SPEED);
						ImGui::InputFloat("movement speed", &pb->movespeed);
						ImGui::InputFloat("jump move speed", &pb->jumpmovespeed);
						ImGui::InputFloat("jump amount", &pb->JumpAmount);
						ImGui::InputFloat("climb speed", &pb->climbspeed);
						ImGui::InputFloat("friction", &pb->friction);
						ImGui::InputInt("coins collected", &pb->coins_collected);
						ImGui::TreePop();
					}
					break;
				case CT_MOVINGPLATFORM:
					mp = &_Engine->_Components.moving_platforms[id];
					if (ImGui::TreeNode("MovingPlatform")) {
						ImGui::InputFloat2("position 1", &mp->point1[0]);
						ImGui::InputFloat2("position 2", &mp->point2[0]);
						ImGui::InputDouble("time period", &mp->time_period);
						ImGui::InputDouble("timer", &mp->timer);
						ImGui::TreePop();
					}
					break;
				case CT_COLLECTABLE:
					co = &_Engine->_Components.collectables[id];
					if (ImGui::TreeNode("Collectable")) {
						ImGui::InputInt("type", (int*)&co->type);
						switch (co->type) {
						case CollectableType::Coin:
							ImGui::InputInt("value", &co->val_i);
							break;
						}
						ImGui::TreePop();
					}
					break;
				case CT_ENEMYBEHAVIOR:
					if (ImGui::TreeNode("EnemyBehavior")) {
						ImGui::TreePop();
					}
					break;
				case CT_EXPLODINGSPRITE:
					es = &_Engine->_Components.exploding_sprites[id];
					if (ImGui::TreeNode("Exploding sprite")) {
						//ImGui::InputFloat("explode timer", &es->explodeTimer);
						ImGui::SliderFloat("explode timer", &es->explodeTimer, 0.0, es->explodeTime);
						ImGui::InputFloat("explode time", &es->explodeTime);
						ImGui::Checkbox("finished exploding", &es->finishedExploding);
						ImGui::Checkbox("should draw", &es->shoulddraw);
						if (_Engine->_Components.transforms.find(id) != _Engine->_Components.transforms.end()) {
							transform = &_Engine->_Components.transforms[id];
							ImGui::Image((ImTextureID)es->texture, ImVec2(transform->scale.x, transform->scale.y));
						}
						ImGui::TreePop();
					}
					break;
				default:
					std::cout << "bad entity id " << id << " component type " << componentType << std::endl;
					return;
				}
			}
			if (ImGui::Button("delete")) {
				idsToDelete.push_back(id);
			}
		}
	}
	for (auto id : idsToDelete) {
		_Engine->DeleteEntity(id);
	}
}

void EditorUserInterface::DoTileSetSelectWindow()
{
	ITileset* tileset = _Engine->Renderer->GetTileset();
	ImGui::Begin("tileset");
	ImGui::InputInt2("Tile width and height", &tileset->TileWidthAndHeightPx[0]);
	// open Dialog Simple
	if (ImGui::Button("Open File Dialog"))
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".png,.jpg", ".");

	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			// action
			FileChosen(filePathName);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
	ImGui::End();
}

void EditorUserInterface::DoToolSelectWindow()
{
	
	ImGui::Begin("tool");
	static const char* current_item = "Draw single tile";
	if (ImGui::BeginCombo("Tool", current_item)) {
		for (auto tool : _EditorTools) {
			if (ImGui::Selectable(tool->name.c_str())) {
				current_item = tool->name.c_str();
				_SelectedTool = tool;
			}
		}
		ImGui::EndCombo();
	}
	ImGui::End();
}




EditorUserInterface::EditorUserInterface(LuaVMService* vm) 
	: _VM(vm)
{
	_VM->DoFile("editortools.lua");
	
}


EditorUserInterface::~EditorUserInterface()
{
	for (auto ptr : _EditorTools) {
		delete ptr;
	}
}

void EditorUserInterface::DoGui()
{
	ITileset* tileset = _Engine->Renderer->GetTileset();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	DoTileSetSelectWindow();
	DoToolSelectWindow();
	if (tileset->Tiles.size() > 0) {
		ImGui::Begin("tiles");
		DoTileButtonsWindow();
		ImGui::End();
	}
	if (_Engine->TileLayers.size() > 0) {
		ImGui::Begin("layers");
		DoLayersWindow();
		ImGui::End();
	}
	std::vector<unsigned int> idsDeleted;
	if (!_Engine->_Entities.empty()) {
		ImGui::Begin("entities");
		DoEntitiesWindow(idsDeleted);
		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	for (auto id : idsDeleted) {
		_Engine->DeleteEntity(id);
	}
}

void EditorUserInterface::IncrementTilePickerScale(float amount)
{
	_TilePickerScale += amount;
}


void EditorUserInterface::DrawEngineOverlay(const IRenderer2D* renderer, const Camera2D& camera)
{
	using namespace glm;
	if (_SelectedTileLayer < 0) return;
	vec4 cameraTLBR = camera.GetTLBR(renderer->GetW(), renderer->GetH());
	int skipped = 0;
	
	TileLayer& tl = _Engine->TileLayers[_SelectedTileLayer];//_SelectedTileLayer == _Engine.TileLayers.size() ? _SelectedTileLayer-1 : _SelectedTileLayer];
	ITileset* tileset = _Engine->Renderer->GetTileset();
	auto width = tl.GetWidth();
	auto height = tl.GetHeight();
	for (int i = 0; i < tl.Tiles.size(); i++) {
		auto tileIndex = tl.Tiles[i];
		vec2 worldPos;
		auto xCoord = i % width;
		auto yCoord = i / width;
		worldPos.x = (float)xCoord;
		worldPos.y = (float)yCoord;
		worldPos *= vec2(tileset->TileWidthAndHeightPx);
		vec4 tileTLBR = vec4(
			worldPos.y,
			worldPos.x,
			worldPos.y + (float)tileset->TileWidthAndHeightPx.y,
			worldPos.x + (float)tileset->TileWidthAndHeightPx.x
		);
		worldPos += vec2(tileset->TileWidthAndHeightPx) * 0.5f;
		
		if (!AABBCollision(cameraTLBR, tileTLBR)) {
			skipped++;
			continue;
		}

		renderer->DrawWireframeRect(worldPos, vec2(tileset->TileWidthAndHeightPx), 0.0,glm::vec4(0.0,0.0,0.0,1.0), camera);
	}

	for (const auto& [key, value] : _Engine->_Components.sprites) {
		if (_Engine->_Components.transforms.find(key) != _Engine->_Components.transforms.end()) {
			Transform& transform = _Engine->_Components.transforms[key];
			renderer->DrawWireframeRect(transform.pos, transform.scale, transform.rot, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), camera);
			if (_Engine->_Components.physicses.find(key) != _Engine->_Components.physicses.end()) {
				auto& collider = _Engine->_Components.physicses[key].collider;
				auto width = abs(transform.scale.x) - collider.MinusPixelsLeft - collider.MinusPixelsRight;
				auto x_offset = collider.MinusPixelsLeft / 2.0f - collider.MinusPixelsRight / 2.0f;
				auto height = abs(transform.scale.y) - collider.MinusPixelsBottom - collider.MinusPixelsTop;
				auto y_offset = collider.MinusPixelsTop / 2.0f - collider.MinusPixelsBottom / 2.0f;
				renderer->DrawSolidRect(transform.pos + vec2(x_offset, y_offset), vec2(width, height), 0, vec4(0.0, 0.5, 0.5, 0.5), camera);
			}
			
		}
	}
	_SelectedTool->drawOverlay(renderer, camera);
}

void EditorUserInterface::cursorPositionCallbackHandler(double xpos, double ypos, bool imGuiWantsMouse, Camera2D& camera)
{
	ITileset* tileset = _Engine->Renderer->GetTileset();
	_lastMouseRawY = ypos;
	_lastMouseRawX = xpos;
	if (!imGuiWantsMouse) {
		if (_SelectedTileLayer < 0) return;
		_LastMouseWorld = camera.MouseScreenPosToWorld(_lastMouseRawX, _lastMouseRawY, WindowW, WindowH);
		glm::ivec2 tileDims = tileset->TileWidthAndHeightPx;
		int tileLayerH = _Engine->TileLayers[_SelectedTileLayer].GetHeight();
		int tileLayerW = _Engine->TileLayers[_SelectedTileLayer].GetWidth();
		int x = floor(_LastMouseWorld.x / (float)tileDims.x);
		int y = floor(_LastMouseWorld.y / (float)tileDims.y);
		if (x < 0 || x >= tileLayerW || y < 0 || y >= tileLayerH) {
			return;
		}
		
		_TileIndexHovvered = y * tileLayerW + x;
		if (_SelectedTool->InputRequirement & CursorPositionMove) {
			_SelectedTool->handleMouseMove(xpos,ypos,imGuiWantsMouse,camera);
		}
	}
}

void EditorUserInterface::scrollCallbackHandler(GLFWwindow* window, double xoffset, double yoffset, bool imGuiWantsMouse, Camera2D& camera)
{
	const float zoomSensitivity = 1;
	const float moveSensitivity = 5;
	if (!imGuiWantsMouse) {
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
			camera.FocusPosition = _LastMouseWorld;
			camera.Zoom += yoffset * zoomSensitivity;
			if (camera.Zoom < 0)
				camera.Zoom = 0;
		}
		else {
			camera.FocusPosition += glm::vec2(moveSensitivity * (float)xoffset, moveSensitivity * (float)yoffset);
		}
	}
	else {

		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
			IncrementTilePickerScale(yoffset * zoomSensitivity);
	}
}

void EditorUserInterface::mouseButtonCallbackHandler(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera)
{
	if (_SelectedTile == nullptr) return;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		_LeftMouseDragStart = _LastMouseWorld;
		_LeftMouseDragging = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		_LeftMouseDragging = false;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		_RightMouseDragStart = _LastMouseWorld;
		_RightMouseDragging = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		_RightMouseDragging = false;
	}

	if (_SelectedTool->InputRequirement & MouseButton) {
		_SelectedTool->handleMouseButton(button, action, mods, imGuiWantsMouse, camera);
	}
}

void EditorUserInterface::keyboardButtonCallbackHandler(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput)
{

	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		_Engine->GotoPlayMode();
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
		_Engine->SaveCurrentLevel("test.lua");
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
		_Engine->CueLevel("test.lua");//LoadLevel("test.lua");
	}
	if (_SelectedTool->InputRequirement & KeyboardButton) {
		_SelectedTool->handleKeyboard(window,key,scancode,action,mods,wantKeyboardInput);
	}
}

void EditorUserInterface::frameBufferSizeChangeCallbackHandler(GLFWwindow* window, int newwidth, int newheight, Camera2D& camera)
{
	//camera.FocusPosition.x = newwidth / 2;
	//camera.FocusPosition.y = newheight / 2;
	WindowH = newheight;
	WindowW = newwidth;
}

void EditorUserInterface::SetEngine(Engine* engine)
{
	 _Engine = engine; 
	 _EditorTools.push_back(new SelectTool(this, engine));
	 _EditorTools.push_back(new DrawSingleTileTool(this, engine));
	 _EditorTools.push_back(new FloodFillTool(this, engine));
	 _SelectedTool = _EditorTools[1];
	 lua_State* L = _VM->GetL();
	 lua_getglobal(L, "EditorTools");
	 int size = luaL_len(L, -1);
	 for (int i = 0; i < size; i++) {
		 lua_geti(L, -1, i + 1);
		 lua_getfield(L, -1, "name");
		 std::string name = luaL_checkstring(L, -1);
		 std::cout << name << std::endl;
		 lua_pop(L, 1);
		 lua_getfield(L, -1, "inputRequirements");
		 EditorToolInputRequirement inpt = (EditorToolInputRequirement)luaL_checkinteger(L, -1);
		 _EditorTools.push_back(new LuaScriptedTool(this, engine, name,inpt, L));
		 lua_pop(L, 2);
	 }
	 lua_pop(L, 1);

}



bool EditorUserInterface::FileChosen(std::string path)
{
	ITileset* tileset = _Engine->Renderer->GetTileset();
	tileset->LoadTilesFromImgFile(path);
	_SelectedTileLayer = 0;
	
	return true;
}