#include "Program.h"

#include "src/core/Application.h"
#include "src/media/Rendering.h"
#include "src/media/Audio.h"
#include "src/core/Input.h"
#include "src/core/Window.h"
#include "src/core/Logging.h"
#include "src/core/Timing.h"
#include "src/core/Scripts.h"

Program* program = NULL;


// Called once the timer counts down from some set-value. It pushes a string to the Lua stack as an identifier.
void Program::yield_timer_callback()
{
	std::string chunk = luaL_checkstring(L(), -1);

	// Check if this string is on our library.
	if (FIND(program->ChunkPauseLibrary, chunk) == true)
	{
		// If so, then that means that chunk yielded per a timer, and now that timer has finished.
		// So we resume by setting the status of the stack-top exec_obj to be unyielded.
		if (program->CurrentExecObjTop->name == chunk) // Just in case.
			program->CurrentExecObjTop->current_state = ExecutableObject::EXECOBJ_UNYIELD;
	}
}


void Program::Init()
{
	register_functions();

	init_events();
	init_states();

	Lua_RunScriptFile("game/scripts/main.lua");

	create_c_callback("YieldTimerCallback", "TIMER", yield_timer_callback);

	validate_labels();

	ExecObjOrder.push("MAIN_MENU");
	ExecObjOrder.push("SPLASHSCREEN"); // First exec_obj pushed last.

	for (auto& e : InitChunksLibrary)
	{		
		// Push chunk of code to top of callstack, which immediately begins execution.
		Chunk chunk = { ("INIT " + S(e.first)), "INIT_BLOCK", 0, 0, e.second };
		
		execute_chunk(L(), &chunk);
	}

	INVOKE_EVENT("START");

	CurrentExecObjTop = ExecObjLibrary[ExecObjOrder.top()];

	InitClock();
	SetClockRate(60);
}

void Program::OnUpdate()
{
	input();

	update_current_state();

	render_layers();

	INVOKE_EVENT("CONTINUOUS");

	TickClock();
}

void Program::Finalise()
{
	for (auto& m : ImageLibrary)
	{
		Render_DeleteTexture(m.second);
		LOG_INFO("Deleted image: '" + m.first + "'");
	}
	for (auto& m : MusicLibrary)
	{
		Mixer_FreeMusic(m.second);
		LOG_INFO("Deleted music: '" + m.first + "'");
	}
	for (auto& m : AudioLibrary)
	{
		Mixer_FreeAudio(m.second);
		LOG_INFO("Deleted audio: '" + m.first + "'");
	}
	for (auto& m : VariableLibrary)
	{
		delete m.second.data;
		m.second.data = NULL;
		LOG_INFO("Deleted variable: '" + m.first + "'");
	}
	for (auto& m : ExecObjLibrary)
	{
		delete m.second;
		m.second = NULL;
		LOG_INFO("Deleted executable object: '" + m.first + "'");
	}
	for (auto& m : LightUDPointerList)
	{
		delete m;
		m = NULL;
		LOG_INFO("Deleted light userdata pointer object.");
	}
}

void Program::input()
{
	if (GetKeyDown(SDL_SCANCODE_ESCAPE) == true)
	{
		App_Quit();
	}
	if (GetMouseButtonPressed(Mouse_LeftButton))
	{
		App_Quit();
	}
}



void Program::register_functions()
{
	// Functions which take table, syntax: 'FUNC { x }'
	Lua_RegisterFunction("VARIABLE", Program::L_Variable);
	Lua_RegisterFunction("IMAGE", Program::L_Image);
	Lua_RegisterFunction("MUSIC", Program::L_Music);
	Lua_RegisterFunction("AUDIO", Program::L_Audio);
	Lua_RegisterFunction("FONT", Program::L_Font);
	Lua_RegisterFunction("EVENT", Program::L_Event);
	Lua_RegisterFunction("STATE", Program::L_State);
	Lua_RegisterFunction("LABEL", Program::L_Label);
	Lua_RegisterFunction("INIT", Program::L_Init);

	// Normal functions, syntax: Func(x);'
	Lua_RegisterFunction("Pause", Program::L_Pause);
	Lua_RegisterFunction("Quit", Program::L_Quit);

	Lua_RegisterFunction("ShowImage", Program::L_ShowImage);
	Lua_RegisterFunction("PlayAudio", Program::L_PlayAudio);
	Lua_RegisterFunction("PlayMusic", Program::L_PlayMusic);
}




void Program::update_current_state()
{
	// We execute the top object (resume it) if it hasn't been started yet or if we need to resume it.
	if (CurrentExecObjTop->current_state == ExecutableObject::EXECOBJ_UNSTARTED or
		CurrentExecObjTop->current_state == ExecutableObject::EXECOBJ_UNYIELD)
	{
		execute_object(L(), CurrentExecObjTop);
	}
	// Check if the exec_obj exited successfully.
	if (CurrentExecObjTop->current_state == ExecutableObject::EXECOBJ_SUSPENDED)
	{
		// If so, we change the new top which will begin execution next frame.
		ExecObjOrder.pop();
		if (ExecObjOrder.size() == 0)
		{
			App_Quit();
			return;
		}
		CurrentExecObjTop = ExecObjLibrary[ExecObjOrder.top()];
	}
}



