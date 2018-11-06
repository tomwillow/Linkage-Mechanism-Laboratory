#pragma once
#include <Windows.h>  

int ReadRegKey(HKEY root, TCHAR * subDir, TCHAR * regKey, TCHAR* regValue);
int WriteRegKey(HKEY root, TCHAR * subDir, TCHAR * regKey,const TCHAR * regValue);
int DeleteRegTree(HKEY root, TCHAR * subDir);

int UnRegisterFileAssociation(TCHAR *strExt, TCHAR *strAppKey);
bool CheckFileAssociation(TCHAR *strAppKey, TCHAR *strAppName);
int RegisterFileAssociation(TCHAR *strExt, TCHAR *strAppKey, TCHAR *strAppName, TCHAR *strDefaultIcon, int iResourceId,const TCHAR *strDescribe);
