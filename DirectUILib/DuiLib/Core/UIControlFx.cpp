#include "StdAfx.h"

namespace DuiLib {
	class CEffectLock
	{
		HANDLE	mMutex;
	public:
		CEffectLock() {mMutex = CreateMutex(NULL,FALSE,NULL);}
		virtual ~CEffectLock() {CloseHandle(mMutex);}
		void Lock() {WaitForSingleObject(mMutex,INFINITE);}
		void UnLock() {ReleaseMutex(mMutex);}
	};

	CEffectLock mEffectLock;


	QkString CControlUI::GetEffectStyle()
	{
		return m_strEffectStyle;
	}

	void CControlUI::SetAnimEffects( bool bEnableEffect )
	{
		VIEWSTATEMASK_APPLY(VIEWSTATEMASK_EnabledEffect, bEnableEffect);
	}

	bool CControlUI::GetAnimEffects()
	{
		return m_bEnabledEffect;
	}

	void CControlUI::SetEffectsZoom( int iZoom )
	{
		GetCurrentEffect().m_iZoom = iZoom;
	}

	int CControlUI::GetEffectsZoom()
	{
		return GetCurrentEffect().m_iZoom;
	}

	void CControlUI::SetEffectsFillingBK( DWORD dFillingBK )
	{
		GetCurrentEffect().m_dFillingBK = dFillingBK;
	}

	DWORD CControlUI::GetEffectsFillingBK()
	{
		return GetCurrentEffect().m_dFillingBK;
	}

	//************************************
	// Method:    SetEffectsOffectX
	// FullName:  CControlUI::SetEffectsOffectX
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: int iOffectX
	// Note:	  
	//************************************
	void CControlUI::SetEffectsOffectX( int iOffectX )
	{
		GetCurrentEffect().m_iOffectX = iOffectX;
	}


	int CControlUI::GetEffectsOffectX()
	{
		return GetCurrentEffect().m_iOffectX;
	}

	void CControlUI::SetEffectsOffectY( int iOffectY )
	{
		GetCurrentEffect().m_iOffectY = iOffectY;
	}


	int CControlUI::GetEffectsOffectY()
	{
		return GetCurrentEffect().m_iOffectY;
	}


	void CControlUI::SetEffectsAlpha( int iAlpha )
	{
		GetCurrentEffect().m_iAlpha = iAlpha;
	}


	int CControlUI::GetEffectsAlpha()
	{
		return GetCurrentEffect().m_iAlpha;
	}


	void CControlUI::SetEffectsRotation( float fRotation )
	{
		GetCurrentEffect().m_fRotation = fRotation;
	}


	float CControlUI::GetEffectsRotation()
	{
		return GetCurrentEffect().m_fRotation;
	}


	void CControlUI::SetEffectsNeedTimer( int iNeedTimer )
	{
		GetCurrentEffect().m_iNeedTimer = iNeedTimer;
	}


	int CControlUI::GetEffectsNeedTimer()
	{
		return GetCurrentEffect().m_iNeedTimer;
	}

	bool CControlUI::GetEffectEnabled(int fx_section)
	{
		return effects.size()>fx_section && effects[fx_section].m_bEnableEffect;
	}

	vector<TEffectAge> & CControlUI::GetEffects()
	{
		if (!effects.size())
		{
			effects.resize(5); // only resize once.
			TEffectAge& m_tCurEffects = effects[0];
			m_tCurEffects.m_bEnableEffect	= false;
			m_tCurEffects.m_iZoom			= -1;
			m_tCurEffects.m_dFillingBK		= 0xffffffff;
			m_tCurEffects.m_iOffectX		= 0;
			m_tCurEffects.m_iOffectY		= 0;
			m_tCurEffects.m_iAlpha			= -255;
			m_tCurEffects.m_fRotation		= 0.0;
			m_tCurEffects.m_iNeedTimer		= 350;

			memcpy(&effects[1],&m_tCurEffects,sizeof(TEffectAge));
			memcpy(&effects[2],&m_tCurEffects,sizeof(TEffectAge));
			memcpy(&effects[3],&m_tCurEffects,sizeof(TEffectAge));
		}
		return effects;
	}

	TEffectAge & CControlUI::GetCurrentEffect()
	{
		return GetEffects()[0];
	}

	TEffectAge* CControlUI::GetMouseInEffect()
	{
		return &GetEffects()[1];
	}


