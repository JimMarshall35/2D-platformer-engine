#include "PlayerBehaviorSystem.h"

#include "Camera2D.h"
#include "Tileset.h"
#include <algorithm>
#include <iostream>
#include "Engine.h"
#include "AABB.h"
#define KNOCKBACK_AMOUNTX 200.0;
#define KNOCKBACK_AMOUNTY 1000.0;

#pragma region ISystem implementation

void PlayerBehaviorSystem::Update(float delta_t, Camera2D& camera, Engine& engine)
{
	auto& components = engine._Components;
	OperateOnComponentGroup(CT_PHYSICS, CT_TRANSFORM, CT_PLAYERBEHAVIOR, CT_ANIMATION) {
		auto& pb = components.player_behaviors[entityID];
		auto& ph = components.physicses[entityID];
		auto& tr = components.transforms[entityID];
		auto& an = components.animations[entityID];

		
		StateMachineSystem::RunStateMachine(delta_t, camera, engine,pb,entityID);
		
		// do things common to all states
		if (pb.rightPressed && pb.state != Stabbing) {
			if (tr.scale.x < 0) {
				tr.scale.x *= -1;
			}
		}
		else if (pb.leftPressed && pb.state != Stabbing) {
			if (tr.scale.x > 0) {
				tr.scale.x *= -1;
			}
		}
		if (pb.state != KnockBack) 
			ph.velocity.x = std::clamp(ph.velocity.x, -pb.MAX_X_SPEED, pb.MAX_X_SPEED);

		camera.FocusPosition = tr.pos;
	}
}

#pragma endregion

#pragma region StateMachineSystem implementation

bool PlayerBehaviorSystem::DoGlobalTransitions(float delta_t, Camera2D& camera, Engine& engine, EntityID id, PlayerState& newstate)
{
	auto& components = engine._Components;
	// check state transitions that are possible from all states
	auto& pb = components.player_behaviors[id];
	auto& ph = components.physicses[id];
	auto& an = components.animations[id];
	auto& tr = components.transforms[id];
	auto& health = components.healths[id];
	if (health.current == 0 && pb.state != Dead && pb.laststate != Dead) {
		newstate = Dead;
		return true;
	}
	if (pb.colliding_enemy != 0 && pb.state != Dead && pb.laststate != Dead && pb.state != KnockBack) {
		health.current--;
		newstate = KnockBack;
		return true;
	}
	return false;
}

#pragma endregion

#pragma region ctor

PlayerBehaviorSystem::PlayerBehaviorSystem(Engine* engine)
{
	_Behaviormap.push_back(nullptr);
	_Behaviormap.push_back(std::unique_ptr<StateBehaviorBase<PlayerState>>(new WalkStateBehavior()));
	_Behaviormap.push_back(std::unique_ptr<StateBehaviorBase<PlayerState>>(new JumpUpStateBehavior()));
	_Behaviormap.push_back(std::unique_ptr<StateBehaviorBase<PlayerState>>(new JumpDownStateBehavior()));
	_Behaviormap.push_back(std::unique_ptr<StateBehaviorBase<PlayerState>>(new JumpLandStateBehavior()));
	_Behaviormap.push_back(std::unique_ptr<StateBehaviorBase<PlayerState>>(new ClimbStateBehavior()));
	_Behaviormap.push_back(std::unique_ptr<StateBehaviorBase<PlayerState>>(new KnockbackStateBehavior()));
	_Behaviormap.push_back(std::unique_ptr<StateBehaviorBase<PlayerState>>(new DeadStateBehavior()));
	_Behaviormap.push_back(std::unique_ptr<StateBehaviorBase<PlayerState>>(new StabStateBehavior()));

}

#pragma endregion

#pragma region ladder helpers 

