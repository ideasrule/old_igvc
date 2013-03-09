
#include "IPCMessage.h"
#include "ipc.h"
#include "Handler.h"
#include <stdio.h>
#include <vector>
#include <string>

namespace Pave_Libraries_Common
{
	// Common formats have not been defined on initialization
	bool IPCMessage::commonFormatsAreDefined = false;
	
	// Constructor takes the name, a format string representing the message
	// content, the length of the message, a bool representing or not the
	// length specified is IPC_VARIABLE_LENGTH, and whether or not IPC should
	// automatically take care of marshalling the message
	IPCMessage::IPCMessage(char *name, char *format, unsigned int length,
						   bool variableLength, bool marshalling)
	{
		this->name = name;
		this->format = format;
		this->length = length;
		this->isMarshalled = marshalling;
		if(variableLength)
			this->length = IPC_VARIABLE_LENGTH;
		isDefined = false;
		isSubscribed = false;
		subscribers = new vector<SubscriberData*>();
		// Default: automatic memory management after data sent to subscribers
		//			data procesed through queue.
		isAllAutoFreeNotFresh = true;
	}

	bool IPCMessage::define()
	{
		IPC_RETURN_TYPE result;
		if(isDefined && commonFormatsAreDefined)
			return true;
		
		if(!commonFormatsAreDefined)
		{
			for(int i = 0; i < CommonFormats::num_commonFormats; i++)
			{
				result = IPC_defineFormat(CommonFormats::commonFormatNames[i],
										  CommonFormats::commonFormats[i]);
				if(result != IPC_OK)
				{
					printf("Error defining format!\nName: %s\nFormat: %s\n",
						   CommonFormats::commonFormatNames[i],
						   CommonFormats::commonFormats[i]);
					return false;
				}
			}
			commonFormatsAreDefined = true;
		}

		result = IPC_defineMsg(name, length, format);
		if(result != IPC_OK)
			printf("Error defining message!\nName: %s\nLength: %d\nFormat: %s\n",
				   name, length, format);
		else
			isDefined = true;

		// Constrain the message queue by default
		setQueueLength(DEFAULT_QUEUE_LENGTH);

		return result == IPC_OK;
	}
	
	// Publish an instance of the message
	bool IPCMessage::publish(void *content)
	{
		return publish(content, this->length);
	}
	
	// Publish an instance of the message with the given length
	bool IPCMessage::publish(void *content, unsigned int length)
	{
		// If the message isn't defined, define it, then publish it
		if(!isDefined)
			define();
		if(!isDefined)
			return false;
		
		// Error if the message lengths don't match
		if(this->length != IPC_VARIABLE_LENGTH && length != this->length)
		{
			printf("Error publishing message: length mismatch!\nName: "
				   "%s\nLength: %ud\nInput Length: %ud\n", name,
				   this->length, length);
			return false;
		}

		IPC_RETURN_TYPE result;
		// Publish to IPC
		if(this->isMarshalled)
			result = IPC_publishData(this->name, content);
		else
			result = IPC_publish(this->name, length, content);
		if(result != IPC_OK)
			printf("Error publishing message!\nName: %s\nLength: %ud\n", name,
				   length);
		return result == IPC_OK;
	}

	// Subscribe a function to this IPC message
	bool IPCMessage::subscribe(void (*handler)(void *data), bool autoFree,
							   bool freshCopy)
	{
		SubscriberData *subscriber = new SubscriberData;
		subscriber->type = FUNCTION;
		subscriber->handler = handler;
		subscriber->extra = NULL;
		subscriber->autoFree = autoFree;
		subscriber->freshCopy = freshCopy;
		return addSubscriber(subscriber);
	}

	// Subscribe the different Handler abstract classes
	bool IPCMessage::subscribe(Handler0 *handler, bool autoFree, bool freshCopy)
	{
		SubscriberData *subscriber = new SubscriberData;
		subscriber->type = HANDLER0;
		subscriber->handler = handler;
		subscriber->extra = NULL;
		subscriber->autoFree = autoFree;
		subscriber->freshCopy = freshCopy;
		return addSubscriber(subscriber);
	}

