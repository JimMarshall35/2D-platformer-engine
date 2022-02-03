#pragma once

#include "Tileset.h"
#include "Tilelayer.h"
#include <memory>
#include "IRenderer2D.h"
#include "Camera2D.h"
#include "ECS.h"
#include "ISystem.h"



class IEditorUserInterface;
class ILevelSerializer;

enum class EngineMode {
	Edit,
	Play
};

class Engine : public ECS
{
private:
	std::unique_ptr<IEditorUserInterface> _Editor;
	std::unique_ptr<ILevelSerializer> _LevelSerializer;
	EngineMode _CurrentMode = EngineMode::Edit;
	std::unique_ptr<IRenderer2D> _Renderer;
	Camera2D _EditorCam;
	Camera2D _GameCam;
	
	std::unique_ptr<ISystem> _AnimationSystem;
	std::unique_ptr<ISystem> _PhysicsSystem;
	std::unique_ptr<ISystem> _PlayerBehaviorSystem;
	std::unique_ptr<ISystem> _MovingPlatformSystem;
	std::unique_ptr<ISystem> _EnemyBehaviorSystem;
	std::unique_ptr<ISystem> _CollectableSystem;
	std::unique_ptr<ISystem> _ExplodingSpritesSystem;

	void SpritesSystemDraw(const Camera2D& cam);
	void ExplodingSpritesSystemDraw(const Camera2D& cam);
	void GotoEditMode();
public:
	EntityID _Player1 = 0;


	glm::ivec2 CollidableLayerWidthAndHeight;
	TileSet _Tileset;
	std::vector<TileLayer> _TileLayers;
	Engine(IEditorUserInterface* editorUI, ILevelSerializer* serializer, IRenderer2D* renderer);
	void DrawBackgroundLayers(const Camera2D& camera);
	bool AABBCollision(const glm::vec4& aabb1, const glm::vec4& aabb2) const;
	void Draw();
	void Update(double delta_t);

	void CursorPosCallbackHandler(double xpos, double ypos, bool imGuiWantsMouse);
	void ScrollCallbackHandler(GLFWwindow* window, double xoffset, double yoffset, bool imGuiWantsMouse);
	void MouseButtonCallbackHandler(int button, int action, int mods, bool imGuiWantsMouse);
	void KeyBoardButtonCallbackHandler(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput);
	void FrameBufferSizeCallbackHandler(GLFWwindow* window, int newwidth, int newheight);
	bool LoadLevel(std::string filepath);
	void SaveCurrentLevel(std::string filepath);

	void GotoPlayMode();
};

