#include "AnimationSystem.h"
#include "Engine.h"
#include "IRenderer2D.h"
#include "ITileset.h"

void AnimationSystem::Update(float delta_t, Camera2D& camera, Engine& engine)
{
	auto& components = engine._Components;
	auto tileset = engine.Renderer->GetTileset();
	OperateOnComponentGroup(CT_ANIMATION, CT_SPRITE) {
		auto& val = components.animations[entityID];
		auto& frames = tileset->AnimationsMap[val.animationName];
		if (val.isAnimating) {
			val.timer += delta_t;
			if (val.timer > 1 / val.fps) {
				val.timer = 0;
				val.onframe++;

				if (val.onframe >= val.numframes && val.shouldLoop) {
					val.onframe = 0;
				}
				else if (val.onframe >= val.numframes && !val.shouldLoop) {
					val.onframe--;
					val.isAnimating = false;
				}
				//std::cout << frames[val.onframe] << std::endl;
			}
			components.sprites[entityID].texture = frames[val.onframe];
		}

	}
}
