// stdafx.cpp : 只包括标准包含文件的源文件
// Monitor.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

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


