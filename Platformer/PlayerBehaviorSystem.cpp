#include "PlayerBehaviorSystem.h"

#include "Camera2D.h"
#include "Tileset.h"
#include <algorithm>
#include <iostream>
#include "Engine.h"

#define KNOCKBACK_AMOUNTX 200.0;
#define KNOCKBACK_AMOUNTY 1000.0;

void PlayerBehaviorSystem::Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers)
{
	OperateOnComponentGroup(CT_PHYSICS, CT_TRANSFORM, CT_PLAYERBEHAVIOR, CT_ANIMATION) {
		auto& pb = components.player_behaviors[entityID];
		auto& ph = components.physicses[entityID];
		auto& tr = components.transforms[entityID];
		auto& an = components.animations[entityID];

		
		PlayerState newstate;
		
		// run state machine
		newstate = _Behaviormap[pb.state]->Update(components,delta_t,camera,tileset,tilelayers,entityID);
		assert(newstate > NoState && newstate <= Dead);
		pb.laststate = pb.state;
		pb.state = newstate;
		if (pb.state != pb.laststate) {
			_Behaviormap[pb.laststate]->OnExit(components, delta_t, camera, tileset, tilelayers, entityID);
			_Behaviormap[pb.state]->OnEnter(components, delta_t, camera, tileset, tilelayers, entityID);
		}
		// do things common to all states
		if (pb.rightPressed) {
			if (tr.scale.x < 0) {
				tr.scale.x *= -1;
			}
		}
		else if (pb.leftPressed) {
			if (tr.scale.x > 0) {
				tr.scale.x *= -1;
			}
		}
		if (pb.state != KnockBack)
			ph.velocity.x = std::clamp(ph.velocity.x, -pb.MAX_X_SPEED, pb.MAX_X_SPEED);
		
		
		
		camera.FocusPosition = tr.pos;
	}
	
	
}

PlayerBehaviorSystem::PlayerBehaviorSystem(Engine* e)
	:ISystem(e)
{
	_Behaviormap.insert(std::make_pair<PlayerState, std::unique_ptr<PlayerStateBehaviorBase>>(Walk,      std::unique_ptr<PlayerStateBehaviorBase>(new WalkStateBehavior(_Engine))));
	_Behaviormap.insert(std::make_pair<PlayerState, std::unique_ptr<PlayerStateBehaviorBase>>(JumpUp,    std::unique_ptr<PlayerStateBehaviorBase>(new JumpUpStateBehavior(_Engine))));
	_Behaviormap.insert(std::make_pair<PlayerState, std::unique_ptr<PlayerStateBehaviorBase>>(JumpDown,  std::unique_ptr<PlayerStateBehaviorBase>(new JumpDownStateBehavior(_Engine))));
	_Behaviormap.insert(std::make_pair<PlayerState, std::unique_ptr<PlayerStateBehaviorBase>>(JumpLand,  std::unique_ptr<PlayerStateBehaviorBase>(new JumpLandStateBehavior(_Engine))));
	_Behaviormap.insert(std::make_pair<PlayerState, std::unique_ptr<PlayerStateBehaviorBase>>(Climb,     std::unique_ptr<PlayerStateBehaviorBase>(new ClimbStateBehavior(_Engine))));
	_Behaviormap.insert(std::make_pair<PlayerState, std::unique_ptr<PlayerStateBehaviorBase>>(KnockBack, std::unique_ptr<PlayerStateBehaviorBase>(new KnockbackStateBehavior(_Engine))));
	_Behaviormap.insert(std::make_pair<PlayerState, std::unique_ptr<PlayerStateBehaviorBase>>(Dead,      std::unique_ptr<PlayerStateBehaviorBase>(new DeadStateBehavior(_Engine))));
}

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
	auto bottomMid = vec2(pos.x, pos.y + ((bottomRight.y - topLeft.y) / 2 + 1.0f));
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

PlayerState PlayerBehaviorSystem::WalkStateBehavior::Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
	PlayerState statefrombase = PlayerStateBehaviorBase::Update(components, delta_t, camera, tileset, tilelayers, id);
	if (statefrombase != NoState) 
		return statefrombase;

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
		if (pb.upPressed) {
			an.isAnimating = true;
			return Climb;
		}
		if (pb.downPressed) {
			an.isAnimating = true;
			return Climb;
		}
	}

	return Walk;
}

void PlayerBehaviorSystem::WalkStateBehavior::OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
	auto& an = components.animations[id];
	an.animationName = "walk";
	an.numframes = 4;
	an.onframe = 0;
	an.fps = 10;
}

void PlayerBehaviorSystem::WalkStateBehavior::OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
	auto& an = components.animations[id];
	an.isAnimating = true;
}

PlayerState PlayerBehaviorSystem::JumpUpStateBehavior::Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
	PlayerState statefrombase = PlayerStateBehaviorBase::Update(components, delta_t, camera, tileset, tilelayers, id);
	if (statefrombase != NoState)
		return statefrombase;

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
	return JumpUp;
}

void PlayerBehaviorSystem::JumpUpStateBehavior::OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
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

void PlayerBehaviorSystem::JumpUpStateBehavior::OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
	auto& pb = components.player_behaviors[id];
	pb.jumping = false;
}

