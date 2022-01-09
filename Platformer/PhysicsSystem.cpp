#include "PhysicsSystem.h"
#include "Tileset.h"
#include "ECS.h"
#include <algorithm>

void PhysicsSystem::Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers)
{
	using namespace glm;
	for (auto& [entityID, phys] : components.physicses) {
		// get relavent components
		const auto& collider = phys.collider;
		
		auto& transform = components.transforms[entityID];
		
		// get refererence to phys.velocity for convenience
		auto& vel = phys.velocity;
		
		// apply the collider to the transform to get new width height and pos (pos is point in the center of the collider)
		auto width = abs(transform.scale.x) - collider.MinusPixelsLeft - collider.MinusPixelsRight;
		auto x_offset = collider.MinusPixelsLeft / 2.0f - collider.MinusPixelsRight / 2.0f;
		auto height = abs(transform.scale.y) - collider.MinusPixelsBottom - collider.MinusPixelsTop;
		auto y_offset = collider.MinusPixelsTop / 2.0f - collider.MinusPixelsBottom / 2.0f;
		auto pos = transform.pos + vec2(x_offset, y_offset);
		// get top left and bottom right of the collider
		auto topLeft = vec2(pos.x - width / 2.0f, pos.y - height / 2.0f);
		auto bottomRight = vec2(pos.x + width / 2.0f, pos.y + height / 2.0f);
		// bottom / top touching by checking the tile at points just above or below the collider
		auto bottomMid = vec2(pos.x, pos.y + ((bottomRight.y - topLeft.y) / 2) + 3.0f);
		phys.lastbottomTouching = phys.bottomTouching;
		if (SolidTileAtCoords(floor(bottomMid.x / 16.0f), floor(bottomMid.y / 16.0f), tilelayers) ||
			SolidTileAtCoords(floor((bottomRight.x - 0.1) / 16.0f), floor(bottomRight.y / 16.0f), tilelayers) ||
			SolidTileAtCoords(floor((topLeft.x + 0.1) / 16.0f), floor(bottomMid.y / 16.0f),tilelayers)) {
			phys.bottomTouching = true;
			//vel.y = 0;
		}
		else {
			phys.bottomTouching = false;
			
		}
		// if the collider isn't collidable, continue, having set the bottomTouching of phys
		if (!collider.Collidable) {
			transform.pos += vel * (float)delta_t;
			continue;
		}
		//gravity
		if (!phys.bottomTouching) {
			vel.y += 500 * delta_t;
		}
		// "Smear" the colliders top left and bottom right according to velocity to get bounds of all cells that could possibly collide this frame
		vec2 expandedBottomRight = bottomRight;
		vec2 expandedTopLeft = topLeft;
		if (vel.x > 0) {
			expandedBottomRight.x += (float)delta_t * vel.x;
		}
		else if (vel.x < 0) {
			expandedTopLeft.x += (float)delta_t * vel.x;
		}
		if (vel.y > 0) {
			expandedBottomRight.y += (float)delta_t * vel.y;
		}
		else if (vel.y < 0) {
			expandedTopLeft.y += (float)delta_t * vel.y;
		}
		// get these in cell coordinate form
		auto topLeftCells = ivec2(floor(expandedTopLeft.x / 16.0f), floor(expandedTopLeft.y / 16.0f));
		auto bottomRightCells = ivec2(floor(expandedBottomRight.x / 16.0f), floor(expandedBottomRight.y / 16.0f));
		// get a rect describing the moving sprite
		rect dynamic = { topLeft, vec2(width,height),&vel };
#ifdef  TEST_COLLISION
		_TestTiles.clear();
#endif //  TEST_COLLISION
		// iterate through nearby cells
		std::vector<CollidedTile> collidedTiles;
		for (int y = topLeftCells.y; y <= bottomRightCells.y; y++) {
			for (int x = topLeftCells.x; x <= bottomRightCells.x; x++) {
#ifdef DEBUG
				_TestTiles.push_back(y * CollidableLayerWidthAndHeight.x + x);
#endif // DEBUG
				// immediately discard those that don't contain a solid tile
				if (!SolidTileAtCoords(x, y, tilelayers)) continue; 
				// check for collision between the collider and the tile
				auto tileTL = vec2(x * 16.0f, y * 16.0f);
				vec2 cp, cn;
				float ct;
				rect tile = { tileTL, vec2(16.0f,16.0f), nullptr };
				if (DynamicRectVsRect(&dynamic, delta_t, tile, cp, cn, ct)) {
					// if there is a collision store a rect struct representing the tile into a vector along with the contact time which will be used to sort
					collidedTiles.push_back({ tile, ct });
				}
			}
		}
		// sort collided tiles by contact time
		std::sort(collidedTiles.begin(), collidedTiles.end(), [](const CollidedTile& a, const CollidedTile& b) {
			return a.contact_t < b.contact_t;
			});
		vec2 cp, cn;
		float ct;
		// resolve the collisions in their new order
		for (auto collided : collidedTiles) {
			ResolveDynamicRectVsRect(&dynamic, delta_t, &collided.rect);
		}
		// apply velocity to position
		transform.pos += vel * (float)delta_t;
	}
}

