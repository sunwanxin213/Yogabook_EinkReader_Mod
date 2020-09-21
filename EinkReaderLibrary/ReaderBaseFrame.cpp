/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include <sstream>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <cctype>
#include "ReaderBaseFrame.h"
#include "CYesNoPromptDlg.h"
#include "PDFOverwriteDlg.h"
#include "ConvertProgressDlg.h"
#include "OpenFileLockedDlg.h"
#include "XCtl.h"
#include <shellapi.h>

#include "SmCuveImp.h"
#include "stdio.h"
#include "HellFun.h"
#include <tlhelp32.h>
#include "winsock.h"

#include "cmmStrHandle.h"
#include "math.h"
#include "cmmPath.h"
#include "MsgDefine.h"
#include <Shlobj.h>
#include "time.h"
#include "util.h"

DEFINE_BUILTIN_NAME(Reader_BaseFrame)

const wchar_t* kReaderTempPath = LR"###(Lenovo\Reader)###";
const wchar_t* kCommandLineParamFile = L"cmdparam";

using std::wostringstream;
using std::unique_ptr;
using std::make_unique;

CReaderBaseFrame::CReaderBaseFrame(void)
{
	mpFileHistoryDlg = NULL;
	mpTipFrame = NULL;
	mpPdfPicture = NULL;
	mpJumpPage = NULL;
	mpIterBackground = NULL;
	mpThumbnailDlg = NULL;
	mpPreNextButton = NULL;
	mpSnapShot = NULL;
	mpIterToast = NULL;
	mlHoldInput = 0;
	mxLastGcUiNumber = 0;
	muLastGcTick = 0;
	mbGcMode = false;
	mpToolbarH = NULL;
	mbIsTxt = false;
	mpLoadingView = NULL;
	mbIsLoadingSuccess = false;
	mpHighlight = NULL;
	mbIsScreenAuto = true;

	promptDlg = nullptr;
	pdfOverwriteDlg = nullptr;
	convertProgressDlg = nullptr;
	askConvertDlg = nullptr;

	//txt�ֺ�
	mdwFontsizeIndex = 1;
	mdwFontSizeArray[0] = 9;
	mdwFontSizeArray[1] = 12;
	mdwFontSizeArray[2] = 15;
	mdwFontSizeArray[3] = 18;
	mdwFontSizeArray[4] = 21;

	//�ʿ��
	mdwPenWidthIndex = 1;
	mfArrayPenWidthArray[0] = 0.5f;
	mfArrayPenWidthArray[1] = 1.0f;
	mfArrayPenWidthArray[2] = 2.0f;
	mfArrayPenWidthArray[3] = 4.0f;
	mfArrayPenWidthArray[4] = 8.0f;
	mfArrayFwPenWidthArray[0] = 0x1;
	mfArrayFwPenWidthArray[1] = 0x1;
	mfArrayFwPenWidthArray[2] = 0x2;
	mfArrayFwPenWidthArray[3] = 0x4;
	mfArrayFwPenWidthArray[4] = 0x7;
	mfArrayFwFingerWidthArray[0] = 0x1;
	mfArrayFwFingerWidthArray[1] = 0x1;
	mfArrayFwFingerWidthArray[2] = 0x2;
	mfArrayFwFingerWidthArray[3] = 0x4;
	mfArrayFwFingerWidthArray[4] = 0x7;

	mdwPenColorIndex = 0;

	mszTempFile[0] = UNICODE_NULL;
	mszSrcFile[0] = UNICODE_NULL;

	mbIsSetPartial = true;
	mulPageIndex = 0;
	mulPageCount = 0;
	

	mulPenMode = PEN_MODE_NONE;
	mbIsHand = false;
	//mhFile = INVALID_HANDLE_VALUE;
	enableCapacitivepen = true;

	//Sleep(1000 * 20);

	wostringstream pathBuffer;
	const int bufferSize = 2048;
	unique_ptr<wchar_t[]> buffer = make_unique<wchar_t[]>(bufferSize);
	GetTempPathW(bufferSize, buffer.get());
	pathBuffer << buffer.get() << kReaderTempPath << L"\\" << kCommandLineParamFile;
	if (GetFileAttributesW(pathBuffer.str().c_str()) == INVALID_FILE_ATTRIBUTES) return;

	FILE* f = nullptr;
	_wfopen_s(&f, pathBuffer.str().c_str(), L"rt,ccs=utf-8");
	if (f == nullptr) return;
	fgetws(buffer.get(), bufferSize, f);
	if (wcsstr(buffer.get(), L"-open") == buffer.get())
	{
		wstring s = buffer.get();
		size_t i;
		for (i = wcslen(L"-open "); i < s.size() && s[i] == ' '; i++);
		m_cmdLineOpenFileName = s.substr(i);
	}

	fclose(f);
	DeleteFileW(pathBuffer.str().c_str());
}


CReaderBaseFrame::~CReaderBaseFrame(void)
{
	while (mdHistroyPath.Size() > 0)
	{
		delete mdHistroyPath.GetEntry(0);
		mdHistroyPath.RemoveByIndex(0);
	}

	//if (mhFile != INVALID_HANDLE_VALUE)
	//	CloseHandle(mhFile);
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CReaderBaseFrame::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		// ���÷��֧����ק
		mpIterator->ModifyStyles(EITR_STYLE_POPUP);
		

		EinkuiGetSystem()->SetEinkUpdatingCallBack((PXUI_EINKUPDATING_CALLBACK)&EinkUpdating, this);
		EinkuiGetSystem()->EnablePaintboard();
#ifdef SHOW_BCOVER_WINDOW
		EinkuiGetSystem()->ShowMainWindow(true);
#endif

		EinkuiGetSystem()->CaptureWindowsMessage(WM_EI_ACTIVATE, this, (PWINMSG_CALLBACK)&CReaderBaseFrame::OnActivity);
		EinkuiGetSystem()->CaptureWindowsMessage(WM_EI_HOMEBAR_MODE_CHANGE, this, (PWINMSG_CALLBACK)&CReaderBaseFrame::OnHomebarChanged);
		EinkuiGetSystem()->CaptureWindowsMessage(WM_EI_LAPTOP_MODE_CHANGE, this, (PWINMSG_CALLBACK)&CReaderBaseFrame::OnModeChanged);
		EinkuiGetSystem()->CaptureWindowsMessage(WM_EI_RESET_TP_AREA, this, (PWINMSG_CALLBACK)&CReaderBaseFrame::OnNotifyResetTpArea);
		// Register listener to Open Office Docs with 'TimeStamp' method [zhuhl5@20200121]
		//EinkuiGetSystem()->CaptureWindowsMessage(WM_EI_READER_CONVERT_OFFICE_FILE_WITHTS, this, (PWINMSG_CALLBACK)&CReaderBaseFrame::OnOpenOfficeFileWithTS);
		EinkuiGetSystem()->CaptureWindowsMessage(WM_POWERBROADCAST, this, (PWINMSG_CALLBACK)&CReaderBaseFrame::OnPowerChangeMsg);
		
		/*RAWINPUTDEVICE Rid = {};

		Rid.usUsagePage = 0x01;
		Rid.usUsage = 0x06;
		Rid.dwFlags = RIDEV_INPUTSINK;
		Rid.hwndTarget = EinkuiGetSystem()->GetMainWindow();

		auto regResult = RegisterRawInputDevices(&Rid, 1, sizeof(RAWINPUTDEVICE));
		if (!regResult)
		{

		}
		EinkuiGetSystem()->CaptureWindowsMessage(WM_INPUT, this, (PWINMSG_CALLBACK)&CReaderBaseFrame::OnInputChangeMsg);*/
		//mpIterator->SetTimer(EINK_SETTINGS_TIMER_ID_PAINT, MAXULONG32, 1000, NULL);
		//ImmDisableIME(-1);	//�ر����뷨

		//mpIterator->SetTimer(1,MAXULONG32,3000,NULL);

		EiSetWaveformMode(GI_WAVEFORM_DU2);
		EiCleanupScreen(0xff);
		Sleep(585); //DU260+15*5   ��16֡ˢ��ÿ֡���5ms
		EiSetWaveformMode(GI_WAVEFORM_GC16);

		ShowToolBar(false);
		mpIterator->SetTimer(RBF_TIMER_ENABL_PARTIAL, 1, 1800, NULL);
		//mpIterator->SetTimer(RBF_TIMER_INIT, 1, 100, NULL);
		Init();

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

ULONG CReaderBaseFrame::InitOnCreate(
	IN IEinkuiIterator* npParent,	// ������ָ��
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;
	ICfKey* lpSubKey = NULL;

	do 
	{
		//���ȵ��û���
		leResult = 	CXuiElement::InitOnCreate(npParent,npTemplete,nuEID);
		if(leResult != ERESULT_SUCCESS)
			break;

		EI_SYSTEM_INFO ldSysInfo;
		EiGetSystemInfo(&ldSysInfo);
		EiSetScreenOrient(ldSysInfo.ulOrient);
		

		//EI_SYSTEM_INFO ldInfo;
		//EiGetSystemInfo(&ldInfo);
		EiSetHandWritingMode(GIHW_OWNER_DRAW);
		

		//EiGetSystemInfo(&ldInfo);

		mpIterToast = mpIterator->GetSubElementByID(2);
		BREAK_ON_NULL(mpIterToast);
		mpIterToast->SetVisible(false);
		mpIterToast->SetDefaultZOrder(900000);
		
		mpIterBackground = mpIterator->GetSubElementByID(1);
		BREAK_ON_NULL(mpIterBackground);

		//��ҳ
		lpSubKey = mpTemplete->OpenKey(L"PreNextButton");
		mpPreNextButton = CPreNextButton::CreateInstance(mpIterator, lpSubKey);
		CMM_SAFE_RELEASE(lpSubKey);
		BREAK_ON_NULL(mpPreNextButton);

		//����ѡ�����
		lpSubKey = mpTemplete->OpenKey(L"Highlight");
		mpHighlight = CHighlight::CreateInstance(mpIterator, lpSubKey);
		CMM_SAFE_RELEASE(lpSubKey);
		BREAK_ON_NULL(mpHighlight);
		mpHighlight->GetIterator()->SetVisible(false);

		//PDFͼƬ
		lpSubKey = mpTemplete->OpenKey(L"PicturePreview");
		mpPdfPicture = CPdfPicture::CreateInstance(mpIterator, lpSubKey);
		CMM_SAFE_RELEASE(lpSubKey);
		BREAK_ON_NULL(mpPdfPicture);
		mpPreNextButton->SetPdfPicture(mpPdfPicture,mpHighlight);
		mpHighlight->SetPdfPicture(mpPdfPicture);

		//����������
		lpSubKey = mpTemplete->OpenKey(L"ToolbarH");
		mpToolbarH = CToolbarH::CreateInstance(mpIterator, lpSubKey);
		CMM_SAFE_RELEASE(lpSubKey);
		BREAK_ON_NULL(mpToolbarH);
		

		//�ײ�������
		lpSubKey = mpTemplete->OpenKey(L"ToolbarBottom");
		mpToolbarBottom = CToolbarBottom::CreateInstance(mpIterator, lpSubKey);
		CMM_SAFE_RELEASE(lpSubKey);
		BREAK_ON_NULL(mpToolbarBottom);
		mpToolbarBottom->GetIterator()->BringToTop();

		

		OnRotated(ldSysInfo.ulOrient);

		leResult = ERESULT_SUCCESS;


	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}


//����ϵͳ��Ϣ
ERESULT __stdcall CReaderBaseFrame::OnPowerChangeMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& Result)
{
	do
	{
		if (wParam == PBT_POWERSETTINGCHANGE)
		{
			POWERBROADCAST_SETTING* lpSetting = (POWERBROADCAST_SETTING*)lParam;
			if (lpSetting->PowerSetting == GUID_CONSOLE_DISPLAY_STATE)
			{
				DWORD ldwStatus = 0;
				memcpy_s(&ldwStatus, sizeof(DWORD), lpSetting->Data, sizeof(DWORD));
				/*if (ldwStatus == 0x1)
				{
					EiSetPartialUpdate(FALSE);
					EinkuiGetSystem()->ClearEinkBuffer();
					EinkuiGetSystem()->UpdateView(true);
				}*/
				
			}
		}
		else if (wParam == PBT_APMRESUMEAUTOMATIC)
		{
			OutputDebugString(L"PBT_APMRESUMEAUTOMATIC a");
			//ExitProcess(0);
		}
		else if (wParam == PBT_APMSUSPEND)
		{
			OutputDebugString(L"PBT_APMSUSPEND ");
		}



	} while (false);

	return ERESULT_WINMSG_SENDTO_NEXT;
}

ERESULT __stdcall CReaderBaseFrame::OnInputChangeMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT & Result)
{
	/*do
	{
		BYTE buffer[0x1000]{};
		HRAWINPUT hRawInput = (HRAWINPUT)lParam;
		PRAWINPUT pri = (PRAWINPUT)buffer;
		UINT uint_size = 0x1000;
		uint_size = GetRawInputData(hRawInput, RID_INPUT, pri, &uint_size, sizeof(RAWINPUTHEADER));
		mpPdfPicture->SvcDebugOutFmt("Press Button: key = %d . Flag = %d  . Type = %d .", pri->data.keyboard.VKey, pri->data.keyboard.Flags, pri->header.dwType);
		//LOGW(L"Key = %d Flag = %d", pri->data.keyboard.VKey, pri->data.keyboard.Flags);
		if (pri->header.dwType == RIM_TYPEKEYBOARD)
		{

			if (pri->data.keyboard.VKey == VK_F19)
			{
				//˫����ñ��ť
				if (mpPdfPicture != nullptr && enableCapacitivepen)
				{
					mpPdfPicture->SvcDebugOutFmt("Double press Button.");
					CExMessage::PostMessage(mpIterator, mpIterator, EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_PRE);
				}

			}
			else if (pri->data.keyboard.VKey == VK_F20)
			{
				//������ñ��ť
				if (mpPdfPicture != nullptr && enableCapacitivepen)
				{
					mpPdfPicture->SvcDebugOutFmt("Signle Press Button.");
					CExMessage::PostMessage(mpIterator, mpIterator, EEVT_ER_PRE_NEXT_CLICKED, PNB_BT_NEXT);
				}
			}
		}

	} while (false);*/

	return ERESULT_WINMSG_SENDTO_NEXT;
}

//������̬�ı��֪ͨ
ERESULT __stdcall CReaderBaseFrame::OnModeChanged(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT & Result)
{
	do
	{
		CExMessage::SendMessage(mpIterator, mpIterator, EEVT_HB_MODE_CHANGE, (ULONG)wParam);

	} while (false);

	return ERESULT_WINMSG_SENDTO_NEXT;
}

ERESULT __stdcall CReaderBaseFrame::OnOpenOfficeFileWithTS(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT & Result)
{
	//ͨ���Ҽ���office�ĵ�
	CExMessage::PostMessage(mpIterator, mpIterator, EEVT_CONVERT_OPEN_OFFICE_FILE, static_cast<int64_t>(wParam));
	return ERESULT_SUCCESS;
}

