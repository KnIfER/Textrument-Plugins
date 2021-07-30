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

#ifndef MODEL_VIEW_CONTROLLER_H
#define MODEL_VIEW_CONTROLLER_H


#ifndef UNITY_BUILD_SINGLE_INCLUDE
#include <vector>
#include "McOptionsManager.h"
#endif


class IModel;
class IController;
class MultiClipboardProxy;


class IModel : public SettingsObserver
{
public:
	IModel() {}
	~IModel() {}

	void AddController( IController * pController );
	void RemoveController( IController * pController );

protected:
	void OnModified() { NotifyControllers(); }
	void NotifyControllers();

private:
	// There are private so only the MVC framework here can access these
	typedef std::vector<IController *> ControllersList;
	ControllersList Controllers;
	ControllersList::iterator FindController( IController * pController );
};


class IController : public SettingsObserver
{
public:
	IController() : pModel( 0 ) {}
	virtual void Init( IModel * pNewModel, MultiClipboardProxy * pClipboardProxy, McOptionsManager * pSettings )
	{
		pNewModel->AddController( this );
		pSettings->AddSettingsObserver( this );
	}

protected:
	IModel * GetModel() { return pModel; }
	virtual void OnModelModified() {}

private:
	friend class IModel;
	IModel * pModel;
	void SetModel( IModel * pNewModel ) { pModel = pNewModel; }
};


#endif