#include "StdAfx.h"

#ifndef TRACE
#define TRACE
#endif

namespace DuiLib {
///////////////////////////////////////////////////////////////////////////////////////
//
//
//
XMarkupNode::XMarkupNode() : m_pOwner(NULL)
{
}

XMarkupNode::XMarkupNode(XMarkupParser* pOwner, int iPos) : m_pOwner(pOwner), m_iPos(iPos), m_nAttributes(0)
{
}

XMarkupNode XMarkupNode::GetSibling()
{
	if( m_pOwner == NULL ) return XMarkupNode();
	ULONG iPos = m_pOwner->m_pElements[m_iPos].iNext;
	if( iPos == 0 ) return XMarkupNode();
	return XMarkupNode(m_pOwner, iPos);
}

bool XMarkupNode::HasSiblings() const
{
	if( m_pOwner == NULL ) return false;
	ULONG iPos = m_pOwner->m_pElements[m_iPos].iNext;
	return iPos > 0;
}

XMarkupNode XMarkupNode::GetChild()
{
	if( m_pOwner == NULL ) return XMarkupNode();
	ULONG iPos = m_pOwner->m_pElements[m_iPos].iChild;
	if( iPos == 0 ) return XMarkupNode();
	return XMarkupNode(m_pOwner, iPos);
}

XMarkupNode XMarkupNode::GetChild(LPCTSTR pstrName)
{
	if( m_pOwner == NULL ) return XMarkupNode();
	ULONG iPos = m_pOwner->m_pElements[m_iPos].iChild;
	while( iPos != 0 ) {
		if( _tcsicmp(m_pOwner->m_pstrXML + m_pOwner->m_pElements[iPos].iStart, pstrName) == 0 ) {
			return XMarkupNode(m_pOwner, iPos);
		}
		iPos = m_pOwner->m_pElements[iPos].iNext;
	}
	return XMarkupNode();
}

bool XMarkupNode::HasChildren() const
{
	if( m_pOwner == NULL ) return false;
	return m_pOwner->m_pElements[m_iPos].iChild != 0;
}

XMarkupNode XMarkupNode::GetParent()
{
	if( m_pOwner == NULL ) return XMarkupNode();
	ULONG iPos = m_pOwner->m_pElements[m_iPos].iParent;
	if( iPos == 0 ) return XMarkupNode();
	return XMarkupNode(m_pOwner, iPos);
}

int XMarkupNode::GetLevel()
{
	int lv=0;
	XMarkupNode node = *this;
	while(node.IsValid()){
		node = node.GetParent();
		lv++;
	}
	return lv;
}

bool XMarkupNode::IsValid() const
{
	return m_pOwner != NULL && m_iPos<m_pOwner->m_nElements;
}

LPCTSTR XMarkupNode::GetName() const
{
	if( m_pOwner == NULL ) return NULL;
	return m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iStart;
}

XMarkupParser::XMLELEMENT XMarkupNode::GetTagElement() const
{
	return m_pOwner->m_pElements[m_iPos];
}

LPCTSTR XMarkupNode::GetValue() const
{
	if( m_pOwner == NULL ) return NULL;
	return m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iData;
}

LPCTSTR XMarkupNode::GetAttributeName(int iIndex)
{
	if( m_pOwner == NULL ) return NULL;
	if( m_nAttributes == 0 ) _MapAttributes();
	if( iIndex < 0 || iIndex >= m_nAttributes ) return _T("");
	return m_pOwner->m_pstrXML + m_aAttributes[iIndex].iName;
}

LPCTSTR XMarkupNode::GetAttributeValue(int iIndex)
{
	if( m_pOwner == NULL ) return NULL;
	if( m_nAttributes == 0 ) _MapAttributes();
	if( iIndex < 0 || iIndex >= m_nAttributes ) return _T("");
	return m_pOwner->m_pstrXML + m_aAttributes[iIndex].iValue;
}

LPCTSTR XMarkupNode::GetAttributeValue(LPCTSTR pstrName)
{
	if( m_pOwner == NULL ) return NULL;
	if( m_nAttributes == 0 ) _MapAttributes();
	for( int i = 0; i < m_nAttributes; i++ ) {
		if( _tcsicmp(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName) == 0 ) return m_pOwner->m_pstrXML + m_aAttributes[i].iValue;
	}
	return _T("");
}

bool XMarkupNode::GetAttributeValue(int iIndex, LPTSTR pstrValue, SIZE_T cchMax)
{
	if( m_pOwner == NULL ) return false;
	if( m_nAttributes == 0 ) _MapAttributes();
	if( iIndex < 0 || iIndex >= m_nAttributes ) return false;
	_tcsncpy(pstrValue, m_pOwner->m_pstrXML + m_aAttributes[iIndex].iValue, cchMax);
	return true;
}

bool XMarkupNode::GetAttributeValue(LPCTSTR pstrName, LPTSTR pstrValue, SIZE_T cchMax)
{
	if( m_pOwner == NULL ) return false;
	if( m_nAttributes == 0 ) _MapAttributes();
	for( int i = 0; i < m_nAttributes; i++ ) {
		if( _tcsicmp(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName) == 0 ) {
			_tcsncpy(pstrValue, m_pOwner->m_pstrXML + m_aAttributes[i].iValue, cchMax);
			return true;
		}
	}
	return false;
}

int XMarkupNode::GetAttributeCount()
{
	if( m_pOwner == NULL ) return 0;
	if( m_nAttributes == 0 ) _MapAttributes();
	return m_nAttributes;
}

bool XMarkupNode::HasAttributes()
{
	if( m_pOwner == NULL ) return false;
	if( m_nAttributes == 0 ) _MapAttributes();
	return m_nAttributes > 0;
}

bool XMarkupNode::HasAttribute(LPCTSTR pstrName)
{
	if( m_pOwner == NULL ) return false;
	if( m_nAttributes == 0 ) _MapAttributes();
	for( int i = 0; i < m_nAttributes; i++ ) {
		if( _tcsicmp(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName) == 0 ) return true;
	}
	return false;
}

void XMarkupNode::_MapAttributes()
{
	m_nAttributes = 0;
	LPCTSTR pstr = m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iStart;
	LPCTSTR pstrEnd = m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iData;
	pstr += _tcslen(pstr) + 1;
	while( pstr < pstrEnd ) {
		m_pOwner->_SkipWhitespace(pstr);
		m_aAttributes[m_nAttributes].iName = pstr - m_pOwner->m_pstrXML;
		pstr += _tcslen(pstr) + 1;
		m_pOwner->_SkipWhitespace(pstr);
		if( *pstr++ != _T('\"') ) return; // if( *pstr != _T('\"') ) { pstr = ::CharNext(pstr); return; }
		
		m_aAttributes[m_nAttributes++].iValue = pstr - m_pOwner->m_pstrXML;
		if( m_nAttributes >= MAX_XML_ATTRIBUTES ) return;
		pstr += _tcslen(pstr) + 1;
	}
}


///////////////////////////////////////////////////////////////////////////////////////
//
//
//

XMarkupParser::XMarkupParser(LPCTSTR pstrXML)
{
	m_pstrXML = NULL;
	m_pElements = NULL;
	m_nElements = 0;
	m_bPreserveWhitespace = true;
	if( pstrXML != NULL ) Load(pstrXML);
}

XMarkupParser::~XMarkupParser()
{
	_bIsDebug = false;
	Release();
}

bool XMarkupParser::IsValid() const
{
	return m_pElements != NULL;
}

void XMarkupParser::SetPreserveWhitespace(bool bPreserve)
{
	m_bPreserveWhitespace = bPreserve;
}

bool XMarkupParser::Load(LPCTSTR pstrXML, bool copy)
{
	Release();
	SIZE_T cchLen = _tcslen(pstrXML) + 1;
	if (_bShouldDeleteData=copy)
	{
		m_pstrXML = static_cast<LPTSTR>(malloc(cchLen * sizeof(TCHAR)));
		::CopyMemory(m_pstrXML, pstrXML, cchLen * sizeof(TCHAR));
	}
	else {
		m_pstrXML = (LPTSTR)pstrXML;
	}
	bool bRes = _Parse();
	if( !bRes ) Release();
	return bRes;
}

bool XMarkupParser::LoadFromMem(BYTE* pByte, DWORD dwSize, int encoding)
{
	auto lastData = m_pstrXML;
#ifdef _UNICODE
	if (encoding == XMLFILE_ENCODING_UTF8)
	{
		if( dwSize >= 3 && pByte[0] == 0xEF && pByte[1] == 0xBB && pByte[2] == 0xBF ) 
		{
			pByte += 3; dwSize -= 3;
		}
		DWORD nWide = ::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pByte, dwSize, NULL, 0 );

		m_pstrXML = static_cast<LPTSTR>(malloc((nWide + 1)*sizeof(TCHAR)));
		::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pByte, dwSize, m_pstrXML, nWide );
		m_pstrXML[nWide] = _T('\0');
	}
	else if (encoding == XMLFILE_ENCODING_ASNI)
	{
		DWORD nWide = ::MultiByteToWideChar( CP_ACP, 0, (LPCSTR)pByte, dwSize, NULL, 0 );

		m_pstrXML = static_cast<LPTSTR>(malloc((nWide + 1)*sizeof(TCHAR)));
		::MultiByteToWideChar( CP_ACP, 0, (LPCSTR)pByte, dwSize, m_pstrXML, nWide );
		m_pstrXML[nWide] = _T('\0');
	}
	else
	{
		if ( dwSize >= 2 && ( ( pByte[0] == 0xFE && pByte[1] == 0xFF ) || ( pByte[0] == 0xFF && pByte[1] == 0xFE ) ) )
		{
			dwSize = dwSize / 2 - 1;

			if ( pByte[0] == 0xFE && pByte[1] == 0xFF )
			{
				pByte += 2;

				for ( DWORD nSwap = 0 ; nSwap < dwSize ; nSwap ++ )
				{
					register CHAR nTemp = pByte[ ( nSwap << 1 ) + 0 ];
					pByte[ ( nSwap << 1 ) + 0 ] = pByte[ ( nSwap << 1 ) + 1 ];
					pByte[ ( nSwap << 1 ) + 1 ] = nTemp;
				}
			}
			else
			{
				pByte += 2;
			}

			m_pstrXML = static_cast<LPTSTR>(malloc((dwSize + 1)*sizeof(TCHAR)));
			::CopyMemory( m_pstrXML, pByte, dwSize * sizeof(TCHAR) );
			m_pstrXML[dwSize] = _T('\0');

			pByte -= 2;
		}
	}
