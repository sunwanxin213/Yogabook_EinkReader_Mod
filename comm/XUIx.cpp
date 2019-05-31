/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "stdafx.h"

#include "cmmBaseObj.h"
#include "cmmstruct.h"
#include "Einkui.h"
#include "Xuix.h"
#include "ElementImp.h"
#include "XCtl.h"






//////////////////////////////////////////////////////////////////////////
// CExMessage
IXelManager* CExMessage::gpElementManager = NULL;
void* CExMessage::DataInvalid=NULL;

DEFINE_BUILTIN_NAME(CExWinPromptBox)

// Postģʽ������Ϣ������Я��һ���ַ�����ע�⣬�ַ���һ��Ҫ����\0��β
// ���û�����ݷ��ͣ�rDataToPost��дCExMessage::DataInvalid;		nuFast��Ϣ�����ȼ���EMSG_POST_NORMAL,EMSG_POST_FAST,EMSG_POST_REVERSE
ERESULT CExMessage::PostMessageWithText(IEinkuiIterator* npReceiver,IEinkuiIterator* npSender,ULONG nuMsgID,const wchar_t* nswText,ULONG nuFast)
{
	ERESULT luResult;
	IEinkuiMessage* lpMsgIntf;
	int liBytes;

	if(nswText != NULL)
		liBytes = (int)(wcslen(nswText)+1)*sizeof(wchar_t);
	else
		liBytes = 0;

	lpMsgIntf = MakeUpMessage(npSender,false,nuMsgID,nswText,liBytes,NULL,0);

	if(lpMsgIntf == NULL)
		return ERESULT_INSUFFICIENT_RESOURCES;

	luResult = gpElementManager->PostMessage(npReceiver,lpMsgIntf,nuFast);

	lpMsgIntf->Release();

	return luResult;
}

// Sendģʽ������Ϣ������Я��һ���ַ�����ע�⣬�ַ���һ��Ҫ����\0��β
// ���û�����ݷ��ͣ�rDataToPost��дCExMessage::DataInvalid
ERESULT CExMessage::SendMessageWithText(IEinkuiIterator* npReceiver,IEinkuiIterator* npSender,ULONG nuMsgID,const wchar_t* nswText,void* npBufferForReceive,int niBytesOfBuffer)
{
	ERESULT luResult;
	IEinkuiMessage* lpMsgIntf;
	int liBytes;

	if(nswText != NULL)
		liBytes = (int)(wcslen(nswText)+1)*sizeof(wchar_t);
	else
		liBytes = 0;

	lpMsgIntf = MakeUpMessage(npSender,true,nuMsgID,nswText,liBytes,npBufferForReceive,niBytesOfBuffer);

	if(lpMsgIntf == NULL)
		return ERESULT_INSUFFICIENT_RESOURCES;

	luResult = gpElementManager->SendMessage(npReceiver,lpMsgIntf);

	lpMsgIntf->Release();

	return luResult;
}

// Postģʽ������Ϣ������Я������������
// ���û�����ݷ��ͣ�rDataToPost��дCExMessage::DataInvalid;		nuFast��Ϣ�����ȼ���EMSG_POST_NORMAL,EMSG_POST_FAST,EMSG_POST_REVERSE
ERESULT CExMessage::PostMessageWithBuffer(IEinkuiIterator* npReceiver,IEinkuiIterator* npSender,ULONG nuMsgID,const void* npBuffer,int niBytes,ULONG nuFast)
{
	ERESULT luResult;
	IEinkuiMessage* lpMsgIntf;

	lpMsgIntf = MakeUpMessage(npSender,false,nuMsgID,npBuffer,niBytes,NULL,0);

	if(lpMsgIntf == NULL)
		return ERESULT_INSUFFICIENT_RESOURCES;

	luResult = gpElementManager->PostMessage(npReceiver,lpMsgIntf,nuFast);

	lpMsgIntf->Release();

	return luResult;
}




// ��ʾWindows��MessageBox������ͬWindows����ȫһ��
int CExWinPromptBox::MessageBox(const wchar_t* nswText,const wchar_t* nswTitle,UINT nuType)
{
	ST_MESSAGEBOX ldContext;

	ldContext.Text = nswText;
	ldContext.Title = nswTitle;
	ldContext.Type = nuType;
	ldContext.Result = 0;

	EinkuiGetSystem()->CallBackByWinUiThread(NULL,(PXUI_CALLBACK)&CExWinPromptBox::MessageBoxCallBack,0,&ldContext);

	return ldContext.Result;
}


ERESULT __stdcall CExWinPromptBox::MessageBoxCallBack(ULONG nuFlag,LPVOID npContext)
{
	ST_MESSAGEBOX* lpContext = (ST_MESSAGEBOX*)npContext;

	lpContext->Result = ::MessageBox(EinkuiGetSystem()->GetMainWindow(),lpContext->Text,lpContext->Title,lpContext->Type);

	return ERESULT_SUCCESS;
}


// ��ʾWindows��MessageBox������ͬWindows����ȫһ��
int CExWinPromptBox::MessageBoxEx(const wchar_t* nswText,const wchar_t* nswTitle,UINT nuType,WORD nsuLanguage)
{
	ST_MESSAGEBOX ldContext;

	ldContext.Text = nswText;
	ldContext.Title = nswTitle;
	ldContext.Type = nuType;
	ldContext.Language = nsuLanguage;
	ldContext.Result = 0;

	EinkuiGetSystem()->CallBackByWinUiThread(NULL,(PXUI_CALLBACK)&CExWinPromptBox::MessageBoxExCallBack,0,&ldContext);

	return ldContext.Result;
}


