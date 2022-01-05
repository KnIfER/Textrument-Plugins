#include "StdAfx.h"
#include "Utils.h"

namespace DuiLib
{

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CDuiPoint::CDuiPoint()
	{
		x = y = 0;
	}

	CDuiPoint::CDuiPoint(const POINT& src)
	{
		x = src.x;
		y = src.y;
	}

	CDuiPoint::CDuiPoint(int _x, int _y)
	{
		x = _x;
		y = _y;
	}

	CDuiPoint::CDuiPoint(LPARAM lParam)
	{
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CDuiSize::CDuiSize()
	{
		cx = cy = 0;
	}

	CDuiSize::CDuiSize(const SIZE& src)
	{
		cx = src.cx;
		cy = src.cy;
	}

	CDuiSize::CDuiSize(const RECT rc)
	{
		cx = rc.right - rc.left;
		cy = rc.bottom - rc.top;
	}

	CDuiSize::CDuiSize(int _cx, int _cy)
	{
		cx = _cx;
		cy = _cy;
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CDuiRect::CDuiRect()
	{
		left = top = right = bottom = 0;
	}

	CDuiRect::CDuiRect(const RECT& src)
	{
		left = src.left;
		top = src.top;
		right = src.right;
		bottom = src.bottom;
	}

	CDuiRect::CDuiRect(int iLeft, int iTop, int iRight, int iBottom)
	{
		left = iLeft;
		top = iTop;
		right = iRight;
		bottom = iBottom;
	}

	int CDuiRect::GetWidth() const
	{
		return right - left;
	}

	int CDuiRect::GetHeight() const
	{
		return bottom - top;
	}

	void CDuiRect::Empty()
	{
		left = top = right = bottom = 0;
	}

	bool CDuiRect::IsNull() const
	{
		return (left == 0 && right == 0 && top == 0 && bottom == 0); 
	}

	void CDuiRect::Join(const RECT& rc)
	{
		if( rc.left < left ) left = rc.left;
		if( rc.top < top ) top = rc.top;
		if( rc.right > right ) right = rc.right;
		if( rc.bottom > bottom ) bottom = rc.bottom;
	}

	void CDuiRect::ResetOffset()
	{
		::OffsetRect(this, -left, -top);
	}

	void CDuiRect::Normalize()
	{
		if( left > right ) { int iTemp = left; left = right; right = iTemp; }
		if( top > bottom ) { int iTemp = top; top = bottom; bottom = iTemp; }
	}

	void CDuiRect::Offset(int cx, int cy)
	{
		::OffsetRect(this, cx, cy);
	}

	void CDuiRect::Inflate(int cx, int cy)
	{
		::InflateRect(this, cx, cy);
	}

	void CDuiRect::Deflate(int cx, int cy)
	{
		::InflateRect(this, -cx, -cy);
	}

	void CDuiRect::Union(CDuiRect& rc)
	{
		::UnionRect(this, this, &rc);
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CStdPtrArray::CStdPtrArray(int iPreallocSize) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(iPreallocSize)
	{
		ASSERT(iPreallocSize>=0);
		if( iPreallocSize > 0 ) m_ppVoid = static_cast<LPVOID*>(malloc(iPreallocSize * sizeof(LPVOID)));
	}

	CStdPtrArray::CStdPtrArray(const CStdPtrArray& src) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(0)
	{
		for(int i=0; i<src.GetSize(); i++)
			Add(src.GetAt(i));
	}

	CStdPtrArray::~CStdPtrArray()
	{
		if( m_ppVoid != NULL ) free(m_ppVoid);
	}

	void CStdPtrArray::Empty()
	{
		//if( m_ppVoid != NULL ) free(m_ppVoid);
		//m_ppVoid = NULL;
		//m_nCount = m_nAllocated = 0;
		::ZeroMemory(m_ppVoid, m_nCount * sizeof(LPVOID));
		m_nCount = 0;
	}

	void CStdPtrArray::Resize(int iSize)
	{
		Empty();
		//todo fix
		m_ppVoid = static_cast<LPVOID*>(malloc(iSize * sizeof(LPVOID)));
		::ZeroMemory(m_ppVoid, iSize * sizeof(LPVOID));
		m_nAllocated = iSize;
		m_nCount = iSize;
	}

	bool CStdPtrArray::IsEmpty() const
	{
		return m_nCount == 0;
	}

	bool CStdPtrArray::Add(LPVOID pData)
	{
		if( ++m_nCount >= m_nAllocated) {
			int nAllocated = m_nAllocated * 2;
			if( nAllocated == 0 ) nAllocated = 11;
			LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
			if( ppVoid != NULL ) {
				m_nAllocated = nAllocated;
				m_ppVoid = ppVoid;
			}
			else {
				--m_nCount;
				return false;
			}
		}
		m_ppVoid[m_nCount - 1] = pData;
		return true;
	}

	bool CStdPtrArray::InsertAt(int iIndex, LPVOID pData)
	{
		if( iIndex == m_nCount ) return Add(pData);
		if( iIndex < 0 || iIndex > m_nCount ) return false;
		if( ++m_nCount >= m_nAllocated) {
			int nAllocated = m_nAllocated * 2;
			if( nAllocated == 0 ) nAllocated = 11;
			LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
			if( ppVoid != NULL ) {
				m_nAllocated = nAllocated;
				m_ppVoid = ppVoid;
			}
			else {
				--m_nCount;
				return false;
			}
		}
		memmove(&m_ppVoid[iIndex + 1], &m_ppVoid[iIndex], (m_nCount - iIndex - 1) * sizeof(LPVOID));
		m_ppVoid[iIndex] = pData;
		return true;
	}

	bool CStdPtrArray::SetAt(int iIndex, LPVOID pData)
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return false;
		m_ppVoid[iIndex] = pData;
		return true;
	}

	bool CStdPtrArray::Remove(int iIndex)
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return false;
		if( iIndex < --m_nCount ) ::CopyMemory(m_ppVoid + iIndex, m_ppVoid + iIndex + 1, (m_nCount - iIndex) * sizeof(LPVOID));
		return true;
	}
	
	LPVOID CStdPtrArray::RemoveAt(int iIndex)
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
		LPVOID ret = m_ppVoid[iIndex];
		if( iIndex < --m_nCount ) ::CopyMemory(m_ppVoid + iIndex, m_ppVoid + iIndex + 1, (m_nCount - iIndex) * sizeof(LPVOID));
		return ret;
	}

