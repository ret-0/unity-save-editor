#ifndef LOG_HPP
#define LOG_HPP

#include "pop.hpp"

// 'log' is taken :(
namespace logger
{
	enum {
		INFO,
		SUCCESS,
		WARNING,
		ERROR,
		DEBUG
	};
	
	struct log_t {
		int type;
		string message;
		string source;
	};

	extern vector<log_t> logs;
}

namespace logger
{
	void DrawLatestLog();
	void DrawLogs(string sourceFilter);
	void Info(string source, string message);
	void Success(string source, string message);
	void Warning(string source, string message);
	void Error(string source, string message);
	void Debug(string source, string message);
	void Fatal(string source, string message);
}

#endif
