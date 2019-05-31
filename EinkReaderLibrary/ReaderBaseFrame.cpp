/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "ReaderBaseFrame.h"
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


DEFINE_BUILTIN_NAME(Reader_BaseFrame)

CReaderBaseFrame::CReaderBaseFrame(void)
{
	mpFileOpenDlg = NULL;
	mpTipFrame = NULL;
	mpPdfPicture = NULL;
	mpJumpPage = NULL;
	mpIterBackground = NULL;
	mpPreNextButton = NULL;
	mpZoomControl = NULL;
	mpZoomControlTxt = NULL;
	mpSnapShot = NULL;
	mpIterToast = NULL;
	mlHoldInput = 0;
	mxLastGcUiNumber = 0;
	muLastGcTick = 0;
	mbGcMode = false;
	mpToolbarH = NULL;
	mbIsTxt = false;

	//txt�ֺ�
	mdwFontsizeIndex = 1;
	mdwFontSizeArray[0] = 9;
	mdwFontSizeArray[1] = 12;
	mdwFontSizeArray[2] = 15;
	mdwFontSizeArray[3] = 18;
	mdwFontSizeArray[4] = 21;

	mszTempFile[0] = UNICODE_NULL;
	mszSrcFile[0] = UNICODE_NULL;

	mbIsSetPartial = true;
	mulPageIndex = 0;

	//Sleep(1000 * 20);
}


