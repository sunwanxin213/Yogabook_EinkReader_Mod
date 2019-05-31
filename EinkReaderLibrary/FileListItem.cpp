/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "FileListItem.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"

DEFINE_BUILTIN_NAME(FileListItem)

CFileListItem::CFileListItem(void)
{
	mpszFilePath = new wchar_t[MAX_PATH];
	mdwClickTicount = 0;
}


CFileListItem::~CFileListItem(void)
{
	delete mpszFilePath;
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CFileListItem::OnElementCreate(IEinkuiIterator* npIterator)
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

ULONG CFileListItem::InitOnCreate(
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

		do
		{
			//��ȡ�������ַ���
			//Ϊ�˷��뷽�㣬�ַ��������root/string
			lpProfile = EinkuiGetSystem()->GetCurrentWidget()->GetDefaultFactory()->GetTempleteFile();
			ICfKey* lpCfKey = NULL;
			if (lpProfile != NULL)
			{
				lpCfKey = lpProfile->OpenKey(L"String2/Title3");
				if (lpCfKey != NULL)
					lpCfKey->GetValue(mpszItem, MAX_PATH * sizeof(wchar_t));

			}

		} while (false);

		

		//��ȡ������
		mpIterFolderIcon = mpIterator->GetSubElementByID(1);
		BREAK_ON_NULL(mpIterFolderIcon);

		mpIterPdfIcon = mpIterator->GetSubElementByID(5);
		BREAK_ON_NULL(mpIterPdfIcon);

		mpIterName = mpIterator->GetSubElementByID(2);
		BREAK_ON_NULL(mpIterName);

		mpIterAttrib = mpIterator->GetSubElementByID(3);
		BREAK_ON_NULL(mpIterAttrib);

		mpIterBt = mpIterator->GetSubElementByID(FLI_BT);
		BREAK_ON_NULL(mpIterBt);
		
		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);
	CMM_SAFE_RELEASE(lpProfile);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

//����·��
void CFileListItem::SetPath(wchar_t* npszPath, wchar_t* npszDisplayName)
{
	do 
	{
		BREAK_ON_NULL(npszPath);

		//���ȳ��Ի�ȡ�ļ�����
		if ((GetFileAttributes(npszPath)&FILE_ATTRIBUTE_DIRECTORY) == false)
		{
			//���ļ�
			mpIterFolderIcon->SetVisible(false);
			mpIterPdfIcon->SetVisible(true);

			ProcFile(npszPath);
		}
		else
		{
			//���ļ���
			mpIterFolderIcon->SetVisible(true);
			mpIterPdfIcon->SetVisible(false);

			ProcFolder(npszPath, npszDisplayName);
		}

		wcscpy_s(mpszFilePath, MAX_PATH, npszPath);

	} while (false);
}

//���ļ��Ĵ���
void CFileListItem::ProcFile(wchar_t* npszPath)
{
	do 
	{
		BREAK_ON_NULL(npszPath);
		WIN32_FILE_ATTRIBUTE_DATA ldData;
		GetFileAttributesEx(npszPath, GetFileExInfoStandard, &ldData);
		SYSTEMTIME ldSystime;
		FileTimeToSystemTime(&ldData.ftLastWriteTime,&ldSystime);

		wchar_t lszString[MAX_PATH] = { 0 };
		swprintf_s(lszString, MAX_PATH, L"%04d/%02d/%02d  %02d:%02d",
			ldSystime.wYear, ldSystime.wMonth, ldSystime.wDay,
			ldSystime.wHour, ldSystime.wMinute);
		CExMessage::SendMessageWithText(mpIterAttrib, mpIterator, EACT_LABEL_SET_TEXT, lszString, NULL, 0);

		wchar_t* lpszFileName = wcsrchr(npszPath, L'\\')+1;
		CExMessage::SendMessageWithText(mpIterName, mpIterator, EACT_LABEL_SET_TEXT, lpszFileName, NULL, 0);

	} while (false);
}

//��ȡĿ¼��ָ���ļ���Ŀ¼����
DWORD CFileListItem::GetFolderCount(wchar_t* npszPath, wchar_t* npszName)
{
	DWORD ldwCount = 0;

	do
	{
		BREAK_ON_NULL(npszPath);

		wchar_t lszFindPath[MAX_PATH] = { 0 };
		wcscpy_s(lszFindPath, MAX_PATH, npszPath);
		if (npszName == NULL)
			wcscat_s(lszFindPath, MAX_PATH, L"\\*.*");
		else
			wcscat_s(lszFindPath, MAX_PATH, npszName);

		WIN32_FIND_DATA FindFileData;
		ZeroMemory(&FindFileData, sizeof(WIN32_FIND_DATA));
		HANDLE hFindFile = FindFirstFile(lszFindPath, &FindFileData);

		if (hFindFile == INVALID_HANDLE_VALUE)
			break;

		BOOL bContinue = true;

		while (bContinue != false)
		{
			//bIsDotsΪ���ʾ��.��..
			bool lbIsDots = (wcscmp(FindFileData.cFileName, L".") == 0 || wcscmp(FindFileData.cFileName, L"..") == 0);

			if (lbIsDots == false)
			{
				if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
				{
					//��Ŀ¼
					wchar_t* lpPath = new wchar_t[MAX_PATH];
					wcscpy_s(lpPath, MAX_PATH, npszPath);
					wcscat_s(lpPath, MAX_PATH, L"\\");
					wcscat_s(lpPath, MAX_PATH, FindFileData.cFileName);
					//�ж�һ�£���������ػ�ϵͳ�ļ��У��Ͳ�չʾ��
					if ((GetFileAttributes(lpPath)&FILE_ATTRIBUTE_HIDDEN) == 0 && (GetFileAttributes(lpPath)&FILE_ATTRIBUTE_SYSTEM) == 0)
						if (npszName == NULL)
							ldwCount++;
				}
				else
				{
					//���ļ�,npszName=null����ֻ��ҪĿ¼������
					if (npszName != NULL)
						ldwCount++;

				}
			}

			//Ѱ����һ�ļ�
			bContinue = FindNextFile(hFindFile, &FindFileData);

		}

		FindClose(hFindFile);

	} while (false);

	return ldwCount;
}

//���ļ��еĴ���
void CFileListItem::ProcFolder(wchar_t* npszPath, wchar_t* npszDisplayName)
{
	do
	{
		BREAK_ON_NULL(npszPath);

		//��ȡʱ��
		WIN32_FILE_ATTRIBUTE_DATA ldData;
		GetFileAttributesEx(npszPath, GetFileExInfoStandard, &ldData);
		SYSTEMTIME ldSystime;
		FileTimeToSystemTime(&ldData.ftLastWriteTime, &ldSystime);

		//��ȡ�ļ���Ŀ��ֻ����Ŀ¼��pdf�ļ�
		DWORD ldwCount = GetFolderCount(npszPath, NULL);
		ldwCount += GetFolderCount(npszPath, L"\\*.pdf");
		ldwCount += GetFolderCount(npszPath, L"\\*.epub");
		ldwCount += GetFolderCount(npszPath, L"\\*.mobi");
		ldwCount += GetFolderCount(npszPath, L"\\*.txt");

		wchar_t lszString[MAX_PATH] = { 0 };
		swprintf_s(lszString, MAX_PATH, L"%d %s | %04d/%02d/%02d  %02d:%02d", ldwCount, mpszItem,
			ldSystime.wYear, ldSystime.wMonth, ldSystime.wDay,
			ldSystime.wHour, ldSystime.wMinute);
		CExMessage::SendMessageWithText(mpIterAttrib, mpIterator, EACT_LABEL_SET_TEXT, lszString, NULL, 0);

		wchar_t* lpszFileName = wcsrchr(npszPath, L'\\');
		if (lpszFileName == NULL)
			lpszFileName = npszPath;
		else
			lpszFileName = lpszFileName + 1;

		//���ָ������ʾ���ƾ�ʹ��ָ����
		if (npszDisplayName != NULL)
			lpszFileName = npszDisplayName;
		CExMessage::SendMessageWithText(mpIterName, mpIterator, EACT_LABEL_SET_TEXT, lpszFileName, NULL, 0);


	} while (false);
}

//��ť�����¼�
ERESULT CFileListItem::OnCtlButtonClick(IEinkuiIterator* npSender)
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
				CExMessage::PostMessageWithText(mpIterator->GetParent(), mpIterator, EEVT_ER_LIST_CLICK, mpszFilePath);
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
//ERESULT CFileListItem::ParseMessage(IEinkuiMessage* npMsg)
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
void CFileListItem::OnTimer(
	PSTEMS_TIMER npStatus
	)
{

}

//Ԫ�زο��ߴ緢���仯
ERESULT CFileListItem::OnElementResized(D2D1_SIZE_F nNewSize)
{
	//CExMessage::SendMessage(mpIterBtFull, mpIterator, EACT_BUTTON_SET_ACTION_RECT, nNewSize);
	////mpIterLineOne->SetSize(nNewSize.width, mpIterLineOne->GetSize().height);

	//mpIterBtOk->SetPosition(nNewSize.width - 85, mpIterBtOk->GetPositionY());

	return ERESULT_SUCCESS;
}


//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CFileListItem::OnElementShow(bool nbIsShow)
{
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);

	return ERESULT_SUCCESS;
}