	TEffectAge* CControlUI::GetMouseOutEffect()
	{
		return &GetEffects()[2];
	}


	TEffectAge* CControlUI::GetClickInEffect()
	{
		return &GetEffects()[3];
	}

	void CControlUI::SetEffectsStyle( LPCTSTR pstrEffectStyle,TEffectAge* pTEffectAge /*= NULL*/ )
	{
		try
		{
			m_strEffectStyle = GetManager()->GetEffectsStyle(pstrEffectStyle);

			if(m_strEffectStyle.IsEmpty() && pstrEffectStyle)
			{
				AnyEffectsAdvProfiles(pstrEffectStyle,pTEffectAge);
				AnyEasyEffectsPorfiles(pstrEffectStyle,pTEffectAge);
			}
			else if(!m_strEffectStyle.IsEmpty())
			{
				AnyEffectsAdvProfiles(m_strEffectStyle,pTEffectAge);
				AnyEasyEffectsPorfiles(m_strEffectStyle,pTEffectAge);
			}
		}
		catch (...)
		{
			throw "CControlUI::SetEffectsStyle";
		}
	}

	void CControlUI::AnyEffectsAdvProfiles( LPCTSTR pstrEffects,TEffectAge* pTEffectAge /*= NULL*/ )
	{
		try
		{
			QkString sItem;
			QkString sValue;
			LPCTSTR pstr = NULL;

			TEffectAge* pcTEffectAge = pTEffectAge?pTEffectAge:&GetCurrentEffect();

			while( *pstrEffects != _T('\0') ) {
				sItem.Empty();
				sValue.Empty();
				while( *pstrEffects > _T('\0') && *pstrEffects <= _T(' ') ) pstrEffects = ::CharNext(pstrEffects);
				while( *pstrEffects != _T('\0') && *pstrEffects != _T('=') && *pstrEffects > _T(' ') ) {
					LPTSTR pstrTemp = ::CharNext(pstrEffects);
					while( pstrEffects < pstrTemp) {
						sItem += *pstrEffects++;
					}
				}
				while( *pstrEffects > _T('\0') && *pstrEffects <= _T(' ') ) pstrEffects = ::CharNext(pstrEffects);
				if( *pstrEffects++ != _T('=') ) break;
				while( *pstrEffects > _T('\0') && *pstrEffects <= _T(' ') ) pstrEffects = ::CharNext(pstrEffects);
				if( *pstrEffects++ != _T('\'') ) break;
				while( *pstrEffects != _T('\0') && *pstrEffects != _T('\'') ) {
					LPTSTR pstrTemp = ::CharNext(pstrEffects);
					while( pstrEffects < pstrTemp) {
						sValue += *pstrEffects++;
					}
				}
				if( *pstrEffects++ != _T('\'') ) break;
				if( !sValue.IsEmpty() ) {

					//
					// // ------------- 有点、奇怪。
					// 
					
					//TCHAR buffer[200]={0};
					//wsprintf(buffer,TEXT("position=%s %d"), sItem, sItem == _T("anim"));
					//::MessageBox(NULL, buffer, TEXT(""), MB_OK);


					if( sItem == _T("zoom") ) { 
						pcTEffectAge->m_iZoom = (_ttoi(sValue.GetData()));
					}
					else if( sItem == _T("fillingbk") ){
						if(sValue == _T("none"))
							sValue == _T("#ffffffff");

						if( *sValue.GetData() == _T('#'))
							sValue = ::CharNext(sValue.GetData());

						pstr = STR2ARGB(sValue.GetData(), pcTEffectAge->m_dFillingBK);
						ASSERT(pstr);
					}
					else if( sItem == _T("offsetx") ) pcTEffectAge->m_iOffectX = (_ttoi(sValue.GetData()));
					else if( sItem == _T("offsety") ) pcTEffectAge->m_iOffectY = (_ttoi(sValue.GetData()));
					else if( sItem == _T("alpha") ) pcTEffectAge->m_iAlpha = (_ttoi(sValue.GetData()));
					else if( sItem == _T("rotation") ) pcTEffectAge->m_fRotation = (float)(_tstof(sValue.GetData()));
					else if( sItem == _T("needtimer") ) pcTEffectAge->m_iNeedTimer = (_ttoi(sValue.GetData()));
				}
				if( *pstrEffects++ != _T(' ') ) break;
			}
			pcTEffectAge->m_bEnableEffect = true;
		}
		catch (...)
		{
			throw "CControlUI::AnyEffectsAdvProfiles";
		}
	}


