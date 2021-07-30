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

#ifndef UNITY_BUILD_SINGLE_INCLUDE
#include "MultiClipOLEDragDrop.h"
#endif


MultiClipOLEDataObject::MultiClipOLEDataObject()
: m_lRefCount( 1 )
, m_nNumFormats( 1 )
{
	::ZeroMemory( &m_StgMedium, sizeof(STGMEDIUM) );
	m_StgMedium.tymed = TYMED_HGLOBAL;

	m_FormatEtc[0].cfFormat = CF_UNICODETEXT;
	m_FormatEtc[1].cfFormat = ::RegisterClipboardFormat( TEXT("MSDEVColumnSelect") );
	for ( int i = 0; i < 2; i++ )
	{
		m_FormatEtc[i].ptd = NULL;
		m_FormatEtc[i].dwAspect = DVASPECT_CONTENT;
		m_FormatEtc[i].lindex = -1;
		m_FormatEtc[i].tymed = TYMED_HGLOBAL;
	}
}


MultiClipOLEDataObject::~MultiClipOLEDataObject()
{
}



void MultiClipOLEDataObject::SetMultiClipDragData( const void * pData, unsigned int numberOfBytes, bool isColumnMode )
{
	if ( m_StgMedium.hGlobal != NULL )
	{
		::GlobalFree( m_StgMedium.hGlobal );
		m_StgMedium.hGlobal = NULL;
	}

	HGLOBAL hMem = ::GlobalAlloc( GHND, numberOfBytes );
	char * ptr = (char *) ::GlobalLock( hMem );
	// copy the text in the selected list box item
	::CopyMemory( ptr, pData, numberOfBytes );
	::GlobalUnlock( hMem );

	m_StgMedium.hGlobal = hMem;
	m_nNumFormats = isColumnMode ? 2 : 1;
}


ULONG __stdcall MultiClipOLEDataObject::AddRef()
{
	// increment object reference count
	return InterlockedIncrement( &m_lRefCount );
}


ULONG __stdcall MultiClipOLEDataObject::Release()
{
	// decrement object reference count
	LONG count = InterlockedDecrement( &m_lRefCount );

	if ( count == 0 )
	{
		delete this;
		return 0;
	}
	else
	{
		return count;
	}
}


HRESULT __stdcall MultiClipOLEDataObject::QueryInterface( REFIID iid, void ** ppvObject )
{
	// check to see what interface has been requested
	if ( iid == IID_IDataObject || iid == IID_IUnknown )
	{
		AddRef();
		*ppvObject = this;
		return S_OK;
	}
	else
	{
		*ppvObject = 0;
		return E_NOINTERFACE;
	}
}


int MultiClipOLEDataObject::LookupFormatEtc( FORMATETC * pFormatEtc )
{
	for ( int i = 0; i < m_nNumFormats; i++ )
	{
		if ( (pFormatEtc->tymed & m_FormatEtc[i].tymed) &&
			  pFormatEtc->cfFormat == m_FormatEtc[i].cfFormat &&
			  pFormatEtc->dwAspect == m_FormatEtc[i].dwAspect )
		{
			return i;
		}
	}
	return -1;
}


HGLOBAL MultiClipOLEDataObject::DuplicateMemory( HGLOBAL hMem )
{
	// lock the source memory object
	DWORD len = GlobalSize( hMem );
	PVOID source = GlobalLock( hMem );

	// create a fixed "global" block - i.e. just
	// a regular lump of our process heap
	PVOID dest = GlobalAlloc( GMEM_FIXED, len );
	CopyMemory( dest, source, len );

	GlobalUnlock(hMem);

	return dest;
}


HRESULT __stdcall MultiClipOLEDataObject::GetData( FORMATETC * pFormatEtc, STGMEDIUM * pMedium )
{
	int idx;

	// try to match the requested FORMATETC with one of our supported formats
	if ( ( idx = LookupFormatEtc( pFormatEtc ) ) == -1 )
	{
		return DV_E_FORMATETC;
	}

	// found a match! transfer the data into the supplied storage-medium
	pMedium->tymed = m_FormatEtc[idx].tymed;
	pMedium->pUnkForRelease = 0;

	switch ( m_FormatEtc[idx].tymed )
	{
	case TYMED_HGLOBAL:
		pMedium->hGlobal = DuplicateMemory( m_StgMedium.hGlobal );
		break;

	default:
		return DV_E_FORMATETC;
	}

	return S_OK;
}


HRESULT __stdcall MultiClipOLEDataObject::GetDataHere( FORMATETC * pFormatEtc, STGMEDIUM * pMedium )
{
	// GetDataHere is only required for IStream and IStorage mediums
	// It is an error to call GetDataHere for things like HGLOBAL and other clipboard formats
	//
	//	OleFlushClipboard 
	//
	return DATA_E_FORMATETC;
}