#else // !_UNICODE
	if (encoding == XMLFILE_ENCODING_UTF8)
	{
		if( dwSize >= 3 && pByte[0] == 0xEF && pByte[1] == 0xBB && pByte[2] == 0xBF ) 
		{
			pByte += 3; dwSize -= 3;
		}
		DWORD nWide = ::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pByte, dwSize, NULL, 0 );

		LPWSTR w_str = static_cast<LPWSTR>(malloc((nWide + 1)*sizeof(WCHAR)));
		::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pByte, dwSize, w_str, nWide );
		w_str[nWide] = L'\0';

		DWORD wide = ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)w_str, nWide, NULL, 0, NULL, NULL);

		m_pstrXML = static_cast<LPTSTR>(malloc((wide + 1)*sizeof(TCHAR)));
		::WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)w_str, nWide, m_pstrXML, wide, NULL, NULL);
		m_pstrXML[wide] = _T('\0');

		free(w_str);
	}
	else if (encoding == XMLFILE_ENCODING_UNICODE)
	{
		if ( dwSize >= 2 && ( ( pByte[0] == 0xFE && pByte[1] == 0xFF ) || ( pByte[0] == 0xFF && pByte[1] == 0xFE ) ) )
		{
			dwSize = dwSize / 2 - 1;

			if ( pByte[0] == 0xFE && pByte[1] == 0xFF )
			{
				pByte += 2;

				for ( DWORD nSwap = 0 ; nSwap < dwSize ; nSwap ++ )
				{
					register CHAR nTemp = pByte[ ( nSwap << 1 ) + 0 ];
					pByte[ ( nSwap << 1 ) + 0 ] = pByte[ ( nSwap << 1 ) + 1 ];
					pByte[ ( nSwap << 1 ) + 1 ] = nTemp;
				}
			}
			else
			{
				pByte += 2;
			}

			DWORD nWide = ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pByte, dwSize, NULL, 0, NULL, NULL);
			m_pstrXML = static_cast<LPTSTR>(malloc((nWide + 1)*sizeof(TCHAR)));
			::WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)pByte, dwSize, m_pstrXML, nWide, NULL, NULL);
			m_pstrXML[nWide] = _T('\0');

			pByte -= 2;
		}
	}
	else
	{
		m_pstrXML = static_cast<LPTSTR>(malloc((dwSize + 1)*sizeof(TCHAR)));
		::CopyMemory( m_pstrXML, pByte, dwSize * sizeof(TCHAR) );
		m_pstrXML[dwSize] = _T('\0');
	}
