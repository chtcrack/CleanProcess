// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// Monitor.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"
void MyDebug(const TCHAR * sz, ...)
{
	TCHAR szData[1024] = { 0 };

	va_list args;

	va_start(args, sz);

	_vsntprintf_s(szData, sizeof(szData) - 1, sz, args);

	va_end(args);

	OutputDebugString(szData);

	return;
}


