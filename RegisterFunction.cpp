#include "tchar_head.h"
#include "RegisterFunction.h"

#include <Windows.h>

#include <shlobj.h>//SHChangeNotify
#include <Shlwapi.h>//SHDeleteKey
#pragma comment(lib,"Advapi32.lib")//RegCreateKeyEx
#pragma comment(lib,"Shlwapi.lib")//SHDeleteKey
#pragma comment(lib,"Shell32.lib")//SHChangeNotify

//-------------------------------------------------------------------------  
// 检查文件关联  
//-------------------------------------------------------------------------  
// @strAppKey           [in]: 扩展名在注册表中的键值(txtfile)  
// @strAppName          [in]: 需要关联的应用程序(c:\app\app.exe)  
//-------------------------------------------------------------------------  
// @return              : true表示成功
//-------------------------------------------------------------------------  
bool CheckFileAssociation(TCHAR *strAppKey, TCHAR *strAppName)
{
	//Shell\Open\Command xxx.exe
	TCHAR strKeyPath[MAX_PATH];
	TCHAR strValue[MAX_PATH];
	TCHAR strValueReal[MAX_PATH];
	wsprintf(strKeyPath, TEXT("%s\\Shell\\Open\\Command"), strAppKey);
	wsprintf(strValue, TEXT("\"%s\" \"%%1\""), strAppName);

	if (ReadRegKey(HKEY_CLASSES_ROOT, strKeyPath, TEXT(""), strValueReal) == ERROR_SUCCESS)
	{
		//success
		if (_tcscmp(strValue, strValueReal) == 0)
			return true;
		else
			return false;
	}
	else
		return false;
}

//-------------------------------------------------------------------------  
// 注册文件图标关联  
//-------------------------------------------------------------------------  
// @strExt              [in]: 需要检测的文件后缀(.txt)  
// @strAppKey           [in]: 扩展名在注册表中的键值(txtfile)  
// @strAppName          [in]: 需要关联的应用程序(c:\app\app.exe)  
// @strDefaultIcon      [in]: 关联的图标文件(ico exe dll)  
// @iResourceId         [in]: 图标文件在资源中的编号(101)  
// @strDescribe         [in]: 描述  
//-------------------------------------------------------------------------  
// @return              : 0表示成功，其他表示错误码  
//-------------------------------------------------------------------------  
int RegisterFileAssociation(TCHAR *strExt, TCHAR *strAppKey, TCHAR *strAppName, TCHAR *strDefaultIcon, int iResourceId, TCHAR *strDescribe)
{
	TCHAR strTemp[MAX_PATH];
	long ret;

	//.txt
	ret = WriteRegKey(HKEY_CLASSES_ROOT, strExt, TEXT(""), strAppKey);
	if (ret != ERROR_SUCCESS) return ret;

	//描述
	ret = WriteRegKey(HKEY_CLASSES_ROOT, strAppKey, TEXT(""), strDescribe);
	if (ret != ERROR_SUCCESS) return ret;

	//图标
	wsprintf(strTemp, TEXT("%s\\DefaultIcon"), strAppKey);
	TCHAR strIconValue[MAX_PATH];
	wsprintf(strIconValue, TEXT("%s,-%d"), strDefaultIcon, iResourceId);//xxxx.ico,-101
	ret = WriteRegKey(HKEY_CLASSES_ROOT, strTemp, TEXT(""), strIconValue);
	if (ret != ERROR_SUCCESS) return ret;

	//Shell\Open
	wsprintf(strTemp, TEXT("%s\\Shell"), strAppKey);
	ret = WriteRegKey(HKEY_CLASSES_ROOT, strTemp, TEXT(""), TEXT("Open"));
	if (ret != ERROR_SUCCESS) return ret;

	//Shell\Open\Command xxx.exe
	TCHAR strTemp2[MAX_PATH];
	wsprintf(strTemp, TEXT("%s\\Shell\\Open\\Command"), strAppKey);
	wsprintf(strTemp2, TEXT("\"%s\" \"%%1\""), strAppName);
	ret = WriteRegKey(HKEY_CLASSES_ROOT, strTemp, TEXT(""), strTemp2);
	if (ret != ERROR_SUCCESS) return ret;

	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);

	return ERROR_SUCCESS;
}

