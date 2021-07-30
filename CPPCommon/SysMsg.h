//this file is part of notepad++
//Copyright (C)2003 Don HO ( donho@altern.org )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef M30_IDE_COMMUN_H
#define M30_IDE_COMMUN_H

#include <windows.h>
#include <string>
#include <vector>
#include <time.h>

void systemMessage(const TCHAR *title);
//DWORD ShortToLongPathName(LPCTSTR lpszShortPath, LPTSTR lpszLongPath, DWORD cchBuffer);
void printInt(int int2print);
void printStr(const TCHAR *str2print);
void writeLog(const TCHAR *logFileName, const TCHAR *log2write);
int filter(unsigned int code, struct _EXCEPTION_POINTERS *ep);
int getCpFromStringValue(const TCHAR * encodingStr);
//std::basic_string<TCHAR> purgeMenuItemString(const TCHAR * menuItemStr, bool keepAmpersand = false);

#define CP_ANSI_LATIN_1 1252
#define CP_BIG5 950

#ifdef UNICODE
	#define NppMainEntry wWinMain
	#define generic_strtol wcstol
	#define generic_strncpy wcsncpy
	#define generic_stricmp wcsicmp
	#define generic_strncmp wcsncmp
	#define generic_strnicmp wcsnicmp
	#define generic_strncat wcsncat
	#define generic_strchr wcschr
	#define generic_atoi _wtoi
	#define generic_atof _wtof
	#define generic_strtok wcstok
	#define generic_strftime wcsftime
	#define generic_fprintf fwprintf
	#define generic_sscanf swscanf
	#define generic_fopen _wfopen
	#define generic_fgets fgetws
	#define generic_stat _wstat
	#define COPYDATA_FILENAMES COPYDATA_FILENAMESW
#else
	#define NppMainEntry WinMain
	#define generic_strtol strtol
	#define generic_strncpy strncpy
	#define generic_stricmp stricmp
	#define generic_strncmp strncmp
	#define generic_strnicmp strnicmp
	#define generic_strncat strncat
	#define generic_strchr strchr
	#define generic_atoi atoi
	#define generic_atof atof
	#define generic_strtok strtok
	#define generic_strftime strftime
	#define generic_fprintf fprintf
	#define generic_sscanf sscanf
	#define generic_fopen fopen
	#define generic_fgets fgets
	#define generic_stat _stat
	#define COPYDATA_FILENAMES COPYDATA_FILENAMESA
#endif

//void char2wchar(const char* pszCHAR, wchar_t* pszWCHAR, UINT codepage);
//void wchar2char(const wchar_t* pszWCHAR, char* pszCHAR, UINT codepage);
std::wstring string2wstring(const std::string & rString, UINT codepage);
std::string wstring2string(const std::wstring & rwString, UINT codepage);



#endif //M30_IDE_COMMUN_H
