#pragma once

#include <string>
#include "ILoader.h"
#include "ILexer.h"

#include "ScintillaTypes.h"
#include "ScintillaMessages.h"
#include "ScintillaCall.h"

#include "Scintilla.h"
#include "Lexilla.h"
#include "LexillaAccess.h"

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API SciEditText : public CControlUI, public Scintilla::ScintillaCall
	{
		DECLARE_DUICONTROL(SciEditText)
	public:
		SciEditText();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void Init() override;
		void SetPos(RECT rc, bool bNeedInvalidate = true) override;

		void resize();
	};

} // namespace DuiLib