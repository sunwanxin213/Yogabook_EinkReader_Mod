/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "ToolbarH.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"
#include "ReaderBaseFrame.h"


DEFINE_BUILTIN_NAME(ToolbarH)

CToolbarH::CToolbarH(void)
{
	mpIterBackground = NULL;
	mpIterBtThumbnail = NULL;
	mbIsTwoScreen = false;
	//mbIsLCDClose = true;
	miDocType = -1;
	mpMenuTxt = NULL;
	mpMenuPdf = NULL;
	mpMenuEpubMobi = NULL;
	miCurrentPageInkCount = 0;
	mdwAttrib = 0;
	mbIsHScreen = false;
	mpEditToolbar = NULL;
	mulScreenOritent = 10;
}


CToolbarH::~CToolbarH(void)
{
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CToolbarH::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		mpIterator->ModifyStyles(EITR_STYLE_POPUP);
		
		//mpIterPicture->SetRotation(90.0f);

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}


// ��������
ERESULT CToolbarH::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	ERESULT luResult = ERESULT_SUCCESS;

	if (!(rPoint.x < 0.0f || rPoint.x >= mpIterator->GetSizeX()
		|| rPoint.y < 0.0f || rPoint.y >= mpIterator->GetSizeY()))
	{
		luResult = ERESULT_MOUSE_OWNERSHIP;
	}

	return luResult;
}

ULONG CToolbarH::InitOnCreate(
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


		//��ȡ������
		mpIterBtFileOpen = mpIterator->GetSubElementByID(TBH_BT_OPEN_FILE);
		BREAK_ON_NULL(mpIterBtFileOpen);

		mpIterBtThumbnail = mpIterator->GetSubElementByID(TBH_BT_THUMBNAIL);
		BREAK_ON_NULL(mpIterBtThumbnail);


		mpIterBackground = mpIterator->GetSubElementByID(1);
		BREAK_ON_NULL(mpIterBackground);
		//mpIterBackground->SetVisible(false);
		
		mpIterBtMore = mpIterator->GetSubElementByID(TBH_BT_MORE);
		BREAK_ON_NULL(mpIterBtMore);

		//�༭������
		lpSubKey = mpTemplete->OpenKey(L"EditToolbar");
		mpEditToolbar = CEditToolbar::CreateInstance(mpIterator, lpSubKey);
		CMM_SAFE_RELEASE(lpSubKey);
		BREAK_ON_NULL(mpEditToolbar);

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

//txt�ĸ���˵�
void CToolbarH::ShowTxtMoreMenu()
{
	if (mpMenuTxt != NULL)
	{
		mpMenuTxt->ExitModal();
		mpMenuTxt = NULL;
	}

	ICfKey* lpSubKey = mpTemplete->OpenKey(L"MenuTxt");
	mpMenuTxt = CMenuTxt::CreateInstance(mpIterator, lpSubKey);
	EI_SIZE ldPaintSize;
	EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);
	mpMenuTxt->GetIterator()->SetSize(float(ldPaintSize.w), float(ldPaintSize.h));
	mpMenuTxt->SetTxtFontSizeIndex(mdwTxtFontSizeIndex);
	mpMenuTxt->SetScreenOritent(mulScreenOritent);
	//mpMenuTxt->SetLCDButton(mbIsLCDClose);
	mpMenuTxt->DoModal();

	CMM_SAFE_RELEASE(lpSubKey);
	mpMenuTxt = NULL;
}

