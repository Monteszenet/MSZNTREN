#pragma once

#include "Utility.h"

#include <string>

#define M_DEBUG

#define ERRLEV_LENIENT 0	// Crash only on fatal errors.
#define ERRLEV_MEDIUM 1		// Crash on fatal and normal errors.
#define ERRLEV_STRONG 2		// Crash on fatal errors, normal errors, and warnings.

#define ERROR_TREATMENT_LEVEL ERRLEV_STRONG

#define LOGTYPE_LUA 0
#define LOGTYPE_DEBUG 1
#define LOGTYPE_INFO 2
#define LOGTYPE_WARN 3
#define LOGTYPE_ERROR 4
#define LOGTYPE_FATAL 5

#define uint8_t unsigned char

struct MLog
{
	enum MLogType
	{
		LUA = LOGTYPE_LUA,
		DEBUG = LOGTYPE_DEBUG,
		INFO = LOGTYPE_INFO,
		WARN = LOGTYPE_WARN,
		ERROR = LOGTYPE_ERROR,
		FATAL = LOGTYPE_FATAL
	} log_type;

	std::string msg;
};

void InitLogging(std::string path);
void FinaliseLogging();

void FlushLog();
void DumpTrace(int num_trace);

void Log_Process(std::string msg);
void Log_Lua(std::string msg);
void Log_Debug(std::string msg);
void Log_Info(std::string msg);
void Log_Warn(std::string msg);
void Log_Error(std::string msg);
void Log_Fatal(std::string msg);

#ifdef M_DEBUG
#define LOG_PROCESS(msg) Log_Process(msg)
#define LOG_DEBUG(msg) Log_Debug(msg)
#else
#define LOG_PROCESS(msg)
#define LOG_DEBUG(msg)
#endif

#define LOG_LUA(msg) Log_Lua(msg)
#define LOG_INFO(msg) Log_Info(msg)
#define LOG_WARN(msg) Log_Warn(msg)
#define LOG_ERROR(msg) Log_Error(msg)
#define LOG_FATAL(msg) Log_Fatal(msg)

#define LOG_DEMARCATION "[-------------------]"

#define S(integer) std::to_string(integer)
#define C(string) CStrToString(string)
//#define C(str) str.c_str()