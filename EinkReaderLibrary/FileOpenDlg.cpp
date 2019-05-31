/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "ElementImp.h"
#include "FileOpenDlg.h"
#include "MsgDefine.h"
#include <stdio.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#include <Shellapi.h>

DEFINE_BUILTIN_NAME(FileOpenDlg)

CFileOpenDlg::CFileOpenDlg()
{
	mulCurrentPage = 1;
	mszCurrentPath[0] = UNICODE_NULL;
	mszDisplayPath[0] = UNICODE_NULL;
}

CFileOpenDlg::~CFileOpenDlg(void)
{
	//���ԭ�����б�
	ClearFilePath();
	
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CFileOpenDlg::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if (CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		mpIterator->ModifyStyles(EITR_STYLE_POPUP);

		

		/*mpIteratorPre = mpIterator->GetSubElementByID(TS_ID_BT_PRE);
		BREAK_ON_NULL(mpIteratorPre);

		mpIteratorNext = mpIterator->GetSubElementByID(TS_ID_BT_NEXT);
		BREAK_ON_NULL(mpIteratorNext);*/


	
		mpIterator->SetTimer(100, 1, 10, NULL);

		//mpIterator->SetPosition((),mpIterator->GetPositionY());
		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}

ULONG CFileOpenDlg::InitOnCreate(
	IN IEinkuiIterator* npParent,	// ������ָ��
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��EUIϵͳ�Զ�����
	)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;
	ICfKey* lpSubKey = NULL;

	do 
	{
		//���ȵ��ø���ķ���
		leResult = 	CXuiElement::InitOnCreate(npParent, npTemplete,nuEID);
		if(leResult != ERESULT_SUCCESS)
			break;

		//����list����
		lpSubKey = mpTemplete->OpenKey(L"ListItem");
		D2D1_POINT_2F ldPos = {0.0f,189};
		for (int i=0;i<FP_LIST_MAX;i++)
		{
			CFileListItem* lpListItem = CFileListItem::CreateInstance(mpIterator, lpSubKey);
			lpListItem->GetIterator()->SetPosition(ldPos);
			lpListItem->GetIterator()->SetVisible(false);
			ldPos.y += 140.0f;
			mdList.Insert(i, lpListItem);
		}
		CMM_SAFE_RELEASE(lpSubKey);
		

		mpIteratorClose = mpIterator->GetSubElementByID(FP_ID_BT_CLOSE);
		BREAK_ON_NULL(mpIteratorClose);

		mpIteratorPre = mpIterator->GetSubElementByID(FP_ID_BT_PRE);
		BREAK_ON_NULL(mpIteratorPre);

		mpIteratorNext = mpIterator->GetSubElementByID(FP_ID_BT_NEXT);
		BREAK_ON_NULL(mpIteratorNext);

		mpIteratorPage = mpIterator->GetSubElementByID(2);
		BREAK_ON_NULL(mpIteratorPage);

		mpIteratorPath = mpIterator->GetSubElementByID(5);
		BREAK_ON_NULL(mpIteratorPath);
		CExMessage::SendMessage(mpIteratorPath, mpIterator, EACT_LABEL_SET_MAX_HEIGHT, 44);

		mpIteratorBtBack = mpIterator->GetSubElementByID(FP_ID_BT_Back);
		BREAK_ON_NULL(mpIteratorBtBack);

		mpIteratorImBack = mpIterator->GetSubElementByID(6);
		BREAK_ON_NULL(mpIteratorImBack);
		

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	return leResult;
}

//����ԭ����·������
void CFileOpenDlg::ClearFilePath(void)
{
	while (mdFolderPathList.Size() > 0)
	{
		delete mdFolderPathList.GetEntry(0);
		mdFolderPathList.RemoveByIndex(0);
	}
}

//��ʼ��list,Ĭ����ʾ���������ļ��м��̷�
void CFileOpenDlg::InitList(void)
{
	wchar_t* lpszPath = NULL;
	CFileListItem* lpListItem = NULL;

	do 
	{
		ClearFilePath();
		while (mdFolderLevel.Size() > 0)
		{
			mdFolderLevel.RemoveByIndex(0);
		}

		//����
		lpszPath = new wchar_t[MAX_PATH];
		SHGetSpecialFolderPath(NULL, lpszPath, CSIDL_DESKTOP, FALSE);
		mdFolderPathList.Insert(mdFolderPathList.Size(), lpszPath);

		//�ҵ��ĵ�
		lpszPath = new wchar_t[MAX_PATH];
		SHGetSpecialFolderPath(NULL, lpszPath, CSIDL_MYDOCUMENTS, FALSE);
		mdFolderPathList.Insert(mdFolderPathList.Size(), lpszPath);

		//����
		wchar_t* lpszDownPath = new wchar_t[MAX_PATH];
		wcscpy_s(lpszDownPath, MAX_PATH, lpszPath);
		*(wcsrchr(lpszDownPath, L'\\') + 1) = UNICODE_NULL;
		wcscat_s(lpszDownPath, MAX_PATH, L"Downloads");
		mdFolderPathList.Insert(mdFolderPathList.Size(), lpszDownPath);

		mpIteratorImBack->SetVisible(false);
		mpIteratorPath->SetVisible(false);
		mpIteratorPre->SetEnable(false);
		mpIteratorNext->SetEnable(false);

		CExMessage::SendMessageWithText(mpIteratorPage, mpIterator, EACT_LABEL_SET_TEXT, L"1/1", NULL, 0);

		//��ȡ�����̷�
		wchar_t lszDrive[MAX_PATH] = { 0 };
		int i = 0;
		GetLogicalDriveStrings(MAX_PATH, lszDrive);
		while (lszDrive[i] != UNICODE_NULL)
		{
			lpszPath = new wchar_t[MAX_PATH];
			lpszPath[0] = lszDrive[i];
			lpszPath[1] = L':';
			lpszPath[2] = UNICODE_NULL;
			mdFolderPathList.Insert(mdFolderPathList.Size(), lpszPath);

			i += 4;
		}

		//�������ҳ��
		mulMaxPage = mdFolderPathList.Size() / FP_LIST_MAX;
		if (mulMaxPage*FP_LIST_MAX < (ULONG)mdFolderPathList.Size())
			mulMaxPage++; //ҳ�����ʹ�ý�1��

		mpIteratorPre->SetEnable(false);
		mpIteratorNext->SetEnable(false);
		if (mulMaxPage > 1)
		{
			mpIteratorNext->SetEnable(true);
		}

		//�򿪵�һҳ
		SetPage(1);

	} while (false);
}

void CFileOpenDlg::DoModal(bool nbIsEnableCancel)
{
	do 
	{
		mpIteratorClose->SetEnable(nbIsEnableCancel);

		mpIterator->SetActive();
		mpIterator->BringToTop();
		EinkuiGetSystem()->UpdateView(true);
		EinkuiGetSystem()->DoModal(mpIterator);
		

		mpIterator->Close();
		//mpIterator->Release();

	} while (false);
}

void CFileOpenDlg::ExitModal()
{
	
	EinkuiGetSystem()->ExitModal(mpIterator,0);
	
}

//��Ϣ������
ERESULT CFileOpenDlg::ParseMessage(IEinkuiMessage* npMsg)
{
	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	switch (npMsg->GetMessageID())
	{
	case EMSG_MODAL_ENTER:
	{
		//// ����Ҫ�����ĶԻ���
		//mpIterator->SetVisible(true);
		luResult = ERESULT_SUCCESS;
		break;
	}
	case EEVT_ER_LIST_CLICK:
	{
		//listĳ����
		wchar_t* lpszPath = (wchar_t*)npMsg->GetInputData();
		ListClick(lpszPath);
		break;
	}
	case EEVT_RBG_SELECTED_CHANGED:
	{
		//ѡ���˲�ͬ��ҳ��
		ULONG lulID = 0;
		luResult = CExMessage::GetInputData(npMsg, lulID);
		if (luResult != ERESULT_SUCCESS)
			break;

		if (lulID == FP_ID_GROUP_FILE)
		{
			mpIteratorPage->SetVisible(true);
			mpIteratorPre->SetVisible(true);
			mpIteratorNext->SetVisible(true);

			SetPage(mulCurrentPage);

			if (mdFolderLevel.Size() >= 1 && mszCurrentPath[0] != UNICODE_NULL)
			{
				//ֻ���ڷ�������ʾ
				mpIteratorPath->SetVisible(true);
				mpIteratorImBack->SetVisible(true);
			}
		}
		else if (lulID == FP_ID_GROUP_HISTORY)
		{
			wchar_t lszString[MAX_PATH] = { 0 };
			mpIteratorPath->SetVisible(false);
			mpIteratorImBack->SetVisible(false);

			do
			{
				//��ʾҳ��
				swprintf_s(lszString, MAX_PATH, L"%d/%d", 1, 1);
				CExMessage::SendMessageWithText(mpIteratorPage, mpIterator, EACT_LABEL_SET_TEXT, lszString, NULL, 0);
				mpIteratorPage->SetVisible(false);
				mpIteratorPre->SetVisible(false);
				mpIteratorNext->SetVisible(false); //��ʷ��¼ֻ��ʾһҳ��������ҳ

				//����list����
				int liBegin = (mulCurrentPage - 1) * FP_LIST_MAX;
				int i = 0, k = 0;
				for (i = 0, k = 0; i < mpdHistroyPath->Size() && k < FP_LIST_MAX; i++, k++)
				{
					if (GetFileAttributes(mpdHistroyPath->GetEntry(i)) == INVALID_FILE_ATTRIBUTES)
					{
						//����ļ��������ˣ��Ͳ���ʾ��һ����
						k--;
						continue;
					}

					CFileListItem* lpItem = mdList.GetEntry(k);
					lpItem->SetPath(mpdHistroyPath->GetEntry(i),NULL);
					lpItem->GetIterator()->SetVisible(true);
				}

				for (int j = k; j < FP_LIST_MAX; j++)
				{
					//û����ô�����ˣ��Ѻ��������
					mdList.GetEntry(j)->GetIterator()->SetVisible(false);
				}

			} while (false);
		}

		break;
	}
	default:
		luResult = ERESULT_NOT_SET;
		break;
	}

	if(luResult == ERESULT_NOT_SET)
	{
		luResult = CXuiElement::ParseMessage(npMsg); // ���û����ͬ��������ע�⣺һ��Ҫ��������ֱ�ӻ���
	}

	return luResult;
}

//list����
void CFileOpenDlg::ListClick(wchar_t* npszFilePath)
{
	do 
	{
		BREAK_ON_NULL(npszFilePath);

		//���ȳ��Ի�ȡ�ļ�����
		if ((GetFileAttributes(npszFilePath)&FILE_ATTRIBUTE_DIRECTORY) == false)
		{
			//���ļ���֪ͨ�����ڲ������Լ�
			bool lbRet = false;
			mpIterator->SetVisible(false);
			CExMessage::SendMessageWithText(mpIterator->GetParent(), mpIterator, EEVT_ER_OPEN_FILE_PATH, npszFilePath,&lbRet,sizeof(lbRet));
			if (lbRet != false)
				ExitModal(); //˵���ļ��򿪳ɹ��ˣ��Ǿ��˳��Լ�
			else
				mpIterator->SetTimer(FP_TIMER_ID_SHOW, 1, 3000, NULL);
				
				//mpIteratorClose->SetEnable(false); //���ļ�ʧ��,����ȡ����ť
		}
		else
		{
			//���ļ��У������ �ļ���
			EnterFolder(npszFilePath);
		}

	} while (false);
}

//��ȡĿ¼��ָ���ļ���Ŀ¼
DWORD CFileOpenDlg::GetFolderPath(wchar_t* npszPath, wchar_t* npszName)
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

			while (lbIsDots == false)
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
						mdFolderPathList.Insert(mdFolderPathList.Size(), lpPath);
				}
				else
				{
					//���ļ�,npszName=null����ֻ��ҪĿ¼������
					if (npszName != NULL)
					{
						wchar_t* lpPath = new wchar_t[MAX_PATH];
						wcscpy_s(lpPath, MAX_PATH, npszPath);
						wcscat_s(lpPath, MAX_PATH, L"\\");
						wcscat_s(lpPath, MAX_PATH, FindFileData.cFileName);
						mdFolderPathList.Insert(mdFolderPathList.Size(), lpPath);
					}

				}

				break;
			}

			//Ѱ����һ�ļ�
			bContinue = FindNextFile(hFindFile, &FindFileData);

		}

		FindClose(hFindFile);

	} while (false);

	return ldwCount;
}