ERESULT __stdcall CExWinPromptBox::MessageBoxExCallBack(ULONG nuFlag,LPVOID npContext)
{
	ST_MESSAGEBOX* lpContext = (ST_MESSAGEBOX*)npContext;

	lpContext->Result = ::MessageBoxEx(EinkuiGetSystem()->GetMainWindow(),lpContext->Text,lpContext->Title,lpContext->Type,lpContext->Language);

	return ERESULT_SUCCESS;
}

// ���������
HCURSOR CExWinPromptBox::SetCursor(HCURSOR nhCursor)
{
	if(EinkuiGetSystem()->CallBackByWinUiThread(NULL,(PXUI_CALLBACK)&CExWinPromptBox::SetCursorCallBack,0,&nhCursor,true)!= ERESULT_SUCCESS)
		nhCursor =NULL;

	return nhCursor;
}

ERESULT __stdcall CExWinPromptBox::SetCursorCallBack(ULONG nuFlag,LPVOID npContext)
{
	HCURSOR* lpCursor = (HCURSOR*)npContext;

	*lpCursor = ::SetCursor(*lpCursor);

	return ERESULT_SUCCESS;
}






//���ڼ��㱻�Ŵ�ͼ�ϵ������Ӧ��ԭͼ������
//rdSrcSizeԭͼ��С
//rdDestSize��ǰ����ʾ��С
//rdPointҪת��������
//nuMethodʹ�õķŴ�ʽ��1��ESPB_DRAWBMP_EXTEND����չ�߷�ʽ�� 2������ֵ��ʾֱ������ 
//rdExtendLine���ʹ�õ���ESPB_DRAWBMP_EXTEND�Ŵ�ʽ����Ҫ������չ������
D2D1_POINT_2F CExPoint::BigToOldPoint(D2D1_SIZE_F& rdSrcSize,D2D1_SIZE_F& rdDestSize,D2D1_POINT_2F& rdPoint,ULONG nuMethod,D2D1_POINT_2F& rdExtendLine)
{
	D2D1_POINT_2F ldPoint = rdPoint;

	do 
	{
		if((rdDestSize.width - rdSrcSize.width) < 1.0f && (rdDestSize.height - rdSrcSize.height) < 1.0f)
			break; //û�зŴ�

		if (nuMethod == ESPB_DRAWBMP_EXTEND)
		{
			//��չ�߷�ʽ
			if (rdPoint.x > rdExtendLine.x)
			{
				ldPoint.x = rdPoint.x - (rdDestSize.width-rdSrcSize.width);
				if(ldPoint.x < rdExtendLine.x)
					ldPoint.x = rdExtendLine.x;
			}
			else
			{
				ldPoint.x = rdPoint.x;
			}

			if (rdPoint.y > rdExtendLine.y)
			{
				ldPoint.y = rdPoint.y - (rdDestSize.height-rdSrcSize.height);
				if(ldPoint.y < rdExtendLine.y)
					ldPoint.y = rdExtendLine.y;
			}
			else
			{
				ldPoint.y = rdPoint.y;
			}
		}
		else
		{
			//ֱ�ӷŴ�ķ�ʽ
			rdPoint.x *= rdDestSize.width / rdSrcSize.width;
			rdPoint.y *= rdDestSize.height / rdSrcSize.height;
		}

	} while (false);

	return ldPoint;
}


bool CExHotkey::RegisterHotKey(
	IEinkuiIterator* npFocusOn,	// ��ע�˶���������Ӷ���NULL��ʾ������
	IEinkuiIterator* npReceiver,	// �������տ�ݼ��Ķ���
	ULONG nuSysHotkeyID		// ϵͳĬ�ϵ�ID����lwUI�ļ�IXelManager����ĩβ��ϵͳĬ�Ͽ�ݼ�ID��
	)
{
	bool lbOK = false;

	switch(nuSysHotkeyID)
	{
	case EHOTKEY_COPY:
		EinkuiGetSystem()->GetElementManager()->RegisterHotKey(npReceiver,EHOTKEY_COPY,'C',true,false,false,npFocusOn);
		break;
	case EHOTKEY_CUT:
		EinkuiGetSystem()->GetElementManager()->RegisterHotKey(npReceiver,EHOTKEY_CUT,'X',true,false,false,npFocusOn);
		break;
	case EHOTKEY_PASTE:
		EinkuiGetSystem()->GetElementManager()->RegisterHotKey(npReceiver,EHOTKEY_PASTE,'V',true,false,false,npFocusOn);
		break;
	case EHOTKEY_SELECT_ALL:
		EinkuiGetSystem()->GetElementManager()->RegisterHotKey(npReceiver,EHOTKEY_SELECT_ALL,'A',true,false,false,npFocusOn);
		break;
	case EHOTKEY_ESC:
		EinkuiGetSystem()->GetElementManager()->RegisterHotKey(npReceiver,EHOTKEY_ESC,VK_ESCAPE,false,false,false,npFocusOn);
		break;
	case EHOTKEY_ENTER:
		EinkuiGetSystem()->GetElementManager()->RegisterHotKey(npReceiver,EHOTKEY_ENTER,VK_RETURN,false,false,false,npFocusOn);
		break;
	case EHOTKEY_DELETE:
		EinkuiGetSystem()->GetElementManager()->RegisterHotKey(npReceiver,EHOTKEY_DELETE,VK_DELETE,false,false,false,npFocusOn);
		break;
	case EHOTKEY_ALTF4:
		EinkuiGetSystem()->GetElementManager()->RegisterHotKey(npReceiver,EHOTKEY_ALTF4,VK_F4,false,false,true,npFocusOn);
		break;
	default:;
	}

	return lbOK;
}

