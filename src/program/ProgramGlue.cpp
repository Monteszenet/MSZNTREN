#include "Program.h"

#include "src/core/Logging.h"
#include "src/core/Application.h"
#include "src/core/Scripts.h"

// Glue 
 
extern "C" int Program::L_Variable(lua_State* L)
{
	Variable var;

	lua_rawgeti(L, -1, 1); // Pushes value of table[1] to stack.
	std::string str_type = luaL_checkstring(L, -1);
	lua_pop(L, 1); // Pop value off stack.

	lua_rawgeti(L, -1, 2); // Pushes value of table[2] to stack.
	var.name = luaL_checkstring(L, -1);
	lua_pop(L, 1);

	if (str_type == "INTEGER")
		var.type = Variable::MVAR_INT;
	else if (str_type == "FLOAT")
		var.type = Variable::MVAR_FLOAT;
	else if (str_type == "STRING")
		var.type = Variable::MVAR_STRING;
	else
		var.type = Variable::MVAR_NULL;

	var.data = NULL;

	lua_rawgeti(L, -1, 3); 
	switch (var.type)
	{
	case Variable::MVAR_NULL:
		LOG_ERROR("Attempted to create variable with null type.");
		break;
	case Variable::MVAR_INT:
	{	
		int* i = new int;
		*i = (int)luaL_checkinteger(L, -1);
		var.data = i;
		break;
	}
	case Variable::MVAR_FLOAT:
	{
		double* d = new double;
		*d = (double)luaL_checknumber(L, -1);
		var.data = d;
		break;
	}
	case Variable::MVAR_STRING:
	{
		std::string* s = new std::string;
		*s = luaL_checkstring(L, -1);
		var.data = s;
		break; 
	}
	}
	lua_pop(L, 1);

	if (var.data == NULL)
		LOG_ERROR("Attempted to create variable with null type.");

	program->VariableLibrary[var.name] = var;

	LOG_INFO("Created variable: '" + var.name + "' of type '" + str_type + "'");

	return 0;
}

extern "C" int Program::L_Image(lua_State* L)
{
	lua_rawgeti(L, -1, 1); 
	std::string name = luaL_checkstring(L, -1);
	lua_pop(L, 1); // Pop value off stack.

	lua_rawgeti(L, -1, 2); 
	std::string path = luaL_checkstring(L, -1);
	lua_pop(L, 1);

	MTexture* texture = Render_CreateTexture(path);

	program->ImageLibrary[name] = texture;

	LOG_INFO("Created image: '" + name + "'; path: '" + path + "'");

	LightUDPointer* ptr = new LightUDPointer;
	*ptr = { texture, LightUDPointer::LUDPTR_IMAGE };
	program->LightUDPointerList.push_back(ptr);
	lua_pushlightuserdata(L, ptr);

	return 1;
}

extern "C" int Program::L_Music(lua_State* L)
{
	lua_rawgeti(L, -1, 1); 
	std::string name = luaL_checkstring(L, -1);
	lua_pop(L, 1); 

	lua_rawgeti(L, -1, 2);
	std::string path = luaL_checkstring(L, -1);
	lua_pop(L, 1);

	MMusic* music = Mixer_CreateMusic(path);

	program->MusicLibrary[name] = music;

	LOG_INFO("Created music: '" + name + "'; path: '" + path + "'");

	LightUDPointer* ptr = new LightUDPointer;
	*ptr = { music, LightUDPointer::LUDPTR_MUSIC };
	program->LightUDPointerList.push_back(ptr);
	lua_pushlightuserdata(L, ptr);

	return 1;
}

extern "C" int Program::L_Audio(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	std::string name = luaL_checkstring(L, -1);
	lua_pop(L, 1); 

	lua_rawgeti(L, -1, 2);
	std::string path = luaL_checkstring(L, -1);
	lua_pop(L, 1);

	MAudio* audio = Mixer_CreateAudio(path);

	program->AudioLibrary[name] = audio;

	LOG_INFO("Created audio: '" + name + "'; path: '" + path + "'");

	LightUDPointer* ptr = new LightUDPointer; 
	*ptr = { audio, LightUDPointer::LUDPTR_AUDIO };
	program->LightUDPointerList.push_back(ptr);
	lua_pushlightuserdata(L, ptr);

	return 1;
}

extern "C" int Program::L_Font(lua_State* L)
{
	lua_rawgeti(L, -1, 1); 
	std::string path = luaL_checkstring(L, -1);
	lua_pop(L, 1); 

	lua_rawgeti(L, -1, 2);
	int ptsize = (int)luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	lua_rawgeti(L, -1, 3);
	std::string name = luaL_checkstring(L, -1);
	lua_pop(L, 1);

	program->ProgramFont = CreateFont(path, ptsize, name);

	LOG_INFO("Opened font: '" + name + "'; path: '" + path + "'");

	return 0;
}

extern "C" int Program::L_Event(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	std::string str1 = luaL_checkstring(L, -1);
	lua_pop(L, 1);
	
	std::string str2 = "";
	LuaThread thread = 0;

	lua_rawgeti(L, -1, 2); 
	// Check if second argument is string or function.
	if (lua_type(L, -1) == LUA_TSTRING) // If string, event has trigger/hook.
	{
		// Str1 is callback, str2 is name.
		str2 = luaL_checkstring(L, -1);

		lua_pop(L, 1);

		lua_rawgeti(L, -1, 3);
		thread = Lua_CreateChunkThread();
		lua_pop(L, 1);

		// Name, callback, function.
		program->create_event(str2, str1, thread);
	}
	else if (lua_type(L, -1) == LUA_TFUNCTION) // If function, then event only has two arguments, no hook.
	{
		// Str1 is name, str2 is null.

		thread = Lua_CreateChunkThread();
		lua_pop(L, 1);

		// Name, null, function.
		program->create_event(str1, str2, thread);
	}

	return 0;
}