//�����ļ���
void CFileOpenDlg::EnterFolder(wchar_t* npszPath, bool nbIsBack)
{
	do 
	{
		BREAK_ON_NULL(npszPath);

		wcscpy_s(mszCurrentPath, MAX_PATH, npszPath);
		mpIteratorImBack->SetEnable(true);

		//���ԭ�����б�
		ClearFilePath();

		//�����������ļ���e
		GetFolderPath(npszPath, NULL);
		GetFolderPath(npszPath, L"\\*.pdf");
		GetFolderPath(npszPath, L"\\*.epub");
		GetFolderPath(npszPath, L"\\*.mobi");
		GetFolderPath(npszPath, L"\\*.txt");

		wchar_t lszDisplayName[MAX_PATH] = { 0 };
		wchar_t* lpszFolderPath = npszPath;
		if (mdFolderLevel.Size() <= 0 && wcslen(lpszFolderPath) > 3)
		{
			//��ȡ��ʾ����
			if (StrStrI(lpszFolderPath, L"Desktop") != NULL)
				GetDisplayName(FOLDERID_Desktop, lszDisplayName, MAX_PATH);
			else if (StrStrI(lpszFolderPath, L"Documents") != NULL)
				GetDisplayName(FOLDERID_Documents, lszDisplayName, MAX_PATH);
			else if (StrStrI(lpszFolderPath, L"Pictures") != NULL)
				GetDisplayName(FOLDERID_Pictures, lszDisplayName, MAX_PATH);
			else if (StrStrI(lpszFolderPath, L"Downloads") != NULL)
				GetDisplayName(FOLDERID_Downloads, lszDisplayName, MAX_PATH);

			
		}
		if (nbIsBack == false)
		{
			//ֻ�в��Ƿ���ʱ������·��
			if (lszDisplayName[0] != UNICODE_NULL)
			{
				//ʹ����ʾ����
				wcscpy_s(mszDisplayPath, MAX_PATH, lszDisplayName);
			}
			else
			{
				wchar_t* lpszFileName = wcsrchr(npszPath, L'\\');
				if (lpszFileName == NULL)
					wcscpy_s(mszDisplayPath, MAX_PATH, npszPath);
				else
				{
					if (mszDisplayPath[0] != UNICODE_NULL)
						wcscat_s(mszDisplayPath, MAX_PATH, lpszFileName);
					else
						wcscpy_s(mszDisplayPath, MAX_PATH, npszPath);
				}

			}
		}

		CExMessage::SendMessageWithText(mpIteratorPath, mpIterator, EACT_LABEL_SET_TEXT, mszDisplayPath, NULL, 0);

		mpIteratorPath->SetVisible(true);
		mpIteratorImBack->SetVisible(true);

		//�������ҳ��
		mulMaxPage = mdFolderPathList.Size() / FP_LIST_MAX;
		if (mulMaxPage*FP_LIST_MAX < (ULONG)mdFolderPathList.Size())
			mulMaxPage++; //ҳ�����ʹ�ý�1��

		mpIteratorPre->SetEnable(false);
		mpIteratorNext->SetEnable(false);
		mpIteratorPage->SetVisible(true);

		if (nbIsBack == false)
		{
			//������Ƿ�����һ�����ͽ����һҳ
			mdFolderLevel.Insert(0, 1);
			SetPage(1);
		}
		else
		{
			//�Ƿ�����һ��������֮ǰ�뿪ʱ��ҳ��
			if(mdFolderLevel.Size()>0)
				mdFolderLevel.RemoveByIndex(0);

			SetPage(mdFolderLevel.Size() > 0 ? mdFolderLevel.GetEntry(0) : 1);
		}
		
		if (mulMaxPage <= 0)
		{
			//˵���ǿ��ļ���
			//mpIteratorPage->SetVisible(false);
			//��ʾҳ��
			wchar_t lszString[MAX_PATH] = { 0 };
			swprintf_s(lszString, MAX_PATH, L"%d/%d", 1, 1);
			CExMessage::SendMessageWithText(mpIteratorPage, mpIterator, EACT_LABEL_SET_TEXT, lszString, NULL, 0);

			break;
		}

		if (mulCurrentPage < mulMaxPage)
		{
			mpIteratorNext->SetEnable(true);
		}
		if(mulCurrentPage > 1)
		{
			mpIteratorPre->SetEnable(true);
		}

	} while (false);
}

