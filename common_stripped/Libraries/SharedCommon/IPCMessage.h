/*****************************************************************************
 *	IPCMessage.h
 *
 *	Provides a C++ wrapper for the messaging format in the IPC framework.
 *	
 *	IPC Manual: http://www.cs.cmu.edu/afs/cs/project/TCA/ftp/IPC_Manual.pdf
 *	
 *	IPCMessage is the centerpiece of the object-oriented wrapper for IPC,
 *	IPC++. In the case of IPCMessage, it is a container class for IPC messages
 *	and provides methods to automatically deal with the extra overhead with 
 *	messages in the IPC framework, such as auto-defining messages on the IPC
 *	network before publish/subscribe
 *	
 *	
 *	
 *	
 *****************************************************************************/
#pragma once
#include "ipc.h"
#include "Handler.h"
#include <vector>
#include "CommonFormats.h"

#define DEFAULT_QUEUE_LENGTH 10 // prevent central overflow

namespace Pave_Libraries_Common
{

	using namespace std;

	// Helper enums and structs
	enum SubscriberType {FUNCTION, HANDLER0, HANDLER1, HANDLER2, HANDLER3, HANDLER4, HANDLER5};
	struct SubscriberData
	{
		SubscriberType type;
		bool autoFree;
		bool freshCopy;
		void *handler;
		void *extra;
	};

	// A container class for IPC messages
	class IPCMessage
	{
	public:
		// Constructor takes the name, a format string representing the message
		// content, the length of the message, a bool representing or not the
		// length specified is IPC_VARIABLE_LENGTH, and whether or not IPC 
		// should automatically take care of marshalling the message
		IPCMessage(char *name, char *format, unsigned int length,
				   bool variableLength, bool marshalling);

		// Publish an instance of the message
		bool publish(void *content);
		
		// Publish an instance of the message with the given length
		bool publish(void *content, unsigned int length);
		
		// Subscribe methods

		// Subscribe a function
		bool subscribe(void (*handler)(void *data), bool autoFree, bool freshCopy);

		// Subscribe abstract handler classes
		bool subscribe(Handler0 *handler, bool autoFree, bool freshCopy);
		bool subscribe(Handler1 *handler, bool autoFree, bool freshCopy);
		bool subscribe(Handler2 *handler, bool autoFree, bool freshCopy);
		bool subscribe(Handler3 *handler, bool autoFree, bool freshCopy);
		bool subscribe(Handler4 *handler, bool autoFree, bool freshCopy);
		bool subscribe(Handler5 *handler, bool autoFree, bool freshCopy);
		bool setQueueLength(int length);

		const char* getName();
		const char* getFormat();
		int getLength();

	private:
		// Defines the IPC message
		bool define();
		bool subscribeSelf();
		bool addSubscriber(SubscriberData *subscriber);

		// Message info

		// The IPC message name
		char *name;

		// The IPC message definition string
		char *format;

		// The length of the message
		unsigned int length;

		bool isDefined;
		bool isSubscribed;
		bool isMarshalled;

		static bool commonFormatsAreDefined;

		bool isAllAutoFreeNotFresh;
		//bool autoFreeSet;
		//bool autoFree;

		// Client handlers
		vector<SubscriberData*> *subscribers;
		//void (*clientRawHandler)(void *data);
		// A static function as the callback so IPC can get a pointer to it
		

		// Callback functions
		static void rawHandlerCallback(MSG_INSTANCE msgInstance, BYTE_ARRAY callData, void *clientData);
		static void marshalledHandlerCallback(MSG_INSTANCE msgInstance, BYTE_ARRAY callData, void *clientData);
		void processSubscribers(void *data);
	};

	// More helper enums and structs
	struct CallbackData
	{
		IPCMessage *instance;
		void *extraData;
	};
}