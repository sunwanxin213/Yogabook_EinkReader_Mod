/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */



#pragma once

#ifdef UNICODE

#define GetTextFileCodeType	GetTextFileCodeTypeW

#else
#define GetTextFileCodeType	GetTextFileCodeTypeA

#endif

#include "cmmBaseDef.h"
#include <windows.h>
#include <string>	// for string
using std::string;
using std::wstring;

namespace cmmStrHandle{

//////////////////////////////////////////////////////////////////////////
// �ַ�������ת��

// wide characters to multi bytes characters by specified code page
bool RealWideCharToMultibyte(IN const UINT nuiCodePage, IN const wstring& roWStrToConvert, OUT string& roStrConverted);

// multi bytes characters to wide characters by specified code page
bool RealMultibyteToWideChar(IN const UINT nuiCodePage, IN const string& roStrToConvert, OUT wstring& roWStrConverted);

// unicode to ansi
bool UnicodeToAnsi(IN const wstring& roWStrToConvert, OUT string& roStrConverted);

// unicode to utf-8
bool UnicodeToUtf8(IN const wstring& roWStrToConvert, OUT string& roStrConverted);

// ansi to unicode
bool AnsiToUnicode(IN const string& roStrToConvert, OUT wstring& roWStrConverted);

// ansi to utf-8
bool AnsiToUtf8(IN const string& roStrToConvert, OUT string& roStrConverted);

// utf-8 to unicode
bool Utf8ToUnicode(IN const string& roStrToConvert, OUT wstring& roWStrConverted);

// utf-8 to ansi
bool Utf8ToAnsi(IN const string& roStrToConvert, OUT string& roStrConverted);


//////////////////////////////////////////////////////////////////////////
// ���븨��

// get file code type
enum FileCodeType
{
	FCT_ERROR = 0,
	FCT_ANSI,
	FCT_UNICODE,
	FCT_UTF8,
	FCT_UNICODE_BIG_ENDIAN
};
FileCodeType GetTextFileCodeTypeA(IN const string& roStrFileName);
FileCodeType GetTextFileCodeTypeW(IN const wstring& roWStrFileName);


//////////////////////////////////////////////////////////////////////////
// �ַ�תת��

// number converts to characters
bool Int64ToStr(IN const LONGLONG nllIntegerToConvert, OUT string& roStrConverted);

bool Int64ToWStr(IN const LONGLONG nllIntegerToConvert, OUT wstring& roWStrConverted);

bool UInt64ToStr(IN const ULONGLONG nullIntegerToConvert, OUT string& roStrConverted);

bool UInt64ToWStr(IN const ULONGLONG nullIntegerToConvert, OUT wstring& roWStrConverted);



//////////////////////////////////////////////////////////////////////////
// �ļ�·������

// ������
//		��ȡ�������ļ�·�����ʱ���յ�Ψһ·���������Ѿ����ڳ�ͻ����ͨ�����ļ��������ۼ�����)
bool GetUniqueFilePath(IN const wchar_t* nswFilePath, OUT wchar_t* nswUniquePath, IN int niCchBuffer);
}