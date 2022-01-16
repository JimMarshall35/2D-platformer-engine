#include "MovingPlaformSystem.h"
#include "ECS.h"
#include "Tileset.h"
#include "ECS.h"
#include <algorithm>
#include <iostream>
#define M_PI           3.14159265358979323846  /* pi */

void MovingPlaformSystem::Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers)
{
	using namespace glm;
	for(auto& [entityID,val] : components.moving_platforms) {
			/**/
		auto& movingPlatform = components.moving_platforms[entityID];
		auto& transform = components.transforms[entityID];
		auto& phys = components.physicses[entityID];
		movingPlatform.timer += delta_t;
		vec2 movementCenter = glm::mix(movingPlatform.point1, movingPlatform.point2, 0.5f);
		float t = sin(movingPlatform.timer / (movingPlatform.time_period / M_PI));
		movingPlatform.lastpos = transform.pos;
		
		vec2 newpos = mix(movementCenter, movingPlatform.point2, t);
		transform.pos = newpos;
		phys.velocity = vec2(0);
		//std::cout << t << std::endl;
		//transform.pos = glm::mix(movingPlatform.point1, movingPlatform.point2, t);
	}
}
