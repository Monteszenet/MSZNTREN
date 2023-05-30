#pragma once

#include <unordered_map>

#include "libs/sdl/include/SDL.h"

#define MouseButton unsigned char
#define Mouse_LeftButton 1
#define Mouse_MiddleButton 2
#define Mouse_RightButton 3

void InitInput();
void FinaliseInput();

void UpdateKey(Uint32 type, SDL_Scancode code);
void UpdateInput();
void UpdateMouse(SDL_Event e);

bool GetKeyDown(SDL_Scancode code);
bool GetKeyUp(SDL_Scancode code);
bool GetKeyHold(SDL_Scancode code);

int GetMouseX();
int GetMouseY();
bool GetMouseButtonPressed(MouseButton button);

std::string* GetTextInputString();
void StartTextInput(std::string* string_ptr);
void StopTextInput();