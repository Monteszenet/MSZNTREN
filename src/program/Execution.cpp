#include "Program.h"

#include "src/core/Logging.h"

Program::ExecutableObject* Program::create_executable_object(ExecutableObject* exec_object)
{
	ExecObjLibrary[exec_object->name] = exec_object;
	return exec_object;
}

int Program::execute_object(lua_State* L, ExecutableObject* exec_obj)
{
	ExecObjsRunning.push(exec_obj);

	exec_obj->current_state = ExecutableObject::EXECOBJ_RUNNING;

	int result = execute_chunk(L, &exec_obj->chunk);

	if (result == Chunk::EXEC_EXIT)
	{
		exec_obj->current_state = ExecutableObject::EXECOBJ_SUSPENDED;

		ExecObjsRunning.pop();
	}
	else if (result == Chunk::EXEC_YIELD)
	{
		exec_obj->current_state = ExecutableObject::EXECOBJ_YIELDED;
	}


	return result;
}

int Program::yield_object(lua_State* L)
{
	return yield_chunk(L);
}

int Program::execute_chunk(lua_State* L, Chunk* chunk)
{
	LOG_INFO("EXECUTING CHUNK '" + chunk->name + "', TYPE '" + chunk->type + "'");

	int result = Lua_ExecuteChunkThread(chunk->thread, chunk->args, &chunk->results);

	if (result == Chunk::EXEC_EXIT)
		LOG_INFO("FINISHED EXECUTING CHUNK '" + chunk->name + "', TYPE '" + chunk->type + "'");
	else if (result == Chunk::EXEC_YIELD)
		LOG_INFO("YIELDED CHUNK '" + chunk->name + "', TYPE '" + chunk->type + "'");

	return result;
}

int Program::yield_chunk(lua_State* L)
{
	LOG_INFO("YIELDING CHUNK '" + CurrentExecObjTop->chunk.name +
		"', TYPE '" + CurrentExecObjTop->chunk.type + "'");

	Lua_SuspendChunkThread();

	LOG_INFO("RESUMING CHUNK FROM YIELD '" + CurrentExecObjTop->chunk.name +
		"', TYPE '" + CurrentExecObjTop->chunk.type + "'");

	return 0;
}