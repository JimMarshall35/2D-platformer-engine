#pragma once
#include "ISystem.h"
#include <unordered_map>
#include <memory>
#include "Engine.h"
#include <algorithm>    // std::find
#include <vector>       // std::vector


template <typename StateEnum>
class StateBehaviorBase {
public:
	StateBehaviorBase(Engine* e) { _Engine = e; }
	virtual StateEnum Update(float delta_t, Camera2D& camera, Engine& engine, EntityID id) = 0;
	virtual void OnEnter(float delta_t, Camera2D& camera, Engine& engine, EntityID id) = 0;
	virtual void OnExit(float delta_t, Camera2D& camera, Engine& engine, EntityID id) = 0;
protected:
	Engine* _Engine;
};

// StateEnum must be a sequential enum starting from zero with zero being "no state". 
template <typename StateEnum>
class StateMachineSystem : public ISystem
{
public:
	StateMachineSystem(Engine* engine) { _Engine = engine; };

	/// <summary>
	/// called from every state to check 
	/// things that can cause a transition from any
	/// state. IE "if health == 0 then die, if hit then knockback"
	/// </summary>
	/// <param name="delta_t"></param>
	/// <param name="camera"></param>
	/// <param name="engine"></param>
	/// <param name="newstate"> outputs a new state </param>
	/// <returns> bool representing whether there should be a transition to a new state </returns>
	virtual bool DoGlobalTransitions(float delta_t, Camera2D& camera, Engine& engine, PlayerState& newstate) = 0;

	virtual void Update(float delta_t, Camera2D& camera, Engine& engine) override {

		StateEnum& state = _CurrentBehaviorComponent->state;
		StateEnum& laststate = _CurrentBehaviorComponent->laststate;

		// check to see if there's been a transition to a new state 
		if (state != laststate 
			&& laststate > 0 && laststate < _Behaviormap.size()) {
			// call on exit and on enter functions
			_Behaviormap[laststate]->OnExit(delta_t, camera, engine, _EntityID);
			_Behaviormap[state]->OnEnter(delta_t, camera, engine, _EntityID);
		}
		
		// run state machine to get new state
		StateEnum newstate;
		newstate = _Behaviormap[state]->Update(delta_t, camera, engine, _EntityID);

		// set laststate and then new state 
		laststate = state;
		state = newstate;

		// check for transitions that can occur in any state 
		bool shouldChange = DoGlobalTransitions(delta_t, camera, engine, newstate);
		if (shouldChange) {
			// if a "global transition" has happened then overwrite the result set by the state machine
			// and set the state to the one that DoGlobalTransitions has returned
			state = newstate;
		}
	}
	
protected:
	EntityID _EntityID;
	BehaviorComponent<StateEnum>* _CurrentBehaviorComponent;
	std::vector<std::unique_ptr<StateBehaviorBase<StateEnum>>> _Behaviormap;
	Engine* _Engine;
};