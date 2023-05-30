#include "Media.h"

#include <iostream>

#include "libs/sdl/include/SDL_image.h"
#include "libs/sdl/include/SDL_ttf.h"
#include "libs/sdl/include/SDL_mixer.h"

#include "src/core/Logging.h"
#include "src/core/Window.h"
#include "Rendering.h"
#include "Audio.h"

void InitMedia()
{
	if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0)
		LOG_FATAL("Could not load SDL Image library. SDL Error: " + C(SDL_GetError()));
	if (TTF_Init() != 0)
		LOG_FATAL("Could not load SDL TTF library. SDL Error: " + C(SDL_GetError()));
	if (Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_MID | MIX_INIT_OPUS) == 0)
		LOG_FATAL("Could not load SDL Mixer library. SDL Error: " + C(SDL_GetError()));

	InitRendering();
	InitMixer();
}

void FinaliseMedia()
{
	FinaliseMixer();
	FinaliseRendering();

	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
}

void OnUpdateMedia()
{
	
}