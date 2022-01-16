#include "EditorUI.h"
#include "ImGuiFileDialog.h"
#include "stb/stb_image.h"
#include <queue>
#include "Engine.h"
#include "ILevelSerializer.h"


void EditorUserInterface::DeleteLastLayer()
{
	if (_Engine->_TileLayers.size() > 1) {
		_Engine->_TileLayers.pop_back();
		if (_SelectedTileLayer >= _Engine->_TileLayers.size()) 
			_SelectedTileLayer--;
	}
	std::cout << "sdjoigsd" << std::endl;
	
}

void EditorUserInterface::PushNewTileLayer(unsigned int width_tiles, unsigned int height_tiles)
{
	TileLayer t;
	t.SetWidthAndHeight(width_tiles, height_tiles);
	_Engine->_TileLayers.push_back(t);
}

void EditorUserInterface::DoTileButtonsWindow() {
	unsigned int oncol = 0;
	
	for (Tile& t : _Engine->_Tileset.Tiles) {
		auto dims = _Engine->_Tileset.GetTileDims(t.ID);
		int texWidth = dims.x;
		int texHeight = dims.y;
		ImVec2 uv0 = ImVec2(0.0f, 0.0f);
		ImVec2 uv1 = ImVec2(1.0,1.0);
		if (ImGui::ImageButton((void*)(intptr_t)t.Texture, ImVec2(texWidth * _TilePickerScale, texHeight * _TilePickerScale), uv0, uv1, 0)) {
			_SelectedTile = &t;
		}
		oncol++;
		if (oncol >= _Engine->_Tileset.TileSetWidthAndHeightTiles.x) {
			oncol = 0;
		}
		else {
			ImGui::SameLine();
		}
	}
}

