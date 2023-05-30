#include "Audio.h"

#include <unordered_map>

#include "libs/sdl/include/SDL_mixer.h"

#include "src/core/Logging.h"

#define Channels std::unordered_map<std::string, int>

struct MAudioData
{
	Channels channels;

} * mixer = NULL;

void InitMixer()
{
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);

	mixer = new MAudioData;

	mixer->channels["sound"] = 0;
}

void FinaliseMixer()
{
	delete mixer;

	Mix_CloseAudio();
}

MAudio* Mixer_CreateAudio(std::string path)
{
	MAudio* new_music = new MAudio;
	
	new_music->path = path;
	new_music->handle = Mix_LoadWAV(path.c_str());

	if (new_music->handle == NULL)
		LOG_ERROR("Unable to load audio. Path given: '" + path + "'\n[SDL ERROR] " + C(SDL_GetError()));

	return new_music;
}

void Mixer_FreeAudio(MAudio* audio)
{
	Mix_FreeChunk(audio->handle);
	audio->handle = NULL;

	delete audio;
	audio = NULL;
}

MMusic* Mixer_CreateMusic(std::string path)
{
	MMusic* new_music = new MMusic;

	new_music->path = path;
	new_music->handle = Mix_LoadMUS(path.c_str());

	if (new_music->handle == NULL)
		LOG_ERROR("Unable to load audio. Path given: '" + path + "'\n[SDL ERROR] " + C(SDL_GetError()));

	return new_music;
}
void Mixer_FreeMusic(MMusic* music)
{
	Mix_FreeMusic(music->handle);
	music->handle = NULL;

	delete music;
	music = NULL;
}

void Mixer_PlayMusic(MMusic* music, int loops)
{
	int i = Mix_PlayMusic(music->handle, loops);
	if (i == -1)
		LOG_ERROR("Unable to play audio.\n[SDL ERROR] " + C(SDL_GetError()));
}
void Mixer_StopMusic()
{
	Mix_HaltMusic();
}