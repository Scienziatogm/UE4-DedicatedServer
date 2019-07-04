// Copyright 2004-2016 YaS-Online, Inc. All Rights Reserved.

#pragma once

DECLARE_LOG_CATEGORY_EXTERN( LogServerConsole, Log, All );

#if WITH_SERVER_CODE
	#if PLATFORM_WINDOWS
		#include "Public/Windows/WindowsConsoleOutputDevice.h"
		#include "Public/Windows/WindowsHWrapper.h"
		#define FOutputDeviceConsolePlatform FWindowsConsoleOutputDevice	
	#elif PLATFORM_MAC
		#include "Public/Mac/MacConsoleOutputDevice.h"
		#define FOutputDeviceConsolePlatform FMacConsoleOutputDevice
	#elif PLATFORM_LINUX
		#include "Public/Linux/LinuxConsoleOutputDevice.h"
		#define FOutputDeviceConsolePlatform FLinuxConsoleOutputDevice

		struct coords
		{
			int X;
			int Y;

			coords( int Y, int X )
			{
				this->Y = Y;
				this->X = X;
			}

			coords()
			{
				this->Y = 0;
				this->X = 0;
			}
		};
	#else
		#error How did you get here?
	#endif

	class FServerConsole : public FOutputDeviceConsolePlatform
	{
		public:
			FServerConsole();
			virtual ~FServerConsole();

			void Tick();
			void SendNullInput();

			void Show( bool bShowWindow ) override;
			bool IsShown() override;
			bool IsAttached() override;

			void Serialize( const TCHAR* sData, ELogVerbosity::Type eVerbosity, const class FName& sCategory ) override;

			#if PLATFORM_WINDOWS
				void Serialize( const TCHAR* sData, ELogVerbosity::Type eVerbosity, const class FName& sCategory, const double fTime ) override;
			#endif

		protected:
			void ClearInputLine();
			void RedrawInputLine();

			#if PLATFORM_WINDOWS
				COORD								GetCursorPosition();
				bool								SetCursorPosition( COORD hCursorPosition );
			#elif PLATFORM_LINUX
				coords								GetCursorPosition();
				bool								SetCursorPosition( coords hCursorPosition );
			#endif

		private:
			FOutputDeviceConsolePlatform*			m_pConsole;
			FString									m_sInput;
			FString									m_sUserInput;
			TArray<FString>							m_hCommandHistory;
			int32									m_iCommandHistoryIndex;

			mutable FCriticalSection				m_hLock;

			#if PLATFORM_WINDOWS
				HANDLE								m_hOutputHandle;
				HANDLE								m_hInputHandle;

				INPUT_RECORD						m_hCachedInputEvent;
			#endif
	};
#endif
