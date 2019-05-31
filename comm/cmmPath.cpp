/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


//////////////////////////////////////////////////////////////////////////
// �����ļ���Ŀ¼ת�����ߺ���
#include "stdafx.h"
#include "Windows.h"
#include "cmmPath.h"
#include "cmmstruct.h"
#include "Shlobj.h"

CFilePathName::CFilePathName()
{
	msiLength = 0;
	mszPathName[0]=UNICODE_NULL;
}

CFilePathName::CFilePathName(const class CFilePathName& src)
{
	*this = src;
}

CFilePathName::CFilePathName(const wchar_t* nszPathName)
{
	*this = nszPathName;
}


// ��̬��������
CFilePathName* CFilePathName::Create(const wchar_t* nszPathName)
{
	CFilePathName* lpObj = new CFilePathName(nszPathName);
	return lpObj;
}

// ��̬��������
CFilePathName* CFilePathName::Duplicate()
{
	CFilePathName* lpObj = new CFilePathName(*this);
	return lpObj;
}

// ��ǰĿ¼
bool CFilePathName::SetByCurrentPath(void)
{
	int liLength;

	GetCurrentDirectory(MAX_PATH,mszPathName); 
	liLength = (int)wcslen(mszPathName);
	if(liLength+1 >= MAX_PATH || liLength <= 0)
	{
		mszPathName[0] = UNICODE_NULL;
		msiLength = 0;
		return false;
	}

	msiLength = (short)liLength;
	mszPathName[msiLength++] = L'\\';
	mszPathName[msiLength] = UNICODE_NULL;

	return true;
}

// ģ������Ŀ¼���ļ���
bool CFilePathName::SetByModulePathName(void)
{
	int liLength;

	liLength = GetModuleFileName(GetModuleHandle(NULL),mszPathName,MAX_PATH);
	if(liLength >= MAX_PATH || liLength <= 0)
	{
		mszPathName[0] = UNICODE_NULL;
		msiLength = 0;
		return false;
	}

	msiLength = (short)liLength;

	return true;
}

// ��õ�ǰ�û���AppData\Roaming Ŀ¼ ���磺C:\Users\UserName\AppData\Roaming
// ���øú�����·��������"\"
bool CFilePathName::SetByUserAppData(void)
{
	bool lbRet = false;

	do 
	{
		//��ȡĿ¼
		if(SHGetSpecialFolderPath(NULL,mszPathName,CSIDL_LOCAL_APPDATA,FALSE) == FALSE)
			break;

		int liLength = (int)wcslen(mszPathName);
		if(liLength+1 >= MAX_PATH || liLength <= 0)
		{
			mszPathName[0] = UNICODE_NULL;
			msiLength = 0;
			break;
		}

		msiLength = (short)liLength;
		mszPathName[msiLength++] = L'\\';
		mszPathName[msiLength] = UNICODE_NULL;

		lbRet = true;

	} while (false);
	
	return lbRet;
}

// ����ΪĿ¼�������βû��'\\'�ͼ���
void CFilePathName::AssurePath(void)
{
	if(msiLength>0 && mszPathName[msiLength-1]!=L'\\' && msiLength < MAX_PATH)
	{
		mszPathName[msiLength++] = L'\\';
		mszPathName[msiLength] = 0;
	}
}

// ����Ŀ¼�����ļ���������falseʧ�ܣ����ܵ�ԭ���޷�ת��λ�ã����߽������
//	ʵ���÷���ȡͬ��Ŀ¼����L".\\"��ȡ�ϲ�Ŀ¼����"..\\'
bool CFilePathName::Transform(const wchar_t* nszTransTo,bool nbForceBePath)
{
	cmmStack<wchar_t*> loSeparators;

	// ����ɨ��һ�鵱ǰ����
	USHORT i;

	for (i=0;i<msiLength;i++)
	{
		if(mszPathName[i]==UNICODE_NULL)
			break;
		if(mszPathName[i] == L'\\')
		{
			loSeparators.Push(&mszPathName[i]);
		}
	}

	// ����ת������
	for(i=0;i<MAX_PATH;i++)
	{
		if(nszTransTo[i]==L'.')
		{
			i++;
			if(nszTransTo[i]==L'.' && nszTransTo[i+1]==L'\\')
			{
				i++;

				loSeparators.Pop();
			}
		}

		if(nszTransTo[i]!=L'\\')
			break;
	}

	if(loSeparators.Size() <= 0)
		return false;// ����Ĳ��

	// ����Ŀ¼���������
	short j = (short)(loSeparators.Top() - mszPathName)+1;
	for(;i<MAX_PATH && j<MAX_PATH;i++,j++)
	{
		if((mszPathName[j] = nszTransTo[i])==UNICODE_NULL)
			break;
	}
	if(nbForceBePath != false)
	{
		if(j>= MAX_PATH)
			return false;
		if(j >= 0 && mszPathName[j-1]!=L'\\')
		{
			mszPathName[j++] = L'\\';
			mszPathName[j] = UNICODE_NULL;
		}
	}

	msiLength = j;

	return true;
}
// ����Ŀ¼�����ļ���������falseʧ�ܣ����ܵ�ԭ���޷�ת��λ�ã����߽������
//	ʵ���÷���ȡͬ��Ŀ¼����L".\\"��ȡ�ϲ�Ŀ¼����"..\\'
bool CFilePathName::Transform(const CFilePathName& roPath)
{
	return Transform(roPath.mszPathName);
}

