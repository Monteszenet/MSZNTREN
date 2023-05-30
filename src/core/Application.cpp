#include "Application.h"

#include <iostream>

#include "libs/sdl/include/SDL.h"
#include "src/media/Media.h"
#include "Scripts.h"
#include "Input.h"
#include "Window.h"
#include "Logging.h"
#include "src/program/Program.h"

Application* app;

void Application::Run()
{
	try {
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);		LOG_INFO("Initialised SDL.");
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		//SDL_GL_SetSwapInterval(1);

		EnterScriptingContext();

		CompileLuaToMSZNT("game\\scripts");

		InitLuaEnvironment();

		Lua_RunScriptFile("game/scripts/config.lua");

		CreateWindow(m_name, m_window_x, m_window_y);

		InitMedia();

		InitInput();

		CREATE_PROGRAM;
		PROGRAM->Init();				LOG_INFO("Initialised program.");

		Window_Show();				LOG_INFO("Showed window.");

		m_running = true;

		LOG_INFO(LOG_DEMARCATION);	 LOG_INFO("Entered program runtime.");

		while (m_running == true)
		{
			handle_input();

			PROGRAM->OnUpdate();
		}

		LOG_INFO(LOG_DEMARCATION); LOG_INFO("Exited program runtime.");

		PROGRAM->Finalise();

		ExitScriptingContext();
		FinaliseMedia();
		FinaliseInput();
		DestroyWindow();
		SDL_Quit();

	}
	catch (MLog error)
	{
		LOG_INFO(LOG_DEMARCATION); LOG_INFO("Quitting program due to fatal error.");
	};

	return;
}

void Application::handle_input()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			m_running = false;
			break;
		case SDL_TEXTINPUT:
			//*GetTextInputString() += e.text.text;
			break;
		case SDL_TEXTEDITING:
			//composition = e.edit.text;
			//cursor = e.edit.start;
			//selection_len = e.edit.length;
			break;
		}
		UpdateKey(e.type, e.key.keysym.scancode);
		UpdateMouse(e);
	}
	UpdateInput();
}



void App_Config(std::string title, std::string version, int w, int h)
{
	app->m_name = title;
	app->m_version = version;
	app->m_window_x = w;
	app->m_window_y = h;
}

void App_Quit()
{
	app->m_running = false;
}

Application* CreateApplication(ApplicationParameters params)
{
	app = new Application;
	app->m_base_directory = params.base_dir;
	app->m_command_line_args = params.command_line_args;
	return app;
}

void DeleteApplication()
{
	delete app;
	app = NULL;
}
