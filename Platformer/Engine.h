#pragma once
#define SEPARATE_PHYSICS_THREAD
#include "Tileset.h" //TODO: this needs to go... also make interface for input
#include "Tilelayer.h"
#include <memory>
#include "Camera2D.h"
#include "ECS.h"
#include "ISystem.h"
#include "IAudioPlayer.h"
#include "Box2dContextService.h"

class IRenderer2D;
class IEditorUserInterface;
class ILevelSerializer;

// these 2 enums below aren't used yet
enum HighLevelEngineState : unsigned int {
	NullState,
	StartUp = 1,
	TitleScreen =2,
	MainMenu = 3,
	LoadingLevel = 4,
	Playing = 5,
	Editing = 6,
	PausedMenu = 7,
	HLStateMax
};
enum HighLevelEngineEvent : unsigned int {
	NoEvent,
	Continue = 1,
	Play = 2,
	Pause = 3,
	Start = 4,
	GotoEditorMode = 5,
	QuitToMainMenu = 6,
	Save = 7,
	HLEventMax
};


enum class EngineMode {
	Edit,
	Play
};

class Engine : public ECS
{
private:
	
	std::unique_ptr<IEditorUserInterface> _Editor;
	std::unique_ptr<ILevelSerializer> _LevelSerializer;
	EngineMode _CurrentMode = EngineMode::Play;
	Camera2D _EditorCam;
	Camera2D _GameCam;
	std::vector<std::unique_ptr<ISystem>> _Systems;
	std::string _NewLvlToLoad = "";
private:
	void InitializeSystems();
	void SpritesSystemDraw(const Camera2D& cam);
	void ExplodingSpritesSystemDraw(const Camera2D& cam);
	void GotoEditMode();
	bool LoadLevel(std::string filepath);
public:
	Box2dContextService Box2dContext = Box2dContextService(this); // temporary test spike - will add interface
	EntityID _Player1 = 0;
	std::unique_ptr<IRenderer2D> Renderer;
	std::unique_ptr<IAudioPlayer> AudioPlayer;

	glm::ivec2 CollidableLayerWidthAndHeight;
	std::vector<TileLayer> TileLayers;
	Engine(IEditorUserInterface* editorUI, ILevelSerializer* serializer, IRenderer2D* renderer, IAudioPlayer* audio);
	void DrawBackgroundLayers(const Camera2D& camera);
	void Draw();
	void Update(double delta_t);

	void CursorPosCallbackHandler(double xpos, double ypos, bool imGuiWantsMouse);
	void ScrollCallbackHandler(GLFWwindow* window, double xoffset, double yoffset, bool imGuiWantsMouse);
	void MouseButtonCallbackHandler(int button, int action, int mods, bool imGuiWantsMouse);
	void KeyBoardButtonCallbackHandler(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput);
	void FrameBufferSizeCallbackHandler(GLFWwindow* window, int newwidth, int newheight);
	
	void SaveCurrentLevel(std::string filepath);
	void CueLevel(std::string filepath) { _NewLvlToLoad = filepath; }
	virtual bool DeleteEntity(EntityID id);

	void GotoPlayMode();
};