void CFilePathName::UnlockBuffer()
{
	for (msiLength =0;msiLength < MAX_PATH;msiLength++)
	{
		if(mszPathName[msiLength] == UNICODE_NULL)
			break;
	}
}

// ���
void CFilePathName::Clean(void)
{
	mszPathName[0] = UNICODE_NULL;
	msiLength = 0;
}

// ȡ�ļ����������ǰ�������һ��Ŀ¼���򷵻�L"\0"
const wchar_t* CFilePathName::GetFileName(
	int* npLength	// �������ֵ��ַ�����������'\0'��β
	)const
{
	int i;
	for(i=msiLength-1;i>= 0;i--)
	{
		if(mszPathName[i] == L'\\')
			break;
	}
	if(i<0)
		i=0;	// ������Ŀ¼
	else
		i++;

	if(npLength != NULL)
		*npLength = (msiLength-i);

	return mszPathName+i;
}

// ȡ��չ���������ǰ����Ĳ���һ���ļ������߲�������չ�����򷵻�L"\0"
const wchar_t* CFilePathName::GetExtName(void)const
{
	int i;
	for(i=msiLength-1;i>= 0;i--)
	{
		if(mszPathName[i] == L'\\')
		{
			i = -1;
			break;
		}
		if(mszPathName[i]==L'.')
			break;
	}
	if(i<0)
		i = msiLength;	// �պ�ָ��L'\0'
	else
		i++;

	return mszPathName+i;
}

// ����
void CFilePathName::operator=(const class CFilePathName& src)
{
	msiLength = src.msiLength;
	if(msiLength > 0)
		RtlCopyMemory(mszPathName,src.mszPathName,(msiLength+1)*sizeof(wchar_t));
}

// ���ַ�����ֵ
void CFilePathName::operator=(const wchar_t* nswSrc)
{
	for (msiLength =0;msiLength < MAX_PATH;msiLength++)
	{
		if((mszPathName[msiLength] = nswSrc[msiLength])==UNICODE_NULL)
			break;
	}
}


// ֻ����ȫ��Ŀ¼������ᱣ��'\\'
bool CFilePathName::CopyPath(const class CFilePathName& src)
{
	*this = src;
	return Transform(L".\\");
}
// �ж��ļ���չ���Ƿ����ָ�������ͣ�ʹ��\0�ָ�������������\0��ʾ���ͽ���������ֵ<0������ϣ����򷵻ص�������ϵ����ͱ�ţ���0��ʼ
int CFilePathName::ExtNameMatch(const wchar_t* nszFilter)const
{
	int liIndex;
	const wchar_t* lpExtName = GetExtName();

	for(liIndex=0;*nszFilter != UNICODE_NULL;nszFilter++,liIndex++)
	{
		if(_wcsicmp(lpExtName,nszFilter)==0)
			return liIndex;

		while(*nszFilter != UNICODE_NULL)
			nszFilter++;
	}

	return -1;
}

// ֱ�Ӹ����ַ���
void CFilePathName::operator+=(const class CFilePathName& src)throw(...)
{
	if(msiLength+src.msiLength >= MAX_PATH)
		throw mszPathName;
	RtlCopyMemory(mszPathName+msiLength,src.mszPathName,(src.msiLength+1)*sizeof(wchar_t));
	msiLength += src.msiLength;
}

void CFilePathName::operator+=(const wchar_t* nszAppend)throw(...)
{
	for(;msiLength < MAX_PATH;msiLength++)
	{
		if((mszPathName[msiLength] = *nszAppend++)==UNICODE_NULL)
			break;
	}
	if(msiLength >= MAX_PATH)
		throw mszPathName;
}

bool CFilePathName::operator==(const class CFilePathName& right)
{
	if(msiLength != right.msiLength)
		return false;

	for (int i=0;i<msiLength;i++)
	{
		if(mszPathName[i] != right.mszPathName[i] && ((mszPathName[i]|0x20) != (right.mszPathName[i]|0x20) || (mszPathName[i]|0x20) < L'a' || (mszPathName[i]|0x20) > L'z'))
		{
			return false;
			break;
		}
	}

	return true;
}

bool CFilePathName::operator!=(const class CFilePathName& right)
{
	return !(*this==right);
}
