#include "Window.h"

#include "Logging.h"

struct Window
{
public:
	SDL_Window* m_window;
	std::string m_name;
	int m_width;
	int m_height;
	bool m_fullscreen;
	bool m_maximised;

}* window;



void CreateWindow(std::string name, int width, int height)
{
	window = new Window;
	window->m_name = name;
	window->m_width = width;
	window->m_height = height;
	window->m_fullscreen = false;
	window->m_maximised = false;

	window->m_window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
	if (window->m_window == NULL)
		LOG_FATAL("Failed to create application window.\n[SDL ERROR] " + C(SDL_GetError()));
}

void DestroyWindow()
{
	SDL_DestroyWindow(window->m_window);
	window = NULL;
}



void Window_Show()
{
	SDL_ShowWindow(window->m_window);
}

SDL_Window* Window_GetHandle()
{
	return window->m_window;
}

std::string Window_GetName()
{
	return window->m_name;
}

void Window_SetName(std::string name)
{
	window->m_name = name;
	SDL_SetWindowTitle(window->m_window, window->m_name.c_str());
}

int Window_GetWidth()
{
	return window->m_width;
}

int Window_GetHeight()
{
	return window->m_height;
}

void Window_SetWidthHeight(int w, int h)
{
	window->m_width = w;
	window->m_width = h;
	SDL_SetWindowSize(window->m_window, window->m_width, window->m_height);
}

bool Window_GetFullscreen()
{
	return window->m_fullscreen;
}

void Window_SetFullscreen()
{
	window->m_fullscreen = true;
	SDL_SetWindowFullscreen(window->m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void Window_UnsetFullscreen()
{
	window->m_fullscreen = false;
	SDL_SetWindowFullscreen(window->m_window, 0);
}

bool Window_GetMaximised()
{
	return window->m_maximised;
}

void Window_SetMaximised()
{
	window->m_maximised = true;
	SDL_MaximizeWindow(window->m_window);
}

void Window_UnsetMaximised()
{
	window->m_maximised = false;
	SDL_RestoreWindow(window->m_window);
}