//��ʱ��
void CFileOpenDlg::OnTimer(PSTEMS_TIMER npStatus)
{
	if (npStatus->TimerID == FP_TIMER_ID_SHOW)
	{
		mpIterator->SetVisible(true);
	}
	else
	{
		mpIterator->SetActive();
		mpIterator->BringToTop();
	}

}


//��ť�����¼�
ERESULT CFileOpenDlg::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		ULONG llBtnID = npSender->GetID();
		switch (llBtnID)
		{
		case FP_ID_BT_CLOSE:		// �ر�	
		{
			ExitModal();

			break;
		}
		case FP_ID_BT_PRE:
		{
			//��һҳ
			NextPage(false);
			
			break;
		}
		case FP_ID_BT_NEXT:
		{
			//��һҳ
			NextPage(true);

			break;
		}
		case FP_ID_BT_Back:
		{
			//�����ϼ�Ŀ¼
			BackFolder();
			break;
		}
		default:
			break;
		}


		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}

//Ԫ�زο��ߴ緢���仯
ERESULT CFileOpenDlg::OnElementResized(D2D1_SIZE_F nNewSize)
{
	/*EI_SIZE ldPaintSize;
	EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);

	D2D1_POINT_2F ldParentPos = mpIterator->GetParent()->GetPosition();
	FLOAT lfWidth = mpIterator->GetSizeX();
	FLOAT lfX = (ldPaintSize.w - lfWidth) / 2;
	lfX = lfX - ldParentPos.x;
	mpIterator->SetPosition(lfX, mpIterator->GetPositionY());*/

	return ERESULT_SUCCESS;
}

