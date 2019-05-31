/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "Einkui.h"

#include "ElementImp.h"
#include "EvButtonImp.h"
#include "EvListImp.h"
#include "EvPopupMenuImp.h"
#include "EvMenuButtonImp.h"
#include "EvButtonImp.h"


DEFINE_BUILTIN_NAME(MenuButton)

CEvMenuButton::CEvMenuButton(void)
{
	mpoPopupMenu = NULL;
	mpoButton = NULL;

	
}


CEvMenuButton::~CEvMenuButton(void)
{
}

ULONG CEvMenuButton::InitOnCreate(
	IN IEinkuiIterator* npParent,	// ������ָ��ss
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do 
	{
		//���ȵ��û���
		leResult = 	CXuiElement::InitOnCreate(npParent,npTemplete,nuEID);
		if(leResult != ERESULT_SUCCESS)
			break;

		leResult = ERESULT_SUCCESS;
	}while(false);


	return leResult;
}
//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEvMenuButton::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		LoadResource();

		// ���ð�ť��Ӧ����
		if(NULL != mpoButton)
		{
			D2D1_SIZE_F ldfSize;
			ldfSize = mpoButton->GetIterator()->GetSize();
			CExMessage::PostMessage(mpoButton->GetIterator(), mpIterator, EACT_BUTTON_SET_ACTION_RECT, ldfSize);
		}

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

//ERESULT CEvMenuButton::OnElementDestroy()
//{
//	ERESULT lResult = ERESULT_UNSUCCESSFUL;
//
//	return lResult;
//}
//
//������Ϣ
ERESULT CEvMenuButton::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npPaintBoard);

		if(mpBgBitmap != NULL)
			npPaintBoard->DrawBitmap(D2D1::RectF(0,0,(FLOAT)mpBgBitmap->GetWidth(),(FLOAT)mpBgBitmap->GetHeight()),
			mpBgBitmap,ESPB_DRAWBMP_LINEAR);

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

void CEvMenuButton::LoadResource()
{
	// ����Button
	ICfKey* lpBtnKey = mpTemplete->GetSubKey(TF_ID_MENUBUTTON_BUTTON);
	if(NULL != lpBtnKey && NULL == mpoButton)
	{
		mpoButton = CEvButton::CreateInstance(mpIterator, lpBtnKey, ID_OF_MENUBUTTON_BUTTON);
		if(NULL != mpoButton)			// ���ø�MenuButton��С��Ϊ��ť�Ĵ�С
		{
			mpIterator->SetSize(mpoButton->GetIterator()->GetSize());
		}
	}
	// ��ȡPopupMenuID
	miPopupMenuID = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_MENUBUTTON_POPUPMENU_ID, -1);

	LoadSubPopupMenu();
}

