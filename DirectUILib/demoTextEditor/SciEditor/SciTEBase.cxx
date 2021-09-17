// SciTE - Scintilla based Text Editor
/** @file SciTEBase.cxx
 ** Platform independent base class of editor.
 **/
// Copyright 1998-2011 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <cmath>

#include <tuple>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <memory>
#include <chrono>
#include <atomic>
#include <mutex>
#include <thread>

#include <fcntl.h>
#include <sys/stat.h>

#include "ILoader.h"

#include "ScintillaTypes.h"
#include "ScintillaMessages.h"
#include "ScintillaCall.h"

#include "Scintilla.h"
#include "SciLexer.h"

#include "GUI.h"
#include "StringList.h"
#include "StringHelpers.h"
#include "FilePath.h"
#include "StyleDefinition.h"
#include "PropSetFile.h"
#include "StyleWriter.h"
#include "Extender.h"
#include "JobQueue.h"

#include "Cookie.h"
#include "Worker.h"
#include "FileWorker.h"
//#include "MatchMarker.h"
//#include "EditorConfig.h"
#include "SciTEBase.h"

Searcher::Searcher() {
	wholeWord = false;
	matchCase = false;
	regExp = false;
	unSlash = false;
	wrapFind = true;
	reverseFind = false;

	searchStartPosition = 0;
	replacing = false;
	havefound = false;
	failedfind = false;
	findInStyle = false;
	findStyle = 0;
	closeFind = CloseFind::closeAlways;

	focusOnReplace = false;
}

void Searcher::InsertFindInMemory() {
	memFinds.Insert(findWhat);
}

// The find and replace dialogs and strips often manipulate boolean
// flags based on dialog control IDs and menu IDs.
bool &Searcher::FlagFromCmd(int cmd) noexcept {
	static bool notFound = false;
	switch (cmd) {
	//case IDWHOLEWORD:
	//case IDM_WHOLEWORD:
	//	return wholeWord;
	//case IDMATCHCASE:
	//case IDM_MATCHCASE:
	//	return matchCase;
	//case IDREGEXP:
	//case IDM_REGEXP:
	//	return regExp;
	//case IDUNSLASH:
	//case IDM_UNSLASH:
	//	return unSlash;
	//case IDWRAP:
	//case IDM_WRAPAROUND:
	//	return wrapFind;
	//case IDDIRECTIONUP:
	//case IDM_DIRECTIONUP:
	//	return reverseFind;
	}
	return notFound;
}