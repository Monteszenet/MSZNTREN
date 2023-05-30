#include "Scripts.h"

#include <unordered_map>
#include <stack>

#include "Application.h"
#include "Logging.h"
#include "LuaGlue.h"

struct Scripts
{
	lua_State* L = NULL;
	std::unordered_map<M_SCRIPT, std::string> scripts;
	std::stack<LuaThread> threads_executing;

} scripts;

lua_State* L = scripts.L;

bool CheckLua(int r);



void EnterScriptingContext()
{
	L = luaL_newstate();

	luaopen_base(L);
	luaopen_string(L);
	luaopen_math(L);
	luaL_openlibs(L);
}

void ExitScriptingContext()
{
	lua_close(L);
}

lua_State* GetLuaState()
{
	return L;
}

const char* m_getluastring(lua_State* L, int idx)
{
	const char* str = lua_tostring(L, -1);
	if (str == NULL)
		LOG_FATAL("Failed string conversion.");
	return str;
}

bool CheckLua(int r)
{
	if (r != LUA_OK)
	{
		std::string lua_error = LUA_GETSTRING(L, -1);
		LOG_ERROR("[LUA ERROR] " + lua_error);
		return false;
	}
	return true;
}

void InitLuaEnvironment()
{
	/* Logging enums */ {
		lua_pushinteger(L, LOGTYPE_LUA);
		lua_setglobal(L, "LOG_LUA");
		lua_pushinteger(L, LOGTYPE_DEBUG);
		lua_setglobal(L, "LOG_DEBUG");
		lua_pushinteger(L, LOGTYPE_INFO);
		lua_setglobal(L, "LOG_INFO");
		lua_pushinteger(L, LOGTYPE_WARN);
		lua_setglobal(L, "LOG_WARN");
		lua_pushinteger(L, LOGTYPE_ERROR);
		lua_setglobal(L, "LOG_ERROR");
		lua_pushinteger(L, LOGTYPE_FATAL);
		lua_setglobal(L, "LOG_FATAL");
	}

	lua_register(L, "APP_CONFIG", L_AppConfig);
	lua_register(L, "LOG", L_Log);
	lua_register(L, "PRINT", L_Print);
}

void Lua_RegisterFunction(std::string name, LuaCFunction function)
{
	lua_register(L, name.c_str(), function);
}



void CompileLuaToMSZNT(std::string path)
{
	scripts.scripts[NULL_SCRIPT] = "";
	scripts.scripts[(M_SCRIPT)scripts.scripts.size()] = "game/scripts/config.lua";
}

M_SCRIPT Lua_GetScript(std::string script)
{
	for (auto& s : scripts.scripts)
	{
		if (s.second == script)
			return s.first;
	}
	LOG_ERROR("Couldn't locate script: " + script);
	return NULL_SCRIPT;
}

void Lua_RunScript(M_SCRIPT script)
{
	if (scripts.scripts.find(script) != scripts.scripts.end())
	{
		if (CheckLua(luaL_dofile(L, scripts.scripts[script].c_str())) == false)
			LOG_ERROR("Failed to run script.");
#ifdef M_DEBUG
		LOG_INFO("Ran script: '" + scripts.scripts[script] + "'");
#endif
		return;
	}
	else
	{
		LOG_ERROR("Attempted to run invalid/unknown script.");
		return;
	}
}

void Lua_RunScriptFile(std::string path)
{
	if (CheckLua(luaL_dofile(L, path.c_str())) == false)
		LOG_ERROR("Failed to run script from path: " + path);
#ifdef M_DEBUG
	LOG_INFO("Ran script: '" + path + "'");
#endif
}

void Lua_RunScript(std::string script)
{
	if (CheckLua(luaL_dostring(L, script.c_str())) == false)
		LOG_ERROR("Failed to run script:\n" + script);
#ifdef M_DEBUG
	LOG_INFO("Ran script:\n" + script);
#endif
}



LuaThread Lua_CreateChunkThread()
{
	lua_State* func_thread = lua_newthread(L); // Pushes new thread to top of stack.
	lua_pushvalue(L, -2); // Pushes function at -2 now to top of stack.
	lua_xmove(L, func_thread, 1); //Moves the function to the top of the new stack, and pops it off the old.
	return luaL_ref(L, LUA_REGISTRYINDEX); // We now return a reference to that thread.
}

void Lua_DestroyChunkThread(LuaThread thread)
{
	luaL_unref(L, LUA_REGISTRYINDEX, thread);
}

int Lua_ExecuteChunk(LuaFunction chunk, int args, int results)
{
	// TODO

	//lua_call(L, args, results);
	return 0;
}

int Lua_ExecuteChunkThread(LuaThread thread, int args, int* results)
{
	// Get thread after checking its validity.
	lua_State* func_thread = Lua_CheckThread(thread);

	// The caller will have pushed any functions to this chunk already, in order.
	// Here, we move (+ pop!) those to the top of this thread's stack.
	for (int i = 0; i < args; i++)
		lua_xmove(L, func_thread, i - args);

	// We count our threads and their execution.
	scripts.threads_executing.push(thread);

	// Takes a function at stack-top of the given thread.
	int result = lua_resume(func_thread, NULL, args, results);
	// We return here once yield is called on the thread or if the thread finishes
	// execution by itself.

	// We then pop it if it returned by finishing execution.
	if (result == LUA_OK)
		scripts.threads_executing.pop();
	// If it yielded, we do nothing. Thats for the caller to deal with.
	else if (result == LUA_YIELD)
		;
	// Else, it returned an error.
	else
		LOG_FATAL("Error while executing thread.\n[LUA ERROR] " + C(LUA_GETSTRING(func_thread, -1)));

	return result;
}
void Lua_m_SuspendChunkThread()
{
	if (scripts.threads_executing.size() == 0)
		LOG_FATAL("Attempted to suspend thread execution while no threads are executing.");
	scripts.threads_executing.pop();
}

void Lua_CheckChunk(LuaFunction func)
{
	// TODO
	//// Push function reference to stack-top.
	//lua_rawgeti(L, LUA_REGISTRYINDEX, func);
	//lua_State* func_thread = lua_tothread(L, -1);
	//lua_pop(L, 1);
}

lua_State* Lua_CheckThread(LuaThread thread)
{
	// Push thread reference to stack-top.
	lua_rawgeti(L, LUA_REGISTRYINDEX, thread);
	lua_State* func_thread = lua_tothread(L, -1);
	lua_pop(L, 1);

	if (func_thread == NULL)
		LOG_FATAL("Attempted to index nonexistent thread.");

	if (lua_isyieldable(func_thread) == false)
		LOG_FATAL("Attempted to index invalid thread.");

	return func_thread;
}