void CReaderBaseFrame::InternalOpenOfficeFile(uint64_t timeStamp)
{
	wostringstream pathBuffer;

	const int bufferSize = 2048;
	unique_ptr<wchar_t[]> buffer = make_unique<wchar_t[]>(bufferSize);
	GetTempPathW(bufferSize, buffer.get());
	pathBuffer << buffer.get() << kReaderTempPath << L"\\openfile" << timeStamp;
	auto tempFileName = pathBuffer.str();

	FILE* f = nullptr;
	_wfopen_s(&f, tempFileName.c_str(), L"rt,ccs=utf-8");
	if (f == nullptr) return;
	fgetws(buffer.get(), bufferSize, f);
	fclose(f);

	DeleteFileW(tempFileName.c_str());

	bool convertSuccess, canceled;
	wstring resourceName;

	mpToolbarH->HideMoreMenu();

	if (EInkReaderUtil::IsOfficeFileName(buffer.get()))
	{
		if (showAskDlg())
		{
			//��ʼת��
			std::tie(convertSuccess, resourceName, canceled) = ConvertAndOpenOfficeFile(buffer.get());

			if (!convertSuccess)//δ����ת�� or ת��ʧ��
			{
				if (!canceled)//����ȡ�����µ�ʧ��
					ShowFileOpenFailDialog(resourceName.c_str());
				if (mpPdfPicture != nullptr && mpPdfPicture->GetDoc() == nullptr)
					CExMessage::PostMessage(mpIterator, mpIterator, EEVT_ER_OPEN_FILE, CExMessage::DataInvalid);
			}
		}

	}
	else
	{
		//��һ��Ҫ�򿪵��ļ��ǲ�������ʷ��¼��
		bool lbRet = false;
		wchar_t lpszFilePath[2048] = { 0 };
		wcscpy_s(lpszFilePath, 2047, buffer.get());
		CharLowerW(lpszFilePath);
		HISTORY_FILE_ATTRIB* lpFileAttrib = NULL;
		for (int i = 0; i < mdHistroyPath.Size(); i++)
		{
			if (_wcsicmp(lpszFilePath, mdHistroyPath.GetEntry(i)->FilePath) == 0)
			{
				lpFileAttrib = mdHistroyPath.GetEntry(i);
				break;
			}
		}
		lbRet = OpenFile(lpszFilePath, lpFileAttrib);
	}
}

//����֪ͨӦ���л���ǰ̨���̨
ERESULT __stdcall CReaderBaseFrame::OnActivity(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& Result)
{
	do
	{
		CExMessage::SendMessage(mpIterator, mpIterator, EEVT_HB_ACTIVITE, (ULONG)wParam);

	} while (false);

	return ERESULT_SUCCESS;
}

//homebar״̬�仯֪ͨ
ERESULT __stdcall CReaderBaseFrame::OnHomebarChanged(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& Result)
{
	do
	{
		CExMessage::SendMessage(mpIterator, mpIterator, EEVT_ER_HOMEBAR_CHANGED, (ULONG)wParam);

	} while (false);

	return ERESULT_WINMSG_SENDTO_NEXT;
}

//Tp��reset�ˣ���Ҫ��������tp area
ERESULT __stdcall CReaderBaseFrame::OnNotifyResetTpArea(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& Result)
{
	do
	{
		SET_TP_AREA ldArea;
		ldArea.Rect.x = 0;
		ldArea.Rect.w = 1920;
		ldArea.Rect.y = 0;
		ldArea.Rect.h = 1080;
		ldArea.Index = 0;
		ldArea.Flag = 0;
		EiSetTpArea(ldArea);

		ldArea.Flag = SET_SP_AREA_TOUCH_PEN;
		EiSetTpArea(ldArea);

	} while (false);

	return ERESULT_WINMSG_SENDTO_NEXT;
}

//��ť�����¼�
ERESULT CReaderBaseFrame::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;


	return lResult;
}


void CReaderBaseFrame::HoldInput(bool nbSet)
{
	if (nbSet != false)
	{
		EinkuiGetSystem()->GetElementManager()->CleanHumanInput(true);
		InterlockedExchange(&mlHoldInput, 1);
		mpIterator->SetTimer(RBF_TIMER_HOLDINPUT, 1, 2000, NULL);	// ���û��ˢ�£�2���Ҳ���Զ����
	}
	else
	{
		if (InterlockedExchange(&mlHoldInput, 0) == 1) // ֻ�е�ǰ��������״̬���ŵ��ù��������������
			EinkuiGetSystem()->GetElementManager()->CleanHumanInput(false);
	}
}

//�������ض�ʱ��
void CReaderBaseFrame::ResetHideTime(bool lbIsOpen)
{
	//mpIterator->KillTimer(RBF_TIMER_TSHOW_TOOLBAR);
	//if(lbIsOpen != false)
	//	mpIterator->SetTimer(RBF_TIMER_TSHOW_TOOLBAR, 1, 5000, NULL);
}

//��ʾ�ļ���ҳ��
void CReaderBaseFrame::ShowFileOpenDlg(void)
{
	if (mpFileHistoryDlg != NULL)
	{
		mpFileHistoryDlg->ExitModal();
		mpFileHistoryDlg = NULL;
	}

	if (mpHighlight != NULL)
		mpHighlight->HideSelect();

	ICfKey* lpSubKey = mpTemplete->OpenKey(L"OpenFileHistory");
	mpFileHistoryDlg = CFileHistoryDlg::CreateInstance(mpIterator, lpSubKey);;
	CheckHistoryList();
	mpFileHistoryDlg->SetHistoryList(&mdHistroyPath);
	mpFileHistoryDlg->GetIterator()->SetSize(mpIterator->GetSize());

	//EiSetPartialUpdate(TRUE);
	//�����ǰû�д��ļ����ļ��򿪶Ի����ȡ����ť���û�
	bool lbEnable = true;
	if (mszSrcFile[0] == UNICODE_NULL || GetFileAttributes(mszSrcFile) == INVALID_FILE_ATTRIBUTES)
		lbEnable = false;

	mpPreNextButton->SetPenMode(PEN_MODE_NONE);
	wstring selectedFile;
	HISTORY_FILE_ATTRIB* selectedHistoryItem = nullptr;
	std::tie(selectedHistoryItem, selectedFile) = mpFileHistoryDlg->DoModal(lbEnable);
	mpPreNextButton->SetPenMode(mulPenMode);

	if (selectedFile.size() > 0 || selectedHistoryItem != nullptr)
	{
		const wstring& fileName = selectedFile.size() > 0 ? selectedFile : selectedHistoryItem->FilePath;
		// �򿪵��������Ѿ��򿪵��ļ���ʲô�������ˡ�
		//if (mdHistroyPath.Size() > 0 && _wcsicmp(fileName.c_str(), mdHistroyPath[0]->FilePath) == 0)
		//	return;

		if (selectedHistoryItem != nullptr)
		{
			//���б����office�ĵ�����Ҫת��
			if (EInkReaderUtil::IsOfficeFileName(selectedHistoryItem->FilePath))
			{
				if (showAskDlg())
				{
					bool convertResult, canceled;
					wstring resourceName;
					std::tie(convertResult, resourceName, canceled) = ConvertAndOpenOfficeFile(selectedHistoryItem->FilePath);
					if (!convertResult)
					{
						if (!canceled)
							ShowFileOpenFailDialog(resourceName.c_str());
						if (mpPdfPicture != nullptr && mpPdfPicture->GetDoc() == nullptr)
							CExMessage::PostMessage(mpIterator, mpIterator, EEVT_ER_OPEN_FILE, CExMessage::DataInvalid);
					}
				}
			}
			else
			{
				this->OpenFile(selectedHistoryItem->FilePath, selectedHistoryItem);
			}
		}
		else
		{
			const int kBufferSize = 2048;
			unique_ptr<wchar_t[]> buffer = make_unique<wchar_t[]>(kBufferSize);
			wcscpy_s(buffer.get(), 2048, fileName.c_str());
			if (EInkReaderUtil::IsOfficeFileName(fileName))
			{
				if (showAskDlg())
				{
					if (mpFileHistoryDlg != NULL)
					{
						mpFileHistoryDlg->ExitModal();
						CMM_SAFE_RELEASE(lpSubKey);
						mpFileHistoryDlg = NULL;
					}

					bool convertResult, canceled;
					wstring resourceName;
					std::tie(convertResult, resourceName, canceled) = ConvertAndOpenOfficeFile(fileName.c_str());
					if (!convertResult)
					{
						if (!canceled)
							ShowFileOpenFailDialog(resourceName.c_str());
						if (mpPdfPicture != nullptr && mpPdfPicture->GetDoc() == nullptr)
							CExMessage::PostMessage(mpIterator, mpIterator, EEVT_ER_OPEN_FILE, CExMessage::DataInvalid);
					}
				}
			}
			else
			{
				this->OpenFile(buffer.get(), nullptr);
			}
		}
	}

	//EiSetPartialUpdate(FALSE);
	CMM_SAFE_RELEASE(lpSubKey);
	mpFileHistoryDlg = NULL;

	/*
	if (mpPdfPicture->GetPageCount() == 0)
	{
		//���´��ļ�
		if(mbIsTxt == false || GetFileSize(mszSrcFile)>0)
			Init();
	}
	*/
}

