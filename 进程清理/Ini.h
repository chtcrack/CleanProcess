#pragma once

class CIni
{
private:
	CString m_strFileName;
public:
	CIni(CString strFileName):m_strFileName(strFileName)
	{
		CString TmpStr = strFileName;

		if (TmpStr.Find("//",0)==-1)
		{
			TmpStr = L_Strpath(TmpStr);
		}
		m_strFileName = TmpStr;
	}
public:
	//一般性操作：
	BOOL SetFileName(LPCTSTR lpFileName);  //设置文件名
	CString GetFileName(void); //获得文件名
	BOOL SetValue(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpValue,bool bCreate=true); //设置键值，bCreate是指段名及键名未存在时，是否创建。
	BOOL SetValue(LPCTSTR lpSection, LPCTSTR lpKey, DWORD Value,bool bCreate=true);
	CString GetValue(LPCTSTR lpSection, LPCTSTR lpKey); //得到键值.
	BOOL DelSection(LPCTSTR strSection);  //删除段名
	BOOL DelKey(LPCTSTR lpSection, LPCTSTR lpKey);  //删除键名

public:
	//高级操作：
	int GetSections(CStringArray& arrSection);  //枚举出全部的段名
	int GetKeyValues(CStringArray& arrKey,CStringArray& arrValue,LPCTSTR lpSection);  //枚举出一段内的全部键名及值
	BOOL DelAllSections();
	CString L_Strpath(CString filename);//得到当前程序目录路径
	CString L_Strpath(char* filename);//得到当前程序路径
};



/*
使用方法：
CIni ini("c://a.ini");
int n;

/*获得值
TRACE("%s",ini.GetValue("段1","键1"));
 */

/*添加值
ini.SetValue("自定义段","键1","值");
ini.SetValue("自定义段2","键1","值",false);
 */

/*枚举全部段名
CStringArray arrSection;
n=ini.GetSections(arrSection);
for(int i=0;i<n;i++)
TRACE("%s/n",arrSection[i]);
 */

/*枚举全部键名及值
CStringArray arrKey,arrValue;
n=ini.GetKeyValues(arrKey,arrValue,"段1");
for(int i=0;i<n;i++)
TRACE("键：%s/n值：%s/n",arrKey[i],arrValue[i]);
*/

/*删除键值
ini.DelKey("段1","键1");
*/

/*删除段
ini.DelSection("段1");
*/

/*删除全部
ini.DelAllSections();
*/