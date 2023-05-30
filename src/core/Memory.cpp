#include "Memory.h"

#include "Logging.h"

#include <map>

#define Pointer void*
#define Memory void*

struct {
	std::map<Pointer, Memory> voids;

} memory;

template<typename T>
T* m_malloc()
{
	T* t = new T;
	memory.voids[t] = t;
	LOG_DEBUG("Allocated memory");
	return t;
}

void m_free(Pointer t)
{
	if (memory.voids.find(t) != memory.voids.end())
	{
		delete memory.voids[t];
		memory.voids[t] = NULL;
		memory.voids.erase(t);
		LOG_DEBUG("Freed memory");
		return;
	}
	else
	{
		LOG_FATAL("Attempted to delete invalid/unknown memory.");
		return;
	}
}

void CleanUpMemory()
{
	if (memory.voids.size() != 0)
	{
		for (auto& v : memory.voids)
		{
			delete v.second;
			v.second = NULL;
			memory.voids.erase(v.first);
		}
		return;
	}
	else
	{
		return;
	}
}