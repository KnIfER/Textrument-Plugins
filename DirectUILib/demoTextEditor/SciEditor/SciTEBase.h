// SciTE - Scintilla based Text Editor
/** @file SciTEBase.h
 ** Definition of platform independent base class of editor.
 **/
// Copyright 1998-2011 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef SCITEBASE_H
#define SCITEBASE_H

extern const GUI::gui_char appName[];

extern const GUI::gui_char propUserFileName[];
extern const GUI::gui_char propGlobalFileName[];
extern const GUI::gui_char propAbbrevFileName[];

constexpr int IntFromTwoShorts(short a, short b) noexcept {
	return (a) | ((b) << 16);
}

/**
 * The order of menus on Windows - the Buffers menu may not be present
 * and there is a Help menu at the end.
 */
enum {
	menuFile = 0, menuEdit = 1, menuSearch = 2, menuView = 3,
	menuTools = 4, menuOptions = 5, menuLanguage = 6, menuBuffers = 7,
	menuHelp = 8
};

namespace SA = Scintilla;

constexpr int StyleMax = static_cast<int>(SA::StylesCommon::Max);
constexpr int StyleDefault = static_cast<int>(SA::StylesCommon::Default);

struct SelectedRange {
	SA::Position position;
	SA::Position anchor;
	SelectedRange(SA::Position position_= SA::InvalidPosition, SA::Position anchor_= SA::InvalidPosition) noexcept :
		position(position_), anchor(anchor_) {
	}
};

class RecentFile : public FilePath {
public:
	SelectedRange selection;
	SA::Line scrollPosition;
	RecentFile() {
		scrollPosition = 0;
	}
	RecentFile(const FilePath &path_, SelectedRange selection_, SA::Line scrollPosition_) :
		FilePath(path_), selection(selection_), scrollPosition(scrollPosition_) {
	}
	RecentFile(RecentFile const &) = default;
	RecentFile(RecentFile &&) = default;
	RecentFile &operator=(RecentFile const &) = default;
	RecentFile &operator=(RecentFile &&) = default;
	~RecentFile() override = default;
	void Init() noexcept override {
		FilePath::Init();
		selection.position = SA::InvalidPosition;
		selection.anchor = SA::InvalidPosition;
		scrollPosition = 0;
	}
};

struct BufferState {
public:
	RecentFile file;
	std::vector<SA::Line> foldState;
	std::vector<SA::Line> bookmarks;
};

class Session {
public:
	FilePath pathActive;
	std::vector<BufferState> buffers;
};

struct FileWorker;

class Buffer {
public:
	RecentFile file;
	void *doc;
	bool isDirty;
	bool isReadOnly;
	bool failedSave;
	bool useMonoFont;
	enum { empty, reading, readAll, opened } lifeState;
	UniMode unicodeMode;
	time_t fileModTime;
	time_t fileModLastAsk;
	time_t documentModTime;
	enum { fmNone, fmTemporary, fmMarked, fmModified} findMarks;
	std::string overrideExtension;	///< User has chosen to use a particular language
	std::vector<SA::Line> foldState;
	std::vector<SA::Line> bookmarks;
	FileWorker *pFileWorker;
	PropSetFile props;
	enum FutureDo { fdNone=0, fdFinishSave=1 } futureDo;
	Buffer() :
		file(), doc(nullptr), isDirty(false), isReadOnly(false), failedSave(false), useMonoFont(false), lifeState(empty),
		unicodeMode(uni8Bit), fileModTime(0), fileModLastAsk(0), documentModTime(0),
		findMarks(fmNone), pFileWorker(nullptr), futureDo(fdNone) {}

	~Buffer() = default;
	void Init() {
		file.Init();
		isDirty = false;
		isReadOnly = false;
		failedSave = false;
		useMonoFont = false;
		lifeState = empty;
		unicodeMode = uni8Bit;
		fileModTime = 0;
		fileModLastAsk = 0;
		documentModTime = 0;
		findMarks = fmNone;
		overrideExtension = "";
		foldState.clear();
		bookmarks.clear();
		pFileWorker = nullptr;
		futureDo = fdNone;
	}