//������ʷ��¼
void CFileOpenDlg::SetHistoryList(cmmVector<wchar_t*>* npdHistroyPath)
{
	mpdHistroyPath = npdHistroyPath;

	if (mpdHistroyPath != NULL && mpdHistroyPath->Size() > 0)
	{
		//���֮ǰ�Ѿ��򿪹��ļ����ʹ��Ǹ��ļ����ڵ�Ŀ¼
		wchar_t lszPath[MAX_PATH] = { 0 };
		wcscpy_s(lszPath, MAX_PATH, mpdHistroyPath->GetEntry(0));

		*(wcsrchr(lszPath, L'\\')) = UNICODE_NULL;
		if (GetFileAttributes(lszPath) != INVALID_FILE_ATTRIBUTES)
		{
			while (mdFolderLevel.Size() > 0)
			{
				mdFolderLevel.RemoveByIndex(0);
			}

			//Ϊ�����������һ��һ�����أ��������Ӽ�¼
			wchar_t* lpszTemp = wcsstr(lszPath, L"\\");
			do 
			{
				mdFolderLevel.Insert(0, 1);
				if(lpszTemp == NULL)
					break; //????
				lpszTemp++;
				lpszTemp = wcsstr(lpszTemp, L"\\");

			} while (lpszTemp != NULL);

			EnterFolder(lszPath);
		}
		else
		{
			//Ŀ¼��������
			InitList();
		}
	}
	else
	{
		InitList();
	}
}


