/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "ThumbnailDlg.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"
#include "PreNextButton.h"
#include "..\ECtl\EvButtonImp.h"

DEFINE_BUILTIN_NAME(ThumbnailDlg)

CThumbnailDlg::CThumbnailDlg(void)
{
	mpMenuThumbnail = NULL;
	mpIterBtPageNumber = NULL;
	mpIteratorPre = NULL;
	mpIteratorNext = NULL;
	mpIteratorSelect = NULL;

	miDocType = DOC_TYPE_PDF;
	mpPageProgress = NULL;

	mpDocument = NULL;

	mulCurrentPage = 0;
	mulDocMaxPage = 0;
	mulMaxPage = 0;
	mulAnnotPage = 0;
	mulCurrentDoc = 1;

	mszPageAll[0] = UNICODE_NULL;
	mszPageAnnot[0] = UNICODE_NULL;

	mulSelectType = THUMBNAIL_SELECT_ALL;
}


CThumbnailDlg::~CThumbnailDlg(void)
{
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CThumbnailDlg::OnElementCreate(IEinkuiIterator* npIterator)
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


ULONG CThumbnailDlg::InitOnCreate(
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
		leResult = CXuiElement::InitOnCreate(npParent, npTemplete, nuEID);
		if (leResult != ERESULT_SUCCESS)
			break;


		//��ȡ������
		mpIterBtPageNumber = mpIterator->GetSubElementByID(TD_BT_NUMBER);
		BREAK_ON_NULL(mpIterBtPageNumber);

		mpIteratorPre = mpIterator->GetSubElementByID(TD_BT_PRE);
		BREAK_ON_NULL(mpIteratorPre);

		mpIteratorNext = mpIterator->GetSubElementByID(TD_BT_NEXT);
		BREAK_ON_NULL(mpIteratorNext);

		mpIteratorSelect = mpIterator->GetSubElementByID(TD_BT_SELECT);
		BREAK_ON_NULL(mpIteratorSelect);

		//ҳ����ת
		lpSubKey = mpTemplete->OpenKey(L"PageProgress");
		mpPageProgress = CPageProgress::CreateInstance(mpIterator, lpSubKey);
		CMM_SAFE_RELEASE(lpSubKey);
		BREAK_ON_NULL(mpPageProgress);
		mpPageProgress->GetIterator()->SetVisible(false);


		//����list����
		lpSubKey = mpTemplete->OpenKey(L"ListItem");
		for (int i = 0; i < TD_ITEM_MAX; i++)
		{
			CThumbnailListItem* lpListItem = CThumbnailListItem::CreateInstance(mpIterator, lpSubKey);
			//lpListItem->GetIterator()->SetVisible(false);

			mdList.Insert(i, lpListItem);
		}
		CMM_SAFE_RELEASE(lpSubKey);


		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

//��ʾ������Ԫ��
void CThumbnailDlg::ShowItem(bool nbIsShow)
{
	mpIterator->SetVisible(nbIsShow);
		
}

//��ť�����¼�
ERESULT CThumbnailDlg::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{

		PostMessageToParent(EEVT_RESET_HIDE_TIME, true);

		ULONG llBtnID = npSender->GetID();
		switch (llBtnID)
		{
		case TD_BT_PRE:
		{
			//��һҳ
			NextPage(false);

			//mpIterator->SetVisible(false);
			break;
		}
		case TD_BT_NEXT:
		{
			//��һҳ
			NextPage(true);

			//mpIterator->SetVisible(false);
			break;
		}
		case TD_BT_NUMBER:
		{
			//����ҳ��
			//PostMessageToParent(EEVT_ER_TWO_SCREEN, true);
			if (mpPageProgress != NULL)
			{
				mpPageProgress->SetData((FLOAT)mulCurrentPage, (FLOAT)mulMaxPage);
				mpPageProgress->GetIterator()->SetVisible(!mpPageProgress->GetIterator()->IsVisible());
			}
				

			break;
		}
		case TD_BT_SELECT:
		{
			if (mpMenuThumbnail != NULL)
			{
				mpMenuThumbnail->ExitModal();
				mpMenuThumbnail = NULL;
			}

			ICfKey* lpSubKey = mpTemplete->OpenKey(L"Menu");
			mpMenuThumbnail = CMenuThumbnail::CreateInstance(mpIterator, lpSubKey);
			EI_SIZE ldPaintSize;
			EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);
			mpMenuThumbnail->GetIterator()->SetSize(float(ldPaintSize.w), float(ldPaintSize.h));
			mpMenuThumbnail->SetData(mulSelectType, mszPageAll, mszPageAnnot);
			mpMenuThumbnail->DoModal();

			CMM_SAFE_RELEASE(lpSubKey);
			mpMenuThumbnail = NULL;

			EinkuiGetSystem()->UpdateView(true);

			break;

		}
		case TD_BT_CLOSE:
		{
			ExitModal();
			break;
		}
		default:
			break;
		}


		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}


//������Ϣ
ERESULT CThumbnailDlg::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npPaintBoard);

		if (mpBgBitmap != NULL)
			npPaintBoard->DrawBitmap(D2D1::RectF(0, 0, mpIterator->GetSizeX(), mpIterator->GetSizeY()),
				mpBgBitmap,
				ESPB_DRAWBMP_EXTEND);

		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}

