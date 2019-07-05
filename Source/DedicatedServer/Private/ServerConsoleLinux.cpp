// Copyright 2004-2016 YaS-Online, Inc. All Rights Reserved.

#include "DedicatedServerPrivatePCH.h"
#if PLATFORM_LINUX
	#include <iostream>
	#include <sstream>
	#include <string>
#endif

void DumpConsoleHelp();

#if WITH_SERVER_CODE && PLATFORM_LINUX
	void FServerConsole::Tick()
	{
		int hInputEvents = getch();
		if ( hInputEvents == ERR )
		{
			// Handle Error
			return;
		}
		if ( hInputEvents == '\n' )
		{
			auto& sInput = m_sInput;
			AsyncTask( ENamedThreads::GameThread, [this, sInput]()
			{
				TCHAR sOutput[MAX_SPRINTF] = TEXT( "" );
				if ( sInput.StartsWith( TEXT( "help" ) ) )
				{
					ClearInputLine();
					m_sInput.Empty();

					DumpConsoleHelp();

					attron( COLOR_PAIR( 1 ) );
					FCString::Sprintf( sOutput, TEXT( "> %s%s" ), *sInput, LINE_TERMINATOR );
					printf( "%s", TCHAR_TO_ANSI( sOutput ) );
					refresh();
					attroff( COLOR_PAIR( 1 ) );

					m_hCommandHistory.Add( sInput );

					RedrawInputLine();
				}
				else if ( GEngine->Exec( GWorld, *sInput ) )
				{
					ClearInputLine();
					m_sInput.Empty();

					attron( COLOR_PAIR( 1 ) );
					FCString::Sprintf( sOutput, TEXT( "> %s%s" ), *sInput, LINE_TERMINATOR );
					printf( "%s", TCHAR_TO_ANSI( sOutput ) );
					refresh();
					attroff( COLOR_PAIR( 1 ) );

					m_hCommandHistory.Add( sInput );

					RedrawInputLine();
				}
				else if ( GWorld->GetAuthGameMode() != nullptr && GWorld->GetAuthGameMode()->ProcessConsoleExec( *sInput, *GLog, nullptr ) )
				{
					ClearInputLine();
					m_sInput.Empty();

					attron( COLOR_PAIR( 1 ) );
					FCString::Sprintf( sOutput, TEXT( "> %s%s" ), *sInput, LINE_TERMINATOR );
					printf( "%s", TCHAR_TO_ANSI( sOutput ) );
					refresh();
					attroff( COLOR_PAIR( 1 ) );

					m_hCommandHistory.Add( sInput );

					RedrawInputLine();
				}
				else
				{
					ClearInputLine();
					m_sInput.Empty();

					attron( COLOR_PAIR( 1 ) );
					FCString::Sprintf( sOutput, TEXT( "Unknown Command: %s%s" ), *sInput, LINE_TERMINATOR );
					printf( "%s", TCHAR_TO_ANSI( sOutput ) );
					refresh();
					attroff( COLOR_PAIR( 1 ) );

					RedrawInputLine();
				}
			} );
		}
		else if ( hInputEvents == '\b' )
		{
			if ( m_sInput.Len() >= 1 )
			{
				m_sInput.RemoveAt( m_sInput.Len() - 1 );
				m_sInput.AppendChar( ' ' );
				RedrawInputLine();
				m_sInput.RemoveAt( m_sInput.Len() - 1 );
				SetCursorPosition( COORD( LINES - 1, m_sInput.Len() ) );
			}
		}
		else if ( hInputEvents == 27 ) // Escape Key Event
		{
			ClearInputLine();
			m_sInput.Empty();
		}
		else if ( hInputEvents == '\t' )
		{
			// ToDo: AutoCompletion
		}
		else if ( hInputEvents == KEY_UP )
		{
			if ( m_hCommandHistory.Num() == 0 ) return;

			if ( m_iCommandHistoryIndex == -1 ) m_iCommandHistoryIndex = m_hCommandHistory.Num() - 1;
			else --m_iCommandHistoryIndex;

			if ( m_iCommandHistoryIndex < 0 ) m_iCommandHistoryIndex = 0;

			if ( ( m_iCommandHistoryIndex + 1 ) == m_hCommandHistory.Num() && m_sUserInput.Len() == 0 ) m_sUserInput = m_sInput;

			m_sInput = m_hCommandHistory[m_iCommandHistoryIndex];

			RedrawInputLine();
		}
		else if ( hInputEvents == KEY_DOWN )
		{
			if ( m_iCommandHistoryIndex == -1 ) return;

			++m_iCommandHistoryIndex;

			if ( m_iCommandHistoryIndex > m_hCommandHistory.Num() ) m_iCommandHistoryIndex = m_hCommandHistory.Num();

			if ( m_iCommandHistoryIndex == m_hCommandHistory.Num() )
			{
				if ( m_sUserInput.Len() > 0 ) m_sInput = m_sUserInput;
				else m_sInput.Empty();

				RedrawInputLine();
			}
			else
			{
				m_sInput = m_hCommandHistory[m_iCommandHistoryIndex];

				RedrawInputLine();
			}
		}
		else if ( hInputEvents == KEY_LEFT )
		{
			COORD hCursorPosition( GetCursorPosition() );

			if ( hCursorPosition.X > 0 )
			{
				--hCursorPosition.X;
				SetCursorPosition( hCursorPosition );
			}
		}
		else if ( hInputEvents == KEY_RIGHT )
		{
			COORD hCursorPosition( GetCursorPosition() );

			if ( hCursorPosition.X < m_sInput.Len() )
			{
				++hCursorPosition.X;
				SetCursorPosition( hCursorPosition );
			}
		}
		else if ( hInputEvents == KEY_HOME )
		{
			COORD hCursorPosition( GetCursorPosition() );
			hCursorPosition.X = 0;
			SetCursorPosition( hCursorPosition );
		}
		else if ( hInputEvents == KEY_END )
		{
			COORD hCursorPosition( GetCursorPosition() );
			hCursorPosition.X = m_sInput.Len();
			SetCursorPosition( hCursorPosition );
		}
		else if ( hInputEvents == KEY_DC )
		{
			COORD hCursorPosition( GetCursorPosition() );

			if ( hCursorPosition.X <= m_sInput.Len() )
			{
				m_sInput.RemoveAt( hCursorPosition.X );
				m_sInput.AppendChar( ' ' );

				RedrawInputLine();

				m_sInput.RemoveAt( m_sInput.Len() - 1 );

				SetCursorPosition( hCursorPosition );
			}
		}
		else if ( hInputEvents == KEY_RESIZE )
		{
			refresh();
		}
		else
		{
			m_sInput.AppendChar( hInputEvents );
			RedrawInputLine();
		}
	}

	void FServerConsole::SendNullInput()
	{
		endwin();
	}

	void FServerConsole::ClearInputLine()
	{
		COORD hCursorPosition( GetCursorPosition() );
		if ( hCursorPosition.X == 0 ) return;
		COORD nCursorPosition( hCursorPosition.Y, 0 );
		SetCursorPosition( nCursorPosition );
		clrtoeol();
	}

	void FServerConsole::RedrawInputLine()
	{
		ClearInputLine();

		if ( !m_sInput.IsEmpty() )
		{
			attron( COLOR_PAIR( 1 ) );
			printf( "%s", TCHAR_TO_ANSI( *m_sInput ) );
			SetCursorPosition( COORD( LINES - 1, m_sInput.Len() ) );
			refresh();
			attroff( COLOR_PAIR( 1 ) );
		}
	}

	COORD FServerConsole::GetCursorPosition()
	{
		COORD retval;
		getyx( stdscr, retval.Y, retval.X );
		return retval;
	}

	bool FServerConsole::SetCursorPosition( COORD hCursorPosition )
	{
		int retval = move( hCursorPosition.Y, hCursorPosition.X );
		refresh();
		return retval == OK ? true : false;
	}
#endif
