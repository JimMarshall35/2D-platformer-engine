#include "CollectableSystem.h"
#include "Engine.h"
#include "AABB.h"

void CollectableSystem::Update(float delta_t, Camera2D& camera, Engine& engine)
{
	using namespace glm;
	auto& components = engine._Components;

	std::vector<EntityID> todelete;
	OperateOnComponentGroup(CT_COLLECTABLE, CT_TRANSFORM) {
		auto& tr = components.transforms[entityID];
		auto& co = components.collectables[entityID];
		if (co.collected) {
			auto& es = components.exploding_sprites[entityID];
			if (es.finishedExploding) {
				todelete.push_back(entityID);
			}
			continue;
		}
		EntityID player1ID = engine._Player1;
		if (player1ID != 0) {
			vec4 myTLBR(
				tr.pos.y - (tr.scale.y * 0.5f),
				tr.pos.x - (tr.scale.x * 0.5f),
				tr.pos.y + (tr.scale.y * 0.5f),
				tr.pos.x + (tr.scale.x * 0.5f)
			);
			// TODO: MAKE HELPERS FOR COLLISIONS
			auto& player_tr = components.transforms[player1ID];
			auto& player_ph = components.physicses[player1ID];
			float pvelx = player_ph.velocity.x * delta_t;
			float pvely = player_ph.velocity.y * delta_t;
			vec2 physicsBR_p1(
				((player_tr.pos.x + pvelx) + (abs(player_tr.scale.x) * 0.5)) - player_ph.collider.MinusPixelsRight,
				((player_tr.pos.y + pvely) + (player_tr.scale.y * 0.5)) - player_ph.collider.MinusPixelsBottom
			);
			vec2 physicsTL_p1(
				((player_tr.pos.x + pvelx) - (abs(player_tr.scale.x) * 0.5)) + player_ph.collider.MinusPixelsRight,
				((player_tr.pos.y + pvely) - (player_tr.scale.y * 0.5)) + player_ph.collider.MinusPixelsTop
			);
			vec4 p1TLBR(
				physicsTL_p1.y,
				physicsTL_p1.x,
				physicsBR_p1.y,
				physicsBR_p1.x
			);
			auto& player_pb = components.player_behaviors[player1ID];
			if (AABBCollision(myTLBR, p1TLBR)) {
				//todelete.push_back(entityID);
				co.collected = true;
				const auto& sp = components.sprites[entityID];
				engine.AddComponentToEntity<ExplodingSprite>(components.exploding_sprites, ExplodingSprite{ sp.texture }, entityID);
				engine.RemoveComponentFromEntity<Sprite>(components.sprites, entityID);

				switch (co.type) {
				case CollectableType::Coin:
					player_pb.coins_collected += co.val_i;
					engine.AudioPlayer->PlayClip(_CoinAudioClip, 0.05f);
					break;
				default:
					std::cout << "invalid collectable type" << std::endl;
				}
			}
		}
	}
	for (auto id : todelete) {
		engine.DeleteEntity(id);
	}
}

void CollectableSystem::Initialize(Engine* engine)
{
	_CoinAudioClip = engine->AudioPlayer->LoadClip("341695__projectsu012__coins-1.wav");
}
