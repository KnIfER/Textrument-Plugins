#ifndef __UIMARKUP_H__
#define __UIMARKUP_H__

#pragma once

namespace DuiLib {

	enum
	{
		XMLFILE_ENCODING_UTF8 = 0,
		XMLFILE_ENCODING_UNICODE = 1,
		XMLFILE_ENCODING_ASNI = 2,
	};

	class XMarkupParser;
	class XMarkupNode;

	class UILIB_API XMarkupParser
	{
		friend class XMarkupNode;
	public:
		XMarkupParser(LPCTSTR pstrXML = NULL);
		~XMarkupParser();

		bool Load(LPCTSTR pstrXML, bool copy=false);
		bool LoadFromMem(BYTE* pByte, DWORD dwSize, int encoding = XMLFILE_ENCODING_UTF8);
		bool LoadFromFile(LPCTSTR pstrFilename, int encoding = XMLFILE_ENCODING_UTF8);
		void Release();
		bool IsValid() const;

		void Print();

		void SetPreserveWhitespace(bool bPreserve = true);
		const QkString & GetLastErrorMessage() const;
		const QkString & GetLastErrorLocation() const;

		XMarkupNode GetRoot();

		XMarkupNode GetNodeAt(int pos);

		typedef struct tagXMLELEMENT
		{
			ULONG iStart;
			ULONG iChild;
			ULONG iNext;
			ULONG iParent;
			ULONG iData;
		} XMLELEMENT;

		void SetDebug(){ _bIsDebug = true; };

	private:

		LPTSTR m_pstrXML;
		XMLELEMENT* m_pElements;
		ULONG m_nElements;
		ULONG m_nReservedElements;
		bool m_bPreserveWhitespace;

		QkString _ErrorMsg;
		QkString _ErrorXML;

		bool _bIsDebug;
		bool _bShouldDeleteData;

	private:
		// 将XML布局分析如全局容器数组
		bool _Parse();
		// 将XML布局分析如全局容器数组（递归）
		bool _Parse(LPTSTR& pstrText, ULONG iParent);
		XMLELEMENT* _ReserveElement();
		inline void _SkipWhitespace(LPTSTR& pstr) const;
		inline void _SkipWhitespace(LPCTSTR& pstr) const;
		inline void _SkipIdentifier(LPTSTR& pstr) const;
		inline void _SkipIdentifier(LPCTSTR& pstr) const;
		bool _ParseData(LPTSTR& pstrText, LPTSTR& pstrData, char cEnd);
		void _ParseMetaChar(LPTSTR& pstrText, LPTSTR& pstrDest);
		bool _ParseAttributes(LPTSTR& pstrText);
		bool _Failed(LPCTSTR pstrError, LPCTSTR pstrLocation = NULL);
	};


	class UILIB_API XMarkupNode
	{
		friend class XMarkupParser;

	public:
		XMarkupNode();
		XMarkupNode(XMarkupParser* pOwner, int iPos);

		bool IsValid() const;

		// 获取父节点
		XMarkupNode GetParent();

		// 获取节点深度
		int GetLevel();

		// 获取索引，指向分析下来的节点内容
		int XMarkupNode::GetMarkedPos()
		{
			return m_iPos;
		}

		XMarkupNode GetSibling();
		XMarkupNode GetChild();
		XMarkupNode GetChild(LPCTSTR pstrName);

		bool HasSiblings() const;
		bool HasChildren() const;
		bool HasParent() const;
		LPCTSTR GetName() const;
		LPCTSTR GetValue() const;

		XMarkupParser::XMLELEMENT GetTagElement() const;

		bool HasAttributes();
		bool HasAttribute(LPCTSTR pstrName);
		int GetAttributeCount();
		LPCTSTR GetAttributeName(int iIndex);
		LPCTSTR GetAttributeValue(int iIndex);
		LPCTSTR GetAttributeValue(LPCTSTR pstrName);
		bool GetAttributeValue(int iIndex, LPTSTR pstrValue, SIZE_T cchMax);
		bool GetAttributeValue(LPCTSTR pstrName, LPTSTR pstrValue, SIZE_T cchMax);

		XMarkupParser* GetMarkUp() const {
			return IsValid()?m_pOwner:NULL;
		};

	private:
		void _MapAttributes();

		enum { MAX_XML_ATTRIBUTES = 64 };

		typedef struct
		{
			ULONG iName;
			ULONG iValue;
		} XMLATTRIBUTE;

		int m_iPos;
		int m_nAttributes;

		// 内存
		XMLATTRIBUTE m_aAttributes[MAX_XML_ATTRIBUTES];

		// xml 全局容器
		XMarkupParser* m_pOwner;
	};

} // namespace DuiLib

#endif // __UIMARKUP_H__