//��Ϣ������
ERESULT CThumbnailDlg::ParseMessage(IEinkuiMessage* npMsg)
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
	case EEVT_ER_PAGE_JUMP:
	{
		//ҳ����ת
		int liPage = 1;
		luResult = CExMessage::GetInputData(npMsg, liPage);
		if (luResult != ERESULT_SUCCESS)
			break;

		EnterPage(liPage);

		mpIteratorPre->SetEnable(mulCurrentPage <= 1 ? false : true);
		mpIteratorNext->SetEnable(mulCurrentPage < mulMaxPage ? true : false);

		break;
	}
	case EEVT_THUMBNAIL_CLICK:
	{
		//����ͼ�����
		int liPage = 1;
		luResult = CExMessage::GetInputData(npMsg, liPage);
		if (luResult != ERESULT_SUCCESS)
			break;

		SendMessageToParent(EEVT_ER_PAGE_JUMP, liPage,NULL,0);
		ExitModal();

		break;
	} 
	case EEVT_THUMBNAIL_SELECT:
	{
		luResult = CExMessage::GetInputData(npMsg, mulSelectType);
		if (luResult != ERESULT_SUCCESS)
			break;

		if (mpPageProgress != NULL)
			mpPageProgress->GetIterator()->SetVisible(false);

		SelectAll(mulSelectType == THUMBNAIL_SELECT_ALL ? true : false);


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
void CThumbnailDlg::OnTimer(
	PSTEMS_TIMER npStatus
	)
{

}

//���õ�ǰ�򿪵��ļ�����
void CThumbnailDlg::SetDoctype(int niType)
{
	//if (miDocType != niType)
	{

		miDocType = niType;

		//�ر���ҳ������
		mpPageProgress->GetIterator()->SetVisible(false);
		
		if (miDocType == DOC_TYPE_PDF)
		{
			mpIteratorSelect->SetVisible(true);
		}
		else
		{
			mpIteratorSelect->SetVisible(false);
		}
	}
	
}

void CThumbnailDlg::DoModal()
{
	do
	{
		EiSetHomebarStatus(GI_HOMEBAR_HIDE);
		EiSetBatteryStatus(GI_BATTERY_HIDE);
		mpIterator->SetActive();
		mpIterator->BringToTop();
		EinkuiGetSystem()->EnablePaintboard(false);
		EinkuiGetSystem()->UpdateView(true);
		EinkuiGetSystem()->DoModal(mpIterator);

		EiSetHomebarStatus(GI_HOMEBAR_SHOW);
		EiSetBatteryStatus(GI_BATTERY_SHOW);
		mpIterator->Close();
		//mpIterator->Release();

	} while (false);
}

void CThumbnailDlg::ExitModal()
{
	EinkuiGetSystem()->ExitModal(mpIterator, 0);

}

//����ȫ����ɸѡ
void CThumbnailDlg::SelectAll(bool nbIsAll)
{
	do 
	{
		//�������ҳ��
		ULONG lulThumbnailPage = 0;
		if (nbIsAll != false)
		{
			
			lulThumbnailPage = mulDocMaxPage;
			CExMessage::SendMessageWithText(mpIteratorSelect, mpIterator, EACT_BUTTON_SETTEXT, mszPageAll);
		}
		else
		{
			lulThumbnailPage = mulAnnotPage;

			CExMessage::SendMessageWithText(mpIteratorSelect, mpIterator, EACT_BUTTON_SETTEXT, mszPageAnnot);

			if (mdAnnotPageNumber.Size() <= 0 && lulThumbnailPage > 0)
			{
				for (ULONG i=0;i<mulDocMaxPage;i++)
				{
					wchar_t lpszFolderPath[MAX_PATH] = { 0 };
					bool lbIsAnnot = false;
					mpDocument->GetThumbnailPathName(i, lpszFolderPath, &lbIsAnnot);
					if (lbIsAnnot != false)
					{
						mdAnnotPageNumber.Insert(-1, i);
						if(ULONG(mdAnnotPageNumber.Size()) >= lulThumbnailPage)
							break; //�ҹ��ˣ�����Ҫ��������
					}
				}
			}
		}
			
		mulMaxPage = lulThumbnailPage / TD_ITEM_MAX;
		if (mulMaxPage*TD_ITEM_MAX < lulThumbnailPage)
			mulMaxPage++; //ҳ�����ʹ�ý�1��

		mpIteratorPre->SetEnable(false);
		mpIteratorNext->SetEnable(false);
		if (mulMaxPage > 1)
		{
			mpIteratorNext->SetEnable(true);
		}

		EnterPage(1);

	} while (false);
}

//�����ĵ�����ָ��
void CThumbnailDlg::SetPdfPicture(CPdfPicture* npPdfPicture)
{
	do 
	{
		BREAK_ON_NULL(npPdfPicture);

		mpDocument = npPdfPicture->GetDoc();
		BREAK_ON_NULL(mpDocument);

		mulAnnotPage = mpDocument->GetAnnotPageCount();
		mulDocMaxPage = (ULONG)mpDocument->GetPageCount();

		ULONG lulS = 0;
		mulCurrentDoc = npPdfPicture->GetCurrentPageNumber(lulS);
		ULONG lulIndex = int(mulCurrentDoc / TD_ITEM_MAX) + 1;

		GetTranslateString(mulDocMaxPage, mulAnnotPage);

		SelectAll(true);

		//���뵱ǰҳ����������ͼ
		if (lulIndex > 1)
		{
			EnterPage(lulIndex);
			mpIteratorPre->SetEnable(mulCurrentPage <= 1 ? false : true);
			mpIteratorNext->SetEnable(mulCurrentPage < mulMaxPage ? true : false);
		}
			

	} while (false);

	
}//��ȡ�����ַ���
void CThumbnailDlg::GetTranslateString(ULONG nulPageCount, ULONG nulPageAnnot)
{
	IConfigFile* lpProfile = NULL;

	do
	{
		//����
		//�޸�����
		wchar_t lszText[MAX_PATH] = { 0 };
		lpProfile = EinkuiGetSystem()->GetCurrentWidget()->GetDefaultFactory()->GetTempleteFile();
		ICfKey* lpCfKey = NULL;
		if (lpProfile != NULL)
		{
			lpCfKey = lpProfile->OpenKey(L"String2/AllPage");
			if (lpCfKey != NULL)
			{
				lpCfKey->GetValue(lszText, MAX_PATH * sizeof(wchar_t));
				CMM_SAFE_RELEASE(lpCfKey);

				swprintf_s(mszPageAll, lszText, nulPageCount);
			}

			lpCfKey = lpProfile->OpenKey(L"String2/InkPage");
			if (lpCfKey != NULL)
			{
				lpCfKey->GetValue(lszText, MAX_PATH * sizeof(wchar_t));
				CMM_SAFE_RELEASE(lpCfKey);

				swprintf_s(mszPageAnnot, lszText, nulPageAnnot);
			}
		}
		CMM_SAFE_RELEASE(lpCfKey);

	} while (false);

	CMM_SAFE_RELEASE(lpProfile);
}

//���¶�λԪ��
void CThumbnailDlg::RelocationItem(void)
{
	if (mpIteratorNext != NULL)
	{
		EI_SIZE ldPaintSize;
		//D2D1_SIZE_F ldActionSize;

		EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);
		float lfWidth = 0.0f;
		D2D1_POINT_2F ldPos;
		ldPos.y = 10.0f;

		if (mpMenuThumbnail != NULL)
		{
			mpMenuThumbnail->GetIterator()->SetSize(float(ldPaintSize.w), float(ldPaintSize.h));
		}

		float lfBackHeight = 80.0f;

		if (ldPaintSize.w > ldPaintSize.h)
		{
			//����
			D2D1_POINT_2F ldPos = { 0.0f,80.0f };
			int li = 0;
			for (li = 0; li < TD_ITEM_MAX; li++)
			{
				if (mdList.Size() <= li)
					break;

				mdList.GetEntry(li)->GetIterator()->SetPosition(ldPos);
				mdList.GetEntry(li)->GetIterator()->SetSize(480.0f,251.0f);
				ldPos.x += 480.0f;

				if (((li + 1) % 4) == 0)
				{
					ldPos.x = 0.0f;
					ldPos.y += 251.0f;
				}
			}

		}
		else
		{
			//����
			D2D1_POINT_2F ldPos = { 0.0f,80.0f };
			int li = 0;
			for (li = 0; li < TD_ITEM_MAX; li++)
			{
				if (mdList.Size() <= li)
					break;

				mdList.GetEntry(li)->GetIterator()->SetPosition(ldPos);
				mdList.GetEntry(li)->GetIterator()->SetSize(270.0f, 461.0f);
				ldPos.x += 270.0f;

				if (((li + 1) % 4) == 0)
				{
					ldPos.x = 0.0f;
					ldPos.y += 461.0f;
				}
			}
		}

		lfWidth = mpIteratorPre->GetSizeX() + mpIteratorNext->GetSizeX() + mpIterBtPageNumber->GetSizeX();

		ldPos.x = (ldPaintSize.w - lfWidth) - 20.0f;

		mpIteratorPre->SetPosition(ldPos);
		ldPos.x += mpIteratorPre->GetSizeX();
		mpIterBtPageNumber->SetPosition(ldPos);
		ldPos.x += mpIterBtPageNumber->GetSizeX();
		mpIteratorNext->SetPosition(ldPos);

		ldPos.x = mpIterBtPageNumber->GetPositionX() - (mpPageProgress->GetIterator()->GetSizeX() - mpIterBtPageNumber->GetSizeX()) / 2.0f;
		ldPos.y = 80.0f;
		if (ldPos.x + mpPageProgress->GetIterator()->GetSizeX() > ldPaintSize.w)
			ldPos.x = ldPaintSize.w - mpPageProgress->GetIterator()->GetSizeX()-20.0f; //���������Ļ����������

		mpPageProgress->GetIterator()->SetPosition(ldPos);
		ldPos.x = mpIterBtPageNumber->GetPositionX() - ldPos.x + mpIterBtPageNumber->GetSizeX()/2.0f;
		ldPos.y = -12.0f;
		mpPageProgress->SetArrowPos(ldPos);


	}
}

