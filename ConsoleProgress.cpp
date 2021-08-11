#include "stdafx.h"
#include <shobjidl.h>


#define AppName "ConsoleProgress"

bool ParseState(_In_ const wchar_t* string, TBPFLAG& outState);
bool ParseProgress(_In_ const wchar_t* string, ULONGLONG& outProgress);


int _tmain(int argc, wchar_t* argv[])
{
	// Check arguments.
	bool changeState = false;
	TBPFLAG state = TBPF_NORMAL;
	bool changeProgress = false;
	bool flashWindow = false;
	ULONGLONG progress = 0;
	if (argc == 2 && _wcsicmp(argv[1], L"reset") == 0)
	{
		state = TBPF_NOPROGRESS;
		changeState = true;
	}
	else if (argc == 2 && _wcsicmp(argv[1], L"indeterminate") == 0)
	{
		state = TBPF_INDETERMINATE;
		changeState = true;
	}
	else if (argc == 2 && _wcsicmp(argv[1], L"flash") == 0)
	{
		flashWindow = true;
	}
	else if (argc == 2)
	{
		if (ParseState(argv[1], state))
			changeState = true;
		else if (ParseProgress(argv[1], progress))
			changeProgress = true;
	}
	else if (argc == 3)
	{
		if (ParseState(argv[1], state) && ParseProgress(argv[2], progress))
		{
			changeState = true;
			changeProgress = true;
		}
	}

	// Show usage information if the arguments were invalid.
	if (!changeState && !changeProgress && !flashWindow)
	{
		std::cout <<
			AppName " (c) 2014-2021 Travis Spomer\n"
			"Changes the current console window's taskbar progress indicator.\n\n"
			"Usage:\n"
			"\t" AppName " [ reset | indeterminate | flash | [ normal | warning | error ] [ 0-100 ] ]\n\n"
			"Usage examples:\n\n"
			"\t" AppName " reset         - returns the console to normal\n"
			"\t" AppName " indeterminate - sets the state to indeterminate\n"
			"\t" AppName " normal 25     - sets the state to normal and the progress to 25%\n"
			"\t" AppName " warning       - sets the state to warning without changing the progress\n"
			"\t" AppName " 100           - sets the progress to 100% without changing the state\n"
			"\t" AppName " flash         - flash to get the user's attention\n\n"
			"(Has no effect in Windows Terminal or other virtual terminals.)\n\n";
	}

	// What's the HWND of our console?
	HWND hwndConsole = GetConsoleWindow();
	if (hwndConsole == 0)
	{
		// IMPORTANT: This will succeed for virtual terminals such as the one used in Windows Terminal, but it will
		// return a hidden HWND, not the real UI HWND.
		// Going forward, Windows Terminal / virtual terminals won't support any of this, and we'd instead have to
		// use console control sequences to achieve the same effects. See:
		// https://github.com/microsoft/terminal/issues/3004
		std::cerr << "Failed to figure out what console we're in.  :(";
		return 1;
	}

	// Try to change its status.
	if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
	{
		std::cerr << "y u no com??";
		return 1;
	}
	Microsoft::WRL::ComPtr<ITaskbarList3> spTaskbarList;
	HRESULT hr = CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, &spTaskbarList);
	if (FAILED(hr))
	{
		std::cerr << "Failed to initialize ITaskbarList3.  :(";
		return 1;
	}

	if (changeState)
		spTaskbarList->SetProgressState(hwndConsole, state);
	if (changeProgress)
		spTaskbarList->SetProgressValue(hwndConsole, progress, 100);
	if (flashWindow)
		FlashWindow(hwndConsole, /* bInvert: */ true);

	// All done!
	return 0;
}


// Parses an input string, and returns true and changes outState if it is a valid state.
bool ParseState(_In_ const wchar_t* string, TBPFLAG& outState)
{
	if (string[0] == L'-' || string[0] == L'/')
		string++;
	if (_wcsicmp(string, L"normal") == 0)
	{
		outState = TBPF_NORMAL;
		return true;
	}
	if (_wcsicmp(string, L"warning") == 0)
	{
		outState = TBPF_PAUSED;
		return true;
	}
	if (_wcsicmp(string, L"error") == 0)
	{
		outState = TBPF_ERROR;
		return true;
	}
	return false;
}


// Parses an input string, and returns true and changes outProgress if it is a valid integer 0-100.
bool ParseProgress(_In_ const wchar_t* string, ULONGLONG& outProgress)
{
	std::wistringstream input(string);
	ULONGLONG parsed = 0;
	if (input >> parsed && parsed >= 0 && parsed <= 100)
	{
		outProgress = parsed;
		return true;
	}
	return false;
}