	int CStdPtrArray::Find(LPVOID pData) const
	{
		for( int i = 0; i < m_nCount; i++ ) if( m_ppVoid[i] == pData ) return i;
		return -1;
	}

	int CStdPtrArray::FindEx(LPVOID pData, int findSt, int findCnt, bool reverse) const
	{
		int findEd = m_nCount;
		if (findSt<0)
			findSt = findEd + findSt;
		if (findSt<0 || findSt>m_nCount)
			return -1;
		if (reverse)
		{
			findEd = findCnt>0?max(findSt - findCnt, -1):-1;
			for( int i = findSt; i > findEd; i-- ) 
				if( m_ppVoid[i] == pData ) return i;
		}
		else
		{
			findEd = findCnt>0?min(findSt + findCnt, findEd):findEd;
			for( int i = findSt; i < findEd; i++ ) 
				if( m_ppVoid[i] == pData ) return i;
		}
		return -1;
	}

	int CStdPtrArray::GetSize() const
	{
		return m_nCount;
	}

	LPVOID* CStdPtrArray::GetData()
	{
		return m_ppVoid;
	}

	LPVOID CStdPtrArray::GetAt(int iIndex) const
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
		return m_ppVoid[iIndex];
	}

	LPVOID CStdPtrArray::operator[] (int iIndex) const
	{
		ASSERT(iIndex>=0 && iIndex<m_nCount);
		return m_ppVoid[iIndex];
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CStdValArray::CStdValArray(int iElementSize, int iPreallocSize /*= 0*/) : 
	m_pVoid(NULL), 
		m_nCount(0), 
		m_iElementSize(iElementSize), 
		m_nAllocated(iPreallocSize)
	{
		ASSERT(iElementSize>0);
		ASSERT(iPreallocSize>=0);
		if( iPreallocSize > 0 ) m_pVoid = static_cast<LPBYTE>(malloc(iPreallocSize * m_iElementSize));
	}

	CStdValArray::~CStdValArray()
	{
		if( m_pVoid != NULL ) free(m_pVoid);
	}

	void CStdValArray::Empty()
	{   
		m_nCount = 0;  // NOTE: We keep the memory in place
	}

	bool CStdValArray::IsEmpty() const
	{
		return m_nCount == 0;
	}

	bool CStdValArray::Add(LPCVOID pData)
	{
		if( ++m_nCount >= m_nAllocated) {
			int nAllocated = m_nAllocated * 2;
			if( nAllocated == 0 ) nAllocated = 11;
			LPBYTE pVoid = static_cast<LPBYTE>(realloc(m_pVoid, nAllocated * m_iElementSize));
			if( pVoid != NULL ) {
				m_nAllocated = nAllocated;
				m_pVoid = pVoid;
			}
			else {
				--m_nCount;
				return false;
			}
		}
		::CopyMemory(m_pVoid + ((m_nCount - 1) * m_iElementSize), pData, m_iElementSize);
		return true;
	}

	bool CStdValArray::Remove(int iIndex)
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return false;
		if( iIndex < --m_nCount ) ::CopyMemory(m_pVoid + (iIndex * m_iElementSize), m_pVoid + ((iIndex + 1) * m_iElementSize), (m_nCount - iIndex) * m_iElementSize);
		return true;
	}

	int CStdValArray::GetSize() const
	{
		return m_nCount;
	}

	LPVOID CStdValArray::GetData()
	{
		return static_cast<LPVOID>(m_pVoid);
	}

	LPVOID CStdValArray::GetAt(int iIndex) const
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
		return m_pVoid + (iIndex * m_iElementSize);
	}

	LPVOID CStdValArray::operator[] (int iIndex) const
	{
		ASSERT(iIndex>=0 && iIndex<m_nCount);
		return m_pVoid + (iIndex * m_iElementSize);
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	QkString QkString::_EmptyInstance = L"";

	QkString::QkString() : m_pstr(m_szBuffer), _capacity(0), _isBuffer(0)
	{
		m_szBuffer[0] = '\0';
		_size = 0;
	}

	QkString::QkString(const TCHAR ch) : m_pstr(m_szBuffer), _capacity(0), _isBuffer(0)
	{
		m_szBuffer[0] = ch;
		m_szBuffer[1] = '\0';
		_size = 1;
	}

	QkString::QkString(const CHAR* ch) : m_pstr(m_szBuffer), _capacity(0), _isBuffer(0)
	{
		_size = 0;
		*this = ch;
	}

	QkString::QkString(LPCTSTR lpsz, int nLen) : m_pstr(m_szBuffer), _capacity(0), _isBuffer(0)
	{      
		ASSERT(!::IsBadStringPtr(lpsz,-1) || lpsz==NULL);
		m_szBuffer[0] = '\0';
		_size = 0;
		Assign(lpsz, nLen);
	}

	QkString::QkString(const QkString& src) : m_pstr(m_szBuffer), _capacity(0), _isBuffer(0)
	{
		m_szBuffer[0] = '\0';
		_size = 0;
		Assign(src);
	}

	QkString::~QkString()
	{
		if( m_pstr != m_szBuffer ) free(m_pstr);
	}

	size_t QkString::GetLength() const
	{ 
		return _size; 
		//return (int) _tcslen(m_pstr); 
	}

	QkString::operator LPCTSTR() const 
	{ 
		return m_pstr; 
	}

	bool QkString::EnsureCapacity(size_t nNewLength, bool release)
	{
		if (_isBuffer && _capacity>=nNewLength) return true;
		if( nNewLength > MAX_LOCAL_STRING_LEN ) 
		{
			if( m_pstr == m_szBuffer ) 
			{
				m_pstr = static_cast<LPTSTR>(malloc((nNewLength + 1) * sizeof(TCHAR)));
				if (m_pstr)
				{
					_tcsncpy(m_pstr, m_szBuffer, GetLength());
					m_pstr[_size] = 0;
					_capacity = nNewLength;
					return true;
				}
				else
				{
					m_pstr = m_szBuffer;
					// malloc failed
				}
			}
			else if(_capacity<nNewLength)
			{
				size_t newCapacity = nNewLength*2 + 1;
				if (newCapacity<nNewLength)
					newCapacity = nNewLength;
				LPTSTR _pstr = static_cast<LPTSTR>(realloc(m_pstr, (newCapacity + 1) * sizeof(TCHAR)));
				if (_pstr)
				{
					m_pstr = _pstr;
					_capacity = newCapacity;
					return true;
				}
				// realloc failed.
			}
			else return true;
		}
		else {
			if( m_pstr != m_szBuffer && release ) 
			{
				// never happen ? 
				// will if Assign -> enssure cap -> ...
				// _tcscpy(m_szBuffer, m_pstr); // dont bother
				free(m_pstr);
				_capacity = 0;
				m_pstr = m_szBuffer;
			}
			return true;
		}
		// ensure capacity failed.
		return false;
	}

	size_t QkString::RecalcSize()
	{
		size_t ret = _size;
		if(SUCCEEDED(StringCchLength(m_pstr, max(_capacity, MAX_LOCAL_STRING_LEN), &ret)))
			_size = ret;
		return ret;
	}

	void QkString::ReFit()
	{
		size_t sz = RecalcSize();
		if (sz > MAX_LOCAL_STRING_LEN)
		{
			if (_capacity+MAX_LOCAL_STRING_LEN>sz)
			{
				LPTSTR _pstr = static_cast<LPTSTR>(realloc(m_pstr, (sz + 1) * sizeof(TCHAR)));
				if (_pstr)
				{
					m_pstr = _pstr;
					_capacity = sz;
				}
				// realloc failed.
			}
		}
		else if (m_pstr!=m_szBuffer)
		{
			_tcsncpy(m_szBuffer, m_pstr, sz);
			free(m_pstr);
			_capacity = 0;
			m_pstr = m_szBuffer;
		}
	}

	// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/strcpy-wcscpy-mbscpy?view=msvc-160
	
	// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/strncpy-strncpy-l-wcsncpy-wcsncpy-l-mbsncpy-mbsncpy-l?view=msvc-160
	
	// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/strncat-strncat-l-wcsncat-wcsncat-l-mbsncat-mbsncat-l?view=msvc-160

	void QkString::Prepend(LPCTSTR pstr, int nLength)
	{
		//QkString backup = *this;
		//Assign(pstr);
		//Append(backup, backup.GetLength());
		if( !pstr ) return;
		nLength = (nLength < 0 ? (int) _tcslen(pstr) : nLength);
		int nNewLength = GetLength() + nLength;
		if (EnsureCapacity(nNewLength, false))
		{
			wmemmove(m_pstr+nLength, m_pstr, GetLength());
			//TCHAR backup = m_pstr[nLength];
			_tcsncpy(m_pstr, pstr, nLength);
			//m_pstr[nLength] = backup;
			m_pstr[_size = nNewLength] = L'\0';
		}
	}

	void QkString::Append(LPCTSTR pstr, int nLength)
	{
		nLength = (nLength < 0 ? (int) _tcslen(pstr) : nLength);
		int nNewLength = GetLength() + nLength;
		if (EnsureCapacity(nNewLength, false))
		{
			//todo
			_tcsncat(m_pstr, pstr, nLength);
			_size = nNewLength;
		}
	}

	void QkString::Assign(LPCTSTR pstr, int cchMax)
	{
		if( pstr == NULL ) pstr = _T("");
		cchMax = (cchMax < 0 ? (int) _tcslen(pstr) : cchMax);
		if (EnsureCapacity(cchMax, true))
		{
			_tcsncpy(m_pstr, pstr, cchMax);
			m_pstr[cchMax] = '\0';
			_size = cchMax;
		}
	}

	bool QkString::IsEmpty() const 
	{ 
		return m_pstr[0] == '\0'; 
	}

	void QkString::Empty() 
	{ 
		if (_size)
		{
			_size = 0;
			if (!_isBuffer)
			{
				if( m_pstr != m_szBuffer ) 
				{
					free(m_pstr);
					_capacity = 0;
					m_pstr = m_szBuffer;
				}
			}
			m_pstr[0] = '\0'; 
		}
	}

	LPCTSTR QkString::GetData() const
	{
		return m_pstr;
	}

	LPCSTR QkString::GetData(std::string & buffer) const
	{
		size_t max_cap = GetLength()*2;
		if (buffer.capacity()<max_cap)
		{
			buffer.resize(MAX(max_cap*1.2, max_cap));
		}
		CHAR* data = (CHAR*)buffer.c_str();
		size_t len = ::WideCharToMultiByte(::GetACP(), 0, GetData(), GetLength(), data, max_cap, NULL, NULL);
		data[len] = '\0';
		return buffer.c_str();
	}

	LPCSTR QkString::GetData(std::string & buffer, LPCWSTR pStr)
	{
		size_t length = lstrlen(pStr);
		size_t max_cap = length*2;
		if (buffer.capacity()<max_cap)
		{
			buffer.resize(MAX(max_cap*1.2, max_cap));
		}
		CHAR* data = (CHAR*)buffer.c_str();
		size_t len = ::WideCharToMultiByte(::GetACP(), 0, pStr, length, data, max_cap, NULL, NULL);
		data[len] = '\0';
		return data;
	}

	TCHAR QkString::GetAt(int nIndex) const
	{
		return m_pstr[nIndex];
	}

	TCHAR QkString::operator[] (int nIndex) const
	{ 
		return m_pstr[nIndex];
	}   

	const QkString& QkString::operator=(const QkString& src)
	{      
		Assign(src, src.GetLength());
		return *this;
	}

	const QkString& QkString::operator=(LPCTSTR lpStr)
	{      
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtr(lpStr,-1));
			Assign(lpStr);
		}
		else
		{
			Empty();
		}
		return *this;
	}

