#include "Engine.h"
#include "ILevelSerializer.h"
#include "IEditorUI.h"
#include <algorithm>
#include "AnimationSystem.h"
#include "PhysicsSystem.h"
#include "PlayerBehaviorSystem.h"
#include "MovingPlaformSystem.h"
#include "EnemyBehaviorSystem.h"
#include "CollectableSystem.h"
#include "ExplodingSpriteUpdateSystem.h"
#include "IRenderer2D.h"
#include "AABB.h"


#pragma region Update

void Engine::Update(double delta_t)
{

	if (_NewLvlToLoad != "") {
		LoadLevel(_NewLvlToLoad);
		_NewLvlToLoad = "";
		_CurrentMode = EngineMode::Play;
	}
	switch (_CurrentMode) {
	case EngineMode::Play:
		Engine& e = *this;
		for (auto& sys : _Systems) {
			sys->Update(delta_t, _GameCam, e);
		}
		break;
	}
}

#pragma endregion

#pragma region input handlers

void Engine::CursorPosCallbackHandler(double xpos, double ypos, bool imGuiWantsMouse)
{
	switch (_CurrentMode) {
	case EngineMode::Edit:
		_Editor->cursorPositionCallbackHandler(xpos, ypos, imGuiWantsMouse, _EditorCam);
		break;
	case EngineMode::Play:
		break;
	}
}

void Engine::ScrollCallbackHandler(GLFWwindow* window, double xoffset, double yoffset, bool imGuiWantsMouse)
{
	switch (_CurrentMode) {
	case EngineMode::Edit:
		_Editor->scrollCallbackHandler(window, xoffset, yoffset, imGuiWantsMouse, _EditorCam);
		break;
	case EngineMode::Play:
		break;
	}
}

void Engine::MouseButtonCallbackHandler(int button, int action, int mods, bool imGuiWantsMouse)
{
	switch (_CurrentMode) {
	case EngineMode::Edit:
		_Editor->mouseButtonCallbackHandler(button, action, mods, imGuiWantsMouse, _EditorCam);
		break;
	case EngineMode::Play:
		break;
	}
}

void Engine::KeyBoardButtonCallbackHandler(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput)
{
	switch (_CurrentMode) {
	case EngineMode::Edit:
		_Editor->keyboardButtonCallbackHandler(window, key, scancode, action, mods, wantKeyboardInput);
		break;
	case EngineMode::Play:
		for (auto& [entityID, value] : _Components.player_behaviors) {
			if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
				value.leftPressed = true;
			}
			else if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) {
				value.leftPressed = false;
			}
			if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
				value.rightPressed = true;
			}
			else if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) {
				value.rightPressed = false;
			}

			if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
				value.upPressed = true;
			}
			else if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
				value.upPressed = false;
			}

			if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
				value.downPressed = true;
			}
			else if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
				value.downPressed = false;
			}

			if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
				value.spacePressed = true;
			} 

			if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
				value.attackPressed = true;
			}
			else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) {
				value.attackPressed = false;
			}
		}
		if (key == GLFW_KEY_P && action == GLFW_PRESS) {
			GotoEditMode();
		}
		break;
	}
}

void Engine::FrameBufferSizeCallbackHandler(GLFWwindow* window, int newwidth, int newheight)
{
	glViewport(0, 0, newwidth, newheight);
	Renderer->SetH(newheight);
	Renderer->SetW(newwidth);
	switch (_CurrentMode) {
	case EngineMode::Edit:
		_Editor->frameBufferSizeChangeCallbackHandler(window, newwidth, newheight, _EditorCam);
		break;
	}
	
}

#pragma endregion

#pragma region load save level

bool Engine::LoadLevel(std::string filepath)
{
	return _LevelSerializer->DeSerialize(*this, filepath);
}

void Engine::SaveCurrentLevel(std::string filepath)
{
	_LevelSerializer->Serialize(*this, filepath);
}

#pragma endregion

#pragma region switch between play and edit mode

void Engine::GotoPlayMode()
{
	_CurrentMode = EngineMode::Play;
	//_Player1 = SetupPlayer(glm::vec2(120, 60), glm::vec2(32, 32), glm::vec4(0));
}

void Engine::GotoEditMode() {
	//DeleteEntity(_Player1);
	_CurrentMode = EngineMode::Edit;
	_EditorCam.FocusPosition = _Components.transforms[_Player1].pos;
}