	void CControlUI::AnyEasyEffectsPorfiles( LPCTSTR pstrEffects,TEffectAge* pTEffectAge /*= NULL*/ )
	{
		try
		{
			QkString sItem;
			QkString sValue;
			QkString sAnim;
			LPCTSTR pstr = NULL;

			TEffectAge* pcTEffectAge = pTEffectAge?pTEffectAge:&GetCurrentEffect();

			while( *pstrEffects != _T('\0') ) {
				sItem.Empty();
				sValue.Empty();
				while( *pstrEffects > _T('\0') && *pstrEffects <= _T(' ') ) pstrEffects = ::CharNext(pstrEffects);
				while( *pstrEffects != _T('\0') && *pstrEffects != _T('=') && *pstrEffects > _T(' ') ) {
					LPTSTR pstrTemp = ::CharNext(pstrEffects);
					while( pstrEffects < pstrTemp) {
						sItem += *pstrEffects++;
					}
				}
				while( *pstrEffects > _T('\0') && *pstrEffects <= _T(' ') ) pstrEffects = ::CharNext(pstrEffects);
				if( *pstrEffects++ != _T('=') ) break;
				while( *pstrEffects > _T('\0') && *pstrEffects <= _T(' ') ) pstrEffects = ::CharNext(pstrEffects);
				if( *pstrEffects++ != _T('\'') ) break;
				while( *pstrEffects != _T('\0') && *pstrEffects != _T('\'') ) {
					LPTSTR pstrTemp = ::CharNext(pstrEffects);
					while( pstrEffects < pstrTemp) {
						sValue += *pstrEffects++;
					}
				}
				if( *pstrEffects++ != _T('\'') ) break;
				if( !sValue.IsEmpty() ) {
					if( sItem == _T("anim") ){
						sAnim = sValue;

						if(sValue == _T("zoom+")){
							if(pcTEffectAge->m_iZoom > 0)	pcTEffectAge->m_iZoom = (pcTEffectAge->m_iZoom - pcTEffectAge->m_iZoom*2);
							if(pcTEffectAge->m_iZoom == 0) pcTEffectAge->m_iZoom = -1;
							pcTEffectAge->m_iAlpha = -255;
							pcTEffectAge->m_fRotation = 0.0;
						}
						else if(sValue == _T("zoom-")){
							if(pcTEffectAge->m_iZoom < 0)	pcTEffectAge->m_iZoom = (pcTEffectAge->m_iZoom - pcTEffectAge->m_iZoom*2);
							if(pcTEffectAge->m_iZoom == 0) pcTEffectAge->m_iZoom = 1;
							pcTEffectAge->m_iAlpha = 255;
							pcTEffectAge->m_fRotation = 0.0;
						}
						else if(sValue == _T("left2right")){
							if(pcTEffectAge->m_iOffectX > 0)	pcTEffectAge->m_iOffectX = (pcTEffectAge->m_iOffectX - pcTEffectAge->m_iOffectX*2);
							pcTEffectAge->m_iAlpha = 255;
							pcTEffectAge->m_iZoom = 0;
							pcTEffectAge->m_iOffectY = 0;
							pcTEffectAge->m_fRotation = 0.0;
						}
						else if(sValue == _T("right2left")){
							if(pcTEffectAge->m_iOffectX < 0)	pcTEffectAge->m_iOffectX = (pcTEffectAge->m_iOffectX - pcTEffectAge->m_iOffectX*2);
							pcTEffectAge->m_iAlpha = 255;
							pcTEffectAge->m_iZoom = 0;
							pcTEffectAge->m_iOffectY = 0;
							pcTEffectAge->m_fRotation = 0.0;
						}
						else if(sValue == _T("top2bottom")){
							if(pcTEffectAge->m_iOffectY > 0)	pcTEffectAge->m_iOffectY = (pcTEffectAge->m_iOffectY - pcTEffectAge->m_iOffectY*2);
							pcTEffectAge->m_iAlpha = 255;
							pcTEffectAge->m_iZoom = 0;
							pcTEffectAge->m_iOffectX = 0;
							pcTEffectAge->m_fRotation = 0.0;
						}
						else if(sValue == _T("bottom2top")){
							if(pcTEffectAge->m_iOffectY < 0)	pcTEffectAge->m_iOffectY = (pcTEffectAge->m_iOffectY - pcTEffectAge->m_iOffectY*2);
							pcTEffectAge->m_iAlpha = 255;
							pcTEffectAge->m_iZoom = 0;
							pcTEffectAge->m_iOffectX = 0;
							pcTEffectAge->m_fRotation = 0.0;
						}
					}
					else if( sItem == _T("offset") ){
						if(sAnim == _T("zoom+") || sAnim == _T("zoom-")){
							pcTEffectAge->m_iOffectX = 0;
							pcTEffectAge->m_iOffectY = 0;
						}
						else if(sAnim == _T("left2right") || sAnim == _T("right2left")){
							pcTEffectAge->m_iOffectX = _ttoi(sValue.GetData());
							pcTEffectAge->m_iOffectY = 0;

							if(sAnim == _T("left2right"))
								if(pcTEffectAge->m_iOffectX > 0)	pcTEffectAge->m_iOffectX = (pcTEffectAge->m_iOffectX - pcTEffectAge->m_iOffectX*2);
								else if(sAnim == _T("right2left"))
									if(pcTEffectAge->m_iOffectX < 0)	pcTEffectAge->m_iOffectX = (pcTEffectAge->m_iOffectX - pcTEffectAge->m_iOffectX*2);

						}
						else if(sAnim == _T("top2bottom") || sAnim == _T("bottom2top")){
							pcTEffectAge->m_iOffectX = 0;
							pcTEffectAge->m_iOffectY = _ttoi(sValue.GetData());

							if(sAnim == _T("top2bottom"))
								if(pcTEffectAge->m_iOffectY > 0)	pcTEffectAge->m_iOffectY = (pcTEffectAge->m_iOffectY - pcTEffectAge->m_iOffectY*2);
							if(sAnim == _T("bottom2top"))
								if(pcTEffectAge->m_iOffectY < 0)	pcTEffectAge->m_iOffectY = (pcTEffectAge->m_iOffectY - pcTEffectAge->m_iOffectY*2);
						}
					}
					else if( sItem == _T("needtimer") ) pcTEffectAge->m_iNeedTimer = (_ttoi(sValue.GetData()));
					else if( sItem == _T("fillingbk") ){
						if(sValue == _T("none"))
							sValue == _T("#ffffffff");

						if( *sValue.GetData() == _T('#'))
							sValue = ::CharNext(sValue.GetData());

						pstr = STR2ARGB(sValue.GetData(), pcTEffectAge->m_dFillingBK);
						ASSERT(pstr);
					}
				}
				if( *pstrEffects++ != _T(' ') ) break;
			}
			pcTEffectAge->m_bEnableEffect = true;
		}
		catch (...)
		{
			throw "CControlUI::AnyEasyEffectsPorfiles";
		}
	}


