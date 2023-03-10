#pragma once

#include "windows.h"

void LogIs(int show, HWND tag, const TCHAR* msg, ...);

void LogIs(int show, const TCHAR* msg, ...);

void LogIs(const TCHAR* msg, ...);

#if UNICODE
void LogIs(int show, HWND tag, const CHAR* msg, ...);

void LogIs(int show, const CHAR* msg, ...);

void LogIs(const CHAR* msg, ...);
#endif

#define LogTo(LABEL, ...) LogIs(1|0x20, ""#LABEL"", ##__VA_ARGS__);

#define xLogTo(LABEL, ...) LogIs(2|0x20, ""#LABEL"", ##__VA_ARGS__);

#define pLogTo(msg) LogIs(1|0x10|0x20, msg);

#define xpLogTo(msg) LogIs(2|0x10|0x20, msg);


#define LogToW(LABEL, ...) LogIs(1|0x20, L""#LABEL"", ##__VA_ARGS__);

#define xLogToW(LABEL, ...) LogIs(2|0x20, L""#LABEL"", ##__VA_ARGS__);


