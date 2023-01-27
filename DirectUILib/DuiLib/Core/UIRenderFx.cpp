#include "StdAfx.h"

namespace DuiLib {
	// todo render markdown 
	void CRenderEngine::DrawHtmlText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor, RECT* prcLinks, QkString* sLinks, int& nLinkRects, int iFont, UINT uStyle)
	{
		// 考虑到在xml编辑器中使用<>符号不方便，可以使用{}符号代替
		// 支持标签嵌套（如<l><b>text</b></l>），但是交叉嵌套是应该避免的（如<l><b>text</l></b>）
		// The string formatter supports a kind of "mini-html" that consists of various short tags:
		//
		//   Bold:             <b>text</b>
		//   Color:            <c #xxxxxx>text</c>  where x = RGB in hex
		//   Font:             <f x>text</f>        where x = font id
		//   Italic:           <i>text</i>
		//   Image:            <i x y z>            where x = image name and y = imagelist num and z(optional) = imagelist id
		//   Link:             <a x>text</a>        where x(optional) = link content, normal like app:notepad or http:www.xxx.com
		//   NewLine           <n>                  
		//   Paragraph:        <p x>text</p>        where x = extra pixels indent in p
		//   Raw Text:         <r>text</r>
		//   Selected:         <s>text</s>
		//   Underline:        <u>text</u>
		//   X Indent:         <x i>                where i = hor indent in pixels
		//   Y Indent:         <y i>                where i = ver indent in pixels 

		ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
		if( pstrText == NULL || pManager == NULL ) return;
		if( ::IsRectEmpty(&rc) ) return;

		bool bDraw = (uStyle & DT_CALCRECT) == 0;

		CStdPtrArray aFontArray(10);
		CStdPtrArray aColorArray(10);
		CStdPtrArray aPIndentArray(10);

		RECT rcClip = { 0 };
		::GetClipBox(hDC, &rcClip);
		HRGN hOldRgn = ::CreateRectRgnIndirect(&rcClip);
		HRGN hRgn = ::CreateRectRgnIndirect(&rc);
		if( bDraw ) ::ExtSelectClipRgn(hDC, hRgn, RGN_AND);

		TFontInfo* pDefFontInfo = pManager->GetFontInfo(iFont);
		if(pDefFontInfo == NULL) {
			pDefFontInfo = pManager->GetDefaultFontInfo();
		}
		TEXTMETRIC* pTm = &pDefFontInfo->tm;
		HFONT hOldFont = (HFONT) ::SelectObject(hDC, pDefFontInfo->hFont);
		::SetBkMode(hDC, TRANSPARENT);
		::SetTextColor(hDC, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
		DWORD dwBkColor = pManager->GetDefaultSelectedBkColor();
		::SetBkColor(hDC, RGB(GetBValue(dwBkColor), GetGValue(dwBkColor), GetRValue(dwBkColor)));

		// If the drawstyle include a alignment, we'll need to first determine the text-size so
		// we can draw it at the correct position...
		if( ((uStyle & DT_CENTER) != 0 || (uStyle & DT_RIGHT) != 0 || (uStyle & DT_VCENTER) != 0 || (uStyle & DT_BOTTOM) != 0) && (uStyle & DT_CALCRECT) == 0 ) {
			RECT rcText = { 0, 0, 9999, 100 };
			int nLinks = 0;
			DrawHtmlText(hDC, pManager, rcText, pstrText, dwTextColor, NULL, NULL, nLinks, iFont, uStyle | DT_CALCRECT);
			if( (uStyle & DT_SINGLELINE) != 0 ){
				if( (uStyle & DT_CENTER) != 0 ) {
					rc.left = rc.left + ((rc.right - rc.left) / 2) - ((rcText.right - rcText.left) / 2);
					rc.right = rc.left + (rcText.right - rcText.left);
				}
				if( (uStyle & DT_RIGHT) != 0 ) {
					rc.left = rc.right - (rcText.right - rcText.left);
				}
			}
			if( (uStyle & DT_VCENTER) != 0 ) {
				rc.top = rc.top + ((rc.bottom - rc.top) / 2) - ((rcText.bottom - rcText.top) / 2);
				rc.bottom = rc.top + (rcText.bottom - rcText.top);
			}
			if( (uStyle & DT_BOTTOM) != 0 ) {
				rc.top = rc.bottom - (rcText.bottom - rcText.top);
			}
		}

		bool bHoverLink = false;
		QkString sHoverLink;
		POINT ptMouse = pManager->GetMousePos();
		for( int i = 0; !bHoverLink && i < nLinkRects; i++ ) {
			if( ::PtInRect(prcLinks + i, ptMouse) ) {
				sHoverLink = *(QkString*)(sLinks + i);
				bHoverLink = true;
			}
		}

		POINT pt = { rc.left, rc.top };
		int iLinkIndex = 0;
		int cyLine = pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1);
		int cyMinHeight = 0;
		int cxMaxWidth = 0;
		POINT ptLinkStart = { 0 };
		bool bLineEnd = false;
		bool bInRaw = false;
		bool bInLink = false;
		bool bInSelected = false;
		int iLineLinkIndex = 0;

		// 排版习惯是图文底部对齐，所以每行绘制都要分两步，先计算高度，再绘制
		CStdPtrArray aLineFontArray;
		CStdPtrArray aLineColorArray;
		CStdPtrArray aLinePIndentArray;
		LPCTSTR pstrLineBegin = pstrText;
		bool bLineInRaw = false;
		bool bLineInLink = false;
		bool bLineInSelected = false;
		int cyLineHeight = 0;
		bool bLineDraw = false; // 行的第二阶段：绘制
		while( *pstrText != _T('\0') ) {
			if( pt.x >= rc.right || *pstrText == _T('\n') || bLineEnd ) {
				if( *pstrText == _T('\n') ) pstrText++;
				if( bLineEnd ) bLineEnd = false;
				if( !bLineDraw ) {
					if( bInLink && iLinkIndex < nLinkRects ) {
						::SetRect(&prcLinks[iLinkIndex++], ptLinkStart.x, ptLinkStart.y, MIN(pt.x, rc.right), pt.y + cyLine);
						QkString *pStr1 = (QkString*)(sLinks + iLinkIndex - 1);
						QkString *pStr2 = (QkString*)(sLinks + iLinkIndex);
						*pStr2 = *pStr1;
					}
					for( int i = iLineLinkIndex; i < iLinkIndex; i++ ) {
						prcLinks[i].bottom = pt.y + cyLine;
					}
					if( bDraw ) {
						bInLink = bLineInLink;
						iLinkIndex = iLineLinkIndex;
					}
				}
				else {
					if( bInLink && iLinkIndex < nLinkRects ) iLinkIndex++;
					bLineInLink = bInLink;
					iLineLinkIndex = iLinkIndex;
				}
				if( (uStyle & DT_SINGLELINE) != 0 && (!bDraw || bLineDraw) ) break;
				if( bDraw ) bLineDraw = !bLineDraw; // !
				pt.x = rc.left;
				if( !bLineDraw ) pt.y += cyLine;
				if( pt.y > rc.bottom && bDraw ) break;
				ptLinkStart = pt;
				cyLine = pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1);
				if( pt.x >= rc.right ) break;
			}
			else if( !bInRaw && ( *pstrText == _T('<') || *pstrText == _T('{') )
				&& ( pstrText[1] >= _T('a') && pstrText[1] <= _T('z') )
				&& ( pstrText[2] == _T(' ') || pstrText[2] == _T('>') || pstrText[2] == _T('}') ) ) {
					pstrText++;
					LPCTSTR pstrNextStart = NULL;
					switch( *pstrText ) {
					case _T('a'):  // Link
						{
							pstrText++;
							while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
							if( iLinkIndex < nLinkRects && !bLineDraw ) {
								QkString *pStr = (QkString*)(sLinks + iLinkIndex);
								pStr->Empty();
								while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') ) {
									LPCTSTR pstrTemp = ::CharNext(pstrText);
									while( pstrText < pstrTemp) {
										*pStr += *pstrText++;
									}
								}
							}

							DWORD clrColor = dwTextColor;
							if(clrColor == 0) pManager->GetDefaultLinkFontColor();
							if( bHoverLink && iLinkIndex < nLinkRects ) {
								QkString *pStr = (QkString*)(sLinks + iLinkIndex);
								if( sHoverLink == *pStr ) clrColor = pManager->GetDefaultLinkHoverFontColor();
							}
							//else if( prcLinks == NULL ) {
							//    if( ::PtInRect(&rc, ptMouse) )
							//        clrColor = pManager->GetDefaultLinkHoverFontColor();
							//}
							aColorArray.Add((LPVOID)clrColor);
							::SetTextColor(hDC,  RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
							TFontInfo* pFontInfo = pDefFontInfo;
							if( aFontArray.GetSize() > 0 ) pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
							if( pFontInfo->bUnderline == false ) {
								HFONT hFont = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
								if( hFont == NULL ) hFont = pManager->AddFont(g_tmpFontId, pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
								pFontInfo = pManager->GetFontInfo(hFont);
								aFontArray.Add(pFontInfo);
								pTm = &pFontInfo->tm;
								::SelectObject(hDC, pFontInfo->hFont);
								cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
							}
							ptLinkStart = pt;
							bInLink = true;
						}
						break;
					case _T('b'):  // Bold
						{
							pstrText++;
							TFontInfo* pFontInfo = pDefFontInfo;
							if( aFontArray.GetSize() > 0 ) pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
							if( pFontInfo->bBold == false ) {
								HFONT hFont = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize, true, pFontInfo->bUnderline, pFontInfo->bItalic);
								if( hFont == NULL ) hFont = pManager->AddFont(g_tmpFontId, pFontInfo->sFontName, pFontInfo->iSize, true, pFontInfo->bUnderline, pFontInfo->bItalic);
								pFontInfo = pManager->GetFontInfo(hFont);
								aFontArray.Add(pFontInfo);
								pTm = &pFontInfo->tm;
								::SelectObject(hDC, pFontInfo->hFont);
								cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
							}
						}
						break;
					case _T('c'):  // Color
						{
							pstrText++;
							while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
							if( *pstrText == _T('#')) pstrText++;
							DWORD clrColor = _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 16);
							aColorArray.Add((LPVOID)clrColor);
							::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
						}
						break;
					case _T('f'):  // Font
						{
							pstrText++;
							while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
							LPCTSTR pstrTemp = pstrText;
							int iFont = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
							if( pstrTemp != pstrText ) {
								TFontInfo* pFontInfo = pManager->GetFontInfo(iFont);
								aFontArray.Add(pFontInfo);
								pTm = &pFontInfo->tm;
								::SelectObject(hDC, pFontInfo->hFont);
							}
							else {
								QkString sFontName;
								int iFontSize = 10;
								QkString sFontAttr;
								bool bBold = false;
								bool bUnderline = false;
								bool bItalic = false;
								while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') && *pstrText != _T(' ') ) {
									pstrTemp = ::CharNext(pstrText);
									while( pstrText < pstrTemp) {
										sFontName += *pstrText++;
									}
								}
								while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
								if( isdigit(*pstrText) ) {
									iFontSize = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
								}
								while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
								while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') ) {
									pstrTemp = ::CharNext(pstrText);
									while( pstrText < pstrTemp) {
										sFontAttr += *pstrText++;
									}
								}
								sFontAttr.MakeLower();
								if( sFontAttr.Find(_T("bold")) >= 0 ) bBold = true;
								if( sFontAttr.Find(_T("underline")) >= 0 ) bUnderline = true;
								if( sFontAttr.Find(_T("italic")) >= 0 ) bItalic = true;
								HFONT hFont = pManager->GetFont(sFontName, iFontSize, bBold, bUnderline, bItalic);
								if( hFont == NULL ) hFont = pManager->AddFont(g_tmpFontId, sFontName, iFontSize, bBold, bUnderline, bItalic);
								TFontInfo* pFontInfo = pManager->GetFontInfo(hFont);
								aFontArray.Add(pFontInfo);
								pTm = &pFontInfo->tm;
								::SelectObject(hDC, pFontInfo->hFont);
							}
							cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
						}
						break;
					case _T('i'):  // Italic or Image
						{    
							pstrNextStart = pstrText - 1;
							pstrText++;
							QkString sImageString = pstrText;
							int iWidth = 0;
							int iHeight = 0;
							while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
							const TImageInfo* pImageInfo = NULL;
							QkString sName;
							while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') && *pstrText != _T(' ') ) {
								LPCTSTR pstrTemp = ::CharNext(pstrText);
								while( pstrText < pstrTemp) {
									sName += *pstrText++;
								}
							}
							if( sName.IsEmpty() ) { // Italic
								pstrNextStart = NULL;
								TFontInfo* pFontInfo = pDefFontInfo;
								if( aFontArray.GetSize() > 0 ) pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
								if( pFontInfo->bItalic == false ) {
									HFONT hFont = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, pFontInfo->bUnderline, true);
									if( hFont == NULL ) hFont = pManager->AddFont(g_tmpFontId, pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, pFontInfo->bUnderline, true);
									pFontInfo = pManager->GetFontInfo(hFont);
									aFontArray.Add(pFontInfo);
									pTm = &pFontInfo->tm;
									::SelectObject(hDC, pFontInfo->hFont);
									cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
								}
							}
							else {
								while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
								int iImageListNum = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
								if( iImageListNum <= 0 ) iImageListNum = 1;
								while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
								int iImageListIndex = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
								if( iImageListIndex < 0 || iImageListIndex >= iImageListNum ) iImageListIndex = 0;

								if( _tcsstr(sImageString.GetData(), _T("file=\'")) != NULL || _tcsstr(sImageString.GetData(), _T("res=\'")) != NULL ) {
									QkString sImageResType;
									QkString sImageName;
									LPCTSTR pStrImage = sImageString.GetData();
									QkString sItem;
									QkString sValue;
									while( *pStrImage != _T('\0') ) {
										sItem.Empty();
										sValue.Empty();
										while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
										while( *pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ') ) {
											LPTSTR pstrTemp = ::CharNext(pStrImage);
											while( pStrImage < pstrTemp) {
												sItem += *pStrImage++;
											}
										}
										while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
										if( *pStrImage++ != _T('=') ) break;
										while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
										if( *pStrImage++ != _T('\'') ) break;
										while( *pStrImage != _T('\0') && *pStrImage != _T('\'') ) {
											LPTSTR pstrTemp = ::CharNext(pStrImage);
											while( pStrImage < pstrTemp) {
												sValue += *pStrImage++;
											}
										}
										if( *pStrImage++ != _T('\'') ) break;
										if( !sValue.IsEmpty() ) {
											if( sItem == _T("file") || sItem == _T("res") ) {
												sImageName = sValue;
											}
											else if( sItem == _T("restype") ) {
												sImageResType = sValue;
											}
										}
										if( *pStrImage++ != _T(' ') ) break;
									}

									pImageInfo = pManager->GetImageEx((LPCTSTR)sImageName, sImageResType);
								}
								else
									pImageInfo = pManager->GetImageEx((LPCTSTR)sName);

								if( pImageInfo ) {
									iWidth = pImageInfo->nX;
									iHeight = pImageInfo->nY;
									if( iImageListNum > 1 ) iWidth /= iImageListNum;

									if( pt.x + iWidth > rc.right && pt.x > rc.left && (uStyle & DT_SINGLELINE) == 0 ) {
										bLineEnd = true;
									}
									else {
										pstrNextStart = NULL;
										if( bDraw && bLineDraw ) {
											CDuiRect rcImage(pt.x, pt.y + cyLineHeight - iHeight, pt.x + iWidth, pt.y + cyLineHeight);
											if( iHeight < cyLineHeight ) { 
												rcImage.bottom -= (cyLineHeight - iHeight) / 2;
												rcImage.top = rcImage.bottom -  iHeight;
											}
											CDuiRect rcBmpPart(0, 0, iWidth, iHeight);
											rcBmpPart.left = iWidth * iImageListIndex;
											rcBmpPart.right = iWidth * (iImageListIndex + 1);
											CDuiRect rcCorner(0, 0, 0, 0);
											DrawImage(hDC, pImageInfo->hBitmap, rcImage, rcImage, rcBmpPart, rcCorner, \
												pImageInfo->bAlpha, 255);
										}

										cyLine = MAX(iHeight, cyLine);
										pt.x += iWidth;
										cyMinHeight = pt.y + iHeight;
										cxMaxWidth = MAX(cxMaxWidth, pt.x);
									}
								}
								else pstrNextStart = NULL;
							}
						}
						break;
					case _T('n'):  // Newline
						{
							pstrText++;
							if( (uStyle & DT_SINGLELINE) != 0 ) break;
							bLineEnd = true;
						}
						break;
					case _T('p'):  // Paragraph
						{
							pstrText++;
							if( pt.x > rc.left ) bLineEnd = true;
							while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
							int cyLineExtra = (int)_tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
							aPIndentArray.Add((LPVOID)cyLineExtra);
							cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + cyLineExtra);
						}
						break;
					case _T('r'):  // Raw Text
						{
							pstrText++;
							bInRaw = true;
						}
						break;
					case _T('s'):  // Selected text background color
						{
							pstrText++;
							bInSelected = !bInSelected;
							if( bDraw && bLineDraw ) {
								if( bInSelected ) ::SetBkMode(hDC, OPAQUE);
								else ::SetBkMode(hDC, TRANSPARENT);
							}
						}
						break;
					case _T('u'):  // Underline text
						{
							pstrText++;
							TFontInfo* pFontInfo = pDefFontInfo;
							if( aFontArray.GetSize() > 0 ) pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
							if( pFontInfo->bUnderline == false ) {
								HFONT hFont = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
								if( hFont == NULL ) hFont = pManager->AddFont(g_tmpFontId, pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
								pFontInfo = pManager->GetFontInfo(hFont);
								aFontArray.Add(pFontInfo);
								pTm = &pFontInfo->tm;
								::SelectObject(hDC, pFontInfo->hFont);
								cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
							}
						}
						break;
					case _T('x'):  // X Indent
						{
							pstrText++;
							while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
							int iWidth = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
							pt.x += iWidth;
							cxMaxWidth = MAX(cxMaxWidth, pt.x);
						}
						break;
					case _T('y'):  // Y Indent
						{
							pstrText++;
							while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
							cyLine = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
						}
						break;
					}
					if( pstrNextStart != NULL ) pstrText = pstrNextStart;
					else {
						while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') ) pstrText = ::CharNext(pstrText);
						pstrText = ::CharNext(pstrText);
					}
			}
			else if( !bInRaw && ( *pstrText == _T('<') || *pstrText == _T('{') ) && pstrText[1] == _T('/') )
			{
				pstrText++;
				pstrText++;
				switch( *pstrText )
				{
				case _T('c'):
					{
						pstrText++;
						aColorArray.Remove(aColorArray.GetSize() - 1);
						DWORD clrColor = dwTextColor;
						if( aColorArray.GetSize() > 0 ) clrColor = (int)aColorArray.GetAt(aColorArray.GetSize() - 1);
						::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
					}
					break;
				case _T('p'):
					pstrText++;
					if( pt.x > rc.left ) bLineEnd = true;
					aPIndentArray.Remove(aPIndentArray.GetSize() - 1);
					cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
					break;
				case _T('s'):
					{
						pstrText++;
						bInSelected = !bInSelected;
						if( bDraw && bLineDraw ) {
							if( bInSelected ) ::SetBkMode(hDC, OPAQUE);
							else ::SetBkMode(hDC, TRANSPARENT);
						}
					}
					break;
				case _T('a'):
					{
						if( iLinkIndex < nLinkRects ) {
							if( !bLineDraw ) ::SetRect(&prcLinks[iLinkIndex], ptLinkStart.x, ptLinkStart.y, MIN(pt.x, rc.right), pt.y + pTm->tmHeight + pTm->tmExternalLeading);
							iLinkIndex++;
						}
						aColorArray.Remove(aColorArray.GetSize() - 1);
						DWORD clrColor = dwTextColor;
						if( aColorArray.GetSize() > 0 ) clrColor = (int)aColorArray.GetAt(aColorArray.GetSize() - 1);
						::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
						bInLink = false;
					}
				case _T('b'):
				case _T('f'):
				case _T('i'):
				case _T('u'):
					{
						pstrText++;
						aFontArray.Remove(aFontArray.GetSize() - 1);
						TFontInfo* pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
						if( pFontInfo == NULL ) pFontInfo = pDefFontInfo;
						if( pTm->tmItalic && pFontInfo->bItalic == false ) {
							ABC abc;
							::GetCharABCWidths(hDC, _T(' '), _T(' '), &abc);
							pt.x += abc.abcC / 2; // 简单修正一下斜体混排的问题, 正确做法应该是http://support.microsoft.com/kb/244798/en-us
						}
						pTm = &pFontInfo->tm;
						::SelectObject(hDC, pFontInfo->hFont);
						cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
					}
					break;
				}
				while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') ) pstrText = ::CharNext(pstrText);
				pstrText = ::CharNext(pstrText);
			}
			else if( !bInRaw &&  *pstrText == _T('<') && pstrText[2] == _T('>') && (pstrText[1] == _T('{')  || pstrText[1] == _T('}')) )
			{
				SIZE szSpace = { 0 };
				::GetTextExtentPoint32(hDC, &pstrText[1], 1, &szSpace);
				if( bDraw && bLineDraw ) ::TextOut(hDC, pt.x, pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, &pstrText[1], 1);
				pt.x += szSpace.cx;
				cxMaxWidth = MAX(cxMaxWidth, pt.x);
				pstrText++;pstrText++;pstrText++;
			}
			else if( !bInRaw &&  *pstrText == _T('{') && pstrText[2] == _T('}') && (pstrText[1] == _T('<')  || pstrText[1] == _T('>')) )
			{
				SIZE szSpace = { 0 };
				::GetTextExtentPoint32(hDC, &pstrText[1], 1, &szSpace);
				if( bDraw && bLineDraw ) ::TextOut(hDC, pt.x,  pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, &pstrText[1], 1);
				pt.x += szSpace.cx;
				cxMaxWidth = MAX(cxMaxWidth, pt.x);
				pstrText++;pstrText++;pstrText++;
			}
			else if( !bInRaw &&  *pstrText == _T(' ') )
			{
				SIZE szSpace = { 0 };
				::GetTextExtentPoint32(hDC, _T(" "), 1, &szSpace);
				// Still need to paint the space because the font might have
				// underline formatting.
				if( bDraw && bLineDraw ) ::TextOut(hDC, pt.x,  pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, _T(" "), 1);
				pt.x += szSpace.cx;
				cxMaxWidth = MAX(cxMaxWidth, pt.x);
				pstrText++;
			}
			else
			{
				POINT ptPos = pt;
				int cchChars = 0;
				int cchSize = 0;
				int cchLastGoodWord = 0;
				int cchLastGoodSize = 0;
				LPCTSTR p = pstrText;
				LPCTSTR pstrNext;
				SIZE szText = { 0 };
				if( !bInRaw && *p == _T('<') || *p == _T('{') ) p++, cchChars++, cchSize++;
				while( *p != _T('\0') && *p != _T('\n') ) {
					// This part makes sure that we're word-wrapping if needed or providing support
					// for DT_END_ELLIPSIS. Unfortunately the GetTextExtentPoint32() call is pretty
					// slow when repeated so often.
					// TODO: Rewrite and use GetTextExtentExPoint() instead!
					if( bInRaw ) {
						if( ( *p == _T('<') || *p == _T('{') ) && p[1] == _T('/') 
							&& p[2] == _T('r') && ( p[3] == _T('>') || p[3] == _T('}') ) ) {
								p += 4;
								bInRaw = false;
								break;
						}
					}
					else {
						if( *p == _T('<') || *p == _T('{') ) break;
					}
					pstrNext = ::CharNext(p);
					cchChars++;
					cchSize += (int)(pstrNext - p);
					szText.cx = cchChars * pTm->tmMaxCharWidth;
					if( pt.x + szText.cx >= rc.right ) {
						::GetTextExtentPoint32(hDC, pstrText, cchSize, &szText);
					}
					if( pt.x + szText.cx > rc.right ) {
						if( pt.x + szText.cx > rc.right && pt.x != rc.left) {
							cchChars--;
							cchSize -= (int)(pstrNext - p);
						}
						if( (uStyle & DT_WORDBREAK) != 0 && cchLastGoodWord > 0 ) {
							cchChars = cchLastGoodWord;
							cchSize = cchLastGoodSize;                 
						}
						if( (uStyle & DT_END_ELLIPSIS) != 0 && cchChars > 0 ) {
							cchChars -= 1;
							LPCTSTR pstrPrev = ::CharPrev(pstrText, p);
							if( cchChars > 0 ) {
								cchChars -= 1;
								pstrPrev = ::CharPrev(pstrText, pstrPrev);
								cchSize -= (int)(p - pstrPrev);
							}
							else 
								cchSize -= (int)(p - pstrPrev);
							pt.x = rc.right;
						}
						bLineEnd = true;
						cxMaxWidth = MAX(cxMaxWidth, pt.x);
						break;
					}
					if (!( ( p[0] >= _T('a') && p[0] <= _T('z') ) || ( p[0] >= _T('A') && p[0] <= _T('Z') ) )) {
						cchLastGoodWord = cchChars;
						cchLastGoodSize = cchSize;
					}
					if( *p == _T(' ') ) {
						cchLastGoodWord = cchChars;
						cchLastGoodSize = cchSize;
					}
					p = ::CharNext(p);
				}

				::GetTextExtentPoint32(hDC, pstrText, cchSize, &szText);
				if( bDraw && bLineDraw ) {
					if( (uStyle & DT_SINGLELINE) == 0 && (uStyle & DT_CENTER) != 0 ) {
						ptPos.x += (rc.right - rc.left - szText.cx)/2;
					}
					else if( (uStyle & DT_SINGLELINE) == 0 && (uStyle & DT_RIGHT) != 0) {
						ptPos.x += (rc.right - rc.left - szText.cx);
					}
					::TextOut(hDC, ptPos.x, ptPos.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, pstrText, cchSize);
					if( pt.x >= rc.right && (uStyle & DT_END_ELLIPSIS) != 0 ) 
						::TextOut(hDC, ptPos.x + szText.cx, ptPos.y, _T("..."), 3);
				}
				pt.x += szText.cx;
				cxMaxWidth = MAX(cxMaxWidth, pt.x);
				pstrText += cchSize;
			}

			if( pt.x >= rc.right || *pstrText == _T('\n') || *pstrText == _T('\0') ) bLineEnd = true;
			if( bDraw && bLineEnd ) {
				if( !bLineDraw ) {
					aFontArray.Resize(aLineFontArray.GetSize());
					::CopyMemory(aFontArray.GetData(), aLineFontArray.GetData(), aLineFontArray.GetSize() * sizeof(LPVOID));
					aColorArray.Resize(aLineColorArray.GetSize());
					::CopyMemory(aColorArray.GetData(), aLineColorArray.GetData(), aLineColorArray.GetSize() * sizeof(LPVOID));
					aPIndentArray.Resize(aLinePIndentArray.GetSize());
					::CopyMemory(aPIndentArray.GetData(), aLinePIndentArray.GetData(), aLinePIndentArray.GetSize() * sizeof(LPVOID));

					cyLineHeight = cyLine;
					pstrText = pstrLineBegin;
					bInRaw = bLineInRaw;
					bInSelected = bLineInSelected;

					DWORD clrColor = dwTextColor;
					if( aColorArray.GetSize() > 0 ) clrColor = (int)aColorArray.GetAt(aColorArray.GetSize() - 1);
					::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
					TFontInfo* pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
					if( pFontInfo == NULL ) pFontInfo = pDefFontInfo;
					pTm = &pFontInfo->tm;
					::SelectObject(hDC, pFontInfo->hFont);
					if( bInSelected ) ::SetBkMode(hDC, OPAQUE);
				}
				else {
					aLineFontArray.Resize(aFontArray.GetSize());
					::CopyMemory(aLineFontArray.GetData(), aFontArray.GetData(), aFontArray.GetSize() * sizeof(LPVOID));
					aLineColorArray.Resize(aColorArray.GetSize());
					::CopyMemory(aLineColorArray.GetData(), aColorArray.GetData(), aColorArray.GetSize() * sizeof(LPVOID));
					aLinePIndentArray.Resize(aPIndentArray.GetSize());
					::CopyMemory(aLinePIndentArray.GetData(), aPIndentArray.GetData(), aPIndentArray.GetSize() * sizeof(LPVOID));
					pstrLineBegin = pstrText;
					bLineInSelected = bInSelected;
					bLineInRaw = bInRaw;
				}
			}

			ASSERT(iLinkIndex<=nLinkRects);
		}

		nLinkRects = iLinkIndex;

		// Return size of text when requested
		if( (uStyle & DT_CALCRECT) != 0 ) {
			rc.bottom = MAX(cyMinHeight, pt.y + cyLine);
			rc.right = MIN(rc.right, cxMaxWidth);
		}

		if( bDraw ) ::SelectClipRgn(hDC, hOldRgn);
		::DeleteObject(hOldRgn);
		::DeleteObject(hRgn);

		::SelectObject(hDC, hOldFont);
	}
} // namespace DuiLib
