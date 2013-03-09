#include "IPCConnect.h"
#include "Data.h"
#include "ipc.h"
#include <stdio.h>

namespace Pave_Libraries_Common
{
	void IPCConnect(const char *moduleName)
	{
		IPC_setVerbosity(IPC_Print_Warnings);
		bool success = false;
		char *server = Data::getString("IPC_Server", success);
		IPC_RETURN_TYPE result;
		if(success)
		{
			printf("IPC Server: %s\n\n", server);
			result = IPC_connectModule(moduleName, server);
		}
		else
		{
			printf("IPC Server: localhost (default)\n\n");
			result = IPC_connectModule(moduleName, "localhost");
		}
		if(result == IPC_OK)
			printf("Connected to server!\n");
		else
			printf("Unable to connect to server!\n");
	}
}