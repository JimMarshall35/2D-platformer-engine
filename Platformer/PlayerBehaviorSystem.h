#pragma once
#include "ISystem.h"
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include "StateMachineSystem.h"
using EntityID = int64_t;

enum PlayerState : unsigned int;

class PlayerBehaviorSystem : public StateMachineSystem<PlayerState>
{
public:
	PlayerBehaviorSystem(Engine* e);
	void Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers);
private:
	
	static bool IsStandingOnLadder(const FloorCollider& collider, const Transform& transform, std::vector<TileLayer>& tileLayers);
	static bool LadderAtCoordinates(const int x, const int y, std::vector<TileLayer>& tileLayers);
	static bool IsAtLadderBottom(const FloorCollider& collider, const Transform& transform, std::vector<TileLayer>& tileLayers);


	class WalkStateBehavior : public StateBehaviorBase<PlayerState> {
	public:
		WalkStateBehavior(Engine* e):StateBehaviorBase<PlayerState>(e){}
		// Inherited via PlayerStateBehaviorBase
		virtual PlayerState Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
	};

	class JumpUpStateBehavior : public StateBehaviorBase<PlayerState> {
	public:
		JumpUpStateBehavior(Engine* e) :StateBehaviorBase<PlayerState>(e) {}
		// Inherited via IPlayerStateBehavior
		virtual PlayerState Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
	};

	class JumpDownStateBehavior : public StateBehaviorBase<PlayerState> {
	public:
		JumpDownStateBehavior(Engine* e) :StateBehaviorBase<PlayerState>(e) {}
		// Inherited via IPlayerStateBehavior
		virtual PlayerState Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
	};

	class JumpLandStateBehavior : public StateBehaviorBase<PlayerState> {
	public:
		JumpLandStateBehavior(Engine* e) :StateBehaviorBase<PlayerState>(e) {}
		// Inherited via IPlayerStateBehavior
		virtual PlayerState Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
	};

	class ClimbStateBehavior : public StateBehaviorBase<PlayerState> {
	public:
		ClimbStateBehavior(Engine* e) :StateBehaviorBase<PlayerState>(e) {}
		// Inherited via IPlayerStateBehavior
		virtual PlayerState Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
	};

	class KnockbackStateBehavior : public StateBehaviorBase<PlayerState> {
	public:
		KnockbackStateBehavior(Engine* e) :StateBehaviorBase<PlayerState>(e) {}
		// Inherited via IPlayerStateBehavior
		virtual PlayerState Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
	private:
		const double knockback_time = 0.250;
		const double knockback_flash_time = 0.05;
	};

	class DeadStateBehavior : public StateBehaviorBase<PlayerState> {
	public:
		DeadStateBehavior(Engine* e) :StateBehaviorBase<PlayerState>(e) {}
		// Inherited via PlayerStateBehaviorBase
		virtual PlayerState Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
	};	 

	// Inherited via StateMachineSystem
	virtual bool DoGlobalTransitions(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id, PlayerState& newstate) override;
};