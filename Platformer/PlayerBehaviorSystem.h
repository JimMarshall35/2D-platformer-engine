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
	PlayerBehaviorSystem(Engine* engine);
	void Update(float delta_t, Camera2D& camera, Engine& engine) override;
private:
	
	static bool IsStandingOnLadder(const FloorCollider& collider, const Transform& transform, std::vector<TileLayer>& tileLayers);
	static bool LadderAtCoordinates(const int x, const int y, std::vector<TileLayer>& tileLayers);
	static bool IsAtLadderBottom(const FloorCollider& collider, const Transform& transform, std::vector<TileLayer>& tileLayers);


	class WalkStateBehavior : public StateBehaviorBase<PlayerState> {
	public:
		// Inherited via PlayerStateBehaviorBase
		virtual PlayerState Update(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
		virtual void OnEnter(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
		virtual void OnExit(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
	};

	class JumpUpStateBehavior : public StateBehaviorBase<PlayerState> {
	public:
		// Inherited via IPlayerStateBehavior
		virtual PlayerState Update(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
		virtual void OnEnter(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
		virtual void OnExit(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
	};

	class JumpDownStateBehavior : public StateBehaviorBase<PlayerState> {
	public:
		// Inherited via IPlayerStateBehavior
		virtual PlayerState Update(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
		virtual void OnEnter(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
		virtual void OnExit(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
	};

	class JumpLandStateBehavior : public StateBehaviorBase<PlayerState> {
	public:
		// Inherited via IPlayerStateBehavior
		virtual PlayerState Update(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
		virtual void OnEnter(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
		virtual void OnExit(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
	};

	class ClimbStateBehavior : public StateBehaviorBase<PlayerState> {
	public:
		// Inherited via IPlayerStateBehavior
		virtual PlayerState Update(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
		virtual void OnEnter(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
		virtual void OnExit(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
	};

	class KnockbackStateBehavior : public StateBehaviorBase<PlayerState> {
	public:
		// Inherited via IPlayerStateBehavior
		virtual PlayerState Update(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
		virtual void OnEnter(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
		virtual void OnExit(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
	private:
		const double knockback_time = 0.250;
		const double knockback_flash_time = 0.05;
	};

	class DeadStateBehavior : public StateBehaviorBase<PlayerState> {
	public:
		// Inherited via PlayerStateBehaviorBase
		virtual PlayerState Update(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
		virtual void OnEnter(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
		virtual void OnExit(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
	};	 

	class StabStateBehavior : public StateBehaviorBase<PlayerState> {
	public:
		// Inherited via StateBehaviorBase
		virtual PlayerState Update(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
		virtual void OnEnter(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
		virtual void OnExit(float delta_t, Camera2D& camera, Engine& engine, EntityID id) override;
	};

	// Inherited via StateMachineSystem
	virtual bool DoGlobalTransitions(float delta_t, Camera2D& camera, Engine& engine, EntityID id, PlayerState& newstate) override;
};