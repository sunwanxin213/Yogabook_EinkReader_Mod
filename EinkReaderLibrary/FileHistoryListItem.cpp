/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "FileHistoryListItem.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"
#include <time.h>

DEFINE_BUILTIN_NAME(FileHistoryListItem)

CFileHistoryListItem::CFileHistoryListItem(void)
{
	mdwClickTicount = 0;
	mpIterFolderIcon = NULL;
	mpIterModifyIcon = NULL;
	mpIterName = NULL;
	mpIterAttrib = NULL;
}


CFileHistoryListItem::~CFileHistoryListItem(void)
{

}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CFileHistoryListItem::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		//mpIterator->ModifyStyles(EITR_STYLE_POPUP);
		
		//mpIterPicture->SetRotation(90.0f);

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

ULONG CFileHistoryListItem::InitOnCreate(
	IN IEinkuiIterator* npParent,	// ������ָ��
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;
	ICfKey* lpSubKey = NULL;
	IConfigFile* lpProfile = NULL;

	do 
	{
		//���ȵ��û���
		leResult = 	CXuiElement::InitOnCreate(npParent,npTemplete,nuEID);
		if(leResult != ERESULT_SUCCESS)
			break;


		//��ȡ������
		mpIterFolderIcon = mpIterator->GetSubElementByID(1);
		BREAK_ON_NULL(mpIterFolderIcon);

		mpIterName = mpIterator->GetSubElementByID(2);
		BREAK_ON_NULL(mpIterName);

		mpIterAttrib = mpIterator->GetSubElementByID(3);
		BREAK_ON_NULL(mpIterAttrib);

		mpIterModifyIcon = mpIterator->GetSubElementByID(4);
		BREAK_ON_NULL(mpIterModifyIcon);

		mpIterBt = mpIterator->GetSubElementByID(FLI_BT);
		BREAK_ON_NULL(mpIterBt);

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);
	CMM_SAFE_RELEASE(lpProfile);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

//�����ļ�����
void CFileHistoryListItem::SetData(HISTORY_FILE_ATTRIB* npFileAttrib)
{
	do 
	{
		BREAK_ON_NULL(npFileAttrib);
		mpFileAttrib = npFileAttrib;
		wchar_t lpszPicPath[MAX_PATH] = { 0 };
		wchar_t* lpszFilePath = npFileAttrib->FilePath;
		int liLastChar = int(wcslen(lpszFilePath) - 1);
		if (lpszFilePath[liLastChar] == 't')
			wcscpy_s(lpszPicPath, MAX_PATH, L".\\Pic\\txt.png");
		else if (lpszFilePath[liLastChar] == 'b')
			wcscpy_s(lpszPicPath, MAX_PATH, L".\\Pic\\epub.png");
		else if (lpszFilePath[liLastChar] == 'i')
			wcscpy_s(lpszPicPath, MAX_PATH, L".\\Pic\\mobi.png");
		else if (lpszFilePath[liLastChar] == 'f')
			wcscpy_s(lpszPicPath, MAX_PATH, L".\\Pic\\pdf.png");

		ProcFile(npFileAttrib);

		CExMessage::SendMessageWithText(mpIterFolderIcon, mpIterator, EACT_PICTUREFRAME_CHANGE_PIC, lpszPicPath);


	} while (false);
}

//�Ƿ����õ��
void CFileHistoryListItem::SetEnable(bool nbIsEnable)
{
	mpIterBt->SetEnable(nbIsEnable);
}

//���ļ��Ĵ���
void CFileHistoryListItem::ProcFile(HISTORY_FILE_ATTRIB* npFileAttrib)
{
	do 
	{
		BREAK_ON_NULL(npFileAttrib);

		SYSTEMTIME ldSystime;
		GetSystemTime(&ldSystime);
		//FileTimeToSystemTime(&ldData.ftLastWriteTime,&ldSystime);
		struct tm ldTime;
		time_t ldReadTime = (time_t)npFileAttrib->TimeStamp;
		localtime_s(&ldTime, &ldReadTime);
		ldTime.tm_year = ldTime.tm_year + 1900;
		wchar_t lszString[MAX_PATH] = { 0 };
		/*swprintf_s(lszString, MAX_PATH, L"%d%s | %04d/%02d/%02d  %02d:%02d",
			npFileAttrib->ReadProgress,L"%",
			ldTime.tm_year + 1900, ldTime.tm_mon+1, ldTime.tm_mday,
			ldTime.tm_hour, ldTime.tm_min);*/

		

		if (ldSystime.wYear == ldTime.tm_year)
		{
			swprintf_s(lszString, MAX_PATH, L"%d%s | %02d-%02d",
				npFileAttrib->ReadProgress, L"%", ldTime.tm_mon + 1, ldTime.tm_mday);
		}
		else
		{
			swprintf_s(lszString, MAX_PATH, L"%d%s | %04d-%02d-%02d",
				npFileAttrib->ReadProgress, L"%",
				ldTime.tm_year, ldTime.tm_mon + 1, ldTime.tm_mday);
		}
		
		if (npFileAttrib->IsModify == 0)
		{
			//����ʾ�޸�ͼ�� 
			mpIterModifyIcon->SetVisible(false);
			mpIterAttrib->SetPosition(mpIterModifyIcon->GetPosition());
		}
		else
		{
			//��ʾ�޸�ͼ��
			mpIterModifyIcon->SetVisible(true);
			mpIterAttrib->SetPosition(mpIterModifyIcon->GetPositionX() + 40.0f, mpIterModifyIcon->GetPositionY());

			wchar_t lszTemp[MAX_PATH] = { 0 };
			wcscpy_s(lszTemp, MAX_PATH, lszString);
			swprintf_s(lszString, MAX_PATH, L" | %s", lszTemp);
		}

		CExMessage::SendMessageWithText(mpIterAttrib, mpIterator, EACT_LABEL_SET_TEXT, lszString, NULL, 0);

		wchar_t* lpszFileName = wcsrchr(npFileAttrib->FilePath, L'\\')+1;
		CExMessage::SendMessageWithText(mpIterName, mpIterator, EACT_LABEL_SET_TEXT, lpszFileName, NULL, 0);



	} while (false);
}

//��ť�����¼�
ERESULT CFileHistoryListItem::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		ULONG llBtnID = npSender->GetID();
		switch (llBtnID)
		{
		case FLI_BT:
		{
			//�Լ������
			if (GetTickCount() - mdwClickTicount > 400)
			{
				//Ϊ�˷�ֹ˫��
				mdwClickTicount = GetTickCount();
				CExMessage::PostMessage(mpIterator->GetParent(), mpIterator, EEVT_ER_LIST_CLICK, mpFileAttrib);
				//CExMessage::PostMessageWithText(mpIterator->GetParent(), mpIterator, EEVT_ER_LIST_CLICK, mpFileAttrib->FilePath);
			}
			
			break;
		}
		default:
			break;
		}


		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}