PlayerState PlayerBehaviorSystem::JumpDownStateBehavior::Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
	PlayerState statefrombase = PlayerStateBehaviorBase::Update(components, delta_t, camera, tileset, tilelayers, id);
	if (statefrombase != NoState)
		return statefrombase;

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
	return JumpDown;
}

void PlayerBehaviorSystem::JumpDownStateBehavior::OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
	auto& an = components.animations[id];
	an.animationName = "jump_down";
	an.numframes = 1;
	an.onframe = 0;
	an.shouldLoop = true;
}

void PlayerBehaviorSystem::JumpDownStateBehavior::OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
}

PlayerState PlayerBehaviorSystem::JumpLandStateBehavior::Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
	PlayerState statefrombase = PlayerStateBehaviorBase::Update(components, delta_t, camera, tileset, tilelayers, id);
	if (statefrombase != NoState)
		return statefrombase;

	auto& ph = components.physicses[id];
	auto& pb = components.player_behaviors[id];
	ph.velocity.x += ph.velocity.x > 0 ? -pb.friction : pb.friction;
	if (pb.leftPressed || pb.rightPressed) {
		pb.spacePressed = false;
		return Walk;
	}
	if (pb.spacePressed) {
		
		return JumpUp;
	}
	return JumpLand;
}

void PlayerBehaviorSystem::JumpLandStateBehavior::OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
	auto& an = components.animations[id];
	an.numframes = 1;
	an.onframe = 0;
	an.animationName = "jump_land";
}

void PlayerBehaviorSystem::JumpLandStateBehavior::OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
}

PlayerState PlayerBehaviorSystem::ClimbStateBehavior::Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
	PlayerState statefrombase = PlayerStateBehaviorBase::Update(components, delta_t, camera, tileset, tilelayers, id);
	if (statefrombase != NoState)
		return statefrombase;

	auto& ph = components.physicses[id];
	auto& pb = components.player_behaviors[id];
	auto& tr = components.transforms[id];
	ph.collider.Collidable = false;
	ph.velocity.x = 0;
	if (pb.upPressed) {
		ph.velocity.y = -pb.climbspeed;
	}
	else if (pb.downPressed) {
		ph.velocity.y = pb.climbspeed;;
	}
	else {
		ph.velocity.y = 0;
	}
	if (ph.bottomTouching && ph.velocity.y < 0) {
		ph.collider.Collidable = true;
		pb.spacePressed = false;
		return Walk;
	}
	if (IsAtLadderBottom(ph.collider, tr, tilelayers)) {
		pb.spacePressed = false;
		ph.collider.Collidable = true;
		return Walk;
	}
	return Climb;
}

void PlayerBehaviorSystem::ClimbStateBehavior::OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
	auto& an = components.animations[id];
	an.animationName = "climb";
	an.numframes = 3;
	an.onframe = 0;
	an.shouldLoop = true;
}

void PlayerBehaviorSystem::ClimbStateBehavior::OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
}

PlayerState PlayerBehaviorSystem::KnockbackStateBehavior::Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
	auto& pb = components.player_behaviors[id];
	auto& sp = components.sprites[id];

	pb.colliding_enemy = 0;
	PlayerState statefrombase = PlayerStateBehaviorBase::Update(components, delta_t, camera, tileset, tilelayers, id);
	if (statefrombase != NoState)
		return statefrombase;

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

void PlayerBehaviorSystem::KnockbackStateBehavior::OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
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

void PlayerBehaviorSystem::KnockbackStateBehavior::OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
	components.sprites[id].shoulddraw = true;
}

PlayerState PlayerBehaviorSystem::PlayerStateBehaviorBase::Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
	// check state transitions that are possible from all states
	auto& pb = components.player_behaviors[id];
	auto& ph = components.physicses[id];
	auto& an = components.animations[id];
	auto& tr = components.transforms[id];
	auto& health = components.healths[id];
	if (health.current == 0) {
		return Dead;
	}
	if (pb.colliding_enemy != 0) {
		health.current--;
		return KnockBack;
	}
	return NoState;
}

PlayerState PlayerBehaviorSystem::DeadStateBehavior::Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
	const auto& es = components.exploding_sprites[id];
	auto& ph = components.physicses[id];
	ph.velocity = glm::vec2(0.0, 0.0);
	if (es.finishedExploding) {
		return Walk;
	}
	return Dead;
}

void PlayerBehaviorSystem::DeadStateBehavior::OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
	const auto& sp = components.sprites[id];
	_Engine->AddComponentToEntity<ExplodingSprite>(components.exploding_sprites, ExplodingSprite{ sp.texture },id);
	_Engine->RemoveComponentFromEntity<Sprite>(components.sprites, id);
}

void PlayerBehaviorSystem::DeadStateBehavior::OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id)
{
	const auto& es = components.exploding_sprites[id];
	// set colliding_enemy to zero so that player isn't knocked back when they come to life
	auto& pb = components.player_behaviors[id];
	pb.colliding_enemy = 0;
	// switch exploding sprite for normal one
	_Engine->AddComponentToEntity<Sprite>(components.sprites, Sprite{ es.texture }, id);
	_Engine->RemoveComponentFromEntity<ExplodingSprite>(components.exploding_sprites, id);
	//refill health
	auto& health = components.healths[id];
	health.current = health.max;
}
