#pragma once

#include <string>

#include "libs/sdl/include/SDL_mixer.h"

#define M_MIXER_INFINITE_LOOP -1

struct MAudio
{
	Mix_Chunk* handle = NULL;
	std::string path;
};

struct MMusic
{
	Mix_Music* handle = NULL;
	std::string path;
};

void InitMixer();
void FinaliseMixer();

MAudio* Mixer_CreateAudio(std::string path);
void Mixer_FreeAudio(MAudio* audio);

MMusic* Mixer_CreateMusic(std::string path);
void Mixer_FreeMusic(MMusic* music);

void Mixer_PlayMusic(MMusic* music, int loops);
void Mixer_StopMusic();