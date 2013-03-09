#pragma once

namespace Pave_Libraries_Common
{
	// Abstract classes that allow multiple handlers
	class Handler0
	{
	public:
		virtual void handler0(void *data) = 0;
	};
	
	class Handler1
	{
	public:
		virtual void handler1(void *data) = 0;
	};
	
	class Handler2
	{
	public:
		virtual void handler2(void *data) = 0;
	};

	class Handler3
	{
	public:
		virtual void handler3(void *data) = 0;
	};

	class Handler4
	{
	public:
		virtual void handler4(void *data) = 0;
	};

	class Handler5
	{
	public:
		virtual void handler5(void *data) = 0;
	};
}