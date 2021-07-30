![](./res/paste_muple.ico)  VS-like Multiple Clipboard Plugin for Notepad++.

- Originally written by LoonyChewy
- This repo needs to be compiled against the [《图创文本》](https://github.com/KnIfER/Textrument) Opensource Project.
- Compatible with NotepadPlus v8. 
- Supports Darkmode
- Store **more** to the list (default 20), but show **less** for the paste menu (default 10).



MultiClipboard 2
================

Author: LoonyChewy
Website: http://www.peepor.net/loonchew/index.php?p=multiclipboard

Summary
-------
This is a plugin for Notepad++. It keep tracks of the most recently copied text in Notepad++
and/or the OS clipboard, and allows the user to retrieve and paste these text back to their
documents.

Features
--------
+ Natively supports unicode
+ Keeps text that is copied to the OS clipboard, either
   1. when copied from within Notepad++ (the current option)
   2. from any program currently running
   3. or from any program, but only when text is pasted into Notepad++
+ A sidebar with a list to display all the stored text
   1. Shift the relative position of the text
   2. Delete the text
   3. Paste the text by double-clicking it, or clicking the paste icon
   4. An edit box in the sidebar to edit the current selected text in the list
   5. Paste all clipboard items into the document
   6. Drag and drop clipboard items from the list to the document
+ A pop-up paste menu that displays the stored text selecting text to paste
   1. Ctrl-Shift-V (customisable) to activate
   2. Customisable width
   3. Numeric mnemonic
+ Option to use cyclic clipboard ring instead of paste menu
   1. Ctrl-Shift-V (customisable) repeatedly to cyclic through, release keyboard to select
+ Option to use middle mouse button click to paste last stored text from clipboard
   1. Shift-Middle click to show paste menu
+ Option to auto copy selected text from document to clipboard
+ Supports native language customisation via NativeLang plugin
+ Option to persist clipboard texts across editing sessions

Useful Tips
-----------
To show the multiclipboard paste menu via (right click) context menu, add the following line to %Notepad++Directory%\contextMenu.xml 
<Item PluginEntryName="MultiClipboard" pluginCommandItemName="MultiClipboard Paste"/>

Known Issues
------------
- When the document is converted from one EOL type to another in Notepad++, the clipboard text aren't updated
- When keymapping of paste is changed from Ctrl-V or Shift-Insert to something else, MultiClipboard will not be able to hook into the paste operation. This is a limitation of Notepad++, because it does not notify plugins of paste operations nor expose its shortcut mappings to plugins

Possible Todos
--------------
- For paste all, allow customisable text in between items
- Improve the options dialog. It doesn't scale well when more options are added
- Create a custom made paste menu, so that 2nd item can be automatically selected
  As well as popup tooltip to display all the entire text
- Allow drag and drop to reorder clipboard list. Implement listbox as OLE drop target
- Remember the position of the splitter in the MultiClip Viewer across sessions

Acknowledgements
----------------
Icons taken from http://www.famfamfam.com/lab/icons/silk/preview.php

Version History
---------------
9 Sep 2011 - MultiClipboard 2.1
-----------
1.  Limit text entries in the listbox of MultiClip Viewer to 100 chars if text is too long.
2.  Allow 'Delete' key to delete selected item in MultiClip Viewer
3.  Option to ignore text greater than a certain size to improve plugin performance.
    Text size is a hidden option in the config xml
4.  Option to not allow text greater than a certain size to be edited in MultiClip Viewer,
    to improve plugin performance. Text size is a hidden option in the config xml
5.  Some internal changes to improve performance a little bit
6.  Rename localisation file to more standard name suffixes, eg _en, _de
7.  Fix issue when Notepad++ is converting text format between ANSI and UTF/Unicode, the
    entire text is copied into system clipboard during this process, and hence into
    MultiClipboard. Now the plugin will recognise and ignore this process
8.  Added toolbar button to delete selected item in MultiClip Viewer
9.  Added toolbar button to copy selected item to OS clipboard in MultiClip Viewer
10. Added toolbar button to paste all item from the clipboard
11. When pasting all items, add option to paste them in reverse order
12. When pasting all items, add option to add newline character between items
13. Added toolbar button to show MultiClipboard options dialog
14. Make items in listbox of MultiClip Viewer draggable onto editor to insert text
15. Remembers whether the clipboard item is a column mode selection
    and paste it accordingly.
    Requires Notepad++ 5.5 for rectangular selection to work properly.
16. When auto copying selected text, text is copied immediately after selection.
    When paste is done on selected text, the previous text in the system clipboard is pasted over the selection instead.
17. Added option to persist clipboard texts across editing sessions
(Technical)
1. Switch to unity build configuration. Full release build time went down from 27 secs to 8 secs
2. Use link time code generation in release build to hopefully improve runtime performance
3. Updated project file to Visual Studio 2010
4. Use scintilla header from Scintilla version 2.01, which comes with Notepad++ 5.5

20 Feb 2009 - MultiClipboard 2.0
-----------
1. Added cyclic clipboard paste as an alternative to paste menu on Ctrl-Shift-V
2. Added auto copying of selected text to clipboard
3. Added middle click paste and shift-middle click to show paste menu
4. Paste menu pop-up location is at caret when activated by keyboard, and at mouse cursor
   when activated by mouse
(Internal code changes)
5. Refactored the code for the settings dialog (no user functionalities changed)
6. Refactored the code to delete the IView class and inherit all MVC classes from IController
   as the two classes are too similar
7. Added a Init( ... ) function to IController class to encapsulate all initialisations
   within itself


22 Nov 2008 - MultiClipboard 2.0 Preview 2
-----------
1. Added NativeLang support
2. Updated UI look and feel and icons to match those of other Notepad++ plugins, modify listbox and editbox fonts (to courier new, non bold) and tab stop width for the latter (4 chars)
3. After deleting a clipviewer item, the next item in the list will be selected
4. Edit box will be enabled only when a valid list item is selected
5. After clicking or double-clicking a list item, input focus will go back to the active scintilla view
6. Added an options dialog, and these settings are saved to xml file for persistent across sessions
7. Upgraded to unicode version. From now on this plugin will only work with unicode version of Notepad++ (version 5.1 unicode and above)
8. Added the original paste menu that is activated via Ctrl-Shift-V