HRESULT __stdcall MultiClipOLEDataObject::QueryGetData( FORMATETC * pFormatEtc )
{
	return ( LookupFormatEtc( pFormatEtc ) == -1 ) ? DV_E_FORMATETC : S_OK;
}


HRESULT __stdcall MultiClipOLEDataObject::GetCanonicalFormatEtc( FORMATETC * pFormatEct, FORMATETC * pFormatEtcOut )
{
	// Apparently we have to set this field to NULL even though we don't do anything else
	pFormatEtcOut->ptd = NULL;
	return E_NOTIMPL;
}


HRESULT __stdcall MultiClipOLEDataObject::SetData( FORMATETC * pFormatEtc, STGMEDIUM * pMedium,  BOOL fRelease )
{
	return E_NOTIMPL;
}


HRESULT __stdcall MultiClipOLEDataObject::EnumFormatEtc( DWORD dwDirection, IEnumFORMATETC ** ppEnumFormatEtc )
{
	if ( dwDirection == DATADIR_GET )
	{
		// for Win2k+ you can use the SHCreateStdEnumFmtEtc API call, however
		// to support all Windows platforms we need to implement IEnumFormatEtc ourselves.
		return MultiClipOLEEnumFormatEtc::CreateEnumFormatEtc( m_nNumFormats, m_FormatEtc, ppEnumFormatEtc );
	}
	else
	{
		// the direction specified is not support for drag+drop
		return E_NOTIMPL;
	}
}


HRESULT __stdcall MultiClipOLEDataObject::DAdvise( FORMATETC * pFormatEtc, DWORD advf, IAdviseSink * pAdvSink, DWORD * pdwConnection )
{
	return OLE_E_ADVISENOTSUPPORTED;
}


HRESULT __stdcall MultiClipOLEDataObject::DUnadvise( DWORD dwConnection )
{
	return OLE_E_ADVISENOTSUPPORTED;
}


HRESULT __stdcall MultiClipOLEDataObject::EnumDAdvise( IEnumSTATDATA ** ppEnumAdvise )
{
	return OLE_E_ADVISENOTSUPPORTED;
}


HRESULT MultiClipOLEDataObject::CreateDataObject( MultiClipOLEDataObject ** ppDataObject )
{
	if ( ppDataObject == 0 )
	{
		return E_INVALIDARG;
	}

	*ppDataObject = new MultiClipOLEDataObject();

	return (*ppDataObject) ? S_OK : E_OUTOFMEMORY;
}


HRESULT MultiClipOLEEnumFormatEtc::CreateEnumFormatEtc( UINT nNumFormats, FORMATETC * pFormatEtc, IEnumFORMATETC ** ppEnumFormatEtc )
{
	if ( nNumFormats == 0 || pFormatEtc == 0 || ppEnumFormatEtc == 0 )
	{
		return E_INVALIDARG;
	}

	*ppEnumFormatEtc = new MultiClipOLEEnumFormatEtc( pFormatEtc, nNumFormats );

	return ( *ppEnumFormatEtc ) ? S_OK : E_OUTOFMEMORY;
}


void MultiClipOLEEnumFormatEtc::DeepCopyFormatEtc( FORMATETC * dest, FORMATETC * source )
{
	// copy the source FORMATETC into dest
	*dest = *source;

	if ( source->ptd )
	{
		// allocate memory for the DVTARGETDEVICE if necessary
		dest->ptd = (DVTARGETDEVICE *) CoTaskMemAlloc( sizeof( DVTARGETDEVICE ) );

		// copy the contents of the source DVTARGETDEVICE into dest->ptd
		*(dest->ptd) = *(source->ptd);
	}
}


MultiClipOLEEnumFormatEtc::MultiClipOLEEnumFormatEtc( FORMATETC * pFormatEtc, int nNumFormats )
{
	m_lRefCount = 1;
	m_nIndex = 0;
	m_nNumFormats = nNumFormats;
	m_pFormatEtc = new FORMATETC[nNumFormats];

	// copy the FORMATETC structures
	for ( int i = 0; i < nNumFormats; i++ )
	{	
		DeepCopyFormatEtc( &m_pFormatEtc[i], &pFormatEtc[i] );
	}
}


MultiClipOLEEnumFormatEtc::~MultiClipOLEEnumFormatEtc()
{
	if ( m_pFormatEtc )
	{
		for ( ULONG i = 0; i < m_nNumFormats; i++ )
		{
			if ( m_pFormatEtc[i].ptd )
			{
				CoTaskMemFree( m_pFormatEtc[i].ptd );
			}
		}

		delete[] m_pFormatEtc;
	}
}