//pdf�ĸ���˵�
void CToolbarH::ShowPDFMoreMenu()
{
	if (mpMenuPdf != NULL)
	{
		mpMenuPdf->ExitModal();
		mpMenuPdf = NULL;
	}

	ICfKey* lpSubKey = mpTemplete->OpenKey(L"MenuPDF");
	mpMenuPdf = CMenuPdf::CreateInstance(mpIterator, lpSubKey);
	EI_SIZE ldPaintSize;
	EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);
	mpMenuPdf->GetIterator()->SetSize(float(ldPaintSize.w), float(ldPaintSize.h));

	auto zoomStatus = ((CReaderBaseFrame*)GetIterator()->GetParent()->GetElementObject())->GetZoomStatus();
	mpMenuPdf->SetPageTwo(mbIsTwoScreen, mbIsHScreen && (zoomStatus != ZoomStatus::AUTO_ZOOM));

	mpMenuPdf->SetScreenOritent(mulScreenOritent);
	mpMenuPdf->SetCurrentPageInkCount(miCurrentPageInkCount);
	//mpMenuPdf->SetLCDButton(mbIsLCDClose);

	//�����˵�ʱ�͹ر�����
	CExMessage::SendMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_HIDE_HIGHLIGHT, CExMessage::DataInvalid);

	ULONG lulOldMode = PEN_MODE_NONE;
	CExMessage::SendMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_PEN_MODE, PEN_MODE_NONE,&lulOldMode,sizeof(ULONG));
	mpMenuPdf->DoModal();
	if(lulOldMode != PEN_MODE_NONE)
		CExMessage::SendMessage(EinkuiGetSystem()->GetCurrentWidget()->GetHomePage(), mpIterator, EEVT_PEN_MODE, lulOldMode);

	CMM_SAFE_RELEASE(lpSubKey);
	mpMenuPdf = NULL;
}

//epub/mobi�ĸ���˵�
void CToolbarH::ShowEpubMobiMoreMenu()
{
	if (mpMenuEpubMobi != NULL)
	{
		mpMenuEpubMobi->ExitModal();
		mpMenuEpubMobi = NULL;
	}

	ICfKey* lpSubKey = mpTemplete->OpenKey(L"MenuEpub");
	mpMenuEpubMobi = CMenuEpubMobi::CreateInstance(mpIterator, lpSubKey);
	EI_SIZE ldPaintSize;
	EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);
	mpMenuEpubMobi->GetIterator()->SetSize(float(ldPaintSize.w), float(ldPaintSize.h));
	mpMenuEpubMobi->SetPageTwo(mbIsTwoScreen, mbIsHScreen);
	mpMenuEpubMobi->SetScreenOritent(mulScreenOritent);
	//mpMenuEpubMobi->SetLCDButton(mbIsLCDClose);
	mpMenuEpubMobi->DoModal();

	CMM_SAFE_RELEASE(lpSubKey);
	mpMenuEpubMobi = NULL;

}

void CToolbarH::HideMoreMenu()
{
	if (mpMenuEpubMobi != nullptr)
	{
		mpMenuEpubMobi->ExitModal();
		mpMenuEpubMobi = nullptr;
	}

	if (mpMenuPdf != nullptr)
	{
		mpMenuPdf->ExitModal();
		mpMenuPdf = nullptr;
	}

	if (mpMenuTxt != nullptr)
	{
		mpMenuTxt->ExitModal();
		mpMenuTxt = nullptr;
	}
}

//���ñ�״̬��ťѡ��
void CToolbarH::SetPenStatusSelect(ULONG nulID)
{
	if(mpEditToolbar != NULL && nulID != PEN_MODE_NONE)
		mpEditToolbar->SetSelect(nulID);
}

//�ĵ��������
void CToolbarH::DocmentLoadComplete(void)
{
	
}

//����ͼ�������
void CToolbarH::ThumbnailsLoadComplete(void)
{
	if (mpIterBtThumbnail != NULL)
		mpIterBtThumbnail->SetVisible(miDocType == DOC_TYPE_TXT ? false : true);
}

//bool CToolbarH::GetBCoverState()
//{
//	bool BCoverisLightorNot = mbIsLCDClose;
//	return BCoverisLightorNot;
//}
//����ע�����BCoverState��ֵ�趨�Ƿ�ر�B��
//void CToolbarH::SetBCoverState()
//{
//	UpdateBCoverState();
//	EiCloseBCover(!mbIsLCDClose);
//}

//void CToolbarH::UpdateBCoverState()
//{
//	BOOL dwState = true;
//	DWORD dwresult = 1;
//	DWORD dwSize = sizeof(DWORD);
//	HKEY lhKey = NULL;
//	DWORD ldwRet = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Lenovo\\Eink-PdfReader", NULL, KEY_READ | KEY_WOW64_64KEY, &lhKey);
//	if (ldwRet == ERROR_SUCCESS)
//	{
//		ldwRet = RegQueryValueEx(lhKey, L"BCoverState", NULL, NULL, (LPBYTE)&dwresult, &dwSize);
//		if (dwresult == 0)
//		{
//			dwState = false;
//		}
//	}
//	RegCloseKey(lhKey);
//	mbIsLCDClose = dwState;
//}