bool PhysicsSystem::SolidTileAtCoords(int x, int y, const std::vector<TileLayer>& tileLayers )
{
	if (x < 0) return true;
	if (y < 0) return false;
	for (const auto& tl : tileLayers) {
		if (tl.Type == Solid) {

			int width = tl.GetWidth();
			int height = tl.GetHeight();
			if (x >= width) return true;
			if (y >= height) return true;
			if (tl.Tiles[(width * y) + x] != 0) {
				return true;
			}
		}
	}
	return false;
}

bool PhysicsSystem::RayVsRect(const glm::vec2& ray_origin, const glm::vec2& ray_dir, const PhysicsSystem::rect* target, glm::vec2& contact_point, glm::vec2& contact_normal, float& t_hit_near)
{
	using namespace glm;

	contact_normal = { 0,0 };
	contact_point = { 0,0 };

	// Cache division
	vec2 invdir = 1.0f / ray_dir;

	// Calculate intersections with rectangle bounding axes
	vec2 t_near = (target->pos - ray_origin) * invdir;
	vec2 t_far = (target->pos + target->size - ray_origin) * invdir;

	if (std::isnan(t_far.y) || std::isnan(t_far.x)) return false;
	if (std::isnan(t_near.y) || std::isnan(t_near.x)) return false;

	// Sort distances
	if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
	if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

	// Early rejection		
	if (t_near.x > t_far.y || t_near.y > t_far.x) return false;

	// Closest 'time' will be the first contact
	t_hit_near = std::max(t_near.x, t_near.y);

	// Furthest 'time' is contact on opposite side of target
	float t_hit_far = std::min(t_far.x, t_far.y);

	// Reject if ray direction is pointing away from object
	if (t_hit_far < 0)
		return false;

	// Contact point of collision from parametric line equation
	contact_point = ray_origin + t_hit_near * ray_dir;

	if (t_near.x > t_near.y)
		if (invdir.x < 0)
			contact_normal = { 1, 0 };
		else
			contact_normal = { -1, 0 };
	else if (t_near.x < t_near.y)
		if (invdir.y < 0)
			contact_normal = { 0, 1 };
		else
			contact_normal = { 0, -1 };

	// Note if t_near == t_far, collision is principly in a diagonal
	// so pointless to resolve. By returning a CN={0,0} even though its
	// considered a hit, the resolver wont change anything.
	return true;
}

bool PhysicsSystem::DynamicRectVsRect(const rect* r_dynamic, const float fTimeStep, const rect& r_static, glm::vec2& contact_point, glm::vec2& contact_normal, float& contact_time)
{
	// Check if dynamic rectangle is actually moving - we assume rectangles are NOT in collision to start
	if (r_dynamic->vel->x == 0 && r_dynamic->vel->y == 0)
		return false;

	// Expand target rectangle by source dimensions
	rect expanded_target;
	expanded_target.pos = r_static.pos - r_dynamic->size / 2.0f;
	expanded_target.size = r_static.size + r_dynamic->size;

	if (RayVsRect(r_dynamic->pos + r_dynamic->size / 2.0f, *r_dynamic->vel * fTimeStep, &expanded_target, contact_point, contact_normal, contact_time))
		return (contact_time >= 0.0f && contact_time < 1.0f);
	else
		return false;
}


bool PhysicsSystem::ResolveDynamicRectVsRect(rect* r_dynamic, const float fTimeStep, rect* r_static)
{
	using namespace glm;
	vec2 contact_point, contact_normal;
	float contact_time = 0.0f;
	if (DynamicRectVsRect(r_dynamic, fTimeStep, *r_static, contact_point, contact_normal, contact_time))
	{
		if (contact_normal.y > 0) r_dynamic->contact[0] = r_static; else nullptr;
		if (contact_normal.x < 0) r_dynamic->contact[1] = r_static; else nullptr;
		if (contact_normal.y < 0) r_dynamic->contact[2] = r_static; else nullptr;
		if (contact_normal.x > 0) r_dynamic->contact[3] = r_static; else nullptr;

		*r_dynamic->vel += contact_normal * vec2(std::abs(r_dynamic->vel->x), std::abs(r_dynamic->vel->y)) * (1 - contact_time);
		return true;
	}

	return false;
}
