#pragma once

enum State {
	IDLE,
	STEP_IN,
	GOTO_SPOT,
	RAISE_HAND,
	SELECTION,
	SELECTION_POST,
	RESULT,
	PROFILE_CONFIRMED,
	MORE_THAN_ONE
};

class AppState {

public:

	static const string toString(const State& state)
	{
		string str;
#define X(state) case State::state: str = #state; break;
		switch (state)
		{
			X(IDLE);
			X(STEP_IN);
			X(GOTO_SPOT);
			X(RAISE_HAND);
			X(SELECTION);
			X(SELECTION_POST);
			X(RESULT);
			X(PROFILE_CONFIRMED);
			X(MORE_THAN_ONE);
		default:
			str = "undefined";
		}
#undef X

		return str;
	}

	static const std::vector<std::string> getStates()
	{
		std::vector<std::string> states;
#define X(state) states.push_back(#state);
		X(IDLE);
		X(STEP_IN);
		X(GOTO_SPOT);
		X(RAISE_HAND);
		X(SELECTION);
		X(SELECTION_POST);
		X(RESULT);
		X(PROFILE_CONFIRMED);
		X(MORE_THAN_ONE);
#undef X
		return states;
	}

};
