#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "libs/lua/include/lua.hpp"

#define LuaCFunction lua_CFunction
#define LuaRef int
#define LuaNullRef 0
#define LuaFunction LuaRef
#define LuaThread LuaRef

#define M_SCRIPT unsigned int
#define NULL_SCRIPT 0

void EnterScriptingContext();
void ExitScriptingContext();

lua_State* GetLuaState();

void InitLuaEnvironment();
void Lua_RegisterFunction(std::string name, LuaCFunction function);

void CompileLuaToMSZNT(std::string path);

M_SCRIPT Lua_GetScript(std::string script);
void Lua_RunScript(M_SCRIPT script);
void Lua_RunScriptFile(std::string path);
void Lua_RunScript(std::string script);

// Takes a function on the stack-top.
LuaThread Lua_CreateChunkThread();
void Lua_DestroyChunkThread(LuaThread thread);

int Lua_ExecuteChunk(LuaFunction chunk, int args, int results);
int Lua_ExecuteChunkThread(LuaThread thread, int args, int* results);
void Lua_m_SuspendChunkThread();
#define Lua_SuspendChunkThread() Lua_m_SuspendChunkThread(); lua_yield(L, 0);

void Lua_CheckChunk(LuaFunction func);
lua_State* Lua_CheckThread(LuaThread thread);

#define L() GetLuaState()

const char* m_getluastring(lua_State* L, int idx);

#define LUA_GETSTRING(L, idx) m_getluastring(L, idx)