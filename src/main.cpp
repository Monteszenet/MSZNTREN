#include "core/Application.h"
#include "core/Logging.h"
#include "core/Utility.h"

#include "libs/sdl/include/SDL.h"


int main(int argc, char* args[])
{
    InitLogging("files\\log.txt");
    
    LOG_INFO(LOG_DEMARCATION);	 LOG_INFO("Entering program init-time.");
    LOG_INFO("Initialised logging.");
    
    Application* app = CreateApplication({ GetFileDirectory(), GetCLArgs(argc, args) });
    app->Run();
    DeleteApplication();
    
    FlushLog();

    return 0;
}

//#include "libs/lua/include/lua.hpp"
//
//#include <iostream>
//
//#include <vector>
//
//#define thread_ref int
//
//std::vector<thread_ref> threads;
//
//thread_ref threadstack_top = 0;
//
//void m_yield(lua_State* L)
//{
//	std::cout << "yielding coroutine.\n";
//	lua_yield(L, 0);
//}
//
//void create_coroutine(lua_State* L)
//{
//	lua_State* func_thread = lua_newthread(L); // Pushes new thread to top of stack.
//	lua_pushvalue(L, -2); // Pushes function at -2 now to top of stack.
//	lua_xmove(L, func_thread, 1); //Moves the function to the top of the new stack, and pops it off the old.
//	threads.push_back(luaL_ref(L, LUA_REGISTRYINDEX)); // We now return a reference to that thread.
//	std::cout << "created coroutine.\n";
//}
//
//int run_coroutine_after_yielding(lua_State* L)
//{
//	// Push referenced thread.
//	lua_rawgeti(L, LUA_REGISTRYINDEX, threads[threadstack_top]);
//	// Thread has its chunk pushed at its stack-top.
//	lua_State* func_thread = lua_tothread(L, -1);
//	lua_pop(L, 1);
//
//	std::cout << "executing coroutine after a yield.\n";
//
//	int results = 0;
//
//	// Takes a function at stack-top of the given thread.
//	int result = lua_resume(func_thread, NULL, 0, &results);
//	// We return here once yield is called on the thread or if the thread finishes
//	// execution by itself.
//
//	std::cout << "suspended coroutine.\n";
//
//	// We then pop it if it returned by finishing execution.
//	if (result == LUA_OK)
//	{
//		std::cout << "finished executing coroutine.\n";
//	}
//	// If it yielded, we do nothing. Thats for the caller to deal with.
//	else if (result == LUA_YIELD)
//	{
//		run_coroutine_after_yielding(L);
//		std::cout << "coroutine yielded.\n";
//	}
//	// Else, it returned an error.
//	else
//		std::cout << "coroutine error. " << luaL_checkstring(L, -1) << "\n";
//
//	return result;
//
//}
//
//int run_coroutine(lua_State* L, thread_ref thread)
//{
//	// Push referenced thread.
//	lua_rawgeti(L, LUA_REGISTRYINDEX, thread);
//	// Thread has its chunk pushed at its stack-top.
//	lua_State* func_thread = lua_tothread(L, -1);
//	lua_pop(L, 1);
//
//	std::cout << "executing coroutine.\n";
//
//	int results = 0;
//
//	// Takes a function at stack-top of the given thread.
//	int result = lua_resume(func_thread, NULL, 0, &results);
//	// We return here once yield is called on the thread or if the thread finishes
//	// execution by itself.
//
//	std::cout << "suspended coroutine.\n";
//
//	// We then pop it if it returned by finishing execution.
//	if (result == LUA_OK)
//		std::cout << "finished executing coroutine.\n";
//	// If it yielded, we do nothing. Thats for the caller to deal with.
//	else if (result == LUA_YIELD)
//	{
//		std::cout << "coroutine yielded.\n";
//		run_coroutine_after_yielding(L);
//	}
//	// Else, it returned an error.
//	else
//		std::cout << "coroutine error.\n";;
//
//	return result;
//}
//
//int L_create_coroutine(lua_State* L)
//{
//	// Push function made by lua call.
//	lua_rawgeti(L, -1, 1);
//	// Create coroutine from it.
//	create_coroutine(L);
//	lua_pop(L, 1);
//
//	return 0;
//}
//
//int L_call_coroutine(lua_State* L)
//{
//	int i = (int)luaL_checkinteger(L, -1);
//	std::cout << "calling coroutine " << i << "\n";
//	lua_pop(L, 1);
//	threadstack_top = i;
//	m_yield(L);
//
//
//	return 0;
//}
//
//int main(int argc, char* args[])
//{
//	std::string script = R"(
//    create_coroutine { function()
//        print("hi 1 1");
//		call_co( 1 );
//		print("hi 1 2");
//    end }
//
//	create_coroutine { function()
//        print("hi 2");
//    end }
//
//)";
//
//	lua_State* L = luaL_newstate();
//
//	luaopen_base(L);
//	luaopen_string(L);
//
//	lua_register(L, "create_coroutine", L_create_coroutine);
//	lua_register(L, "call_co", L_call_coroutine);
//
//	luaL_dostring(L, script.c_str());
//
//	std::cout << "start\n";
//	run_coroutine(L, threads[0]);
//	std::cout << "end\n";
//
//	lua_close(L);
//
//	return 0;
//}