ERESULT CEvMenuButton::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������

	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	switch (npMsg->GetMessageID())
	{
	case EEVT_BUTTON_CLICK:					// ��ť������
		{
			OnBtnClick();

			luResult = ERESULT_SUCCESS;
		}
		break;

	case EEVT_MENUITEM_MOUSE_HOVER:
		{
			mpIterator->PostMessageToParent(npMsg);
			luResult = ERESULT_SUCCESS;
		}
		break;

	case EEVT_MENUITEM_CLICK:					// ���Ӷ���Ĳ˵�����
		{
			this->GetIterator()->PostMessageToParent(npMsg);

			BREAK_ON_NULL(mpoButton);
			// ���Ӳ˵�����ʱ��ȡ����ť��Checked״̬
			bool lbIsCheck = false;
			luResult = CExMessage::PostMessage(mpoButton->GetIterator(), mpIterator, EACT_BUTTON_SET_CHECKED, lbIsCheck);
		}
		break;

	case EEVT_MENUBUTTON_SET_SUBMENU_VISIBLE:
		{
			if(npMsg->GetInputDataSize() != sizeof(bool))
				break;

			bool lbIsVisible = *(bool*)npMsg->GetInputData();
			if(NULL != mpoPopupMenu)
				mpoPopupMenu->GetIterator()->SetVisible(lbIsVisible);
			bool lbIsChecked = false;
			if(false != lbIsVisible)
			{
				lbIsChecked = true;
			}
			if(NULL != mpoButton)
			{
				// ������Ϣ����ť��ʹ֮����Checked״̬
				luResult = CExMessage::PostMessage(mpoButton->GetIterator(), mpIterator, EACT_BUTTON_SET_CHECKED, lbIsChecked);
			}
			else
				luResult = ERESULT_SUCCESS;
		}
		break;

	case EEVT_BUTTON_MOUSE_IN:
	case EEVT_BUTTON_MOUSE_OUT:
		{
			STEMS_STATE_CHANGE ldStrteChange;
			ldStrteChange.State = npMsg->GetMessageID() == EEVT_BUTTON_MOUSE_IN? 1 : 0;
			ldStrteChange.Related = npMsg->GetMessageSender();
			OnMouseFocus(&ldStrteChange);

			luResult = ERESULT_SUCCESS;
		}
		break;

	case EEVT_MENUBUTTON_INSERT_MENUITEM:
		{
			if(npMsg->GetInputDataSize() != sizeof(STCTL_MENUBUTTON_INSERT_MENUITEM)
				|| npMsg->GetInputData() == NULL)
				break;

			STCTL_MENUBUTTON_INSERT_MENUITEM ldInfo = *(PSTCTL_MENUBUTTON_INSERT_MENUITEM)npMsg->GetInputData();
			IEinkuiIterator* lpoIterPopupMenu = GetPopupMenuByUniqueID(ldInfo.UniquePopupMenuId);
			if(NULL == lpoIterPopupMenu)
				break;

			if(ERESULT_SUCCESS != CExMessage::PostMessage(lpoIterPopupMenu, mpIterator,
				EACT_POPUPMENU_INSERT_MENUITEM_BY_CREATE, ldInfo.PopupMenuInfo))
				break;

			luResult = ERESULT_SUCCESS;
		}
		break;

	default:
		luResult = ERESULT_NOT_SET;
		break;
	}

	if(luResult == ERESULT_NOT_SET)
	{
		luResult = CXuiElement::ParseMessage(npMsg); // ���û����ͬ��������ע�⣺һ��Ҫ��������ֱ�ӻ���
		//luResult = ERESULT_UNEXPECTED_MESSAGE;	// ���û�л��࣬��������ʱ��ɾ�����䣻
	}

	return luResult;
}

void CEvMenuButton::LoadSubPopupMenu()
{
	if(-1 == miPopupMenuID)
		return;

	// �����Ӳ˵�ģ�壬������
	ICfKey* lpoPopupMenuKey = mpTemplete->GetParentsKey();
	while(NULL != lpoPopupMenuKey->GetParentsKey())
		lpoPopupMenuKey = lpoPopupMenuKey->GetParentsKey();
	lpoPopupMenuKey = lpoPopupMenuKey->GetSubKey(L"PopupMenu");
	if(NULL == lpoPopupMenuKey)
		return;

	// ��Ѱ��IDΪmiPopupMenuID���Ӳ˵�
	lpoPopupMenuKey = lpoPopupMenuKey->MoveToSubKey();
	while(NULL != lpoPopupMenuKey)
	{
		if(miPopupMenuID == lpoPopupMenuKey->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_MAIN_ID, -1))
			break;
		else
			lpoPopupMenuKey = lpoPopupMenuKey->MoveToNextKey();
	}

	if(NULL == lpoPopupMenuKey)
		return;

	mpoPopupMenu = CEvPopupMenu::CreateInstance(mpIterator, lpoPopupMenuKey, ID_OF_MENUBUTTON_POPUPMENU);
	if(NULL == mpoPopupMenu)
		return;

	mpoPopupMenu->GetIterator()->SetVisible(false);

	SetPopupMenuPosition();
}

