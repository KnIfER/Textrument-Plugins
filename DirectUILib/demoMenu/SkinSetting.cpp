#include "StdAfx.h"
#include "SkinSetting.h"


CSkinSetting::CSkinSetting(LPCTSTR lpSkinFile,LPCTSTR lpSkinFolder,UILIB_RESOURCETYPE iResType,LPCTSTR lpResID,LPCTSTR lpZipFileName)
:
m_SkinFile(lpSkinFile),
m_SkinFolder(lpSkinFolder),
m_ResourceType(iResType),
m_ResourceID(lpResID),
m_ZIPFileName(lpZipFileName)
{
}


CSkinSetting::~CSkinSetting(void)
{
}

QkString CSkinSetting::GetSkinFile()
{
	return m_SkinFile;
}

QkString CSkinSetting::GetSkinFolder()
{
	return m_SkinFolder;
}

UILIB_RESOURCETYPE CSkinSetting::GetResourceType() const
{
	return m_ResourceType;
}

LPCTSTR CSkinSetting::GetResourceID() const
{
	return m_ResourceID;
}

QkString CSkinSetting::GetZIPFileName() const
{
	return m_ZIPFileName;
}