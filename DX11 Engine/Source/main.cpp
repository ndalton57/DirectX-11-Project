#pragma once

#include "Game.h"
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

#define FAST_SHUTDOWN 0 // Work-around for long deallocation times. Shuts program down immediately, and leaks
						// all currently allocated memory. Windows will automatically clean it up, though.
						// Net result is no different since you're shutting the program down anyway.

// Global Intializations //
HWND gWindow;
float gFoV = 90;
float gSensitivity = 0.35f;
int gScreenWidth;
int gScreenHeight;
int gWindowWidth = DEFAULT_WIDTH;
int gWindowHeight = DEFAULT_HEIGHT;

LONG WINAPI CrashDumpToFile(_EXCEPTION_POINTERS* pExceptionInfo);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;

	switch (message)
	{
	case WM_QUIT:
		message = WM_DESTROY;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int cmdShow)
{
	// Crash Dump //
	SetUnhandledExceptionFilter(CrashDumpToFile);

	// Memory Leak Detection //
	#if _DEBUG
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		_CrtSetBreakAlloc(-1);
	#endif

	// - Window Initialization - //
	MSG message;
	RECT rScreen;
	WNDCLASSEX windowClass;

	// Grab Screen Resolution //
	GetWindowRect(GetDesktopWindow(), &rScreen);
	gScreenWidth = rScreen.right;
	gScreenHeight = rScreen.bottom;

	// Fill Window Properties //
	ZeroMemory(&message, sizeof(message));
	ZeroMemory(&windowClass, sizeof(windowClass));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(107));
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszMenuName = MAKEINTRESOURCE(107);
	windowClass.lpszClassName = L"DX11 Engine";

	RegisterClassEx(&windowClass);

	// Create Window //
	gWindow = CreateWindow(L"DX11 Engine", L"DX11 Engine", WINDOW_STYLE,
						   WINDOW_START_X, WINDOW_START_Y, WINDOW_WIDTH , WINDOW_HEIGHT, 
						   NULL, NULL, hInstance, NULL);

	if (!gWindow) // If creation of window fails
		return false;

	ShowWindow(gWindow, cmdShow);
	UpdateWindow(gWindow);

	// Intialize Global Classes //
	CTime cTime;

	// Seed Random //
	srand((uint)(CURRENT_TIME));

	// - Main Loop - //
	Game game;
	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			// Game Update //
			TICK;
			game.Update();
		}
	}

	#if FAST_SHUTDOWN
		exit(3);
	#else
		return 0;
	#endif
}

LONG WINAPI CrashDumpToFile(_EXCEPTION_POINTERS* pExceptionInfo)
{
	HANDLE hFile = ::CreateFile(L"dumpfile.mdmp", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExceptionInfo;
		ExInfo.ClientPointers = NULL;

		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);

		::CloseHandle(hFile);
	}

	return 0;
}