#pragma once

#include "libs/sdl/include/SDL.h"

#include <string>

void CreateWindow(std::string name, int width, int height);
void DestroyWindow();

// <api>_<command>, Lua-binded functions.

void Window_Show();
SDL_Window* Window_GetHandle();
std::string Window_GetName();
void Window_SetName(std::string name);
int Window_GetWidth();
int Window_GetHeight();
void Window_SetWidthHeight(int w, int h);
bool Window_GetFullscreen();
void Window_SetFullscreen();
void Window_UnsetFullscreen();
bool Window_GetMaximised();
void Window_SetMaximised();
void Window_UnsetMaximised();