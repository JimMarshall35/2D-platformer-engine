#include "EnemyBehaviorSystem.h"
#include "Tileset.h"
#include "ECS.h"
#include <iostream>
#include "Engine.h"

bool AABBCollision(const glm::vec4& bb1, const glm::vec4& bb2)
{
	return ((bb1[3] > bb2[1]) && (bb2[3] > bb1[1])) &&
		((bb1[2] > bb2[0]) && (bb2[2] > bb1[0]));
}

void EnemyBehaviorSystem::Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers)
{
	using namespace glm;
	const float speed = 20;
	
	OperateOnComponentGroup(CT_ENEMYBEHAVIOR, CT_TRANSFORM, CT_PHYSICS) {
		auto& enemy = components.enemy_behaviors[entityID];
		auto& tr = components.transforms[entityID];
		auto& phys = components.physicses[entityID];
		vec2 bottomleft(
			((tr.pos.x + phys.velocity.x * delta_t) - (tr.scale.x*0.5)),// + phys.collider.MinusPixelsLeft,
			((tr.pos.y + phys.velocity.y * delta_t) + (tr.scale.y*0.5))// - phys.collider.MinusPixelsBottom
		);
		vec2 bottomright(
			((tr.pos.x + phys.velocity.x * delta_t) + (tr.scale.x * 0.5)),// - phys.collider.MinusPixelsRight,
			((tr.pos.y + phys.velocity.y * delta_t) + (tr.scale.y * 0.5))// - phys.collider.MinusPixelsBottom
		);
		ivec2 br(floor(bottomright.x / 16.0f), floor(bottomright.y / 16.0f));
		ivec2 bl(floor(bottomleft.x / 16.0f), floor(bottomleft.y / 16.0f));
		if (!SolidTileAtCoords(br.x,br.y,tilelayers,phys) && SolidTileAtCoords(bl.x, bl.y, tilelayers, phys)) {
			phys.velocity.x *= -1.0;
			tr.scale.x *= -1.0f;
		}
		else if (!SolidTileAtCoords(bl.x,bl.y, tilelayers, phys) && SolidTileAtCoords(br.x, br.y, tilelayers, phys)) {
			phys.velocity.x *= -1.0;
			tr.scale.x *= -1.0f;
		}
		vec2 physicsBR(
			(tr.pos.x + (tr.scale.x * 0.5)) - phys.collider.MinusPixelsRight,
			(tr.pos.y + (tr.scale.y * 0.5)) - phys.collider.MinusPixelsBottom
		);
		vec2 physicsTL(
			(tr.pos.x - (tr.scale.x * 0.5)) + phys.collider.MinusPixelsRight,
			(tr.pos.y - (tr.scale.y * 0.5)) + phys.collider.MinusPixelsBottom
		);
		vec4 myTLBR(
			physicsTL.y,
			physicsTL.x,
			physicsBR.y,
			physicsBR.x
		);
		EntityID player1ID = _Engine->_Player1;
		auto& player_tr = components.transforms[player1ID];
		auto& player_ph = components.physicses[player1ID];
		vec2 physicsBR_p1(
			(player_tr.pos.x + (player_tr.scale.x * 0.5)) - player_ph.collider.MinusPixelsRight,
			(player_tr.pos.y + (player_tr.scale.y * 0.5)) - player_ph.collider.MinusPixelsBottom
		);
		vec2 physicsTL_p1(
			(player_tr.pos.x - (player_tr.scale.x * 0.5)) + player_ph.collider.MinusPixelsRight,
			(player_tr.pos.y - (player_tr.scale.y * 0.5)) + player_ph.collider.MinusPixelsBottom
		);
		vec4 p1TLBR(
			physicsTL_p1.y,
			physicsTL_p1.x,
			physicsBR_p1.y,
			physicsBR_p1.x
		);
		if (AABBCollision(myTLBR, p1TLBR)) {
			std::cout << "collision" << std::endl;
		}
	}
}

bool EnemyBehaviorSystem::SolidTileAtCoords(int x, int y, const std::vector<TileLayer>& tileLayers, const Physics& phys)
{
	if (x < 0) return true;
	if (y < 0) return false;
	for (const auto& tl : tileLayers) {
		if (tl.Type == Solid || tl.Type == OneWayUp) {

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