	bool IPCMessage::subscribe(Handler1 *handler, bool autoFree, bool freshCopy)
	{
		SubscriberData *subscriber = new SubscriberData;
		subscriber->type = HANDLER1;
		subscriber->handler = handler;
		subscriber->extra = NULL;
		subscriber->autoFree = autoFree;
		subscriber->freshCopy = freshCopy;
		return addSubscriber(subscriber);
	}
	bool IPCMessage::subscribe(Handler2 *handler, bool autoFree, bool freshCopy)
	{
		SubscriberData *subscriber = new SubscriberData;
		subscriber->type = HANDLER2;
		subscriber->handler = handler;
		subscriber->extra = NULL;
		subscriber->autoFree = autoFree;
		subscriber->freshCopy = freshCopy;
		return addSubscriber(subscriber);
	}
	bool IPCMessage::subscribe(Handler3 *handler, bool autoFree, bool freshCopy)
	{
		SubscriberData *subscriber = new SubscriberData;
		subscriber->type = HANDLER3;
		subscriber->handler = handler;
		subscriber->extra = NULL;
		subscriber->autoFree = autoFree;
		subscriber->freshCopy = freshCopy;
		return addSubscriber(subscriber);
	}
	bool IPCMessage::subscribe(Handler4 *handler, bool autoFree, bool freshCopy)
	{
		SubscriberData *subscriber = new SubscriberData;
		subscriber->type = HANDLER4;
		subscriber->handler = handler;
		subscriber->extra = NULL;
		subscriber->autoFree = autoFree;
		subscriber->freshCopy = freshCopy;
		return addSubscriber(subscriber);
	}
	bool IPCMessage::subscribe(Handler5 *handler, bool autoFree, bool freshCopy)
	{
		SubscriberData *subscriber = new SubscriberData;
		subscriber->type = HANDLER5;
		subscriber->handler = handler;
		subscriber->extra = NULL;
		subscriber->autoFree = autoFree;
		subscriber->freshCopy = freshCopy;
		return addSubscriber(subscriber);
	}

	// Centralized function for adding subscribers
	bool IPCMessage::addSubscriber(SubscriberData *subscriber)
	{
		// Define message in IPC if not yet defined
		// isDefined set to true in define() if operation is sucess
		if(!isDefined)
			define();
		// If define operation was unsuccessful, isDefined remains false,
		// delete the subscriber
		if(!isDefined)
		{
			delete subscriber;
			return false;
		}
		// Subscribe the message callback to IPC if not already subscribed
		// isSubscribed set to true in subscribeSelf() if operation is success
		if(!isSubscribed)
			subscribeSelf();
		// If subscribe operation was unsucessful, isSubscribed remains false,
		// delete the subscriber
		if(!isSubscribed)
		{
			delete subscriber;
			return false;
		}
		// if subscriber does not want auto memory management, or if the
		// subscriber wants the latest data, default setting is false
		if(!(subscriber->autoFree) || (subscriber->freshCopy))
			isAllAutoFreeNotFresh = false;
		/*if(!autoFreeSet)
		{
		autoFreeSet = true;
		autoFree = subscriber->autoFree;
		}
		else if(subscriber->autoFree != autoFree)
		{
		printf("Error: Attempted to add subscriber with different autoFree setting!");
		delete subscriber;
		return false;
		}*/
		// If everything is sucessful, add the subscriber to the vector of
		// subscribers to this message
		subscribers->push_back(subscriber);
		return true;
	}
	
	// Private helper function to handle subscription to message
	bool IPCMessage::subscribeSelf()
	{
		if(isSubscribed)
			return true;
		IPC_RETURN_TYPE result;
		// Call IPC subscribe functions according to whether the message uses
		// marshalled or unmarshalled data. Different callbacks are passed
		// according to whether marshalled or unmarshalled data is used.
		if(!isMarshalled)
			result = IPC_subscribe(name, IPCMessage::rawHandlerCallback, this);
		else
			result = IPC_subscribeData(name, IPCMessage::marshalledHandlerCallback, this);

		// success if return of IPC calls is IPC_OK
		if(result == IPC_OK)
		{
			isSubscribed = true;
			return true;
		}
		return false;
	}