	void CControlUI::TriggerEffects( TEffectAge* pTEffectAge /*= NULL*/ )
	{
		try
		{
			try
			{
				//mEffectLock.Lock();
				TEffectAge* pcTEffect = pTEffectAge?pTEffectAge:&GetCurrentEffect();

				if(GetManager() && m_bEnabledEffect && pcTEffect->m_bEnableEffect)
				{
					//GetManager()->AddAnimationJob(CDxAnimationUI(UIANIMTYPE_FLAT,0,GetEffectsNeedTimer(),GetEffectsFillingBK(),GetEffectsFillingBK(),GetPos(),GetEffectsOffectX(),GetEffectsOffectY(),GetEffectsZoom(),GetEffectsAlpha(),GetEffectsRotation()));
					GetManager()->AddAnimationJob(CDxAnimationUI(UIANIMTYPE_FLAT,0,pcTEffect->m_iNeedTimer,pcTEffect->m_dFillingBK,pcTEffect->m_dFillingBK,GetPos(),pcTEffect->m_iOffectX,pcTEffect->m_iOffectY,pcTEffect->m_iZoom,pcTEffect->m_iAlpha,(float)pcTEffect->m_fRotation));
				}
				//mEffectLock.UnLock();
			}
			catch (...)
			{
				throw "CControlUI::TriggerEffects";
			}
		}
		catch (...)
		{
			throw "CControlUI::TriggerEffects";
		}
	}

}