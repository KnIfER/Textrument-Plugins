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

#ifndef MULTI_CLIP_VIEWER_DIALOG_H
#define MULTI_CLIP_VIEWER_DIALOG_H

#ifndef UNITY_BUILD_SINGLE_INCLUDE
#include "ModelViewController.h"
#include "DockingDlgInterface.h"
#include "SplitterPanel.h"
#include "ToolbarPanel.h"
#include "ToolBar.h"
#include "CBListbox.h"
#include "CBEditbox.h"
#include "MultiClipOLEDragDrop.h"
#endif

// ID for dockable window
#define	MULTICLIPBOARD_DOCKABLE_WINDOW_INDEX 0


class MultiClipViewerDialog : public DockingDlgInterface, public IController
{
public:
	MultiClipViewerDialog();
	~MultiClipViewerDialog();
	// Not inherited from DockingDlgInterface
	virtual void Init( IModel * pNewModel, MultiClipboardProxy * pClipboardProxy, McOptionsManager * pSettings );
	virtual void Shutdown();
	// Inherited from DockingDlgInterface
	virtual void destroy() {}

	void ShowDialog( bool Show = TRUE );

	virtual void OnModelModified();

	void refreshDarkMode();
protected:
	// Overload DockingDlgInterface's dialog procedure
	virtual INT_PTR CALLBACK run_dlgProc( UINT msg, WPARAM wp, LPARAM lp );

	// Toolbar commands
	void tb_cmd(UINT message);

	void SetSplitterOrientation();

private:
	tTbData TBData;
	bool IsShown;
	UINT DragListMessage;
	MultiClipOLEDataObject * pDataObject;
	MultiClipOLEDropSource * pDropSource;

	// Don't allow display and editing of text greater than NoEditLargeTextSize
	BOOL bNoEditLargeText;
	UINT NoEditLargeTextSize;
	UINT LargeTextDisplaySize;
	BOOL bPasteAllReverseOrder;
	BOOL bPasteAllEOLBetweenItems;

	SplitterPanel _cbViewer;
	ToolbarPanel _cbListP;
	ToolbarPanel _cbEditP;
	ToolBar _cbToolbar;
	CBListbox _cbList;
	CBEditbox _cbEdit;

	void ShowClipText();
	void OnListSelectionChanged();
	void OnListDoubleClicked();
	void OnEditBoxUpdated();
	// Get toolbar tooltips
	void OnToolBarRequestToolTip( LPNMHDR nmhdr );
	void OnToolBarCommand( UINT Cmd );
	BOOL OnDragListMessage( LPDRAGLISTINFO pDragListInfo );

	void PasteSelectedItem();
	void PasteAllItems();
	void DeleteSelectedItem();
	void DeleteAllItems();
	void CopySelectedItemToClipboard();

	virtual void OnObserverAdded( McOptionsManager * SettingsManager );
	virtual void OnSettingsChanged( const stringType & GroupName, const stringType & SettingName );
};


#endif