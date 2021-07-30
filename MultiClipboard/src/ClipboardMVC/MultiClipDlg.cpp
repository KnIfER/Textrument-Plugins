/*
This file is part of MultiClipboard Plugin for Notepad++
Copyright (C) 2009 LoonyChewy

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
#include "PluginDefinition.h"
#include "MultiClipDlg.h"
#include "resource.h"
#include "ArraysOfClips.h"
#include "MultiClipboardProxy.h"
#include "NativeLang_def.h"
#include "McOptions.h"
#include "OptionsDlg.h"
#endif


//************************ define here your toolbar layout *********************

// messages needs to be added in resource.h

ToolBarButtonUnit ListBoxToolBarButtons[] = {
	{IDM_EX_UP, -1, -1, -1, IDB_EX_UP },
	{IDM_EX_DOWN, -1, -1, -1, IDB_EX_DOWN },
	{IDM_EX_PASTE, -1, -1, -1, IDB_EX_PASTE },
	{IDM_EX_PASTE_ALL, -1, -1, -1, IDB_EX_PASTE_ALL },
	{IDM_EX_COPY_TO_CLIPBOARD, -1, -1, -1, IDB_EX_COPY_TO_CLIPBOARD },
	{IDM_EX_DELETE, -1, -1, -1, IDB_EX_DELETE },
	{IDM_EX_DELETE_ALL, -1, -1, -1, IDB_EX_DELETE_ALL },
	{IDM_EX_OPTIONS, -1, -1, -1, IDB_EX_OPTIONS }
};
#define ListBoxToolBarSize sizeof(ListBoxToolBarButtons)/sizeof(ToolBarButtonUnit)

//	Note: On change, keep sure to change order of IDM_EX_... also in function GetNameStrFromCmd
LPTSTR ListBoxToolBarToolTip[] = {
	TEXT("Move Item Up"),
	TEXT("Move Item Down"),
	TEXT("Paste Item"),
	TEXT("Paste All Items"),
	TEXT("Copy To OS Clipboard"),
	TEXT("Delete Item"),
	TEXT("Delete All Items"),
	TEXT("MultiClipboard Options")
};


extern HINSTANCE g_hInstance;
extern NppData g_NppData;
extern MultiClipboardProxy g_ClipboardProxy;
extern OptionsDlg _optDlg;


MultiClipViewerDialog::MultiClipViewerDialog()
: DockingDlgInterface(IDD_DOCK_DLG)
, IsShown( false )
, DragListMessage( 0 )
, bNoEditLargeText( TRUE )
, NoEditLargeTextSize( 10000 )
, LargeTextDisplaySize( 2048 )
, bPasteAllReverseOrder( TRUE )
, bPasteAllEOLBetweenItems( TRUE )
, pDataObject( NULL )
, pDropSource( NULL )
{
}

MultiClipViewerDialog::~MultiClipViewerDialog()
{
}

void MultiClipViewerDialog::Init( IModel * pNewModel, MultiClipboardProxy * pClipboardProxy, McOptionsManager * pSettings )
{
	DockingDlgInterface::init( g_hInstance, g_NppData._nppHandle );
	IController::Init( pNewModel, pClipboardProxy, pSettings );
	DragListMessage = ::RegisterWindowMessage( DRAGLISTMSGSTRING );

	MultiClipOLEDataObject::CreateDataObject( &pDataObject );
	MultiClipOLEDropSource::CreateDropSource( &pDropSource );
}


void MultiClipViewerDialog::Shutdown()
{
	pDataObject->Release();
	pDropSource->Release();
}

#define SELF_REFRESH WM_USER+9

void MultiClipViewerDialog::ShowDialog( bool Show )
{
	if ( !isCreated() )
	{
		//::MessageBox(NULL, TEXT("isCreated"), TEXT(""), MB_OK);
		create( &TBData );

		// define the default docking behaviour
		TBData.pszName = new TCHAR[MAX_PATH]; //here
		if ( !NLGetText( g_hInstance, g_NppData._nppHandle, TEXT("MultiClip Viewer"), (LPTSTR)TBData.pszName, MAX_PATH) )
		{
			lstrcpy( (LPTSTR)TBData.pszName, TEXT("MultiClip Viewer") );
		}
		TBData.uMask			= DWS_DF_CONT_LEFT | DWS_ICONTAB;
		TBData.hIconTab		= (HICON)::LoadImage(_hInst, MAKEINTRESOURCE(IDI_MULTICLIPBOARD), IMAGE_ICON, 0, 0, LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);
		TBData.pszModuleName	= getPluginFileName();
		TBData.dlgID			= MULTICLIPBOARD_DOCKABLE_WINDOW_INDEX;

		::SendMessage( _hParent, NPPM_DMMREGASDCKDLG, 0, (LPARAM)&TBData );
	}
	//DockingDlgInterface::display(Show);

	display( Show );

	//setClosed(!Show);
	::SendMessage(nppData._nppHandle, NPPM_SETMENUITEMCHECK, funcItem[0]._cmdID, Show);


	IsShown = Show;
	//::SendMessage( _hSelf, SELF_REFRESH, LocationPos, 0);
	ShowClipText();
}


INT_PTR CALLBACK MultiClipViewerDialog::run_dlgProc( UINT msg, WPARAM wp, LPARAM lp )
{
	//if(1)return 0;
	if ( msg == DragListMessage )
	{
		return OnDragListMessage( (LPDRAGLISTINFO)lp );
	}

	switch ( msg )
	{
		case WM_INITDIALOG:
		{
			NppDarkMode::initDarkMode();

			_cbViewer.init( _hInst, _hSelf );

			_cbListP.init( _hInst, _cbViewer.getHSelf() );

			_cbList.init( _hInst, _cbListP.getHSelf() );

			_cbViewer.pChildWin1 = &_cbListP;
			_cbListP.SetChildWindow( &_cbList );
			_cbToolbar.init( _hInst, _cbListP.getHSelf(), TB_STANDARD, ListBoxToolBarButtons, ListBoxToolBarSize );
			_cbToolbar.display();
			_cbListP.SetToolbar( &_cbToolbar );


			_cbEditP.init( _hInst, _cbViewer.getHSelf() );
			_cbEdit.init( _hInst, _cbEditP.getHSelf() );
			_cbViewer.pChildWin2 = &_cbEditP;
			_cbEditP.SetChildWindow( &_cbEdit );
			_cbEdit.EnableEditBox( FALSE );

			refreshDarkMode();


			bool enlarge_text = ::SendMessage(g_NppData._nppHandle, NPPM_GETENLARGETEXT, 0, 0);
			if(enlarge_text) {
				auto hFont = CreateFontIndirectly(-21, true);
				setWindowFont(_hSelf, hFont);
			}

			break;
		}

		case WM_SIZE:
		case WM_MOVE:
		{
			RECT rc;
			getClientRect(rc);
			SetSplitterOrientation();
			_cbViewer.reSizeTo(rc);
			//::MoveWindow(_hSelf, rc.left, rc.top, rc.right, rc.bottom, 0);
			//UpdateWindow(_hSelf);
			break;
		}

		case WM_COMMAND:
		{
			if ( (HWND)lp == _cbList.getHSelf() )
			{
				switch ( HIWORD(wp) )
				{
				case LBN_SELCHANGE:
					OnListSelectionChanged();
					return 0;

				case LBN_DBLCLK:
					OnListDoubleClicked();
					return 0;

				case LBN_DELETEITEM:
					DeleteSelectedItem();
					return 0;
				}
			}
			else if ( (HWND)lp == _cbEdit.getHSelf() )
			{
				switch ( HIWORD(wp) )
				{
				case EN_UPDATE:
					OnEditBoxUpdated();
					return 0;
				}
			}
			else if ( (HWND)lp == _cbToolbar.getHSelf() )
			{
				OnToolBarCommand( LOWORD(wp) );
				return 0;
			}
		}
		break;

		case WM_NOTIFY:
		{
			LPNMHDR nmhdr = (LPNMHDR) lp;
			if ( nmhdr->hwndFrom == _hParent )
			{
				switch ( LOWORD( nmhdr->code ) )
				{
				case DMN_FLOAT:
				case DMN_DOCK:
					{
						if ( LOWORD( nmhdr->code ) == DMN_FLOAT )
						{
							_isFloating = true;
						}
						else
						{
							_isFloating = false;
							_iDockedPos = HIWORD( nmhdr->code );
						}
						SetSplitterOrientation();
						break;
					}
				default:
					// Parse all other notifications to docking dialog interface
					return DockingDlgInterface::run_dlgProc( msg, wp, lp );
				}
			}
			else if ( nmhdr->code == TTN_GETDISPINFO )
			{
				OnToolBarRequestToolTip(nmhdr);

				return TRUE;
			}
			else if ( nmhdr->code == RBN_CHEVRONPUSHED )
			{
				NMREBARCHEVRON * lpnm = (NMREBARCHEVRON*)nmhdr;
				if (lpnm->wID == REBAR_BAR_TOOLBAR)
				{
					POINT pt;
					pt.x = lpnm->rc.left;
					pt.y = lpnm->rc.bottom;
					ClientToScreen( nmhdr->hwndFrom, &pt );
					OnToolBarCommand( _cbToolbar.doPopop( pt ) );
					return TRUE;
				}
				break;
			}
			else
			{
				// Parse all other notifications to docking dialog interface
				return DockingDlgInterface::run_dlgProc( msg, wp, lp );
			}
			break;
		}

		case WM_DESTROY:
			// Destroy icon of tab
			::DestroyIcon( TBData.hIconTab );
			break;

		default:
			return DockingDlgInterface::run_dlgProc( msg, wp, lp );
	}

	return TRUE;
}


void MultiClipViewerDialog::SetSplitterOrientation()
{
	RECT rc = {0};

	getClientRect(rc);
	if ( _isFloating )
	{
		if ( (rc.bottom-rc.top) >= (rc.right-rc.left) )
		{
			_cbViewer.SetSplitterPanelOrientation( ESPO_VERTICAL );
		}
		else
		{
			_cbViewer.SetSplitterPanelOrientation( ESPO_HORIZONTAL );
		}
	}
	else
	{
		if ( _iDockedPos == APP_LAYOUT_RNG_LEFT || _iDockedPos == APP_LAYOUT_RNG_RIGHT )
		{
			_cbViewer.SetSplitterPanelOrientation( ESPO_VERTICAL );
		}
		else
		{
			_cbViewer.SetSplitterPanelOrientation( ESPO_HORIZONTAL );
		}
	}
}


void MultiClipViewerDialog::OnModelModified()
{
	ShowClipText();
}

void MultiClipViewerDialog::refreshDarkMode()
{
	NppDarkMode::refreshDarkMode(getHSelf());

	if (isCreated())
	{
		NppDarkMode::autoSubclassAndThemeChildControls(_hSelf);
		::SendMessage(getHSelf(), WM_SIZE, 0, 0);
	}
	if (_optDlg.isCreated())
	{
		::SendMessage(_optDlg.getHSelf(), NPPN_DARKCONF_CHANGED, 0, 0);
	}
}


void MultiClipViewerDialog::ShowClipText()
{
	if ( !IsShown )
	{
		return;
	}

	ArraysOfClips * pClipboardList = (ArraysOfClips *)GetModel();
	if ( !pClipboardList )
	{
		return;
	}

	_cbList.ClearAll();
	for ( unsigned int i = 0; i < pClipboardList->GetNumText(); ++i )
	{
		_cbList.AddItem( pClipboardList->GetText(i).text );
	}
}


void MultiClipViewerDialog::OnListSelectionChanged()
{
	INT Index = _cbList.GetCurrentSelectionIndex();
	if ( Index == LB_ERR )
	{
		return;
	}

	ArraysOfClips * pClipboardList = (ArraysOfClips *)GetModel();
	const std::wstring & text = pClipboardList->GetText( Index ).text;
	// Check if text is too large, and we only want to display a bit of it, read-only
	if ( bNoEditLargeText && text.size() > NoEditLargeTextSize )
	{
		// Get internationalised string for large text
		std::wstring largeTextDisplay( TEXT("*** Large Text ***\r\n") );
		std::vector< TCHAR > largeText(512);
		int len = NLGetText( g_hInstance, g_NppData._nppHandle, TEXT("Large Text"), &largeText[0], largeText.capacity() );
		if ( len > 0 )
		{
			largeTextDisplay = std::wstring( TEXT("***") ) + &largeText[0] + std::wstring( TEXT("***\r\n") );
		}

		// Append snippet of text to edit box, up to 1024 chars long
		unsigned int textSize = text.size() > LargeTextDisplaySize ? LargeTextDisplaySize : text.size();
		largeTextDisplay += text.substr( 0, textSize );
		_cbEdit.SetText( largeTextDisplay.c_str() );
		// And make it enabled, but read-only
		_cbEdit.EnableEditBox();
		_cbEdit.SetEditBoxReadOnly( TRUE );
	}
	else
	{
		// Else, just display all the text and make it editable
		_cbEdit.SetText( text.c_str() );
		_cbEdit.EnableEditBox();
		// And make it read-write
		_cbEdit.SetEditBoxReadOnly( FALSE );
	}
	//g_ClipboardProxy.SetFocusToDocument();
}


void MultiClipViewerDialog::OnListDoubleClicked()
{
	PasteSelectedItem();
}


void MultiClipViewerDialog::OnEditBoxUpdated()
{
	ArraysOfClips * pClipboardList = (ArraysOfClips *)IController::GetModel();
	if ( !pClipboardList )
	{
		return;
	}

	std::wstring text;
	_cbEdit.GetText( text );
	int SelIndex = _cbList.GetCurrentSelectionIndex();
	pClipboardList->EditText( SelIndex, text );
	_cbList.SetCurrentSelectedItem( SelIndex );
}


void MultiClipViewerDialog::OnToolBarRequestToolTip( LPNMHDR nmhdr )
{
	// Tooltip request of toolbar
	LPTOOLTIPTEXT lpttt;

	lpttt = (LPTOOLTIPTEXT)nmhdr;
	lpttt->hinst = _hInst;

	// Specify the resource identifier of the descriptive
	// text for the given button.
	int resId = (int)lpttt->hdr.idFrom;
	int ToolTipIndex = resId - ListBoxToolBarButtons[0]._cmdID;

	TCHAR ToolTipText[MAX_PATH];
	int len = NLGetText( g_hInstance, g_NppData._nppHandle, ListBoxToolBarToolTip[ToolTipIndex], ToolTipText, sizeof(ToolTipText) );
	if ( len == 0 )
	{
		lpttt->lpszText = ListBoxToolBarToolTip[ToolTipIndex];
	}
	else
	{
		lpttt->lpszText = ToolTipText;
	}
}


void MultiClipViewerDialog::OnToolBarCommand( UINT Cmd )
{
	ArraysOfClips * pClipboardList = (ArraysOfClips *)IController::GetModel();
	if ( !pClipboardList )
	{
		return;
	}

	// Commands here don't need an active selection in the list box
	switch ( Cmd )
	{
	case IDM_EX_PASTE_ALL:
		PasteAllItems();
		return;

	case IDM_EX_DELETE_ALL:
		DeleteAllItems();
		return;

	case IDM_EX_OPTIONS:
		_optDlg.ShowDialog(!_optDlg.isVisible());
		return;
	}

	// Commands below need an active selection in the list box
	int SelIndex = _cbList.GetCurrentSelectionIndex();
	if ( SelIndex < 0 || SelIndex >= (int)pClipboardList->GetNumText() )
	{
		return;
	}

	switch ( Cmd )
	{
	case IDM_EX_UP:
		if ( SelIndex > 0 )
		{
			pClipboardList->SetTextNewIndex( SelIndex, SelIndex-1 );
			_cbList.SetCurrentSelectedItem( SelIndex-1 );
		}
		break;

	case IDM_EX_DOWN:
		if ( SelIndex < (int)pClipboardList->GetNumText()-1 )
		{
			pClipboardList->SetTextNewIndex( SelIndex, SelIndex+1 );
			_cbList.SetCurrentSelectedItem( SelIndex+1 );
		}
		break;

	case IDM_EX_PASTE:
		PasteSelectedItem();
		break;

	case IDM_EX_DELETE:
		DeleteSelectedItem();
		break;

	case IDM_EX_COPY_TO_CLIPBOARD:
		CopySelectedItemToClipboard();
		break;
	}
}


BOOL MultiClipViewerDialog::OnDragListMessage( LPDRAGLISTINFO pDragListInfo )
{

	if ( pDragListInfo->uNotification == DL_BEGINDRAG )
	{
		int listBoxItem = LBItemFromPt( _cbList.getHSelf(), pDragListInfo->ptCursor, FALSE );
		if ( listBoxItem >= 0 )
		{

			ArraysOfClips * pClipboardList = (ArraysOfClips *)IController::GetModel();
			if ( pClipboardList )
			{
				const DataOfClip & textItem = pClipboardList->GetText( listBoxItem );
				unsigned int textSizeInBytes = (textItem.text.size() + 1) * 2;	// Wide char = 2 bytes + null terminator

				pDataObject->SetMultiClipDragData( textItem.text.c_str(), textSizeInBytes, textItem.textMode == TCM_COLUMN );
				DWORD dwEffect = 0;
				::DoDragDrop( pDataObject, pDropSource, DROPEFFECT_COPY, &dwEffect );
			}
		}

		// Store the return value in DWL_MSGRESULT. Set to false so we don't receive the rest of the drag messages
		// http://support.microsoft.com/kb/183115
		SetWindowLongPtr( getHSelf(), DWLP_MSGRESULT, FALSE );
	}
	return TRUE;
}


void MultiClipViewerDialog::PasteSelectedItem()
{
	INT Index = _cbList.GetCurrentSelectionIndex();
	if ( Index == LB_ERR )
	{
		return;
	}

	_cbEdit.EnableEditBox();
	ArraysOfClips * pClipboardList = (ArraysOfClips *)GetModel();
	g_ClipboardProxy.PasteTextToNpp( pClipboardList->GetText( Index ) );
	g_ClipboardProxy.SetFocusToDocument();
}


void MultiClipViewerDialog::PasteAllItems()
{
	ArraysOfClips * pClipboardList = (ArraysOfClips *)GetModel();
	if ( pClipboardList->GetNumText() == 0 )
	{
		return;
	}

	std::wstring eolChar;
	if ( bPasteAllEOLBetweenItems )
	{
		eolChar = g_ClipboardProxy.GetEOLString();
	}

	// Paste all the clipboard text into this string stream first
	g_ClipboardProxy.BeginUndoAction();
	if ( bPasteAllReverseOrder )
	{
		// Use signed type for index because --index will cause value to wrap around when index == 0
		for ( int index = pClipboardList->GetNumText() - 1; index >= 0; --index )
		{
			g_ClipboardProxy.ReplaceSelectionText( pClipboardList->GetText( (unsigned int) index ) );
			g_ClipboardProxy.ReplaceSelectionText( eolChar );
		}
	}
	else
	{
		for ( unsigned int index = 0; index < pClipboardList->GetNumText(); ++index )
		{
			g_ClipboardProxy.ReplaceSelectionText( pClipboardList->GetText( (unsigned int) index ) );
			g_ClipboardProxy.ReplaceSelectionText( eolChar );
		}
	}
	g_ClipboardProxy.EndUndoAction();
	g_ClipboardProxy.SetFocusToDocument();
}


void MultiClipViewerDialog::DeleteSelectedItem()
{
	ArraysOfClips * pClipboardList = (ArraysOfClips *)IController::GetModel();
	if ( !pClipboardList )
	{
		return;
	}
	int SelIndex = _cbList.GetCurrentSelectionIndex();
	if ( SelIndex < 0 || SelIndex >= (int)pClipboardList->GetNumText() )
	{
		return;
	}

	pClipboardList->RemoveText( SelIndex );
	// Select the next item in the list
	_cbList.SetCurrentSelectedItem( SelIndex, FALSE );
	// Check whether selection is successful
	SelIndex = _cbList.GetCurrentSelectionIndex();
	if ( SelIndex < 0 || SelIndex >= (int)pClipboardList->GetNumText() )
	{
		// Not successful, clear and disable textbox
		_cbEdit.SetText( std::wstring() );
		_cbEdit.EnableEditBox( FALSE );
	}
	else
	{
		OnListSelectionChanged();
	}
}


void MultiClipViewerDialog::DeleteAllItems()
{
	ArraysOfClips * pClipboardList = (ArraysOfClips *)IController::GetModel();
	if ( !pClipboardList )
	{
		return;
	}

	pClipboardList->RemoveAllTexts();
	// Clear and disable textbox since there is nothing left
	_cbEdit.SetText( std::wstring() );
	_cbEdit.EnableEditBox( FALSE );
}


void MultiClipViewerDialog::CopySelectedItemToClipboard()
{
	INT Index = _cbList.GetCurrentSelectionIndex();
	if ( Index == LB_ERR )
	{
		return;
	}

	_cbEdit.EnableEditBox();
	ArraysOfClips * pClipboardList = (ArraysOfClips *)GetModel();
	g_ClipboardProxy.SetTextToSystemClipboard( pClipboardList->GetText( Index ) );
	g_ClipboardProxy.SetFocusToDocument();
}


void MultiClipViewerDialog::OnObserverAdded( McOptionsManager * SettingsManager )
{
	SettingsObserver::OnObserverAdded( SettingsManager );

	// Add default settings if it doesn't exists
	SET_SETTINGS_BOOL( SETTINGS_GROUP_MULTI_CLIP_VIEWER, SETTINGS_NO_EDIT_LARGE_TEXT, bNoEditLargeText )
	SET_SETTINGS_INT( SETTINGS_GROUP_MULTI_CLIP_VIEWER, SETTINGS_NO_EDIT_LARGE_TEXT_SIZE, NoEditLargeTextSize )
	SET_SETTINGS_INT( SETTINGS_GROUP_MULTI_CLIP_VIEWER, SETTINGS_LARGE_TEXT_DISPLAY_SIZE, LargeTextDisplaySize )
	SET_SETTINGS_BOOL( SETTINGS_GROUP_MULTI_CLIP_VIEWER, SETTINGS_PASTE_ALL_REVERSE, bPasteAllReverseOrder )
	SET_SETTINGS_BOOL( SETTINGS_GROUP_MULTI_CLIP_VIEWER, SETTINGS_PASTE_ALL_NEWLINE_BETWEEN, bPasteAllEOLBetweenItems )
}


void MultiClipViewerDialog::OnSettingsChanged( const stringType & GroupName, const stringType & SettingName )
{
	if ( GroupName != SETTINGS_GROUP_MULTI_CLIP_VIEWER )
	{
		return;
	}

	IF_SETTING_CHANGED_BOOL( SETTINGS_GROUP_MULTI_CLIP_VIEWER, SETTINGS_NO_EDIT_LARGE_TEXT, bNoEditLargeText )
	else IF_SETTING_CHANGED_INT( SETTINGS_GROUP_MULTI_CLIP_VIEWER, SETTINGS_NO_EDIT_LARGE_TEXT_SIZE, NoEditLargeTextSize )
	else IF_SETTING_CHANGED_INT( SETTINGS_GROUP_MULTI_CLIP_VIEWER, SETTINGS_LARGE_TEXT_DISPLAY_SIZE, LargeTextDisplaySize )
	else IF_SETTING_CHANGED_BOOL( SETTINGS_GROUP_MULTI_CLIP_VIEWER, SETTINGS_PASTE_ALL_REVERSE, bPasteAllReverseOrder )
	else IF_SETTING_CHANGED_BOOL( SETTINGS_GROUP_MULTI_CLIP_VIEWER, SETTINGS_PASTE_ALL_NEWLINE_BETWEEN, bPasteAllEOLBetweenItems )
}