// Copyright 2016 Scott Bevin, All Rights Reserved

#if defined( AXPLATFORM_WINDOWS )

#include "AXThreading.h"

#include <windows.h>  

// Sets the name of a thread
void AXThreading::AXThread::SetThreadName( const AXString& name )
{
	mName = name;

	const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)  
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType; // Must be 0x1000.  
		LPCSTR szName; // Pointer to name (in user addr space).  
		DWORD dwThreadID; // Thread ID (-1=caller thread).  
		DWORD dwFlags; // Reserved for future use, must be zero.  
	} THREADNAME_INFO;
#pragma pack(pop)  

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = name.c_str();
	info.dwThreadID = ::GetThreadId( static_cast<HANDLE>( mNativeThread->native_handle( ) ) );
	info.dwFlags = 0;

#pragma warning(push)  
#pragma warning(disable: 6320 6322)  
	__try
	{
		RaiseException( MS_VC_EXCEPTION, 0, sizeof( info ) / sizeof( ULONG_PTR ), ( ULONG_PTR* )&info );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
#pragma warning(pop) 
}

#endif // #if defined( AXPLATFORM_WINDOWS )