void EditorUserInterface::DoLayersWindow()
{
	int numLayers = _Engine->_TileLayers.size();
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
		for (TileLayer& t : _Engine->_TileLayers) {
			
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
				const std::string items[] = {  "Decoration","Solid","Kills", "PickUp",  "Ladder"};
				unsigned int& item_current_idx = (unsigned int&)t.Type;
				
				const char* previewVal = items[item_current_idx].c_str();
				if (ImGui::BeginCombo("Layer type", previewVal)) {
					
					for (int i = 0; i < 5; i++) {
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


void EditorUserInterface::DoEntitiesWindow()
{
	Transform* transform = nullptr;
	Physics* phys = nullptr;
	Sprite* sprite = nullptr;
	Animation* anim = nullptr;
	MovingPlatform* mp = nullptr;
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
				case CT_HEALTHS:
					ImGui::Text("Healths");
					break;
				case CT_SPRITE:
					sprite = &_Engine->_Components.sprites[id];
					
					if (_Engine->_Components.transforms.find(id) != _Engine->_Components.transforms.end()) {
						transform = &_Engine->_Components.transforms[id];
						if (ImGui::TreeNode("Sprite")) {

							ImGui::Image((ImTextureID)sprite->texture, ImVec2(transform->scale.x, transform->scale.y));
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
				default:
					std::cout << "bad entity id " << id << " component type " << componentType << std::endl;
					return;
				}
			}
		}
	}
}

void EditorUserInterface::DoTileSetSelectWindow()
{
	ImGui::Begin("tileset");
	ImGui::InputInt2("Tile width and height", &_Engine->_Tileset.TileWidthAndHeightPx[0]);
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
		for (auto const& [enumval, name]: _EditorToolNameMap) {
			if (ImGui::Selectable(name)) {
				current_item = name;
				_CurrentEditorTool = enumval;
			}
		}
		ImGui::EndCombo();
	}
	ImGui::End();
}




EditorUserInterface::~EditorUserInterface()
{
}

void EditorUserInterface::DoGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	DoTileSetSelectWindow();
	DoToolSelectWindow();
	if (_Engine->_Tileset.Tiles.size() > 0) {
		ImGui::Begin("tiles");
		DoTileButtonsWindow();
		ImGui::End();
	}
	if (_Engine->_TileLayers.size() > 0) {
		ImGui::Begin("layers");
		DoLayersWindow();
		ImGui::End();
	}
	if (!_Engine->_Entities.empty()) {
		ImGui::Begin("entities");
		DoEntitiesWindow();
		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorUserInterface::IncrementTilePickerScale(float amount)
{
	_TilePickerScale += amount;
}


void EditorUserInterface::DrawEngineOverlay(const Renderer2D& renderer, const Camera2D& camera)
{
	using namespace glm;
	if (_SelectedTileLayer < 0) return;
	vec4 cameraTLBR = camera.GetTLBR(renderer.WindowW, renderer.WindowH);
	int skipped = 0;
	
	TileLayer& tl = _Engine->_TileLayers[_SelectedTileLayer];//_SelectedTileLayer == _Engine._TileLayers.size() ? _SelectedTileLayer-1 : _SelectedTileLayer];
	TileSet& tileSet = _Engine->_Tileset;
	auto width = tl.GetWidth();
	auto height = tl.GetHeight();
	for (int i = 0; i < tl.Tiles.size(); i++) {
		auto tileIndex = tl.Tiles[i];
		vec2 worldPos;
		auto xCoord = i % width;
		auto yCoord = i / width;
		worldPos.x = (float)xCoord;
		worldPos.y = (float)yCoord;
		worldPos *= vec2(tileSet.TileWidthAndHeightPx);
		vec4 tileTLBR = vec4(
			worldPos.y,
			worldPos.x,
			worldPos.y + (float)tileSet.TileWidthAndHeightPx.y,
			worldPos.x + (float)tileSet.TileWidthAndHeightPx.x
		);
		worldPos += vec2(tileSet.TileWidthAndHeightPx) * 0.5f;
		
		if (!_Engine->AABBCollision(cameraTLBR, tileTLBR)) {
			skipped++;
			continue;
		}

		renderer.DrawWireframeRect(worldPos, vec2(tileSet.TileWidthAndHeightPx), 0.0,glm::vec4(0.0,0.0,0.0,1.0), camera);
	}

	for (const auto& [key, value] : _Engine->_Components.sprites) {
		if (_Engine->_Components.transforms.find(key) != _Engine->_Components.transforms.end()) {
			Transform& transform = _Engine->_Components.transforms[key];
			renderer.DrawWireframeRect(transform.pos, transform.scale, transform.rot, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), camera);
			if (_Engine->_Components.physicses.find(key) != _Engine->_Components.physicses.end()) {
				auto& collider = _Engine->_Components.physicses[key].collider;
				auto width = abs(transform.scale.x) - collider.MinusPixelsLeft - collider.MinusPixelsRight;
				auto x_offset = collider.MinusPixelsLeft / 2.0f - collider.MinusPixelsRight / 2.0f;
				auto height = abs(transform.scale.y) - collider.MinusPixelsBottom - collider.MinusPixelsTop;
				auto y_offset = collider.MinusPixelsTop / 2.0f - collider.MinusPixelsBottom / 2.0f;
				renderer.DrawSolidRect(transform.pos + vec2(x_offset, y_offset), vec2(width, height), 0, vec4(0.0, 0.5, 0.5, 0.5), camera);
			}
			
		}
	}


	vec2 worldPos;
	auto xCoord = _TileIndexHovvered % width;
	auto yCoord = _TileIndexHovvered / width;
	worldPos.x = (float)xCoord;
	worldPos.y = (float)yCoord;
	worldPos *= vec2(tileSet.TileWidthAndHeightPx);
	worldPos += vec2(tileSet.TileWidthAndHeightPx) * 0.5f;
	switch (_CurrentEditorTool) {
	case EditorTool::DrawSingleTile:
		//renderer.DrawSolidRect(worldPos, vec2(tileSet.TileWidthAndHeightPx), 0.0, glm::vec4(0.0, 1.0, 0.0, 0.5), camera);
		renderer.DrawWireframeRect(worldPos, vec2(tileSet.TileWidthAndHeightPx), 0.0, glm::vec4(1.0, 1.0, 1.0, 1.0), camera);
		break;
	case EditorTool::Select:
		if (_LeftMouseDragging) {
			glm::vec2 scale;
			scale.x = _LeftMouseDragStart.x - _LastMouseWorld.x;
			scale.y = _LeftMouseDragStart.y - _LastMouseWorld.y;
			renderer.DrawWireframeRect(_LeftMouseDragStart + scale * -0.5f, scale, 0.0, glm::vec4(1.0, 1.0, 1.0, 1.0), camera);
		}
		for (unsigned int tile : _SelectedTiles) {
			auto xCoord = tile % width;
			auto yCoord = tile / width;
			worldPos.x = (float)xCoord;
			worldPos.y = (float)yCoord;
			worldPos *= vec2(tileSet.TileWidthAndHeightPx);
			worldPos += vec2(tileSet.TileWidthAndHeightPx) * 0.5f;
			renderer.DrawSolidRect(worldPos, vec2(tileSet.TileWidthAndHeightPx), 0, vec4(0.0f, 0.5f, 0.5f, 0.4f), camera);
		}
		break;
	}
	

	
}

void EditorUserInterface::cursorPositionCallbackHandler(double xpos, double ypos, bool imGuiWantsMouse, Camera2D& camera)
{
	_lastMouseRawY = ypos;
	_lastMouseRawX = xpos;
	if (!imGuiWantsMouse) {
		if (_SelectedTileLayer < 0) return;
		_LastMouseWorld = camera.MouseScreenPosToWorld(_lastMouseRawX, _lastMouseRawY, WindowW, WindowH);
		glm::ivec2 tileDims = _Engine->_Tileset.TileWidthAndHeightPx;
		int tileLayerH = _Engine->_TileLayers[_SelectedTileLayer].GetHeight();
		int tileLayerW = _Engine->_TileLayers[_SelectedTileLayer].GetWidth();
		int x = floor(_LastMouseWorld.x / (float)tileDims.x);
		int y = floor(_LastMouseWorld.y / (float)tileDims.y);
		if (x < 0 || x >= tileLayerW || y < 0 || y >= tileLayerH) {
			return;
		}
		switch (_CurrentEditorTool) {
		case EditorTool::DrawSingleTile:
			if (_LeftMouseDragging) {
				_Engine->_TileLayers[_SelectedTileLayer].Tiles[_TileIndexHovvered] = _SelectedTile->ID;
			}
			else if (_RightMouseDragging) {
				_Engine->_TileLayers[_SelectedTileLayer].Tiles[_TileIndexHovvered] = 0;
			}
			break;
		}
		_TileIndexHovvered = y * tileLayerW + x;
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

	if (!imGuiWantsMouse) {
		switch (_CurrentEditorTool) {
		case EditorTool::DrawSingleTile:
			
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				_Engine->_TileLayers[_SelectedTileLayer].Tiles[_TileIndexHovvered] = _SelectedTile->ID;
			}
			else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
				_Engine->_TileLayers[_SelectedTileLayer].Tiles[_TileIndexHovvered] = 0;
			}
			
			break;
		case EditorTool::Select:
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
				GetNewSelection();
			}
			break;
		case EditorTool::FloodFillTile:
			FloodFill();
			break;
		case EditorTool::DrawSquareOfTile:
			break;
		default:
			throw;
			break;
		}
		
	}
}

