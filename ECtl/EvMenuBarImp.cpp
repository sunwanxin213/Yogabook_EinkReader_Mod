/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "Einkui.h"

#include "ElementImp.h"
#include "EvMenuBarImp.h"
#include "EvButtonImp.h"
#include "EvListImp.h"
#include "EvPopupMenuImp.h"
#include "EvMenuButtonImp.h"


DEFINE_BUILTIN_NAME(MenuBar)

CEvMenuBar::CEvMenuBar(void)
{
	mbIsSubMenuVisible = false;
	mpoLastShowMenuButton = NULL;
}


CEvMenuBar::~CEvMenuBar(void)
{

}


ULONG CEvMenuBar::InitOnCreate(
	IN IEinkuiIterator* npParent,	// ������ָ��
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

		LoadResource();

		leResult = ERESULT_SUCCESS;
	}while(false);


	return leResult;
}
//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEvMenuBar::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		mpIterator->ModifyStyles(EITR_STYLE_POPUP);


		//// ע��Alt + F4 �ȼ�
		//mulHotKeyID = VK_F4;
		//mulHotKeyID |= 0x00000100;

		//// ����ע��ȫ���ȼ�
		//EinkuiGetSystem()->GetElementManager()->RegisterHotKey(mpIterator, mulHotKeyID, VK_F4, false, false, true, NULL);

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

//ERESULT CEvMenuBar::OnElementDestroy()
//{
//	ERESULT lResult = ERESULT_UNSUCCESSFUL;
//
//	// 	do
//	// 	{
//	// 		//CDropTargetMgr::UnregisterDropWindow(EinkuiGetSystem()->GetMainWindow(),mpDropTargetManager);
//	// 
//	// 		lResult = ERESULT_SUCCESS;
//	// 
//	// 	}while(false);
//
//	return lResult;
//}
//
//������Ϣ
ERESULT CEvMenuBar::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npPaintBoard);

		if(mpBgBitmap != NULL)
			npPaintBoard->DrawBitmap(D2D1::RectF(0,0,mpIterator->GetSizeX(),mpIterator->GetSizeY()),
			mpBgBitmap,ESPB_DRAWBMP_LINEAR);

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

void CEvMenuBar::LoadResource()
{

}

ERESULT CEvMenuBar::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������

	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	switch (npMsg->GetMessageID())
	{
	case EEVT_MENUITEM_CLICK:			// ���Ӷ���Ĳ˵�����
		{
			this->GetIterator()->PostMessageToParent(npMsg);

			// ȡ�������˵�״̬
			mbIsSubMenuVisible = false;
			if(NULL != mpoLastShowMenuButton)
			{
				CExMessage::PostMessage(mpoLastShowMenuButton, mpIterator, EEVT_MENUBUTTON_SET_SUBMENU_VISIBLE, false);
				mbIsSubMenuVisible = false;
				mpoLastShowMenuButton = NULL;
			}
			

			luResult = ERESULT_SUCCESS;
		}
		break;

	case EEVT_MENUITEM_MOUSE_HOVER:
		{
			mpIterator->PostMessageToParent(npMsg);
			luResult = ERESULT_SUCCESS;
		}
		break;

	case EEVT_MENUBUTTON_CLICK:					// MenuButton�����
		{
			if(npMsg->GetInputDataSize() != sizeof(bool))
				break;

			mbIsSubMenuVisible = *(bool*)npMsg->GetInputData();

			// �����Ӳ˵�,����µ�ǰ�������Ӳ˵�
			if(false != mbIsSubMenuVisible)
			{
				mpoLastShowMenuButton = npMsg->GetMessageSender();
				if(NULL == mpoLastShowMenuButton)
					break;

				if(false == mpoLastShowMenuButton->GetElementObject()->IsKindOf(GET_BUILTIN_NAME(MenuButton)))
				{
					mpoLastShowMenuButton = NULL;
					break;
				}
			}
			else
			{
				if(NULL != mpoLastShowMenuButton)
				{
					CExMessage::PostMessage(mpoLastShowMenuButton, mpIterator, EEVT_MENUBUTTON_SET_SUBMENU_VISIBLE, false);
					mbIsSubMenuVisible = false;
					mpoLastShowMenuButton = NULL;
				}
			}

			luResult = ERESULT_SUCCESS;
		}
		break;

	case EACT_MENUBAR_ANY_SUBMENU_VISIBLE:		// ѯ���Ƿ����Ӳ˵���չ��	
		{
			if(npMsg->GetOutputBufferSize() != sizeof(bool))
			{
				break;
			}

			// ��ȡ�������ĵ�ַ
			bool* lpbIsShow = (bool*)npMsg->GetOutputBuffer();
			// ������ݵ��������
			*lpbIsShow = mbIsSubMenuVisible;

			npMsg->SetOutputDataSize(sizeof(bool));

			luResult = ERESULT_SUCCESS;
		}
		break;

	case EACT_MENUBAR_HIDE_LAST_SUBMENU:		
		{
			if(NULL != mpoLastShowMenuButton)
			{
				// ������Ϣ�����Ӳ˵���
				bool lbIsVisible = false;
				CExMessage::PostMessage(mpoLastShowMenuButton, mpIterator, EEVT_MENUBUTTON_SET_SUBMENU_VISIBLE, lbIsVisible);
				mpoLastShowMenuButton = npMsg->GetMessageSender();
				if(NULL == mpoLastShowMenuButton)
					break;

				if(false == mpoLastShowMenuButton->GetElementObject()->IsKindOf(GET_BUILTIN_NAME(MenuButton)))
				{
					mpoLastShowMenuButton = NULL;
					break;
				}
			}
			luResult = ERESULT_SUCCESS;
		}
		break;
	case EMSG_ELEMENT_ACTIVATED:
		{
			//����״̬�ı�
			STEMS_ELEMENT_ACTIVATION* lpActive;
			luResult = CExMessage::GetInputDataBuffer(npMsg,lpActive);
			if(luResult != ERESULT_SUCCESS)
				break;

			if (lpActive->State == 0 && mpIterator->IsVisible() != false)			// �ɼ�״̬��ʧȥ����״̬����Ҫ���ص�ǰչ���Ĳ˵�
			{
				if(NULL != mpoLastShowMenuButton)
				{
					CExMessage::PostMessage(mpoLastShowMenuButton, mpIterator, EEVT_MENUBUTTON_SET_SUBMENU_VISIBLE, false);
					mbIsSubMenuVisible = false;
				}
			}

			luResult = ERESULT_SUCCESS;
		}
		break;

	case EACT_MENUBAR_INSERT_NEW_MENUITEM:
		{
			if(npMsg->GetInputDataSize() != sizeof(STCTL_MENUBAR_INSERT_MENUITEM)
				|| npMsg->GetInputData() == NULL)
				break;

			STCTL_MENUBAR_INSERT_MENUITEM ldInfo = *(PSTCTL_MENUBAR_INSERT_MENUITEM)npMsg->GetInputData();
			IEinkuiIterator* lpoIterMenuButton = mpIterator->GetSubElementByID(ldInfo.MenuButtonID);
			if(NULL == lpoIterMenuButton)
				break;

			if(ERESULT_SUCCESS != CExMessage::PostMessage(lpoIterMenuButton, mpIterator, EEVT_MENUBUTTON_INSERT_MENUITEM, ldInfo.MenuButtonInfo))
				break;

			luResult = ERESULT_SUCCESS;
		}
		break;

	case EACT_MENUBAR_GET_POPUPMENU_BY_COMMANDID:
		{
			ULONG *lplCommandID = NULL;
			if(ERESULT_SUCCESS != CExMessage::GetInputDataBuffer(npMsg, lplCommandID)
				|| sizeof(IEinkuiIterator*) != npMsg->GetOutputBufferSize())
			{
				luResult = ERESULT_WRONG_PARAMETERS;
				break;
			}

			IEinkuiIterator** lpIter = (IEinkuiIterator**)npMsg->GetOutputBuffer();
			(*lpIter) = GetPopupMenuByCommandID(*lplCommandID);

			if(NULL == *lpIter)
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

//��ť�����¼�
ERESULT CEvMenuBar::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		LONG llCommandID = 0;
		if(npSender->GetID() == TF_ID_MENUBAR_BTN_MIN)		// ��С��
		{
			llCommandID = 0x00000001;
		}
		else if(npSender->GetID() == TF_ID_MENUBAR_BTN_CLOSE)		// �ر�
		{
			llCommandID = 0x00000002;
		}
		else
			break;

		EinkuiGetSystem()->GetElementManager()->SimplePostMessage(
			mpIterator->GetParent(), EEVT_MENUITEM_CLICK, &llCommandID, sizeof(long));

		lResult = ERESULT_SUCCESS;
	}while(false);

	return lResult;
}

