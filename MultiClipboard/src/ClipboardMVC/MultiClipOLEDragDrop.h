/*
 This file is copied and adapted from
 OLE Drag and Drop Tutorial - http://www.catch22.net/node/34
*/
/*
This file is part of MultiClipboard Plugin for Notepad++
Copyright (C) 2010 LoonyChewy

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

#ifndef MULTI_CLIP_OLE_DRAG_DROP
#define MULTI_CLIP_OLE_DRAG_DROP


#ifndef UNITY_BUILD_SINGLE_INCLUDE
#include <windows.h>
#endif


class MultiClipOLEDataObject : public IDataObject
{
public:
	// IUnknown members
	HRESULT __stdcall QueryInterface( REFIID iid, void ** ppvObject );
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	// IDataObject members
	HRESULT __stdcall GetData( FORMATETC * pFormatEtc, STGMEDIUM * pmedium );
	HRESULT __stdcall GetDataHere( FORMATETC * pFormatEtc, STGMEDIUM * pmedium );
	HRESULT __stdcall QueryGetData( FORMATETC * pFormatEtc );
	HRESULT __stdcall GetCanonicalFormatEtc( FORMATETC * pFormatEct, FORMATETC * pFormatEtcOut );
	HRESULT __stdcall SetData( FORMATETC * pFormatEtc, STGMEDIUM * pMedium, BOOL fRelease );
	HRESULT __stdcall EnumFormatEtc( DWORD dwDirection, IEnumFORMATETC ** ppEnumFormatEtc );
	HRESULT __stdcall DAdvise( FORMATETC * pFormatEtc, DWORD advf, IAdviseSink *, DWORD * );
	HRESULT __stdcall DUnadvise( DWORD dwConnection );
	HRESULT __stdcall EnumDAdvise( IEnumSTATDATA **ppEnumAdvise );

	// Constructor / Destructor
	MultiClipOLEDataObject();
	~MultiClipOLEDataObject();
	void SetMultiClipDragData( const void * pData, unsigned int numberOfBytes, bool isColumnMode );

	static HRESULT CreateDataObject( MultiClipOLEDataObject ** ppDataObject );
	static HGLOBAL DuplicateMemory( HGLOBAL hMem );

private:
	// any private members and functions
	LONG m_lRefCount;
	FORMATETC m_FormatEtc[2];
	STGMEDIUM m_StgMedium;
	LONG m_nNumFormats;
	int LookupFormatEtc( FORMATETC * pFormatEtc );
};


class MultiClipOLEEnumFormatEtc : public IEnumFORMATETC
{
public:
	// IUnknown members
	HRESULT __stdcall QueryInterface( REFIID iid, void ** ppvObject );
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	// IEnumFormatEtc members
	HRESULT __stdcall Next( ULONG celt, FORMATETC * rgelt, ULONG * pceltFetched );
	HRESULT __stdcall Skip( ULONG celt ); 
	HRESULT __stdcall Reset();
	HRESULT __stdcall Clone( IEnumFORMATETC ** ppEnumFormatEtc );

	// Construction / Destruction
	MultiClipOLEEnumFormatEtc( FORMATETC * pFormatEtc, int nNumFormats );
	~MultiClipOLEEnumFormatEtc();

	static HRESULT CreateEnumFormatEtc( UINT nNumFormats, FORMATETC * pFormatEtc, IEnumFORMATETC ** ppEnumFormatEtc );
	static void DeepCopyFormatEtc( FORMATETC * dest, FORMATETC * source );

private:

	LONG m_lRefCount;			// Reference count for this COM interface
	ULONG m_nIndex;				// current enumerator index
	ULONG m_nNumFormats;		// number of FORMATETC members
	FORMATETC * m_pFormatEtc;	// array of FORMATETC objects
};


class MultiClipOLEDropSource : public IDropSource
{
public:
	// IUnknown members
	HRESULT __stdcall QueryInterface( REFIID iid, void ** ppvObject );
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	// IDropSource members
	HRESULT __stdcall QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState );
	HRESULT __stdcall GiveFeedback( DWORD dwEffect );

	// Constructor / Destructor
	MultiClipOLEDropSource();
	~MultiClipOLEDropSource();

	static HRESULT CreateDropSource( MultiClipOLEDropSource ** ppDropSource );

private:
	// private members and functions
	LONG m_lRefCount;
};


#endif