/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "ElementImp.h"
#include "FileHistoryDlg.h"
#include "MsgDefine.h"
#include <stdio.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#include <Shellapi.h>

DEFINE_BUILTIN_NAME(FileHistoryDlg)

CFileHistoryDlg::CFileHistoryDlg()
{
	mpFileOpenDlg = NULL;
	mpIteratorClose = NULL;
	mpIteratorMore = NULL;
	mpIteratorOpen = NULL;
	mpDeleteHistory = NULL;
	mpIteratorSelected = NULL;
}

CFileHistoryDlg::~CFileHistoryDlg(void)
{
	
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CFileHistoryDlg::OnElementCreate(IEinkuiIterator* npIterator)
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


		//����Homebar
		EiSetHomebarStatus(GI_HOMEBAR_COLLAPSE);
		mpIterator->SetTimer(100, 1, 10, NULL);

		//mpIterator->SetPosition((),mpIterator->GetPositionY());
		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}

ULONG CFileHistoryDlg::InitOnCreate(
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
		for (int i = 0; i < FHD_LIST_MAX; i++)
		{
			CFileHistoryListItem* lpListItem = CFileHistoryListItem::CreateInstance(mpIterator, lpSubKey);
			lpListItem->GetIterator()->SetVisible(false);

			mdList.Insert(i, lpListItem);
		}
		CMM_SAFE_RELEASE(lpSubKey);
		

		mpIteratorClose = mpIterator->GetSubElementByID(FHD_BT_CLOSE);
		BREAK_ON_NULL(mpIteratorClose);

		mpIteratorOpen = mpIterator->GetSubElementByID(FHD_BT_OPEN);
		BREAK_ON_NULL(mpIteratorOpen);

		mpIteratorMore = mpIterator->GetSubElementByID(FHD_BT_MORE);
		BREAK_ON_NULL(mpIteratorMore);

		mpIteratorSelected = mpIterator->GetSubElementByID(1);
		BREAK_ON_NULL(mpIteratorSelected);
		mpIteratorSelected->SetVisible(false);

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	return leResult;
}

//���¶�λԪ��
void CFileHistoryDlg::Relocation(void)
{
	if (mpIteratorMore != NULL)
	{
		EI_SIZE ldPaintSize;
		EinkuiGetSystem()->GetPaintboardSize(&ldPaintSize);
		if (ldPaintSize.w > ldPaintSize.h)
		{
			//����
			D2D1_POINT_2F ldPos = { 80.0f,340.0f };
			int li = 0;
			for (li = 0; li < FHD_LIST_MAX; li++)
			{
				if (mdList.Size() <= li)
					break;

				mdList.GetEntry(li)->GetIterator()->SetPosition(ldPos);
				ldPos.x += 586.0f;

				if (((li + 1) % 3) == 0)
				{
					ldPos.x = 80.0f;
					ldPos.y += 160.0f;
				}
			}

			mpIteratorSelected->SetSize(586.0f, 160.0f);
			mpIteratorSelected->SetPosition(80.0f, 340.0f);

		}
		else
		{
			//����
			D2D1_POINT_2F ldPos = { 20.0f,340.0f };
			int li = 0;
			for (li = 0; li < FHD_LIST_MAX; li++)
			{
				if (mdList.Size() <= li)
					break;
				mdList.GetEntry(li)->GetIterator()->SetPosition(ldPos);
				ldPos.x += 520.0f;

				if ((li % 2) != 0)
				{
					ldPos.x = 20.0f;
					ldPos.y += 160.0f;
				}
			}

			mpIteratorSelected->SetSize(520.0f, 160.0f);
			mpIteratorSelected->SetPosition(20.0f, 340.0f);
		}

		if(mpIteratorClose->IsVisible() == false)
			mpIteratorMore->SetPosition(ldPaintSize.w - mpIteratorMore->GetSizeX() - 100.0f, mpIteratorMore->GetPositionY());
		else
			mpIteratorMore->SetPosition(ldPaintSize.w - mpIteratorMore->GetSizeX(), mpIteratorMore->GetPositionY());

		SetOpenFilePos();
	}
	

}

//��ʼ��list,Ĭ����ʾ���������ļ��м��̷�
void CFileHistoryDlg::InitList(void)
{
	do 
	{
		for (int li = 0; li < FHD_LIST_MAX; li++)
		{
			if (li < mpdHistroyPath->Size())
			{
				mdList.GetEntry(li)->SetData(mpdHistroyPath->GetEntry(li));
				mdList.GetEntry(li)->GetIterator()->SetVisible(true);
			}
			else
			{
				mdList.GetEntry(li)->GetIterator()->SetVisible(false);
			}
				
		}

		Relocation();

	} while (false);
}

std::tuple<HISTORY_FILE_ATTRIB*, wstring> CFileHistoryDlg::DoModal(bool nbIsEnableCancel)
{
	if (nbIsEnableCancel != false)
	{
		EiSetHomebarStatus(GI_HOMEBAR_HIDE);//����homebar
		
	}
	EiSetBatteryStatus(GI_BATTERY_HIDE);
	mpIteratorClose->SetVisible(nbIsEnableCancel);

	//��ǰ���ļ�ѡ���
	mpIteratorSelected->SetVisible(nbIsEnableCancel);
	if (mpdHistroyPath->Size() <= 0)
		mpIteratorSelected->SetVisible(false);

	//mdList.GetEntry(0)->SetEnable(!nbIsEnableCancel);

	Relocation();

	mpIterator->SetActive();
	mpIterator->BringToTop();
	EinkuiGetSystem()->EnablePaintboard(false);
	EinkuiGetSystem()->ClearEinkBuffer();
	EinkuiGetSystem()->UpdateView(true);
	EinkuiGetSystem()->DoModal(mpIterator);

	auto result = std::make_tuple(m_resultHistoryFile, m_resultFileName);
	mpIterator->Close();

	return result;
}

