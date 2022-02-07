An engine for 2d platformers with a level editor - still a work in progress


EASY:
- fix resizing of layers in editor (layers contents should be preserved if there is a resize)
- optimize rendering of tile based backgrounds (NOT efficient at the moment)
- fix annoying bug where you must change tile size to the right value manually in edit mode
- wrap some renderer functions in lua so that lua script based tools can draw on the editor overlay like the hard coded tools do

MEDIUM:
- add some shader based effects
- Add edit undo / redo functionality to editor (command pattern)
- investigate possibility of running some of the systems in parrallel - would this be worth it / practical
- sort out tile sizes in general, allow for possibility of different background layers containing different sized tiles
- look at a different data structure to store components in ie an array
- make an interface for input, supply to engine by DI similar to renderer - important

HARD:
- make a state machine class derived from StateMachineSystem that can be used for lua scripted state machines
- make collision detection and physics PERFECT
- add particle effects system
- IMPORTANT: add sound (will probably go with the low level library port audio)
- infinite grid in edit mode?
- completely revamp collision detection to allow for sloped tiles