//���õ�ǰ�򿪵��ļ�����
void CToolbarH::SetDoctype(int niType)
{
	if(mpIterBtThumbnail != NULL)
		mpIterBtThumbnail->SetVisible(false);

	if (miDocType != niType)
	{
		miDocType = niType;

		if (mpEditToolbar != NULL)
		{
			mpEditToolbar->GetIterator()->SetVisible(miDocType == DOC_TYPE_PDF ? true : false);
			if (miDocType == DOC_TYPE_PDF)
			{
				if ((mdwAttrib&FILE_ATTRIBUTE_READONLY) != false)
				{
					mpEditToolbar->GetIterator()->SetVisible(false);
				}				
			}
			

		}
		
		Relocation();
	}

	if (mpEditToolbar != NULL && mpEditToolbar->GetIterator()->IsVisible() != false)
	{
		mpEditToolbar->init();
	}

}

//�����ļ�����
void CToolbarH::SetFileAttrib(DWORD ndwAttrib)
{
	if (mdwAttrib != ndwAttrib)
		miDocType = -1;

	mdwAttrib = ndwAttrib;
}

//��ȡ�ļ�����
DWORD CToolbarH::GetFileAttrib(void)
{
	return mdwAttrib;
}

//��ť�����¼�
ERESULT CToolbarH::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		ULONG llBtnID = npSender->GetID();
		switch (llBtnID)
		{
		case TBH_BT_OPEN_FILE:
		{
			//���ļ��Ի���
			PostMessageToParent(EEVT_ER_OPEN_FILE, CExMessage::DataInvalid);

			//mpIterator->SetVisible(false);
			break;
		}
		case TBH_BT_THUMBNAIL:
		{
			//���ļ��Ի���
			PostMessageToParent(EEVT_ENTER_THUMBNAIL_DLG, CExMessage::DataInvalid);

			//mpIterator->SetVisible(false);
			break;
		}
		case TBH_BT_MORE:
		{
			//����˵�
			if (miDocType == DOC_TYPE_PDF)
			{
				if((mdwAttrib&FILE_ATTRIBUTE_READONLY) == false)
					ShowPDFMoreMenu();
				else
					ShowEpubMobiMoreMenu(); //����ļ���ֻ���ģ��˵��ͺ�epub mobiһ��
			}
			else if (miDocType == DOC_TYPE_EPUB || miDocType == DOC_TYPE_MOBI)
				ShowEpubMobiMoreMenu();
			else if (miDocType == DOC_TYPE_TXT)
				ShowTxtMoreMenu();

			EinkuiGetSystem()->UpdateView(true);
			

			//mpIterator->SetVisible(false);
			break;
		}
		//case TBH_BT_TWO:
		//{
		//	//˫����ʾ
		//	PostMessageToParent(EEVT_ER_TWO_SCREEN, true);
		//	

		//	SetDuopageButton(true);
		//	break;
		//}
		//case TBH_BT_ONE_PIC:
		//{
		//	//������ʾ
		//	PostMessageToParent(EEVT_ER_TWO_SCREEN, false);

		//	SetDuopageButton(false);
		//	//mpIterBtTwo->SetVisible(true);
		//	//mpIterBtOne->SetVisible(false);

		//	break;
		//}
		//case TBH_BT_ZOOM:
		//{
		//	//����
		//	PostMessageToParent(EEVT_ER_ENTER_ZOOM, true);

		//	mpIterator->SetVisible(false);

		//	break;
		//}
		//case TBH_BT_SNAPSHOT:
		//{
		//	//����
		//	PostMessageToParent(EEVT_ER_ENTER_SNAPSHOT, CExMessage::DataInvalid);

		//	mpIterator->SetVisible(false);
		//	break;
		//}
		default:
			break;
		}


		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}

//����txt�ֺ�
void CToolbarH::SetTxtFontSizeIndex(DWORD ndwIndex)
{
	mdwTxtFontSizeIndex = ndwIndex;
}

