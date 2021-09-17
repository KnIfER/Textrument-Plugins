
#include "InsituDebug.h"
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <strsafe.h>

HWND mainAppWnd;

void LogIs(int show, HWND hWnd, const TCHAR* msg, va_list & args)
{
	if (hWnd==(HWND)-2)
	{
		return;
	}
	if(show)
	{
		if (hWnd==(HWND)-1)
		{
			hWnd = mainAppWnd;
		}

		TCHAR buffer[512]={};
		// https://docs.microsoft.com/en-us/windows/win32/api/strsafe/nf-strsafe-stringcchvprintfw
		int ret = StringCchVPrintfW(buffer, 512, msg, args);
		if (SUCCEEDED(ret))
		{
			bool bNeedLF=buffer[ret-1]!='\n';
			if (show==1)
			{
				::OutputDebugString(buffer);
				::OutputDebugStringA("\n");
			}
			else if (show==2)
			{
				::MessageBox(hWnd, buffer, TEXT(""), MB_OK);
			}
			else if (show==3)
			{
				::OutputDebugString(buffer);
				::OutputDebugStringA("\n");
			}
		}
	}
}

void LogIs(int show, HWND hWnd, const TCHAR* msg, ...)
{
	va_list args = NULL;
	va_start(args, msg);
	LogIs(show, hWnd, msg, args);
}

void LogIs(int show, const TCHAR* msg, ...)
{
	va_list args = NULL;
	va_start(args, msg);
	LogIs(show, (HWND)-1, msg, args);
}

void LogIs(const TCHAR* msg, ...)
{
	va_list args = NULL;
	va_start(args, msg);
	LogIs(1, (HWND)-1, msg, args);
}

#if UNICODE

void LogIs(int show, HWND hWnd, const CHAR* msg, va_list & args)
{
	if (!msg)
	{
		return;
	}
	if (hWnd==(HWND)-2)
	{
		return;
	}
	if(show)
	{
		if (hWnd==(HWND)-1)
		{
			hWnd = mainAppWnd;
		}

		CHAR buffer[512]={};
		// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/vsprintf-s-vsprintf-s-l-vswprintf-s-vswprintf-s-l?view=msvc-160
		// vsprintf_s and vswprintf_s return the number of characters written, 
		//  not including the terminating null character, 
		//  or a negative value if an output error occurs. If
		int ret=vsprintf_s(buffer, msg, args);
		if (ret>0)
		{
			bool bNeedLF=buffer[ret-1]!='\n';
			if (show==1)
			{
				::OutputDebugStringA(buffer);
				if (bNeedLF)
					::OutputDebugStringA("\n");
			}
			else if (show==2)
			{
				::MessageBoxA(hWnd, buffer, (""), MB_OK);
			}
			else if (show==3)
			{
				::OutputDebugStringA(buffer);
				if (bNeedLF)
					::OutputDebugStringA("\n");
			}
		}

	}
}

void LogIs(int show, HWND hWnd, const CHAR* msg, ...)
{
	va_list args = NULL;
	va_start(args, msg);
	LogIs(show, hWnd, msg, args);
}

void LogIs(int show, const CHAR* msg, ...)
{
	va_list args = NULL;
	va_start(args, msg);
	LogIs(show, (HWND)-1, msg, args);
}

void LogIs(const CHAR* msg, ...)
{
	va_list args = NULL;
	va_start(args, msg);
	LogIs(1, (HWND)-1, msg, args);
}
#endif