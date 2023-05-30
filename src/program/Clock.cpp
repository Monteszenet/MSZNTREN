#include "Program.h"

#include "src/core/Timing.h"
#include "src/core/Logging.h"

struct Timer
{
	int seconds;
	uint64_t initial_time;
};

struct Clock
{
	int fps = 60;
	uint64_t milifps = 60000;

	uint64_t initial_time = 0;
	uint64_t total_elapsed_time = 0;

	uint64_t frame_start_time = 0;
	uint64_t frame_end_time = 0;
	uint64_t frame_elapsed_time = 0;

	int64_t sleep_time = 0;

	uint64_t counted_frames = 0;
	float avgfps = 0;

	LIBRARY(Timer) timer_library;

} clock;

void Program::InitClock()
{
	clock.initial_time = GetCurrentTime();

	clock.frame_start_time = clock.initial_time;

}

void Program::SetClockRate(int seconds)
{
	if (seconds <= 0)
		LOG_FATAL("Attempted to set FPS to an invalid value: " + S(seconds));
	clock.fps = seconds;
	clock.milifps = seconds * 1000;
}

uint64_t Program::GetClockTime()
{
	return GetCurrentTime();
}

void Program::TickClock()
{
	clock.counted_frames += 1;

	clock.frame_end_time = GetCurrentTime();

	clock.avgfps = clock.counted_frames / (clock.frame_end_time / 1000.f);

	clock.total_elapsed_time = clock.frame_end_time - clock.initial_time;

	clock.frame_elapsed_time = clock.frame_end_time - clock.frame_start_time;
	
	// No-no divide by 0.
	if (clock.frame_elapsed_time == 0)
		clock.frame_elapsed_time = 1;

	clock.sleep_time = ( 1000 / clock.fps ) - clock.frame_elapsed_time;
	if (( 1000 / clock.frame_elapsed_time ) < (1000 / clock.fps))
	{
		//LOG_INFO(S(clock.sleep_time));
		//PauseApplication((uint32_t)clock.sleep_time);
	}

	for (auto& t : clock.timer_library)
	{
		if (clock.frame_end_time - t.second.initial_time > t.second.seconds * 1000)
		{
			lua_pushstring(L(), t.first.c_str());
			clock.timer_library.erase(t.first);
			INVOKE_EVENT("TIMER");
		}
	}

	clock.frame_start_time = GetCurrentTime();
}



void Program::SetTimer(std::string name, int seconds)
{
	clock.timer_library[name] = { seconds, GetCurrentTime() };
}