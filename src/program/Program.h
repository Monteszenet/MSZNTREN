#pragma once

#include <unordered_map>
#include <map>
#include <vector>
#include <stack>

#include "src/media/Rendering.h"
#include "src/media/Audio.h"
#include "src/core/Input.h"
#include "src/core/Window.h"
#include "src/core/Scripts.h"

#include "Events.h"

#define uint64_t unsigned long long

#define LIBRARY(type) std::unordered_map<std::string, type>
#define LIST(type) std::vector<type> 
#define PAIR(type1, type2) std::pair<type1, type2>
#define STACK(type) std::stack<type>
#define ID_LIBRARY(type) std::unordered_map<int, type>
#define ORDERED_ID_LIBRARY(type) std::map<int, type>

typedef void (*C_EventCallback) ();

#define FIND(library, value) (library.find(value) != library.end())

class Program
{
public:
	void Init();
	void OnUpdate();
	void Finalise();

private:
	struct Variable
	{
		std::string name;

		enum {
			MVAR_NULL,
			MVAR_INT,
			MVAR_FLOAT,
			MVAR_STRING
		} type;

		void* data;
	};
	struct Chunk
	{
		enum {
			EXEC_EXIT,
			EXEC_YIELD,
			EXEC_ERROR,
		};
		std::string name;
		std::string type;
		int args;
		int results;
		LuaThread thread;
		C_EventCallback c_callback = NULL;
		bool yielded = true;
		bool popped = false;
	};
	struct ExecutableObject
	{
		std::string name;
		std::string type;
		Chunk chunk;

		enum {
			EXECOBJ_UNSTARTED,
			EXECOBJ_SUSPENDED,
			EXECOBJ_YIELDED,
			EXECOBJ_UNYIELD,
			EXECOBJ_RUNNING,
			EXECOBJ_ERROR
		} current_state = EXECOBJ_UNSTARTED;

		ExecutableObject(std::string _name, std::string _type, Chunk _chunk)
		{
			name = _name;
			type = _type;
			chunk = _chunk;
		}

		ExecutableObject() {}
	};
	struct Event : public ExecutableObject
	{
		HOOK_TYPE hook = 0; 
		bool enabled = false;
		bool triggerable = false;

		Event(std::string _name, std::string _type, Chunk _chunk, 
			HOOK_TYPE _hook, bool _enabled, bool _triggerable) : ExecutableObject(_name, _type, _chunk)
		{
			hook = _hook;
			enabled = _enabled;
			triggerable = _triggerable;
		}

		Event() {}
	};
	struct State : public ExecutableObject
	{
		State(std::string _name, std::string _type, Chunk _chunk) : ExecutableObject(_name, _type, _chunk)
		{

		}
		State() {}
	};
	struct Label : public ExecutableObject
	{
		Label(std::string _name, std::string _type, Chunk _chunk) : ExecutableObject(_name, _type, _chunk)
		{

		}
		Label() {}
	};

	struct LightUDPointer
	{
		void* data;
		enum {
			LUDPTR_IMAGE,
			LUDPTR_MUSIC,
			LUDPTR_AUDIO,
		} type;
	};

private:
	struct RenderableImage
	{
		std::string name;
		MTexture* texture;
		int x = 0;
		int y = 0;
		int w = 0;
		int h = 0;
		double angle = 0;
	};

private:
	LIBRARY(ExecutableObject*) ExecObjLibrary;
	STACK(ExecutableObject*) ExecObjsRunning;
	LIBRARY(Event*) EventLibrary;
	LIBRARY(PAIR(HOOK_TYPE, int)) EventHooks;
	LIBRARY(State*) StateLibrary;
	STACK(State*) StateCallStack;
	STACK(Chunk) ChunkCallStack;
	ORDERED_ID_LIBRARY(LuaFunction) InitChunksLibrary;
	LIBRARY(Label*) LabelLibrary;

	STACK(std::string) ExecObjOrder;
	ExecutableObject* CurrentExecObjTop = NULL;

	LIBRARY(std::string) ChunkPauseLibrary;

private:
	LIBRARY(Variable) VariableLibrary;
	LIBRARY(MTexture*) ImageLibrary;
	LIBRARY(MMusic*) MusicLibrary;
	LIBRARY(MAudio*) AudioLibrary;
	LIST(LightUDPointer*) LightUDPointerList;
	FONT_ID ProgramFont = -1;

private:
	LIST(std::string) RenderLayersConfig =
	{ "Master" };
	LIBRARY(LIST(RenderableImage)) RenderLayers;


private:
	void input();

	void register_functions();
	
	void update_current_state();

	std::string dump_callstack(std::string func);
	void fatal_error(std::string errormsg, std::string func);

private:
	void render_layers();

private:
	ExecutableObject* create_executable_object(ExecutableObject* exec_object);

	int execute_object(lua_State* L, ExecutableObject* exec_obj);
	int yield_object(lua_State* L);
	int execute_chunk(lua_State* L, Chunk* chunk);
	int yield_chunk(lua_State* L);

	//void callstack_push(lua_State* L, Chunk* chunk);
	//void callstack_pop(lua_State* L, bool yield=true);
	//void callstack_clear(lua_State* L);

private:
	void init_events();
	void create_event(std::string name, std::string _hook, LuaFunction thread);
	void create_c_callback(std::string name, std::string _hook, C_EventCallback func);
	void call_event(lua_State* L, Event* _event);
	//void disable_event(lua_State* L, Event* _event);
	void invoke_event(HOOK_TYPE _event);
	#define INVOKE_EVENT(hook_event) invoke_event(EventHooks[hook_event].first)

private:
	void init_states();
	void create_state(std::string name, LuaFunction thread);
	void call_state(lua_State* L, State* _state);
	//void jump_state(lua_State* L, State _state);
	//void return_state(lua_State* L);

private:
	void create_label(std::string name, LuaFunction thread);
	void call_label(lua_State* L, Label* _state);
	void validate_labels();

private:
	void InitClock();
	// Takes seconds.
	void SetClockRate(int seconds);
	// Returns in miliseconds.
	uint64_t GetClockTime();
	void TickClock();
	void SetTimer(std::string name, int seconds);

#define GetClockTimeSeconds() (GetClockTime() / 1000)

private:
	void static yield_timer_callback();

private:
	int static L_Variable(lua_State* L);
	int static L_Image(lua_State* L);
	int static L_Music(lua_State* L);
	int static L_Audio(lua_State* L);
	int static L_Font(lua_State* L);
	int static L_Event(lua_State* L);
	int static L_State(lua_State* L);
	int static L_Label(lua_State* L);
	int static L_Init(lua_State* L);

	//int static L_CallEvent(lua_State* L);
	//int static L_DisableEvent(lua_State* L);
	//int static L_CallState(lua_State* L);
	//int static L_JumpState(lua_State* L);
	//int static L_Return(lua_State* L);

	int static L_Pause(lua_State* L);
	int static L_Quit(lua_State* L);

	int static L_ShowImage(lua_State* L);
	int static L_PlayAudio(lua_State* L);
	int static L_PlayMusic(lua_State* L);
};





extern Program* program;

#define PROGRAM program
#define CREATE_PROGRAM PROGRAM = new Program