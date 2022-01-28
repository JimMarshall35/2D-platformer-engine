#include "Engine.h"
#include "Renderer2D.h"
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

#pragma region Update

void Engine::Update(double delta_t)
{
	switch (_CurrentMode) {
	case EngineMode::Play:
		_MovingPlatformSystem->Update(delta_t, _GameCam, *this);
		_PlayerBehaviorSystem->Update(delta_t, _GameCam, *this);
		_EnemyBehaviorSystem->Update(delta_t, _GameCam, *this);
		_CollectableSystem->Update(delta_t, _GameCam, *this);
		_PhysicsSystem->Update(delta_t, _GameCam, *this);
		_AnimationSystem->Update(delta_t, _GameCam, *this);
		_ExplodingSpritesSystem->Update(delta_t, _GameCam, *this);
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
	_Renderer.WindowH = newheight;
	_Renderer.WindowW = newwidth;
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

		DrawBackgroundLayers(_Renderer, _EditorCam);
		SpritesSystemDraw(_EditorCam);
		ExplodingSpritesSystemDraw(_EditorCam);
		_Editor->DrawEngineOverlay(_Renderer, _EditorCam);
		_Editor->DoGui();

		break;
	case EngineMode::Play:
		DrawBackgroundLayers(_Renderer, _GameCam);
		SpritesSystemDraw(_GameCam);
		ExplodingSpritesSystemDraw(_GameCam);
		glm::vec2 playerpos = _Components.transforms[_Player1].pos;
		break;
	}
}

void Engine::DrawBackgroundLayers(const Renderer2D& renderer, const Camera2D& camera)
{
	using namespace glm;
	int skipped = 0;
	vec4 cameraTLBR = camera.GetTLBR(renderer.WindowW, renderer.WindowH);
	for (const TileLayer& tl : _TileLayers) {
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
			worldPos *= vec2(_Tileset.TileWidthAndHeightPx);
			vec4 tileTLBR = vec4(
				worldPos.y,
				worldPos.x,
				worldPos.y + (float)_Tileset.TileWidthAndHeightPx.y,
				worldPos.x + (float)_Tileset.TileWidthAndHeightPx.x
			);
			worldPos += vec2(_Tileset.TileWidthAndHeightPx) * 0.5f;

			if (!AABBCollision(cameraTLBR, tileTLBR)) {
				skipped++;
				continue;
			}
			Tile& t = _Tileset.Tiles[tileIndex - 1];
			renderer.DrawWholeTexture(worldPos, vec2(_Tileset.TileWidthAndHeightPx), 0.0, t.Texture, camera);
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
		vec4 cameraTLBR = cam.GetTLBR(_Renderer.WindowW, _Renderer.WindowH);
		vec4 tileTLBR = vec4(
			transform.pos.y - abs(transform.scale.y) * 0.5f,
			transform.pos.x - abs(transform.scale.x) * 0.5f,
			transform.pos.y + abs(transform.scale.y) * 0.5f,
			transform.pos.x + abs(transform.scale.x) * 0.5f
		);
		if (AABBCollision(cameraTLBR, tileTLBR)) {
			_Renderer.DrawWholeTexture(transform.pos, transform.scale, transform.rot, value.texture, cam);
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
		_Renderer.DrawExplodingTexture(tr.pos, tr.scale, tr.rot, es.texture, cam, es.explodeTimer);
	}
}

#pragma endregion

#pragma region ctor

Engine::Engine(IEditorUserInterface* editorUI, ILevelSerializer* serializer)
{
	_Renderer.Init();
	_Editor = std::unique_ptr<IEditorUserInterface>(editorUI);
	_Editor->SetEngine(this);
	_LevelSerializer = std::unique_ptr<ILevelSerializer>(serializer);
	_GameCam.FocusPosition = glm::vec2(32, 0);
	_GameCam.Zoom = 2.0f;
	_AnimationSystem = std::unique_ptr<ISystem>(new AnimationSystem());
	_PhysicsSystem = std::unique_ptr<ISystem>(new PhysicsSystem());
	_PlayerBehaviorSystem = std::unique_ptr<ISystem>(new PlayerBehaviorSystem(this));
	_MovingPlatformSystem = std::unique_ptr<ISystem>(new MovingPlaformSystem());
	_EnemyBehaviorSystem = std::unique_ptr<ISystem>(new EnemyBehaviorSystem());
	_CollectableSystem = std::unique_ptr<ISystem>(new CollectableSystem());
	_ExplodingSpritesSystem = std::unique_ptr<ISystem>(new ExplodingSpriteUpdateSystem());
}

#pragma endregion

#pragma region helpers

bool Engine::AABBCollision(const glm::vec4& bb1, const glm::vec4& bb2) const
{
	return ((bb1[3] > bb2[1]) && (bb2[3] > bb1[1])) &&
		((bb1[2] > bb2[0]) && (bb2[2] > bb1[0]));
}

#pragma endregion


