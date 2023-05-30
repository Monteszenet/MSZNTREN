#include "States.h"

#include "Program.h"

#include "src/core/Logging.h"



void Program::init_states()
{

}

void Program::create_state(std::string name, LuaFunction thread)
{
	Chunk chunk = { name, "STATE", 0, 0, thread };
	State* state = new State(name, "STATE", chunk);
	create_executable_object(state);

	StateLibrary[name] = state;
}

//void Program::jump_state(lua_State* L, State* _state)
//{
//	INVOKE_EVENT(HOOK_STATE_ENTERED_STR);
//
//	//for (int i = 0; i < StateCallStack.size(); i++)
//	//	StateCallStack.pop();
//	//StateCallStack.push(_state);
//
//	// We clear the stack without any chunk execution, and then push
//	// Our new sole chunk, the state.
//	callstack_clear(L);
//	callstack_push(L, &_state.chunk);
//}

void Program::call_state(lua_State* L, State* _state)
{
	INVOKE_EVENT(HOOK_STATE_ENTERED_STR);

	//StateCallStack.push(_state);

	//callstack_push(L, &_state->chunk);
}

//void Program::return_state(lua_State* L)
//{
//	callstack_pop(L);
//}