#endif // _UNICODE
	if (lastData!=m_pstrXML)
	{
		_bShouldDeleteData = true;
	}
	bool bRes = _Parse();
	if( !bRes ) Release();
	return bRes;
}

bool XMarkupParser::LoadFromFile(LPCTSTR pstrFilename, int encoding)
{
	Release();
	QkString sFile = pstrFilename;
	if( CPaintManagerUI::GetResourceZip().IsEmpty() ) {
		if (sFile.GetLength()<3 || sFile[1]!=':' && sFile[1]!='/')
		{
			sFile.Prepend(CPaintManagerUI::GetResourcePath());
		}
		//sFile += pstrFilename;
		HANDLE hFile = ::CreateFile(sFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if( hFile == INVALID_HANDLE_VALUE ) return _Failed(_T("Error opening file"));
		DWORD dwSize = ::GetFileSize(hFile, NULL);
		TCHAR* err_code = 0;
		if( dwSize == 0 ) err_code=_T("File is empty");
		else if ( dwSize > 4096*1024 ) err_code=_T("File too large");
		if( err_code ) { ::CloseHandle( hFile ); return _Failed(err_code); }

		DWORD dwRead = 0;
		BYTE* pByte = new BYTE[ dwSize ];
		::ReadFile( hFile, pByte, dwSize, &dwRead, NULL );
		::CloseHandle( hFile );
		if( dwRead != dwSize ) {
			delete[] pByte;
			pByte = NULL;
			Release();
			return _Failed(_T("Could not read file"));
		}

		bool ret = LoadFromMem(pByte, dwSize, encoding);
		delete[] pByte;
		pByte = NULL;

		return ret;
	}
	else {
		sFile += CPaintManagerUI::GetResourceZip();
		HZIP hz = NULL;
		if( CPaintManagerUI::IsCachedResourceZip() ) hz = (HZIP)CPaintManagerUI::GetResourceZipHandle();
		else {
			QkString sFilePwd = CPaintManagerUI::GetResourceZipPwd();
#ifdef UNICODE
			char* pwd = w2a((wchar_t*)sFilePwd.GetData());
			hz = OpenZip(sFile.GetData(), pwd);
			if(pwd) delete[] pwd;
#else
			hz = OpenZip(sFile.GetData(), sFilePwd.GetData());
#endif
		}
		if( hz == NULL ) return _Failed(_T("Error opening zip file"));
		ZIPENTRY ze; 
		int i = 0; 
		QkString key = pstrFilename;
		key.Replace(_T("\\"), _T("/"));
		if( FindZipItem(hz, key, true, &i, &ze) != 0 ) return _Failed(_T("Could not find ziped file"));
		DWORD dwSize = ze.unc_size;

		TCHAR* err_code = 0;
		if( dwSize == 0 ) err_code=_T("File is empty");
		else if ( dwSize > 4096*1024 ) err_code=_T("File too large");
		if( err_code ) { if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz); return _Failed(err_code); }

		BYTE* pByte = new BYTE[ dwSize ];
		int res = UnzipItem(hz, i, pByte, dwSize);
		if( res != 0x00000000 && res != 0x00000600) {
			delete[] pByte;
			if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz);
			return _Failed(_T("Could not unzip file"));
		}
		if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz);
		bool ret = LoadFromMem(pByte, dwSize, encoding);
		delete[] pByte;
		pByte = NULL;
		return ret;
	}
}