//-------------------------------------------------------------------------  
// 取消文件图标关联  
//-------------------------------------------------------------------------  
// @strExt              [in]: 需要检测的文件后缀(.txt)  
// @strAppKey           [in]: 扩展名在注册表中的键值(txtfile)  
//-------------------------------------------------------------------------  
// @return              : 0表示成功，其他表示错误码  
//-------------------------------------------------------------------------  
int UnRegisterFileAssociation(TCHAR *strExt, TCHAR *strAppKey)
{
	long ret;

	ret = DeleteRegTree(HKEY_CLASSES_ROOT, strExt);
	if (ret != ERROR_SUCCESS) return ret;

	ret = DeleteRegTree(HKEY_CLASSES_ROOT, strAppKey);
	if (ret != ERROR_SUCCESS) return ret;

	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
	return ERROR_SUCCESS;
}

//-------------------------------------------------------------------------  
// 写注册表的的方法  
//-------------------------------------------------------------------------  
// @root            [in]: 注册表根  
// @subDir          [in]: 子结构  
// @regKey          [in]: 注册表项键  
// @regValue        [in]: 要写入的注册表值  
//-------------------------------------------------------------------------  
// @return              : 0表示成功，其他表示错误码  
//-------------------------------------------------------------------------  
int WriteRegKey(HKEY root, TCHAR * subDir, TCHAR * regKey, TCHAR * regValue)
{
	TCHAR strTemp[_MAX_PATH];
	HKEY hKey;
	_tcscpy(strTemp, subDir);

	TCHAR* sclass = TEXT("");
	DWORD nbf = 0;
	long ret = RegCreateKeyEx(root, strTemp, 0, TEXT(""), REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hKey, &nbf);
	if (ret != ERROR_SUCCESS) return ret;

#ifdef UNICODE
	ret = RegSetValueEx(hKey, regKey, 0, REG_SZ, (LPBYTE)regValue, _tcslen(regValue) * 2 + 1);
#else
	ret = RegSetValueEx(hKey, regKey, 0, REG_SZ, (LPBYTE)regValue, _tcslen(regValue) + 1);
#endif

	if (ret != ERROR_SUCCESS) return ret;
	RegCloseKey(hKey);

	return 0;
}

//-------------------------------------------------------------------------  
// 写注册表的的方法  
//-------------------------------------------------------------------------  
// @root            [in] : 注册表根  
// @subDir          [in] : 子结构  
// @regKey          [in] : 注册表项键  
// @regValue        [out]: 读的的注册表值  
//-------------------------------------------------------------------------  
// @return               : 0表示成功，其他表示错误码  
//-------------------------------------------------------------------------  
int ReadRegKey(HKEY root, TCHAR * subDir, TCHAR * regKey, TCHAR* regValue)
{
	TCHAR strTemp[_MAX_PATH];
	HKEY hKey;
	wsprintf(strTemp, subDir);

	long ret = (::RegOpenKeyEx(root, strTemp, 0, KEY_READ, &hKey));
	if (ret != ERROR_SUCCESS) return ret;

	DWORD valueType = REG_SZ;
	DWORD cbData = 255;

	ret = ::RegQueryValueEx(hKey, regKey, NULL, &valueType, (LPBYTE)regValue, &cbData);
	if (ret != ERROR_SUCCESS) return ret;
	RegCloseKey(hKey);

	return 0;
}
//-------------------------------------------------------------------------  
// 删注册表的的方法  
//-------------------------------------------------------------------------  
// @root            [in] : 注册表根  
// @subDir          [in] : 子结构  
//-------------------------------------------------------------------------  
// @return               : 0表示成功，其他表示错误码  
//-------------------------------------------------------------------------  
int DeleteRegTree(HKEY root, TCHAR * subDir)
{
	return ::SHDeleteKey(root, subDir);
}