/*
This file is part of MultiClipboard Plugin for Notepad++
Copyright (C) 2009 LoonyChewy

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#ifndef MULTI_CLIPBOARD_STRING_TYPE
#define MULTI_CLIPBOARD_STRING_TYPE

#ifndef UNITY_BUILD_SINGLE_INCLUDE
#include <string>
#endif

#ifdef UNICODE
	typedef std::wstring stringType;
	typedef wchar_t TCHAR;
#else
	typedef std::string stringType;
	typedef char TCHAR;
#endif //UNICODE


#ifndef TEXT
	#ifdef UNICODE
		#define __TEXT(quote) L##quote
	#else
		#define __TEXT(quote) ##quote
	#endif //UNICODE
	#define TEXT(quote) __TEXT(quote)
#endif	// TEXT


#endif	// MULTI_CLIPBOARD_STRING_TYPE