CReaderBaseFrame::~CReaderBaseFrame(void)
{
	while (mdHistroyPath.Size() > 0)
	{
		delete mdHistroyPath.GetEntry(0);
		mdHistroyPath.RemoveByIndex(0);
	}
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
		EinkuiGetSystem()->CaptureWindowsMessage(WM_EI_RESET_TP_AREA, this, (PWINMSG_CALLBACK)&CReaderBaseFrame::OnNotifyResetTpArea);
		EinkuiGetSystem()->CaptureWindowsMessage(WM_POWERBROADCAST, this, (PWINMSG_CALLBACK)&CReaderBaseFrame::OnPowerChangeMsg);
		//mpIterator->SetTimer(EINK_SETTINGS_TIMER_ID_PAINT, MAXULONG32, 1000, NULL);
		//ImmDisableIME(-1);	//�ر����뷨

		//mpIterator->SetTimer(1,MAXULONG32,3000,NULL);

		EiSetWaveformMode(GI_WAVEFORM_DU2);
		EiCleanupScreen(0xff);
		Sleep(585); //DU260+15*5   ��16֡ˢ��ÿ֡���5ms
		EiSetWaveformMode(GI_WAVEFORM_GC16);

		ShowToolBar(true);
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

		//PDFͼƬ
		lpSubKey = mpTemplete->OpenKey(L"PicturePreview");
		mpPdfPicture = CPdfPicture::CreateInstance(mpIterator, lpSubKey);
		CMM_SAFE_RELEASE(lpSubKey);
		BREAK_ON_NULL(mpPdfPicture);

		//����������
		lpSubKey = mpTemplete->OpenKey(L"ToolbarH");
		mpToolbarH = CToolbarH::CreateInstance(mpIterator, lpSubKey);
		CMM_SAFE_RELEASE(lpSubKey);
		BREAK_ON_NULL(mpToolbarH);
		mpToolbarH->GetIterator()->BringToTop();
		
		//����
		lpSubKey = mpTemplete->OpenKey(L"ZoomControl");
		mpZoomControl = CZoomControl::CreateInstance(mpIterator, lpSubKey);
		CMM_SAFE_RELEASE(lpSubKey);
		BREAK_ON_NULL(mpZoomControl);
		mpZoomControl->GetIterator()->SetVisible(false);

		//Txt����
		lpSubKey = mpTemplete->OpenKey(L"ZoomControlTxt");
		mpZoomControlTxt = CZoomControlTxt::CreateInstance(mpIterator, lpSubKey);
		CMM_SAFE_RELEASE(lpSubKey);
		BREAK_ON_NULL(mpZoomControlTxt);
		mpZoomControlTxt->GetIterator()->SetVisible(false);

		
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

//����֪ͨӦ���л���ǰ̨���̨
ERESULT __stdcall CReaderBaseFrame::OnActivity(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& Result)
{
	do
	{
		CExMessage::SendMessage(mpIterator, mpIterator, EEVT_HB_ACTIVITE, (ULONG)wParam);

	} while (false);

	return ERESULT_WINMSG_SENDTO_NEXT;
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
		if (mpFileOpenDlg != NULL)
		{
			mpFileOpenDlg->ExitModal();
			mpFileOpenDlg = NULL;
		}
			

		{
			ICfKey* lpSubKey = mpTemplete->OpenKey(L"OpenFile");
			mpFileOpenDlg = CFileOpenDlg::CreateInstance(mpIterator, lpSubKey);;
			//mpFileOpenDlg->SetSelect(mulKeyboardLanguageIndex);

			SetOpenFilePos();
			mpFileOpenDlg->SetHistoryList(&mdHistroyPath);

			//EiSetPartialUpdate(TRUE);
			//�����ǰû�д��ļ����ļ��򿪶Ի����ȡ����ť���û�
			bool lbEnable = true;
			if (mszSrcFile[0] == UNICODE_NULL || GetFileAttributes(mszSrcFile) == INVALID_FILE_ATTRIBUTES)
				lbEnable = false;
			mpFileOpenDlg->DoModal(lbEnable);
			//EiSetPartialUpdate(FALSE);
			CMM_SAFE_RELEASE(lpSubKey);
			mpFileOpenDlg = NULL;

			if (mpPdfPicture->GetPageCount() == 0)
			{
				//���´��ļ�
				Init();
			}
		}
	
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
		mpJumpPage->DoModal();
		//EiSetPartialUpdate(FALSE);
		CMM_SAFE_RELEASE(lpSubKey);
		mpJumpPage = NULL;
		break;
	}
	case EEVT_ER_SHOW_TIP:
	{
		ICfKey* lpSubKey = mpTemplete->OpenKey(L"TipFrame");
		mpTipFrame = CTipFrame::CreateInstance(mpIterator, lpSubKey);;
		//mpFileOpenDlg->SetSelect(mulKeyboardLanguageIndex);
		mpTipFrame->GetIterator()->SetSize(mpIterator->GetSize());
		//EiSetPartialUpdate(TRUE);
		mpTipFrame->DoModal();
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

			EiSetWaveformMode(GI_WAVEFORM_DU2);
			EiCleanupScreen(0xff);
			Sleep(585); //DU260+15*5   ��16֡ˢ��ÿ֡���5ms
			EiSetWaveformMode(GI_WAVEFORM_GC16);
			EiSetPartialUpdate(FALSE);

			mpIterator->SetTimer(RBF_TIMER_ENABL_PARTIAL, 1, 1800, NULL);
			mpIterator->SetTimer(RBF_TIMER_TSHOW_TOOLBAR, 1, 5000, NULL);

			//����ղŴ򿪵��ļ���ɾ���ˣ��ͽ�����ļ��Ի���
			if (mszSrcFile[0] != UNICODE_NULL && GetFileAttributes(mszSrcFile) == INVALID_FILE_ATTRIBUTES)
			{
				mpPdfPicture->CloseFile();

				if(mpFileOpenDlg == NULL) //ֻ����û�е���������²Ŵ�
					CExMessage::PostMessage(mpIterator, mpIterator, EEVT_ER_OPEN_FILE, CExMessage::DataInvalid);
			}
		}
		else
		{
			//�л�����̨
			//��������
			//ExitProcess(0);
			mpIterator->KillTimer(RBF_TIMER_TSHOW_TOOLBAR);
			//mpToolbarH->GetIterator()->SetVisible(true);
			 //�����Ϳ������л�������һ֡ʱ�ر�partialˢ��һ��

			
			//mpIterator->SetTimer(RBF_TIMER_EXIT, 1, 400, NULL);
			//ExitProcess(0);
		}

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
		}
		else if(lulHomebarMode == GI_HOMEBAR_COLLAPSE)
		{
			//homebar��������ʾ�Լ��Ĺ�����
			mpToolbarH->GetIterator()->SetVisible(true);
			mpIterator->KillTimer(RBF_TIMER_TSHOW_TOOLBAR);
			mpIterator->SetTimer(RBF_TIMER_TSHOW_TOOLBAR, 1, 5000, NULL);
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
		int liPage = 1;
		luResult = CExMessage::GetInputData(npMsg, liPage);
		if (luResult != ERESULT_SUCCESS)
			break;

		mpPdfPicture->GoToPage(liPage);
		ShowPageInfo();

		//GotoPage(liPage);

		break;
	}
	case EEVT_ER_OPEN_FILE_PATH:
	{
		//Ҫ�򿪵��ļ�·��
		wchar_t* lpszFilePath = (wchar_t*)npMsg->GetInputData();
		bool lbRet = OpenFile(lpszFilePath);

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
			if (lulFlag == PNB_BT_PRE)
			{
				mpPdfPicture->PageFoward(false);
				ShowPageInfo();			
			}
			else if (lulFlag == PNB_BT_NEXT)
			{
				mpPdfPicture->PageFoward(true);
				ShowPageInfo();
			}
			else if (lulFlag == PNB_BT_MIDDLE)
			{
				//��ʾ�����ع�����
				ShowToolBar(!mpToolbarH->GetIterator()->IsVisible());
			}
			else
				break;

			HoldInput(true);

		} while (false);

		//???niu ���Ӷ�PageFoward()ʧ�ܵ��жϺʹ���

		break;
	}
	case EEVT_ER_TWO_SCREEN:
	{
		//�л�˫������
		bool lbIsDoubleScreen = false;
		luResult = CExMessage::GetInputData(npMsg, lbIsDoubleScreen);
		if (luResult != ERESULT_SUCCESS)
			break;

		mpPdfPicture->EnableDuopageView(lbIsDoubleScreen);
		ShowPageInfo();

		CHellFun::SetRegData(RbF_REG_DOUBLE_SCREEN, lbIsDoubleScreen==false?0:1);

		mpIterator->KillTimer(RBF_TIMER_TSHOW_TOOLBAR);
		mpIterator->SetTimer(RBF_TIMER_TSHOW_TOOLBAR, 1, 5000, NULL);

		HoldInput(true);

		break;
	}
	case EEVT_ER_ENTER_ZOOM:
	{
		//��������ģʽ
		bool lbIsZoom = false;
		luResult = CExMessage::GetInputData(npMsg, lbIsZoom);
		if (luResult != ERESULT_SUCCESS)
			break;

		if (lbIsZoom == false)
		{
			//��ʾ������
			//mpToolbarH->GetIterator()->SetVisible(true);

			mpZoomControl->GetIterator()->SetVisible(false);
			mpZoomControlTxt->GetIterator()->SetVisible(false);

			mpPreNextButton->GetIterator()->SetVisible(true);
		}
		else
		{
			//���ع�����
			mpToolbarH->GetIterator()->SetVisible(false);

			if (mbIsTxt == false)
			{
				mpZoomControl->SetFatRatio(mpPdfPicture->GetFatRatio());
				mpZoomControl->GetIterator()->SetVisible(true);

			}
			else
			{
				mpZoomControlTxt->SetFontsize(mdwFontsizeIndex);
				mpZoomControlTxt->GetIterator()->SetVisible(true);

			}
			
			mpPreNextButton->GetIterator()->SetVisible(false);
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
		// �����������
		RECT* lpOut = (RECT*)npMsg->GetOutputBuffer();
		*lpOut = ldRect;
		npMsg->SetOutputDataSize(sizeof(RECT));

		//��ȡ��ʵ��С
		D2D1_SIZE_F ldMaxSize;
		D2D1_RECT_F ldViewRect;
		mpPdfPicture->GetRectOfViewportOnPage(ldMaxSize, ldViewRect);
		mpZoomControl->SetRectOfViewportOnPage(ldMaxSize, ldViewRect);

		break;
	}
	case EEVT_ER_SET_ZOOM:
	{
		//���÷Ŵ����
		float lfRotio = 1.0f;
		luResult = CExMessage::GetInputData(npMsg, lfRotio);
		if (luResult != ERESULT_SUCCESS)
			break;

		RECT ldRect;
		mpPdfPicture->SetScaleRatio(lfRotio, ldRect);
		CHellFun::SetRegData(RbF_REG_RATIO, DWORD(lfRotio*100));

		// �����������
		RECT* lpOut = (RECT*)npMsg->GetOutputBuffer();
		*lpOut = ldRect;
		npMsg->SetOutputDataSize(sizeof(RECT));

		//��ȡ��ʵ��С
		D2D1_SIZE_F ldMaxSize;
		D2D1_RECT_F ldViewRect;
		mpPdfPicture->GetRectOfViewportOnPage(ldMaxSize, ldViewRect);
		mpZoomControl->SetRectOfViewportOnPage(ldMaxSize, ldViewRect);

		break;
	}
	case EEVT_ER_ENTER_SNAPSHOT:
	{
		//����
		ICfKey* lpSubKey = mpTemplete->OpenKey(L"SnapShot");
		mpSnapShot = CSnapShot::CreateInstance(mpIterator, lpSubKey);;
		//mpFileOpenDlg->SetSelect(mulKeyboardLanguageIndex);
		mpSnapShot->GetIterator()->SetSize(mpIterator->GetSize());
		//EiSetPartialUpdate(TRUE);
		mpSnapShot->DoModal();
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
		mbIsSetPartial = true;
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

//��ʾ�����ع�����
void CReaderBaseFrame::ShowToolBar(bool nbIsShow)
{
	if (nbIsShow == false)
	{
		//����
		mpToolbarH->GetIterator()->SetVisible(false);
		mpIterator->KillTimer(RBF_TIMER_TSHOW_TOOLBAR);
	}
	else
	{
		//��ʾ
		mpToolbarH->GetIterator()->SetVisible(true);
		mpIterator->KillTimer(RBF_TIMER_TSHOW_TOOLBAR);
		mpIterator->SetTimer(RBF_TIMER_TSHOW_TOOLBAR,1,5000,NULL);

		EiSetHomebarStatus(GI_HOMEBAR_COLLAPSE);
	}
}

void CReaderBaseFrame::ShowPageInfo(void)
{
	wchar_t lszString[MAX_PATH] = { 0 };
	ULONG pageNo1=0, pageNo2=0;

	pageNo1 = mpPdfPicture->GetCurrentPageNumber(pageNo2);
	if (pageNo1 == 0)
		pageNo1 = 1;

	{
		IConfigFile* lpProfile = NULL;
		wchar_t lszText[MAX_PATH] = { 0 };

		do
		{
			//��ȡ�������ַ���
			//Ϊ�˷��뷽�㣬�ַ��������root/string
			lpProfile = EinkuiGetSystem()->GetCurrentWidget()->GetDefaultFactory()->GetTempleteFile();
			ICfKey* lpCfKey = NULL;
			if (lpProfile != NULL)
			{
				if (pageNo2 != 0)
					lpCfKey = lpProfile->OpenKey(L"String2/Title2");
				else
					lpCfKey = lpProfile->OpenKey(L"String2/Title1");

				if (lpCfKey != NULL)
					lpCfKey->GetValue(lszText, MAX_PATH * sizeof(wchar_t));
				
			}

			CMM_SAFE_RELEASE(lpCfKey);

		} while (false);

		CMM_SAFE_RELEASE(lpProfile);
		

		if(pageNo2 != 0)
			swprintf_s(lszString, MAX_PATH, lszText, pageNo1,pageNo2, mpPdfPicture->GetPageCount());
		else
			swprintf_s(lszString, MAX_PATH, lszText, pageNo1, mpPdfPicture->GetPageCount()==0?1: mpPdfPicture->GetPageCount());

		mpToolbarH->SetPage(lszString);

		if (mulPageIndex != pageNo1)
		{
			//���¼�¼����
			mulPageIndex = pageNo1;

			
			PAGE_PDF_CONTEXT ldContent;
			if (mpPdfPicture->GetCrtPageContext(&ldContent) != false)
			{
				CHellFun::SetRegData(RbF_REG_PAGE_NUMBER, pageNo1);
				CHellFun::SetRegData(RbF_REG_PAGE_CONTENT, ldContent.pageContext);
				CHellFun::SetRegData(RbF_REG_PAGE_CONTENT2, ldContent.pageContext2);
			}
		}
		

	}
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
				mpIterToast->SetPosition(lfX, mpIterator->GetSizeY() - 200);

				mpIterToast->SetVisible(true);
				mpIterToast->BringToTop();
				mpIterator->SetTimer(RBF_TIMER_TOAST, 1, 3000, NULL);
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

//��Ҫ�򿪵��ļ�copy����ʱĿ¼
void CReaderBaseFrame::CopyFileToTemp(IN wchar_t* npszSrc, OUT wchar_t* npszDest, IN LONG nlLen)
{
	do 
	{
		BREAK_ON_NULL(npszSrc);
		BREAK_ON_NULL(npszDest);
		if (npszDest[0] != UNICODE_NULL)
		{
			wchar_t lszTemp[MAX_PATH] = { 0 };
			wcscpy_s(lszTemp, MAX_PATH, npszDest);
			lszTemp[wcslen(lszTemp) + 1] = UNICODE_NULL; //˫0��β
			//����Ѵ��ڣ���ɾ��
			SHFILEOPSTRUCT ldShfile;
			memset(&ldShfile, 0, sizeof(ldShfile));
			ldShfile.pFrom = lszTemp;
			ldShfile.fFlags = FOF_ALLOWUNDO | FOF_NOERRORUI | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;
			ldShfile.wFunc = FO_DELETE;
			SHFileOperation(&ldShfile);
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
			mpLoadingView->DoModal(NULL,lhCopyfileThread);
			mpLoadingView = NULL;
			CMM_SAFE_RELEASE(lpSubKey);

			CloseHandle(lhCopyfileThread);
		}

	} while (false);

}

//�û�ѡ����Ҫ�򿪵��ļ�
bool CReaderBaseFrame::OpenFile(wchar_t* npszFilePath)
{
	bool lbRet = false;

	do 
	{
		BREAK_ON_NULL(npszFilePath);
		mpPdfPicture->CloseFile();
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

		ULONG lulRet = mpPdfPicture->OpenFile(mszTempFile);
		mbIsSetPartial = true;
		if (lulRet != EDERR_SUCCESS/* && lulRet != EDERR_EMPTY_CONTENT*/)
		{
			if (mbIsTxt == false || GetFileSize(npszFilePath) > 0)
			{
				//���ļ�ʧ��
				ShowToast(L"OpenFileFail");
				mszSrcFile[0] = UNICODE_NULL;

				//CExMessage::PostMessage(mpIterator, mpIterator, EEVT_ER_OPEN_FILE, CExMessage::DataInvalid);

				break;
			}
		}
		mpIterToast->SetVisible(false);

		mpPdfPicture->GoToPage((IEdPage_ptr)NULL);
		mpPdfPicture->EnableDuopageView(mpToolbarH->GetDuopageStatus());

		ShowPageInfo();
		mpToolbarH->SetFileName(lpszFileName);

		//��¼����ʷ
		//�ȿ�������ǲ����Ѿ���¼����
		bool lbIsHave = false;
		for (int i=0;i<mdHistroyPath.Size();i++)
		{
			wchar_t* lpszPath = mdHistroyPath.GetEntry(i);
			if (_wcsicmp(npszFilePath, lpszPath) == 0)
			{
				//�Ѿ����ˣ������ᵽ��һ������
				mdHistroyPath.RemoveByIndex(i);
				mdHistroyPath.Insert(0, lpszPath);

				lbIsHave = true;
				break;
			}
		}

		if (lbIsHave == false)
		{
			//���ӵ��б�
			wchar_t* lpszPath = new wchar_t[MAX_PATH];
			wcscpy_s(lpszPath, MAX_PATH, npszFilePath);
			mdHistroyPath.Insert(0, lpszPath);

			if (mdHistroyPath.Size() > RBF_HISTROY_MAX)
			{
				//����������ˣ�ɾ�����һ��
				delete[] mdHistroyPath.GetEntry(mdHistroyPath.Size() - 1);
				mdHistroyPath.RemoveByIndex(mdHistroyPath.Size() - 1);
			}
		}

		//���浽ע���
		wchar_t lszText[MAX_PATH];
		DWORD ldwLen = 0;
		for (int i = 0; i < mdHistroyPath.Size(); i++)
		{
			swprintf_s(lszText, MAX_PATH, L"history%d", i);
			CHellFun::SetRegData(lszText, 0, mdHistroyPath.GetEntry(i));
		}

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
		ShowToolBar(false);
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

	if (mpZoomControl != NULL)
	{
		mpZoomControl->GetIterator()->SetSize((FLOAT)ldPaintSize.w, (FLOAT)ldPaintSize.h);
		if(mpZoomControl->GetIterator()->IsVisible() != false)
			mpZoomControl->SetFatRatio(mpPdfPicture->GetFatRatio());
	}

	if (mpZoomControlTxt != NULL)
	{
		mpZoomControlTxt->GetIterator()->SetSize((FLOAT)ldPaintSize.w, (FLOAT)ldPaintSize.h);
		/*if (mpZoomControlTxt->GetIterator()->IsVisible() != false)
			mpZoomControlTxt->SetFatRatio(mpPdfPicture->GetFatRatio());*/
	}

	if (mpPreNextButton != NULL)
	{
		mpPreNextButton->GetIterator()->SetSize((FLOAT)ldPaintSize.w, (FLOAT)ldPaintSize.h);
		//mpPreNextButton->SetRotation(nuOrient);
	}

	if (mpToolbarH != NULL)
		mpToolbarH->GetIterator()->SetSize((FLOAT)ldPaintSize.w, 60.0f);

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

		if (mpLoadingView != NULL)
			mpLoadingView->GetIterator()->SetSize(mpIterator->GetSize());

		if (mpToolbarH != NULL)
			mpToolbarH->GetIterator()->SetSize((FLOAT)ldPaintSize.w, 60.0f);

		if(mpIterBackground != NULL)
			mpIterBackground->SetSize(nNewSize);
		//mpTipFrame->GetIterator()->SetSize(nNewSize);
		SetOpenFilePos();
		SetJumpPagePos();

		if(mpSnapShot != NULL)
			mpSnapShot->GetIterator()->SetSize(mpIterator->GetSize());
	
		////���������ఴť��Ȳ�һ��
		//ULONG lulWidth = 600;
		//if (nNewSize.width > nNewSize.height)
		//	lulWidth = 600;
		//else
		//	lulWidth = 400;

		//mpIterLineTwo->SetSize(mpIterLineTwo->GetSize().width, nNewSize.height);
		//mpIterLineTwo->SetPosition(lulWidth, mpIterLineTwo->GetPositionY());

		//mpIterLineOne->SetSize(nNewSize.width, mpIterLineOne->GetSize().height);
		//mpIterBackground->SetSize(nNewSize);

		////mpIterBtGroup->SetSize(lulWidth, mpIterBtGroup->GetSizeY());
		//D2D1_RECT_F ldRect;
		//ldRect.left = 0;
		//ldRect.top = 0;
		//ldRect.right = lulWidth;
		//ldRect.bottom = 1000;
		//mpIterBtGroup->SetVisibleRegion(ldRect);

		//mpPicturePreview->GetIterator()->SetSize(nNewSize);

		//mpSettingHelp->GetIterator()->SetSize(nNewSize.width - lulWidth, nNewSize.height);
		//mpCoverSet->GetIterator()->SetSize(nNewSize.width - lulWidth, nNewSize.height);
		//mpCHaloKeyboard->GetIterator()->SetSize(nNewSize.width - lulWidth, nNewSize.height);

		//mpSettingHelp->GetIterator()->SetPosition(lulWidth, mpSettingHelp->GetIterator()->GetPositionY());
		//mpCoverSet->GetIterator()->SetPosition(lulWidth, mpCoverSet->GetIterator()->GetPositionY());
		//mpCHaloKeyboard->GetIterator()->SetPosition(lulWidth, mpCHaloKeyboard->GetIterator()->GetPositionY());

	} while (false);

	return ERESULT_SUCCESS;
}


//���ô��ļ����ڵ�λ��
void CReaderBaseFrame::SetOpenFilePos(void)
{
	if (mpFileOpenDlg != NULL)
	{
		EI_SIZE ldPaintSize;
		EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);

		FLOAT lfX = (ldPaintSize.w - mpFileOpenDlg->GetIterator()->GetSizeX()) / 2;
		lfX = lfX - mpIterator->GetPosition().x;
		D2D1_POINT_2F ldPos;
		ldPos.x = lfX;
		if (ldPaintSize.w > ldPaintSize.h)
		{
			ldPos.y = 100.0f;
		}
		else
		{
			ldPos.y = 482.0f;
		}
		mpFileOpenDlg->GetIterator()->SetPosition(ldPos);
	}

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
	wchar_t lszText[MAX_PATH] = { 0 };
	
	do 
	{
		
		DWORD ldwValue = 0;
		ldwRet = RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Lenovo\\Eink-PdfReader", 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &lhKey);
		if (ldwRet == ERROR_SUCCESS)
		{
			//��ȡ��ʷ��¼
			for (int i=0;i<RBF_HISTROY_MAX; i++)
			{
				swprintf_s(lszText, MAX_PATH, L"history%d", i);
				ldwLen = MAX_PATH * sizeof(wchar_t);
				if (RegQueryValueEx(lhKey, lszText, NULL, NULL, (BYTE*)&lszText, &ldwLen) == ERROR_SUCCESS)
				{
					wchar_t* lpszPath = new wchar_t[MAX_PATH];
					wcscpy_s(lpszPath, MAX_PATH, lszText);
					mdHistroyPath.Insert(-1, lpszPath);
				}
			}

			
		}

		if (mdHistroyPath.Size() > 0)
		{
			//�ϴο����ڼ�ҳ��
			ldwLen = sizeof(DWORD);
			DWORD ldwPageNumber = 1;
			RegQueryValueEx(lhKey, RbF_REG_PAGE_NUMBER, NULL, NULL, (BYTE*)&ldwPageNumber, &ldwLen);

			DWORD ldwPageContent = 0;
			RegQueryValueEx(lhKey, RbF_REG_PAGE_CONTENT, NULL, NULL, (BYTE*)&ldwPageContent, &ldwLen);
			DWORD ldwPageContent2 = 0;
			RegQueryValueEx(lhKey, RbF_REG_PAGE_CONTENT2, NULL, NULL, (BYTE*)&ldwPageContent2, &ldwLen);

			//�ϴ����õ�txt�ֺ�
			mdwFontsizeIndex = 1;
			RegQueryValueEx(lhKey, RbF_REG_TXT_FONT_SIZE_INDEX, NULL, NULL, (BYTE*)&mdwFontsizeIndex, &ldwLen);
			if (mdwFontsizeIndex > ZCT_FONTSIZE_LEVEL || mdwFontsizeIndex < 0)
				mdwFontsizeIndex = 1;

			//˵���򿪹��ļ�,�ָ��ϴδ�״̬
			mpPdfPicture->SetFontSize(mdwFontSizeArray[mdwFontsizeIndex]);
			if (GetFileAttributes(mdHistroyPath.GetEntry(0)) == INVALID_FILE_ATTRIBUTES)
			{
				//�ļ���������
				CExMessage::PostMessage(mpIterator, mpIterator, EEVT_ER_OPEN_FILE, CExMessage::DataInvalid);
				break;
			}
			wcscpy_s(mszSrcFile, MAX_PATH, mdHistroyPath.GetEntry(0));
			CopyFileToTemp(mdHistroyPath.GetEntry(0), mszTempFile, MAX_PATH);
			

			//�ļ���
			wchar_t* lpszFileName = wcsrchr(mdHistroyPath.GetEntry(0), L'\\');
			if (lpszFileName == NULL)
				lpszFileName = mdHistroyPath.GetEntry(0);
			else
				lpszFileName = lpszFileName + 1;

			if (lpszFileName[wcslen(lpszFileName) - 1] == 't')
				mbIsTxt = true;
			else
				mbIsTxt = false;

			PAGE_PDF_CONTEXT ldPageContent;
			ldPageContent.pageContext = ldwPageContent;
			ldPageContent.pageContext2 = ldwPageContent2;
			ldPageContent.pageIndex = ldwPageNumber;
			mulPageIndex = 0;
			ULONG lulRet = mpPdfPicture->OpenFile(mszTempFile, &ldPageContent);
			mbIsSetPartial = true;
			if (lulRet != EDERR_SUCCESS/* && lulRet != EDERR_EMPTY_CONTENT*/)
			{
				if (mbIsTxt == false || GetFileSize(mdHistroyPath.GetEntry(0)) > 0)
				{
					//��ʧ���ˣ������ļ���������
					mszSrcFile[0] = UNICODE_NULL;
					CExMessage::PostMessage(mpIterator, mpIterator, EEVT_ER_OPEN_FILE, CExMessage::DataInvalid);
					break;
				}
			}

			//�Ƿ�˫��
			ldwLen = sizeof(DWORD);
			RegQueryValueEx(lhKey, RbF_REG_DOUBLE_SCREEN, NULL, NULL, (BYTE*)&ldwValue, &ldwLen);
			mpPdfPicture->EnableDuopageView(ldwValue==0?false:true);
			mpToolbarH->SetDuopageButton(ldwValue == 0 ? false : true);

			EI_SIZE ldPaintSize;
			EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);
			if (ldPaintSize.w > ldPaintSize.h)
			{
				mpPdfPicture->EnableDuopageView(ldwValue == 0 ? false : true);
				mpToolbarH->SetDuopageButton(ldwValue == 0 ? false : true);
			}
			else
			{
				mpToolbarH->SetDuopageButton(false);
				mpPdfPicture->EnableDuopageView(false);
			}

			//���ܱ任����������������λ�ã��м�
			if (ldwPageNumber > 1 && ldwPageContent == 0 && ldwPageContent2 == 0)
			{
				mpPdfPicture->GoToPage(ldwPageNumber);
			}
			else
			{
				//ֻ��txt��ʽ�ļ������������ַ�ʽ
				mpPdfPicture->GoToPage((IEdPage_ptr)NULL);
			}

			ShowPageInfo();
			mpToolbarH->SetFileName(lpszFileName);
		}
		else
		{
			//˵���ǵ�һ�δ򿪣�ֱ�ӵ������ļ��������ľͲ���Ҫ��ʼ����
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

		if ((nxNumber - npThis->mxLastGcUiNumber >= 10 && luCrtTick - npThis->muLastGcTick > 1000*5) || luCrtTick - npThis->muLastGcTick > 1000 * 60 * 5)	// ʮ֡���������
		{
			// ���ý���GCģʽ ???niu
			EinkuiGetSystem()->ClearEinkBuffer();
			EiSetPartialUpdate(FALSE);

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

				npThis->mbGcMode = false;
			}
		}

	} while (false);
	

	return ERESULT_SUCCESS;
}