//��Ϣ������
ERESULT CToolbarH::ParseMessage(IEinkuiMessage* npMsg)
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
	case EEVT_ER_ENTER_SNAPSHOT:	
	{
		//����
		//PostMessageToParent(EEVT_SHOW_TOOLBAR, false);
		PostMessageToParent(EEVT_ER_ENTER_SNAPSHOT, CExMessage::DataInvalid);
		//PostMessageToParent(EEVT_SHOW_TOOLBAR, true);

		break;
	}
	case EEVT_ER_SET_TXT_ZOOM:
	{
		//����txt�Ŵ�
		luResult = CExMessage::GetInputData(npMsg, mdwTxtFontSizeIndex);
		if (luResult != ERESULT_SUCCESS)
			break;

		PostMessageToParent(EEVT_ER_SET_TXT_ZOOM, mdwTxtFontSizeIndex);
	}
	case EEVT_ER_TWO_SCREEN:
	{
		//��˫ҳ
		luResult = CExMessage::GetInputData(npMsg, mbIsTwoScreen);
		if (luResult != ERESULT_SUCCESS)
			break;

		PostMessageToParent(EEVT_ER_TWO_SCREEN, mbIsTwoScreen);

		break;
	}
	case EEVT_CLOSE_B_SCREEN:
	{
		//�رջ�����B��
		//luResult = CExMessage::GetInputData(npMsg, mbIsLCDClose);
		//if (luResult != ERESULT_SUCCESS)
		//	break;

		//PostMessageToParent(EEVT_CLOSE_B_SCREEN, mbIsLCDClose);

		break;
	}
	case EEVT_SET_SCREEN_STATUS:
	{
		//��Ļ����
		luResult = CExMessage::GetInputData(npMsg, mulScreenOritent);
		if (luResult != ERESULT_SUCCESS)
			break;

		PostMessageToParent(EEVT_SET_SCREEN_STATUS, mulScreenOritent);

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

//��ʱ��
void CToolbarH::OnTimer(
	PSTEMS_TIMER npStatus
	)
{

}

//���¶�λԪ��λ��
void CToolbarH::Relocation(void)
{
	if (mpIterBackground != NULL)
	{
		EI_SIZE ldPaintSize;

		EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);
		CExMessage::SendMessageWithText(mpIterBackground, mpIterator, EACT_PICTUREFRAME_CHANGE_PIC, L".\\Pic\\toolbar_h.png");
		mpIterBackground->SetSize((float)ldPaintSize.w, 80.0f);

		D2D1_POINT_2F ldPos;
		ldPos.y = 0.0f;

		if (mpMenuPdf != NULL)
			mpMenuPdf->GetIterator()->SetSize(float(ldPaintSize.w), float(ldPaintSize.h));
		if (mpMenuTxt != NULL)
			mpMenuTxt->GetIterator()->SetSize(float(ldPaintSize.w), float(ldPaintSize.h));
		if (mpMenuEpubMobi != NULL)
			mpMenuEpubMobi->GetIterator()->SetSize(float(ldPaintSize.w), float(ldPaintSize.h));

		if (ldPaintSize.w > ldPaintSize.h)
		{
			//����
			mbIsHScreen = true;

			//���ఴť
			if (miDocType == DOC_TYPE_PDF && (mdwAttrib&FILE_ATTRIBUTE_READONLY) == false)
				ldPos.x = 1290.0f;
			else
				ldPos.x = ldPaintSize.w - mpIterBtMore->GetSizeX() - 120.0f;

			mpIterBtMore->SetPosition(ldPos);
			mpEditToolbar->GetIterator()->SetPosition(630.0f, mpEditToolbar->GetIterator()->GetPositionY());



			/*ldPos.x = 1564.0f;
			mpIterBtSnap->SetPosition(ldPos);

			ldPos.x = 1464.0f;
			mpIterBtJump->SetPosition(ldPos);

			ldPos.x = 1364.0f;
			mpIterBtTwo->SetPosition(ldPos);
			if(mbIsTxt == false && mbIsTwoScreen == false)
			mpIterBtTwo->SetVisible(true);

			mpIterBtOne->SetPosition(ldPos);
			if (mbIsTxt == false && mbIsTwoScreen != false)
			mpIterBtOne->SetVisible(true);

			CExMessage::SendMessage(mpIterFileName, mpIterator, EACT_LABEL_SET_MAX_WIDTH, 900);

			ldActionSize.width = 100.0f;
			ldActionSize.height = 80.0f;
			CExMessage::SendMessage(mpIterBtFileOpen, mpIterator, EACT_BUTTON_SET_ACTION_RECT, ldActionSize);*/

		}
		else
		{
			//����
			mbIsHScreen = false;
			
			//���ఴť
			if (miDocType == DOC_TYPE_PDF && (mdwAttrib&FILE_ATTRIBUTE_READONLY) == false)
				ldPos.x = 865.0f;
			else
				ldPos.x = ldPaintSize.w - mpIterBtMore->GetSizeX() - 120.0f;

			mpIterBtMore->SetPosition(ldPos);
			mpEditToolbar->GetIterator()->SetPosition(230.0f, mpEditToolbar->GetIterator()->GetPositionY());

		}

		//mpIterPage->SetPosition(mpIterFileName->GetPositionX() + mpIterFileName->GetSizeX() + 20.0f, mpIterPage->GetPositionY());
	}
}

