#include "Data.h"
#include <cstdio>
#include <cmath>
#include <string>
#include <iostream>
using namespace std;

namespace Pave_Libraries_Common
{
	bool Data::initialized = false;
	vector<char*> Data::keys;
	vector<char*> Data::values;

	char* Data::getString(const char *key, bool &success)
	{
		if(!initialized)
			initialize();

		for(int i = 0; i < (int)keys.size(); i++)
		{
			if(strcmp(keys[i], key) == 0)
			{
				success = true;
				return values[i];
			}
		}
		success = false;
		return NULL;
	}

	double Data::getDouble(const char *key, bool &success)
	{
		char *str = getString(key, success);
		if(!success)
			return 0;
		success = true;
		return atof(str);
	}

	int Data::getInt(char *key, bool &success)
	{
		char *str = getString(key, success);
		if(!success)
			return 0;
		success = true;
		return atoi(str);
	}

	void Data::initialize()
	{
		if(initialized)
			return;
		string contents;
		char buffer[1001];
		initialized = true;
		FILE *dataFile;
		fopen_s(&dataFile, DATA_FILE, "r");
		if(!dataFile) return;
		int count;
		while((count = fread(buffer, 1, 1000, dataFile)) > 0)
		{
			buffer[count] = '\0';
			contents += buffer;
		}
		fclose(dataFile);
		vector<string> lines;
		vector<string> fields;
		stringTokenize(contents, lines, LINE_END);
		char *currentKey, *currentValue;
		for(int i = 0; i < (int)lines.size(); i++)
		{
			stringTokenize(lines[i], fields, DATA_SEPARATOR);
			if(fields.size() != 2) continue;
			currentKey = new char[fields[0].length() + 1];
			strcpy_s(currentKey, fields[0].length()+1, fields[0].c_str());
			currentValue = new char[fields[1].length() + 1];
			strcpy_s(currentValue, fields[1].length()+1, fields[1].c_str());
			keys.push_back(currentKey);
			values.push_back(currentValue);
		}

		// Debugging output
		//for(int i = 0; i < values.size(); i++)
		//{
		//	cout << "Key: " << keys[i] << "\n";
		//	cout << "Value: " << values[i] << "\n";
		//}
	}

	void Data::stringTokenize(string &input, vector<string> &output, const char *token)
	{
		output.clear();
		int location;
		int tokenLength = strlen(token);
		while((location = input.find(token)) >= 0)
		{
			output.push_back(input.substr(0,location));
			input = input.substr(std::min(location + tokenLength, (int)input.length() - 1), std::max((int)input.length() - location - tokenLength, 0));
		}
		if(input.length() > 0)
			output.push_back(input);
	}
}