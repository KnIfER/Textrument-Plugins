#ifndef __UTILS_H__
#define __UTILS_H__

#pragma once
#include "OAIdl.h"

namespace DuiLib
{
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API STRINGorID
	{
	public:
		STRINGorID(LPCTSTR lpString) : m_lpstr(lpString)
		{ }
		STRINGorID(UINT nID) : m_lpstr(MAKEINTRESOURCE(nID))
		{ }
		LPCTSTR m_lpstr;
		size_t dataLen=0;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiPoint : public tagPOINT
	{
	public:
		CDuiPoint();
		CDuiPoint(const POINT& src);
		CDuiPoint(int x, int y);
		CDuiPoint(LPARAM lParam);
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiSize : public tagSIZE
	{
	public:
		CDuiSize();
		CDuiSize(const SIZE& src);
		CDuiSize(const RECT rc);
		CDuiSize(int cx, int cy);
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiRect : public tagRECT
	{
	public:
		CDuiRect();
		CDuiRect(const RECT& src);
		CDuiRect(int iLeft, int iTop, int iRight, int iBottom);

		int GetWidth() const;
		int GetHeight() const;
		void Empty();
		bool IsNull() const;
		void Join(const RECT& rc);
		void ResetOffset();
		void Normalize();
		void Offset(int cx, int cy);
		void Inflate(int cx, int cy);
		void Deflate(int cx, int cy);
		void Union(CDuiRect& rc);
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CStdPtrArray
	{
	public:
		CStdPtrArray(int iPreallocSize = 0);
		CStdPtrArray(const CStdPtrArray& src);
		~CStdPtrArray();

		void Empty();
		void Resize(int iSize);
		bool IsEmpty() const;
		int Find(LPVOID pData) const;
		int FindEx(LPVOID pData, int findSt=0, int findCnt=-1, bool reverse=false) const;
		bool Add(LPVOID pData);
		bool SetAt(int iIndex, LPVOID pData);
		bool InsertAt(int iIndex, LPVOID pData);
		bool Remove(int iIndex);
		LPVOID RemoveAt(int iIndex);
		int GetSize() const;
		LPVOID* GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPVOID* m_ppVoid;
		int m_nCount;
		int m_nAllocated;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CStdValArray
	{
	public:
		CStdValArray(int iElementSize, int iPreallocSize = 0);
		~CStdValArray();

		void Empty();
		bool IsEmpty() const;
		bool Add(LPCVOID pData);
		bool Remove(int iIndex);
		int GetSize() const;
		LPVOID GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPBYTE m_pVoid;
		int m_iElementSize;
		int m_nCount;
		int m_nAllocated;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API QkString
	{
	public:
		enum { MAX_LOCAL_STRING_LEN = 15 };

		QkString();
		QkString(const TCHAR ch);
		QkString(const CHAR* ch);
		QkString(const QkString& src);
		QkString(LPCTSTR lpsz, int nLen = -1);
		~QkString();

		static QkString & EmptyInstance(LPCTSTR lpstrId=0)
		{
			if (!lpstrId)
			{
				if (_EmptyInstance.GetLength()>0)
				{
					// 放空 ： 释放栈区数据，保存类结构中的本地数据。
					_EmptyInstance.Empty();
				}
			}
			else
			{
				_EmptyInstance = lpstrId;
			}
			return _EmptyInstance;
		};
		void Empty();
		size_t GetLength() const;
		bool IsEmpty() const;
		TCHAR GetAt(int nIndex) const;

		bool EnsureCapacity(size_t newSz, bool release=false);
		void AsBuffer(bool isBuffer=true){
			_isBuffer = isBuffer;
		}
		size_t Capacity(){
			return m_pstr==m_szBuffer?MAX_LOCAL_STRING_LEN:_capacity;
		};
		size_t RecalcSize();
		void ReFit();

		void Prepend(QkString & other) {
			Prepend(other, other.GetLength());
		};
		void Prepend(LPCTSTR pstr, int nLength = -1);
		void Append(QkString & other) {
			Append(other, other.GetLength());
		};
		void Append(LPCTSTR pstr, int nLength = -1);
		void Assign(QkString & other) {
			Assign(other, other.GetLength());
		};
		void Assign(LPCTSTR pstr, int nLength = -1);
		//QkString& AssignThenReturn(LPCTSTR pstr, int nLength = -1) {
		//	Assign(pstr, nLength);
		//	return *this;
		//};
		LPCTSTR GetData() const;
		LPCSTR GetData(std::string & buffer, int offset=0, int length=-1) const;
		static LPCSTR GetData(std::string & buffer, LPCWSTR pStr);

		void SetAt(int nIndex, TCHAR ch);
		operator LPCTSTR() const;

		TCHAR operator[] (int nIndex) const;
		const QkString& operator=(const QkString& src);
		const QkString& operator=(const TCHAR ch);
		const QkString& operator=(LPCTSTR pstr);
#ifdef _UNICODE
		const QkString& operator=(LPCSTR lpStr);
		const QkString& operator+=(LPCSTR lpStr);
#else
		const QkString& operator=(LPCWSTR lpwStr);
		const QkString& operator+=(LPCWSTR lpwStr);
#endif
		QkString operator+(const QkString& src) const;
		QkString operator+(LPCTSTR pstr) const;
		const QkString& operator+=(const QkString& src);
		const QkString& operator+=(LPCTSTR pstr);
		const QkString& operator+=(const TCHAR ch);

		bool operator == (LPCTSTR str) const;
		bool operator != (LPCTSTR str) const;
		bool operator == (const QkString & str) const;
		bool operator != (const QkString & str) const;
		bool operator <= (LPCTSTR str) const;
		bool operator <  (LPCTSTR str) const;
		bool operator >= (LPCTSTR str) const;
		bool operator >  (LPCTSTR str) const;

		int Compare(LPCTSTR pstr) const;
		int CompareNoCase(LPCTSTR pstr) const;

		void MakeUpper();
		void MakeLower();

		QkString Left(int nLength) const;
		QkString Mid(int iPos, int nLength = -1) const;
		void MidFast(int iPos, int nLength = -1);
		//QkString * MidFake(int iPos, int nLength = -1);
		QkString Right(int nLength) const;

		int Find(TCHAR ch, int iPos = 0) const;
		int Find(LPCTSTR pstr, int iPos = 0) const;
		int ReverseFind(TCHAR ch) const;
		int Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo);
		void Trim();
		void Split(const QkString & delim, std::vector<QkString> & ret);

		bool StartWith(const QkString & prefix, bool ignoreCase=false, int toffset=0) const;
		bool EndWith(const QkString & other, bool ignoreCase=false) const;

		int __cdecl Format(LPCTSTR pstrFormat, ...);
		int __cdecl SmallFormat(LPCTSTR pstrFormat, ...);

	protected:
		int __cdecl InnerFormat(LPCTSTR pstrFormat, va_list Args);

	protected:
		LPTSTR m_pstr;
		TCHAR m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
		size_t _size;
		size_t _capacity;
		bool _isBuffer;
		static QkString _EmptyInstance;
	};

	static std::vector<QkString> StrSplit(QkString text, QkString sp)
	{
		std::vector<QkString> vResults;
		int pos = text.Find(sp, 0);
		while (pos >= 0)
		{
			QkString t = text.Left(pos);
			vResults.push_back(t);
			text = text.Right(text.GetLength() - pos - sp.GetLength());
			pos = text.Find(sp);
		}
		vResults.push_back(text);
		return vResults;
    }
	/////////////////////////////////////////////////////////////////////////////////////
	//

	struct TITEM
	{
		QkString Key;
		LPVOID Data;
		struct TITEM* pPrev;
		struct TITEM* pNext;
	};

	class UILIB_API QkStringPtrMap
	{
	public:
		QkStringPtrMap(int nSize = 83);
		~QkStringPtrMap();

		void Resize(int nSize = 83);
		LPVOID Find(LPCTSTR key, bool optimize = true) const;
		bool Insert(const QkString & key, LPVOID pData);
		bool Insert(LPCTSTR key, LPVOID pData);
		LPVOID Set(LPCTSTR key, LPVOID pData);
		bool Remove(LPCTSTR key);
		void RemoveAll();
		int GetSize() const;
		const TITEM* GetSlotAt(int iIndex) const;
		LPCTSTR GetKeyAt(int iIndex) const;
		LPVOID GetValueAt(int iIndex) const;
		bool GetKeyValueAt(int iIndex, LPCTSTR & key, LPVOID & value) const;
		LPCTSTR operator[] (int nIndex) const;

	protected:
		LPVOID FindIntern(UINT hash, const QkString & key, bool optimize = true) const;
		TITEM** m_aT;
		int m_nBuckets;
		int m_nCount;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CWaitCursor
	{
	public:
		CWaitCursor();
		~CWaitCursor();

	protected:
		HCURSOR m_hOrigCursor;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CDuiVariant : public VARIANT
	{
	public:
		CDuiVariant() 
		{ 
			VariantInit(this); 
		}
		CDuiVariant(int i)
		{
			VariantInit(this);
			this->vt = VT_I4;
			this->intVal = i;
		}
		CDuiVariant(float f)
		{
			VariantInit(this);
			this->vt = VT_R4;
			this->fltVal = f;
		}
		CDuiVariant(LPOLESTR s)
		{
			VariantInit(this);
			this->vt = VT_BSTR;
			this->bstrVal = s;
		}
		CDuiVariant(IDispatch *disp)
		{
			VariantInit(this);
			this->vt = VT_DISPATCH;
			this->pdispVal = disp;
		}

		~CDuiVariant() 
		{ 
			VariantClear(this); 
		}
	};

	//////////////////////////////////////////////////////////////////////////////////////
	//
	static char* w2a(wchar_t* lpszSrc, UINT   CodePage = CP_ACP)
	{
		if (lpszSrc != NULL)
		{
			int  nANSILen = WideCharToMultiByte(CodePage, 0, lpszSrc, -1, NULL, 0, NULL, NULL);
			char* pANSI = new char[nANSILen + 1];
			if (pANSI != NULL)
			{
				ZeroMemory(pANSI, nANSILen + 1);
				WideCharToMultiByte(CodePage, 0, lpszSrc, -1, pANSI, nANSILen, NULL, NULL);
				return pANSI;
			}
		}	
		return NULL;
	}

	static wchar_t* a2w(char* lpszSrc, UINT   CodePage = CP_ACP)
	{
		if (lpszSrc != NULL)
		{
			int nUnicodeLen = MultiByteToWideChar(CodePage, 0, lpszSrc, -1, NULL, 0);
			LPWSTR pUnicode = new WCHAR[nUnicodeLen + 1];
			if (pUnicode != NULL)
			{
				ZeroMemory((void*)pUnicode, (nUnicodeLen + 1) * sizeof(WCHAR));
				MultiByteToWideChar(CodePage, 0, lpszSrc,-1, pUnicode, nUnicodeLen);
				return pUnicode;
			}
		}
		return NULL;
	}

	///////////////////////////////////////////////////////////////////////////////////////
	////
	//struct TImageInfo;
	//class CPaintManagerUI;
	//class UILIB_API CImageString
	//{
	//public:
	//	CImageString();
	//	CImageString(const CImageString&);
	//	const CImageString& operator=(const CImageString&);
	//	virtual ~CImageString();

	//	const QkString& GetAttributeString() const;
	//	void SetAttributeString(LPCTSTR pStrImageAttri);
	//	void ModifyAttribute(LPCTSTR pStrModify);
	//	bool LoadImage(CPaintManagerUI* pManager);
	//	bool IsLoadSuccess();

	//	RECT GetDest() const;
	//	void SetDest(const RECT &rcDest);
	//	const TImageInfo* GetImageInfo() const;

	//private:
	//	void Clone(const CImageString&);
	//	void Clear();
	//	void ParseAttribute(LPCTSTR pStrImageAttri);

	//protected:
	//	friend class CRenderEngine;
	//	QkString	m_sImageAttribute;

	//	QkString	m_sImage;
	//	QkString	m_sResType;
	//	TImageInfo	*m_imageInfo;
	//	bool		m_bLoadSuccess;

	//	RECT	m_rcDest;
	//	RECT	m_rcSource;
	//	RECT	m_rcCorner;
	//	BYTE	m_bFade;
	//	DWORD	m_dwMask;
	//	bool	m_bHole;
	//	bool	m_bTiledX;
	//	bool	m_bTiledY;
	//};

	// 转换十六进制字符串如#FFF,#FFFFFF,#FFFFFFFF,0xFFF,0xFFFFFF,0xFFFFFFFF 为 ARGB 数值。
	LPCTSTR STR2ARGB(LPCTSTR STR, DWORD & ARGB);

	// 转换十进制字符串为整型数值。
	int ParseInt(LPCTSTR STR);

	// 转换十进制字符串为整型数值。
	LPCTSTR STR2Decimal(LPCTSTR STR, long & value);

	// 转换十进制字符串如“整数,整数,整数,整数”,“整数,整数”为矩形。
	LPCTSTR STR2Rect(LPCTSTR STR, RECT & rc);

	// 转换十进制字符串如“整数,整数”,“整数”为SIZE。
	LPCTSTR STR2Size(LPCTSTR STR, SIZE & sz);

	void SetRectInt(RECT & rc, int value);

#define GRAVITY_LEFT 0x1
#define GRAVITY_RIGHT 0x2
#define GRAVITY_HCENTER 0x3
#define GRAVITY_TOP 0x4
#define GRAVITY_BOTTOM 0x8
#define GRAVITY_VCENTER 0xC
#define GRAVITY_CENTER 0xF

#define LONG_FLAG __int64
#define STR(QKSTR) ((LPCWSTR)QKSTR)
#define STRW(QKSTR) ((LPCWSTR)QkString(QKSTR))
#define PRINT_RECT(LABEL, rc, way) LogIs(way, ""#LABEL" (%d, %d, %d, %d)", rc.left, rc.top, rc.right, rc.bottom);

#define DrawImageAt(hdc, str, dest) DrawImage(hdc, str, dest)


#define imgNormal 3
#define imgHot 4
#define imgPushed 5
#define imgDisabled 6
#define imgFocused 7
#define imgPushedFore 8
#define imgHotFore 9

}// namespace DuiLib

#endif // __UTILS_H__