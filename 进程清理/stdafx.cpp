// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// ��������.pch ����Ԥ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"


void MyDebug(const TCHAR * sz, ...)
{
	TCHAR szData[1024]={0};

	va_list args;

	va_start(args, sz);

	_vsntprintf_s(szData, sizeof(szData) - 1, sz, args);

	va_end(args);

	OutputDebugString(szData);

	return;
}
void CString2Char(CString str, char ch[])//CStringתchar[]
{
	int strLength1 = str.GetLength();
	//char chArray[100];
	memset(ch,0, sizeof(ch)); //������������������.
	strncpy(ch, str, strLength1);
}