void printNodes(XMarkupNode & node)
{
	if (node.IsValid())
	{
		::OutputDebugStringA("printNodes:: ");
		for (size_t i = 0; i < node.GetLevel(); i++)
		{
			::OutputDebugStringA("-");
		}
		::OutputDebugString(node.GetName());
		::OutputDebugStringA("\n");
		XMarkupNode cv = node.GetChild();
		while (cv.IsValid())
		{
			printNodes(cv);
			cv = cv.GetSibling();
		}
	}
}

void XMarkupParser::Print()
{
	XMarkupNode rootNode = GetRoot();
	printNodes(rootNode);
}

void XMarkupParser::Release()
{
	if( m_pstrXML != NULL && _bShouldDeleteData ) {
		free(m_pstrXML);
		m_pstrXML = NULL;
	}
	if (!_bIsDebug)
	{
		// release memory
		if( m_pElements != NULL ) {
			free(m_pElements);
			m_pElements = NULL;
		}
	}
	m_nElements = 0;
}

const QkString & XMarkupParser::GetLastErrorMessage() const
{
	return _ErrorMsg;
}

const QkString & XMarkupParser::GetLastErrorLocation() const
{
	return _ErrorXML;
}

XMarkupNode XMarkupParser::GetRoot()
{
	if( m_nElements == 0 ) return XMarkupNode();
	return XMarkupNode(this, 1);
}

