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
#include "AboutDialog.h"
#include "PluginInterface.h"
#include "NativeLang_def.h"
#endif


void AboutDialog::doDialog()
{
    if (!isCreated())
        create(IDD_ABOUT_DLG);

	goToCenter();
}


INT_PTR CALLBACK AboutDialog::run_dlgProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
        case WM_INITDIALOG:
		{
            _emailLink.init( _hInst, _hSelf );
            _emailLink.create( ::GetDlgItem(_hSelf, IDC_EMAIL_LINK), TEXT("mailto:loonychewy@users.sourceforge.net") );

            _urlNppPlugins.init( _hInst, _hSelf );
            _urlNppPlugins.create( ::GetDlgItem(_hSelf, IDC_NPP_PLUGINS_URL), TEXT("http://www.peepor.net/index.php?p=multiclipboard") );

			// Change language
			NLChangeDialog( _hInst, _nppData._nppHandle, _hSelf, TEXT("About") );

			return TRUE;
		}
		case WM_COMMAND:
		{
			switch (wParam)
			{
				case IDOK:
				case IDCANCEL:
					display(FALSE);
					return TRUE;

				default :
					break;
			}
		}
	}
	return FALSE;
}

