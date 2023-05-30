#include "Logging.h"

#include <vector>
#include <fstream>

#include <iostream>

struct MProc
{
	std::string msg;
};

struct MLogData
{	
	std::string log_path;
	std::vector<MLog> log_history;
	std::vector<MProc> trace_history;

} mlog_data;



void InitLogging(std::string path)
{
	mlog_data.log_path = path;
}

void FinaliseLogging()
{

}



void FlushLog()
{
	LOG_INFO("Log flushed to file: '" + mlog_data.log_path + "'");

	std::ofstream log_file;
	log_file.open(mlog_data.log_path);
	
	log_file << "MSZNT LOG FILE.\n\n";

	for (auto& log : mlog_data.log_history)
	{
		std::string log_type;
		switch (log.log_type)
		{
		case MLog::LUA:
			log_type = "[LUA] ";
			break;
		case MLog::DEBUG:
			log_type = "[DEBUG] ";
			break;
		case MLog::INFO:
			log_type = "[INFO] ";
			break;
		case MLog::WARN:
			log_type = "[WARNING] ";
			break;
		case MLog::ERROR:
			log_type = "[ERROR] ";
			break;
		case MLog::FATAL:
			log_type = "[FATAL] ";
			break;
		}
		
		log_file << log_type << log.msg << "\n";
	}
	log_file.close();
}

void DumpTrace(int num_trace)
{

}



void Log_Process(std::string msg)
{
#ifdef M_DEBUG
	std::cout << msg << "\n";
#endif
	MProc proc = { msg };
	mlog_data.trace_history.push_back(proc);
}

void Log_Lua(std::string msg)
{
#ifdef M_DEBUG
	std::cout << msg << "\n";
#endif
	MLog lua = { MLog::LUA, msg };
	mlog_data.log_history.push_back(lua);
}

void Log_Debug(std::string msg)
{
#ifdef M_DEBUG
	std::cout << msg << "\n";
#endif
	MLog debug = { MLog::DEBUG, msg };
	mlog_data.log_history.push_back(debug);
}

void Log_Info(std::string msg)
{
#ifdef M_DEBUG
	std::cout << msg << "\n";
#endif
	MLog info = { MLog::INFO, msg };
	mlog_data.log_history.push_back(info);
}

void Log_Warn(std::string msg)
{
#ifdef M_DEBUG
	std::cout << msg << "\n";
#endif
	MLog warn = { MLog::WARN, msg };
	mlog_data.log_history.push_back(warn);
#if ERROR_TREATMENT_LEVEL == ERRLEV_STRONG
	throw warn;
#endif
}

void Log_Error(std::string msg)
{
#ifdef M_DEBUG
	std::cout << msg << "\n";
#endif
	MLog error = { MLog::ERROR, msg };
	mlog_data.log_history.push_back(error);
#if ERROR_TREATMENT_LEVEL >= ERRLEV_MEDIUM
	throw error;
#endif
}

void Log_Fatal(std::string msg)
{
#ifdef M_DEBUG
	std::cout << msg << "\n";
#endif
	MLog error = { MLog::FATAL, msg };
	mlog_data.log_history.push_back(error);
#if ERROR_TREATMENT_LEVEL >= ERRLEV_LENIENT
	throw error;
#endif
}