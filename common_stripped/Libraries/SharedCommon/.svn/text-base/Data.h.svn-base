#pragma once

#define DATA_FILE "C:\\IGVC\\configuration.txt"
#define DATA_SEPARATOR " = "
#define LINE_END "\n"

#include <vector>
#include <string>

namespace Pave_Libraries_Common
{
	class Data
	{
	public:
		static char* getString(const char *key, bool &succes);
		static double getDouble(const char *key, bool &success);
		static int getInt(char *key, bool &success);
		static void stringTokenize(std::string &input, std::vector<std::string> &output, const char *token);
		static void initialize();
	private:
		static bool initialized;
		static std::vector<char*> keys;
		static std::vector<char*> values;
	};
}