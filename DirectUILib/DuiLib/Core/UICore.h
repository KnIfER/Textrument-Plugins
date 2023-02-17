#pragma once
#define UNICODE 1

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stddef.h>
#include <richedit.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <comdef.h>
#include <gdiplus.h> 
#include <strsafe.h>

#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <set>

#define m_bAutoCalcWidth            (m_cxyFixed.cx==-2)
#define m_bAutoCalcHeight           (m_cxyFixed.cy==-2)
#define m_bFillParentWidth          (m_cxyFixed.cx==-1)
#define m_bFillParentHeight         (m_cxyFixed.cy==-1)

#include "Utils/Utils.h"
#include "Utils/unzip.h"
#include "Utils/VersionHelpers.h"
#include "Core/XMLMarkup.h"
#include "Utils/observer_impl_base.h"
#include "Utils/UIShadow.h"
#include "Utils/UIDelegate.h"
#include "Utils/DragDropImpl.h"
#include "Utils/DPI.h"

#include "Core/UIDefine.h"
#include "Core/UIResourceManager.h"
#include "Core/UIManager.h"
#include "Core/UIDxAnimation.h"

#include "Drawable/drawable.h"

#include "Core/UIBase.h"
#include "Core/ControlFactory.h"
#include "Core/UIControl.h"
#include "Core/UIContainer.h"

#include "Core/InsituDebug.h"

#include "shlwapi.h"

using namespace Gdiplus;