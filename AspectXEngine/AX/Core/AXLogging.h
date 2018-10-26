// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include <stdio.h>
#include <list>
#include "AX/Utils/AXString.h"

#include "AX/Utils/AXUtils.h"
#include "AX/Core/AXSystem.h"
#include "AXSettings.h"
#include "AX/Utils/AXParent.h"
#include "AX/Utils/AXInterface.h"

class AXLogging : public AXParent< AXSystem< AXLogging >, AXLogging >
{
public:
	struct LogLevel
	{
		enum E : uint8_t
		{
			MinLogLevel = 0,

			Info = MinLogLevel,
			Warning,
			Error,

			MaxLogLevel,
		};

		static const AXString& ToString( E val )
		{
			static AXString strings[MaxLogLevel] = { "Info", "Warning", "Error" };
			return strings[val];
		}
	};


	struct LogEntry
	{
		LogLevel::E		mLogLevel;
		AXString		mTag;
		AXString		mMessage;
		AXString		mFile;
		int				mLine;
	};

	class AXILogListener : public AXInterface< AXILogListener >
	{
	public:
		/**
		 * Override to handle a log entry
		 */
		virtual void Log( const LogEntry& entry ) = 0;
	};

	class Settings : public AXSettingsFile::SettingsItem
	{
	public:

		/**
		* Constructor
		*/
		Settings( )
		{
			AXProperty< uint8_t >::MetaType::DropDownOptionsCollection ddCollection;

			for( uint8_t i( LogLevel::MinLogLevel ); i < LogLevel::MaxLogLevel; ++i )
			{
				ddCollection.push_back( AXProperty< uint8_t >::MetaType::DropDownOptionsCollectionEntry( i, LogLevel::ToString( ( LogLevel::E )i ) ) );
			}

			RegisterProperty( mLogLevelFilter, "Log Level" ).DisplayAsDropDown( ddCollection );

			RegisterProperty( mCrashOnError, "Crash On Error" );
		}

	public:
		/**
		* Anything below this log level will be filtered out
		*/
		AXProperty< uint8_t > mLogLevelFilter = LogLevel::Info;

		/**
		 * If true when a AXERROR occurs the game will crash
		 */
		AXProperty< bool > mCrashOnError = false;
	};

public:
	/**
	* Constructor
	*/
	AXLogging();
	
	/**
	* Destructor
	*/
	~AXLogging();
	
	/**
	* Shutdown the system
	*/
	virtual void OnShutdown( ) override;

	/**
	 * Constructs a log entry and fires it out to the listeners, shouldn't be called directly, should call from the provided macros
	 */
	template<typename ... Args>
	static void Log_Internal( LogLevel::E level, const char* tag, const char* file, int line, const char* msg, Args ... args );

	/**
	 * Registers a new listener to receive logs
	 */
	template< class T >
	void RegisterNewListener( );

protected:
	/**
	* Override to register a settings object for this system
	*/
	virtual void CreateEngineSettings( class AXSettingsFile& settings ) override;

private:
	/**
	 * Helper function to return the logging system
	 */
	static AXLogging* Get( );

private:
	/**
	 * The collection of all listeners
	 */
	std::list< AXILogListener* > mListeners;

	/**
	 * Pointer to our loaded settings
	 */
	Settings* mSettings;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Registers a new listener to receive logs
*/
template< class T >
void AXLogging::RegisterNewListener( )
{
	mListeners.push_back( new T( ) );
}

/**
* Constructs a log entry and fires it out to the listeners, shouldn't be called directly, should call from the provided macros
*/
template<typename ... Args>
inline void AXLogging::Log_Internal( LogLevel::E level, const char* tag, const char* file, int line, const char* msg, Args ... args )
{
	if( AXLogging* logger = AXLogging::Get( ) )
	{
		if( !logger->mListeners.empty( ) )
		{
			if( !logger->mSettings || level >= logger->mSettings->mLogLevelFilter.Val() )
			{
				LogEntry entry;
				entry.mLogLevel = level;
				entry.mFile = file;
				entry.mLine = line;
				entry.mTag = tag;
				entry.mMessage = AXUtils::FormatString( msg, args ... );

				for( auto listener : logger->mListeners )
				{
					listener->Log( entry );
				}

				if( logger->mSettings && logger->mSettings->mCrashOnError && level == LogLevel::Error )
				{
					assert( false );
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define AXLOG( TAG, STR, ... ) AXLogging::Log_Internal( AXLogging::LogLevel::Info, (TAG), __FILE__, __LINE__, (STR), __VA_ARGS__ )
#define AXWARN( TAG, STR, ... ) AXLogging::Log_Internal( AXLogging::LogLevel::Warning, (TAG), __FILE__, __LINE__, (STR), __VA_ARGS__ )
#define AXERROR( TAG, STR, ... ) AXLogging::Log_Internal( AXLogging::LogLevel::Error, (TAG), __FILE__, __LINE__, (STR), __VA_ARGS__ )

#define AXLOGONCEVARNAME AXJOIN( Logged, __LINE__ )
#define AXLOGONCEVARCHECKVALNAME AXJOIN( BoolFalse, __LINE__ )

#define AXLOGONCE( TAG, STR, ... ) do{ static bool AXLOGONCEVARNAME( false ); if( !AXLOGONCEVARNAME ){ AXLOG( (TAG), (STR), __VA_ARGS__ ); AXLOGONCEVARNAME = true; } } while( false )
#define AXWARNONCE( TAG, STR, ... ) do{ static bool AXLOGONCEVARNAME( false ); if( !AXLOGONCEVARNAME ){ AXWARN( (TAG), (STR), __VA_ARGS__ ); AXLOGONCEVARNAME = true; } } while( false )
#define AXERRORONCE( TAG, STR, ... ) do{ static bool AXLOGONCEVARNAME( false ); if( !AXLOGONCEVARNAME ){ AXERROR( (TAG), (STR), __VA_ARGS__ ); AXLOGONCEVARNAME = true; } } while( false )

#define AXLOG_UNIMPLEMENTED_FUNCTION AXERRORONCE( "Unimlemented", "Unimlemented function: %s (%s - %d)", __func__, __FILE__, __LINE__ );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Built-in loggers

class AXLogListener_ConsoleOutput : public AXLogging::AXILogListener
{
public:
	/**
	* Override to handle a log entry
	*/
	virtual void Log( const AXLogging::LogEntry& entry ) override;
};