//��Ϣ������
ERESULT CReaderBaseFrame::ParseMessage(IEinkuiMessage* npMsg)
{
	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	switch (npMsg->GetMessageID())
	{
	case EMSG_QUERY_SWGT_ROTATED:
	{
		ULONG luOrient = 0;
		luResult = CExMessage::GetInputData(npMsg, luOrient);
		if (luResult != ERESULT_SUCCESS || npMsg->GetOutputBufferSize() < sizeof(ULONG))
			break;

		if (mbIsScreenAuto == false)
		{
			if (lockvertical == true)
			{
				luOrient = MAXULONG32; //��Ļ����
			}
			else if (luOrient == GIR_90 || luOrient == GIR_270)
			{
				//TBS ����180�ȣ����򿪸�ʱreader���򲻶�
				luOrient = MAXULONG32; //��Ļ����
			}
		}

		*(ULONG*)npMsg->GetOutputBuffer() = luOrient;

		npMsg->SetOutputDataSize(sizeof(ULONG));

		luResult = ERESULT_SUCCESS;
		break;
	}
	break;
	case EMSG_SWGT_ROTATED:
	{
		ULONG luOrient = 0;
		luResult = CExMessage::GetInputData(npMsg, luOrient);
		if (luResult != ERESULT_SUCCESS)
			break;

		OnRotated(luOrient);

		luResult = ERESULT_SUCCESS;
		break;
	}
	case EEVT_ER_OPEN_FILE:
	{
		//ѡ���ļ�
		ShowFileOpenDlg();
	
		break;
	}
	case EEVT_ENTER_THUMBNAIL_DLG:
	{
		//��������ͼ����
		if(mpHighlight != NULL)
			mpHighlight->HideSelect();

		if (mpPdfPicture->GetDocType() == DOC_TYPE_PDF)
		{
			mpPdfPicture->RefreshThumbnail();
			mpPdfPicture->LoadThumbnails(mszSrcFile, false); //PDF��ʽ��Ҫ��ˢ��һ�£������б仯
			Sleep(500);
		}
		

		if (mpThumbnailDlg != NULL)
		{
			mpThumbnailDlg->ExitModal();
			mpThumbnailDlg = NULL;
		}

		ICfKey* lpSubKey = mpTemplete->OpenKey(L"Thumbnail");
		mpThumbnailDlg = CThumbnailDlg::CreateInstance(mpIterator, lpSubKey);
		mpThumbnailDlg->SetPdfPicture(mpPdfPicture);

		mpThumbnailDlg->GetIterator()->SetSize(mpIterator->GetSize());
		mpThumbnailDlg->SetDoctype(mpPdfPicture->GetDocType());

		mpPreNextButton->SetPenMode(PEN_MODE_NONE);
		mpThumbnailDlg->DoModal();
		mpPreNextButton->SetPenMode(mulPenMode);

		//EiSetPartialUpdate(FALSE);
		CMM_SAFE_RELEASE(lpSubKey);
		mpThumbnailDlg = NULL;

		break;
	}
	case EEVT_ER_OPEN_PAGE_JUMP:
	{
		//ҳ����ת
		ICfKey* lpSubKey = mpTemplete->OpenKey(L"JumpPage");
		mpJumpPage = CJumpPage::CreateInstance(mpIterator, lpSubKey);
		ULONG secondPage;
		mpJumpPage->SetCurrentPage(mpPdfPicture->GetCurrentPageNumber(secondPage), mpPdfPicture->GetPageCount());
		SetJumpPagePos();

		//EiSetPartialUpdate(TRUE);
		mpPreNextButton->SetPenMode(PEN_MODE_NONE);
		mpJumpPage->DoModal();
		mpPreNextButton->SetPenMode(mulPenMode);
		
		//EiSetPartialUpdate(FALSE);
		CMM_SAFE_RELEASE(lpSubKey);
		mpJumpPage = NULL;
		break;
	}
	case EEVT_ER_SHOW_TIP:
	{
		ICfKey* lpSubKey = mpTemplete->OpenKey(L"TipFrame");
		mpTipFrame = CTipFrame::CreateInstance(mpIterator, lpSubKey);;
		mpTipFrame->GetIterator()->SetSize(mpIterator->GetSize());
		//EiSetPartialUpdate(TRUE);
		
		mpPreNextButton->SetPenMode(PEN_MODE_NONE);
		mpTipFrame->DoModal();
		mpPreNextButton->SetPenMode(mulPenMode);
		//EiSetPartialUpdate(FALSE);
		CMM_SAFE_RELEASE(lpSubKey);
		mpTipFrame = NULL;

		//mpTipFrame->GetIterator()->SetVisible(false);
		break;
	}
	case EEVT_HB_ACTIVITE:
	{
		//Ӧ���л���ǰ̨���̨
		ULONG lulActivity = 0;
		luResult = CExMessage::GetInputData(npMsg, lulActivity);
		if (luResult != ERESULT_SUCCESS)
			break;

		if (lulActivity == 1)
		{
			//�л���ǰ̨
			//mpIterator->KillTimer(RBF_TIMER_EXIT);
			if (mpFileHistoryDlg == NULL && mpThumbnailDlg == NULL && mpSnapShot == NULL)
			{
				ShowToolBar(true); //ֻ���������������ʾ
			}
			if (mpFileHistoryDlg != NULL || mpThumbnailDlg != NULL)
			{
				//��ѡ���ĵ�ҳ��Ҫ���ص��
				EiSetBatteryStatus(GI_BATTERY_HIDE);
			}
			else
			{
				EiSetHomebarStatus(GI_HOMEBAR_SHOW);
				EiSetBatteryStatus(GI_BATTERY_SHOW);
			}

			EiSetWaveformMode(GI_WAVEFORM_DU2);
			EiCleanupScreen(0xff);
			Sleep(585); //DU260+15*5   ��16֡ˢ��ÿ֡���5ms
			EiSetWaveformMode(GI_WAVEFORM_GC16);
			EiSetPartialUpdate(FALSE);

			mpIterator->SetTimer(RBF_TIMER_ENABL_PARTIAL, 1, 1800, NULL);
			//mpIterator->SetTimer(RBF_TIMER_TSHOW_TOOLBAR, 1, 5000, NULL);

			//mpToolbarH->SetBCoverState();//�ָ�B����ʾ״̬

			//����ղŴ򿪵��ļ���ɾ���ˣ��ͽ�����ļ��Ի���
			if (mszSrcFile[0] != UNICODE_NULL && GetFileAttributes(mszSrcFile) == INVALID_FILE_ATTRIBUTES)
			{
				mpPdfPicture->CloseFile();
				if(mpFileHistoryDlg == NULL) //ֻ����û�е���������²Ŵ�
					CExMessage::PostMessage(mpIterator, mpIterator, EEVT_ER_OPEN_FILE, CExMessage::DataInvalid);
			}
			else if (mszSrcFile[0] != UNICODE_NULL)
			{
				//mhFile = CreateFile(mszSrcFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
			}

			enableCapacitivepen = true;
			EinkuiGetSystem()->ClearEinkBuffer();
			EinkuiGetSystem()->UpdateView(true);
		}
		else
		{
			//�л�����̨
			//��������
			//ExitProcess(0); //????��������ǲ��Դ��룬��ʽ����ʱ��Ҫɾ��

			/*if (mhFile != INVALID_HANDLE_VALUE)
				CloseHandle(mhFile);*/

			mpIterator->KillTimer(RBF_TIMER_TSHOW_TOOLBAR);
			//mpToolbarH->GetIterator()->SetVisible(true);
			//�����Ϳ������л�������һ֡ʱ�ر�partialˢ��һ��
			enableCapacitivepen = false;
			
			//mpIterator->SetTimer(RBF_TIMER_EXIT, 1, 400, NULL);
			//ExitProcess(0);
		}

		break;
	}
	case EEVT_HB_MODE_CHANGE:
	{
		//������̬�ı�
		// GIR_MODE_LAPTOP  2
        // GIR_MODE_TENT 3
        // GIR_MODE_TABLET 4
		ULONG lumodestatus = 0;
		luResult = CExMessage::GetInputData(npMsg, lumodestatus);
		if (luResult != ERESULT_SUCCESS)
			break;

		if (lumodestatus == 2)
		{
			CHellFun::SetRegData(RbF_REG_B_COVER, 1);
		}
		else
		{
			CHellFun::SetRegData(RbF_REG_B_COVER, 0);
		}
		//mpToolbarH->UpdateBCoverState();
		break;
	}
	case EEVT_ER_HOMEBAR_CHANGED:
	{
		//homebar״̬�����仯
		ULONG lulHomebarMode = 0;
		luResult = CExMessage::GetInputData(npMsg, lulHomebarMode);
		if (luResult != ERESULT_SUCCESS)
			break;

		//EiSetPartialUpdate(TRUE);
		if (lulHomebarMode == GI_HOMEBAR_EXPAND)
		{
			//homebarչ���������Լ��Ĺ�����
			mpToolbarH->GetIterator()->SetVisible(false);
			mpToolbarBottom->ShowItem(false);
		}
		else if(lulHomebarMode == GI_HOMEBAR_COLLAPSE)
		{
			//homebar��������ʾ�Լ��Ĺ�����
			if (mpFileHistoryDlg == NULL)
			{
				//���ļ��򿪴��ڲ���ʾ
				mpToolbarH->GetIterator()->SetVisible(true);
				mpToolbarBottom->ShowItem(true);
			}
			
			ResetHideTime();
		}
		else if (lulHomebarMode == GI_HOMEBAR_HIDE)
		{
			//������
		}
		//EiSetPartialUpdate(FALSE);

		break;
	}
	case EEVT_ER_PAGE_JUMP:
	{
		//ҳ����ת
		if (mszSrcFile[0] == UNICODE_NULL)
			break;
		int liPage = 1;
		luResult = CExMessage::GetInputData(npMsg, liPage);
		if (luResult != ERESULT_SUCCESS)
			break;

		if (mZoomStatus == ZoomStatus::AUTO_ZOOM)
		{
			mpPdfPicture->GoToPage(liPage, false);
			PageAutoZoom();
		}
		else
		{
			mpPdfPicture->GoToPage(liPage, true);
		}
		ShowPageInfo();

		RECT ldRect;
		mpPdfPicture->CalcMovalbe(ldRect);
		mpToolbarBottom->ShowMoveButton(ldRect);
		//GotoPage(liPage);

		break;
	}
	case EEVT_ER_OPEN_FILE_PATH:
	{
		//Ҫ�򿪵��ļ�·��
		wchar_t* lpszFilePath = (wchar_t*)npMsg->GetInputData();

		bool lbRet = false;
		if (lpszFilePath != NULL)
		{
			//��һ��Ҫ�򿪵��ļ��ǲ�������ʷ��¼��
			HISTORY_FILE_ATTRIB* lpFileAttrib = NULL;
			for (int i = 0; i < mdHistroyPath.Size(); i++)
			{
				if (_wcsicmp(lpszFilePath, mdHistroyPath.GetEntry(i)->FilePath) == 0)
				{
					lpFileAttrib = mdHistroyPath.GetEntry(i);
					break;
				}
			}
			lbRet = OpenFile(lpszFilePath, lpFileAttrib);
		}
		

		// �����������
		bool* lpOut = (bool*)npMsg->GetOutputBuffer();
		*lpOut = lbRet;
		npMsg->SetOutputDataSize(sizeof(bool));

		break;
	}
	case EEVT_ER_OPEN_HISTORY_FILE_PATH:
	{
		//Ҫ����ʷ��¼�е��ļ�
		HISTORY_FILE_ATTRIB* lpFileAttrib = NULL;
		luResult = CExMessage::GetInputData(npMsg, lpFileAttrib);
		if (luResult != ERESULT_SUCCESS)
			break;

		bool lbRet = OpenFile(lpFileAttrib->FilePath, lpFileAttrib);

		// �����������
		bool* lpOut = (bool*)npMsg->GetOutputBuffer();
		*lpOut = lbRet;
		npMsg->SetOutputDataSize(sizeof(bool));

		break;
	}

	case EEVT_ER_PRE_NEXT_CLICKED:
	{
		//ҳ����ת
		ULONG lulFlag = 1;
		luResult = CExMessage::GetInputData(npMsg, lulFlag);
		if (luResult != ERESULT_SUCCESS)
			break;
		do 
		{
			if (mszSrcFile[0] == UNICODE_NULL)
				break;
			if (lulFlag == PNB_BT_PRE)
			{
				if (mZoomStatus == ZoomStatus::AUTO_ZOOM)
				{
					mpPdfPicture->PageFoward(false, false);
					PageAutoZoom();
				}
				else
				{
					mpPdfPicture->PageFoward(false, true);
				}
				ShowPageInfo();
				mpHighlight->HideSelect();
			}
			else if (lulFlag == PNB_BT_NEXT)
			{
				if (mZoomStatus == ZoomStatus::AUTO_ZOOM)
				{
					mpPdfPicture->PageFoward(true, false);
					PageAutoZoom();
				}
				else
				{
					mpPdfPicture->PageFoward(true, true);
				}
				ShowPageInfo();
				mpHighlight->HideSelect();
			}
			else if (lulFlag == PNB_BT_MIDDLE)
			{
				//��ʾ�����ع�����
				ShowToolBar(!mpToolbarH->GetIterator()->IsVisible());
			}
			else if (lulFlag == PNB_BT_MIDDLE_ZOOM)
			{
				//���ط�ҳ������
				mpToolbarBottom->HidePageProcess(false);
			}
			else
				break;

			if (lulFlag == PNB_BT_PRE || lulFlag == PNB_BT_NEXT)
			{
				RECT ldRect;
				mpPdfPicture->CalcMovalbe(ldRect);
				mpToolbarBottom->ShowMoveButton(ldRect);
			}

			HoldInput(true);

		} while (false);

		//???niu ���Ӷ�PageFoward()ʧ�ܵ��жϺʹ���

		break;
	}
	case EEVT_ER_TWO_SCREEN:
	{
		//�л�˫������
		bool lbIsDoubleScreen = mpToolbarH->GetDuopageStatus();

		mpPdfPicture->EnableDuopageView(lbIsDoubleScreen);
		ShowPageInfo();

		CHellFun::SetRegData(RbF_REG_DOUBLE_SCREEN, lbIsDoubleScreen==false?0:1);

		ResetHideTime();

		HoldInput(true);

		RECT ldRect;
		mpPdfPicture->CalcMovalbe(ldRect);
		mpToolbarBottom->ShowMoveButton(ldRect);
		mpToolbarBottom->EnableAutoZoomButton(!lbIsDoubleScreen);

		mpHighlight->HideSelect();

		break;
	}
	case EEVT_CLOSE_B_SCREEN:
	{
		//bool BScreenStatus = mpToolbarH->GetBCoverState();


		//CHellFun::SetRegData(RbF_REG_B_COVER, BScreenStatus == false ? 0 : 1);

		//BScreenStatus = !BScreenStatus;

		//EiCloseBCover(BScreenStatus);

		//break;
	}
	case EEVT_ER_ENTER_ZOOM:
	{
		//��������ģʽ
		bool lbIsZoom = false;
		luResult = CExMessage::GetInputData(npMsg, lbIsZoom);
		if (luResult != ERESULT_SUCCESS)
			break;

		mZoomStatus = ZoomStatus::ZOOM;
		mdHistroyPath.GetEntry(0)->autoZoom = 0; 
		wchar_t lszText[MAX_PATH];
		swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_AUTO_ZOOM, 0);
		CHellFun::SetRegData(lszText, 0);
		mpPreNextButton->SetZoomStatus(lbIsZoom ? ZoomStatus::ZOOM : ZoomStatus::NONE);
		if (lbIsZoom && !mbIsTxt)
			mpToolbarBottom->SetFatRatio(mpPdfPicture->GetFatRatio());

		if (!lbIsZoom)
		{
			mdHistroyPath.GetEntry(0)->scalingLevel = 0;
			mpToolbarBottom->ShowMoveButton(RECT{ 0,0,0,0 });
			swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_SCALINGLEVEL, 0);
			CHellFun::SetRegData(lszText, 0);
		}
		
		break;
	}
	case EEVT_ER_SET_PAGE_MOVE:
	{
		//ҳ���ƶ�
		POINT ldPos;
		luResult = CExMessage::GetInputData(npMsg, ldPos);
		if (luResult != ERESULT_SUCCESS)
			break;

		RECT ldRect;
		mpPdfPicture->MovePage(ldPos.x, ldPos.y, ldRect);
		if (mZoomStatus == ZoomStatus::AUTO_ZOOM)
		{
			if (mMoveForward == MoveForward::HORIZONTAL)
			{
				ldRect.top = 0;
				ldRect.bottom = 0;
			}
			else if (mMoveForward == MoveForward::VERTICAL)
			{
				ldRect.left = 0;
				ldRect.right = 0;
			}
		}
		// �����������
		RECT* lpOut = (RECT*)npMsg->GetOutputBuffer();

// [zhuhl5@20200116:ZoomRecovery, Position recovery, not done.]
// Make judgement if outer caller doesn't want result rect.
		if (lpOut)
		{
			*lpOut = ldRect;
		}
		npMsg->SetOutputDataSize(sizeof(RECT));

		//��ȡ��ʵ��С
		D2D1_SIZE_F ldMaxSize;
		D2D1_RECT_F ldViewRect;
		mpPdfPicture->GetRectOfViewportOnPage(ldMaxSize, ldViewRect);
		mpToolbarBottom->ShowMoveButton(ldRect);

		mpHighlight->Relocation();

// [zhuhl5@20200116:ZoomRecovery]
// update position to history, but this is just offset, should be absolute position.
		mdHistroyPath.GetEntry(0)->scalingPosX = (float)ldPos.x;
		mdHistroyPath.GetEntry(0)->scalingPosY = (float)ldPos.y;
		break;
	}
	case EEVT_ER_SET_ZOOM:
	{
		//���÷Ŵ����
		double lfRotio = 1.0;
		luResult = CExMessage::GetInputData(npMsg, lfRotio);
		if (luResult != ERESULT_SUCCESS)
			break;

		mZoomStatus = ZoomStatus::ZOOM;
		mdHistroyPath.GetEntry(0)->autoZoom = 0;
		mpPreNextButton->SetZoomStatus(ZoomStatus::ZOOM);

		mpToolbarBottom->SetMoveForward(MoveForward::HORIZONTAL_VERTICAL);
		mpPreNextButton->SetMoveForward(MoveForward::HORIZONTAL_VERTICAL);

		RECT ldRect;
		mpPdfPicture->SetScaleRatio(lfRotio, ldRect);
		CHellFun::SetRegData(RbF_REG_RATIO, DWORD(lfRotio*100));

		mpToolbarBottom->SetRatioString(lfRotio);
		mpToolbarBottom->ShowMoveButton(ldRect);
		
		// �����������
		RECT* lpOut = (RECT*)npMsg->GetOutputBuffer();
		*lpOut = ldRect;
		npMsg->SetOutputDataSize(sizeof(RECT));

		//��ȡ��ʵ��С
		D2D1_SIZE_F ldMaxSize;
		D2D1_RECT_F ldViewRect;
		mpPdfPicture->GetRectOfViewportOnPage(ldMaxSize, ldViewRect);

		SetHandWriteWidth();

		mpHighlight->Relocation();

// [zhuhl5@20200116:ZoomRecovery]
// update scaling ratio to history
		mdHistroyPath.GetEntry(0)->scaling = lfRotio;

		wchar_t lszText[MAX_PATH];
		swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_AUTO_ZOOM, 0);
		CHellFun::SetRegData(lszText, 0);

		break;
	}
