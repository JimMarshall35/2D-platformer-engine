#include "PlayerBehaviorSystem.h"
#include "ECS.h"
#include "Camera2D.h"
#include "Tileset.h"
#include <algorithm>

void PlayerBehaviorSystem::Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers)
{
	const float MAX_X_SPEED = 100;
	const float movespeed = 700;
	const float jumpmovespeed = 700;
	const float JumpAmount = 250;
	const float climbspeed = 50;

	OperateOnComponentGroup(CT_PHYSICS, CT_TRANSFORM, CT_PLAYERBEHAVIOR, CT_ANIMATION) {
		auto& value = components.player_behaviors[entityID];
		auto& phys = components.physicses[entityID];
		auto& transform = components.transforms[entityID];
		auto& animation = components.animations[entityID];
		auto& collider = phys.collider;
		_LastVel = phys.velocity;
		switch (_State) {
		case Walk:
			if (value.leftPressed) {
				animation.isAnimating = true;
				components.physicses[entityID].velocity.x += -movespeed * delta_t;
			}
			else if (value.rightPressed) {
				animation.isAnimating = true;
				components.physicses[entityID].velocity.x += movespeed * delta_t;
			}
			else {
				components.physicses[entityID].velocity.x += components.physicses[entityID].velocity.x > 0 ? -1.0 : 1.0;
				animation.isAnimating = false;
			}
			if (value.spacePressed) {
				value.spacePressed = false;
				value.jumping = true;
				value.jumpcounter = 0;
				phys.velocity.y -= JumpAmount;
				ChangeState(JumpUp, animation);
			}
			else if (!phys.bottomTouching) {
				ChangeState(JumpDown, animation);
			}
			if (IsStandingOnLadder(collider, transform, tilelayers)) {
				if (value.upPressed) {
					animation.isAnimating = true;
					ChangeState(Climb, animation);
				}
				if (value.downPressed) {
					animation.isAnimating = true;
					ChangeState(Climb, animation);
				}
			}

			break;
		case JumpUp:
			if (value.leftPressed) {
				animation.isAnimating = true;
				components.physicses[entityID].velocity.x += -jumpmovespeed * delta_t;
			}
			else if (value.rightPressed) {
				animation.isAnimating = true;
				components.physicses[entityID].velocity.x += jumpmovespeed * delta_t;
			}
			else {
				animation.isAnimating = true;
				components.physicses[entityID].velocity.x += components.physicses[entityID].velocity.x > 0 ? -1.0 : 1.0;
			}
			if (phys.velocity.y > 0) {
				ChangeState(JumpDown, animation);
			}
			if (IsStandingOnLadder(collider, transform, tilelayers)) {
				if (value.upPressed) {
					ChangeState(Climb, animation);
				}
			}
			break;
		case JumpDown:
			if (phys.bottomTouching) {
				ChangeState(JumpLand, animation);
			}
			if (IsStandingOnLadder(collider, transform, tilelayers)) {
				if (value.upPressed) {
					ChangeState(Climb, animation);
				}
			}
			break;
		case JumpLand:
			if (value.leftPressed || value.rightPressed) {
				ChangeState(Walk, animation);
			}
			if (value.spacePressed) {
				value.spacePressed = false;
				value.jumping = true;
				value.jumpcounter = 0;
				phys.velocity.y -= JumpAmount;
				ChangeState(JumpUp, animation);
			}
			components.physicses[entityID].velocity.x += components.physicses[entityID].velocity.x > 0 ? -1.0 : 1.0;
			break;
		case Climb:
			collider.Collidable = false;
			phys.velocity.x = 0;
			if (value.upPressed) {
				phys.velocity.y = -climbspeed;
			}
			else if (value.downPressed) {
				phys.velocity.y = climbspeed;;
			}
			else {
				phys.velocity.y = 0;
			}
			if (phys.bottomTouching && phys.velocity.y < 0) {
					ChangeState(Walk, animation);
					collider.Collidable = true;
			}
			if (IsAtLadderBottom(collider, transform, tilelayers)) {
				ChangeState(Walk, animation);
				collider.Collidable = true;
			}
			break;
		}
		if (value.rightPressed) {
			if (transform.scale.x < 0) {
				transform.scale.x *= -1;
			}
		}
		else if (value.leftPressed) {
			if (transform.scale.x > 0) {
				transform.scale.x *= -1;
			}
		}
		components.physicses[entityID].velocity.x = std::clamp(components.physicses[entityID].velocity.x, -MAX_X_SPEED, MAX_X_SPEED);
		camera.FocusPosition = transform.pos;
	}
	
}

void PlayerBehaviorSystem::ChangeState(AnimationState newState, Animation& animation)
{
	switch (newState)
	{
	case Walk:
		animation.animationName = "walk";
		animation.numframes = 4;
		animation.onframe = 0;
		animation.fps = 10;
		break;
	case JumpUp:
		animation.animationName = "jump_up";
		animation.numframes = 3;
		animation.fps = 5;
		animation.onframe = 0;
		animation.shouldLoop = false;
		break;
	case JumpDown:
		animation.animationName = "jump_down";
		animation.numframes = 1;
		animation.onframe = 0;
		animation.shouldLoop = true;
		break;
	case JumpLand:
		animation.numframes = 1;
		animation.onframe = 0;
		animation.animationName = "jump_land";
		break;
	case Climb:
		animation.animationName = "climb";
		animation.numframes = 3;
		animation.onframe = 0;
		animation.shouldLoop = true;
		break;
	default:
		break;
	}
	_State = newState;
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
