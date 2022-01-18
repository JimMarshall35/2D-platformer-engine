#include "PlayerBehaviorSystem.h"
#include "ECS.h"
#include "Camera2D.h"
#include "Tileset.h"
#include <algorithm>
bool AABBCollision(const Transform& t1, const Transform& t2, const FloorCollider& c1, const FloorCollider& c2)
{
	using namespace glm;
	vec4 bb1, bb2;

	return ((bb1[3] > bb2[1]) && (bb2[3] > bb1[1])) &&
		((bb1[2] > bb2[0]) && (bb2[2] > bb1[0]));
}


void PlayerBehaviorSystem::Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers)
{
	OperateOnComponentGroup(CT_PHYSICS, CT_TRANSFORM, CT_PLAYERBEHAVIOR, CT_ANIMATION) {
		auto& pb = components.player_behaviors[entityID];
		auto& ph = components.physicses[entityID];
		auto& tr = components.transforms[entityID];
		auto& an = components.animations[entityID];
		
		// run state machine
		PlayerState newstate = _Behaviormap[pb.state]->Update(pb,ph,tr,an,delta_t, tilelayers);
		assert(newstate > NoState && newstate <= Climb);
		pb.laststate = pb.state;
		pb.state = newstate;
		if (pb.state != pb.laststate) {
			_Behaviormap[pb.laststate]->OnExit(pb, ph, tr, an, delta_t, tilelayers);
			_Behaviormap[pb.state]->OnEnter(pb, ph, tr, an, delta_t, tilelayers);
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
		ph.velocity.x = std::clamp(components.physicses[entityID].velocity.x, -pb.MAX_X_SPEED, pb.MAX_X_SPEED);
		camera.FocusPosition = tr.pos;
		
	}
	
	
}

PlayerBehaviorSystem::PlayerBehaviorSystem()
{
	_Behaviormap.insert(std::make_pair<PlayerState, std::unique_ptr<IPlayerStateBehavior>>(Walk,     std::unique_ptr<IPlayerStateBehavior>(new WalkStateBehavior())));
	_Behaviormap.insert(std::make_pair<PlayerState, std::unique_ptr<IPlayerStateBehavior>>(JumpUp,   std::unique_ptr<IPlayerStateBehavior>(new JumpUpStateBehavior())));
	_Behaviormap.insert(std::make_pair<PlayerState, std::unique_ptr<IPlayerStateBehavior>>(JumpDown, std::unique_ptr<IPlayerStateBehavior>(new JumpDownStateBehavior())));
	_Behaviormap.insert(std::make_pair<PlayerState, std::unique_ptr<IPlayerStateBehavior>>(JumpLand, std::unique_ptr<IPlayerStateBehavior>(new JumpLandStateBehavior())));
	_Behaviormap.insert(std::make_pair<PlayerState, std::unique_ptr<IPlayerStateBehavior>>(Climb,    std::unique_ptr<IPlayerStateBehavior>(new ClimbStateBehavior())));
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

PlayerState PlayerBehaviorSystem::WalkStateBehavior::Update(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tilelayers)
{
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
		pb.spacePressed = false;
		pb.jumping = true;
		pb.jumpcounter = 0;
		ph.velocity.y -= pb.JumpAmount;
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

void PlayerBehaviorSystem::WalkStateBehavior::OnEnter(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers)
{
	an.animationName = "walk";
	an.numframes = 4;
	an.onframe = 0;
	an.fps = 10;
}

void PlayerBehaviorSystem::WalkStateBehavior::OnExit(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers)
{
	an.isAnimating = true;
}

PlayerState PlayerBehaviorSystem::JumpUpStateBehavior::Update(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tilelayers)
{
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

void PlayerBehaviorSystem::JumpUpStateBehavior::OnEnter(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers)
{
	an.isAnimating = true;
	an.animationName = "jump_up";
	an.numframes = 3;
	an.fps = 5;
	an.onframe = 0;
	an.shouldLoop = false;
}

void PlayerBehaviorSystem::JumpUpStateBehavior::OnExit(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers)
{
}

PlayerState PlayerBehaviorSystem::JumpDownStateBehavior::Update(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tilelayers)
{
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

void PlayerBehaviorSystem::JumpDownStateBehavior::OnEnter(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers)
{
	an.animationName = "jump_down";
	an.numframes = 1;
	an.onframe = 0;
	an.shouldLoop = true;
}

void PlayerBehaviorSystem::JumpDownStateBehavior::OnExit(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers)
{
}

PlayerState PlayerBehaviorSystem::JumpLandStateBehavior::Update(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers)
{
	ph.velocity.x += ph.velocity.x > 0 ? -pb.friction : pb.friction;
	if (pb.leftPressed || pb.rightPressed) {
		pb.spacePressed = false;
		return Walk;
	}
	if (pb.spacePressed) {
		pb.spacePressed = false;
		pb.jumping = true;
		pb.jumpcounter = 0;
		ph.velocity.y -= pb.JumpAmount;
		return JumpUp;
	}
	return JumpLand;
}

void PlayerBehaviorSystem::JumpLandStateBehavior::OnEnter(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers)
{
	an.numframes = 1;
	an.onframe = 0;
	an.animationName = "jump_land";
}

void PlayerBehaviorSystem::JumpLandStateBehavior::OnExit(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers)
{
}

PlayerState PlayerBehaviorSystem::ClimbStateBehavior::Update(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tilelayers)
{
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

void PlayerBehaviorSystem::ClimbStateBehavior::OnEnter(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers)
{
	
	an.animationName = "climb";
	an.numframes = 3;
	an.onframe = 0;
	an.shouldLoop = true;
}

void PlayerBehaviorSystem::ClimbStateBehavior::OnExit(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers)
{
}
