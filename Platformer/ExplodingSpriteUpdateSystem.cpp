#include "ExplodingSpriteUpdateSystem.h"
#include "Engine.h"
void ExplodingSpriteUpdateSystem::Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers)
{
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

ExplodingSpriteUpdateSystem::ExplodingSpriteUpdateSystem(Engine* e)
	:ISystem(e)
{
}
