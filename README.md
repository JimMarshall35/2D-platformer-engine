# 2D-platformer-engine

another attempt at an engine for 2d games - this time, better. Has a less complicated renderer than the last one and Uses lua much more sparingly. It also uses an ECS based game object system rather than a class hierachy for game objects (should be more efficient on the cache and is DEFINITELY easier to serialize / deserialize and is also composable ie new entity types can be composed out of components to some extent without the need to create a new class. comes at the cost of information hiding / encapsulation.

an engine for 2d platformers (OpenGL / C++).

still a work in progress. and being regularly worked on

Built in level editor (ImGUi UI)

strictly ECS based game object storage architecture https://en.wikipedia.org/wiki/Entity_component_system

Games are saved as lua tables, loaded by a lua script which can be altered to easily procedurally generate levels

lua scripting for editor tools means the editor can be extended quite a bit

TODO / ultimate ambitions:

EASY:
- decouple engine from renderer completely, use DI similar to how the UI, lua scripting service and level serializer are injected into the engine ctor
- fix resizing of layers in editor (layers contents should be preserved if there is a resize)
- optimize rendering of tile based backgrounds (NOT efficient at all at the moment)
- fix annoying bug where you must change tile size to the right value manually in edit mode
- wrap some renderer functions in lua so that lua script based tools can draw on the editor overlay like the hard coded tools do

MEDIUM:
- add some shader based effects
- Add edit undo / redo functionality to editor (command pattern)
- investigate possibility of running some of the systems in parrallel - would this be worth it / practical
- sort out tile sizes in general, allow for possibility of different background layers containing different sized tiles
- look at a different data structure to store components in ie an array

HARD:
- lua scripting for several types of component (enemybehavior, moving platform at the very least)
- make collision detection and physics PERFECT
- add particle effects system
- IMPORTANT: add sound (will probably go with the low level library port audio)
- infinite grid in edit mode?
- completely revamp collision detection to allow for sloped tiles