#pragma endregion

#pragma region draw sprites and background

void Engine::Draw()
{
	using namespace glm;
	switch (_CurrentMode) {
	case EngineMode::Edit:

		DrawBackgroundLayers(_EditorCam);
		SpritesSystemDraw(_EditorCam);
		ExplodingSpritesSystemDraw(_EditorCam);
		_Editor->DrawEngineOverlay(Renderer.get(), _EditorCam);
		_Editor->DoGui();

		break;
	case EngineMode::Play:
		DrawBackgroundLayers(_GameCam);
		SpritesSystemDraw(_GameCam);
		ExplodingSpritesSystemDraw(_GameCam);
		glm::vec2 playerpos = _Components.transforms[_Player1].pos;
		break;
	}
}

Engine::Engine(IEditorUserInterface* editorUI, ILevelSerializer* serializer, IRenderer2D* renderer, IAudioPlayer* audio)
	:Renderer(std::unique_ptr<IRenderer2D>(renderer)),
	_LevelSerializer(std::unique_ptr<ILevelSerializer>(serializer)),
	_Editor(std::unique_ptr<IEditorUserInterface>(editorUI)),
	_AudioPlayer(std::unique_ptr<IAudioPlayer>(audio))
{
	Renderer->Init();
	_Editor->SetEngine(this);
	_GameCam.FocusPosition = glm::vec2(32, 0);
	_GameCam.Zoom = 3.0f;

	// make systems
	_Systems.push_back(std::make_unique<AnimationSystem>());
	_Systems.push_back(std::make_unique<PhysicsSystem>());
	_Systems.push_back(std::make_unique<PlayerBehaviorSystem>(this));
	_Systems.push_back(std::make_unique<MovingPlaformSystem>());
	_Systems.push_back(std::make_unique<EnemyBehaviorSystem>());
	_Systems.push_back(std::make_unique<CollectableSystem>());
	_Systems.push_back(std::make_unique<ExplodingSpriteUpdateSystem>());

	//LoadLevel("test.lua");
	//CueLevel("test.lua");
	CueLevel("test.lua");
}

void Engine::DrawBackgroundLayers(const Camera2D& camera)
{
	using namespace glm;
	int skipped = 0;
	ITileset* tileset = Renderer->GetTileset();
	vec4 cameraTLBR = camera.GetTLBR(Renderer->GetW(), Renderer->GetH());
	for (const TileLayer& tl : TileLayers) {
		if (!tl.Visible) continue;
		auto width = tl.GetWidth();
		auto height = tl.GetHeight();
		for (size_t i = 0; i < tl.Tiles.size(); i++) {
			auto tileIndex = tl.Tiles[i];
			if (tileIndex == 0) continue;
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
			Tile& t = tileset->Tiles[tileIndex - 1];
			Renderer->DrawWholeTexture(worldPos, vec2(tileset->TileWidthAndHeightPx), 0.0, t.Texture, camera);
		}
	}
}

void Engine::SpritesSystemDraw(const Camera2D& cam)
{
	using namespace glm;
	for (auto& [key, value] : _Components.sprites) {
		if (!value.shoulddraw)
			continue;
		Transform& transform = _Components.transforms[key];
		vec4 cameraTLBR = cam.GetTLBR(Renderer->GetW(), Renderer->GetH());
		vec4 tileTLBR = vec4(
			transform.pos.y - abs(transform.scale.y) * 0.5f,
			transform.pos.x - abs(transform.scale.x) * 0.5f,
			transform.pos.y + abs(transform.scale.y) * 0.5f,
			transform.pos.x + abs(transform.scale.x) * 0.5f
		);
		if (AABBCollision(cameraTLBR, tileTLBR)) {
			Renderer->DrawWholeTexture(transform.pos, transform.scale, transform.rot, value.texture, cam);
		}
	}
}

void Engine::ExplodingSpritesSystemDraw(const Camera2D& cam)
{
	for (auto& [key, value] : _Components.exploding_sprites) {
		if (!value.shoulddraw)
			continue;
		auto& tr = _Components.transforms[key];
		auto& es = _Components.exploding_sprites[key];
		Renderer->DrawExplodingTexture(tr.pos, tr.scale, tr.rot, es.texture, cam, es.explodeTimer);
	}
}

#pragma endregion

#pragma region ctor



#pragma endregion