#ifdef _UNICODE

	const QkString& QkString::operator=(LPCSTR lpStr)
	{
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtrA(lpStr,-1));
			int cchStr = (int) strlen(lpStr) + 1;
			LPWSTR pwstr = (LPWSTR) _malloca(cchStr*sizeof(TCHAR));
			if( pwstr != NULL ) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr) ;
			Assign(pwstr);
			_freea(pwstr);
		}
		else
		{
			Empty();
		}
		return *this;
	}

	const QkString& QkString::operator+=(LPCSTR lpStr)
	{
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtrA(lpStr,-1));
			int cchStr = (int) strlen(lpStr) + 1;
			LPWSTR pwstr = (LPWSTR) _malloca(cchStr*sizeof(TCHAR));
			if( pwstr != NULL ) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr) ;
			Append(pwstr);
			_freea(pwstr);
		}
		
		return *this;
	}

#else

	const QkString& QkString::operator=(LPCWSTR lpwStr)
	{      
		if ( lpwStr )
		{
			ASSERT(!::IsBadStringPtrW(lpwStr,-1));
			int cchStr = ((int) wcslen(lpwStr) * 2) + 1;
			LPSTR pstr = (LPSTR) _alloca(cchStr);
			if( pstr != NULL ) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
			Assign(pstr);
		}
		else
		{
			Empty();
		}
		
		return *this;
	}

	const QkString& QkString::operator+=(LPCWSTR lpwStr)
	{
		if ( lpwStr )
		{
			ASSERT(!::IsBadStringPtrW(lpwStr,-1));
			int cchStr = ((int) wcslen(lpwStr) * 2) + 1;
			LPSTR pstr = (LPSTR) _alloca(cchStr);
			if( pstr != NULL ) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
			Append(pstr);
		}
		
		return *this;
	}

