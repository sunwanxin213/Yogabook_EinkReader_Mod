/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include <unordered_map>
#include "FileListItem.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"
#include "util.h"
#include <algorithm>

DEFINE_BUILTIN_NAME(FileListItem)

namespace util = EInkReaderUtil;

using std::unordered_map;

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

		/*mpIterPdfIcon = mpIterator->GetSubElementByID(5);
		BREAK_ON_NULL(mpIterPdfIcon)*/;

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
	const unordered_map<wstring, wstring> kIconMap{
		{L".txt", LR"###(.\Pic\txt.png)###"},
		{L".epub", LR"###(.\Pic\epub.png)###"},
		{L".mobi", LR"###(.\Pic\mobi.png)###"},
		{L".pdf", LR"###(.\Pic\pdf.png)###"},
		{L".doc", LR"###(.\Pic\doc.png)###"},
		{L".docx", LR"###(.\Pic\docx.png)###"},
		{L".xls", LR"###(.\Pic\xls.png)###"},
		{L".xlsx", LR"###(.\Pic\xlsx.png)###"},
		{L".ppt", LR"###(.\Pic\ppt.png)###"},
		{L".pptx", LR"###(.\Pic\pptx.png)###"},
		{L".vsd", LR"###(.\Pic\vsd.png)###"},
		{L".vsdx", LR"###(.\Pic\vsdx.png)###"},
	};

	const wchar_t* kFolderIconPath = LR"###(.\Pic\ic_toolbar_folder_normal.png)###";
	const wchar_t* kUnknownFileIconPath = LR"###(.\Pic\common_file.png)###";

	do 
	{
		BREAK_ON_NULL(npszPath);

		wchar_t lpszPicPath[MAX_PATH] = { 0 };

		//���ȳ��Ի�ȡ�ļ�����
		if ((GetFileAttributes(npszPath)&FILE_ATTRIBUTE_DIRECTORY) == false)
		{
			//���ļ�
			wstring&& extName = util::GetFileExtention(npszPath, util::NameCaseOption::ToLower);
			auto iter = kIconMap.find(extName);
			wstring iconPath;

			if (iter == kIconMap.end())
				iconPath = kUnknownFileIconPath;
			else
				iconPath = iter->second;

			wcscpy_s(lpszPicPath, MAX_PATH, iconPath.c_str());
			ProcFile(npszPath);
		}
		else
		{
			//���ļ���
			wcscpy_s(lpszPicPath, MAX_PATH, kFolderIconPath);
			ProcFolder(npszPath, npszDisplayName);
		}

		CExMessage::SendMessageWithText(mpIterFolderIcon, mpIterator, EACT_PICTUREFRAME_CHANGE_PIC, lpszPicPath);

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
		/*swprintf_s(lszString, MAX_PATH, L"%04d/%02d/%02d  %02d:%02d",
			ldSystime.wYear, ldSystime.wMonth, ldSystime.wDay,
			ldSystime.wHour, ldSystime.wMinute);*/

		swprintf_s(lszString, MAX_PATH, L"%02d/%02d",
			ldSystime.wMonth, ldSystime.wDay);

		CExMessage::SendMessageWithText(mpIterAttrib, mpIterator, EACT_LABEL_SET_TEXT, lszString, NULL, 0);

		wchar_t* lpszFileName = wcsrchr(npszPath, L'\\')+1;
		CExMessage::SendMessageWithText(mpIterName, mpIterator, EACT_LABEL_SET_TEXT, lpszFileName, NULL, 0);

	} while (false);
}