//Ԫ�زο��ߴ緢���仯
ERESULT CThumbnailDlg::OnElementResized(D2D1_SIZE_F nNewSize)
{

	RelocationItem();

	return ERESULT_SUCCESS;
}

//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CThumbnailDlg::OnElementShow(bool nbIsShow)
{
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);
	if (nbIsShow != false)
	{
		mpIterator->BringToTop();
	}
	else
	{
		//�Լ�����ʱ�ѷ�ҳ������Ҳ�����ˣ��´���ʾʱ����ʾ����Ҫ�û��ٴε��ҳ������Ż���ʾ
		mpPageProgress->GetIterator()->SetVisible(false);
	}

	return ERESULT_SUCCESS;
}


//����ҳ��������ʾ
void CThumbnailDlg::EnterPage(ULONG nulPage)
{
	wchar_t lszString[MAX_PATH] = { 0 };
	int i = 0, k = 0;

	do
	{
		BREAK_ON_NULL(mpDocument);

		

		if (nulPage < 1)
			nulPage = 1;

		if (nulPage > mulMaxPage)
			nulPage = mulMaxPage;

				   //��¼��ǰҳ��
		if (mulMaxPage <= 0)
			mulCurrentPage = 0; //˵���Ǹ����ļ���
		else
			mulCurrentPage = nulPage;

		//��ʾҳ��
		swprintf_s(lszString, MAX_PATH, L"%d/%d", mulCurrentPage, mulMaxPage);
		CExMessage::SendMessageWithText(mpIterBtPageNumber, mpIterator, EACT_BUTTON_SETTEXT, lszString, NULL, 0);

		//����list����
		int liBegin = (mulCurrentPage - 1) * TD_ITEM_MAX;

		if (mulSelectType == THUMBNAIL_SELECT_ALL)
		{
			//ȫ��ҳ��
			for (i = liBegin, k = 0; i < (int)mulDocMaxPage && k < TD_ITEM_MAX; i++, k++)
			{
				CThumbnailListItem* lpItem = mdList.GetEntry(k);

				wchar_t lpszFolderPath[MAX_PATH] = { 0 };
				bool lbIsAnnot = false;
				mpDocument->GetThumbnailPathName(i, lpszFolderPath, &lbIsAnnot);

				lpItem->SetData(lpszFolderPath, i + 1, (i + 1) == mulCurrentDoc ? true : false);

				lpItem->GetIterator()->SetVisible(true);
			}
		}
		else
		{
			//�ѱ�עҳ��
			for (i = liBegin, k = 0; i < mdAnnotPageNumber.Size() && k < TD_ITEM_MAX; i++, k++)
			{
				CThumbnailListItem* lpItem = mdList.GetEntry(k);

				wchar_t lpszFolderPath[MAX_PATH] = { 0 };
				bool lbIsAnnot = false;
				int liIndex = mdAnnotPageNumber.GetEntry(i);
				mpDocument->GetThumbnailPathName(liIndex, lpszFolderPath, &lbIsAnnot);

				lpItem->SetData(lpszFolderPath, liIndex+1, (liIndex+1) == mulCurrentDoc ? true : false);

				lpItem->GetIterator()->SetVisible(true);
			}
		}
		

	} while (false);

	for (int j = k; j < TD_ITEM_MAX; j++)
	{
		//û����ô�����ˣ��Ѻ��������
		mdList.GetEntry(j)->GetIterator()->SetVisible(false);
	}

}

//��һҳ����һҳ
void CThumbnailDlg::NextPage(bool nbIsNext)
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

		EnterPage(mulCurrentPage);

		mpPageProgress->SetData((FLOAT)mulCurrentPage, (FLOAT)mulMaxPage);

	} while (false);
}