void EditorUserInterface::keyboardButtonCallbackHandler(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput)
{
	switch (_CurrentEditorTool) {
	case EditorTool::Select:
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
		
		break;
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		_Engine->GotoPlayMode();
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
		_Engine->SaveCurrentLevel("test.lua");
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
		_Engine->LoadLevel("level.lua");
	}

}

void EditorUserInterface::frameBufferSizeChangeCallbackHandler(GLFWwindow* window, int newwidth, int newheight, Camera2D& camera)
{
	camera.FocusPosition.x = newwidth / 2;
	camera.FocusPosition.y = newheight / 2;
	WindowH = newheight;
	WindowW = newwidth;
}

void EditorUserInterface::GetNewSelection()
{
	using namespace glm;
	_SelectedTiles.clear();
	TileSet& tileSet = _Engine->_Tileset;
	TileLayer& tl = _Engine->_TileLayers[_SelectedTileLayer];
	auto width = tl.GetWidth();
	auto height = tl.GetHeight();
	float lowestX, lowestY, highestX, highestY;
	if (_LeftMouseDragStart.x <= _LastMouseWorld.x) {
		lowestX = _LeftMouseDragStart.x;
		highestX = _LastMouseWorld.x;
	}
	else {
		lowestX = _LastMouseWorld.x;
		highestX = _LeftMouseDragStart.x;
	}

	if (_LeftMouseDragStart.y <= _LastMouseWorld.y) {
		lowestY = _LeftMouseDragStart.y;
		highestY = _LastMouseWorld.y;
	}
	else {
		lowestY = _LastMouseWorld.y;
		highestY = _LeftMouseDragStart.y;
	}
	vec4 selectionTLBR = vec4(lowestY,lowestX, highestY,highestX);
	int last_i = 0;
	_SelectionWidth = 1;
	for (int i = 0; i < _Engine->_TileLayers[_SelectedTileLayer].Tiles.size(); i++) {
		vec2 worldPos;
		auto xCoord = i % width;
		auto yCoord = i / width;
		worldPos.x = (float)xCoord;
		worldPos.y = (float)yCoord;
		worldPos *= vec2(tileSet.TileWidthAndHeightPx);
		vec4 tileTLBR = vec4(
			worldPos.y,
			worldPos.x,
			worldPos.y + (float)tileSet.TileWidthAndHeightPx.y,
			worldPos.x + (float)tileSet.TileWidthAndHeightPx.x
		);
		if (_Engine->AABBCollision(tileTLBR, selectionTLBR)) {
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

void EditorUserInterface::CopyTiles()
{
	_ClipBoardWidth = _SelectionWidth;
	_ClipBoardLayer = _SelectedTileLayer;
	_ClipBoard.clear();
	for (int tile : _SelectedTiles) {
		_ClipBoard.push_back(_Engine->_TileLayers[_SelectedTileLayer].Tiles[tile]);
	}
}

void EditorUserInterface::CutTiles()
{
	_ClipBoardWidth = _SelectionWidth;
	_ClipBoardLayer = _SelectedTileLayer;
	_ClipBoard.clear();
	for (int tile : _SelectedTiles) {
		_ClipBoard.push_back(_Engine->_TileLayers[_SelectedTileLayer].Tiles[tile]);
		_Engine->_TileLayers[_SelectedTileLayer].Tiles[tile] = 0;
		
	}
}

void EditorUserInterface::PasteTiles()
{
	/*
		unsigned int* src_ptr = _ClipBoard.data();
		int startindex = _SelectedTiles[0];
		int dst_tile = startindex;
		unsigned int* dst_ptr = &_Engine._TileLayers[_SelectedTileLayer].Tiles.data()[dst_tile];
		int dst_layer_width = _Engine._TileLayers[_SelectedTileLayer].GetWidth();
		for (int i = 0; i < (_ClipBoard.size() / _ClipBoardWidth); i++) {
			memcpy(
				(void*)(dst_ptr + i*dst_layer_width),
				(void*)(src_ptr + i*_ClipBoardWidth),
				sizeof(unsigned int) * _ClipBoardWidth
			);

		}
	*/
	int startindex = _SelectedTiles[0];
	int dst_tile = startindex;
	int rows = _ClipBoard.size() / _ClipBoardWidth;
	int copiedTilesOnRow = 0;
	int dst_layer_width = _Engine->_TileLayers[_SelectedTileLayer].GetWidth();
	for (int i = 0; i < _ClipBoard.size(); i++) {
		_Engine->_TileLayers[_SelectedTileLayer].Tiles[dst_tile] = _ClipBoard[i];
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

void EditorUserInterface::DeleteSelection()
{
	for (int tile : _SelectedTiles) {
		_Engine->_TileLayers[_SelectedTileLayer].Tiles[tile] = 0;
	}
}

void EditorUserInterface::FloodFill()
{
	std::queue<unsigned int> q;
	q.push(_TileIndexHovvered);
	TileLayer& tl = _Engine->_TileLayers[_SelectedTileLayer];
	auto tl_w = tl.GetWidth();
	auto tl_h = tl.GetHeight();
	while (!q.empty()) {
		unsigned int current = q.front();
		q.pop();
		unsigned int tile = tl.Tiles[current];
		if (tile != 0) continue;
		tl.Tiles[current] = _SelectedTile->ID;
		
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
		if(leftindex > 0)
			q.push(leftindex);
		if(rightindex < tl.Tiles.size())
			q.push(rightindex);
	}
}

bool EditorUserInterface::FileChosen(std::string path)
{
	_Engine->_Tileset.LoadTilesFromImgFile(path);
	PushNewTileLayer(100, 50);
	PushNewTileLayer(100, 50);
	_SelectedTileLayer = 0;
	
	return true;
}

