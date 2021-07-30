#pragma once
#ifndef _UIBTRANSLATOR_H_
#define _UIBTRANSLATOR_H_

#include "../StdAfx.h"

__declspec(selectany) std::map<std::string, std::string>* localizationMap = NULL;

namespace DuiLib
{
	bool TranslateUI(CPaintManagerUI & m_pm, std::map<std::string, std::string> & localizefile);
}

#endif