XMarkupNode XMarkupParser::GetNodeAt(int pos)
{
	if( m_nElements == 0 ) return XMarkupNode();
	return XMarkupNode(this, pos);
}

bool XMarkupParser::_Parse()
{
	_ReserveElement(); // Reserve index 0 for errors
	_ErrorMsg.Empty();
	_ErrorXML.Empty();
	LPTSTR pstrXML = m_pstrXML;
	return _Parse(pstrXML, 0);
}

bool XMarkupParser::_Parse(LPTSTR& pstrText, ULONG iParent)
{
	_SkipWhitespace(pstrText);
	ULONG iPrevious = 0;
	for( ; ; ) 
	{
		if( *pstrText == _T('\0') && iParent <= 1 ) return true;
		_SkipWhitespace(pstrText);
		if( *pstrText != _T('<') ) return _Failed(_T("Expected start tag"), pstrText);
		if( pstrText[1] == _T('/') ) return true;
		*pstrText++ = _T('\0');
		_SkipWhitespace(pstrText);
		// Skip comment or processing directive
		if( *pstrText == _T('!') || *pstrText == _T('?') ) {
			TCHAR ch = *pstrText;
			if( *pstrText == _T('!') ) ch = _T('-');
			while( *pstrText != _T('\0') && !(*pstrText == ch && *(pstrText + 1) == _T('>')) ) pstrText = ::CharNext(pstrText);
			if( *pstrText != _T('\0') ) pstrText += 2;
			_SkipWhitespace(pstrText);
			continue;
		}
		_SkipWhitespace(pstrText);
		// Fill out element structure
		XMLELEMENT* pEl = _ReserveElement();
		ULONG iPos = pEl - m_pElements;
		pEl->iStart = pstrText - m_pstrXML;
		pEl->iParent = iParent;
		pEl->iNext = pEl->iChild = 0;
		if( iPrevious != 0 ) m_pElements[iPrevious].iNext = iPos;
		else if( iParent > 0 ) m_pElements[iParent].iChild = iPos;
		iPrevious = iPos;
		// Parse name
		LPCTSTR pstrName = pstrText;
		_SkipIdentifier(pstrText);
		LPTSTR pstrNameEnd = pstrText;
		if( *pstrText == _T('\0') ) return _Failed(_T("Error parsing element name"), pstrText);
		// Parse attributes
		if( !_ParseAttributes(pstrText) ) return false;
		_SkipWhitespace(pstrText);
		if( pstrText[0] == _T('/') && pstrText[1] == _T('>') )
		{
			pEl->iData = pstrText - m_pstrXML;
			*pstrText = _T('\0');
			pstrText += 2;
		}
		else
		{
			if( *pstrText != _T('>') ) return _Failed(_T("Expected start-tag closing"), pstrText);
			// Parse node data
			pEl->iData = ++pstrText - m_pstrXML;
			LPTSTR pstrDest = pstrText;
			if( !_ParseData(pstrText, pstrDest, _T('<')) ) return false;
			// Determine type of next element
			if( *pstrText == _T('\0') && iParent <= 1 ) return true;
			if( *pstrText != _T('<') ) return _Failed(_T("Expected end-tag start"), pstrText);
			if( pstrText[0] == _T('<') && pstrText[1] != _T('/') ) 
			{
				if( !_Parse(pstrText, iPos) ) return false;
			}
			if( pstrText[0] == _T('<') && pstrText[1] == _T('/') ) 
			{
				*pstrDest = _T('\0');
				*pstrText = _T('\0');
				pstrText += 2;
				_SkipWhitespace(pstrText);
				SIZE_T cchName = pstrNameEnd - pstrName;
				if( _tcsncmp(pstrText, pstrName, cchName) != 0 ) return _Failed(_T("Unmatched closing tag"), pstrText);
				pstrText += cchName;
				_SkipWhitespace(pstrText);
				if( *pstrText++ != _T('>') ) return _Failed(_T("Unmatched closing tag"), pstrText);
			}
		}
		*pstrNameEnd = _T('\0');
		_SkipWhitespace(pstrText);
	}
}