std::string Program::dump_callstack(std::string func)
{
	std::string msg;

	std::string topobj = CurrentExecObjTop->name;
	std::string topobjtype = CurrentExecObjTop->type;

	msg += "\t" + C("Current function: '") + func + "'\n";
	msg += "\t" + C("Current chunk in execution: '") + topobj + "'\n";
	msg += "\t\t" + C("Exec-Object type: '") + topobjtype + "'";
	

	return msg;
}

void Program::fatal_error(std::string errormsg, std::string func)
{
	LOG_FATAL(errormsg + "\n" + "\tDumped callstack:\n" + dump_callstack(func));
}



//void Program::callstack_push(lua_State* L, Chunk* chunk)
//{
//	// Check if we we're pushing anything at all,
//	// If not, most likely we just came from a pop,
//	// And are resuming the yielded functions below it.
//	if (chunk != NULL)
//	{
//		chunk->yielded = false;
//
//		Chunk* prev_chunk = NULL;
//
//		if (ChunkCallStack.size() > 0) // Get current stack-top, soon to be previous.
//		{
//			prev_chunk = &ChunkCallStack.top();
//		}
//
//		Lua_CheckThread(chunk->thread);
//
//		// Push new chunk, will be executed later.
//		LOG_INFO("PUSHED CHUNK '" + chunk->name + "'\n");
//		ChunkCallStack.push(*chunk);
//
//		// Stop/yield execution of chunk at prev top.
//		if (ChunkCallStack.size() > 1) // We know it's at least 1 as we just pushed something.
//		{
//			Lua_CheckThread(prev_chunk->thread);
//
//			// Yield previous call.
//			if (prev_chunk->yielded == false)
//			{
//				prev_chunk->yielded = true;
//				LOG_INFO("CHUNK YIELDED BY PUSH '" + prev_chunk->name + "'\n");
//
//				Lua_SuspendChunkThread();
//			}
//		}
//	}
//
//	// Check if there's anything to execute at all.
//	if (ChunkCallStack.size() > 0)
//	{
//		// Chunk to execute. Real top.
//		Chunk& topchunk = ChunkCallStack.top();
//
//		// Copy function on its thread, so it's stack has two identical functions.
//		// Resume probably pops off that function.
//		// lua_pushvalue(func_thread, -1);
//
//		int nresults = 0;
//		topchunk.yielded = false;
//		LOG_INFO("ENTERING CHUNK '" + topchunk.name + "'\n");
//
//		int result = Lua_ExecuteChunkThread(topchunk.thread, topchunk.args, &topchunk.results);
//
//		LOG_INFO("EXITING CHUNK '" + topchunk.name + "'\n");
//		topchunk.yielded = true;
//
//		if (result == LUA_OK)
//		{
//			LOG_INFO("CHUNK FINISHED EXECUTION '" + topchunk.name + "'\n");
//			// If chunk is executed succesfully without interruption, 
//			// Pop it off.
//			callstack_pop(L);
//			// When passing NULL as an argument, we mean that we want to just
//			// resume whatever chunk is currently at the top without resuming.
//			callstack_push(L, NULL);
//		}
//		else if (result == LUA_YIELD)
//		{
//			LOG_INFO("CHUNK YIELDED '" + topchunk.name + "'\n");
//			if (topchunk.popped == false) // We do want to resume prior chunks.
//				callstack_push(L, NULL);
//			// We now reusme whatever chunk was is on top of the callstack now, as
//			// a yield means push_chunk() was called, and that would've called m_yield()
//			// which would return to our resume() call. The new top chunk is now the new
//			// chunk which superseded this 'current' one.
//			if (topchunk.popped == true)
//				return; // We do not want to resume prior chunks. Just exit the function. 
//		}
//		else
//		{
//			LOG_ERROR("Failed to execute chunk: '" + topchunk.name +
//				"' of type '" + topchunk.type + "'\n[LUA ERROR] " + lua_tostring(L, -1));
//		}
//	}
//
//	//if (topchunk.args > 0)
//	//	lua_insert(func_thread, (-1 - topchunk.args)); // Move thread to stack[-i], with args ontop.
//}
//
//void Program::callstack_pop(lua_State* L, bool yield)
//{
//	// 'Program::callstack_pop will' automatically resume execution of chunks
//	// Lower in the stack. If this is not wanted, just use callstack_clear();
//
//	if (ChunkCallStack.size() > 0) // Check if table size is > 0, if we have anything to pop.
//	{
//		Chunk& topchunk = ChunkCallStack.top(); // Get current stack - top.
//
//		Lua_CheckThread(topchunk.thread);
//
//		bool yielded = topchunk.yielded;
//		topchunk.yielded = true;
//		if (yield == true)
//			// Set to notify that we do not want to resume this chunk ever, it's gone.
//			topchunk.popped = true;
//
//		ChunkCallStack.pop();
//
//		if (yielded == false)
//		{
//			Lua_SuspendChunkThread();
//		}
//	}
//}
//
//// Will clear the stack without any chunk execution. Yields current stack-top.
//void Program::callstack_clear(lua_State* L)
//{
//	for (int i = 0; i < ChunkCallStack.size(); i++)
//	{
//		callstack_pop(L, false);
//	}
//}

