#pragma once

#include <string>
#include <vector>

#include "libs/lua/include/lua.hpp"

#include "Utility.h"

struct ApplicationParameters
{
	std::string base_dir;
	CommandLineArgs command_line_args;
};

class Application
{	
public:
	void Run();

private:
	const std::string auto_m_name = "MSZNT";
	const std::string auto_m_version = "0.00";
	const int auto_m_window_x = 960;
	const int auto_m_window_y = 720;

	const std::string auto_m_current_context = "NONE";

private:
	std::string m_name = auto_m_name;
	std::string m_version = auto_m_version;
	std::string m_base_directory;
	bool m_running = false;
	int m_window_x = auto_m_window_x;
	int m_window_y = auto_m_window_y;
	CommandLineArgs m_command_line_args;

private:
	void handle_input();

	friend void App_Config(std::string title, std::string version, int w, int h);
	friend void App_Quit();
	friend Application* CreateApplication(ApplicationParameters params);
	friend void DeleteApplication();
};

void App_Config(std::string title, std::string version, int w, int h);
void App_Quit();

Application* CreateApplication(ApplicationParameters params);
void DeleteApplication();