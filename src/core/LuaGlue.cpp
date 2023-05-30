#include "LuaGlue.h"

#include <string>

#include "Application.h"
#include "Logging.h"

// Native -> Lua.

int L_AppConfig(lua_State* L)
{
	App_Config(luaL_checkstring(L, -4), luaL_checkstring(L, -3),
		(int)lua_tonumber(L, -2), (int)lua_tointeger(L, -1));

	return 0;
}

int L_Log(lua_State* L)
{
	int log_i = (int)luaL_checkinteger(L, -2);
	std::string msg = luaL_checkstring(L, -1);

	switch (log_i)
	{
	case LOGTYPE_DEBUG:
		LOG_DEBUG(msg);
		break;
	case LOGTYPE_INFO:
		LOG_INFO(msg);
		break;
	case LOGTYPE_WARN:
		LOG_WARN(msg);
		break;
	case LOGTYPE_ERROR:
		LOG_ERROR(msg);
		break;
	case LOGTYPE_FATAL:
		LOG_FATAL(msg);
		break;
	default:
		LOG_ERROR("Invalid log type.");
		break;
	}

	return 0;
}

int L_Print(lua_State* L)
{
	std::string msg = luaL_checkstring(L, -1);

	LOG_LUA(msg);

	return 0;
}