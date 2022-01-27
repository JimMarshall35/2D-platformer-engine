#pragma once
#include "ISystem.h"
#include <unordered_map>
#include <memory>
#include "Engine.h"

template <typename StateEnum>
class StateBehaviorBase {
public:
	StateBehaviorBase(Engine* e) { _Engine = e; }
	virtual StateEnum Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) = 0;
	virtual void OnEnter(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) = 0;
	virtual void OnExit(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id) = 0;
protected:
	Engine* _Engine;
};

template <typename StateEnum>
class StateMachineSystem : public ISystem
{
public:
	StateMachineSystem(Engine* e) :ISystem(e) { _Engine = e; };
	virtual bool DoGlobalTransitions(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers, EntityID id, PlayerState& newstate) = 0;
	// Inherited via ISystem
	EntityID _EntityID;
	Behavior<StateEnum>* _CurrentBehaviorComponent;
	virtual void Update(Components& components, float delta_t, Camera2D& camera, TileSet& tileset, std::vector<TileLayer>& tilelayers) override {
		StateEnum newstate;
		// check to see if there's bee a transition to a new state 
		if (_CurrentBehaviorComponent->state != _CurrentBehaviorComponent->laststate && _Behaviormap.find(_CurrentBehaviorComponent->laststate) != _Behaviormap.end()) {
			_Behaviormap[_CurrentBehaviorComponent->laststate]->OnExit(components, delta_t, camera, tileset, tilelayers, _EntityID);
			_Behaviormap[_CurrentBehaviorComponent->state]->OnEnter(components, delta_t, camera, tileset, tilelayers, _EntityID);
		}
		
		// run state machine to get new state
		newstate = _Behaviormap[_CurrentBehaviorComponent->state]->Update(components, delta_t, camera, tileset, tilelayers, _EntityID);
		_CurrentBehaviorComponent->laststate = _CurrentBehaviorComponent->state;
		_CurrentBehaviorComponent->state = newstate;

		// check for transitions that can occur in any state
		bool shouldChange = DoGlobalTransitions(components, delta_t, camera, tileset, tilelayers, _EntityID, newstate);
		if (shouldChange) {
			/*
			if a "global transition" has happened then overwrite the result set by the state machine
			and set the state to the one that DoGlobalTransitions has returned
			*/
			_CurrentBehaviorComponent->state = newstate;
			
		}
	}
	
protected:
	Engine* _Engine;
	std::unordered_map<StateEnum, std::unique_ptr<StateBehaviorBase<StateEnum>>> _Behaviormap;
};