	void SetTimeFromFile() {
		fileModTime = file.ModifiedTime();
		fileModLastAsk = fileModTime;
		documentModTime = fileModTime;
		failedSave = false;
	}

	void DocumentModified() noexcept;
	bool NeedsSave(int delayBeforeSave) const;

	void CompleteLoading() noexcept;
	void CompleteStoring();
	void AbandonAutomaticSave();

	bool ShouldNotSave() const noexcept {
		return lifeState != opened;
	}

	void CancelLoad();
};

struct BackgroundActivities {
	int loaders;
	int storers;
	size_t totalWork;
	size_t totalProgress;
	GUI::gui_string fileNameLast;
};

class BufferList {
protected:
	int current;
	int stackcurrent;
	std::vector<int> stack;
public:
	std::vector<Buffer> buffers;
	int length;
	int lengthVisible;
	bool initialised;

	BufferList();
	~BufferList();
	int size() const noexcept {
		return static_cast<int>(buffers.size());
	}
	void Allocate(int maxSize);
	int Add();
	int GetDocumentByWorker(const FileWorker *pFileWorker) const;
	int GetDocumentByName(const FilePath &filename, bool excludeCurrent=false);
	void RemoveInvisible(int index);
	void RemoveCurrent();
	int Current() const noexcept;
	Buffer *CurrentBuffer();
	const Buffer *CurrentBufferConst() const;
	void SetCurrent(int index) noexcept;
	int StackNext();
	int StackPrev();
	void CommitStackSelection();
	void MoveToStackTop(int index);
	void ShiftTo(int indexFrom, int indexTo);
	void Swap(int indexA, int indexB);
	bool SingleBuffer() const noexcept;
	BackgroundActivities CountBackgroundActivities() const;
	bool SavingInBackground() const;
	bool GetVisible(int index) const noexcept;
	void SetVisible(int index, bool visible);
	void AddFuture(int index, Buffer::FutureDo fd);
	void FinishedFuture(int index, Buffer::FutureDo fd);
private:
	void PopStack();
};

// class to hold user defined keyboard short cuts
class ShortcutItem {
public:
	std::string menuKey; // the keyboard short cut
	std::string menuCommand; // the menu command to be passed to "SciTEBase::MenuCommand"
};

class LanguageMenuItem {
public:
	std::string menuItem;
	std::string menuKey;
	std::string extension;
};

enum {
	heightTools = 24,
	heightToolsBig = 32,
	heightTab = 24,
	heightStatus = 20,
	statusPosWidth = 256
};

/// Warning IDs.
enum {
	warnFindWrapped = 1,
	warnNotFound,
	warnNoOtherBookmark,
	warnWrongFile,
	warnExecuteOK,
	warnExecuteKO
};

/// Codes representing the effect a line has on indentation.
enum IndentationStatus {
	isNone,		// no effect on indentation
	isBlockStart,	// indentation block begin such as "{" or VB "function"
	isBlockEnd,	// indentation end indicator such as "}" or VB "end"
	isKeyWordStart	// Keywords that cause indentation
};

struct StyleAndWords {
	int styleNumber;
	std::string words;
	StyleAndWords() noexcept : styleNumber(0) {
	}
	bool IsEmpty() const noexcept { return words.length() == 0; }
	bool IsSingleChar() const noexcept { return words.length() == 1; }
};

struct CurrentWordHighlight {
	enum {
		noDelay,            // No delay, and no word at the caret.
		delay,              // Delay before to highlight the word at the caret.
		delayJustEnded,     // Delay has just ended. This state allows to ignore next HighlightCurrentWord (UpdateUI and SC_UPDATE_CONTENT for setting indicators).
		delayAlreadyElapsed // Delay has already elapsed, word at the caret and occurrences are (or have to be) highlighted.
	} statesOfDelay;
	bool isEnabled;
	bool textHasChanged;
	GUI::ElapsedTime elapsedTimes;
	bool isOnlyWithSameStyle;

