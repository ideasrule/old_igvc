#pragma once

#include "FrameLogger.h"
#include "FrameLoggerText.h"
#include "FrameLoggerBin.h"
#include "FrameLoggerBin32.h"
//#include "FrameLoggerBin32Z.h"
#include "FrameLoggerTextBz2.h"
#include "FrameLoggerBinBz2.h"

#include <string>

class FrameLoggerFactory
{
private:
	static bool existsLog(
		const std::string& directory,
		const std::string& logName,
		const std::string& suffix,
		const std::string& ext) {

		ifstream transformed((directory + "/" + logName + "-" + suffix + "." + ext).c_str());
		if(transformed) {
			transformed.close();
			return true;
		}
		return false;
	}

public:
	static shared_ptr<FrameLogger> createFrameLogReader(const std::string &baseDirectory, const std::string &name)
	{
		std::string directory = baseDirectory + "/" + name + "-VISION";
		ifstream manifest((directory + "/manifest.txt").c_str());
		std::string logName;
		if(manifest) {
			std::string ignore;
			manifest >> ignore;
			manifest >> logName;
			manifest.close();

			typedef shared_ptr<FrameLogger> logptr;
			if(existsLog(directory, logName, "transformed-bin", "bz2")) {
				return logptr(new FrameLoggerBinBz2(baseDirectory, name));
			}
			else if(existsLog(directory, logName, "transformed-txt", "bz2")) {
				return logptr(new FrameLoggerTextBz2(baseDirectory, name));
			}
//			else if(existsLog(directory, logName, "transformed32-c", "bin")) {
//				return logptr(new FrameLoggerBin32Z(baseDirectory, name));
//			}
			else if(existsLog(directory, logName, "transformed", "bin")) {
				return logptr(new FrameLoggerBin(baseDirectory, name));
			}
			else if(existsLog(directory, logName, "transformed32", "bin")) {
				return logptr(new FrameLoggerBin32(baseDirectory, name));
			}
			else if(existsLog(directory, logName, "transformed", "txt")) {
				return logptr(new FrameLoggerText(baseDirectory, name));
			}
		}

		return shared_ptr<FrameLogger>();
	}

	static shared_ptr<FrameLogger> createFrameLogWriter(const std::string &baseDirectory)
	{
		return shared_ptr<FrameLogger>(new FrameLoggerBinBz2(baseDirectory));
	}

private:
	FrameLoggerFactory() {}  // no construction
};
