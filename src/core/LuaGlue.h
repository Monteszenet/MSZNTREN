#include "libs/lua/include/lua.hpp"

// Native -> Lua.

// User customisation of application specification (window size, name, etc.)
int L_AppConfig(lua_State* L);
// Logging from Lua.
int L_Log(lua_State* L);
// Print from Lua to console/log.
int L_Print(lua_State* L);