// [zhuhl5@20200116:ZoomRecovery]
// Notify current zoom level, outer should record this, and use this to recover zoom status, not specific zoom ratio.
	case EEVT_ER_AUTO_ZOOM:
	{
		if (mszSrcFile[0] == UNICODE_NULL)
			break;
		mZoomStatus = ZoomStatus::AUTO_ZOOM;
		mdHistroyPath.GetEntry(0)->autoZoom = 1;
		mpPreNextButton->SetZoomStatus(ZoomStatus::AUTO_ZOOM);
		PageAutoZoom();
		wchar_t lszText[MAX_PATH];
		swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_AUTO_ZOOM, 0);
		CHellFun::SetRegData(lszText, 1);
		break;
	}
	case EEVT_ER_SET_ZOOMLEVEL:
	{
		int scalingLevel = 0;
		luResult = CExMessage::GetInputData(npMsg, scalingLevel);
		if (luResult != ERESULT_SUCCESS)
			break;
		mdHistroyPath.GetEntry(0)->scalingLevel = scalingLevel;
		wchar_t lszText[MAX_PATH];
		swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_SCALINGLEVEL, 0);
		CHellFun::SetRegData(lszText, scalingLevel);
		break;
	}
	case EEVT_ER_ENTER_SNAPSHOT:
	{
		//����
		ICfKey* lpSubKey = mpTemplete->OpenKey(L"SnapShot");
		mpSnapShot = CSnapShot::CreateInstance(mpIterator, lpSubKey);
		mpSnapShot->SetToolBarHeight(mpToolbarH->GetIterator()->GetSizeY(), mpToolbarBottom->GetIterator()->GetSizeY());
		mpSnapShot->GetIterator()->SetSize(mpIterator->GetSize());
		//EiSetPartialUpdate(TRUE);

		EiSetBatteryStatus(GI_BATTERY_HIDE);
		ShowToolBar(false);
		EinkuiGetSystem()->ClearEinkBuffer();
		mpPreNextButton->SetPenMode(PEN_MODE_NONE);
		mpSnapShot->DoModal();
		mpPreNextButton->SetPenMode(mulPenMode);
		ShowToolBar(true);
		EiSetBatteryStatus(GI_BATTERY_SHOW);
		EinkuiGetSystem()->ClearEinkBuffer();
		//EiSetPartialUpdate(FALSE);
		CMM_SAFE_RELEASE(lpSubKey);
		mpSnapShot = NULL;

		break;
	}
	case EEVT_ER_SNAPSHOT_TO_CLIPBRD:
	{
		D2D1_RECT_F ldRect;
		luResult = CExMessage::GetInputData(npMsg, ldRect);
		if (luResult != ERESULT_SUCCESS)
			break;

		mpPdfPicture->CopyToClipboard(ldRect);
		ShowToast(L"SnapShot");

		break;
	}
	case EEVT_ER_SET_TXT_ZOOM:
	{
		//����txt�Ŵ�
		luResult = CExMessage::GetInputData(npMsg, mdwFontsizeIndex);
		if (luResult != ERESULT_SUCCESS)
			break;

		mpPdfPicture->SetFontSize(mdwFontSizeArray[mdwFontsizeIndex]);
		ShowPageInfo();

		//�����ñ��浽ע�����
		CHellFun::SetRegData(RbF_REG_TXT_FONT_SIZE_INDEX, mdwFontsizeIndex);

		break;
	}
	case EEVT_TXT_ARRANGED_START:
	{
		//txt��ʼҳ�����
		mbIsLoadingSuccess = false;
		
		break;
	}
	case EEVT_TXT_ARRANGED_DOING:
	{
		//txtҳ�����ڼ�����
		ShowPageInfo();
		mbIsSetPartial = false;

		int32 liPageIndex = 0;
		luResult = CExMessage::GetInputData(npMsg, liPageIndex);
		if (luResult != ERESULT_SUCCESS)
			break;

		mpPdfPicture->SetLoadingPageIndex(liPageIndex);

		break;
	}
	case EEVT_ARRANGED_COMPLETE:
	{
		//ҳ��������
		int32 liPagecount = 0;
		luResult = CExMessage::GetInputData(npMsg, liPagecount);
		if (luResult != ERESULT_SUCCESS)
			break;

		mbIsSetPartial = true;
		mbIsLoadingSuccess = true;
		mpToolbarH->DocmentLoadComplete();
		if (liPagecount > 0)
			ShowPageInfo();

		break;
	}
	case EEVT_THUMBNAIL_COMPLETE:
	{
		//����ͼ�������
		mpToolbarH->ThumbnailsLoadComplete();
		if(mpThumbnailDlg != NULL)
			mpThumbnailDlg->SetPdfPicture(mpPdfPicture); //����ˢ��һ��

		break;
	}
	case EEVT_RESET_HIDE_TIME:
	{
		//����������������ʱ��
		bool lbIsClose = false;
		luResult = CExMessage::GetInputData(npMsg, lbIsClose);
		if (luResult != ERESULT_SUCCESS)
			break;

		ResetHideTime(lbIsClose);

		break;
	}
	case EEVT_SHOW_TOOLBAR:
	{
		//��ʾ�����ع�����
		bool lbIsShow = false;
		luResult = CExMessage::GetInputData(npMsg, lbIsShow);
		if (luResult != ERESULT_SUCCESS)
			break;
		ShowToolBar(lbIsShow);

		break;
	}
	case EEVT_DELETE_READ_HISTORY:
	{
		//����Ķ���ʷ
		while (mdHistroyPath.Size() > 0)
		{
			delete mdHistroyPath.GetEntry(0);
			mdHistroyPath.RemoveByIndex(0);
		}

		if (mpFileHistoryDlg != NULL)
			mpFileHistoryDlg->SetHistoryList(&mdHistroyPath);

		//��ռ�¼
		wchar_t lszText[MAX_PATH];
		DWORD ldwLen = 0;
		for (int i = 0; i < RBF_HISTROY_MAX; i++)
		{
			swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_PATH, i);
			CHellFun::SetRegData(lszText, 0, L"");
		}

		ClearThumbnail(NULL);
		//mpToolbarH->SetDoctype(mpPdfPicture->GetDocType());

		break;
	}
	case EEVT_SET_PEN_WIDTH:
	{
		//���ñʿ�
		luResult = CExMessage::GetInputData(npMsg, mdwPenWidthIndex);
		if (luResult != ERESULT_SUCCESS)
			break;

		SetHandWriteWidth();

		mpPdfPicture->SetPenWidth(mfArrayPenWidthArray[mdwPenWidthIndex]);

		CHellFun::SetRegData(RbF_REG_PEN_WIDTH_INDEX, mdwPenWidthIndex);
		
		break;
	}
	case EEVT_SET_PEN_COLOR:
	{
		//���ñ���ɫ
		luResult = CExMessage::GetInputData(npMsg, mdwPenColorIndex);
		if (luResult != ERESULT_SUCCESS)
			break;

		
		mpPdfPicture->SetPenColor(mdwPenColorIndex);
		CHellFun::SetRegData(RbF_REG_PEN_COLOR_INDEX, mdwPenColorIndex);

		break;
	}
	case EEVT_PEN_MODE:
	{
		//���ñ�״̬
		if (npMsg->GetOutputBufferSize() >= sizeof(ULONG))
		{
			//�����������Ҫ�õ�ǰһ��״̬
			ULONG* lpOut = (ULONG*)npMsg->GetOutputBuffer();
			*lpOut = mulPenMode;
			npMsg->SetOutputDataSize(sizeof(ULONG));
		}

		luResult = CExMessage::GetInputData(npMsg, mulPenMode);
		if (luResult != ERESULT_SUCCESS)
			break;
		mpPreNextButton->SetPenMode(mulPenMode);
		mpPdfPicture->SetPenmode(mulPenMode);

		if (mulPenMode == PEN_MODE_SELECT)
		{
			EinkuiGetSystem()->EnablePaintboard(false);

			//mpPreNextButton->GetIterator()->SetVisible(false);
			mpHighlight->GetIterator()->SetVisible(true);
			mpHighlight->GetIterator()->BringToTop();
		}
		else if(mulPenMode == PEN_MODE_PEN || mulPenMode == PEN_MODE_ERASER)
		{
			//mpPreNextButton->GetIterator()->SetVisible(true);
			mpHighlight->GetIterator()->SetVisible(false);

		}

		mpToolbarH->SetPenStatusSelect(mulPenMode);

		break;
	}
	case EEVT_HAND_WRITE_MUTE:
	{
		//�Ƿ����ֻ���,��Ĭ��ʽ
		bool lbIsHand = false;
		luResult = CExMessage::GetInputData(npMsg, lbIsHand);
		if (luResult != ERESULT_SUCCESS)
			break;

		mpPreNextButton->SetHandWrite(lbIsHand);
		SetHandWriteWidth();

		ResetHideTime();
		break;
	}
	case EEVT_HAND_WRITE:
	{
		//�Ƿ����ֻ���
		bool lbIsHand = false;
		luResult = CExMessage::GetInputData(npMsg, lbIsHand);
		if (luResult != ERESULT_SUCCESS)
			break;

		mpPreNextButton->SetHandWrite(lbIsHand);
		SetHandWriteWidth();

		if (lbIsHand != false)
		{
			ShowToast(L"HandOpen");
		}
		else
		{
			ShowToast(L"HandClose");
		}

		
		ResetHideTime();

		break;
	}
	case EEVT_UNDO:
	{
		//undo����
		int liStackSize = mpPdfPicture->RedoUndoProc(false);
		mpHighlight->HideSelect();
		// �����������
		/*int* lpOut = (int*)npMsg->GetOutputBuffer();
		*lpOut = liStackSize;
		npMsg->SetOutputDataSize(sizeof(int));*/

		break;
	}
	case EEVT_REDO:
	{
		//redo����
		int liStackSize = mpPdfPicture->RedoUndoProc(true);
		mpHighlight->HideSelect();
		// �����������
		/*int* lpOut = (int*)npMsg->GetOutputBuffer();
		*lpOut = liStackSize;
		npMsg->SetOutputDataSize(sizeof(int));*/

		break;
	}
	case EEVT_UPDATE_PAGE_STATUS:
	{
		// ֪ͨ����������ҳ��״̬
		PAGE_STATUS ldStatus;
		luResult = CExMessage::GetInputData(npMsg, ldStatus);
		if (luResult != ERESULT_SUCCESS)
			break;

		mpToolbarH->UpdatePageStatus(ldStatus);

		break;
	}
	case EEVT_PAGE_CHANGED:
	{
		//ҳ�뷢���仯
		ShowPageInfo();
		break;
	}
	case EEVT_MENU_ERASER_ALL:
	{
		//�����ҳ���б�ע
		mpPdfPicture->ClearCurrentPageAllInk();

		ShowToast(L"ClearAllInk");

		break;
	}
	case EEVT_CLEAR_FULL_SCREEN:
	{
		//��ȫ��
		EiSetPartialUpdate(FALSE);
		muLastGcTick = 0;
		mxLastGcUiNumber = 49;

		EinkuiGetSystem()->ClearEinkBuffer();
		EinkuiGetSystem()->UpdateView(true);

		EiSetPartialUpdate(TRUE);
		

		break;
	}
	case EEVT_FILE_MODIFY:
	{
		//�ļ����޸�
		if (mdHistroyPath.Size() > 0)
		{
			CHellFun::SetRegData(L"historyModify0", 1);

			mdHistroyPath.GetEntry(0)->IsModify = 1;
		}

		break;
	}
	case EEVT_SET_FW_LINE_RECT:
	{
		//��������
		ED_RECT ldRect;
		luResult = CExMessage::GetInputData(npMsg, ldRect);
		if (luResult != ERESULT_SUCCESS)
			break;

		mpPreNextButton->SetFwLineRect(ldRect);
		break;
	}
	case EEVT_SELECT_HIGHLIGHT:
	{
		//ѡ��ָ�����������
		if(mpPdfPicture == NULL)
			break;

		D2D1_RECT_F ldRect;
		luResult = CExMessage::GetInputData(npMsg, ldRect);
		if (luResult != ERESULT_SUCCESS)
			break;

		SELECT_HIGHLIGHT ldHighltght;
		ZeroMemory(&ldHighltght, sizeof(ldHighltght));
		int liCount = mpPdfPicture->SelectHighlight(ldRect, ldHighltght);
		// �����������
		SELECT_HIGHLIGHT* lpOut = (SELECT_HIGHLIGHT*)npMsg->GetOutputBuffer();
		*lpOut = ldHighltght;
		npMsg->SetOutputDataSize(sizeof(int));

		break;
	}
	case EEVT_GET_SELECT_HIGHLIGHT_INFO:
	{
		// ��ȡѡ��������Ϣ
		SELECT_HIGHLIGHT ldHighltght;
		ZeroMemory(&ldHighltght, sizeof(SELECT_HIGHLIGHT));
		ED_RECTF ldRect;
		mpPdfPicture->GetSelectHighlightInfo(ldHighltght, ldRect);
		// �����������
		SELECT_HIGHLIGHT* lpOut = (SELECT_HIGHLIGHT*)npMsg->GetOutputBuffer();
		*lpOut = ldHighltght;
		npMsg->SetOutputDataSize(sizeof(int));

		break;
	}
	case EEVT_HIGHLIGHT_BT_EVENT:
	{
		//������������Ϣ
		int liEvent = 0;
		luResult = CExMessage::GetInputData(npMsg, liEvent);
		if (luResult != ERESULT_SUCCESS)
			break;
		mpPdfPicture->HighlightEvent(liEvent);
		if (liEvent == HIGHLIGHT_BT_COPY)
			ShowToast(L"CopyText");

		break;
	}
	case EEVT_SET_SCREEN_STATUS:
	{
		//��Ļ����
		ULONG lulScreen = 0;
		luResult = CExMessage::GetInputData(npMsg, lulScreen);
		if (luResult != ERESULT_SUCCESS)
			break;

		DWORD ldwOrient = GIR_NONE;
		mbIsScreenAuto = false;
		lockvertical = false;
		DWORD ldwRegOri = (DWORD)lulScreen;
		if (lulScreen == SCREEN_STATUS_AUTO)
		{
			EI_SYSTEM_INFO ldInfo;
			EiGetSystemInfo(&ldInfo);
			ldwOrient = ldInfo.ulOrient;
			mbIsScreenAuto = true;
		}
		else if (lulScreen == SCREEN_STATUS_H)
		{
			EI_SYSTEM_INFO ldInfo;
			EiGetSystemInfo(&ldInfo);
			ldwOrient = ldInfo.ulOrient;
			if (ldwOrient == GIR_90 || ldwOrient == GIR_270)
			{
				//TBS ����180�ȣ����򿪸�ʱreader���򲻶�
				ldwOrient = GIR_NONE;
			}
		}
		else if (lulScreen == SCREEN_STATUS_V)
		{
			ldwOrient = GIR_90;
			lockvertical = true;
		}

		EiSetScreenOrient(ldwOrient);
		EinkuiGetSystem()->ResetPaintboard();
		OnRotated(ldwOrient);

		CHellFun::SetRegData(RbF_REG_SCREEN_ORI, ldwRegOri);

		break;
	}
	case EEVT_PARTIAL_ENABLE:
	{
		//�򿪻�ر�partail
		bool lbEnable = false;
		luResult = CExMessage::GetInputData(npMsg, lbEnable);
		if (luResult != ERESULT_SUCCESS)
			break;

		if (lbEnable == false)
		{
			EiSetPartialUpdate(FALSE);
			OutputDebugString(L"enter gc false partial");

			muLastGcTick = 0;
			mxLastGcUiNumber = 49;
		}
		else
		{
			EiSetPartialUpdate(TRUE);
		}

		break;
	}
	case EEVT_UPDATE_THUMBNAIL_PATH:
	{
		//��������ͼĿ¼
		wchar_t* lpszPath = (wchar_t*)npMsg->GetInputData();
		if (lpszPath != NULL && lpszPath[0] != UNICODE_NULL && mdHistroyPath.Size() > 0)
		{
			wcscpy_s(mdHistroyPath.GetEntry(0)->ThumbanilFilePath, MAX_PATH, lpszPath);
			CHellFun::SetRegData(L"historyThumbanilPath0", 0, mdHistroyPath.GetEntry(0)->ThumbanilFilePath);
		}

		break;
	}
	case EEVT_CONVERT_OPEN_OFFICE_FILE:
	{
		int64_t timeStamp = *reinterpret_cast<const int64_t*>(npMsg->GetInputData());
		InternalOpenOfficeFile(timeStamp);
		break;
	}
	case EEVT_HIDE_HIGHLIGHT:
	{
		if (mpHighlight != NULL)
			mpHighlight->HideSelect();
		break;
	}
	default:
		luResult = ERESULT_NOT_SET;
		break;
	}

	if (luResult == ERESULT_NOT_SET)
	{
		luResult = CXuiElement::ParseMessage(npMsg); // ���û����ͬ��������ע�⣺һ��Ҫ��������ֱ�ӻ���
	}

	return luResult;
}

//���ñʿ�
void CReaderBaseFrame::SetHandWriteWidth()
{
	do
	{
		BREAK_ON_NULL(mpPdfPicture);

		BYTE ld = BYTE(mfArrayFwFingerWidthArray[mdwPenWidthIndex] * mpPdfPicture->GetRealRatio());
		if (mpPreNextButton->GetHandWrite() == false)
			EiSetHandwritingSetting(BYTE(mfArrayFwPenWidthArray[mdwPenWidthIndex] * mpPdfPicture->GetRealRatio()));
		else
			EiSetHandwritingSetting(BYTE(mfArrayFwFingerWidthArray[mdwPenWidthIndex] * mpPdfPicture->GetRealRatio()));

	} while (false);

}

//����
ERESULT CReaderBaseFrame::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{
	HoldInput(false);

	return CXuiElement::OnPaint(npPaintBoard);
}

