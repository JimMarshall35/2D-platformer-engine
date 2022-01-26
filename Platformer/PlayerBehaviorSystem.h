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
using EntityID = int64_t;

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

	
	class PlayerStateBehaviorBase {
	public:
		PlayerStateBehaviorBase(Engine* e){_Engine = e;}
		virtual PlayerState Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id);
		virtual void OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) = 0;
		virtual void OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) = 0;
	protected:
		Engine* _Engine;
	};

	class WalkStateBehavior : public PlayerStateBehaviorBase {
	public:
		WalkStateBehavior(Engine* e):PlayerStateBehaviorBase(e){}
		// Inherited via PlayerStateBehaviorBase
		virtual PlayerState Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
	};

	class JumpUpStateBehavior : public PlayerStateBehaviorBase {
	public:
		JumpUpStateBehavior(Engine* e) :PlayerStateBehaviorBase(e) {}
		// Inherited via IPlayerStateBehavior
		virtual PlayerState Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
	};

	class JumpDownStateBehavior : public PlayerStateBehaviorBase {
	public:
		JumpDownStateBehavior(Engine* e) :PlayerStateBehaviorBase(e) {}
		// Inherited via IPlayerStateBehavior
		virtual PlayerState Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
	};

	class JumpLandStateBehavior : public PlayerStateBehaviorBase {
	public:
		JumpLandStateBehavior(Engine* e) :PlayerStateBehaviorBase(e) {}
		// Inherited via IPlayerStateBehavior
		virtual PlayerState Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
	};

	class ClimbStateBehavior : public PlayerStateBehaviorBase {
	public:
		ClimbStateBehavior(Engine* e) :PlayerStateBehaviorBase(e) {}
		// Inherited via IPlayerStateBehavior
		virtual PlayerState Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
	};

	class KnockbackStateBehavior : public PlayerStateBehaviorBase {
	public:
		KnockbackStateBehavior(Engine* e) :PlayerStateBehaviorBase(e) {}
		// Inherited via IPlayerStateBehavior
		virtual PlayerState Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
	private:
		const double knockback_time = 0.250;
		const double knockback_flash_time = 0.05;
	};

	class DeadStateBehavior : public PlayerStateBehaviorBase {
	public:
		DeadStateBehavior(Engine* e) :PlayerStateBehaviorBase(e) {}
		// Inherited via PlayerStateBehaviorBase
		virtual PlayerState Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
		virtual void OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) override;
	};

	std::unordered_map<PlayerState, std::unique_ptr<PlayerStateBehaviorBase>> _Behaviormap;
	 
};