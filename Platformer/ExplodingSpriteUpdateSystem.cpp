#include "ExplodingSpriteUpdateSystem.h"
#include "Engine.h"
void ExplodingSpriteUpdateSystem::Update(float delta_t, Camera2D& camera, Engine& engine)
{
	auto& components = engine._Components;

	OperateOnComponentGroup(CT_EXPLODINGSPRITE) {
		auto& es = components.exploding_sprites[entityID];
		if (!es.finishedExploding) {
			es.explodeTimer += (float)delta_t;
			if (es.explodeTimer >= es.explodeTime) {
				es.finishedExploding = true;
			}
		}
	}
}