void CFileHistoryDlg::ExitModal()
{
	EiSetHomebarStatus(GI_HOMEBAR_SHOW);//��ʾhomebar
	EiSetBatteryStatus(GI_BATTERY_SHOW);
	EinkuiGetSystem()->ExitModal(mpIterator,0);
	EinkuiGetSystem()->UpdateView(true);
}

//��Ϣ������
ERESULT CFileHistoryDlg::ParseMessage(IEinkuiMessage* npMsg)
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
		HISTORY_FILE_ATTRIB* lpFileAttrib = NULL;

		luResult = CExMessage::GetInputData(npMsg, lpFileAttrib);
		if (luResult != ERESULT_SUCCESS)
			break;

		ListClick(lpFileAttrib);
		break;
	}
	case EEVT_DELETE_READ_HISTORY:
	{
		//�����ʷ��¼
		PostMessageToParent(EEVT_DELETE_READ_HISTORY, CExMessage::DataInvalid);
		mpIteratorSelected->SetVisible(false);

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
void CFileHistoryDlg::ListClick(HISTORY_FILE_ATTRIB* npFileAttrib)
{
	do
	{
		BREAK_ON_NULL(npFileAttrib);

		m_resultHistoryFile = npFileAttrib;

		if (_wcsicmp(npFileAttrib->FilePath, mpdHistroyPath->GetEntry(0)->FilePath) == 0)
		{
			//���������ǵ�ǰ���ļ�����ֱ�ӹرմ���
			ExitModal();
			break;
		}

		bool lbRet = false;
		mpIterator->SetVisible(false);
		CExMessage::SendMessage(mpIterator->GetParent(), mpIterator, EEVT_ER_OPEN_HISTORY_FILE_PATH, npFileAttrib, &lbRet, sizeof(lbRet));
		if (lbRet != false)
			ExitModal(); //˵���ļ��򿪳ɹ��ˣ��Ǿ��˳��Լ�
		else
			mpIterator->SetTimer(FP_TIMER_ID_SHOW, 1, 3000, NULL);

	} while (false);
}


//��ʱ��
void CFileHistoryDlg::OnTimer(PSTEMS_TIMER npStatus)
{
	if (npStatus->TimerID == FHD_TIMER_ID_SHOW)
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
ERESULT CFileHistoryDlg::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		ULONG llBtnID = npSender->GetID();
		switch (llBtnID)
		{
		case FHD_BT_CLOSE:		// �ر�	
		{
			ExitModal();

			break;
		}
		case FHD_BT_OPEN:
		{
			//�������ļ��Ի���
			//ѡ���ļ�
			if (mpFileOpenDlg != NULL)
			{
				mpFileOpenDlg->ExitModal();
				mpFileOpenDlg = NULL;
			}

			ICfKey* lpSubKey = mpTemplete->OpenKey(L"OpenFile");
			mpFileOpenDlg = CFileOpenDlg::CreateInstance(mpIterator, lpSubKey);
			wchar_t* lpHistoryPath = mpdHistroyPath->Size() > 0 ? mpdHistroyPath->GetEntry(0)->FilePath : NULL;
			mpFileOpenDlg->SetHistoryList(lpHistoryPath);
			SetOpenFilePos();
			bool lbIsSuccess = false;
			wstring&& resultFile = mpFileOpenDlg->DoModal(&lbIsSuccess);

			if (lbIsSuccess)
			{
				m_resultFileName = resultFile;
			}

			CMM_SAFE_RELEASE(lpSubKey);
			mpFileOpenDlg = NULL;

			if (lbIsSuccess)
			{
				ExitModal();
			}

			break;
		}
		case FHD_BT_MORE:
		{
			//������ղ˵�
			if (mpDeleteHistory != NULL)
			{
				mpDeleteHistory->ExitModal();
				mpDeleteHistory = NULL;
			}

			ICfKey* lpSubKey = mpTemplete->OpenKey(L"MenuMore");
			mpDeleteHistory = CDeleteHistory::CreateInstance(mpIterator, lpSubKey);
			wchar_t* lpHistoryPath = mpdHistroyPath->Size() > 0 ? mpdHistroyPath->GetEntry(0)->FilePath : NULL;
			mpDeleteHistory->GetIterator()->SetSize(mpIterator->GetSize());
			mpDeleteHistory->DoModal();

			CMM_SAFE_RELEASE(lpSubKey);
			mpDeleteHistory = NULL;

			EinkuiGetSystem()->UpdateView(true);

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
ERESULT CFileHistoryDlg::OnPaint(IEinkuiPaintBoard* npPaintBoard)
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

//Ԫ�زο��ߴ緢���仯
ERESULT CFileHistoryDlg::OnElementResized(D2D1_SIZE_F nNewSize)
{
	if(mpDeleteHistory != NULL)
		mpDeleteHistory->GetIterator()->SetSize(nNewSize);

	Relocation();
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
void CFileHistoryDlg::SetHistoryList(cmmVector<HISTORY_FILE_ATTRIB*>* npdHistroyPath)
{
	mpdHistroyPath = npdHistroyPath;

	InitList();
}

//���ô��ļ����ڵ�λ��
void CFileHistoryDlg::SetOpenFilePos(void)
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