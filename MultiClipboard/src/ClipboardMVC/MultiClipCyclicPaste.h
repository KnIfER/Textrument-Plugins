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

#ifndef MULTI_CLIP_CYCLIC_PASTE_H
#define MULTI_CLIP_CYCLIC_PASTE_H

#ifndef UNITY_BUILD_SINGLE_INCLUDE
#include "ModelViewController.h"
#include "MultiClipboardProxy.h"
#endif


class MultiClipCyclicPaste : public IController, public CyclicPasteListener
{
public:
	MultiClipCyclicPaste();
	virtual void Init( IModel * pNewModel, MultiClipboardProxy * pClipboardProxy, McOptionsManager * pSettings );

	void DoCyclicPaste();
	void ResetPasteIndex();

	// CyclicPasteListener interface
	virtual void OnCyclicPasteBegin();
	virtual void OnCyclicPasteEnd();

	virtual void OnModelModified();

	virtual void OnObserverAdded( McOptionsManager * SettingsManager );
	virtual void OnSettingsChanged( const stringType & GroupName, const stringType & SettingName );

private:
	// Where the last used text selection position in the scintilla edit is
	int selectionPosStart, selectionPosEnd;
	// The index into the clip list for the next text to paste
	unsigned int nextPasteIndex;
};


#endif