////��ҳ
//void CReaderBaseFrame::GotoPage(ULONG nulPage)
//{
//	mpPdfPicture->GoToPage(nulPage);
//
//	wchar_t lszString[MAX_PATH] = { 0 };
//	swprintf_s(lszString, MAX_PATH, L"(��%dҳ/��%dҳ)", nulPage, mpPdfPicture->GetPageCount());
//	mpToolbarH->SetPage(lszString);
//
//	mulCurrentPageNumber = nulPage;
//}

//�����ʷ�ļ�״̬����������ڵ��ļ������
void CReaderBaseFrame::CheckHistoryList(void)
{
	int liIndex = 0;
	while (liIndex < mdHistroyPath.Size())
	{
		HISTORY_FILE_ATTRIB * lpTemp = mdHistroyPath.GetEntry(liIndex);
		if (GetFileAttributes(lpTemp->FilePath) == INVALID_FILE_ATTRIBUTES)
		{
			//�ļ���������
			delete lpTemp;
			mdHistroyPath.RemoveByIndex(liIndex);
		}
		else
		{
			liIndex++;
		}
	}
}

//��ʾ�����ع�����
void CReaderBaseFrame::ShowToolBar(bool nbIsShow)
{
	if (nbIsShow == false)
	{
		//����
		mpToolbarH->GetIterator()->SetVisible(false);
		mpToolbarBottom->ShowItem(false);
		mpIterator->KillTimer(RBF_TIMER_TSHOW_TOOLBAR);

		mpPreNextButton->SetToolbarHeight(0, 0);
		mpHighlight->SetToolbarHeight(0, 0);
	}
	else
	{
		//��ʾ
		mpToolbarH->GetIterator()->SetVisible(true);
		mpToolbarBottom->ShowItem(true);
		
		ResetHideTime();

		//EiSetHomebarStatus(GI_HOMEBAR_COLLAPSE);

		mpPreNextButton->SetToolbarHeight((int)mpToolbarH->GetIterator()->GetSizeY(), (int)mpToolbarBottom->GetIterator()->GetSizeY());
		mpHighlight->SetToolbarHeight((int)mpToolbarH->GetIterator()->GetSizeY(), (int)mpToolbarBottom->GetIterator()->GetSizeY());
	}
}

void CReaderBaseFrame::ShowPageInfo(void)
{
	wchar_t lszString[MAX_PATH] = { 0 };
	ULONG pageNo1=0, pageNo2=0;

	do
	{
		BREAK_ON_NULL(mpPdfPicture);
		if (mpPdfPicture->GetDoc() == NULL)
			break;

		pageNo1 = mpPdfPicture->GetCurrentPageNumber(pageNo2);
		if (pageNo1 == 0)
			pageNo1 = 1;

		//ҳ������
		mpToolbarBottom->SetPage(pageNo2 <= 0 ? pageNo1 : pageNo2, mpPdfPicture->GetPageCount() == 0 ? 1 : mpPdfPicture->GetPageCount());


		//��ǰҳ���ע����
		mpToolbarH->SetCurrentPageInkCount(mpPdfPicture->GetCurrentPageInkCount());

		if (mulPageIndex != pageNo1 || mulPageCount != mpPdfPicture->GetPageCount())
		{
			//���¼�¼����
			mulPageIndex = pageNo1;
			mulPageCount = mpPdfPicture->GetPageCount();
			if (mulPageCount == 0)
				mulPageCount = 1;

			PAGE_PDF_CONTEXT ldContent;
			ZeroMemory(&ldContent, sizeof(ldContent));
			mpPdfPicture->GetCrtPageContext(&ldContent);

			//�Ķ�����
			if (mdHistroyPath.Size() > 0 && mbIsLoadingSuccess != false)
			{
				DWORD ldwProgress = DWORD(floor((float(pageNo1) / mulPageCount) * 100));
				if (ldwProgress <= 0)
					ldwProgress = 1; //������С1
				if (ldwProgress > 100)
					ldwProgress = 100; //�������100

				mdHistroyPath.GetEntry(0)->ReadProgress = ldwProgress;
				CHellFun::SetRegData(L"historyProgress0", ldwProgress);

				time_t lTimeNow;
				time(&lTimeNow);
				CHellFun::SetRegData(L"historyTimestamp0", (DWORD)lTimeNow);

				mdHistroyPath.GetEntry(0)->ReadProgress = ldwProgress;
				mdHistroyPath.GetEntry(0)->TimeStamp = (DWORD)lTimeNow;

				mdHistroyPath.GetEntry(0)->PageContext = ldContent.pageContext;
				CHellFun::SetRegData(L"PageContent0", ldContent.pageContext);

				mdHistroyPath.GetEntry(0)->PageContext2 = ldContent.pageContext2;
				CHellFun::SetRegData(L"PageContent20", ldContent.pageContext2);

				mdHistroyPath.GetEntry(0)->PageNumber = pageNo1;
				CHellFun::SetRegData(L"PageNumber0", pageNo1);
			}


		}

	} while (false);

}

//��ʾtoast
void CReaderBaseFrame::ShowToast(wchar_t* npszKeyName)
{
	IConfigFile* lpProfile = NULL;

	do 
	{
		BREAK_ON_NULL(npszKeyName);

		//��ȡ�������ַ���
		//Ϊ�˷��뷽�㣬�ַ��������root/string
		wchar_t lszText[MAX_PATH] = { 0 };
		lpProfile = EinkuiGetSystem()->GetCurrentWidget()->GetDefaultFactory()->GetTempleteFile();
		ICfKey* lpCfKey = NULL;
		if (lpProfile != NULL)
		{
			swprintf_s(lszText, MAX_PATH, L"String/%s", npszKeyName);
			lpCfKey = lpProfile->OpenKey(lszText);
			if (lpCfKey != NULL)
			{
				lpCfKey->GetValue(lszText, MAX_PATH * sizeof(wchar_t));
				CMM_SAFE_RELEASE(lpCfKey);
				CExMessage::SendMessageWithText(mpIterToast, mpIterator, EACT_LABEL_SET_TEXT, lszText);
				FLOAT lfX = (mpIterator->GetSizeX() - mpIterToast->GetSizeX()) / 2;
				mpIterToast->SetPosition(lfX, mpIterator->GetPositionY() + 100);
				//mpIterToast->SetPosition(lfX, mpIterator->GetSizeY() - 200);

				mpIterToast->SetVisible(true);
				mpIterToast->BringToTop();
				EinkuiGetSystem()->UpdateView(true);
				mpIterator->SetTimer(RBF_TIMER_TOAST, 1, 2000, NULL);
			}
		}
		CMM_SAFE_RELEASE(lpCfKey);

	} while (false);

	CMM_SAFE_RELEASE(lpProfile);
}


// ���ڸ�eink�����̷߳�����Ϣ�����������������߳�
bool CReaderBaseFrame::CopyFileThread(LPVOID npData)
{
	CReaderBaseFrame* lpThis = (CReaderBaseFrame*)npData;
	
	CopyFile(lpThis->mszSrcFile, lpThis->mszTempFile, FALSE);

	return true;
}

bool CReaderBaseFrame::GetAskConvertStatus()
{
	bool result = true;

	HKEY lhKey = NULL;
	DWORD ldwRes = 0;
	DWORD ldwValue = 0;
	DWORD ldwLen = sizeof(DWORD);

	ldwRes = RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Lenovo\\Eink-PdfReader", 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &lhKey);
	if (ldwRes == ERROR_SUCCESS)
	{
		RegQueryValueEx(lhKey, L"AskConvertOrNot", NULL, NULL, (BYTE*)&ldwValue, &ldwLen);
	}

	if (ldwValue == 1)
	{
		//1: ��ѯ��
		result = true;
	}
	else
	{
		//0�� ѯ��
		result = false;
	}
	if (lhKey != NULL)
		RegCloseKey(lhKey);

	return result;
}

bool CReaderBaseFrame::showAskDlg()
{
	if (askConvertDlg != NULL)
	{
		askConvertDlg->ExitModal(PromptDialogAskResult::No);
		askConvertDlg = NULL;
	}

	if (!(GetAskConvertStatus()))
	{

		ICfKey* lpSubKey_1 = mpTemplete->OpenKey(L"AskConvertPrompt");
		askConvertDlg = CAskConvertDlg::CreateInstance(mpIterator, lpSubKey_1);

		PlaceChildInCenter(askConvertDlg->GetIterator());
		//�����˵�ʱ�͹ر�����
		CExMessage::SendMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_PEN_MODE, PEN_MODE_NONE);
		auto AskResult = askConvertDlg->DoModal();
		CMM_SAFE_RELEASE(lpSubKey_1);
		askConvertDlg = NULL;

		switch (AskResult)
		{
		case PromptDialogAskResult::Yes: //����
		{
			return true;
		}
		case PromptDialogAskResult::No: //ȡ��
		{
			CExMessage::SendMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_PEN_MODE, PEN_MODE_PEN);
			return false;
		}
		default:
		{
			CExMessage::SendMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_PEN_MODE, PEN_MODE_PEN);
			return false;
		}
		}
	}

	if (askConvertDlg != NULL)
	{
		askConvertDlg->ExitModal(PromptDialogAskResult::Yes);
		askConvertDlg = NULL;
	}
	return true;
}

//��Ҫ�򿪵��ļ�copy����ʱĿ¼
void CReaderBaseFrame::CopyFileToTemp(IN wchar_t* npszSrc, OUT wchar_t* npszDest, IN LONG nlLen)
{
	do 
	{
		BREAK_ON_NULL(npszSrc);
		BREAK_ON_NULL(npszDest);
		int liLastChar = int(wcslen(npszDest) - 1);
		if (npszDest[0] != UNICODE_NULL && npszDest[liLastChar] != 'f')
		{
			wchar_t lszTemp[MAX_PATH] = { 0 };
			wcscpy_s(lszTemp, MAX_PATH, npszDest);
			lszTemp[wcslen(lszTemp) + 1] = UNICODE_NULL; //˫0��β
			//����Ѵ��ڣ���ɾ��
			SHFILEOPSTRUCT ldShfile;
			memset(&ldShfile, 0, sizeof(ldShfile));
			ldShfile.pFrom = lszTemp;
			ldShfile.fFlags = FOF_NO_UI | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;
			ldShfile.wFunc = FO_DELETE;
			SHFileOperation(&ldShfile);
		}

		liLastChar = int(wcslen(npszSrc) - 1);
		if (npszSrc[liLastChar] == 'f')
		{
			//�����pdf�ļ��Ͳ�copy
			wcscpy_s(npszDest, nlLen, npszSrc);
			break;
		}
		SHGetSpecialFolderPath(NULL, npszDest, CSIDL_COMMON_APPDATA, FALSE);
		wcscat_s(npszDest, nlLen, L"\\EinkSrv\\");
		wcscat_s(npszDest, nlLen, wcsrchr(npszSrc, L'\\') + 1);


		DWORD ldwProtectThreadID = 0;
		HANDLE lhCopyfileThread = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)&CReaderBaseFrame::CopyFileThread,
			this,
			0,
			&ldwProtectThreadID
		);

		if (WaitForSingleObject(lhCopyfileThread, 2000) == WAIT_TIMEOUT)
		{
			ICfKey* lpSubKey = mpTemplete->OpenKey(L"PicturePreview/Loading");
			mpLoadingView = CLoadingView::CreateInstance(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), lpSubKey);
			wchar_t* lpszFileName = wcsrchr(npszSrc, L'\\');
			if (lpszFileName != NULL)
				lpszFileName = lpszFileName + 1;
			mpLoadingView->SetData(lpszFileName);
			mpLoadingView->GetIterator()->SetSize(mpIterator->GetSize());

			mpPreNextButton->SetPenMode(PEN_MODE_NONE);
			mpLoadingView->DoModal(NULL, lhCopyfileThread);
			mpPreNextButton->SetPenMode(mulPenMode);
			
			mpLoadingView = NULL;
			CMM_SAFE_RELEASE(lpSubKey);

			CloseHandle(lhCopyfileThread);
		}

	} while (false);

}