//Ԫ�زο��ߴ緢���仯
ERESULT CToolbarH::OnElementResized(D2D1_SIZE_F nNewSize)
{
	//CExMessage::SendMessage(mpIterBtFull, mpIterator, EACT_BUTTON_SET_ACTION_RECT, nNewSize);
	////mpIterLineOne->SetSize(nNewSize.width, mpIterLineOne->GetSize().height);

	//mpIterBtOk->SetPosition(nNewSize.width - 85, mpIterBtOk->GetPositionY());
	Relocation();

	return ERESULT_SUCCESS;
}

// ���ñʼ���ʼ������
void CToolbarH::SetPenData(DWORD ndwPenWidthIndex, DWORD ndwPenColorIndex)
{
	if (mpEditToolbar != NULL)
		mpEditToolbar->SetData(ndwPenWidthIndex, ndwPenColorIndex);
}

//����ҳ��״̬
void CToolbarH::UpdatePageStatus(PAGE_STATUS ndStatus)
{
	miCurrentPageInkCount = ndStatus.InkCount;

	SetUndoRedoStatus(ndStatus.UndoCount > 0 ? true : false, ndStatus.RedoCount > 0 ? true : false);
}


//����redo,undo��ť״̬
void CToolbarH::SetUndoRedoStatus(bool nbUndoEnable, bool nbRedoEnable)
{
	if (mpEditToolbar != NULL)
		mpEditToolbar->SetUndoRedoStatus(nbUndoEnable, nbRedoEnable);
}

//���õ�ǰҳ���ע����
void CToolbarH::SetCurrentPageInkCount(int niCount)
{
	miCurrentPageInkCount = niCount;
}

//��ȡ��ǰ˫ҳ��ʾ״̬
bool CToolbarH::GetDuopageStatus(void)
{
	bool lbRet = mbIsTwoScreen;
	EI_SIZE ldPaintSize;
	EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);

	if (miDocType == DOC_TYPE_TXT || ldPaintSize.w < ldPaintSize.h)
		lbRet = false; //txtû��˫��,����ʱû��˫��

	return lbRet;
}

void CToolbarH::SetDuopageButton(bool nbSingle)
{
	mbIsTwoScreen = nbSingle;
}

//������Ļ����ť״̬
void CToolbarH::SetScreenOriButton(ULONG nulScreenOri)
{
	mulScreenOritent = nulScreenOri;
}

//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CToolbarH::OnElementShow(bool nbIsShow)
{
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);
	if (nbIsShow != false)
	{
		mpIterator->BringToTop();
	}
	else
	{
		if (mpMenuEpubMobi != NULL)
			mpMenuEpubMobi->ExitModal();
		if (mpMenuPdf != NULL)
			mpMenuPdf->ExitModal();
		if (mpMenuTxt != NULL)
			mpMenuTxt->ExitModal();
	}
	if (mpEditToolbar != NULL)
		mpEditToolbar->ToolbarShow(nbIsShow);

	return ERESULT_SUCCESS;
}