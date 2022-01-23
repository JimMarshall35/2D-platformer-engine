# 2D-platformer-engine

an engine for 2d platformers (OpenGL / C++).

still a work in progress. and being regularly worked on

Built in level editor (ImGUi UI)

strictly ECS based game object storage architecture https://en.wikipedia.org/wiki/Entity_component_system

Games are saved as lua tables, loaded by a lua script which can be altered to easily procedurally generate levels

lua scripting for editor tools means the editor can be extended quite a bit

TODO / ultimate ambitions:

- lua scripting for several types of component (enemybehavior, moving platform at the very least)
- optimize rendering of tile based backgrounds (NOT efficient at all at the moment)
- make collision detection and physics PERFECT
- Add edit undo / redo functionality to editor (command pattern)
- fix resizing of layers in editor (layers contents should be preserved if there is a resize)
- investigate possibility of running some of the systems in parrallel - would this be worth it / practical
- add particle effects system
- add some shader based effects
- decouple engine from renderer completely, use DI similar to how the UI, lua scripting service and level serializer are injected 
- IMPORTANT: add sound 
