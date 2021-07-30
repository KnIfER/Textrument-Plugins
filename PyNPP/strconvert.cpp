#include "Windows.h"
#include <string>
#include "strconvert.h"

/*
 * �� ascii ����� wstring ת��Ϊ utf8 ����� string
 */
std::string wstrtostrutf8(const std::wstring &wstr)
{
    // Convert a Unicode string to an ASCII string
	size_t strLen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
    std::string strTo;
    char *szTo = new char[strLen + 1];
    szTo[strLen] = '\0';
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, szTo, (int)strLen, NULL, NULL);
    strTo = szTo;
    delete[] szTo;
    return strTo;
}

/*
 * �� utf8 ����� string ת��Ϊ wstring
 * �����Ҫ�� utf8 ����� string ת��Ϊ���ر���� string
 * �ڵ���һ�� wstrtostr
 */
std::wstring strtowstrutf8(const std::string &str)
{
    // Convert an ASCII string to a Unicode String
    std::wstring wstrTo;
    wchar_t *wszTo = new wchar_t[str.length() + 1];
    wszTo[str.size()] = L'\0';
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wszTo, (int)str.length());
    wstrTo = wszTo;
    delete[] wszTo;
    return wstrTo;
}

std::string wstrtostr(const std::wstring &wstr)
{
    // Convert a Unicode string to an ASCII string
	size_t strLen = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
    std::string strTo;
    char *szTo = new char[strLen + 1];
    szTo[strLen] = '\0';
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTo, (int)strLen, NULL, NULL);
    strTo = szTo;
    delete[] szTo;
    return strTo;
}

std::wstring strtowstr(const std::string &str)
{
    // Convert an ASCII string to a Unicode String
    std::wstring wstrTo;
    wchar_t *wszTo = new wchar_t[str.length() + 1];
    wszTo[str.size()] = L'\0';
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wszTo, (int)str.length());
    wstrTo = wszTo;
    delete[] wszTo;
    return wstrTo;
}