bool PlayerBehaviorSystem::IsStandingOnLadder(const FloorCollider& collider, const Transform& transform, std::vector<TileLayer>& tileLayers)
{
	using namespace glm;
	auto width = abs(transform.scale.x) - collider.MinusPixelsLeft - collider.MinusPixelsRight;
	auto x_offset = collider.MinusPixelsLeft / 2.0f - collider.MinusPixelsRight / 2.0f;
	auto height = abs(transform.scale.y) - collider.MinusPixelsBottom - collider.MinusPixelsTop;
	auto y_offset = collider.MinusPixelsTop / 2.0f - collider.MinusPixelsBottom / 2.0f;
	auto pos = transform.pos + vec2(x_offset, y_offset);
	// get top left and bottom right of the collider
	auto topLeft = vec2(pos.x - width / 2.0f, pos.y - height / 2.0f);
	auto bottomRight = vec2(pos.x + width / 2.0f, pos.y + height / 2.0f);
	// bottom / top touching by checking the tile at points just above or below the collider
	auto bottomMid = vec2(pos.x, pos.y + ((bottomRight.y - topLeft.y) / 2 - 1.0f));
	bool onLadder = LadderAtCoordinates(floor(bottomMid.x / 16.0f), floor(bottomMid.y / 16.0f),tileLayers);
	bottomMid = vec2(pos.x, pos.y + ((bottomRight.y - topLeft.y) / 2 + 1.0f));
	onLadder |= LadderAtCoordinates(floor(bottomMid.x / 16.0f), floor(bottomMid.y / 16.0f), tileLayers);
	return onLadder;
}
bool PlayerBehaviorSystem::IsAtLadderBottom(const FloorCollider& collider, const Transform& transform, std::vector<TileLayer>& tileLayers) {
	using namespace glm;
	auto width = abs(transform.scale.x) - collider.MinusPixelsLeft - collider.MinusPixelsRight;
	auto x_offset = collider.MinusPixelsLeft / 2.0f - collider.MinusPixelsRight / 2.0f;
	auto height = abs(transform.scale.y) - collider.MinusPixelsBottom - collider.MinusPixelsTop;
	auto y_offset = collider.MinusPixelsTop / 2.0f - collider.MinusPixelsBottom / 2.0f;
	auto pos = transform.pos + vec2(x_offset, y_offset);
	// get top left and bottom right of the collider
	auto topLeft = vec2(pos.x - width / 2.0f, pos.y - height / 2.0f);
	auto bottomRight = vec2(pos.x + width / 2.0f, pos.y + height / 2.0f);
	// bottom / top touching by checking the tile at points just above or below the collider
	auto bottomMid = vec2(pos.x, pos.y + ((bottomRight.y - topLeft.y) / 2) + 2.0f);
	return !LadderAtCoordinates(floor(bottomMid.x / 16.0f), floor(bottomMid.y / 16.0f), tileLayers);
}

