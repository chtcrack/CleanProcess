// stdafx.cpp : 只包括标准包含文件的源文件
// 进程清理.pch 将是预编译头
// stdafx.obj 将包含预编译类型信息

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
void CString2Char(CString str, char ch[])//CString转char[]
{
	int strLength1 = str.GetLength();
	//char chArray[100];
	memset(ch,0, sizeof(ch)); //将数组的垃圾内容清空.
	strncpy(ch, str, strLength1);
}
