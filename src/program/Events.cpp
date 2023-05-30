#include "Program.h"

#include "src/core/Logging.h"

void Program::init_events()
{
	EventHooks[HOOK_CONTINUOUS_STR] = { HOOK_CONTINUOUS, HOOK_CONTINUOUS_ARGS };
	EventHooks[HOOK_EVENT_DISABLED_STR] = { HOOK_EVENT_DISABLED, HOOK_EVENT_DISABLED_ARGS };
	EventHooks[HOOK_STATE_ENTERED_STR] = { HOOK_STATE_ENTERED, HOOK_STATE_ENTERED_ARGS };
	EventHooks[HOOK_START_STR] = { HOOK_START, HOOK_START_ARGS };
	EventHooks[HOOK_TIMER_STR] = { HOOK_TIMER, HOOK_TIMER_ARGS };
}

void Program::create_event(std::string name, std::string _hook, LuaFunction thread)
{
	int hook = HOOK_NULL;
	int args = HOOK_NULL;
	bool triggerable = false;

	// Check if we want to hook to callback at all.
	if (_hook.empty() == false)
	{
		triggerable = true;
		// Check if callback argument is a real possible callback we can hook to.
		for (auto& h : EventHooks)
		{
			if (_hook == h.first)
			{
				hook = h.second.first;
				args = h.second.second;
			}
		}

		if (hook == HOOK_NULL)
			LOG_ERROR("Attempted to hook to invalid callback: '" + _hook + "'");

		LOG_INFO("Created event: '" + name + "'; Hooked to: '" + _hook + "'");
	}
	else
		LOG_INFO("Created event: '" + name + "'");

	Chunk chunk = { name, "EVENT", args, 0, thread };
	Event* event = new Event(name, "EVENT", chunk, hook, true, triggerable);
	create_executable_object(event);

	EventLibrary[name] = event;
}

void Program::create_c_callback(std::string name, std::string _hook, C_EventCallback func)
{
	int hook = 0;
	for (auto& h : EventHooks)
	{
		if (_hook == h.first)
		{
			hook = h.second.first;
		}
	}

	if (hook == HOOK_NULL)
		LOG_ERROR("Attempted to hook to invalid callback: '" + _hook + "'");

	LOG_INFO("Created C event callback: '" + name + "'; Hooked to: '" + _hook + "'");

	Chunk chunk = { name, "C_EVENT_CALLBACK", 0, 0, LuaNullRef, func };
	Event* event = new Event(name, "C_EVENT", chunk, hook, true, true);
	create_executable_object(event);

	EventLibrary[name] = event;
}

void Program::call_event(lua_State* L, Event* _event)
{
	if (_event->chunk.c_callback == NULL)
		execute_object(L, _event);
	else
		_event->chunk.c_callback();

}

//void Program::disable_event(lua_State* L, Event* _event)
//{
//	_event.enabled = false;
//
//	lua_pushstring(L, _event.name.c_str()); // Pushing callback information, in order.
//
//	//Lua_DestroyChunkThread(_event.chunk.thread);
//	program->EventLibrary.erase(_event.name);
//
//	INVOKE_EVENT(HOOK_EVENT_DISABLED_STR);
//}

void Program::invoke_event(HOOK_TYPE _event)
{

	if (_event != 0)
	{
		for (auto& m : EventLibrary)
			if (m.second->hook == _event)
			{
				LOG_INFO("Invoked event: '" + m.first + "'");
				call_event(L(), m.second);
			}
	}
	else
		LOG_ERROR("Attempted to invoke invalid/unknown event hook.");
}