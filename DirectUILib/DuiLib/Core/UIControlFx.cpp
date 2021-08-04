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



	//************************************
	// Method:    GetEffectStyle
	// FullName:  CControlUI::GetEffectStyle
	// Access:    virtual public 
	// Returns:   CDuiString
	// Qualifier: const
	// Note:	  
	//************************************
	CDuiString CControlUI::GetEffectStyle() const
	{
		try
		{
			return m_strEffectStyle;
		}
		catch (...)
		{
			throw "CControlUI::GetEffectStyle";
		}
	}

	//************************************
	// Method:    SetAnimEffects
	// FullName:  CControlUI::SetAnimEffects
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: bool bEnableEffect
	// Note:	  
	//************************************
	void CControlUI::SetAnimEffects( bool bEnableEffect )
	{
		try
		{
			m_bEnabledEffect = bEnableEffect;
		}
		catch (...)
		{
			throw "CControlUI::SetAnimEffects";
		}
	}

	//************************************
	// Method:    GetAnimEffects
	// FullName:  CControlUI::GetAnimEffects
	// Access:    virtual public 
	// Returns:   bool
	// Qualifier: const
	// Note:	  
	//************************************
	bool CControlUI::GetAnimEffects() const
	{
		try
		{
			return m_bEnabledEffect;
		}
		catch (...)
		{
			throw "CControlUI::GetAnimEffects";
		}
	}
	//************************************
	// Method:    SetEffectsZoom
	// FullName:  CControlUI::SetEffectsZoom
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: int iZoom
	// Note:	  
	//************************************
	void CControlUI::SetEffectsZoom( int iZoom )
	{
		try
		{
			m_tCurEffects.m_iZoom = iZoom;
		}
		catch (...)
		{
			throw "CControlUI::SetEffectsZoom";
		}
	}

	//************************************
	// Method:    GetEffectsZoom
	// FullName:  CControlUI::GetEffectsZoom
	// Access:    virtual public 
	// Returns:   int
	// Qualifier: const
	// Note:	  
	//************************************
	int CControlUI::GetEffectsZoom() const
	{
		try
		{
			return m_tCurEffects.m_iZoom;
		}
		catch (...)
		{
			throw "CControlUI::GetEffectsZoom";
		}
	}

	//************************************
	// Method:    SetEffectsFillingBK
	// FullName:  CControlUI::SetEffectsFillingBK
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: DWORD dFillingBK
	// Note:	  
	//************************************
	void CControlUI::SetEffectsFillingBK( DWORD dFillingBK )
	{
		try
		{
			m_tCurEffects.m_dFillingBK = dFillingBK;
		}
		catch (...)
		{
			throw "CControlUI::SetEffectsFillingBK";
		}
	}

	//************************************
	// Method:    GetEffectsFillingBK
	// FullName:  CControlUI::GetEffectsFillingBK
	// Access:    virtual public 
	// Returns:   DWORD
	// Qualifier: const
	// Note:	  
	//************************************
	DWORD CControlUI::GetEffectsFillingBK() const
	{
		try
		{
			return m_tCurEffects.m_dFillingBK;
		}
		catch (...)
		{
			throw "CControlUI::GetEffectsFillingBK";
		}
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
		try
		{
			m_tCurEffects.m_iOffectX = iOffectX;
		}
		catch (...)
		{
			throw "CControlUI::SetEffectsOffectX";
		}
	}

	//************************************
	// Method:    GetEffectsOffectX
	// FullName:  CControlUI::GetEffectsOffectX
	// Access:    virtual public 
	// Returns:   int
	// Qualifier: const
	// Note:	  
	//************************************
	int CControlUI::GetEffectsOffectX() const
	{
		try
		{
			return m_tCurEffects.m_iOffectX;
		}
		catch (...)
		{
			throw "CControlUI::GetEffectsOffectX";
		}
	}

	//************************************
	// Method:    SetEffectsOffectY
	// FullName:  CControlUI::SetEffectsOffectY
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: int iOffectY
	// Note:	  
	//************************************
	void CControlUI::SetEffectsOffectY( int iOffectY )
	{
		try
		{
			m_tCurEffects.m_iOffectY = iOffectY;
		}
		catch (...)
		{
			throw "CControlUI::SetEffectsOffectY";
		}
	}

	//************************************
	// Method:    GetEffectsOffectY
	// FullName:  CControlUI::GetEffectsOffectY
	// Access:    virtual public 
	// Returns:   int
	// Qualifier: const
	// Note:	  
	//************************************
	int CControlUI::GetEffectsOffectY() const
	{
		try
		{
			return m_tCurEffects.m_iOffectY;
		}
		catch (...)
		{
			throw "CControlUI::GetEffectsOffectY";
		}
	}

	//************************************
	// Method:    SetEffectsAlpha
	// FullName:  CControlUI::SetEffectsAlpha
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: int iAlpha
	// Note:	  
	//************************************
	void CControlUI::SetEffectsAlpha( int iAlpha )
	{
		try
		{
			m_tCurEffects.m_iAlpha = iAlpha;
		}
		catch (...)
		{
			throw "CControlUI::SetEffectsAlpha";
		}
	}

	//************************************
	// Method:    GetEffectsAlpha
	// FullName:  CControlUI::GetEffectsAlpha
	// Access:    virtual public 
	// Returns:   int
	// Qualifier: const
	// Note:	  
	//************************************
	int CControlUI::GetEffectsAlpha() const
	{
		try
		{
			return m_tCurEffects.m_iAlpha;
		}
		catch (...)
		{
			throw "CControlUI::GetEffectsAlpha";
		}
	}

	//************************************
	// Method:    SetEffectsRotation
	// FullName:  CControlUI::SetEffectsRotation
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: float fRotation
	// Note:	  
	//************************************
	void CControlUI::SetEffectsRotation( float fRotation )
	{
		try
		{
			m_tCurEffects.m_fRotation = fRotation;
		}
		catch (...)
		{
			throw "CControlUI::SetEffectsRotation";
		}
	}

	//************************************
	// Method:    GetEffectRotation
	// FullName:  CControlUI::GetEffectsRotation
	// Access:    virtual public 
	// Returns:   float
	// Qualifier: 
	// Note:	  
	//************************************
	float CControlUI::GetEffectsRotation()
	{
		try
		{
			return m_tCurEffects.m_fRotation;
		}
		catch (...)
		{
			throw "CControlUI::GetEffectsRotation";
		}
	}

	//************************************
	// Method:    SetEffectsNeedTimer
	// FullName:  CControlUI::SetEffectsNeedTimer
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: int iNeedTimer
	// Note:	  
	//************************************
	void CControlUI::SetEffectsNeedTimer( int iNeedTimer )
	{
		try
		{
			m_tCurEffects.m_iNeedTimer = iNeedTimer;
		}
		catch (...)
		{
			throw "CControlUI::SetEffectsNeedTimer";
		}
	}

	//************************************
	// Method:    GetEffectsNeedTimer
	// FullName:  CControlUI::GetEffectsNeedTimer
	// Access:    virtual public 
	// Returns:   int
	// Qualifier:
	// Note:	  
	//************************************
	int CControlUI::GetEffectsNeedTimer()
	{
		try
		{
			return m_tCurEffects.m_iNeedTimer;
		}
		catch (...)
		{
			throw "CControlUI::GetEffectsNeedTimer";
		}
	}


	//************************************
	// Method:    GetCurEffects
	// FullName:  CControlUI::GetCurEffects
	// Access:    virtual public 
	// Returns:   TEffectAge*
	// Qualifier:
	// Note:	  
	//************************************
	TEffectAge* CControlUI::GetCurEffects()
	{
		try
		{
			return &m_tCurEffects;
		}
		catch (...)
		{
			throw "CControlUI::GetCurEffects";
		}
	}

	//************************************
	// Method:    GetMouseInEffect
	// FullName:  CControlUI::GetMouseInEffect
	// Access:    virtual public 
	// Returns:   TEffectAge*
	// Qualifier:
	// Note:	  
	//************************************
	TEffectAge* CControlUI::GetMouseInEffect()
	{
		try
		{
			return &m_tMouseInEffects;
		}
		catch (...)
		{
			throw "CControlUI::GetMouseInEffect";
		}
	}

	//************************************
	// Method:    GetMouseOutEffect
	// FullName:  CControlUI::GetMouseOutEffect
	// Access:    virtual public 
	// Returns:   TEffectAge*
	// Qualifier:
	// Note:	  
	//************************************
	TEffectAge* CControlUI::GetMouseOutEffect()
	{
		try
		{
			return &m_tMouseOutEffects;
		}
		catch (...)
		{
			throw "CControlUI::GetMouseOutEffect";
		}
	}

	//************************************
	// Method:    GetClickInEffect
	// FullName:  CControlUI::GetClickInEffect
	// Access:    virtual public 
	// Returns:   TEffectAge*
	// Qualifier:
	// Note:	  
	//************************************
	TEffectAge* CControlUI::GetClickInEffect()
	{
		try
		{
			return &m_tMouseClickEffects;
		}
		catch (...)
		{
			throw "CControlUI::GetClickInEffect";
		}
	}

	//************************************
	// Method:    SetEffectsStyle
	// FullName:  CControlUI::SetEffectsStyle
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: LPCTSTR pstrEffectStyle
	// Parameter: TEffectAge * pTEffectAge
	// Note:	  
	//************************************
	void CControlUI::SetEffectsStyle( LPCTSTR pstrEffectStyle,TEffectAge* pTEffectAge /*= NULL*/ )
	{
		try
		{
			m_strEffectStyle = GetManager()->GetEffectsStyle(pstrEffectStyle);

			if(m_strEffectStyle.IsEmpty() && pstrEffectStyle)
			{
				 //TCHAR buffer[100]={0};
				 //wsprintf(buffer,TEXT("position=%s"),  pstrEffectStyle);
				 //::MessageBox(NULL, buffer, TEXT(""), MB_OK);

				AnyEffectsAdvProfiles(pstrEffectStyle,pTEffectAge);
				//AnyEasyEffectsPorfiles(pstrEffectStyle,pTEffectAge);
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

	//************************************
	// Method:    AnyEffectsAdvProfiles
	// FullName:  CControlUI::AnyEffectsAdvProfiles
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: LPCTSTR pstrEffects
	// Parameter: TEffectAge * pTEffectAge
	//************************************
	void CControlUI::AnyEffectsAdvProfiles( LPCTSTR pstrEffects,TEffectAge* pTEffectAge /*= NULL*/ )
	{
		try
		{
			CDuiString sItem;
			CDuiString sValue;
			LPTSTR pstr = NULL;

			TEffectAge* pcTEffectAge = pTEffectAge?pTEffectAge:&m_tCurEffects;

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

						pcTEffectAge->m_dFillingBK = (_tcstoul(sValue.GetData(),&pstr,16));
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


	//************************************
	// Method:    AnyEasyEffectsPorfiles
	// FullName:  CControlUI::AnyEasyEffectsPorfiles
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: LPCTSTR pstrEffects
	// Parameter: TEffectAge * pTEffectAge
	//************************************
	void CControlUI::AnyEasyEffectsPorfiles( LPCTSTR pstrEffects,TEffectAge* pTEffectAge /*= NULL*/ )
	{
		try
		{
			CDuiString sItem;
			CDuiString sValue;
			CDuiString sAnim;
			LPTSTR pstr = NULL;

			TEffectAge* pcTEffectAge = pTEffectAge?pTEffectAge:&m_tCurEffects;

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

						pcTEffectAge->m_dFillingBK = _tcstoul(sValue.GetData(),&pstr,16);
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

	//************************************
	// Method:    TriggerEffects
	// FullName:  CControlUI::TriggerEffects
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: TEffectAge * pTEffectAge
	// Note:	  
	//************************************
	void CControlUI::TriggerEffects( TEffectAge* pTEffectAge /*= NULL*/ )
	{
		try
		{
			try
			{
				mEffectLock.Lock();
				TEffectAge* pcTEffect = pTEffectAge?pTEffectAge:&m_tCurEffects;

				if(GetManager() && m_bEnabledEffect && pcTEffect->m_bEnableEffect)
				{
					//GetManager()->AddAnimationJob(CDxAnimationUI(UIANIMTYPE_FLAT,0,GetEffectsNeedTimer(),GetEffectsFillingBK(),GetEffectsFillingBK(),GetPos(),GetEffectsOffectX(),GetEffectsOffectY(),GetEffectsZoom(),GetEffectsAlpha(),GetEffectsRotation()));
					bool ret = GetManager()->AddAnimationJob(CDxAnimationUI(UIANIMTYPE_FLAT,0,pcTEffect->m_iNeedTimer,pcTEffect->m_dFillingBK,pcTEffect->m_dFillingBK,GetPos(),pcTEffect->m_iOffectX,pcTEffect->m_iOffectY,pcTEffect->m_iZoom,pcTEffect->m_iAlpha,(float)pcTEffect->m_fRotation));
				}
				mEffectLock.UnLock();
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