	void IPCMessage::rawHandlerCallback(MSG_INSTANCE msgInstance, BYTE_ARRAY callData, void *clientData)
	{
		IPCMessage* m = (IPCMessage*)clientData;
		m->processSubscribers(callData);
	}

	void IPCMessage::marshalledHandlerCallback(MSG_INSTANCE msgInstance, BYTE_ARRAY callData, void *clientData)
	{
		IPCMessage* m = (IPCMessage*)clientData;
		m->processSubscribers(callData);//, IPC_dataLength(msgInstance));
	}

	void IPCMessage::processSubscribers(void *data)//, unsigned int length)
	{
		// Only marshall once if at all
		IPC_VARCONTENT_TYPE content;
		IPC_VARCONTENT_PTR varPtr = &content;
		bool usingMarshalled = false;
		if(isMarshalled && !isAllAutoFreeNotFresh && subscribers->size() != 1)
		{
			IPC_marshall(IPC_msgFormatter(name), data, varPtr);
			usingMarshalled = true;
		}

		for(unsigned int i = 0; i < subscribers->size(); i++)
		{
			SubscriberData *subscriber = subscribers->at(i);
			void *dataIn;

			// Handle copies
			if(subscribers->size() == 1 || isAllAutoFreeNotFresh)
				dataIn = data;
			else if(subscriber->autoFree && !(subscriber->freshCopy))
				dataIn = data;
			else if(!isMarshalled)
			{
				dataIn = malloc(length);
				dataIn = memcpy(dataIn, data, length);
			}
			else
				IPC_unmarshall(IPC_msgFormatter(name), varPtr->content, &dataIn);

			switch(subscriber->type)
			{
			case FUNCTION:
				{
					void (*handler)(void*) = (void (*)(void*))subscriber->handler;
					handler(dataIn);
					break;
				}

			case HANDLER0:
				{
					Handler0 *handler = (Handler0*)(subscriber->handler);
					handler->handler0(dataIn);
					break;
				}

			case HANDLER1:
				{
					Handler1 *handler = (Handler1*)(subscriber->handler);
					handler->handler1(dataIn);
					break;
				}

			case HANDLER2:
				{
					Handler2 *handler = (Handler2*)(subscriber->handler);
					handler->handler2(dataIn);
					break;
				}

			case HANDLER3:
				{
					Handler3 *handler = (Handler3*)(subscriber->handler);
					handler->handler3(dataIn);
					break;
				}

			case HANDLER4:
				{
					Handler4 *handler = (Handler4*)(subscriber->handler);
					handler->handler4(dataIn);
					break;
				}

			case HANDLER5:
				{
					Handler5 *handler = (Handler5*)(subscriber->handler);
					handler->handler5(dataIn);
					break;
				}

			default:
				break;
			}

			if(subscriber->autoFree && !isAllAutoFreeNotFresh)
			{
				if(isMarshalled)
					IPC_freeData(IPC_msgFormatter(name), dataIn);
				else
					IPC_freeByteArray(dataIn);
			}
		}

		if(isAllAutoFreeNotFresh)
		{
			if(isMarshalled)
				IPC_freeData(IPC_msgFormatter(name), data);
			else
				IPC_freeByteArray(data);
		}


		if(usingMarshalled)
			IPC_freeByteArray(varPtr->content);
	}

	bool IPCMessage::setQueueLength(int length)
	{
		if(!isDefined)
			define();
		if(!isDefined)
			return false;
		if(length < 1)
			return false;
		return IPC_setMsgQueueLength(name, length) == IPC_OK;
	}

	const char* IPCMessage::getName()
	{
		return name;
	}

	const char* IPCMessage::getFormat()
	{
		return format;
	}

	int IPCMessage::getLength()
	{
		return length;
	}
}
