#include "Input.h"

#include "Logging.h"

Uint8 PastKeyboardMap[SDL_NUM_SCANCODES] = { 0 };

const Uint8* CurrentKeyboardMap;

// [0] x, [1] y
int MousePos[2] = { 0 };

Uint32 MouseState = 0;

Uint8 ButtonState[3] = { 0 };

std::string* text_input_ptr;

void InitInput()
{
	CurrentKeyboardMap = SDL_GetKeyboardState(NULL);
}

void FinaliseInput()
{

}

void UpdateKey(Uint32 type, SDL_Scancode code)
{
	if (type == SDL_KEYDOWN)
	{
		PastKeyboardMap[code] = 0;
	}
	if (type == SDL_KEYUP)
	{
		PastKeyboardMap[code] = 1;
	}
}

void UpdateInput()
{
	CurrentKeyboardMap = SDL_GetKeyboardState(NULL);
	MouseState = SDL_GetMouseState(&MousePos[0], &MousePos[1]);
}

void UpdateMouse(SDL_Event e)
{
	switch (e.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		ButtonState[e.button.button] = e.button.state;
		break;
	}
}

bool GetKeyDown(SDL_Scancode code)
{
	if (CurrentKeyboardMap[code] == 1 and PastKeyboardMap[code] == 0)
		return true;
	else
		return false;
}

bool GetKeyUp(SDL_Scancode code)
{
	if (CurrentKeyboardMap[code] == 0 and PastKeyboardMap[code] == 1)
		return true;
	else
		return false;
}

bool GetKeyHold(SDL_Scancode code)
{
	if (CurrentKeyboardMap[code] == 1)
		return true;
	else
		return false;
}



int GetMouseX()
{
	return MousePos[0];
}

int GetMouseY()
{
	return MousePos[1];
}

bool GetMouseButtonPressed(MouseButton button)
{
	if (MouseState & SDL_BUTTON(button))
	{
		if (ButtonState[button] == SDL_PRESSED)
			return true;
	}
	return false;
}




std::string* GetTextInputString()
{
	return text_input_ptr;
}

void StartTextInput(std::string* string_ptr)
{
	text_input_ptr = string_ptr;
	SDL_StartTextInput();
}

void StopTextInput()
{
	SDL_StopTextInput();
}