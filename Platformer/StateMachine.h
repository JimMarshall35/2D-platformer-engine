#pragma once
#include <unordered_map>
#include <memory>
#include "Engine.h"
#include <algorithm>    // std::find
#include <vector>


template <typename StateEnum>
class StateBehaviorBase {
public:
	virtual StateEnum Update(float delta_t, Camera2D& camera, Engine& engine, EntityID id) = 0;
	virtual void OnEnter(float delta_t, Camera2D& camera, Engine& engine, EntityID id) = 0;
	virtual void OnExit(float delta_t, Camera2D& camera, Engine& engine, EntityID id) = 0;
};

// StateEnum must be a sequential enum starting from zero with zero being "no state". 
template <typename StateEnum>
class StateMachine
{
public:
	void RunStateMachine(float delta_t, Camera2D& camera, Engine& engine, BehaviorComponent<PlayerState>& bc, EntityID id) {

		StateEnum& state = bc.state;
		StateEnum& laststate = bc.laststate;

		// check to see if there's been a transition to a new state 
		if (state != laststate 
			&& laststate > 0 && laststate < _StatesMap.size()) {
			// call on exit and on enter functions
			_StatesMap[laststate]->OnExit(delta_t, camera, engine, id);
			_StatesMap[state]->OnEnter(delta_t, camera, engine, id);
		}
		
		// call update on current state to get new state
		StateEnum newstate;
		newstate = _StatesMap[state]->Update(delta_t, camera, engine, id);

		// set laststate and then new state 
		laststate = state;
		state = newstate;

		// check for transitions that can occur in any state 
		bool shouldChange = DoGlobalTransitions(delta_t, camera, engine,id, newstate);
		if (shouldChange) {
			// if a "global transition" has happened then overwrite the result set by the state machine
			// and set the state to the one that DoGlobalTransitions has returned
			state = newstate;
		}
	}
protected:
	/// <param name="newstate"> outputs a new state </param>
	/// <returns> bool representing whether there should be a transition to the new state </returns>
	virtual bool DoGlobalTransitions(float delta_t, Camera2D& camera, Engine& engine, EntityID id, PlayerState& newstate) = 0;
	std::vector<std::unique_ptr<StateBehaviorBase<StateEnum>>> _StatesMap;
};