//�û�ѡ����Ҫ�򿪵��ļ�,npFileAttrib!=NULL��ʾ����ʷ��¼�е��ļ�
bool CReaderBaseFrame::OpenFile(wchar_t* npszFilePath, HISTORY_FILE_ATTRIB* npFileAttrib)
{
	bool lbRet = false;

	do 
	{
		BREAK_ON_NULL(npszFilePath);
		mpPdfPicture->CloseFile();
		mbIsLoadingSuccess = false;
		/*if (mhFile != INVALID_HANDLE_VALUE)
			CloseHandle(mhFile);*/

		//while (EInkReaderUtil::IsFileLocked(npszFilePath))
		//{
		//	COpenFileLockedDlg* fileLockedDialog = nullptr;
		//	ICfKey* lpSubKey = mpTemplete->OpenKey(L"ConvertFailFileLocked");
		//	fileLockedDialog = COpenFileLockedDlg::CreateInstance(mpIterator, lpSubKey);
		//	PlaceChildInCenter(fileLockedDialog->GetIterator());
		//	if (!fileLockedDialog->DoModal())
		//	{
		//		fileLockedDialog = NULL;
		//		return false;
		//	}
		//	fileLockedDialog = NULL;
		//}

		wcscpy_s(mszSrcFile, MAX_PATH, npszFilePath);

		CopyFileToTemp(npszFilePath, mszTempFile, MAX_PATH);
		
		mulPageIndex = 0;

		wchar_t* lpszFileName = wcsrchr(npszFilePath, L'\\');
		if (lpszFileName == NULL)
			lpszFileName = npszFilePath;
		else
			lpszFileName = lpszFileName + 1;
		if (lpszFileName[wcslen(lpszFileName) - 1] == 't')
			mbIsTxt = true;
		else
			mbIsTxt = false;
		
		PAGE_PDF_CONTEXT ldPageContent;
		ldPageContent.pageContext = 0;
		ldPageContent.pageContext2 = 0;
		ldPageContent.pageIndex = 0;
		if (npFileAttrib != NULL)
		{
			//��ԭ����
			ldPageContent.pageContext = npFileAttrib->PageContext;
			ldPageContent.pageContext2 = npFileAttrib->PageContext2;
			ldPageContent.pageIndex = npFileAttrib->PageNumber;
		}

		FILE* lfile = NULL;
		bool lbIsUsed = true;
		try
		{
			_wfopen_s(&lfile, npszFilePath, L"rb+");
		}
		catch (...)
		{
		}

		if (lfile != NULL)
		{
			lbIsUsed = false;
			fclose(lfile);
		}

		ULONG lulRet = mpPdfPicture->OpenFile(mszTempFile, ldPageContent.pageContext<=0?NULL:&ldPageContent, npszFilePath);
		mbIsSetPartial = true;
		if (lulRet != EDERR_SUCCESS/* && lulRet != EDERR_EMPTY_CONTENT*/)
		{
			if (mbIsTxt == false || GetFileSize(npszFilePath) > 0)
			{
				//���ļ�ʧ��
				ShowToast(L"OpenFileFail");
				mszSrcFile[0] = UNICODE_NULL;
				mpToolbarBottom->GetIterator()->SetVisible(false);
				//CExMessage::PostMessage(mpIterator, mpIterator, EEVT_ER_OPEN_FILE, CExMessage::DataInvalid);
				if (mdHistroyPath.Size() > 0)
					break;
			}
		}

		//mhFile = CreateFile(mszSrcFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);

		mpIterToast->SetVisible(false);
		DWORD ldwAttrib = GetFileAttributes(mszTempFile);
		if (lbIsUsed != false)
			ldwAttrib |= FILE_ATTRIBUTE_READONLY; //�������������ڴ����pdf�������Ǳ��治�˵ģ����������������Ϊ�ļ���ֻ����
		mpToolbarH->SetFileAttrib(ldwAttrib);
		mpToolbarH->SetDoctype(mpPdfPicture->GetDocType());
		mpPreNextButton->SetFileAttrib(ldwAttrib);

		mpToolbarBottom->SetDoctype(mpPdfPicture->GetDocType());

		if (mszSrcFile[0] == UNICODE_NULL && mpToolbarH != NULL && mpToolbarBottom != NULL)
		{
			ShowToast(L"OpenFileFail");
			mpToolbarH->GetIterator()->SetVisible(true);
			break;
		}

		bool lbIsDoubleScreen = mpToolbarH->GetDuopageStatus();
		mpPdfPicture->EnableDuopageView(lbIsDoubleScreen);
		mpToolbarBottom->EnableAutoZoomButton(!lbIsDoubleScreen);

		//���ܱ任����������������λ�ã��м�
		//if (ldPageContent.pageIndex > 1 && ldPageContent.pageContext == 0 && ldPageContent.pageContext2 == 0)
		if (mpPdfPicture->GetDocType() == DOC_TYPE_PDF)
		{
			if (ldPageContent.pageIndex <= 0)
				ldPageContent.pageIndex = 1;
			mpPdfPicture->GoToPage(ldPageContent.pageIndex, true);
		}
		else
		{
			//ֻ��txt��ʽ�ļ������������ַ�ʽ
			mpPdfPicture->GoToPage((IEdPage_ptr)NULL, true);
		}

		//��¼����ʷ
		//�ȿ�������ǲ����Ѿ���¼����
		bool lbIsHave = false;
		wchar_t lszOldThumbanilFilePath[MAX_PATH] = { 0 };
		wchar_t lszNewThumbanilFilePath[MAX_PATH] = { 0 };
		mpPdfPicture->GetThumbanilsPath(lszNewThumbanilFilePath, MAX_PATH);

		for (int i=0;i<mdHistroyPath.Size();i++)
		{
			HISTORY_FILE_ATTRIB* lpHistoryFile = mdHistroyPath.GetEntry(i);
			if (_wcsicmp(npszFilePath, lpHistoryFile->FilePath) == 0)
			{
				//�Ѿ����ˣ������ᵽ��һ������
				mdHistroyPath.RemoveByIndex(i);
				mdHistroyPath.Insert(0, lpHistoryFile);

				wcscpy_s(lszOldThumbanilFilePath, MAX_PATH, lpHistoryFile->ThumbanilFilePath);

				wcscpy_s(lpHistoryFile->ThumbanilFilePath, MAX_PATH, lszNewThumbanilFilePath);

				lbIsHave = true;
				break;
			}
		}

		if (lbIsHave == false)
		{
			//���ӵ��б�
			HISTORY_FILE_ATTRIB* lpHistoryFile = new HISTORY_FILE_ATTRIB();
			wchar_t* lpszPath = new wchar_t[MAX_PATH];
			wcscpy_s(lpHistoryFile->FilePath, MAX_PATH, npszFilePath);
			wcscpy_s(lpHistoryFile->ThumbanilFilePath, MAX_PATH, lszNewThumbanilFilePath);
			lpHistoryFile->IsModify = 0;
			lpHistoryFile->ReadProgress = 1;
			time_t lTimeNow;
			time(&lTimeNow);
			lpHistoryFile->TimeStamp = (DWORD)lTimeNow;

			mdHistroyPath.Insert(0, lpHistoryFile);

			if (mdHistroyPath.Size() > RBF_HISTROY_MAX)
			{
				//����������ˣ�ɾ�����һ��
				//�����������ͼ
				HISTORY_FILE_ATTRIB* lpFileAttrib = mdHistroyPath.GetEntry(mdHistroyPath.Size() - 1);
				ClearThumbnail(lpFileAttrib->ThumbanilFilePath);

				delete[] lpFileAttrib;
				mdHistroyPath.RemoveByIndex(mdHistroyPath.Size() - 1);
			}
		}

		//�鿴�Ƿ���Ҫɾ����������ͼĿ¼
		if (lszOldThumbanilFilePath != NULL && lszOldThumbanilFilePath[0] != UNICODE_NULL && _wcsicmp(lszOldThumbanilFilePath,lszNewThumbanilFilePath) != 0)
		{
			ClearThumbnail(lszOldThumbanilFilePath);
		}

		//���浽ע���
		wchar_t lszText[MAX_PATH];
		DWORD ldwLen = 0;
		for (int i = 0; i < mdHistroyPath.Size(); i++)
		{
			HISTORY_FILE_ATTRIB* lpHistoryFile = mdHistroyPath.GetEntry(i);

			swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_PATH, i);
			CHellFun::SetRegData(lszText, 0, lpHistoryFile->FilePath);

			swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_THUMBNAIL_PATH, i);
			CHellFun::SetRegData(lszText, 0, lpHistoryFile->ThumbanilFilePath);

			swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_MODIFY, i);
			CHellFun::SetRegData(lszText, lpHistoryFile->IsModify);

			swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_PROGRESS, i);
			CHellFun::SetRegData(lszText, lpHistoryFile->ReadProgress);

			swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_TIMESTAMP, i);
			CHellFun::SetRegData(lszText, lpHistoryFile->TimeStamp);

			swprintf_s(lszText, MAX_PATH, L"%s%d", RbF_REG_PAGE_CONTENT, i);
			CHellFun::SetRegData(lszText, lpHistoryFile->PageContext);

			swprintf_s(lszText, MAX_PATH, L"%s%d", RbF_REG_PAGE_CONTENT2, i);
			CHellFun::SetRegData(lszText, lpHistoryFile->PageContext2);

			swprintf_s(lszText, MAX_PATH, L"%s%d", RbF_REG_PAGE_NUMBER, i);
			CHellFun::SetRegData(lszText, lpHistoryFile->PageNumber);

			// Saving scaling data [zhuhl5@20200116]
			swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_AUTO_ZOOM, i);
			CHellFun::SetRegData(lszText, lpHistoryFile->autoZoom);

			swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_SCALING, i);
			CHellFun::SetRegData(lszText, lpHistoryFile->scaling);
			
			swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_SCALINGLEVEL, i);
			CHellFun::SetRegData(lszText, lpHistoryFile->scalingLevel);

			swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_SCALINGPOSX, i);
			CHellFun::SetRegData(lszText, lpHistoryFile->scalingPosX);

			swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_SCALINGPOSY, i);
			CHellFun::SetRegData(lszText, lpHistoryFile->scalingPosY);
		}

		ShowPageInfo();

		ShowToolBar(true);

		mpHighlight->HideSelect();

		// Recover scaling params [zhuhl5@20200116]
		if (npFileAttrib)
		{
			if (npFileAttrib->autoZoom == 1 && !mbIsTxt)
				mpToolbarBottom->SetZoomStatus(ZoomStatus::AUTO_ZOOM);
			else if (npFileAttrib->scalingLevel == 0 && !mbIsTxt)
				mpToolbarBottom->SetZoomStatus(ZoomStatus::NONE);
			else
				mpToolbarBottom->SetZoomStatus(ZoomStatus::ZOOM, npFileAttrib->scalingLevel);
		}
		else
		{
			mpToolbarBottom->SetZoomStatus(ZoomStatus::NONE);
		}
		// Recover zoomed position [zhuhl5@20200116]
/*		Disable for now, and this part is not done developing. Acquiring absolute zoom position needs to be done if we have recovering zoom position as requirement.
		if(npFileAttrib)
		{
			POINT pt{ LONG(npFileAttrib->scalingPosX), LONG(npFileAttrib->scalingPosY) };
			//RECT ldRect;
			CExMessage::PostMessage(mpIterator, mpIterator, EEVT_ER_SET_PAGE_MOVE, pt);
			//CExMessage::SendMessage(mpIterator->GetParent()->GetParent(), mpIterator, EEVT_ER_SET_PAGE_MOVE, ldPos, &ldRect, sizeof(RECT));
		}
*/
		lbRet = true;

	} while (false);

	return lbRet;
}

//��ʱ��
void CReaderBaseFrame::OnTimer(
	PSTEMS_TIMER npStatus
	)
{
	if (npStatus->TimerID == RBF_TIMER_TOAST)
	{
		mpIterator->KillTimer(RBF_TIMER_TOAST);
		mpIterToast->SetVisible(false);
	}
	else if (npStatus->TimerID == RBF_TIMER_TSHOW_TOOLBAR)
	{
		//ShowToolBar(false); //�����Զ����ع�����
	}
	else if (npStatus->TimerID == RBF_TIMER_ENABL_PARTIAL)
	{
		mpIterator->KillTimer(npStatus->TimerID);
		EiSetPartialUpdate(TRUE);
	}
	else if (npStatus->TimerID == RBF_TIMER_INIT)
	{
		mpIterator->KillTimer(npStatus->TimerID);
		Init();
	}
	else if (npStatus->TimerID == RBF_TIMER_EXIT)
	{
		ExitProcess(0);
	}
	else if (npStatus->TimerID == RBF_TIMER_HOLDINPUT)
	{
		HoldInput(false);
	}
	else if (npStatus->TimerID == RBF_TIMER_ACTIVITY)
	{
		mpPreNextButton->SetPenMode(mulPenMode);
	}
}



//ϵͳ��Ļ������ת
void CReaderBaseFrame::OnRotated(ULONG nuOrient)
{
	EI_SIZE ldPaintSize;

	EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);
	
	if (mpPdfPicture != NULL)
	{
		//mpPdfPicture->GetIterator()->SetSize((FLOAT)ldPaintSize.w, (FLOAT)ldPaintSize.h);��һ�����ñ������С�󣬱�����Ĵ�С�ı���Ӧ�����Ѿ���pdfpicture�������˴�С������
		mpPdfPicture->SetRotation(nuOrient);
	}

	mpIterator->SetSize((FLOAT)ldPaintSize.w, (FLOAT)ldPaintSize.h);


	if (mpPreNextButton != NULL)
	{
		mpPreNextButton->GetIterator()->SetSize((FLOAT)ldPaintSize.w, (FLOAT)ldPaintSize.h);
		//mpPreNextButton->SetRotation(nuOrient);
	}

	if (mpToolbarH != NULL)
		mpToolbarH->GetIterator()->SetSize((FLOAT)ldPaintSize.w, 80.0f);

	if (mpToolbarBottom != NULL)
	{
		mpToolbarBottom->GetIterator()->SetSize((FLOAT)ldPaintSize.w, 80.0f);
		mpToolbarBottom->SetFatRatio(mpPdfPicture->GetFatRatio());

		RECT ldRect;
		mpPdfPicture->CalcMovalbe(ldRect);
		mpToolbarBottom->ShowMoveButton(ldRect);
	}
		
	if (mpHighlight != NULL)
	{
		mpHighlight->GetIterator()->SetSize((FLOAT)ldPaintSize.w, (FLOAT)ldPaintSize.h);
		mpHighlight->HideSelect();
	}
	
	if (pdfOverwriteDlg != NULL)
	{
		PlaceChildInCenter(pdfOverwriteDlg->GetIterator());
	}
	if (promptDlg != NULL)
	{
		PlaceChildInCenter(promptDlg->GetIterator());
	}
	if (convertProgressDlg != NULL)
	{
		PlaceChildInCenter(convertProgressDlg->GetIterator());
	}
	if (m_fileOpenFailDlg != NULL)
	{
		PlaceChildInCenter(m_fileOpenFailDlg->GetIterator());
	}

	if (askConvertDlg != NULL)
	{
		PlaceChildInCenter(askConvertDlg->GetIterator());
	}

	if (nuOrient == GIR_90 || nuOrient == GIR_270)
	{
		//����
	}
	else
	{
		//����
			
	}

	ShowPageInfo();
	////�ػ��Լ�
	//EinkuiGetSystem()->UpdateView();
}

//Ԫ�زο��ߴ緢���仯
ERESULT CReaderBaseFrame::OnElementResized(D2D1_SIZE_F nNewSize)
{
	do 
	{
		EI_SIZE ldPaintSize;
		EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);

		//if(mbIsInit == false)
		//	break;

		if (mpPdfPicture != NULL)
		{
			mpPdfPicture->GetIterator()->SetSize((FLOAT)ldPaintSize.w, (FLOAT)ldPaintSize.h);
		}

		if (mpFileHistoryDlg != NULL)
		{
			mpFileHistoryDlg->GetIterator()->SetSize((FLOAT)ldPaintSize.w, (FLOAT)ldPaintSize.h);
		}

		if (mpThumbnailDlg != NULL)
		{
			mpThumbnailDlg->GetIterator()->SetSize((FLOAT)ldPaintSize.w, (FLOAT)ldPaintSize.h);
		}

		if (mpLoadingView != NULL)
			mpLoadingView->GetIterator()->SetSize(mpIterator->GetSize());

		if(mpIterBackground != NULL)
			mpIterBackground->SetSize(nNewSize);

		if(mpSnapShot != NULL)
			mpSnapShot->GetIterator()->SetSize(mpIterator->GetSize());

		if (mZoomStatus == ZoomStatus::AUTO_ZOOM)
			CExMessage::SendMessage(mpIterator, mpIterator, EEVT_ER_AUTO_ZOOM, NULL, NULL, NULL);
	
	} while (false);

	return ERESULT_SUCCESS;
}

//����ҳ����ת���ڵ�λ��
void CReaderBaseFrame::SetJumpPagePos(void)
{
	if (mpJumpPage != NULL)
	{
		EI_SIZE ldPaintSize;
		EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);

		FLOAT lfX = (ldPaintSize.w - mpJumpPage->GetIterator()->GetSizeX()) / 2;
		lfX = lfX - mpIterator->GetPosition().x;
		D2D1_POINT_2F ldPos;
		ldPos.x = lfX;
		if (ldPaintSize.w > ldPaintSize.h)
		{
			ldPos.y = 200.0f;
		}
		else
		{
			ldPos.y = 522.0f;
		}
		mpJumpPage->GetIterator()->SetPosition(ldPos);
	}

}

//�������ͼ�ļ�
void CReaderBaseFrame::ClearThumbnail(const wchar_t* npszPath)
{
	CFilePathName lCachePath;
	if (npszPath == NULL)
	{
		//�������
		lCachePath.SetByUserAppData();
		lCachePath.AssurePath();
		lCachePath += L"EinkReader\\";
	}
	else
	{
		lCachePath.SetPathName(npszPath);
	}

	wchar_t lszTemp[MAX_PATH] = { 0 };
	wcscpy_s(lszTemp, MAX_PATH, lCachePath.GetPathName());
	lszTemp[wcslen(lszTemp) + 1] = UNICODE_NULL; //˫0��β
												 //����Ѵ��ڣ���ɾ��
	SHFILEOPSTRUCT ldShfile;
	memset(&ldShfile, 0, sizeof(ldShfile));
	ldShfile.pFrom = lszTemp;
	ldShfile.fFlags = FOF_NO_UI | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;
	ldShfile.wFunc = FO_DELETE;
	SHFileOperation(&ldShfile);
}

