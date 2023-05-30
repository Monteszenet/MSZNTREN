#include "Program.h"

#include "src/core/Logging.h"

void Program::create_label(std::string name, LuaFunction thread)
{
	Chunk chunk = { name, "LABEL", 0, 0, thread };
	Label* label = new Label(name, "LABEL", chunk);
	create_executable_object(label);

	LabelLibrary[name] = label;
}

void Program::call_label(lua_State* L, Label* label)
{
	execute_object(L, label);;
}

void Program::validate_labels()
{
	bool splashcreen = false;
	bool main_menu = false;

	for (auto& l : LabelLibrary)
	{
		if (l.first == "SPLASHSCREEN")
			splashcreen = true;
		if (l.first == "MAIN_MENU")
			main_menu = true;
	}

	if (splashcreen == false)
		LOG_FATAL("Could not find 'SPLASHCREEN' label.");
	if (main_menu == false)
		LOG_FATAL("Could not find 'MAIN_MENU' label.");
}