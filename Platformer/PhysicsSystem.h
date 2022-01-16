#pragma once
#include "ISystem.h"
#include <glm/glm.hpp>
class PhysicsSystem :
    public ISystem
{
public:
    void Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers);
private:
	struct rect
	{
		glm::vec2 pos;
		glm::vec2 size;
		glm::vec2* vel;
		rect* contact[4];
	};
	struct CollidedTile {
		rect rect;
		float contact_t;
	};
	bool SolidTileAtCoords(int x, int y, const std::vector<TileLayer>& tileLayers);
	bool RayVsRect(const glm::vec2& ray_origin, const glm::vec2& ray_dir, const PhysicsSystem::rect* target, glm::vec2& contact_point, glm::vec2& contact_normal, float& t_hit_near);
	bool DynamicRectVsRect(
		const rect* r_dynamic, const float fTimeStep, const rect& r_static,
		glm::vec2& contact_point, glm::vec2& contact_normal, float& contact_time);
	int64_t MovingPlatformAtPos(const glm::vec2& pos, Components& );
	bool ResolveDynamicRectVsRect(rect* r_dynamic, const float fTimeStep, rect* r_static);
};