	CurrentWordHighlight() {
		statesOfDelay = noDelay;
		isEnabled = false;
		textHasChanged = false;
		isOnlyWithSameStyle = false;
	}
};

class Localization : public PropSetFile, public ILocalize {
	std::string missing;
public:
	bool read;
	Localization() : PropSetFile(true), read(false) {
	}
	GUI::gui_string Text(const char *s, bool retainIfNotFound=true) override;
	void SetMissing(const std::string &missing_) {
		missing = missing_;
	}
};

// Interface between SciTE and dialogs and strips for find and replace
class Searcher {
public:
	std::string findWhat;
	std::string replaceWhat;

	bool wholeWord;
	bool matchCase;
	bool regExp;
	bool unSlash;
	bool wrapFind;
	bool reverseFind;

	SA::Position searchStartPosition;
	bool replacing;
	bool havefound;
	bool failedfind;
	bool findInStyle;
	int findStyle;
	enum class CloseFind { closePrevent, closeAlways, closeOnMatch } closeFind;
	ComboMemory memFinds;
	ComboMemory memReplaces;

	bool focusOnReplace;

	Searcher();

	virtual void SetFindText(const char *sFind) = 0;
	virtual void SetFind(const char *sFind) = 0;
	virtual bool FindHasText() const noexcept = 0;
	void InsertFindInMemory();
	virtual void SetReplace(const char *sReplace) = 0;
	virtual void SetCaretAsStart() = 0;
	virtual void MoveBack() = 0;
	virtual void ScrollEditorIfNeeded() = 0;

	virtual SA::Position FindNext(bool reverseDirection, bool showWarnings=true, bool allowRegExp=true) = 0;
	virtual void HideMatch() = 0;
	enum MarkPurpose { markWithBookMarks, markIncremental };
	virtual void MarkAll(MarkPurpose purpose=markWithBookMarks) = 0;
	virtual intptr_t ReplaceAll(bool inSelection) = 0;
	virtual void ReplaceOnce(bool showWarnings=true) = 0;
	virtual void UIClosed() = 0;
	virtual void UIHasFocus() = 0;
	bool &FlagFromCmd(int cmd) noexcept;
	bool ShouldClose(bool found) const noexcept {
		return (closeFind == CloseFind::closeAlways) || (found && (closeFind == CloseFind::closeOnMatch));
	}
};

// User interface for search options implemented as both buttons and popup menu items
struct SearchOption {
	enum { tWord, tCase, tRegExp, tBackslash, tWrap, tUp };
	const char *label;
	int cmd;	// Menu item
	int id;	// Control in dialog
};

class SearchUI {
protected:
	Searcher *pSearcher;
public:
	SearchUI() noexcept : pSearcher(nullptr) {
	}
	void SetSearcher(Searcher *pSearcher_) noexcept {
		pSearcher = pSearcher_;
	}
};

class IEditorConfig;
struct SCNotification;

struct SystemAppearance {
	bool dark;
	bool highContrast;
	bool operator==(const SystemAppearance &other) const noexcept {
		return dark == other.dark && highContrast == other.highContrast;
	}
};

// Titles appear different in menus and tabs
enum class Title { menu, tab };

int ControlIDOfCommand(unsigned long) noexcept;
SA::Colour ColourOfProperty(const PropSetFile &props, const char *key, SA::Colour colourDefault);
SA::ColourAlpha ColourAlphaOfProperty(const PropSetFile &props, const char *key, SA::ColourAlpha colourDefault);
//void WindowSetFocus(GUI::ScintillaWindow &w);

// Test if an enum class value has the bit flag(s) of test set.
template <typename T>
constexpr bool FlagIsSet(T value, T test) {
	return (static_cast<int>(value) & static_cast<int>(test)) == static_cast<int>(test);
}

#endif