//��ȡĿ¼��ָ���ļ���Ŀ¼����
DWORD CFileListItem::GetFolderCount(wchar_t* npszPath)
{
	DWORD ldwCount = 0;

	const vector<const wchar_t*> relatedExtList{
		L".pdf",
		L".epub",
		L".mobi",
		L".txt",
		L".doc",
		L".docx",
		L".xls",
		L".xlsx",
		L".ppt",
		L".pptx",
		L".vsd",
		L".vsdx",
	};

	if (!npszPath) return 0;

	wchar_t lszFindPath[MAX_PATH] = { 0 };
	wcscpy_s(lszFindPath, MAX_PATH, npszPath);
	swprintf_s(lszFindPath, MAX_PATH, L"%s\\*", npszPath);
	WIN32_FIND_DATA FindFileData;
	ZeroMemory(&FindFileData, sizeof(WIN32_FIND_DATA));
	HANDLE hFindFile = FindFirstFile(lszFindPath, &FindFileData);

	if (hFindFile == INVALID_HANDLE_VALUE)
		return 0;

	BOOL haveNextFile = true;

	vector<wstring> dirList;
	vector<wstring> fileList;

	while (haveNextFile)
	{
		//��.����..
		if (wcscmp(FindFileData.cFileName, L".") == 0 || wcscmp(FindFileData.cFileName, L"..") == 0)
		{
			haveNextFile = FindNextFile(hFindFile, &FindFileData);
			continue;
		}

		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)  // ����Ŀ¼
		{
			wstring result = npszPath;
			result += L"\\";
			result += FindFileData.cFileName;
			// ��������Ŀ¼��ϵͳĿ¼
			if ((GetFileAttributes(result.c_str())&FILE_ATTRIBUTE_HIDDEN) == 0 && (GetFileAttributes(result.c_str())&FILE_ATTRIBUTE_SYSTEM) == 0)
			{
				ldwCount += 1;
			}
		}
		else
		{
			wstring currentExtName = util::GetFileExtention(FindFileData.cFileName, util::NameCaseOption::ToLower);
			if (std::any_of(relatedExtList.begin(), relatedExtList.end(), [currentExtName](const wchar_t* ext) {
				return currentExtName == ext;
			}))
			{
				wstring result = npszPath;
				result += L"\\";
				result += FindFileData.cFileName;

				if ((GetFileAttributes(result.c_str())&FILE_ATTRIBUTE_HIDDEN) == 0 && (GetFileAttributes(result.c_str())&FILE_ATTRIBUTE_SYSTEM) == 0)
				{
					ldwCount += 1;//fileList.push_back(result);
				}
			}

		}

		haveNextFile = FindNextFile(hFindFile, &FindFileData);
	}

	FindClose(hFindFile);
	
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
		DWORD ldwCount = GetFolderCount(npszPath);
		//ldwCount += GetFolderCount(npszPath, L"\\*.pdf");
		//ldwCount += GetFolderCount(npszPath, L"\\*.epub");
		//ldwCount += GetFolderCount(npszPath, L"\\*.mobi");
		//ldwCount += GetFolderCount(npszPath, L"\\*.txt");
		//ldwCount += GetFolderCount(npszPath, L"\\*.docx");
		//ldwCount += GetFolderCount(npszPath, L"\\*.doc");
		//ldwCount += GetFolderCount(npszPath, L"\\*.rtf");
		//ldwCount += GetFolderCount(npszPath, L"\\*.xlsx");
		//ldwCount += GetFolderCount(npszPath, L"\\*.csv");
		//ldwCount += GetFolderCount(npszPath, L"\\*.pptx");
		//ldwCount += GetFolderCount(npszPath, L"\\*.rtf");
		//ldwCount += GetFolderCount(npszPath, L"\\*.ppt");
		//ldwCount += GetFolderCount(npszPath, L"\\*.vsdx");
		//ldwCount += GetFolderCount(npszPath, L"\\*.vsd");

		wchar_t lszString[MAX_PATH] = { 0 };
		/*swprintf_s(lszString, MAX_PATH, L"%d %s | %04d/%02d/%02d  %02d:%02d", ldwCount, mpszItem,
			ldSystime.wYear, ldSystime.wMonth, ldSystime.wDay,
			ldSystime.wHour, ldSystime.wMinute);*/

		swprintf_s(lszString, MAX_PATH, L"%d %s", ldwCount, mpszItem);

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