XMarkupParser::XMLELEMENT* XMarkupParser::_ReserveElement()
{
	if( m_nElements == 0 ) m_nReservedElements = 0;
	if( m_nElements >= m_nReservedElements ) {
		m_nReservedElements += (m_nReservedElements / 2) + 500;
		m_pElements = static_cast<XMLELEMENT*>(realloc(m_pElements, m_nReservedElements * sizeof(XMLELEMENT)));
	}
	return &m_pElements[m_nElements++];
}

void XMarkupParser::_SkipWhitespace(LPCTSTR& pstr) const
{
	while( *pstr > _T('\0') && *pstr <= _T(' ') ) pstr = ::CharNext(pstr);
}

void XMarkupParser::_SkipWhitespace(LPTSTR& pstr) const
{
	while( *pstr > _T('\0') && *pstr <= _T(' ') ) pstr = ::CharNext(pstr);
}

void XMarkupParser::_SkipIdentifier(LPCTSTR& pstr) const
{
	// 属性只能用英文，所以这样处理没有问题
	while( *pstr != _T('\0') && (*pstr == _T('_') || *pstr == _T(':') || _istalnum(*pstr)) ) pstr = ::CharNext(pstr);
}

void XMarkupParser::_SkipIdentifier(LPTSTR& pstr) const
{
	// 属性只能用英文，所以这样处理没有问题
	while( *pstr != _T('\0') && (*pstr == _T('_') || *pstr == _T(':') || _istalnum(*pstr)) ) pstr = ::CharNext(pstr);
}