extern "C" int Program::L_State(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	std::string name = luaL_checkstring(L, -1);
	lua_pop(L, 1);

	lua_rawgeti(L, -1, 2);
	LuaThread thread = Lua_CreateChunkThread();
	lua_pop(L, 1);

	program->create_state(name, thread);

	return 0;
}

extern "C" int Program::L_Label(lua_State* L)
{
	lua_rawgeti(L, -1, 1);
	std::string name = luaL_checkstring(L, -1);
	lua_pop(L, 1);

	lua_rawgeti(L, -1, 2);
	LuaThread thread = Lua_CreateChunkThread();
	lua_pop(L, 1);

	program->create_label(name, thread);

	return 0;
}

extern "C" int Program::L_Init(lua_State* L)
{
	int level = -1;
	LuaThread thread = 0;

	lua_rawgeti(L, -1, 1);
	if (lua_type(L, -1) == LUA_TNUMBER)
	{
		level = (int)lua_tointeger(L, -1);
		lua_pop(L, 1);

		lua_rawgeti(L, -1, 2);
		thread = Lua_CreateChunkThread();
		lua_pop(L, 1);
		
		if (program->InitChunksLibrary.find(level) != program->InitChunksLibrary.end())
		{
			while (program->InitChunksLibrary.find(level) != program->InitChunksLibrary.end())
			{
				level += 1;
			}
		}

		program->InitChunksLibrary[level] = thread;

	}
	else if(lua_type(L, -1) == LUA_TFUNCTION)
	{
		thread = Lua_CreateChunkThread();
		lua_pop(L, 1);

		int max = -1;
		// Get largest current value in map.
		if (program->InitChunksLibrary.size() > 0) // If map is greater than 0, get value of that highest index.
			max = std::prev(program->InitChunksLibrary.end())->first;
		// Else, we just do it at 0.
		program->InitChunksLibrary[max + 1] = thread;
	}

	return 0;
}





//extern "C" int Program::L_DisableEvent(lua_State* L)
//{
//	// Lua call pushes argument(s) to stack in their order.
//	std::string name = luaL_checkstring(L, -1);
//
//	//program->disable_event(L(), program->EventLibrary[name]);
//	
//	return 0;
//}
//
//extern "C" int Program::L_CallEvent(lua_State* L)
//{
//	// Lua call pushes argument(s) to stack in their order.
//	std::string name = luaL_checkstring(L, -1);
//
//	program->call_event(L(), program->EventLibrary[name]);
//
//	return 0;
//}
//
//extern "C" int Program::L_CallState(lua_State* L)
//{
//	std::string name = luaL_checkstring(L, -1);
//
//	program->call_state(L, program->StateLibrary[name]);
//
//	return 0;
//}
//
//extern "C" int Program::L_JumpState(lua_State* L)
//{
//	std::string name = luaL_checkstring(L, -1);
//
//	//program->jump_state(L, program->StateLibrary[name]);
//
//	return 0;
//}
//
//extern "C" int Program::L_Return(lua_State* L)
//{
//	//program->return_state(L);
//
//	return 0;
//}

extern "C" int Program::L_Pause(lua_State * L)
{
	int time = (int)luaL_checkinteger(L, -1);

	LOG_INFO("Beginning pause for (" + S(time) + ") seconds.");

	// Create timer. Upon it finishing, the chunk will resume.
	program->SetTimer(program->CurrentExecObjTop->name, time);

	// Add current chunk name to the pause library. 
	program->ChunkPauseLibrary[program->CurrentExecObjTop->name];

	program->yield_object(L);

	LOG_INFO("Returning pause for (" + S(time) + ") seconds.");

	return 0;
}

extern "C" int Program::L_Quit(lua_State* L)
{
	LOG_INFO("Quitting app from program.");
	App_Quit();

	return 0;
}



extern "C" int Program::L_ShowImage(lua_State * L)
{
	LightUDPointer* ptr = (LightUDPointer*)lua_touserdata(L, -1);
	std::string name;

	if (ptr == NULL)
	{
		name = luaL_checkstring(L, -1);
		MTexture* texture = program->ImageLibrary[name];
		RenderableImage img = { name, texture, 0, 0, texture->width, texture->height, 0.0f };
		program->RenderLayers["MASTER"].push_back(img);
	}
	else
	{
		if (ptr->type != LightUDPointer::LUDPTR_IMAGE)
			program->fatal_error("Invalid object type as argument.", "ShowImage");
		MTexture* texture = (MTexture*)ptr->data;
		for (auto& i : program->ImageLibrary)
			if (i.second == texture)
				name = i.first;
		RenderableImage img = { name, texture, 0, 0, texture->width, texture->height, 0.0f };
		program->RenderLayers["MASTER"].push_back(img);
	}

	return 0;
}

extern "C" int Program::L_PlayAudio(lua_State * L)
{
	

	return 0;
}

extern "C" int Program::L_PlayMusic(lua_State * L)
{


	return 0;
}