//��Ϣ������
//ERESULT CFileHistoryListItem::ParseMessage(IEinkuiMessage* npMsg)
//{
//	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;
//
//	switch (npMsg->GetMessageID())
//	{
//	default:
//		luResult = ERESULT_NOT_SET;
//		break;
//	}
//
//	if (luResult == ERESULT_NOT_SET)
//	{
//		luResult = CXuiElement::ParseMessage(npMsg); // ���û����ͬ��������ע�⣺һ��Ҫ��������ֱ�ӻ���
//	}
//
//	return luResult;
//}

//��ʱ��
void CFileHistoryListItem::OnTimer(
	PSTEMS_TIMER npStatus
	)
{

}

//Ԫ�زο��ߴ緢���仯
ERESULT CFileHistoryListItem::OnElementResized(D2D1_SIZE_F nNewSize)
{
	//CExMessage::SendMessage(mpIterBtFull, mpIterator, EACT_BUTTON_SET_ACTION_RECT, nNewSize);
	////mpIterLineOne->SetSize(nNewSize.width, mpIterLineOne->GetSize().height);

	//mpIterBtOk->SetPosition(nNewSize.width - 85, mpIterBtOk->GetPositionY());

	return ERESULT_SUCCESS;
}


//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CFileHistoryListItem::OnElementShow(bool nbIsShow)
{
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);

	return ERESULT_SUCCESS;
}