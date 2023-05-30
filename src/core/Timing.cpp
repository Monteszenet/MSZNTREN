#include "Timing.h"

#include "libs/sdl/include/SDL.h"

uint64_t GetCurrentTime()
{
	return SDL_GetTicks64();
}

void PauseApplication(uint32_t ms)
{
	SDL_Delay(ms);
}