void CEvMenuButton::OnBtnClick()
{
	// �ڰ�ť�·������Ӳ˵�

	// ������״̬����ʾ״̬
	bool lbIsShow = false;
	if(NULL != mpoPopupMenu)
	{
		if(false == mpoPopupMenu->GetIterator()->IsVisible())	
		{
			mpoPopupMenu->GetIterator()->SetVisible(true);
			lbIsShow = true;
		}
		else			// ����ʾ״̬������״̬
		{
			mpoPopupMenu->GetIterator()->SetVisible(false);
		}
	}

	// ������Ϣ�������ڣ���ʾ�˵���ť�����
	this->PostMessageToParent(EEVT_MENUBUTTON_CLICK, lbIsShow);
}

//���û�����
ERESULT CEvMenuButton::OnElementEnable(bool nbIsEnable)
{
	if(false == nbIsEnable)
	{
		if(NULL != mpoButton)
			mpoButton->GetIterator()->SetEnable(false);
		if(NULL != mpoPopupMenu)
			mpoPopupMenu->GetIterator()->SetEnable(false);
	}
	else
	{
		if(NULL != mpoButton)
			mpoButton->GetIterator()->SetEnable(true);
		if(NULL != mpoPopupMenu)
			mpoPopupMenu->GetIterator()->SetEnable(true);
	}

	return ERROR_SUCCESS;
}

void CEvMenuButton::OnMouseFocus(PSTEMS_STATE_CHANGE npState)
{
	if(0 != npState->State)	// ����
	{
		// �����ǰ�Ӳ˵��Ѿ���ʾ������Ҫ����
		if(NULL != mpoPopupMenu && false != mpoPopupMenu->GetIterator()->IsVisible())
		{
			return;
		}

		// ѯ���Ƿ��Ѿ��������˵�����ʾ
		bool lbIsShow = false;
		this->SendMessageToParent(EACT_MENUBAR_ANY_SUBMENU_VISIBLE, CExMessage::DataInvalid, &lbIsShow, sizeof(bool));


		// ����Ѿ��в˵����������������ò˵���ťʱӦ�õ����˵���ͬʱ������Ϣ�������ڣ�����֮ǰ�����Ĳ˵�
		if(false != lbIsShow)
		{
			// ������Ϣ�������ڣ�����֮ǰ�Ĳ˵�
			this->PostMessageToParent(EACT_MENUBAR_HIDE_LAST_SUBMENU, CExMessage::DataInvalid);
			if(NULL != mpoPopupMenu)
			{
				// ������Ϣ����ť��ʹ֮����Checked״̬
				bool lbIsChecked = true;
				CExMessage::PostMessage(mpoButton->GetIterator(), mpIterator, EACT_BUTTON_SET_CHECKED, lbIsChecked);

				mpoPopupMenu->GetIterator()->SetVisible(true);
			}
		}

	}
}


// ������
//		���úõ����˵���λ��
void CEvMenuButton::SetPopupMenuPosition()
{
	if(NULL == mpoPopupMenu)
		return;

	// ����Ҫ�����Ӳ˵���λ�ã���ť�����½����ң����߰�ť�����½�����
	//int liX = 0, liY = 0;

	// �����½��Ƿ����㹻�ռ�
	//EinkuiGetSystem()->GetElementManager()->GetDesktop()->GetSize();

	if(NULL != mpoButton)
		mpoPopupMenu->GetIterator()->SetPosition(mpoButton->GetIterator()->GetPositionX() - 6, mpoButton->GetIterator()->GetSizeY());
}

// ������
//		��ȡ��MenuButton��ָ��UniqueID��PopupMenu����������ģ�
IEinkuiIterator* CEvMenuButton::GetPopupMenuByUniqueID(
	IN UINT niUniqueID
	)
{
	IEinkuiIterator* lpoResult = NULL;
	do 
	{
		BREAK_ON_NULL(mpoPopupMenu);
		lpoResult = mpoPopupMenu->GetPopupMenuByMainID(niUniqueID);

	} while (false);
	return lpoResult;
}