bool PlayerBehaviorSystem::LadderAtCoordinates(const int x, const int y, std::vector<TileLayer>& tileLayers)
{
	// TODO : make into general helper shared among systems, that incorporates SolidTileAtCoords
	if (x < 0) return true;
	if (y < 0) return true;
	for (const auto& tl : tileLayers) {
		if (tl.Type == Ladder) {

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

#pragma endregion

#pragma region walk state

PlayerState PlayerBehaviorSystem::WalkStateBehavior::Update(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	auto& tilelayers = engine._TileLayers;
	auto& pb = components.player_behaviors[id];
	auto& ph = components.physicses[id];
	auto& an = components.animations[id];
	auto& tr = components.transforms[id];
	if (pb.leftPressed) {
		an.isAnimating = true;
		ph.velocity.x += -pb.movespeed * delta_t;
	}
	else if (pb.rightPressed) {
		an.isAnimating = true;
		ph.velocity.x += pb.movespeed * delta_t;
	}
	else {
		ph.velocity.x += ph.velocity.x > 0 ? -pb.friction : pb.friction;
		an.isAnimating = false;
	}
	if (pb.spacePressed) {
		
		return JumpUp;
	}
	else if (!ph.bottomTouching) {
		return JumpDown;
	}
	if (IsStandingOnLadder(ph.collider, tr, tilelayers)) {
		if (pb.upPressed || (pb.downPressed && !IsAtLadderBottom(ph.collider,tr,tilelayers))){
			return Climb;
		}
	}
	if (pb.attackPressed) {
		return Stabbing;
	}

	return Walk;
}

void PlayerBehaviorSystem::WalkStateBehavior::OnEnter(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	auto& an = components.animations[id];
	an.animationName = "walk";
	an.numframes = 4;
	an.onframe = 0;
	an.fps = 10;
	an.shouldLoop = true;
}

void PlayerBehaviorSystem::WalkStateBehavior::OnExit(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	auto& an = components.animations[id];
	an.isAnimating = true;
}

#pragma endregion

#pragma region jump up state

PlayerState PlayerBehaviorSystem::JumpUpStateBehavior::Update(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	auto& tilelayers = engine._TileLayers;
	auto& pb = components.player_behaviors[id];
	auto& ph = components.physicses[id];
	auto& tr = components.transforms[id];
	if (pb.leftPressed) {
		ph.velocity.x += -pb.jumpmovespeed * delta_t;
	}
	else if (pb.rightPressed) {
		ph.velocity.x += pb.jumpmovespeed * delta_t;
	}
	else {
		ph.velocity.x += ph.velocity.x > 0 ? -1.0 : 1.0;
	}
	if (ph.velocity.y > 0) {
		return JumpDown;
	}
	if (IsStandingOnLadder(ph.collider, tr, tilelayers)) {
		if (pb.upPressed) {
			return Climb;
		}
	}
	if (pb.attackPressed) {
		return Stabbing;
	}
	return JumpUp;
}

void PlayerBehaviorSystem::JumpUpStateBehavior::OnEnter(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	auto& an = components.animations[id];
	auto& pb = components.player_behaviors[id];
	auto& ph = components.physicses[id];

	an.isAnimating = true;
	an.animationName = "jump_up";
	an.numframes = 3;
	an.fps = 5;
	an.onframe = 0;
	an.shouldLoop = false;

	pb.spacePressed = false;
	pb.jumping = true;
	pb.jumpcounter = 0;
	ph.velocity.y -= pb.JumpAmount;
}

void PlayerBehaviorSystem::JumpUpStateBehavior::OnExit(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	auto& pb = components.player_behaviors[id];
	pb.jumping = false;
}

#pragma endregion

#pragma region jump down state

PlayerState PlayerBehaviorSystem::JumpDownStateBehavior::Update(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	auto& tilelayers = engine._TileLayers;
	auto& pb = components.player_behaviors[id];
	auto& ph = components.physicses[id];
	auto& an = components.animations[id];
	auto& tr = components.transforms[id];
	if (pb.leftPressed) {
		an.isAnimating = true;
		ph.velocity.x += -pb.jumpmovespeed * delta_t;
	}
	else if (pb.rightPressed) {
		an.isAnimating = true;
		ph.velocity.x += pb.jumpmovespeed * delta_t;
	}
	if (ph.bottomTouching) {
		return JumpLand;
	}
	if (IsStandingOnLadder(ph.collider, tr, tilelayers)) {
		if (pb.upPressed) {
			return Climb;
		}
	}
	if (pb.attackPressed) {
		return Stabbing;
	}
	return JumpDown;
}

void PlayerBehaviorSystem::JumpDownStateBehavior::OnEnter(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	auto& an = components.animations[id];
	an.animationName = "jump_down";
	an.numframes = 1;
	an.onframe = 0;
	an.shouldLoop = true;
}

void PlayerBehaviorSystem::JumpDownStateBehavior::OnExit(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	
	
}

#pragma endregion

#pragma region jump land state

PlayerState PlayerBehaviorSystem::JumpLandStateBehavior::Update(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	auto& ph = components.physicses[id];
	auto& pb = components.player_behaviors[id];
	ph.velocity.x += ph.velocity.x > 0 ? -pb.friction : pb.friction;
	if (pb.leftPressed || pb.rightPressed) {
		return Walk;
	}
	if (pb.spacePressed) {
		
		return JumpUp;
	}
	if (pb.attackPressed) {
		return Stabbing;
	}
	return JumpLand;
}

void PlayerBehaviorSystem::JumpLandStateBehavior::OnEnter(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	auto& an = components.animations[id];
	an.numframes = 1;
	an.onframe = 0;
	an.animationName = "jump_land";
}

void PlayerBehaviorSystem::JumpLandStateBehavior::OnExit(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{	
}

#pragma endregion

#pragma region climb state

PlayerState PlayerBehaviorSystem::ClimbStateBehavior::Update(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	auto& tilelayers = engine._TileLayers;
	auto& ph = components.physicses[id];
	auto& pb = components.player_behaviors[id];
	auto& tr = components.transforms[id];
	auto& an = components.animations[id];

	ph.collider.Collidable = false;
	ph.velocity.x = 0;
	if (pb.upPressed) {
		ph.velocity.y = -pb.climbspeed;
		an.isAnimating = true;
	}
	else if (pb.downPressed) {
		ph.velocity.y = pb.climbspeed;
		an.isAnimating = true;
	}
	else {
		ph.velocity.y = 0;
		an.isAnimating = false;
	}
	if (!IsStandingOnLadder(ph.collider,tr,tilelayers)) {
		return Walk;
	}
	if (IsAtLadderBottom(ph.collider, tr, tilelayers)) {
		return Walk;
	}
	return Climb;
}

void PlayerBehaviorSystem::ClimbStateBehavior::OnEnter(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	auto& an = components.animations[id];
	an.animationName = "climb";
	an.numframes = 3;
	
	an.onframe = 0;
	an.shouldLoop = true;
}

void PlayerBehaviorSystem::ClimbStateBehavior::OnExit(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	auto& an = components.animations[id];
	auto& pb = components.player_behaviors[id];
	auto& ph = components.physicses[id];
	an.isAnimating = true;
	pb.spacePressed = false;
	ph.collider.Collidable = true;
	//pb.upPressed = false;
	//pb.downPressed = false;
}

#pragma endregion

#pragma region knock back state

PlayerState PlayerBehaviorSystem::KnockbackStateBehavior::Update(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	auto& pb = components.player_behaviors[id];
	auto& sp = components.sprites[id];

	pb.colliding_enemy = 0;

	pb.knockback_timer += delta_t;
	pb.knockback_blink_timer += delta_t;

	if (pb.knockback_blink_timer >= knockback_flash_time) {
		pb.knockback_blink_timer = 0;
		if (sp.shoulddraw) {
			sp.shoulddraw = false;
		}
		else {
			sp.shoulddraw = true;
		}
	}

	if (pb.knockback_timer >= knockback_time) {
		return Walk;
	}
	return KnockBack;
}

void PlayerBehaviorSystem::KnockbackStateBehavior::OnEnter(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	auto& pb = components.player_behaviors[id];
	auto& ph = components.physicses[id];
	auto& tr = components.transforms[id];
	auto& h = components.healths[id];

	auto& enemy_tr = components.transforms[pb.colliding_enemy];
	float xdir = tr.pos.x - enemy_tr.pos.x > 0 ? 1.0 : -1.0;

	pb.colliding_enemy = 0;
	ph.velocity.x = xdir * KNOCKBACK_AMOUNTX;

	components.player_behaviors[id].knockback_timer = 0;
	components.player_behaviors[id].knockback_blink_timer = 0;
}

void PlayerBehaviorSystem::KnockbackStateBehavior::OnExit(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	components.sprites[id].shoulddraw = true;
}

#pragma endregion

#pragma region dead state

PlayerState PlayerBehaviorSystem::DeadStateBehavior::Update(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	const auto& es = components.exploding_sprites[id];
	auto& ph = components.physicses[id];
	ph.velocity = glm::vec2(0.0, 0.0);
	if (es.finishedExploding) {
		return Walk;
	}
	return Dead;
}

void PlayerBehaviorSystem::DeadStateBehavior::OnEnter(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	const auto& sp = components.sprites[id];
	engine.AddComponentToEntity<ExplodingSprite>(components.exploding_sprites, ExplodingSprite{ sp.texture },id);
	engine.RemoveComponentFromEntity<Sprite>(components.sprites, id);
}

void PlayerBehaviorSystem::DeadStateBehavior::OnExit(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	const auto& es = components.exploding_sprites[id];
	// set colliding_enemy to zero so that player isn't knocked back when they come to life
	auto& pb = components.player_behaviors[id];
	pb.colliding_enemy = 0;
	// switch exploding sprite for normal one
	engine.AddComponentToEntity<Sprite>(components.sprites, Sprite{ es.texture }, id);
	engine.RemoveComponentFromEntity<ExplodingSprite>(components.exploding_sprites, id);
	//refill health
	auto& health = components.healths[id];
	health.current = health.max;
}

#pragma endregion

#pragma region stab state

PlayerState PlayerBehaviorSystem::StabStateBehavior::Update(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	auto& an = components.animations[id];
	if (an.isAnimating) {
		return Stabbing;
	}
	else {
		return Walk;
	}
}

void PlayerBehaviorSystem::StabStateBehavior::OnEnter(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	using namespace glm;
	auto& components = engine._Components;
	auto& tr = components.transforms[id];
	auto& an = components.animations[id];
	auto& pb = components.player_behaviors[id];
	auto& ph = components.physicses[id];
	pb.attackPressed = false;
	an.animationName = "stab";
	an.numframes = 3;
	an.onframe = 0;
	an.shouldLoop = false;
	an.isAnimating = true;
	an.fps = 8;
	ph.velocity.x = 0;

	HandleEnemyHits(ph, tr, engine, delta_t);
}

void PlayerBehaviorSystem::StabStateBehavior::OnExit(float delta_t, Camera2D& camera, Engine& engine, EntityID id)
{
	auto& components = engine._Components;
	auto& sp = components.sprites[id];
	auto& an = components.animations[id];
	sp.texture = 138;
	an.animationName = "walk";
}

void PlayerBehaviorSystem::StabStateBehavior::HandleEnemyHits(Physics& ph, Transform& tr, Engine& engine, float delta_t)
{
	using namespace glm;
	auto& components = engine._Components;
	vec2 physicsBR_p1(
		((tr.pos.x) + (abs(tr.scale.x) * 0.5)) - ph.collider.MinusPixelsRight,
		((tr.pos.y) + (tr.scale.y * 0.5)) - ph.collider.MinusPixelsBottom
	);
	vec2 physicsTL_p1(
		((tr.pos.x) - (abs(tr.scale.x) * 0.5)) + ph.collider.MinusPixelsRight,
		((tr.pos.y) - (tr.scale.y * 0.5)) + ph.collider.MinusPixelsTop
	);
	vec4 myTLBR(
		physicsTL_p1.y,
		physicsTL_p1.x,
		physicsBR_p1.y,
		physicsBR_p1.x
	);
	if (tr.scale.x > 0) {
		myTLBR[3] += 8;
	}
	else {
		myTLBR[1] -= 8;
	}
	OperateOnComponentGroup(CT_ENEMYBEHAVIOR, CT_PHYSICS, CT_TRANSFORM) {
		auto& enemy_tr = engine._Components.transforms[entityID];
		auto& enemy_ph = engine._Components.physicses[entityID];
		float pvelx = enemy_ph.velocity.x * delta_t;
		float pvely = enemy_ph.velocity.y * delta_t;
		vec2 physicsBR_enemy(
			((enemy_tr.pos.x + pvelx) + (abs(enemy_tr.scale.x) * 0.5)) - enemy_ph.collider.MinusPixelsRight,
			((enemy_tr.pos.y + pvely) + (enemy_tr.scale.y * 0.5)) - enemy_ph.collider.MinusPixelsBottom
		);
		vec2 physicsTL_enemy(
			((enemy_tr.pos.x + pvelx) - (abs(enemy_tr.scale.x) * 0.5)) + enemy_ph.collider.MinusPixelsRight,
			((enemy_tr.pos.y + pvely) - (enemy_tr.scale.y * 0.5)) + enemy_ph.collider.MinusPixelsTop
		);
		vec4 enemyTLBR(
			physicsTL_enemy.y,
			physicsTL_enemy.x,
			physicsBR_enemy.y,
			physicsBR_enemy.x
		);
		if (AABBCollision(myTLBR, enemyTLBR)) {
			engine.DeleteEntity(entityID);
		}
	}
}

#pragma endregion