//Ԫ�زο��ߴ緢���仯
ERESULT CEvMenuBar::OnElementResized(D2D1_SIZE_F nNewSize)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		// ���ú�������ť��λ��
		IEinkuiIterator* lpoMiniBtn = mpIterator->GetSubElementByID(TF_ID_MENUBAR_BTN_MIN);
		if(NULL != lpoMiniBtn)
		{
			lpoMiniBtn->SetPosition(mpIterator->GetSizeX() - 57, lpoMiniBtn->GetPositionY());

			IEinkuiIterator* lpoCloseBtn = mpIterator->GetSubElementByID(TF_ID_MENUBAR_BTN_CLOSE);
			if(NULL != lpoCloseBtn)
				lpoCloseBtn->SetPosition(lpoMiniBtn->GetPositionX() + lpoMiniBtn->GetSizeX(), lpoMiniBtn->GetPositionY());
		}

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

// ��ȡָ��CommandID��PopupMenu
IEinkuiIterator* CEvMenuBar::GetPopupMenuByCommandID(IN ULONG niCommandID)
{
	IEinkuiIterator* lpoIter = NULL;
	do 
	{
		IEinkuiIterator* lpoTempIter = NULL;
		for(int i = 0; i < mpIterator->GetSubElementCount(); i++)
		{
			lpoTempIter = mpIterator->GetSubElementByZOder(i);
			if(NULL != lpoTempIter && false != lpoTempIter->GetElementObject()->GlobleVerification(L"MenuButton"))
			{
				lpoTempIter = ((CEvMenuButton*)lpoTempIter->GetElementObject())->GetPopupMenuByUniqueID(niCommandID);
				if(NULL != lpoTempIter)
				{
					lpoIter = lpoTempIter;
					break;
				}
			}
		}


		return lpoIter;
	} while (false);
	return lpoIter;
}

////��ݼ���Ϣ
//ERESULT CEvMenuBar::OnHotKey(const STEMS_HOTKEY* npHotKey)
//{
//	if(npHotKey->HotKeyID == mulHotKeyID)		// �ر�
//	{
//		LONG llCommandID = 0x00000002;
//		EinkuiGetSystem()->GetElementManager()->SimplePostMessage(
//			mpIterator->GetParent(), EEVT_MENUITEM_CLICK, &llCommandID, sizeof(long));
//	}
//
//	return ERESULT_KEY_UNEXPECTED;
//}

