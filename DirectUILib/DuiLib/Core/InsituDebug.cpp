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

#define defStackLevel 16

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

inline void printStackTrace(){
	//dbg::dostream as;
	using namespace backward;
	StackTrace st; st.load_here(defStackLevel);
	st.skip_n_firsts(3);
	Printer p;
	std::stringstream stream;
	stream.clear();
	p.snippet  = false;
	//p.color_mode  = backward::ColorMode::always;
	p.reverse = false;
	p.print(st, stream);

	auto tmp = stream.str();
	auto c_str = tmp.c_str();
	::OutputDebugStringA(c_str);
	::OutputDebugStringA("\n");
	//cout << c_str << endl;
}

// flag show :: 0x1 -- log to output. 0x2 -- toast message window. 0x4 -- print stacktrace. 0x10 -- print msg directly. 0x20 -- simlplify format
void LogIs(int show, HWND hWnd, const TCHAR* msg, va_list & args)
{
	if (!msg || hWnd==(HWND)-2 || !show) return;
	if (hWnd==(HWND)-1) hWnd = mainAppWnd;
	TCHAR buffer[512]={};
	TCHAR* pBuffer = buffer;
	QkString tmp;
	int ret;
	if(show&0x10) {
		pBuffer = (TCHAR*)msg;
		ret = lstrlen(pBuffer);
	} else {
		if(show&0x20) {
			QkString tmp = msg;
			tmp.Replace(L"ddl", L"%ld");
			tmp.Replace(L"dd", L"%d");
			tmp.Replace(L"ff2", L"%.2f");
			tmp.Replace(L"ff1", L"%.1f");
			tmp.Replace(L"ff", L"%f");
			tmp.Replace(L"rcrc", L"%d %d %d %d");
			tmp.Replace(L"ss", L"%s");
			msg = (const TCHAR*)tmp.GetData();
		}
		// https://docs.microsoft.com/en-us/windows/win32/api/strsafe/nf-strsafe-stringcchvprintfw
		ret = StringCchVPrintfW(buffer, 512, msg, args); 
	}
	if (SUCCEEDED(ret))
	{
		bool bNeedLF=pBuffer[ret-1]!='\n';
		if (show&0x1)
		{
			::OutputDebugString(pBuffer);
			if (bNeedLF)
				::OutputDebugStringA("\n");
			cout << pBuffer << endl;
		}
		if (show&0x2)
			::MessageBox(hWnd, pBuffer, (L""), MB_OK);
		if(show&0x4) 
			printStackTrace();
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

// flag show :: 0x1 -- log to output. 0x2 -- toast message window. 0x4 -- print stacktrace. 0x10 -- print msg directly. 0x20 -- simlplify format
void LogIs(int show, HWND hWnd, const CHAR* msg, va_list & args)
{
	if (!msg || hWnd==(HWND)-2 || !show) return;
	if (hWnd==(HWND)-1) hWnd = mainAppWnd;
	CHAR buffer[512]={0};
	CHAR* pBuffer = buffer;
	int ret;
	string buffer1;
	if(show&0x10) {
		pBuffer = (CHAR*)msg;
		ret = strlen(pBuffer);
	} else {
		if(show&0x20) {
			QkString tmp = msg;
			tmp.Replace(L"ddl", L"%ld");
			tmp.Replace(L"dd", L"%d");
			tmp.Replace(L"ff2", L"%.2f");
			tmp.Replace(L"ff1", L"%.1f");
			tmp.Replace(L"ff", L"%f");
			tmp.Replace(L"rcrc", L"%d %d %d %d");
			tmp.Replace(L"ss", L"%s");
			msg = (const CHAR*)tmp.GetData(buffer1);
		}
		// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/vsprintf-s-vsprintf-s-l-vswprintf-s-vswprintf-s-l?view=msvc-160
		ret = vsprintf_s(buffer, msg, args);  // negative if an output error occurs.
	}
	if (ret>0)
	{
		bool bNeedLF=pBuffer[ret-1]!='\n';
		if (show&0x1)
		{
			::OutputDebugStringA(pBuffer);
			if (bNeedLF)
				::OutputDebugStringA("\n");
			cout << pBuffer << endl;
		}
		if (show&0x2)
			::MessageBoxA(hWnd, pBuffer, (""), MB_OK);
		if(show&0x4) 
			printStackTrace();
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