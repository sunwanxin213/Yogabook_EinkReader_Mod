/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "stdafx.h"
#include "CommonHeader.h"

#include "XsWidgetImp.h"



DEFINE_BUILTIN_NAME(CXsWidget)

CXsWidget::CXsWidget()
{
	mpFactory = NULL;
	mswModulePath = NULL;
	mswInstance = NULL;
	mswModuleName = NULL;
	mpHomePage = NULL;
	mpInstanceConfig = NULL;
	mbValid = true;

	mpInstanceConfig = NULL;
}

CXsWidget::~CXsWidget()
{
	if (mpHomePage != NULL)
		mpHomePage->Close();
	CMM_SAFE_DELETE(mswModulePath);
	//CMM_SAFE_RELEASE(mpFactory); ����GUIϵͳ�Ĺ�����ɾ��
	CMM_SAFE_RELEASE(mpInstanceConfig);
}


// ��Widget��ģ���ļ�������ʵ�ִ�Widget��DLL���ƣ���"IdeaMain.dll"
const wchar_t* __stdcall CXsWidget::GetModuleName(void)
{
	return mswModuleName;
}

// ��ñ�΢��ʵ������ϵͳ����ʱ�����ĵ�һ��΢��������system��
const wchar_t* __stdcall CXsWidget::GetInstanceName(void)
{
	return mswInstance;
}

// ��ȡ΢�����ڰ�װĿ¼�������к�׺��'\'
const wchar_t* __stdcall CXsWidget::GetWidgetDefaultPath(void)
{
	return mswModulePath;
}

// ���΢������Module�Ĺ����ӿ�
IElementFactory* __stdcall CXsWidget::GetDefaultFactory(void)
{
	return mpFactory;
}

void CXsWidget::SetFactory(IElementFactory* npFactory)
{
	mpFactory = npFactory;
	mpFactory->AddRefer();
}

void CXsWidget::SetHomePage(IEinkuiIterator* npHomePage)
{
	mpHomePage = npHomePage;
	mpHomePage->AddRefer();
}


ERESULT CXsWidget::InitOnCreate(
	IN const wchar_t* nswModulePathName,	// ��Widget��ģ���ļ���·��������ʵ�ִ�Widget��DLL����
	IN const wchar_t* nswInstanceName,	// �������е�ʵ������ʵ����������ͬ�����������ͬ��ʵ������ϵͳ���᷵��ʧ��
	IN ICfKey* npInstanceConfig	// ������ʵ����ר������
	)
{
	int liPathLen = (int)wcslen(nswModulePathName)+1;
	int liInsLen = (int)wcslen(nswInstanceName)+1;
	int liTotal = liPathLen+liInsLen+1;

	mswModulePath = new wchar_t[liTotal];
	if(mswModulePath == NULL)
		return ERESULT_INSUFFICIENT_RESOURCES;

	RtlCopyMemory(mswModulePath,nswModulePathName,liPathLen*sizeof(wchar_t));
	mswModuleName = wcsrchr(mswModulePath,L'\\');
	if(mswModuleName == NULL)
		return ERESULT_UNSUCCESSFUL;

	*mswModuleName++ =UNICODE_NULL;

	mswInstance = mswModulePath + liPathLen;
	RtlCopyMemory(mswInstance,nswInstanceName,liInsLen*sizeof(wchar_t));

	mpInstanceConfig = npInstanceConfig;
	if(mpInstanceConfig != NULL)
		mpInstanceConfig->AddRefer();

	return ERESULT_SUCCESS;
}


// ���΢����ʵ��ר�����ã�һ��΢��Module������һ��������ͬʱ���ж��ʵ�������ͬʱ��һ̨�����ϣ�ÿһ��Windows�û��ʻ��£�����������һ��Idealife���̣�
//		������ҪΪ΢����ÿһ������̬ʵ���ṩһ��ר�����ã������Խ���Ҫ���ڱ������������Ϣ��ŵ����ר��������
ICfKey* __stdcall CXsWidget::GetInstanceConfig(void)
{
	return mpInstanceConfig;
}

//��ȡ΢�����������ʱ�ļ���Ŀ¼,ͬһ����΢���Ĳ�ͬʵ���õ����ǲ�ͬ���ļ���·��
bool __stdcall CXsWidget::GetInstanceTempPath(OUT wchar_t* npswPath,IN LONG nlBufferLen)
{
	bool lbRet = false;
	wchar_t lswTempPath[MAX_PATH] = {0};

	do 
	{
		BREAK_ON_NULL(npswPath);
		if(nlBufferLen <= 0)
			break;

		CFilePathName loDest;
		loDest.SetByUserAppData();
		loDest.AssurePath();
		loDest.Transform(L".\\MagWriter\\Temp\\");

		wcscpy_s(lswTempPath,MAX_PATH,loDest.GetPathName());
		wcscat_s(lswTempPath,MAX_PATH,mswInstance);

		wcscpy_s(npswPath,nlBufferLen,lswTempPath);

		//�������Ŀ¼
		SHCreateDirectory(NULL,lswTempPath);

		lbRet = true;

	} while (false);

	return lbRet;
}
// ���΢��ʵ����Home Page
IEinkuiIterator* __stdcall CXsWidget::GetHomePage(void)
{
	return mpHomePage;
}

// �ر�Widget
void __stdcall CXsWidget::Close(void)
{
	CMMASSERT(mpHomePage);

	// ��������
	mpHomePage->SetVisible(false);

	// ��HomePage�Ƶ�Root�£���ֹSystemWidget�����ظ�ɾ��
	EinkuiGetSystem()->GetElementManager()->SetParentElement(EinkuiGetSystem()->GetElementManager()->GetRootElement(),mpHomePage);

	// ����ϵͳ����Widget�ر�
	CExMessage::PostMessage(NULL,NULL,EMSG_CLOSE_WIDGET,this);
}