void CReaderBaseFrame::ShowFileOpenFailDialog(const wchar_t* resourceName)
{
	if (m_fileOpenFailDlg != NULL)
	{
		m_fileOpenFailDlg->ExitModal();
		m_fileOpenFailDlg = NULL;
	}
	//��ʾת��ʧ�ܵĵ���
	ICfKey* lpSubKey = mpTemplete->OpenKey(resourceName);
	m_fileOpenFailDlg = CFileOpenFailDlg::CreateInstance(mpIterator, lpSubKey);;
	bool lbEnable = true;
	PlaceChildInCenter(m_fileOpenFailDlg->GetIterator());
	m_fileOpenFailDlg->DoModal(&lbEnable);

	CMM_SAFE_RELEASE(lpSubKey);
	m_fileOpenFailDlg = nullptr;
	//��������
	CExMessage::SendMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_PEN_MODE, PEN_MODE_PEN);
}

void CReaderBaseFrame::PlaceChildInCenter(IEinkuiIterator * childIterator)
{
	if (!childIterator) return;
	auto size = GetIterator()->GetSize();
	auto childSize = childIterator->GetSize();
	decltype(childIterator->GetPosition()) position;
	position.x = (size.width - childSize.width) / 2;
	position.y = (size.height - childSize.height) / 2;
	childIterator->SetPosition(position);
}

//��ȡ�ļ���С
ULONG CReaderBaseFrame::GetFileSize(wchar_t* npszFilePath)
{
	ULONG lulRet = 1;
	HANDLE lhTxtFile = INVALID_HANDLE_VALUE;
	LARGE_INTEGER lFileSize;

	do 
	{
		lhTxtFile = CreateFile(npszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
		if (lhTxtFile == INVALID_HANDLE_VALUE)
			break;

		if (GetFileSizeEx(lhTxtFile, &lFileSize) == FALSE)
			break;

		lulRet = lFileSize.LowPart;

	} while (false);

	if (lhTxtFile != INVALID_HANDLE_VALUE)
		CloseHandle(lhTxtFile);

	return lulRet;
}

//��ʼ��
void CReaderBaseFrame::Init(void)
{
	DWORD ldwRet = 0, ldwLen = 0;;
	HKEY lhKey = NULL;
	HKEY lhKey2 = NULL;
	wchar_t lszText[MAX_PATH] = { 0 };
	
	do 
	{
		/*if (mhFile != INVALID_HANDLE_VALUE)
			CloseHandle(mhFile);*/

		DWORD ldwValue = 0;
		//��־����
		ldwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Lenovo\\EinkSDK", 0, KEY_READ | KEY_WOW64_64KEY, &lhKey2);
		if (ldwRet == ERROR_SUCCESS)
		{
			ldwLen = sizeof(DWORD);
			ldwValue = 0;
			RegQueryValueEx(lhKey2, RbF_REG_LOG, NULL, NULL, (BYTE*)&ldwValue, &ldwLen);
			mpPdfPicture->SetLogStatus(ldwValue == 0 ? false : true);
		}

		if (lhKey2 != NULL)
			RegCloseKey(lhKey2);

		ldwRet = RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Lenovo\\Eink-PdfReader", 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &lhKey);
		if (ldwRet == ERROR_SUCCESS)
		{
			//�Ƿ��һ�δ�
			ldwLen = sizeof(DWORD);
			ldwValue = 0;
			RegQueryValueEx(lhKey, RbF_REG_ONCE, NULL, NULL, (BYTE*)&ldwValue, &ldwLen);
			if (ldwValue == 0)
			{
				ldwValue = 1;
				RegSetValueEx(lhKey, RbF_REG_ONCE, NULL, REG_DWORD, (BYTE*)&ldwValue, sizeof(DWORD));

				CFilePathName ldPath;
				ldPath.SetByModulePathName();
				ldPath.Transform(L".\\EInkHelp.exe");
				//��֤exe�Ƿ�ǩ��
				FileCVCheckVaule filecheckvalue;
				wcscpy_s(filecheckvalue.CVPath, ldPath.GetPathName());
				DWORD resultCheck = EiIsFileCertificateValidation(filecheckvalue);
				if (filecheckvalue.checkResult != 0)
				{
					//����reader����ҳ
					ShellExecute(NULL, L"open", ldPath.GetPathName(), L"/first-time-Reader", NULL, SW_SHOW);
				}
			}

			//////////////////////////////////////////////////////////////////////////
			//��������
			ldwLen = sizeof(DWORD);
			ldwValue = 10;
			RegQueryValueEx(lhKey, RbF_REG_SCREEN_ORI, NULL, NULL, (BYTE*)&ldwValue, &ldwLen);
			mpToolbarH->SetScreenOriButton((ULONG)ldwValue);
			DWORD ldwOrient = GIR_NONE;
			mbIsScreenAuto = false;
			if (ldwValue == SCREEN_STATUS_AUTO)
			{
				EI_SYSTEM_INFO ldInfo;
				EiGetSystemInfo(&ldInfo);
				ldwOrient = ldInfo.ulOrient;
				mbIsScreenAuto = true;
			}
			else if (ldwValue == SCREEN_STATUS_H)
			{
				EI_SYSTEM_INFO ldInfo;
				EiGetSystemInfo(&ldInfo);
				ldwOrient = ldInfo.ulOrient;
				if (ldwOrient == GIR_90 || ldwOrient == GIR_270)
				{
					//TBS ����180�ȣ����򿪸�ʱreader���򲻶�
					ldwOrient = GIR_NONE;
				}
			}
			else if (ldwValue == SCREEN_STATUS_V)
			{
				ldwOrient = GIR_90;
			}
			EiSetScreenOrient(ldwOrient);
			EinkuiGetSystem()->ResetPaintboard();
			OnRotated(ldwOrient);
			//////////////////////////////////////////////////////////////////////////

			//�ʿ��
			ldwLen = sizeof(DWORD);
			RegQueryValueEx(lhKey, RbF_REG_PEN_WIDTH_INDEX, NULL, NULL, (BYTE*)&mdwPenWidthIndex, &ldwLen);
			mpPdfPicture->SetPenWidth(mfArrayPenWidthArray[mdwPenWidthIndex]);
			EiSetHandwritingSetting(mfArrayFwPenWidthArray[mdwPenWidthIndex]);

			//����ɫ
			ldwLen = sizeof(DWORD);
			RegQueryValueEx(lhKey, RbF_REG_PEN_COLOR_INDEX, NULL, NULL, (BYTE*)&mdwPenColorIndex, &ldwLen);
			mpPdfPicture->SetPenColor(mdwPenColorIndex);
			if (mpToolbarH != NULL)
				mpToolbarH->SetPenData(mdwPenWidthIndex, mdwPenColorIndex);

			//�ϴ����õ�txt�ֺ�
			mdwFontsizeIndex = 1;
			ldwLen = sizeof(DWORD);
			RegQueryValueEx(lhKey, RbF_REG_TXT_FONT_SIZE_INDEX, NULL, NULL, (BYTE*)&mdwFontsizeIndex, &ldwLen);

			//��˫ҳ
			ldwLen = sizeof(DWORD);
			ldwValue = 0;
			RegQueryValueEx(lhKey, RbF_REG_DOUBLE_SCREEN, NULL, NULL, (BYTE*)&ldwValue, &ldwLen);
			EI_SIZE ldPaintSize;
			EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);
			if (ldPaintSize.w > ldPaintSize.h)
			{
				mpToolbarH->SetDuopageButton(ldwValue == 0 ? false : true);
			}
			else
			{
				mpToolbarH->SetDuopageButton(false);
			}

			//��ȡ��ʷ��¼
			if (mdHistroyPath.Size() <= 0)
			{
				for (int i = 0; i < RBF_HISTROY_MAX; i++)
				{
					swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_PATH, i);
					ldwLen = MAX_PATH * sizeof(wchar_t);
					HISTORY_FILE_ATTRIB* lpHistoryPath = NULL;
					if (RegQueryValueEx(lhKey, lszText, NULL, NULL, (BYTE*)&lszText, &ldwLen) == ERROR_SUCCESS)
					{
						if (lszText[0] == UNICODE_NULL)
						{
							break; //û����Ч��¼
						}

						lpHistoryPath = new HISTORY_FILE_ATTRIB();
						wcscpy_s(lpHistoryPath->FilePath, MAX_PATH, lszText);
						mdHistroyPath.Insert(-1, lpHistoryPath);
					}

					if (lpHistoryPath == NULL)
						break;

					ldwLen = MAX_PATH * sizeof(wchar_t);
					swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_THUMBNAIL_PATH, i);
					if (RegQueryValueEx(lhKey, lszText, NULL, NULL, (BYTE*)&lszText, &ldwLen) == ERROR_SUCCESS)
						wcscpy_s(lpHistoryPath->ThumbanilFilePath, MAX_PATH, lszText);

					swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_MODIFY, i);
					ldwLen = sizeof(DWORD);
					if (RegQueryValueEx(lhKey, lszText, NULL, NULL, (BYTE*)&lpHistoryPath->IsModify, &ldwLen) != ERROR_SUCCESS)
						lpHistoryPath->IsModify = 0;


					swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_PROGRESS, i);
					ldwLen = sizeof(DWORD);
					if (RegQueryValueEx(lhKey, lszText, NULL, NULL, (BYTE*)&lpHistoryPath->ReadProgress, &ldwLen) != ERROR_SUCCESS)
						lpHistoryPath->ReadProgress = 1;

					swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_TIMESTAMP, i);
					ldwLen = sizeof(DWORD);
					if (RegQueryValueEx(lhKey, lszText, NULL, NULL, (BYTE*)&lpHistoryPath->TimeStamp, &ldwLen) != ERROR_SUCCESS)
						lpHistoryPath->TimeStamp = 0;

					swprintf_s(lszText, MAX_PATH, L"%s%d", RbF_REG_PAGE_NUMBER, i);
					ldwLen = sizeof(DWORD);
					if (RegQueryValueEx(lhKey, lszText, NULL, NULL, (BYTE*)&lpHistoryPath->PageNumber, &ldwLen) != ERROR_SUCCESS)
						lpHistoryPath->PageNumber = 0;

					swprintf_s(lszText, MAX_PATH, L"%s%d", RbF_REG_PAGE_CONTENT, i);
					ldwLen = sizeof(DWORD);
					if (RegQueryValueEx(lhKey, lszText, NULL, NULL, (BYTE*)&lpHistoryPath->PageContext, &ldwLen) != ERROR_SUCCESS)
						lpHistoryPath->PageContext = 0;

					swprintf_s(lszText, MAX_PATH, L"%s%d", RbF_REG_PAGE_CONTENT2, i);
					ldwLen = sizeof(DWORD);
					if (RegQueryValueEx(lhKey, lszText, NULL, NULL, (BYTE*)&lpHistoryPath->PageContext2, &ldwLen) != ERROR_SUCCESS)
						lpHistoryPath->PageContext2 = 0;

					// Loading scaling data [zhuhl5@20200116]
					swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_AUTO_ZOOM, i);
					ldwLen = sizeof(float);
					if (RegQueryValueEx(lhKey, lszText, NULL, NULL, (BYTE*)&lpHistoryPath->autoZoom, &ldwLen) != ERROR_SUCCESS)
						lpHistoryPath->autoZoom = 0;

					swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_SCALING, i);
					ldwLen = sizeof(float);
					if (RegQueryValueEx(lhKey, lszText, NULL, NULL, (BYTE*)&lpHistoryPath->scaling, &ldwLen) != ERROR_SUCCESS)
					{
						lpHistoryPath->scaling = 1.0f;
					}
					else
					{
						if (lpHistoryPath->scaling < 1.0f)	// We don't have zoom out.
						{
							lpHistoryPath->scaling = 1.0f;
						}
					}

					swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_SCALINGLEVEL, i);
					ldwLen = sizeof(float);
					if (RegQueryValueEx(lhKey, lszText, NULL, NULL, (BYTE*)&lpHistoryPath->scalingLevel, &ldwLen) != ERROR_SUCCESS)
						lpHistoryPath->scalingLevel = 0;

					swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_SCALINGPOSX, i);
					ldwLen = sizeof(float);
					if (RegQueryValueEx(lhKey, lszText, NULL, NULL, (BYTE*)&lpHistoryPath->scalingPosX, &ldwLen) != ERROR_SUCCESS)
						lpHistoryPath->scalingPosX = 0.0f;

					swprintf_s(lszText, MAX_PATH, L"%s%d", RBF_REG_HISTORY_SCALINGPOSY, i);
					ldwLen = sizeof(float);
					if (RegQueryValueEx(lhKey, lszText, NULL, NULL, (BYTE*)&lpHistoryPath->scalingPosY, &ldwLen) != ERROR_SUCCESS)
						lpHistoryPath->scalingPosY = 0.0f;

				}
				if (mdHistroyPath.Size() <= 0)
					EiSetBatteryStatus(GI_BATTERY_HIDE);
			}
		}
		else
		{
			//preload���޴�ע���·��������reader����ʾ����ҳ
			ldwRet = RegCreateKey(HKEY_CURRENT_USER, L"SOFTWARE\\Lenovo\\Eink-PdfReader", &lhKey);
			if (ldwRet == ERROR_SUCCESS)
			{
				DWORD showhelp = 1;
				ldwRet = RegSetValueEx(lhKey, RbF_REG_ONCE, NULL, REG_DWORD, (BYTE*)&showhelp, sizeof(DWORD));

				CFilePathName ldPath;
				ldPath.SetByModulePathName();
				ldPath.Transform(L".\\EInkHelp.exe");

				//��֤exe�Ƿ�ǩ��
				FileCVCheckVaule filecheckvalue;
				wcscpy_s(filecheckvalue.CVPath, ldPath.GetPathName());
				DWORD resultCheck = EiIsFileCertificateValidation(filecheckvalue);
				if (filecheckvalue.checkResult != 0)
				{
				//����reader����ҳ
					ShellExecute(NULL, L"open", ldPath.GetPathName(), L"/first-time-Reader", NULL, SW_SHOW);
				}
			}
		}
		//�ʿ��
		mpPdfPicture->SetPenWidth(mfArrayPenWidthArray[mdwPenWidthIndex]);
		EiSetHandwritingSetting(mfArrayFwPenWidthArray[mdwPenWidthIndex]);

		//����ɫ
		mpPdfPicture->SetPenColor(mdwPenColorIndex);
		if (mpToolbarH != NULL)
			mpToolbarH->SetPenData(mdwPenWidthIndex, mdwPenColorIndex);

		//txt�ֺ�
		if (mdwFontsizeIndex >= ZCT_FONTSIZE_LEVEL || mdwFontsizeIndex < 0)
			mdwFontsizeIndex = 1;
		mpToolbarH->SetTxtFontSizeIndex(mdwFontsizeIndex);

		if (mdHistroyPath.Size() > 0)
		{
			//˵���򿪹��ļ�,�ָ��ϴδ�״̬
			mpPdfPicture->SetFontSize(mdwFontSizeArray[mdwFontsizeIndex]);

			
			if (GetFileAttributes(mdHistroyPath.GetEntry(0)->FilePath) == INVALID_FILE_ATTRIBUTES)
			{
				//�ļ���������
				CExMessage::PostMessage(mpIterator, mpIterator, EEVT_ER_OPEN_FILE, CExMessage::DataInvalid);
				break;
			}

			//mpToolbarH->SetBCoverState();

			if (!m_cmdLineOpenFileName.empty() && m_cmdLineOpenFileName.size() >= 4) // sizeof ".pdf" == 4
			{
				unique_ptr<wchar_t[]> extName = make_unique<wchar_t[]>(10);
				wcscpy_s(extName.get(), 10, m_cmdLineOpenFileName.c_str() + m_cmdLineOpenFileName.length() - 4);
				for (int i = 0; extName[i] != 0; i++)
				{
					extName[i] = std::tolower(extName[i]);
				}

				if (wcscmp(extName.get(), L".pdf") == 0) // Open PDF files
				{
					unique_ptr<wchar_t[]> buffer = make_unique<wchar_t[]>(2048);
					wcscpy_s(buffer.get(), 2048, m_cmdLineOpenFileName.c_str());
					OpenFile(buffer.get(), nullptr);
				}
				else
				{
					PDFConvert::SubmitConvertTask(m_cmdLineOpenFileName);
					while (!PDFConvert::IsConvertingCompleted()) Sleep(100);
					auto convertResult = PDFConvert::GetConvertResult();
					if (convertResult == PDFConvert::ConvertResult::OK)
					{
						auto pdfFileName = PDFConvert::GetPDFFileFullPath();
						unique_ptr<wchar_t[]> buffer = make_unique<wchar_t[]>(2048);
						wcscpy_s(buffer.get(), 2048, pdfFileName.c_str());
						OpenFile(buffer.get(), nullptr);
					}
				}
			}
			else
			{
				OpenFile(mdHistroyPath.GetEntry(0)->FilePath, mdHistroyPath.GetEntry(0));
			}


			//wcscpy_s(mszSrcFile, MAX_PATH, mdHistroyPath.GetEntry(0)->FilePath);
			//CopyFileToTemp(mdHistroyPath.GetEntry(0)->FilePath, mszTempFile, MAX_PATH);
			//

			////�ļ���
			//wchar_t* lpszFileName = wcsrchr(mdHistroyPath.GetEntry(0)->FilePath, L'\\');
			//if (lpszFileName == NULL)
			//	lpszFileName = mdHistroyPath.GetEntry(0)->FilePath;
			//else
			//	lpszFileName = lpszFileName + 1;

			//if (lpszFileName[wcslen(lpszFileName) - 1] == 't')
			//	mbIsTxt = true;
			//else
			//	mbIsTxt = false;

			//PAGE_PDF_CONTEXT ldPageContent;
			//ldPageContent.pageContext = mdHistroyPath.GetEntry(0)->PageContext;
			//ldPageContent.pageContext2 = mdHistroyPath.GetEntry(0)->PageContext2;
			//ldPageContent.pageIndex = mdHistroyPath.GetEntry(0)->PageNumber;
			//mulPageIndex = 0;
			//ULONG lulRet = mpPdfPicture->OpenFile(mszTempFile, &ldPageContent, mdHistroyPath.GetEntry(0)->FilePath);
			//mbIsSetPartial = true;
			//if (lulRet != EDERR_SUCCESS/* && lulRet != EDERR_EMPTY_CONTENT*/)
			//{
			//	if (mbIsTxt == false || GetFileSize(mdHistroyPath.GetEntry(0)->FilePath) > 0)
			//	{
			//		//��ʧ���ˣ������ļ���������
			//		mszSrcFile[0] = UNICODE_NULL;
			//		CExMessage::PostMessage(mpIterator, mpIterator, EEVT_ER_OPEN_FILE, CExMessage::DataInvalid);
			//		break;
			//	}
			//}

			////mhFile = CreateFile(mszSrcFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);

			////�Ƿ�˫��
			//ldwLen = sizeof(DWORD);
			//RegQueryValueEx(lhKey, RbF_REG_DOUBLE_SCREEN, NULL, NULL, (BYTE*)&ldwValue, &ldwLen);
			//mpToolbarH->SetDuopageButton(ldwValue == 0 ? false : true);
			//DWORD ldwAttrib = GetFileAttributes(mszTempFile);
			//mpPreNextButton->SetFileAttrib(ldwAttrib);
			//mpToolbarH->SetFileAttrib(ldwAttrib);
			//mpToolbarH->SetDoctype(mpPdfPicture->GetDocType());
			//
			//mpToolbarBottom->SetDoctype(mpPdfPicture->GetDocType());

			/*EI_SIZE ldPaintSize;
			EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);
			if (ldPaintSize.w > ldPaintSize.h)
			{
				mpToolbarH->SetDuopageButton(ldwValue == 0 ? false : true);
			}
			else
			{
				mpToolbarH->SetDuopageButton(false);
			}*/

			//mpPdfPicture->EnableDuopageView(mpToolbarH->GetDuopageStatus());

			////���ܱ任����������������λ�ã��м�
			////if (ldPageContent.pageIndex > 1 && ldPageContent.pageContext == 0 && ldPageContent.pageContext2 == 0)
			////ldPageContent.pageIndex = 2;
			//if(mpPdfPicture->GetDocType() == DOC_TYPE_PDF)
			//{
			//	if (ldPageContent.pageIndex <= 0)
			//		ldPageContent.pageIndex = 1;
			//	mpPdfPicture->GoToPage(ldPageContent.pageIndex);
			//}
			//else
			//{
			//	//ֻ��txt��ʽ�ļ������������ַ�ʽ
			//	mpPdfPicture->GoToPage((IEdPage_ptr)NULL);
			//}

			//ShowPageInfo();
			//ShowToolBar(true);
			//mpHighlight->HideSelect();
		}
		else
		{
			//˵���ǵ�һ�δ򿪣�ֱ�ӵ������ļ��������ľͲ���Ҫ��ʼ����
			//ShowFileOpenDlg();
			CExMessage::PostMessage(mpIterator,mpIterator, EEVT_ER_OPEN_FILE, CExMessage::DataInvalid);
			break;
		}

	} while (false);
}