ULONG __stdcall MultiClipOLEEnumFormatEtc::AddRef()
{
	// increment object reference count
	return InterlockedIncrement( &m_lRefCount );
}


ULONG __stdcall MultiClipOLEEnumFormatEtc::Release()
{
	// decrement object reference count
	LONG count = InterlockedDecrement( &m_lRefCount );

	if ( count == 0 )
	{
		delete this;
		return 0;
	}
	else
	{
		return count;
	}
}


HRESULT __stdcall MultiClipOLEEnumFormatEtc::QueryInterface( REFIID iid, void ** ppvObject )
{
	// check to see what interface has been requested
	if ( iid == IID_IEnumFORMATETC || iid == IID_IUnknown )
	{
		AddRef();
		*ppvObject = this;
		return S_OK;
	}
	else
	{
		*ppvObject = 0;
		return E_NOINTERFACE;
	}
}


HRESULT __stdcall MultiClipOLEEnumFormatEtc::Next ( ULONG celt, FORMATETC * pFormatEtc, ULONG * pceltFetched )
{
	ULONG copied  = 0;

	// validate arguments
	if ( celt == 0 || pFormatEtc == 0 )
	{
		return E_INVALIDARG;
	}

	// copy FORMATETC structures into caller's buffer
	while ( m_nIndex < m_nNumFormats && copied < celt )
	{
		DeepCopyFormatEtc( &pFormatEtc[copied], &m_pFormatEtc[m_nIndex] );
		copied++;
		m_nIndex++;
	}

	// store result
	if ( pceltFetched != 0 )
	{
		*pceltFetched = copied;
	}

	// did we copy all that was requested?
	return (copied == celt) ? S_OK : S_FALSE;
}


HRESULT __stdcall MultiClipOLEEnumFormatEtc::Skip( ULONG celt )
{
	m_nIndex += celt;
	return (m_nIndex <= m_nNumFormats) ? S_OK : S_FALSE;
}


HRESULT __stdcall MultiClipOLEEnumFormatEtc::Reset()
{
	m_nIndex = 0;
	return S_OK;
}


HRESULT __stdcall MultiClipOLEEnumFormatEtc::Clone( IEnumFORMATETC ** ppEnumFormatEtc )
{
	HRESULT hResult;

	// make a duplicate enumerator
	hResult = CreateEnumFormatEtc( m_nNumFormats, m_pFormatEtc, ppEnumFormatEtc );

	if ( hResult == S_OK )
	{
		// manually set the index state
		((MultiClipOLEEnumFormatEtc *) *ppEnumFormatEtc)->m_nIndex = m_nIndex;
	}

	return hResult;
}


MultiClipOLEDropSource::MultiClipOLEDropSource() 
{
	m_lRefCount = 1;
}


MultiClipOLEDropSource::~MultiClipOLEDropSource()
{
}


ULONG __stdcall MultiClipOLEDropSource::AddRef()
{
	// increment object reference count
	return InterlockedIncrement( &m_lRefCount );
}


ULONG __stdcall MultiClipOLEDropSource::Release()
{
	// decrement object reference count
	LONG count = InterlockedDecrement( &m_lRefCount );

	if ( count == 0 )
	{
		delete this;
		return 0;
	}
	else
	{
		return count;
	}
}


HRESULT __stdcall MultiClipOLEDropSource::QueryInterface( REFIID iid, void ** ppvObject )
{
	// check to see what interface has been requested
	if ( iid == IID_IDropSource || iid == IID_IUnknown )
	{
		AddRef();
		*ppvObject = this;
		return S_OK;
	}
	else
	{
		*ppvObject = 0;
		return E_NOINTERFACE;
	}
}


HRESULT __stdcall MultiClipOLEDropSource::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
{
	// if the <Escape> key has been pressed since the last call, cancel the drop
	if ( fEscapePressed == TRUE )
	{
		return DRAGDROP_S_CANCEL;
	}

	// if the <LeftMouse> button has been released, then do the drop!
	if ( ( grfKeyState & MK_LBUTTON ) == 0 )
	{
		return DRAGDROP_S_DROP;
	}

	// continue with the drag-drop
	return S_OK;
}


HRESULT __stdcall MultiClipOLEDropSource::GiveFeedback( DWORD dwEffect )
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}


HRESULT MultiClipOLEDropSource::CreateDropSource( MultiClipOLEDropSource ** ppDropSource )
{
	if ( ppDropSource == 0 )
	{
		return E_INVALIDARG;
	}

	*ppDropSource = new MultiClipOLEDropSource();

	return (*ppDropSource) ? S_OK : E_OUTOFMEMORY;
}