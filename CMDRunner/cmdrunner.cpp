/**
	Author: Alwyn Dippenaar. ( alwyn.j.dippenaar@gmail.com )
	The impl for CMD Runner.
**/

#include "cmdrunner.h"


HHOOK g_hKeyboardHook;

char tmpbuf[2048];
bool quit = false;

bool ctl = false;
bool shift = false;
bool esc = false;
bool process = false;

wchar_t key[128];
wchar_t keyval[2048];


PROCESS_INFORMATION* ppi[2048];

void startprocess(void *keyval)
{
	STARTUPINFO* si = new STARTUPINFO();
	PROCESS_INFORMATION* pi = new PROCESS_INFORMATION();

	ZeroMemory(si, sizeof(STARTUPINFO));
	si->cb = sizeof(si);
	ZeroMemory(pi, sizeof(PROCESS_INFORMATION));

	// Start the child process. 
	if (!CreateProcess(NULL,   // No module name (use command line)
		(wchar_t*)keyval,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		si,            // Pointer to STARTUPINFO structure
		pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		OutputDebugString(L"CreateProcess failed..\r\n");
		_endthread();
		return;
	}

	for (int i = 0; i<2048; i++)
	{
		if (!ppi[i])
		{
			ppi[i] = pi;
			break;
		}
	}

	_endthread();
}

/**
	This function handles the key events.
**/
LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam )
{

	ZeroMemory( key,	sizeof(wchar_t) *  128 );
	ZeroMemory( keyval, sizeof(wchar_t) * 2048 );
	

	LPKBDLLHOOKSTRUCT str = NULL;
	str = ( LPKBDLLHOOKSTRUCT ) lParam;

	if ( str->vkCode == VK_ESCAPE )
	{
		esc = (wParam == WM_KEYDOWN);
	}
	if ( str->vkCode == VK_LCONTROL )
	{
		ctl = (wParam == WM_KEYDOWN);
	}
	if (str->vkCode == VK_LSHIFT)
	{
		shift = (wParam == WM_KEYDOWN);
	}


	if (!shift && esc && ctl)
	{
		quit = true;
		return 1;
	}

	if ( quit )
	{

		for ( int i=0; i<2048; i++ )
		{
			if ( ppi[i] ) 
			{
				OutputDebugString(L"Closing oprocess...\r\n");
				CloseHandle( ppi[i]->hProcess );

				OutputDebugString(L"Closing othread...\r\n");
				CloseHandle( ppi[i]->hThread );
				
				delete ppi[i];
				ppi[i] = NULL;
			}
		}

		if (g_hKeyboardHook) UnhookWindowsHookEx(g_hKeyboardHook);
		g_hKeyboardHook = NULL;

		exit(0);
		return 0;
	}

	process = false;

	//Run batches?
	if ( ctl && wParam == WM_KEYDOWN ) 
	{
		switch ( str->vkCode )
		{

		case 0x30:
			wcscpy_s(key, L"0");
			
			break;

		case 0x31:
			wcscpy_s(key, L"1");
			
			break;

		case 0x32:
			wcscpy_s(key, L"2");
			
			break;

		case 0x33:
			wcscpy_s(key, L"3");
			
			break;

		case 0x34:
			wcscpy_s(key, L"4");
			
			break;

		case 0x35:
			wcscpy_s(key, L"5");
			
			break;

		case 0x36:
			wcscpy_s(key, L"6");
			
			break;

		case 0x37:
			wcscpy_s(key, L"7");
			
			break;

		case 0x38:
			wcscpy_s(key, L"8");
			
			break;

		case 0x39:
			wcscpy_s(key, L"9");
			
			break;

		case VK_F1:
			wcscpy_s(key, L"F1");
			
			break;
		case VK_F2:
			wcscpy_s(key, L"F2");
			
			break;
		case VK_F3:
			wcscpy_s(key, L"F3");
			
			break;
		case VK_F4:
			wcscpy_s(key, L"F4");
			
			break;
		case VK_F5:
			wcscpy_s(key, L"F5");
			
			break;
		case VK_F6:
			wcscpy_s(key, L"F6");
			
			break;
		case VK_F7:
			wcscpy_s(key, L"F7");
			
			break;
		case VK_F8:
			wcscpy_s(key, L"F8");
			
			break;
		case VK_F9:
			wcscpy_s(key, L"F9");
			
			break;
		case VK_F10:
			wcscpy_s(key, L"F10");
			
			break;
		case VK_F11:
			wcscpy_s(key, L"F11");
			
			break;
		case VK_F12:
			wcscpy_s(key, L"F12");
			
			break;

		default:
			break;
		}
	}

	process = false;

	if ( wcslen( key ) > 0 )
	{
		GetPrivateProfileString(L"CMDRunner", 
			key, 
			L"NONE", 
			keyval, 
			sizeof(wchar_t) * 2048, 
			L".\\BatchConfig.ini");

		if ( wcscmp( keyval, L"NONE" ) != 0 ) 
		{
			process = true;

			//Run....
			OutputDebugString( L"\r\nRun... " );
			OutputDebugString( keyval );
			OutputDebugString( L" \r\n" );
			
			_beginthread(startprocess, 0, keyval);
		}
	}


	return (process ? 1 : 0);
}


/**
	The main entry point for this app.
*/
int CALLBACK WinMain(  HINSTANCE hInstance,  HINSTANCE hPrevInstance,  LPSTR lpCmdLine,  int nCmdShow )
{
	ZeroMemory( ppi, sizeof(PROCESS_INFORMATION*) * 2048 );


	g_hKeyboardHook =	SetWindowsHookEx( WH_KEYBOARD_LL,  LowLevelKeyboardProc, GetModuleHandle(NULL), 0 );

	MSG msg;
	int bRet = 0;
	while( !quit && (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0 ) 
	{
		Sleep(1);

		if (bRet == -1)
		{
			// handle the error and possibly exit
		}
		else
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	ShowCursor( TRUE );

	if ( g_hKeyboardHook ) UnhookWindowsHookEx( g_hKeyboardHook );

	return 0;

}
