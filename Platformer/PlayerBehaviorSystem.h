#pragma once
#include "ISystem.h"
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
struct Animation;
struct Transform;
struct FloorCollider;
struct Transform;
struct PlayerBehavior;
struct Physics;

enum PlayerState : unsigned int;

class PlayerBehaviorSystem : public ISystem
{
public:
	PlayerBehaviorSystem(Engine* e);
	void Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers);
private:
	
	static bool IsStandingOnLadder(const FloorCollider& collider, const Transform& transform, std::vector<TileLayer>& tileLayers);
	static bool LadderAtCoordinates(const int x, const int y, std::vector<TileLayer>& tileLayers);
	static bool IsAtLadderBottom(const FloorCollider& collider, const Transform& transform, std::vector<TileLayer>& tileLayers);

	
	class IPlayerStateBehavior {
	public:
		virtual PlayerState Update(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers) = 0;
		virtual void OnEnter(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers) = 0;
		virtual void OnExit(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers) = 0;
	};

	class WalkStateBehavior : public IPlayerStateBehavior {
		// Inherited via PlayerStateBehaviorBase
		virtual PlayerState Update(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers) override;
		virtual void OnEnter(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers) override;
		virtual void OnExit(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers) override;
	};

	class JumpUpStateBehavior : public IPlayerStateBehavior {
		// Inherited via IPlayerStateBehavior
		virtual PlayerState Update(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers) override;
		virtual void OnEnter(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers) override;
		virtual void OnExit(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers) override;
	};

	class JumpDownStateBehavior : public IPlayerStateBehavior {
		// Inherited via IPlayerStateBehavior
		virtual PlayerState Update(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers) override;
		virtual void OnEnter(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers) override;
		virtual void OnExit(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers) override;
	};

	class JumpLandStateBehavior : public IPlayerStateBehavior {
		// Inherited via IPlayerStateBehavior
		virtual PlayerState Update(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers) override;
		virtual void OnEnter(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers) override;
		virtual void OnExit(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers) override;
	};

	class ClimbStateBehavior : public IPlayerStateBehavior {
		// Inherited via IPlayerStateBehavior
		virtual PlayerState Update(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers) override;
		virtual void OnEnter(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers) override;
		virtual void OnExit(PlayerBehavior& pb, Physics& ph, Transform& tr, Animation& an, double delta_t, std::vector<TileLayer>& tileLayers) override;
	};

	std::unordered_map<PlayerState, std::unique_ptr<IPlayerStateBehavior>> _Behaviormap;
	 
};