//��ȡ����Ŀ¼�Ķ������ַ���
bool CFileOpenDlg::GetDisplayName(GUID niCSIDL, OUT wchar_t* npszName, IN int niLen)
{
	bool lbRet = false;

	do
	{
		CoInitialize(NULL);
		IShellItem* lpDocItem;
		auto lhrRet = SHGetKnownFolderItem(niCSIDL, KF_FLAG_DEFAULT, NULL, IID_IShellItem, (void**)&lpDocItem); // SHGetKnownFolderPath ���ڻ�ȡĿ¼
		if (SUCCEEDED(lhrRet))
		{
			LPWSTR lpDisplayName = NULL;
			lhrRet = lpDocItem->GetDisplayName(SIGDN_NORMALDISPLAY, &lpDisplayName);

			wcscpy_s(npszName, niLen, lpDisplayName);
			if (lpDisplayName != NULL)
				CoTaskMemFree(lpDisplayName);

			lpDocItem->Release();
		}

		lbRet = true;

	} while (false);

	return lbRet;
}

//����ҳ��������ʾ
void CFileOpenDlg::SetPage(ULONG nulPage)
{
	wchar_t lszString[MAX_PATH] = { 0 };
	int i = 0, k = 0;

	do 
	{
		if(nulPage < 0 || nulPage > mulMaxPage)
			break; //������Ч��Χ

		//��¼��ǰҳ��
		if (mulMaxPage <= 0)
			mulCurrentPage = 0; //˵���Ǹ����ļ���
		else
			mulCurrentPage = nulPage;
		
		//��ʾҳ��
		swprintf_s(lszString, MAX_PATH, L"%d/%d", mulCurrentPage, mulMaxPage);
		CExMessage::SendMessageWithText(mpIteratorPage, mpIterator, EACT_LABEL_SET_TEXT, lszString, NULL, 0);

		//����list����
		int liBegin = (mulCurrentPage - 1) * FP_LIST_MAX;
		
		for (i= liBegin,k=0;i<mdFolderPathList.Size() && k<FP_LIST_MAX;i++,k++)
		{
			CFileListItem* lpItem = mdList.GetEntry(k);

			wchar_t lszDisplayName[MAX_PATH] = { 0 };
			wchar_t* lpszFolderPath = mdFolderPathList.GetEntry(i);
			if (mdFolderLevel.Size() <= 0 && wcslen(lpszFolderPath) > 3)
			{
				//��ȡ��ʾ����
				if (StrStrI(lpszFolderPath, L"Desktop") != NULL)
					GetDisplayName(FOLDERID_Desktop, lszDisplayName, MAX_PATH);
				else if (StrStrI(lpszFolderPath, L"Documents") != NULL)
					GetDisplayName(FOLDERID_Documents, lszDisplayName, MAX_PATH);
				else if (StrStrI(lpszFolderPath, L"Pictures") != NULL)
					GetDisplayName(FOLDERID_Pictures, lszDisplayName, MAX_PATH);
				else if (StrStrI(lpszFolderPath, L"Downloads") != NULL)
					GetDisplayName(FOLDERID_Downloads, lszDisplayName, MAX_PATH);
			}

			lpItem->SetPath(mdFolderPathList.GetEntry(i), lszDisplayName[0] == UNICODE_NULL ? NULL : lszDisplayName);
			//lpItem->SetPath(mdFolderPathList.GetEntry(i));
			lpItem->GetIterator()->SetVisible(true);
		}

		//����ҳ��
		if(mdFolderLevel.Size() > 0)
			mdFolderLevel.GetEntry(0) = nulPage;

	} while (false);

	for (int j = k; j < FP_LIST_MAX; j++)
	{
		//û����ô�����ˣ��Ѻ��������
		mdList.GetEntry(j)->GetIterator()->SetVisible(false);
	}
}

