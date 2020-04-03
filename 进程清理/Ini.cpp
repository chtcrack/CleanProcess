#include "StdAfx.h"
#include ".\Ini.h"

#define MAX_ALLSECTIONS 2048  //ȫ���Ķ���
#define MAX_SECTION 260  //һ����������
#define MAX_ALLKEYS 50000  //ȫ���ļ���,���Կ���ini�ļ���С
#define MAX_KEY 260  //һ����������

EXTERN_C IMAGE_DOS_HEADER __ImageBase;


BOOL CIni::SetFileName(LPCTSTR lpFileName)
{
 CFile file;
 CFileStatus status;


 CString TmpStr = lpFileName;

 if (TmpStr.Find("\\",0)==-1)
 {
	 TmpStr = L_Strpath(TmpStr);
 }


 if(!file.GetStatus(TmpStr,status))
  return TRUE;
 
 m_strFileName=TmpStr;
 return FALSE;
}


CString CIni::L_Strpath(char* filename)
{
	TCHAR buf[_MAX_PATH]; 
	::GetModuleFileName((HINSTANCE)&__ImageBase, buf, _MAX_PATH);
	CString path;
	path.Format("%s",buf);
	int pos = path.ReverseFind('\\');
	path = path.Left(pos);
	path.Format("%s\\%s",path,filename);
	return path;
}


CString CIni::L_Strpath(CString filename)
{
	TCHAR buf[_MAX_PATH]; 
	::GetModuleFileName((HINSTANCE)&__ImageBase, buf, _MAX_PATH);
	CString path;
	path.Format("%s",buf);
	int pos = path.ReverseFind('\\');
	path = path.Left(pos);
	path.Format("%s\\%s",path,filename);
	return path;
}

 
CString CIni::GetFileName(void)
{
 return m_strFileName;
}
 
BOOL CIni::SetValue(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpValue,bool bCreate)
{
 TCHAR lpTemp[MAX_PATH] ={0};
 
 //����if����ʾ�������bCreateΪfalseʱ����û���������ʱ�򷵻�TRUE����ʾ����
 //!*&*none-value*&!* ���Ǹ������ַ�û���ر����壬������д�Ƿ�ֹ������ͬ��
 if (!bCreate)
 {
  GetPrivateProfileString(lpSection,lpKey,"!*&*none-value*&!*",lpTemp,MAX_PATH,m_strFileName);
  if(strcmp(lpTemp,"!*&*none-value*&!*")==0)
   return TRUE;
 }
 
 if(WritePrivateProfileString(lpSection,lpKey,lpValue,m_strFileName))
  return FALSE;
 else
  return GetLastError();
}


BOOL CIni::SetValue(LPCTSTR lpSection, LPCTSTR lpKey, DWORD Value,bool bCreate)
{
	CString str;
	str.Format("%d",Value);
	return SetValue( lpSection, lpKey,str,bCreate);
}


 
CString CIni::GetValue(LPCTSTR lpSection, LPCTSTR lpKey)
{
 DWORD dValue;
 TCHAR lpValue[MAX_PATH] ={0};
 
 dValue=GetPrivateProfileString(lpSection,lpKey,"",lpValue,MAX_PATH,m_strFileName);
 return lpValue;
}
 
BOOL CIni::DelSection(LPCTSTR lpSection)
{
 if(WritePrivateProfileString(lpSection,NULL,NULL,m_strFileName))
  return FALSE;
 else
  return GetLastError();
}
 
BOOL CIni::DelKey(LPCTSTR lpSection, LPCTSTR lpKey)
{
 if(WritePrivateProfileString(lpSection,lpKey,NULL,m_strFileName))
  return FALSE;
 else
  return GetLastError();
}
 
int CIni::GetSections(CStringArray& arrSection)
{
 /*
 ������������
 GetPrivateProfileSectionNames - �� ini �ļ��л�� Section ������
 ��� ini �������� Section: [sec1] �� [sec2]���򷵻ص��� 'sec1',0,'sec2',0,0 �����㲻֪��  
 ini ������Щ section ��ʱ���������� api ����ȡ���� 
 */
 int i;  
 int iPos=0;  
 int iMaxCount;
 TCHAR chSectionNames[MAX_ALLSECTIONS]={0}; //�ܵ���������ַ���
 TCHAR chSection[MAX_SECTION]={0}; //���һ��������
 GetPrivateProfileSectionNames(chSectionNames,MAX_ALLSECTIONS,m_strFileName);
 
 //����ѭ�����ضϵ�����������0
 for(i=0;i<MAX_ALLSECTIONS;i++)
 {
  if (chSectionNames[i]==0)
   if (chSectionNames[i]==chSectionNames[i+1])
    break;
 }
 
 iMaxCount=i+1; //Ҫ��һ��0��Ԫ�ء����ҳ�ȫ���ַ����Ľ������֡�
 arrSection.RemoveAll();//���ԭ����
 
 for(i=0;i<iMaxCount;i++)
 {
  chSection[iPos++]=chSectionNames[i];
  if(chSectionNames[i]==0)
  {   
   arrSection.Add(chSection);
   memset(chSection,0,MAX_SECTION);
   iPos=0;
  }
 
 }
 
 return (int)arrSection.GetSize();
}
 
int CIni::GetKeyValues(CStringArray& arrKey,CStringArray& arrValue, LPCTSTR lpSection)
{
 /*
 ������������
 GetPrivateProfileSection- �� ini �ļ��л��һ��Section��ȫ��������ֵ��
 ���ini����һ���Σ������� "��1=ֵ1" "��2=ֵ2"���򷵻ص��� '��1=ֵ1',0,'��2=ֵ2',0,0 �����㲻֪��  
 ���һ�����е����м���ֵ����������� 
 */
 int i;  
 int iPos=0;
 CString strKeyValue;
 int iMaxCount;
 TCHAR chKeyNames[MAX_ALLKEYS]={0}; //�ܵ���������ַ���
 TCHAR chKey[MAX_KEY]={0}; //�������һ������
 
 GetPrivateProfileSection(lpSection,chKeyNames,MAX_ALLKEYS,m_strFileName);
 
 for(i=0;i<MAX_ALLKEYS;i++)
 {
  if (chKeyNames[i]==0)
   if (chKeyNames[i]==chKeyNames[i+1])
    break;
 }
 
 iMaxCount=i+1; //Ҫ��һ��0��Ԫ�ء����ҳ�ȫ���ַ����Ľ������֡�
 arrKey.RemoveAll();//���ԭ����
 arrValue.RemoveAll();
 
 for(i=0;i<iMaxCount;i++)
 {
  chKey[iPos++]=chKeyNames[i];
  if(chKeyNames[i]==0)
  {
   strKeyValue=chKey;
   arrKey.Add(strKeyValue.Left(strKeyValue.Find("=")));
   arrValue.Add(strKeyValue.Mid(strKeyValue.Find("=")+1));
   memset(chKey,0,MAX_KEY);
   iPos=0;
  }
 
 }
 
 return (int)arrKey.GetSize();
}
 
BOOL CIni::DelAllSections()
{
 int nSection;
 CStringArray arrSection;
 nSection=GetSections(arrSection);
 for(int i=0;i<nSection;i++)
 {
  if(DelSection(arrSection[i]))
   return GetLastError();
 }
 return FALSE;
}


/*
CString str;
char szAppPath[MAX_PATH];
GetModuleFileName(NULL, szAppPath, MAX_PATH);
str.Format("%s",szAppPath);
AfxMessageBox(str);

(strrchr(szAppPath, '\\'))[0] = 0;

str.Format("%s",szAppPath);
AfxMessageBox(str);
*/