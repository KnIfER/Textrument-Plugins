// SciTE - Scintilla based Text Editor
/** @file StringHelpers.h
 ** Definition of widely useful string functions.
 **/
// Copyright 2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef STRINGHELPERS_H
#define STRINGHELPERS_H

bool StartsWith(std::wstring_view s, std::wstring_view start);
bool StartsWith(std::string_view s, std::string_view start);
bool EndsWith(std::wstring_view s, std::wstring_view end);
bool Contains(std::string const &s, char ch) noexcept;

// Substitute is duplicated instead of templated as it was ambiguous when implemented as a template.
int Substitute(std::wstring &s, std::wstring_view sFind, std::wstring_view sReplace);
int Substitute(std::string &s, std::string_view sFind, std::string_view sReplace);

template <typename T>
int Remove(T &s, const T &sFind) {
	return Substitute(s, sFind, T());
}

bool RemoveStringOnce(std::string &s, const char *marker);

std::string StdStringFromInteger(int i);
std::string StdStringFromSizeT(size_t i);
std::string StdStringFromDouble(double d, int precision);

int IntegerFromString(const std::string &val, int defaultValue);
intptr_t IntPtrFromString(const std::string &val, intptr_t defaultValue);
long long LongLongFromString(const std::string &val, long long defaultValue);

// Basic case lowering that converts A-Z to a-z.
// Does not handle non-ASCII characters.
void LowerCaseAZ(std::string &s);

constexpr char MakeUpperCase(char ch) noexcept {
	if (ch < 'a' || ch > 'z')
		return ch;
	else
		return ch - 'a' + 'A';
}

constexpr char MakeLowerCase(char c) noexcept {
	if (c >= 'A' && c <= 'Z') {
		return c - 'A' + 'a';
	} else {
		return c;
	}
}

constexpr bool IsASCII(int ch) noexcept {
	return (ch >= 0) && (ch < 0x80);
}

constexpr bool IsASpace(int ch) noexcept {
	return (ch == ' ') || ((ch >= 0x09) && (ch <= 0x0d));
}

constexpr bool IsADigit(int ch) noexcept {
	return (ch >= '0') && (ch <= '9');
}

constexpr bool IsAlphabetic(int ch) noexcept {
	return ((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z'));
}

constexpr bool IsAlphaNumeric(int ch) noexcept {
	return
		((ch >= '0') && (ch <= '9')) ||
		((ch >= 'a') && (ch <= 'z')) ||
		((ch >= 'A') && (ch <= 'Z'));
}

intptr_t IntegerFromText(const char *s) noexcept;

// StringSplit can be expanded over std::string or GUI::gui_string
template <typename T>
std::vector<T> StringSplit(const T &text, int separator) {
	std::vector<T> vs(text.empty() ? 0 : 1);
	for (typename T::const_iterator it=text.begin(); it!=text.end(); ++it) {
		if (*it == separator) {
			vs.push_back(T());
		} else {
			vs.back() += *it;
		}
	}
	return vs;
}

inline std::vector<GUI::gui_string> ListFromString(const GUI::gui_string &args) {
	return StringSplit(args, '\n');
}

typedef std::tuple<std::string_view, std::string_view> ViewPair;

// Split view around first separator returning the portion before and after the separator.
// If the separator is not present then return whole view and an empty view.
inline ViewPair ViewSplit(std::string_view view, char separator) noexcept {
	const size_t sepPos = view.find_first_of(separator);
	std::string_view first = view.substr(0, sepPos);
	std::string_view second = sepPos == (std::string_view::npos) ? "" : view.substr(sepPos + 1);
	return { first, second };
}

// Safer version of string copy functions like strcpy, wcsncpy, etc.
// Instantiate over fixed length strings of both char and wchar_t.
// May truncate if source doesn't fit into dest with room for NUL.

template <typename T, size_t count>
void StringCopy(T(&dest)[count], const T *source) noexcept {
	for (size_t i=0; i<count; i++) {
		dest[i] = source[i];
		if (!source[i])
			break;
	}
	dest[count-1] = 0;
}

int CompareNoCase(const char *a, const char *b) noexcept;
bool EqualCaseInsensitive(const char *a, const char *b) noexcept;
bool EqualCaseInsensitive(std::string_view a, std::string_view b) noexcept;
bool isprefix(const char *target, const char *prefix) noexcept;

constexpr const char *UTF8BOM = "\xef\xbb\xbf";

std::u32string UTF32FromUTF8(std::string_view s);
unsigned int UTF32Character(const char *utf8) noexcept;
std::string UTF8FromUTF32(unsigned int uch);

std::string Slash(const std::string &s, bool quoteQuotes);
unsigned int UnSlash(char *s) noexcept;
std::string UnSlashString(const char *s);
std::string UnSlashLowOctalString(const char *s);

unsigned int IntFromHexDigit(int ch) noexcept;
unsigned int IntFromHexBytes(std::string_view hexBytes) noexcept;
std::string UnicodeUnEscape(std::string_view s);

class ILocalize {
public:
	virtual GUI::gui_string Text(const char *s, bool retainIfNotFound=true) = 0;
};

/**
 * This is a fixed length list of strings suitable for display in combo boxes
 * as a memory of user entries.
 */
template < int sz >
class EntryMemory {
	std::string entries[sz];
public:
	void Insert(const std::string &s) {
		for (int i = 0; i < sz; i++) {
			if (entries[i] == s) {
				for (int j = i; j > 0; j--) {
					entries[j] = entries[j - 1];
				}
				entries[0] = s;
				return;
			}
		}
		for (int k = sz - 1; k > 0; k--) {
			entries[k] = entries[k - 1];
		}
		entries[0] = s;
	}
	void AppendIfNotPresent(const std::string &s) {
		for (int i = 0; i < sz; i++) {
			if (entries[i] == s) {
				return;
			}
			if (0 == entries[i].length()) {
				entries[i] = s;
				return;
			}
		}
	}
	void AppendList(const std::string &s, char sep = '|') {
		int start = 0;
		int end = 0;
		while (s[end] != '\0') {
			end = start;
			while ((s[end] != sep) && (s[end] != '\0'))
				++end;
			AppendIfNotPresent(s.substr(start, end-start));
			start = end + 1;
		}
	}
	int Length() const noexcept {
		int len = 0;
		for (int i = 0; i < sz; i++)
			if (entries[i].length())
				len++;
		return len;
	}
	std::string At(int n) const {
		return entries[n];
	}
	std::vector<std::string>AsVector() {
		std::vector<std::string> ret;
		for (int i = 0; i < sz; i++) {
			if (entries[i].length())
				ret.push_back(entries[i].c_str());
		}
		return ret;
	}
};

typedef EntryMemory < 10 > ComboMemory;

#endif