#endif // _UNICODE

	const QkString& QkString::operator=(const TCHAR ch)
	{
		Empty();
		m_szBuffer[0] = ch;
		m_szBuffer[1] = '\0';
		return *this;
	}

	QkString QkString::operator+(const QkString& src) const
	{
		QkString sTemp = *this;
		sTemp.Append(src);
		return sTemp;
	}

	QkString QkString::operator+(LPCTSTR lpStr) const
	{
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtr(lpStr,-1));
			QkString sTemp = *this;
			sTemp.Append(lpStr);
			return sTemp;
		}

		return *this;
	}

	const QkString& QkString::operator+=(const QkString& src)
	{      
		Append(src);
		return *this;
	}

	const QkString& QkString::operator+=(LPCTSTR lpStr)
	{      
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtr(lpStr,-1));
			Append(lpStr);
		}
		
		return *this;
	}

	const QkString& QkString::operator+=(const TCHAR ch)
	{      
		TCHAR str[] = { ch, '\0' };
		Append(str);
		return *this;
	}

	bool QkString::operator == (LPCTSTR str) const { return (Compare(str) == 0); };
	bool QkString::operator != (LPCTSTR str) const { return (Compare(str) != 0); };
	bool QkString::operator == (const QkString & str) const{ return _size==str._size && _tcsncmp(m_pstr, str, _size) == 0; };
	bool QkString::operator != (const QkString & str) const{ return _size!=str._size || _tcsncmp(m_pstr, str, _size) != 0; };
	bool QkString::operator <= (LPCTSTR str) const { return (Compare(str) <= 0); };
	bool QkString::operator <  (LPCTSTR str) const { return (Compare(str) <  0); };
	bool QkString::operator >= (LPCTSTR str) const { return (Compare(str) >= 0); };
	bool QkString::operator >  (LPCTSTR str) const { return (Compare(str) >  0); };

	void QkString::SetAt(int nIndex, TCHAR ch)
	{
		ASSERT(nIndex>=0 && nIndex<GetLength());
		m_pstr[nIndex] = ch;
	}

	int QkString::Compare(LPCTSTR lpsz) const 
	{ 
		return _tcscmp(m_pstr, lpsz); 
	}

	int QkString::CompareNoCase(LPCTSTR lpsz) const 
	{ 
		return _tcsicmp(m_pstr, lpsz); 
	}

	void QkString::MakeUpper() 
	{ 
		_tcsupr(m_pstr); 
	}

	void QkString::MakeLower() 
	{ 
		_tcslwr(m_pstr); 
	}

	QkString QkString::Left(int iLength) const
	{
		if( iLength < 0 ) iLength = 0;
		if( iLength > GetLength() ) iLength = GetLength();
		return QkString(m_pstr, iLength);
	}

	void QkString::MidFast(int iPos, int iLength)
	{
		if( iLength < 0 ) iLength = GetLength() - iPos;
		if( iPos + iLength > GetLength() ) iLength = GetLength() - iPos;
		if (iLength<GetLength())
		{
			if (iPos>0)
			{
				wmemmove(m_pstr, m_pstr+iPos, iLength);
			}
			m_pstr[iLength] = '\0';
			_size = iLength;
		}
	}

	QkString QkString::Mid(int iPos, int iLength) const
	{
		if( iLength < 0 ) iLength = GetLength() - iPos;
		if( iPos + iLength > GetLength() ) iLength = GetLength() - iPos;
		if( iLength <= 0 ) return QkString();
		return QkString(m_pstr + iPos, iLength);
	}

	QkString QkString::Right(int iLength) const
	{
		int iPos = GetLength() - iLength;
		if( iPos < 0 ) {
			iPos = 0;
			iLength = GetLength();
		}
		return QkString(m_pstr + iPos, iLength);
	}

	int QkString::Find(TCHAR ch, int iPos /*= 0*/) const
	{
		ASSERT(iPos>=0 && iPos<=GetLength());
		if( iPos != 0 && (iPos < 0 || iPos >= GetLength()) ) return -1;
		LPCTSTR p = _tcschr(m_pstr + iPos, ch);
		if( p == NULL ) return -1;
		return (int)(p - m_pstr);
	}

	int QkString::Find(LPCTSTR pstrSub, int iPos /*= 0*/) const
	{
		ASSERT(!::IsBadStringPtr(pstrSub,-1));
		ASSERT(iPos>=0 && iPos<=GetLength());
		if( iPos != 0 && (iPos < 0 || iPos > GetLength()) ) return -1;
		LPCTSTR p = _tcsstr(m_pstr + iPos, pstrSub);
		if( p == NULL ) return -1;
		return (int)(p - m_pstr);
	}

	int QkString::ReverseFind(TCHAR ch) const
	{
		LPCTSTR p = _tcsrchr(m_pstr, ch);
		if( p == NULL ) return -1;
		return (int)(p - m_pstr);
	}

	int QkString::Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo)
	{
		//QkString sTemp;
		//int nCount = 0;
		//int iPos = Find(pstrFrom);
		//if( iPos < 0 ) return 0;
		//int cchFrom = (int) _tcslen(pstrFrom);
		//int cchTo = (int) _tcslen(pstrTo);
		//while( iPos >= 0 ) {
		//	sTemp = Left(iPos);
		//	sTemp += pstrTo;
		//	sTemp += Mid(iPos + cchFrom);
		//	Assign(sTemp);
		//	iPos = Find(pstrFrom, iPos + cchTo);
		//	nCount++;
		//}
		//return nCount;
		QkString sTemp;
		int last_find_start = 0;
		int nCount = 0;
		int iPos = Find(pstrFrom);
		if( iPos < 0 ) return 0;
		int cchFrom = (int) _tcslen(pstrFrom);
		int cchTo = (int) _tcslen(pstrTo);
		while( iPos >= 0 ) {
			sTemp.Append(m_pstr+last_find_start, iPos-last_find_start);
			sTemp.Append(pstrTo, cchTo);
			iPos = Find(pstrFrom, last_find_start = iPos + cchFrom);
			nCount++;
		}
		if (nCount)
		{
			sTemp.Append(m_pstr+last_find_start, iPos-last_find_start);
			Assign(sTemp);
		}
		return nCount;
	}

	void QkString::Trim()
	{
		int st = 0;
		int ed = GetLength();
		while (st < ed && m_pstr[st] == L' ') st++;
		// todo skip other empty chars
		while (st < ed && m_pstr[ed - 1] == L' ') ed--;
		// todo skip other empty chars
		if (st > 0 || ed < GetLength())
		{
			//Assign(Mid(st, len-st));
			if (st)
			{
				wmemmove(m_pstr, m_pstr+st, ed-st);
			}
			m_pstr[ed-st] = 0;
			_size = ed-st;
		}
	}

	bool QkString::StartWith(const QkString & other, bool ignoreCase, int offset)
	{
		int pos = offset;
		int pos1 = 0;
		int len = GetLength();
		int len1 = other.GetLength();
		if (pos==-1) pos=len-len1; // end with
		if (pos < 0 || pos+len1 > len) return false;
		LPCTSTR data = GetData();
		LPCTSTR data1 = other.GetData();
		if (ignoreCase) 
		{
			while (--len1 >= 0) 
				if (_totlower(data[pos++]) != _totlower(data1[pos1++])) 
					return false;
		}
		else while (--len1 >= 0)
			if (data[pos++] != data1[pos1++])
				return false;
		return true;
	}

	bool QkString::EndWith(const QkString & other, bool ignoreCase)
	{
		return StartWith(other, ignoreCase, -1);
	}

	void QkString::Split(const QkString & delim, std::vector<QkString> & ret)
	{
		int last = 0;  
		int index=Find(delim, last);  
		while (index!=-1)  
		{  
			ret.push_back(Mid(last,index-last));
			index=Find(delim, last=index+delim.GetLength());  
		}  
		if (GetLength()>last+1)  
		{  
			ret.push_back(Mid(last,GetLength()-last));  
		}  
	}
	
	int QkString::Format(LPCTSTR pstrFormat, ...)
	{
		int nRet;
		va_list Args;

		va_start(Args, pstrFormat);
		nRet = InnerFormat(pstrFormat, Args);
		va_end(Args);

		return nRet;
	}

	int QkString::SmallFormat(LPCTSTR pstrFormat, ...)
	{
		QkString sFormat = pstrFormat;
		TCHAR szBuffer[64] = { 0 };
		va_list argList;
		va_start(argList, pstrFormat);
		int iRet = ::_vsntprintf(szBuffer, sizeof(szBuffer), sFormat, argList);
		va_end(argList);
		Assign(szBuffer);
		return iRet;
	}
	
	int QkString::InnerFormat(LPCTSTR pstrFormat, va_list Args)
	{
#if _MSC_VER <= 1400
		TCHAR *szBuffer = NULL;
		int size = 512, nLen, counts;
		szBuffer = (TCHAR*)malloc(size);
		ZeroMemory(szBuffer, size);
		while (TRUE){
			counts = size / sizeof(TCHAR);
			nLen = _vsntprintf (szBuffer, counts, pstrFormat, Args);
			if (nLen != -1 && nLen < counts){
				break;
			}
			if (nLen == -1){
				size *= 2;
			}else{
				size += 1 * sizeof(TCHAR);
			}

			if ((szBuffer = (TCHAR*)realloc(szBuffer, size)) != NULL){
				ZeroMemory(szBuffer, size);
			}else{
				break;
			}
		}

		Assign(szBuffer);
		free(szBuffer);
		return nLen;
#else
		int nLen, totalLen;
		TCHAR *szBuffer;
		nLen = _vsntprintf(NULL, 0, pstrFormat, Args);
		totalLen = (nLen + 1)*sizeof(TCHAR);
		szBuffer = (TCHAR*)malloc(totalLen);
		ZeroMemory(szBuffer, totalLen);
		nLen = _vsntprintf(szBuffer, nLen + 1, pstrFormat, Args);
		Assign(szBuffer);
		free(szBuffer);
		return nLen;

#endif
	}

	/////////////////////////////////////////////////////////////////////////////
	//
	//

	static UINT HashKey(LPCTSTR Key)
	{
		UINT i = 0;
		SIZE_T len = _tcslen(Key);
		while( len-- > 0 ) i = (i << 5) + i + Key[len];
		return i;
	}

	static UINT HashKey(const QkString& Key)
	{
		UINT i = 0;
		SIZE_T len = Key.GetLength();
		LPCTSTR KeyData = Key;
		while( len-- > 0 ) i = (i << 5) + i + KeyData[len];
		return i;
	};

	QkStringPtrMap::QkStringPtrMap(int nSize) : m_nCount(0)
	{
		if( nSize < 16 ) nSize = 16;
		m_nBuckets = nSize;
		m_aT = new TITEM*[nSize];
		memset(m_aT, 0, nSize * sizeof(TITEM*));
	}

	QkStringPtrMap::~QkStringPtrMap()
	{
		if( m_aT ) {
			int len = m_nBuckets;
			while( len-- ) {
				TITEM* pItem = m_aT[len];
				while( pItem ) {
					TITEM* pKill = pItem;
					pItem = pItem->pNext;
					delete pKill;
				}
			}
			delete [] m_aT;
			m_aT = NULL;
		}
	}

	void QkStringPtrMap::RemoveAll()
	{
		this->Resize(m_nBuckets);
	}

	void QkStringPtrMap::Resize(int nSize)
	{
		if( m_aT ) {
			int len = m_nBuckets;
			while( len-- ) {
				TITEM* pItem = m_aT[len];
				while( pItem ) {
					TITEM* pKill = pItem;
					pItem = pItem->pNext;
					delete pKill;
				}
			}
			delete [] m_aT;
			m_aT = NULL;
		}

		if( nSize < 0 ) nSize = 0;
		if( nSize > 0 ) {
			m_aT = new TITEM*[nSize];
			memset(m_aT, 0, nSize * sizeof(TITEM*));
		} 
		m_nBuckets = nSize;
		m_nCount = 0;
	}

	LPVOID QkStringPtrMap::FindIntern(UINT hash, const QkString & key, bool optimize) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return NULL;

		UINT slot = hash % m_nBuckets;
		for( TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext ) {
			if( pItem->Key == key ) {
				if (optimize && pItem != m_aT[slot]) {
					if (pItem->pNext) {
						pItem->pNext->pPrev = pItem->pPrev;
					}
					pItem->pPrev->pNext = pItem->pNext;
					pItem->pPrev = NULL;
					pItem->pNext = m_aT[slot];
					pItem->pNext->pPrev = pItem;
					//将item移动至链条头部
					m_aT[slot] = pItem;
				}
				return pItem->Data;
			}        
		}

		return NULL;
	}

	LPVOID QkStringPtrMap::Find(LPCTSTR key, bool optimize) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return NULL;

		UINT slot = HashKey(key) % m_nBuckets;
		for( TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext ) {
			if( pItem->Key == key ) {
				if (optimize && pItem != m_aT[slot]) {
					if (pItem->pNext) {
						pItem->pNext->pPrev = pItem->pPrev;
					}
					pItem->pPrev->pNext = pItem->pNext;
					pItem->pPrev = NULL;
					pItem->pNext = m_aT[slot];
					pItem->pNext->pPrev = pItem;
					//将item移动至链条头部
					m_aT[slot] = pItem;
				}
				return pItem->Data;
			}        
		}

		return NULL;
	}

	bool QkStringPtrMap::Insert(const QkString & key, LPVOID pData)
	{
		if( m_nBuckets == 0 ) return false;
		UINT hash = HashKey(key);
		if( FindIntern(hash, key) ) return false;

		// Add first in bucket
		UINT slot = hash % m_nBuckets;
		TITEM* pItem = new TITEM;
		pItem->Key = key;
		pItem->Data = pData;
		pItem->pPrev = NULL;
		pItem->pNext = m_aT[slot];
		if (pItem->pNext)
			pItem->pNext->pPrev = pItem;
		m_aT[slot] = pItem;
		m_nCount++;
		return true;
	}

	bool QkStringPtrMap::Insert(LPCTSTR key, LPVOID pData)
	{
		if( m_nBuckets == 0 ) return false;
		if( Find(key) ) return false;

		// Add first in bucket
		UINT slot = HashKey(key) % m_nBuckets;
		TITEM* pItem = new TITEM;
		pItem->Key = key;
		pItem->Data = pData;
		pItem->pPrev = NULL;
		pItem->pNext = m_aT[slot];
		if (pItem->pNext)
			pItem->pNext->pPrev = pItem;
		m_aT[slot] = pItem;
		m_nCount++;
		return true;
	}

	LPVOID QkStringPtrMap::Set(LPCTSTR key, LPVOID pData)
	{
		if( m_nBuckets == 0 ) return pData;

		if (GetSize()>0) {
			UINT slot = HashKey(key) % m_nBuckets;
			// Modify existing item
			for( TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext ) {
				if( pItem->Key == key ) {
					LPVOID pOldData = pItem->Data;
					pItem->Data = pData;
					return pOldData;
				}
			}
		}

		Insert(key, pData);
		return NULL;
	}

	bool QkStringPtrMap::Remove(LPCTSTR key)
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return false;

		UINT slot = HashKey(key) % m_nBuckets;
		TITEM** ppItem = &m_aT[slot];
		while( *ppItem ) {
			if( (*ppItem)->Key == key ) {
				TITEM* pKill = *ppItem;
				*ppItem = (*ppItem)->pNext;
				if (*ppItem)
					(*ppItem)->pPrev = pKill->pPrev;
				delete pKill;
				m_nCount--;
				return true;
			}
			ppItem = &((*ppItem)->pNext);
		}

		return false;
	}

	int QkStringPtrMap::GetSize() const
	{
#if 0//def _DEBUG
		int nCount = 0;
		int len = m_nBuckets;
		while( len-- ) {
			for( const TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) nCount++;
		}
		ASSERT(m_nCount==nCount);
#endif
		return m_nCount;
	}

	const TITEM* QkStringPtrMap::GetSlotAt(int iIndex) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return NULL;

		int pos = 0;
		int len = m_nBuckets;
		while( len-- ) {
			for( TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) {
				if( pos++ == iIndex ) {
					return pItem;
				}
			}
		}

		return NULL;
	}

	LPCTSTR QkStringPtrMap::GetKeyAt(int iIndex) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return false;

		int pos = 0;
		int len = m_nBuckets;
		while( len-- ) {
			for( TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) {
				if( pos++ == iIndex ) {
					return pItem->Key.GetData();
				}
			}
		}

		return NULL;
	}

	LPVOID QkStringPtrMap::GetValueAt(int iIndex) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return false;

		int pos = 0;
		int len = m_nBuckets;
		while( len-- ) {
			for( TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) {
				if( pos++ == iIndex ) {
					return pItem->Data;
				}
			}
		}

		return NULL;
	}

	bool QkStringPtrMap::GetKeyValueAt(int iIndex, LPCTSTR & key, LPVOID & value) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return false;

		int pos = 0;
		int len = m_nBuckets;
		while( len-- ) {
			for( TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) {
				if( pos++ == iIndex ) {
					key = pItem->Key.GetData();
					value = pItem->Data;
					return true;
				}
			}
		}

		return false;
	}

	LPCTSTR QkStringPtrMap::operator[] (int nIndex) const
	{
		return GetKeyAt(nIndex);
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CWaitCursor::CWaitCursor()
	{
		m_hOrigCursor = ::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	}

	CWaitCursor::~CWaitCursor()
	{
		::SetCursor(m_hOrigCursor);
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	//CImageString::CImageString()
	//{
	//	Clear();
	//}

	//CImageString::CImageString(const CImageString& image)
	//{
	//	Clone(image);
	//}

	//const CImageString& CImageString::operator=(const CImageString& image)
	//{
	//	Clone(image);
	//	return *this;
	//}

	//void CImageString::Clone(const CImageString& image)
	//{
	//	m_sImageAttribute = image.m_sImageAttribute;

	//	m_sImage = image.m_sImage;
	//	m_sResType = image.m_sResType;
	//	m_imageInfo = image.m_imageInfo;
	//	m_bLoadSuccess = image.m_bLoadSuccess;

	//	m_rcDest = image.m_rcDest;
	//	m_rcSource = image.m_rcSource;
	//	m_rcCorner = image.m_rcCorner;
	//	m_bFade = image.m_bFade;
	//	m_dwMask = image.m_dwMask;
	//	m_bHole = image.m_bHole;
	//	m_bTiledX = image.m_bTiledX;
	//	m_bTiledY = image.m_bTiledY;
	//}

	//CImageString::~CImageString()
	//{

	//}

	//const QkString& CImageString::GetAttributeString() const
	//{
	//	return m_sImageAttribute;
	//}

	//void CImageString::SetAttributeString(LPCTSTR pStrImageAttri)
	//{
	//	if (m_sImageAttribute == pStrImageAttri) return;
	//	Clear();
	//	m_sImageAttribute = pStrImageAttri;
	//	m_sImage = m_sImageAttribute;
	//}

	//bool CImageString::LoadImage(CPaintManagerUI* pManager)
	//{
	//	m_imageInfo = NULL;
	//	m_bLoadSuccess = true;
	//	ZeroMemory(&m_rcDest, sizeof(RECT));
	//	ZeroMemory(&m_rcSource, sizeof(RECT));
	//	ZeroMemory(&m_rcCorner, sizeof(RECT));
	//	m_bFade = 0xFF;
	//	m_dwMask = 0;
	//	m_bHole = false;
	//	m_bTiledX = false;
	//	m_bTiledY = false;
	//	ParseAttribute(m_sImageAttribute,*pManager->GetDPIObj());
	//	if (!m_bLoadSuccess) return false;

	//	const TImageInfo* data = NULL;
	//	if (m_sResType.IsEmpty())
	//	{
	//		data = pManager->GetImageEx((LPCTSTR)m_sImage, NULL, m_dwMask);
	//	}
	//	else
	//	{
	//		data = pManager->GetImageEx((LPCTSTR)m_sImage, (LPCTSTR)m_sResType, m_dwMask);
	//	}
	//	if (data == NULL)
	//	{
	//		m_bLoadSuccess = false;
	//		return false;
	//	}
	//	else
	//	{
	//		m_bLoadSuccess = true;
	//	}

	//	if (m_rcSource.left == 0 && m_rcSource.right == 0 && m_rcSource.top == 0 && m_rcSource.bottom == 0)
	//	{
	//		m_rcSource.right = data->nX;
	//		m_rcSource.bottom = data->nY;
	//	}
	//	if (m_rcSource.right > data->nX) m_rcSource.right = data->nX;
	//	if (m_rcSource.bottom > data->nY) m_rcSource.bottom = data->nY;
	//	m_imageInfo = const_cast<TImageInfo*>(data);

	//	return true;
	//}

	//bool CImageString::IsLoadSuccess()
	//{
	//	return !m_sImageAttribute.IsEmpty() && m_bLoadSuccess;
	//}

	//void CImageString::ModifyAttribute(LPCTSTR pStrModify)
	//{
	//	//ParseAttribute(pStrModify);
	//}

	//void CImageString::Clear()
	//{
	//	m_sImageAttribute.Empty();
	//	m_sImage.Empty();
	//	m_sResType.Empty();
	//	m_imageInfo = NULL;
	//	m_bLoadSuccess = true;
	//	ZeroMemory(&m_rcDest, sizeof(RECT));
	//	ZeroMemory(&m_rcSource, sizeof(RECT));
	//	ZeroMemory(&m_rcCorner, sizeof(RECT));
	//	m_bFade = 0xFF;
	//	m_dwMask = 0;
	//	m_bHole = false;
	//	m_bTiledX = false;
	//	m_bTiledY = false;
	//}

	//void CImageString::ParseAttribute(LPCTSTR pStrImage)
	//{
	//	if (pStrImage == NULL)
	//		return;

	//	// 1、aaa.jpg
	//	// 2、file='aaa.jpg' res='' restype='0' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0' 
	//	// mask='#FF0000' fade='255' hole='false' xtiled='false' ytiled='false'
	//	QkString sItem;
	//	QkString sValue;
	//	LPTSTR pstr = NULL;

	//	while (*pStrImage != _T('\0'))
	//	{
	//		sItem.Empty();
	//		sValue.Empty();
	//		while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNext(pStrImage);
	//		while (*pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' '))
	//		{
	//			LPTSTR pstrTemp = ::CharNext(pStrImage);
	//			while (pStrImage < pstrTemp)
	//			{
	//				sItem += *pStrImage++;
	//			}
	//		}
	//		while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNext(pStrImage);
	//		if (*pStrImage++ != _T('=')) break;
	//		while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNext(pStrImage);
	//		if (*pStrImage++ != _T('\'')) break;
	//		while (*pStrImage != _T('\0') && *pStrImage != _T('\''))
	//		{
	//			LPTSTR pstrTemp = ::CharNext(pStrImage);
	//			while (pStrImage < pstrTemp)
	//			{
	//				sValue += *pStrImage++;
	//			}
	//		}
	//		if (*pStrImage++ != _T('\'')) break;
	//		if (!sValue.IsEmpty())
	//		{
	//			if (sItem == _T("file") || sItem == _T("res"))
	//			{
	//				m_sImage = sValue;
	//				//if (g_Dpi.GetScale() != 100) {
	//				//	std::wstringstream wss;
	//				//	wss << L"@" << g_Dpi.GetScale() << L".";
	//				//	std::wstring suffix = wss.str();
	//				//	m_sImage.Replace(L".", suffix.c_str());
	//				//}
	//			}
	//			else if (sItem == _T("restype"))
	//			{					
	//				m_sResType = sValue;
	//			}
	//			else if (sItem == _T("dest"))
	//			{
	//				m_rcDest.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);
	//				m_rcDest.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
	//				m_rcDest.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
	//				m_rcDest.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);

	//				//g_Dpi.ScaleRect(&m_rcDest);
	//			}
	//			else if (sItem == _T("source"))
	//			{
	//				m_rcSource.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);
	//				m_rcSource.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
	//				m_rcSource.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
	//				m_rcSource.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
	//				//g_Dpi.ScaleRect(&m_rcSource);
	//			}
	//			else if (sItem == _T("corner"))
	//			{
	//				m_rcCorner.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);
	//				m_rcCorner.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
	//				m_rcCorner.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
	//				m_rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
	//				//g_Dpi.ScaleRect(&m_rcCorner);
	//			}
	//			else if (sItem == _T("mask"))
	//			{
	//				if (sValue[0] == _T('#')) m_dwMask = _tcstoul(sValue.GetData() + 1, &pstr, 16);
	//				else m_dwMask = _tcstoul(sValue.GetData(), &pstr, 16);
	//			}
	//			else if (sItem == _T("fade"))
	//			{
	//				m_bFade = (BYTE)_tcstoul(sValue.GetData(), &pstr, 10);
	//			}
	//			else if (sItem == _T("hole"))
	//			{
	//				m_bHole = (_tcscmp(sValue.GetData(), _T("true")) == 0);
	//			}
	//			else if (sItem == _T("xtiled"))
	//			{
	//				m_bTiledX = (_tcscmp(sValue.GetData(), _T("true")) == 0);
	//			}
	//			else if (sItem == _T("ytiled"))
	//			{
	//				m_bTiledY = (_tcscmp(sValue.GetData(), _T("true")) == 0);
	//			}
	//		}
	//		if (*pStrImage++ != _T(' ')) break;
	//	}
	//}

	//void CImageString::SetDest(const RECT &rcDest)
	//{
	//	m_rcDest = rcDest;
	//}

	//RECT CImageString::GetDest() const
	//{
	//	return m_rcDest;
	//}

	//const TImageInfo* CImageString::GetImageInfo() const
	//{
	//	return m_imageInfo;
	//}

	LPCTSTR STR2ARGB(LPCTSTR STR, DWORD & ARGB)
	{
		bool intOpened=false;
		int index=0;
		ARGB = 0;
		while(index<16) {
			const char & intVal = *(STR++);
			//if (intVal>'\0' && intVal<=' ') continue;
			if (intVal>='0' && intVal<='9')
			{
				ARGB = ARGB*16+(intVal - '0');
				if(!intOpened && intVal) intOpened=true;
			}
			else if (intVal>='A' && intVal<='F')
			{
				ARGB = ARGB*16+(10 + intVal - 'A');
				if(!intOpened && intVal) intOpened=true;
			}
			else if (intVal>='a' && intVal<='f')
			{
				ARGB = ARGB*16+(10 + intVal - 'a');
				if(!intOpened && intVal) intOpened=true;
			}
			else if (intOpened || intVal=='\0')
			{
				break;
			}
			if (intOpened)
			{
				index++;
			}
		}
		if (index==3)
		{
			ARGB = (ARGB&0xF)<<4 | (ARGB&0xF0)<<8 | (ARGB&0xF00)<<12;
			ARGB = ARGB | ARGB>>4 | 0xFF000000;
		}
		if (!intOpened)
		{

		}
		//DWORD val = _tcstoul(STR, 0, 16);
		//assert(ARGB==val);
		//ARGB = _tcstoul(STR, 0, 16);
		return STR;
	}

	int ParseInt(LPCTSTR STR)
	{
		long value=0;
		STR2Decimal(STR, value);
		return value;
	}

	LPCTSTR STR2Decimal(LPCTSTR STR, long & value)
	{
		//long val = _tcstol(STR, 0, 10);
		bool intOpened=false;
		int index=0;
		bool negative = false;
		value = 0;
		while(index<1024) {
			const char & intVal = *(STR++);
			if (intVal>='0' && intVal<='9')
			{
				value = value*10+(intVal - '0');
				if(!intOpened && intVal) intOpened=true;
			}
			else if (!intOpened && intVal=='-')
			{
				negative = !negative;
			}
			else if (intOpened || intVal=='\0')
			{
				if (intVal=='\0')
				{
					STR--;
				}
				break;
			}
			index++;
		}
		if (negative)
		{
			value = -value;
		}
		//assert(value==val);

		//ARGB = _tcstoul(STR, 0, 16);
		return STR;
	}

	LPCTSTR STR2Rect(LPCTSTR STR, RECT & rc)
	{
		STR = STR2Decimal(STR, rc.left);
		STR = STR2Decimal(STR, rc.top);
		if (!*STR)
		{
			rc.right = rc.left;
			rc.bottom = rc.top;
		}
		else
		{
			STR = STR2Decimal(STR, rc.right);
			STR = STR2Decimal(STR, rc.bottom);
		}
		return STR;
	}

	LPCTSTR STR2Size(LPCTSTR STR, SIZE & sz)
	{
		STR = STR2Decimal(STR, sz.cx);
		if (!*STR)
		{
			sz.cy = sz.cx;
		}
		else
		{
			STR = STR2Decimal(STR, sz.cy);
		}
		return STR;
	}

	void SetRectInt(RECT & rc, int value)
	{
		rc.left = value;
		rc.top = value;
		rc.right = value;
		rc.bottom = value;
	}

} // namespace DuiLib