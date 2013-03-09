/*****************************************************************************
 *	CommonFormats.h
 *
 *	A class used to define custom data types in in IPC.
 *	
 *	IPC Manual: http://www.cs.cmu.edu/afs/cs/project/TCA/ftp/IPC_Manual.pdf
 *	
 *	The CommonFormats class consists of two array structures (and an integral
 *	value representing the number of elements in each array). One array is
 *	used to store the name of the custom data type, while the other is used to
 *	store the format string used by IPC to marshall the custom data type. more
 *	information about data marshalling in IPC can be found in the manual (link
 *	above).
 *	
 *	Dependancies: None
 *	
 *****************************************************************************/

#pragma once

namespace Pave_Libraries_Common
{
	class CommonFormats
	{
	public:
		// Number of common formats, also the number of elements in each array
		static int num_commonFormats;

		// Names of custom types for use with IPC
		static char *commonFormatNames[];

		// Format strings of types for use with IPC
		static char *commonFormats[];
	};
}