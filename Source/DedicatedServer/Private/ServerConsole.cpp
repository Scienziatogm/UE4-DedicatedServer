// Copyright 2004-2016 YaS-Online, Inc. All Rights Reserved.

#include "DedicatedServerPrivatePCH.h"

DEFINE_LOG_CATEGORY( LogServerConsole );

#if WITH_SERVER_CODE
	// Fixme: We need to workaround a silly issue inside the engine where the help commands relies on Slate being present...
	// See: https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Runtime/Engine/Private/UnrealEngine.cpp#L3151
	void DumpConsoleHelp()
	{
		UE_LOG(LogEngine, Display, TEXT("Console Help:"));
		UE_LOG(LogEngine, Display, TEXT("============="));
		UE_LOG(LogEngine, Display, TEXT(" "));
		UE_LOG(LogEngine, Display, TEXT("A console variable is a engine wide key value pair. The key is a string usually starting with the subsystem prefix followed"));
		UE_LOG(LogEngine, Display, TEXT("by '.' e.g. r.BloomQuality. The value can be of different tpe (e.g. float, int, string). A console command has no state associated with"));
		UE_LOG(LogEngine, Display, TEXT("and gets executed immediately."));
		UE_LOG(LogEngine, Display, TEXT(" "));
		UE_LOG(LogEngine, Display, TEXT("Console variables can be put into ini files (e.g. ConsoleVariables.ini or BaseEngine.ini) with this syntax:"));
		UE_LOG(LogEngine, Display, TEXT("<Console variable> = <value>"));
		UE_LOG(LogEngine, Display, TEXT(" "));
		UE_LOG(LogEngine, Display, TEXT("DumpConsoleCommands         Lists all console variables and commands that are registered (Some are not registered)"));
		UE_LOG(LogEngine, Display, TEXT("<Console variable>          Get the console variable state"));
		UE_LOG(LogEngine, Display, TEXT("<Console variable> ?        Get the console variable help text"));
		UE_LOG(LogEngine, Display, TEXT("<Console variable> <value>  Set the console variable value"));
		UE_LOG(LogEngine, Display, TEXT("<Console command> [Params]  Execute the console command with optional parameters"));

		UE_LOG(LogEngine, Display, TEXT(" "));

		FString FilePath = FPaths::ProjectSavedDir() + TEXT("ConsoleHelp.html");

		UE_LOG(LogEngine, Display, TEXT("To browse console variables open this: '%s'"), *FilePath);
		UE_LOG(LogEngine, Display, TEXT(" "));
	}

	#if PLATFORM_WINDOWS
		#include "AllowWindowsPlatformTypes.h"

		namespace ConsoleConstants
		{
			uint32 WIN_ATTACH_PARENT_PROCESS = ATTACH_PARENT_PROCESS;
			uint32 WIN_STD_INPUT_HANDLE = STD_INPUT_HANDLE;
			uint32 WIN_STD_OUTPUT_HANDLE = STD_OUTPUT_HANDLE;
			uint32 WIN_STD_ERROR_HANDLE = STD_ERROR_HANDLE;
		}

		#include "HideWindowsPlatformTypes.h"
	#endif

	FServerConsole::FServerConsole()
	{
		m_pConsole = static_cast<FOutputDeviceConsolePlatform*>( GLogConsole );
		m_iCommandHistoryIndex = -1;

		#if PLATFORM_WINDOWS
			m_hOutputHandle = INVALID_HANDLE_VALUE;
			m_hInputHandle = INVALID_HANDLE_VALUE;
		#endif
	}

	FServerConsole::~FServerConsole()
	{
	}

	void FServerConsole::Show( bool bShowWindow )
	{
		if( !m_pConsole->IsShown() ) m_pConsole->Show( bShowWindow );

		#if PLATFORM_WINDOWS
			if( m_hOutputHandle == INVALID_HANDLE_VALUE )
			{
				m_hOutputHandle = ::GetStdHandle( ConsoleConstants::WIN_STD_OUTPUT_HANDLE );
				m_hInputHandle = ::GetStdHandle( ConsoleConstants::WIN_STD_INPUT_HANDLE );

				// Fix input mode as it defaults to 439, which is bigger then all possible flags combined...
				if( m_hInputHandle != INVALID_HANDLE_VALUE ) ::SetConsoleMode( m_hInputHandle, ENABLE_PROCESSED_INPUT );
			}
		#elif PLATFORM_MAC
		#elif PLATFORM_LINUX
			initscr();
			noecho();
			cbreak();
			keypad( stdscr, TRUE );
			scrollok( stdscr, TRUE );
			start_color();
			init_pair( 1, COLOR_GREEN, COLOR_BLACK );
			SetCursorPosition( coords( LINES - 1, 0 ) );
		#else
			#error You shall not pass!
		#endif
	}

	bool FServerConsole::IsShown()
	{
		return m_pConsole->IsShown();
	}

	bool FServerConsole::IsAttached()
	{
		return m_pConsole->IsAttached();
	}

	#if PLATFORM_WINDOWS
		void FServerConsole::Serialize( const TCHAR* sData, ELogVerbosity::Type eVerbosity, const class FName& sCategory, const double fTime )
		{
			FScopeLock hLock( &m_hLock );

			#if PLATFORM_WINDOWS
				COORD hCursorPosition = GetCursorPosition();
			#endif

			ClearInputLine();

			m_pConsole->Serialize( sData, eVerbosity, sCategory, fTime );

			RedrawInputLine();

			#if PLATFORM_WINDOWS
				hCursorPosition.Y = GetCursorPosition().Y;

				SetCursorPosition( hCursorPosition );
			#endif
		}
	#endif //PLATFORM_WINDOWS

	void FServerConsole::Serialize( const TCHAR* sData, ELogVerbosity::Type eVerbosity, const class FName& sCategory )
	{
	#if PLATFORM_WINDOWS
		Serialize( sData, eVerbosity, sCategory, -1.0 );
	#elif PLATFORM_LINUX
		//coords hCursorPosition = GetCursorPosition();

		ClearInputLine();

		m_pConsole->Serialize( sData, eVerbosity, sCategory );

		RedrawInputLine();

		/*hCursorPosition.Y = GetCursorPosition().Y;
		SetCursorPosition( hCursorPosition );*/
	#endif //PLATFORM_WINDOWS
	}
#endif //WITH_SERVER_CODE
