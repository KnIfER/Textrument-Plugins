#include "stdafx.h"
#include "InsituDebug.h"
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <strsafe.h>

#undef min
#undef max
#include "Utils/backward.hpp"
#include "Utils/basic_debugbuf.h"

HWND mainAppWnd;


namespace dbg {
	dostream cout;
	wdostream wcout;
	namespace {
		template <class CharT, class TraitsT = std::char_traits<CharT> >  // https://gist.github.com/comargo/f7fe244e539325cef0f66704b58dc35a
		class basic_debugbuf :
			public std::basic_stringbuf<CharT, TraitsT>
		{
		public:
			virtual ~basic_debugbuf(void)
			{
				sync();
			}
		protected:
			int sync()
			{
				output_debug_string(str().c_str());
				str(std::basic_string<CharT>()); // Clear the string buffer

				return 0;
			}
			void output_debug_string(const CharT *text) {}
		};
		template<>
		void basic_debugbuf<char>::output_debug_string(const char *text)
		{
			//::OutputDebugStringA("\n\n123213\n\n");
			::OutputDebugStringA(text);
		}
		template<>
		void basic_debugbuf<wchar_t>::output_debug_string(const wchar_t *text)
		{
			//::OutputDebugStringA("\n\n567567\n\n");
			::OutputDebugStringW(text);
		}
	}
	template<class CharT, class TraitsT>
	basic_dostream<CharT, TraitsT>::basic_dostream() 
		: std::basic_ostream<CharT, TraitsT>(new basic_debugbuf<CharT, TraitsT>()) 
	{
	}
	template<class CharT, class TraitsT>
	basic_dostream<CharT, TraitsT>::~basic_dostream() 
	{
		delete rdbuf(); 
	}
}


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
		int ret = StringCchVPrintfW(buffer, 512, msg, args); // https://docs.microsoft.com/en-us/windows/win32/api/strsafe/nf-strsafe-stringcchvprintfw
		if (SUCCEEDED(ret))
		{
			bool bNeedLF=buffer[ret-1]!='\n';
			if (show==1 || show==4)
			{
				::OutputDebugString(buffer);
				if(show==4) 
				{
					//dbg::dostream as;
					using namespace backward;
					StackTrace st; st.load_here(8);
					st.skip_n_firsts(2);
					Printer p;
					std::stringstream stream;
					stream.clear();
					p.snippet  = false;
					//p.color_mode  = backward::ColorMode::always;
					p.reverse = false;
					p.print(st, stream);

					::OutputDebugStringA(stream.str().c_str());
				}
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
	::OutputDebugString(L"LogIs\n");
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

		CHAR buffer[512]={0};
		// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/vsprintf-s-vsprintf-s-l-vswprintf-s-vswprintf-s-l?view=msvc-160
		// vsprintf_s and vswprintf_s return the number of characters written, 
		//  not including the terminating null character, 
		//  or a negative value if an output error occurs. If
		int ret=vsprintf_s(buffer, msg, args);
		if (ret>0)
		{
			bool bNeedLF=buffer[ret-1]!='\n';

			if (show==1 || show==4)
			{
				::OutputDebugStringA(buffer);
				if (bNeedLF)
					::OutputDebugStringA("\n");
				if(show==4) {
					using namespace backward;
					StackTrace st; st.load_here(32);
					Printer p; p.print(st);
				}
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