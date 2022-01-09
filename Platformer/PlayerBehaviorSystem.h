#pragma once
#include "ISystem.h"
#include <glm/glm.hpp>
struct Animation;
struct Transform;
struct FloorCollider;

class PlayerBehaviorSystem : public ISystem
{
public:
	void Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers);
private:
	enum AnimationState {
		Walk,
		JumpUp,
		JumpDown,
		JumpLand, 
		Climb
	};
	glm::vec2 _LastVel;
	AnimationState _State = Walk;
	void ChangeState(AnimationState newState, Animation& animationComponent);
	bool IsStandingOnLadder(const FloorCollider& collider, const Transform& transform, std::vector<TileLayer>& tileLayers);
	bool LadderAtCoordinates(const int x, const int y, std::vector<TileLayer>& tileLayers);
	bool IsAtLadderBottom(const FloorCollider& collider, const Transform& transform, std::vector<TileLayer>& tileLayers);
};

