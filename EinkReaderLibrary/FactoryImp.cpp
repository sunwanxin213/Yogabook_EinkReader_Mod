/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


// ECtl.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CfgIface.h"
#include "cmmstruct.h"
#include "cmmBaseObj.h"
#include "cmmPath.h"
#include "XCtl.h"
#include "ElementImp.h"
#include "FactoryImp.h"
#include "SmCuveImp.h"

#include "ReaderBaseFrame.h"
#include "MenuBase.h"


CFactoryImp* CFactoryImp::gpFactoryInstance=NULL;
DEFINE_BUILTIN_NAME(CFactoryImp)

CFactoryImp::CFactoryImp()
{
	mpConfig = NULL;
}
CFactoryImp::~CFactoryImp()
{
	CMM_SAFE_RELEASE(mpConfig);
}

// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
ULONG CFactoryImp::InitOnCreate(void)
{
	if(gpFactoryInstance != NULL)
		return ERESULT_OBJECT_EXISTED;

	gpFactoryInstance = this;

	return ERESULT_SUCCESS;
}

// �������ļ��д�������
IEinkuiIterator* __stdcall CFactoryImp::CreateElement(
	IN IEinkuiIterator* npParent,		// ������ָ��
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	)
{
	IXsElement* lpoElement = NULL;
	IEinkuiIterator* lpILwIterator = NULL;
	wchar_t lswClsName[MAX_PATH] ={0};

	do 
	{
		BREAK_ON_NULL(npTemplete);

		// ��ȡҪ�����Ķ�������
		if(npTemplete->GetValue(lswClsName, MAX_PATH*sizeof(wchar_t)) <= 0)
			break;

		if (0 == _wcsicmp(lswClsName, L"CVfSlMainFrame"))
			lpoElement = CReaderBaseFrame::CreateInstance(npParent, npTemplete,nuEID);
		else if(0 == _wcsicmp(lswClsName, L"Cuve"))
			lpoElement = CSmCuveImp::CreateInstance(npParent, npTemplete,nuEID);
		else if (0 == _wcsicmp(lswClsName, L"MenuBase"))
			lpoElement = CMenuBase::CreateInstance(npParent, npTemplete, nuEID);

	} while (false);

	if(lpoElement != NULL)
		lpILwIterator = lpoElement->GetIterator();	//��ȡIteratorָ��

	return lpILwIterator;
}

// ͨ����������������
IEinkuiIterator* __stdcall CFactoryImp::CreateElement(
	IN IEinkuiIterator* npParent,		// ������ָ��
	IN const wchar_t*		nswClassName,	// ����
	IN ULONG nuEID					// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	)
{
	IXsElement* lpoElement = NULL;
	IEinkuiIterator* lpILwIterator = NULL;

	if(lpoElement != NULL)
		lpILwIterator = lpoElement->GetIterator();	//��ȡIteratorָ��

	return lpILwIterator;
}

// ������Module���׵�Profile�ļ��ӿڣ����صĽӿڵ�����ʹ��ʱ����ҪRelease
IConfigFile* __stdcall CFactoryImp::GetTempleteFile(void)
{
	const wchar_t* lpszWidgetPath = NULL;
	const wchar_t* lpszLanguage = NULL;
	wchar_t lpszConfigFileName[CONFIG_FILE_NAME_MAX_LEN] = {0};

	IConfigFile* lpIConfigFile = NULL;
	do 
	{
		if(mpConfig == NULL)
		{
			lpszWidgetPath = EinkuiGetSystem()->GetCurrentWidget()->GetWidgetDefaultPath();		//��ȡWidget�İ�װ·��
			BREAK_ON_NULL(lpszWidgetPath);

			CFilePathName loConfigFilePath(lpszWidgetPath);
			loConfigFilePath.AssurePath();	//����ΪĿ¼��Ҳ�������������"\"

			BREAK_ON_FALSE(loConfigFilePath.Transform(L"Profile\\"));

			lpszLanguage = EinkuiGetSystem()->GetCurrentLanguage();		//��ȡ��ǰϵͳ���Զ�Ӧ���ַ���,���磺���ļ����Ӧ��chn
			BREAK_ON_NULL(lpszLanguage);

			wcscpy_s(lpszConfigFileName,CONFIG_FILE_NAME_MAX_LEN,L"Widget");		//ƴ���ļ��� ʾ����System_chn.set
			wcscat_s(lpszConfigFileName,CONFIG_FILE_NAME_MAX_LEN,L"_");
			wcscat_s(lpszConfigFileName,CONFIG_FILE_NAME_MAX_LEN,lpszLanguage);
			//wcscat_s(lpszConfigFileName, CONFIG_FILE_NAME_MAX_LEN, L"bg");
			wcscat_s(lpszConfigFileName,CONFIG_FILE_NAME_MAX_LEN,L".set");

			BREAK_ON_FALSE(loConfigFilePath.Transform(lpszConfigFileName));	//ƴ��ȫ·��

			lpIConfigFile = EinkuiGetSystem()->OpenConfigFile(loConfigFilePath.GetPathName(),OPEN_EXISTING);	//�򿪸������ļ�

			if (lpIConfigFile == NULL)
			{
				//����򲻿���ʹ��Ӣ�������ļ�
				wcscpy_s(lpszConfigFileName, CONFIG_FILE_NAME_MAX_LEN, L"Widget");		//ƴ���ļ��� ʾ����System_chn.set
				wcscat_s(lpszConfigFileName, CONFIG_FILE_NAME_MAX_LEN, L"_enu.set");

				BREAK_ON_FALSE(loConfigFilePath.Transform(lpszConfigFileName));	//ƴ��ȫ·��

				lpIConfigFile = EinkuiGetSystem()->OpenConfigFile(loConfigFilePath.GetPathName(), OPEN_EXISTING);	//�򿪸������ļ�

			}

			BREAK_ON_NULL(lpIConfigFile);

			mpConfig = lpIConfigFile;
		}
		else
			lpIConfigFile = mpConfig;

		lpIConfigFile->AddRefer();	//�������ü���

	} while (false);

	return lpIConfigFile;
}

// ��ñ�DLLΨһ�Ĺ�������
CFactoryImp* CFactoryImp::GetUniqueObject(void)
{
	if(gpFactoryInstance ==NULL)
		CFactoryImp::CreateInstance();

	CMMASSERT(gpFactoryInstance !=NULL);

	return gpFactoryInstance;
}