ERESULT __stdcall CReaderBaseFrame::EinkUpdating(ULONGLONG nxNumber, CReaderBaseFrame* npThis)	// nuNumber�Ǹ�����ţ�ÿ�θ��¼�һ���ﵽ���ֵ��ص���
{
	do 
	{
		if (npThis->mbIsSetPartial == false)
		{
			//npThis->mbIsSetPartial = true;
			break; //��ʱ��Ҫ�ر�partial,���ڴ��ڽ���Ƶ���仯�׶�
		}
			

		ULONG luCrtTick = GetTickCount();

		if ((nxNumber - npThis->mxLastGcUiNumber >= 50 && luCrtTick - npThis->muLastGcTick > 1000*5) || luCrtTick - npThis->muLastGcTick > 1000 * 60 * 5)	// ʮ֡���������
		{
			// ���ý���GCģʽ ???niu
			EinkuiGetSystem()->ClearEinkBuffer();
			EiSetPartialUpdate(FALSE);
			OutputDebugString(L"enter gc false partial");
			npThis->mbGcMode = true;
			npThis->muLastGcTick = luCrtTick;
			npThis->mxLastGcUiNumber = nxNumber;
		}
		else
		{
			if (npThis->mbGcMode != FALSE)
			{
				// �����˳�GCģʽ ???niu
				EiSetPartialUpdate(TRUE);
				OutputDebugString(L"enter gc true partial");
				npThis->mbGcMode = false;
			}
		}

	} while (false);
	

	return ERESULT_SUCCESS;
}

tuple<bool, wstring, bool> CReaderBaseFrame::ConvertAndOpenOfficeFile(const wchar_t * filePath)
{
	//ʵ��ת���ĵ�
	static const std::unordered_map<PDFConvert::ConvertResult, const wchar_t*> kErrorDialogMap {
		{ PDFConvert::ConvertResult::OK, L""},//hy
		{ PDFConvert::ConvertResult::OfficeComponentNotInstalled, L"ConvertFailMissingOfficeComponent" },
		{ PDFConvert::ConvertResult::UnsupportedFileType, L"ConvertFailUnsupportedFileType" },
		{ PDFConvert::ConvertResult::FileLocked, L"ConvertFailFileLocked" },
		{ PDFConvert::ConvertResult::ComError, L"ConvertFailConvertFail" },
		{ PDFConvert::ConvertResult::NeedPassword, L"ConvertFailOpenFail" },
		{ PDFConvert::ConvertResult::InvalidFile, L"ConvertFailOpenFail" },
		{ PDFConvert::ConvertResult::ConvertFailed, L"ConvertFailConvertFail" },
		{PDFConvert::ConvertResult::PDFFileLocked, L"ConvertFailPDFFileLocked"}
	};

	auto AskYesNo = [this](const wchar_t* dialogResourceName)->PromptDialogResult {
		if (!dialogResourceName) 
			return PromptDialogResult::No;

		if (promptDlg != NULL)
		{
			promptDlg->ExitModal(PromptDialogResult::No);
			promptDlg = NULL;
		}

		ICfKey* lpSubKey = mpTemplete->OpenKey(dialogResourceName);

		promptDlg = CYesNoPromptDlg::CreateInstance(mpIterator, lpSubKey);
		PlaceChildInCenter(promptDlg->GetIterator());
		auto dialogResult = promptDlg->DoModal();
		CMM_SAFE_RELEASE(lpSubKey);
		promptDlg = nullptr;
		return dialogResult;
	};

	wstring resultFilePath = wstring(filePath) + L".pdf";

	if (EInkReaderUtil::IsFileExists(resultFilePath))//��ʾ����
	{
		if (pdfOverwriteDlg != NULL)
		{
			pdfOverwriteDlg->ExitModal(OverwriteDialogResult::Cancel);
			pdfOverwriteDlg = NULL;
		}

		ICfKey* lpSubKey = mpTemplete->OpenKey(L"PDFOverwritePrompt");
		pdfOverwriteDlg = CPDFOverwriteDlg::CreateInstance(mpIterator, lpSubKey);
		PlaceChildInCenter(pdfOverwriteDlg->GetIterator());
		auto dialogResult = pdfOverwriteDlg->DoModal();
		CMM_SAFE_RELEASE(lpSubKey);
		pdfOverwriteDlg = NULL;

		switch (dialogResult)
		{
		case OverwriteDialogResult::Overwrite://���ǣ�����switchִ��ת������
			break;

		case OverwriteDialogResult::Cancel://ȡ��������
		{
			CExMessage::SendMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_PEN_MODE, PEN_MODE_PEN);
			return std::make_tuple(false, L"", true);
		}

		case OverwriteDialogResult::OpenExisting://�򿪣���ִ��ת��
		{
			if (mpThumbnailDlg != NULL)
			{
				//�����������ͼ����ת���ģ����˳�����ͼ����
				mpThumbnailDlg->ExitModal();
				mpThumbnailDlg = NULL;
			}

			auto buffer = std::make_unique<wchar_t[]>(2048);
			wcscpy_s(buffer.get(), 2048, resultFilePath.c_str());
			OpenFile(buffer.get(), nullptr);
			CExMessage::SendMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_PEN_MODE, PEN_MODE_PEN);
			return std::make_tuple(true, L"", false);
		}

		default:
		{
			CExMessage::SendMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_PEN_MODE, PEN_MODE_PEN);
			return std::make_tuple(false, L"", false);
		}
		
		}
	}

	const int64_t kWarningSize = 5 * 1024 * 1024; //�ļ�����ʱ��5MB���������û�

	if (EInkReaderUtil::GetFileSize(filePath) >= kWarningSize)
	{
		auto dialogResult = AskYesNo(L"BigOfficeFilePrompt");
		if (dialogResult == PromptDialogResult::No)
		{
			CExMessage::SendMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_PEN_MODE, PEN_MODE_PEN);
			return std::make_tuple(false, L"", true);//�ļ�����ʱ���û�ѡ��ȡ��
		}
	}

	if (pdfOverwriteDlg != NULL)
	{
		pdfOverwriteDlg->ExitModal(OverwriteDialogResult::Overwrite);
		pdfOverwriteDlg = NULL;
	}

	if (convertProgressDlg != NULL)
	{
		convertProgressDlg->ExitModal(PDFConvert::ConvertResult::ConvertFailed);
		convertProgressDlg = NULL;
	}
	if (mpThumbnailDlg != NULL)
	{
		//�����������ͼ����ת���ģ����˳�����ͼ����
		mpThumbnailDlg->ExitModal();
		mpThumbnailDlg = NULL;
	}

	ICfKey* lpSubKey = mpTemplete->OpenKey(L"ConvertProgress");
	convertProgressDlg = CConvertProgressDlg::CreateInstance(mpIterator, lpSubKey);;
	PlaceChildInCenter(convertProgressDlg->GetIterator());

	wstring pdfFileName;
	PDFConvert::ConvertResult convertResult;
	std::tie(convertResult, pdfFileName) = convertProgressDlg->DoModal(filePath);

	CMM_SAFE_RELEASE(lpSubKey);
	convertProgressDlg = NULL;

	wstring errorMessage;
	if (convertResult == PDFConvert::ConvertResult::OK)
	{
		unique_ptr<wchar_t[]> buffer = make_unique<wchar_t[]>(2048);
		wcscpy_s(buffer.get(), 2048, pdfFileName.c_str());
		if (mpFileHistoryDlg != nullptr) mpFileHistoryDlg->ExitModal();
		OpenFile(buffer.get(), nullptr);
		errorMessage = kErrorDialogMap.at(PDFConvert::ConvertResult::OK);
		CExMessage::SendMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_PEN_MODE, PEN_MODE_PEN);
		return std::make_tuple(true, errorMessage, false); //���������ֱ�����Ƿ�ɹ���errormessage,�û��Ƿ�ȡ����
	}

	errorMessage = kErrorDialogMap.at(convertResult);
	return std::make_tuple(false, errorMessage, false);
}

void CReaderBaseFrame::PageAutoZoom()
{
	EI_SIZE ldPaintSize;
	EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);
	ldPaintSize.h -= 162;

	ED_RECTF contentRect = mpPdfPicture->CalImageContentRect();
	float wRatio = ldPaintSize.w / (contentRect.right - contentRect.left);
	float hRatio = 0.0f;//ldPaintSize.h / (contentRect.bottom - contentRect.top);
	float largerRotio = wRatio > hRatio ? wRatio : hRatio;

	RECT ldRect;
	float scaleRatio = largerRotio / mpPdfPicture->GetBaseRatio();
	scaleRatio = mpToolbarBottom->AjustAutoZoomLevel(scaleRatio);
	mpPdfPicture->SetScaleRatio(scaleRatio, ldRect);

	float32 realRatio = mpPdfPicture->GetRealRatio();
	if (wRatio < hRatio)
	{
		ldRect.top = 0;
		ldRect.bottom = 0;
		mMoveForward = MoveForward::HORIZONTAL;
		mpPreNextButton->SetMoveForward(MoveForward::HORIZONTAL);
		mpToolbarBottom->SetMoveForward(MoveForward::HORIZONTAL);
	}
	else
	{
		ldRect.left = 0;
		ldRect.right = 0;
		mMoveForward = MoveForward::VERTICAL;
		mpPreNextButton->SetMoveForward(MoveForward::VERTICAL);
		mpToolbarBottom->SetMoveForward(MoveForward::VERTICAL);
	}
	mpToolbarBottom->ShowMoveButton(ldRect);

	float32 posX = contentRect.left * realRatio + ldPaintSize.w / 2.0f;
	float32 posY = contentRect.top * realRatio + ldPaintSize.h / 2.0f;
	mpPdfPicture->MovePageTo(posX, posY);

	SetHandWriteWidth();
	mpHighlight->Relocation();
}

ZoomStatus CReaderBaseFrame::GetZoomStatus()
{
	return mZoomStatus;
}