//������һ��Ŀ¼
void CFileOpenDlg::BackFolder(void)
{
	do 
	{
		if (mdFolderLevel.Size() <= 1 || mszCurrentPath[0] == UNICODE_NULL)
		{
			mszDisplayPath[0] = UNICODE_NULL;
			InitList();
			break; //û��·���ˣ����ص����µ��б�
		}
		
		wchar_t* lpszFind = wcsrchr(mszCurrentPath, L'\\');
		if (lpszFind != NULL)
		{
			//˵������·��
			*(wcsrchr(mszCurrentPath, L'\\')) = UNICODE_NULL;
			*(wcsrchr(mszDisplayPath, L'\\')) = UNICODE_NULL;

			EnterFolder(mszCurrentPath,true);
		}
		else
		{
			//˵�����������
			mszCurrentPath[0] = UNICODE_NULL;
			mszDisplayPath[0] = UNICODE_NULL;

			InitList();
		}

	} while (false);
}

//��һҳ����һҳ
void CFileOpenDlg::NextPage(bool nbIsNext)
{
	do
	{
		if (nbIsNext == false)
		{
			//��һҳ
			mpIteratorNext->SetEnable(true);
			if (mulCurrentPage > 1)
				mulCurrentPage--;

			if (mulCurrentPage <= 1)
			{
				mpIteratorPre->SetEnable(false);

				//�Ѿ��ǵ�һҳ��
			}
		}
		else
		{
			//����
			mpIteratorPre->SetEnable(true);
			if (mulCurrentPage < mulMaxPage)
				mulCurrentPage++;

			if (mulCurrentPage >= mulMaxPage)
			{
				mpIteratorNext->SetEnable(false);
				//�Ѿ��Ǻ�һҳ��
			}
		}

		SetPage(mulCurrentPage);

	} while (false);
}