bool XMarkupParser::_ParseAttributes(LPTSTR& pstrText)
{   
	// 无属性
	LPTSTR pstrIdentifier = pstrText;
	if( *pstrIdentifier == _T('/') && *++pstrIdentifier == _T('>') ) return true;
	if( *pstrText == _T('>') ) return true;
	*pstrText++ = _T('\0');
	_SkipWhitespace(pstrText);
	while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('/') ) {
		_SkipIdentifier(pstrText);
		LPTSTR pstrIdentifierEnd = pstrText;
		_SkipWhitespace(pstrText);
		if( *pstrText != _T('=') ) return _Failed(_T("Error while parsing attributes"), pstrText);
		*pstrText++ = _T(' ');
		*pstrIdentifierEnd = _T('\0');
		_SkipWhitespace(pstrText);
		if( *pstrText++ != _T('\"') ) return _Failed(_T("Expected attribute value"), pstrText);
		LPTSTR pstrDest = pstrText;
		if( !_ParseData(pstrText, pstrDest, _T('\"')) ) return false;
		if( *pstrText == _T('\0') ) return _Failed(_T("Error while parsing attribute string"), pstrText);
		*pstrDest = _T('\0');
		if( pstrText != pstrDest ) *pstrText = _T(' ');
		pstrText++;
		_SkipWhitespace(pstrText);
	}
	return true;
}

bool XMarkupParser::_ParseData(LPTSTR& pstrText, LPTSTR& pstrDest, char cEnd)
{
	while( *pstrText != _T('\0') && *pstrText != cEnd ) {
		if( *pstrText == _T('&') ) {
			while( *pstrText == _T('&') ) {
				_ParseMetaChar(++pstrText, pstrDest);
			}
			if (*pstrText == cEnd)
				break;
		}

		if( *pstrText == _T(' ') ) {
			*pstrDest++ = *pstrText++;
			if( !m_bPreserveWhitespace ) _SkipWhitespace(pstrText);
		}
		else {
			LPTSTR pstrTemp = ::CharNext(pstrText);
			while( pstrText < pstrTemp) {
				*pstrDest++ = *pstrText++;
			}
		}
	}
	// Make sure that MapAttributes() works correctly when it parses
	// over a value that has been transformed.
	LPTSTR pstrFill = pstrDest + 1;
	while( pstrFill < pstrText ) *pstrFill++ = _T(' ');
	return true;
}

void XMarkupParser::_ParseMetaChar(LPTSTR& pstrText, LPTSTR& pstrDest)
{
	if( pstrText[0] == _T('a') && pstrText[1] == _T('m') && pstrText[2] == _T('p') && pstrText[3] == _T(';') ) {
		*pstrDest++ = _T('&');
		pstrText += 4;
	}
	else if( pstrText[0] == _T('l') && pstrText[1] == _T('t') && pstrText[2] == _T(';') ) {
		*pstrDest++ = _T('<');
		pstrText += 3;
	}
	else if( pstrText[0] == _T('g') && pstrText[1] == _T('t') && pstrText[2] == _T(';') ) {
		*pstrDest++ = _T('>');
		pstrText += 3;
	}
	else if( pstrText[0] == _T('q') && pstrText[1] == _T('u') && pstrText[2] == _T('o') && pstrText[3] == _T('t') && pstrText[4] == _T(';') ) {
		*pstrDest++ = _T('\"');
		pstrText += 5;
	}
	else if( pstrText[0] == _T('a') && pstrText[1] == _T('p') && pstrText[2] == _T('o') && pstrText[3] == _T('s') && pstrText[4] == _T(';') ) {
		*pstrDest++ = _T('\'');
		pstrText += 5;
	}
	else {
		*pstrDest++ = _T('&');
	}
}

bool XMarkupParser::_Failed(LPCTSTR pstrError, LPCTSTR pstrLocation)
{
	// Register last error
	TRACE(_T("XML Error: %s"), pstrError);
	if( pstrLocation != NULL ) TRACE(pstrLocation);
	_ErrorMsg = pstrError;
	_ErrorXML = pstrLocation;
	return false; // Always return 'false'
}

} // namespace DuiLib
