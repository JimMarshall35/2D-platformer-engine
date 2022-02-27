#pragma once
#include <functional>

namespace SM {
	struct TransitionInfo {

	};

	using EnterExitTransitionHandler = std::function<bool(TransitionInfo)>;


	template <typename...> class IState;
	template <typename...> class State;

	template <typename TState, typename TTrigger>
	class IState<TState, TTrigger> {
		
	};

	template <typename TState, typename TTrigger>
	class State<TState, TTrigger> {
	private:
		EnterExitTransitionHandler _enterHandler;
		EnterExitTransitionHandler _exitHandler;

	};


};// end of namespace SM