#include "StdAfx.h"
#include "Einkui.h"

#include "ElementImp.h"
#include "cmmstruct.h"
#include "EvMenuItemImp.h"
#include "EvPopupMenuImp.h"

#include "assert.h"


DEFINE_BUILTIN_NAME(PopupMenu)
CEvPopupMenu::CEvPopupMenu(void)
{
	miMainID = -1;

	mpoMenuItemInfo = NULL;

	miItemBgFrameCount = -1;

	mbIsManagerMenuItemEnable = true;
}


CEvPopupMenu::~CEvPopupMenu(void)
{
	CMM_SAFE_RELEASE(mpoItemBgBitmap);
	CMM_SAFE_DELETE(mpoMenuItemInfo);
}

ULONG CEvPopupMenu::InitOnCreate(
	IN IEinkuiIterator* npParent,	// ������ָ��
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do
	{
		//���ȵ��û���
		leResult = CXuiElement::InitOnCreate(npParent, npTemplete, nuEID);
		if (leResult != ERESULT_SUCCESS)
			break;
		LoadResource();

		leResult = ERESULT_SUCCESS;
	} while (false);


	return leResult;
}
//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEvPopupMenu::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if (CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}

//ERESULT CEvPopupMenu::OnElementDestroy()
//{
//	ERESULT lResult = ERESULT_UNSUCCESSFUL;
//
//	// 	do
//	// 	{
//	// 
//	// 		lResult = ERESULT_SUCCESS;
//	// 
//	// 	}while(false);
//
//	return lResult;
//}

//������Ϣ
ERESULT CEvPopupMenu::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npPaintBoard);

		if (mpBgBitmap != NULL)
			npPaintBoard->DrawBitmap(D2D1::RectF(0, 0, (FLOAT)mpIterator->GetSizeX(), (FLOAT)mpIterator->GetSizeY()), mpBgBitmap, ESPB_DRAWBMP_EXTEND);

		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}

void CEvPopupMenu::LoadResource()
{
	bool lbResult = false;
	do
	{
		// ��ȡMainID
		miMainID = (int)this->mpTemplete->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_MAIN_ID, -1);
		// ���û��MainId,�򲻼���
		if (-1 == miMainID)
			break;

		// ����Ϊ��ʹ�����д����õ�MenuItem����ʾ�����Ŀ�ȶ�һ�£���ҪѰ������MenuItem��
		// ��Ϊ��Ҫ�ٴ��������е�MenuItem֮�����֪�����Ŀ�ȣ�������MenuItem����Ҫ�ܶ��ʼ����Ϣ��
		// ��������MenuItemInfo�ṩ�˹�����menuItem��ʼ����Ϣ��ÿ��MenuItem������һ�ݸ���Ϣ�����������������Խ����޸�
		// ��������֮�������ú�����MenuItem�Ŀ��

		// ��ʼ��ItemInfo�ṹ��
		InitMenuItemInfo();
		if (NULL == mpoMenuItemInfo)
			break;

		// ���غñ���ͼ
		// ItemBackGround
		wchar_t* lswBackgGround = (wchar_t*)mpTemplete->QuerySubKeyValueAsBuffer(TF_ID_POPUPMENU_ITEM_BACKGROUND);
		if (NULL != lswBackgGround && UNICODE_NULL != lswBackgGround[0])
		{
			mpoItemBgBitmap = EinkuiGetSystem()->GetAllocator()->LoadImageFile(
				lswBackgGround, L'.' == lswBackgGround[0] ? false : true);
			mpTemplete->ReleaseBuffer(&lswBackgGround);
		}

		// ItemFrameCount 
		miItemBgFrameCount = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_ITEM_FRAMECOUNT, DEFAULT_ITEM_FRAMECOUNT);
		if (NULL != mpoItemBgBitmap)
		{
			mpoItemBgBitmap->SetExtendLine(mpoItemBgBitmap->GetWidth() / miItemBgFrameCount / 2, mpoItemBgBitmap->GetHeight() / 2);
		}


		// ��������MenuItem
		ICfKey* lpMenuItemKey = mpTemplete->GetSubKey(TF_ID_POPUPMENU_MENU_ITEM);
		BREAK_ON_NULL(lpMenuItemKey);
		CEvMenuItem* lpoMenuItem = NULL;
		lpMenuItemKey = lpMenuItemKey->MoveToSubKey();
		while (lpMenuItemKey != NULL)	//ѭ������������Ԫ��
		{
			lpoMenuItem = CEvMenuItem::CreateInstance(mpIterator, lpMenuItemKey);
			if (NULL == lpoMenuItem)			// ��������ʧ�ܵģ�������
				break;

			lpoMenuItem->GetIterator()->SetSize((FLOAT)lpMenuItemKey->QuerySubKeyValueAsLONG(L"Width", 0), (FLOAT)lpMenuItemKey->QuerySubKeyValueAsLONG(L"Height", 0));
			lpoMenuItem->SetBgBitmapPtr(mpoItemBgBitmap, miItemBgFrameCount);

			lpoMenuItem->SetMenuItemInfo(mpoMenuItemInfo);			// �ṩ�˵�����Ϣ���������ز˵�����Դ

			// ��������뵽����ĩβ
			if (moMenuItemVec.Insert(-1, lpoMenuItem) < 0)
				break;

			lpMenuItemKey = lpMenuItemKey->MoveToNextKey();	//����һ����Ԫ�ؼ�
		}

		ReLayoutMenuItem();

		lbResult = true;
	} while (false);

	if (false == lbResult)
	{
		PrintDebugString(L"PopupMenu_LoadResource ʧ�ܡ�");
	}
}

// ��ʼ��MenuItemInfo
void CEvPopupMenu::InitMenuItemInfo()
{
	if (NULL == mpTemplete)
		return;

	CMM_SAFE_DELETE(mpoMenuItemInfo);

	mpoMenuItemInfo = new ST_MENUITEM_INFO;
	memset(mpoMenuItemInfo, 0, sizeof(ST_MENUITEM_INFO));

	// LeftWidth
	mpoMenuItemInfo->LeftWidth = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_LEFT_WIDTH, DEFAULT_LEFT_WIDTH);

	// LeftIconWidth
	mpoMenuItemInfo->LeftIconWidth = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_LEFT_ICON_WIDTH, -1);

	// LeftIconHeight
	mpoMenuItemInfo->LeftIconHeight = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_LEFT_ICON_HEIGHT, -1);

	// RightWidth
	mpoMenuItemInfo->RightWidth = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_RIGHT_WIDTH, DEFAULT_RIGHT_WIDTH);

	// RightIconWidth
	mpoMenuItemInfo->RightIconWidth = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_RIGHT_ICON_WIDTH, -1);

	// RightIconHeight
	mpoMenuItemInfo->RightIconHeight = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_RIGHT_ICON_HEIGHT, -1);

	// MiddleWidth
	mpoMenuItemInfo->MiddleWidth = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_MIDDLE_WIDTH, DEFAULT_MIDDLE_WIDTH);

	// FontName
	mpTemplete->QuerySubKeyValue(TF_ID_POPUPMENU_FONT_NAME, mpoMenuItemInfo->FontName, sizeof(mpoMenuItemInfo->FontName) * sizeof(wchar_t));

	// FontSize
	mpoMenuItemInfo->FontSize = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_FONT_SIZE, DEFAULT_FONT_SIZE);

	// FontColor
	mpoMenuItemInfo->FontColor = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_FONT_COLOR, DEFAULT_FONT_COLOR);

	// FontDisabledColor
	mpoMenuItemInfo->FontDisableColor = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_FONT_DISABLE_COLOR, DEFAULT_FONT_DISABLE_COLOR);

	// FontFocusColor
	mpoMenuItemInfo->FontFocusColor = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_FONT_FOCUS_COLOR, DEFAULT_FONT_FOCUS_COLOR);

	// ItemHeight
	mpoMenuItemInfo->ItemHeight = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_ITEM_HEIGHT, -1);
}

ERESULT CEvPopupMenu::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������
	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	switch (npMsg->GetMessageID())
	{
	case EACT_POPUPMENU_IS_MANAGER_MENUITEM_ENABLE:
	{
		bool* lpbIsSet = NULL;
		if (ERESULT_SUCCESS != CExMessage::GetInputDataBuffer(npMsg, lpbIsSet))
		{
			luResult = ERESULT_WRONG_PARAMETERS;
			break;
		}

		mbIsManagerMenuItemEnable = *lpbIsSet;

		luResult = ERESULT_SUCCESS;

		break;
	}
	case EEVT_MENUITEM_CLICK:
	{
		this->GetIterator()->PostMessageToParent(npMsg);

		// ���Ӳ˵�����ʱ��Ҫ���ص�����
		mpIterator->SetVisible(false);

		luResult = ERESULT_SUCCESS;
	}
	break;

	case EEVT_MENUITEM_MOUSE_HOVER:
	{
		mpIterator->PostMessageToParent(npMsg);
		luResult = ERESULT_SUCCESS;
	}
	break;

	case EEVT_MENUITEM_GET_FOCUS:				// ���Ӳ˵������˽���
	{
		IEinkuiIterator* lpIterMenuItem = npMsg->GetMessageSender();
		// �����ǵ�ǰPopupMenu�����������MenuItem
		if (NULL != lpIterMenuItem)
			OnMenuItemGetFocused(lpIterMenuItem);

		luResult = ERESULT_SUCCESS;
	}
	break;

	case EACT_POPUPMENU_INSERT_MENUITEM_BY_CREATE:			// �����µĲ˵���
	{
		if (npMsg->GetInputDataSize() != sizeof(STCTL_POPUPMENU_MENUITEMINFO)
			|| npMsg->GetInputData() == NULL)
			break;

		STCTL_POPUPMENU_MENUITEMINFO ldInfo = *(PSTCTL_POPUPMENU_MENUITEMINFO)npMsg->GetInputData();
		if (false == OnInsertMenuItem(ldInfo))
			break;

		luResult = ERESULT_SUCCESS;
	}
	break;

	case EACT_POPUPMENU_INSERT_MENUITEM_BY_EXIST:
	{
		PSTCTL_POPUPMENU_MENUITEMINSERT lpdMenuItemInserted = NULL;
		if (ERESULT_SUCCESS != CExMessage::GetInputDataBuffer(npMsg, lpdMenuItemInserted))
		{
			luResult = ERESULT_UNSUCCESSFUL;
			break;
		}
		BREAK_ON_FALSE(lpdMenuItemInserted->MenuItem->GetElementObject()->GlobleVerification(L"MenuItem"));

		CEvMenuItem* lpoMenuItem = (CEvMenuItem*)lpdMenuItemInserted->MenuItem->GetElementObject();

		moMenuItemVec.Insert(lpdMenuItemInserted->Index, lpoMenuItem);

		lpoMenuItem->SetBgBitmapPtr(mpoItemBgBitmap, miItemBgFrameCount);

		lpoMenuItem->SetMenuItemInfo(mpoMenuItemInfo);

		// ���¼���PopupMenu����
		ReLayoutMenuItem();

		luResult = ERESULT_SUCCESS;
	}
	break;

	case EACT_POPUPMENU_RELAYOUT_MENUITEM:			// ���²���
	{
		ReLayoutMenuItem();

		luResult = ERESULT_SUCCESS;
	}
	break;

	case EACT_POPUPMENU_DELETE_MENUITEM_BY_COMMANDID:
	{
		if (npMsg->GetInputDataSize() != sizeof(UINT) || NULL == npMsg->GetInputData())
			break;

		if (false == DeleteItemByCommandID(*(UINT*)npMsg->GetInputData()))
			break;

		luResult = ERESULT_SUCCESS;
	}
	break;

	case EACT_POPUPMENU_DELETE_MENUITEM_BY_INDEX:
	{
		if (npMsg->GetInputDataSize() != sizeof(int) || NULL == npMsg->GetInputData())
			break;

		if (false == DeleteItemByIndex(*(int*)npMsg->GetInputData()))
			break;

		luResult = ERESULT_SUCCESS;
	}
	break;

	case EACT_POPUPMENU_GET_MENUITEM_BY_COMMANDID:
	{
		if (npMsg->GetInputDataSize() != sizeof(UINT) || NULL == npMsg->GetInputData()
			|| npMsg->GetOutputBufferSize() != sizeof(IEinkuiIterator*) || npMsg->GetOutputBuffer() == NULL)
			break;

		IEinkuiIterator** lpIter = (IEinkuiIterator**)npMsg->GetOutputBuffer();
		UINT luCommandID = *(UINT*)npMsg->GetInputData();
		(*lpIter) = GetItemByCommandID(luCommandID);
		BREAK_ON_NULL(*lpIter);

		luResult = ERESULT_SUCCESS;
	}
	break;

	case EACT_POPUPMENU_GET_MENUITEM_BY_INDEX:
	{
		if (npMsg->GetInputDataSize() != sizeof(UINT) || NULL == npMsg->GetInputData()
			|| npMsg->GetOutputBufferSize() != sizeof(IEinkuiIterator*) || npMsg->GetOutputBuffer() == NULL)
			break;

		IEinkuiIterator** lpIter = (IEinkuiIterator**)npMsg->GetOutputBuffer();
		UINT luIndex = *(UINT*)npMsg->GetInputData();
		(*lpIter) = GetItemByIndex(luIndex);
		BREAK_ON_NULL(*lpIter);

		luResult = ERESULT_SUCCESS;
	}
	break;

	case EMSG_ELEMENT_ACTIVATED:
	{
		//����״̬�ı�
		STEMS_ELEMENT_ACTIVATION* lpActive = NULL;
		luResult = CExMessage::GetInputDataBuffer(npMsg, lpActive);
		if (luResult != ERESULT_SUCCESS)
			break;

		if (lpActive->State == 0 && mpIterator->IsVisible() != false)			// �ɼ�״̬��ʧȥ����״̬��
		{
			mpIterator->SetVisible(false);
		}

		luResult = ERESULT_SUCCESS;
	}
	break;


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


// ������
//		�������Ӳ˵����ý���
void CEvPopupMenu::OnMenuItemGetFocused(
	IN IEinkuiIterator* npIterMenuItem
)
{
	// ������Ϣ�����������˵��ʧȥ����
	STEMS_STATE_CHANGE ldStateChange;
	ldStateChange.Related = npIterMenuItem;
	ldStateChange.State = 0;
	for (int i = 0; i < moMenuItemVec.Size(); ++i)
	{
		IEinkuiIterator* lpMenuItem = moMenuItemVec.GetEntry(i)->GetIterator();
		if (NULL != lpMenuItem && npIterMenuItem != lpMenuItem)
		{
			EinkuiGetSystem()->GetElementManager()->SimplePostMessage(
				lpMenuItem, EMSG_MOUSE_FOCUS, &ldStateChange, sizeof(STEMS_STATE_CHANGE));
		}
	}
}

ERESULT CEvPopupMenu::OnElementShow(bool nbIsShow)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if (false != nbIsShow && false != mbIsManagerMenuItemEnable)			// ��ʾ��ʱ�򣬲鿴��������Ŀ���״̬
		{
			//// ֪ͨ�����Ӳ˵���
			//IUnificSetting* lpoUnificSetting = GetUnificSetting();
			//BREAK_ON_NULL(lpoUnificSetting);
			//for(int i = 0; i < moMenuItemVec.Size(); ++i)
			//{
			//	moMenuItemVec.GetEntry(i)->GetIterator()->SetEnable(lpoUnificSetting->GetItemEnable(moMenuItemVec.GetEntry(i)->GetCommandID()));
			//}
		}
		else							// ���ص�ʱ��Ҫ���ؼ����˵�
		{
			for (int i = 0; i < moMenuItemVec.Size(); ++i)
			{
				moMenuItemVec.GetEntry(i)->HideCascadeMenu();
			}
		}



		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}

// ������
//		��ȡ����MainID��PopupMenu(����MenuItem����������PopupMenu)
// ����ֵ��
//		�ɹ����ض�ӦID�ĵ�������ʧ�ܷ���NULL
IEinkuiIterator* CEvPopupMenu::GetPopupMenuByMainID(
	IN UINT niUinqueID
)
{
	IEinkuiIterator* lpoResult = NULL;
	do
	{
		if (niUinqueID == miMainID)
		{
			lpoResult = mpIterator;
			break;
		}

		for (int i = 0; i < moMenuItemVec.Size(); ++i)
		{
			if (moMenuItemVec.GetEntry(i)->GetMenuItemType() == CEvMenuItem::ENUM_MENUITEM_TYPE_APPLE_EXTEND
				|| moMenuItemVec.GetEntry(i)->GetMenuItemType() == CEvMenuItem::ENUM_MENUITEM_TYPE_EXTEND)
			{
				CEvPopupMenu* lpoPopupMenu = moMenuItemVec.GetEntry(i)->GetPopupMenu();
				if (NULL != lpoPopupMenu)
				{
					lpoResult = lpoPopupMenu->GetPopupMenuByMainID(niUinqueID);
					if (NULL != lpoResult)
						break;
				}
			}
		}

	} while (false);
	return lpoResult;
}

// ������
//		�����µĲ˵���
bool CEvPopupMenu::OnInsertMenuItem(
	IN STCTL_POPUPMENU_MENUITEMINFO ndMenuInfo
)
{
	bool lbResult = false;
	do
	{
		if (ndMenuInfo.Type == (UINT)CEvMenuItem::ENUM_MENUITEM_TYPE_NORMAL)
		{
			CEvMenuItem* lpoMenuItem = CEvMenuItem::CreateInstance(mpIterator, NULL, ndMenuInfo.MenuItemId);
			BREAK_ON_NULL(lpoMenuItem);

			moMenuItemVec.Insert(ndMenuInfo.Index, lpoMenuItem);

			lpoMenuItem->GetIterator()->SetSize((FLOAT)mpTemplete->QuerySubKeyValueAsLONG(L"MenuItem/Width", 0), (FLOAT)mpTemplete->QuerySubKeyValueAsLONG(L"MenuItem/Height", 0));
			lpoMenuItem->SetCommandID(ndMenuInfo.UniqueMenuItemId);

			lpoMenuItem->SetBgBitmapPtr(mpoItemBgBitmap, miItemBgFrameCount);

			lpoMenuItem->SetMenuItemType((CEvMenuItem::ENUM_MENUITEM_TYPE)ndMenuInfo.Type);

			lpoMenuItem->SetMenuItemInfo(mpoMenuItemInfo);

			CExMessage::SendMessage(lpoMenuItem->GetIterator(), mpIterator, EACT_MENUITEM_CHANGE_TEXT, ndMenuInfo.MenuText);

			if (ndMenuInfo.HotKeyInfo != NULL)
			{
				CExMessage::SendMessage(lpoMenuItem->GetIterator(), mpIterator, EACT_MENUITEM_CHANGE_HOTKEY, *ndMenuInfo.HotKeyInfo);
			}

			// ���¼���PopupMenu����
			ReLayoutMenuItem();
		}
		else
			break;

		lbResult = true;
	} while (false);
	return lbResult;
}


// ������
//		���¼���˵���Ĳ���
void CEvPopupMenu::ReLayoutMenuItem()
{
	if (0 == moMenuItemVec.Size())
	{
		return;
	}
	int liMaxNameWidth = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_TEXT_WIDTH, 0);				// ������ƿ��
	int liMaxHotKeyWidth = 0;			// ����ȼ����
	for (int i = 0; i < this->moMenuItemVec.Size(); ++i)
	{
		// ��ȡ������MenuItem�ĸ����ֵ������
		if (liMaxNameWidth < moMenuItemVec.GetEntry(i)->GetNameAreaWidth())
		{
			liMaxNameWidth = moMenuItemVec.GetEntry(i)->GetNameAreaWidth();
		}
		if (liMaxHotKeyWidth < moMenuItemVec.GetEntry(i)->GetHotKeyAreaWidth())
		{
			liMaxHotKeyWidth = moMenuItemVec.GetEntry(i)->GetHotKeyAreaWidth();
		}
	}

	// ��ȡMenuItem��ʾ��λ��
	D2D1_RECT_F ldAlignRect;
	ldAlignRect.left = (FLOAT)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_ALIGN_LEFT, 0);
	ldAlignRect.right = (FLOAT)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_ALIGN_RIGHT, 0);
	ldAlignRect.top = (FLOAT)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_ALIGN_TOP, 0);
	ldAlignRect.bottom = (FLOAT)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_ALIGN_BOTTOM, 0);


	// ���ú�MenuItem����Ϣ
	IEinkuiIterator* lpIter = NULL;
	float lfItemHeightCount = ldAlignRect.top;
	for (int i = 0; i < this->moMenuItemVec.Size(); ++i)
	{
		// ���úò˵����ƺ��ȼ�������
		moMenuItemVec.GetEntry(i)->SetNameAreaWidth(liMaxNameWidth);
		moMenuItemVec.GetEntry(i)->SetHotKeyAreaWidth(liMaxHotKeyWidth);

		// �ò˵����Լ����³ߴ�
		moMenuItemVec.GetEntry(i)->UpdateSize();

		if (false != moMenuItemVec.GetEntry(i)->GetIterator()->IsVisible())
		{
			// ���úø����λ��
			moMenuItemVec.GetEntry(i)->GetIterator()->SetPosition(ldAlignRect.left,
				lfItemHeightCount);

			lfItemHeightCount += moMenuItemVec.GetEntry(i)->GetIterator()->GetSizeY();
		}
	}

	// ���ú�PopupMenu�Ĵ�С
	FLOAT lfWidth = ldAlignRect.left + moMenuItemVec.GetEntry(0)->GetIterator()->GetSizeX() + ldAlignRect.right;
	FLOAT lfHeight = lfItemHeightCount + ldAlignRect.bottom;
	mpIterator->SetSize(lfWidth, lfHeight);
}

// ������
//		����CommandIDɾ���˵���
bool CEvPopupMenu::DeleteItemByCommandID(
	IN UINT niCommandID
)
{
	bool lbResult = false;
	do
	{
		int i = 0;
		for (; i < moMenuItemVec.Size(); ++i)
		{
			if (moMenuItemVec.GetEntry(i)->GetCommandID() == niCommandID)
			{
				break;
			}
		}

		if (i == moMenuItemVec.Size())
			break;

		IEinkuiIterator* lpoIter = moMenuItemVec.GetEntry(i)->GetIterator();
		if (false == moMenuItemVec.RemoveByIndex(i))
			break;

		if (NULL != lpoIter)
		{
			lpoIter->Close();
		}


		ReLayoutMenuItem();

		lbResult = true;
	} while (false);
	return lbResult;

}

// ������
//		��������ɾ���˵���,����-1ɾ��ȫ����
bool CEvPopupMenu::DeleteItemByIndex(
	IN int niIndex
)
{
	bool lbResult = false;
	do
	{
		if (niIndex >= 0 &&
			niIndex < moMenuItemVec.Size())
		{
			IEinkuiIterator* lpoIter = moMenuItemVec.GetEntry(niIndex)->GetIterator();
			if (false == moMenuItemVec.RemoveByIndex(niIndex))
				break;

			if (NULL != lpoIter)
			{
				lpoIter->Close();
			}
		}
		else if (-1 == niIndex)
		{
			for (int i = 0; i < moMenuItemVec.Size(); ++i)
			{
				moMenuItemVec.GetEntry(i)->GetIterator()->Close();
			}
			moMenuItemVec.Clear();
		}
		else
			break;

		ReLayoutMenuItem();

		lbResult = true;
	} while (false);
	return lbResult;
}

// ������
//		����CommandID��ȡ�˵���
IEinkuiIterator* CEvPopupMenu::GetItemByCommandID(
	IN UINT niCommandID
)
{
	IEinkuiIterator* lpoIter = NULL;
	do
	{
		for (int i = 0; i < moMenuItemVec.Size(); ++i)
		{
			if (moMenuItemVec.GetEntry(i)->GetCommandID() == niCommandID)
			{
				lpoIter = moMenuItemVec.GetEntry(i)->GetIterator();
				break;
			}
		}

	} while (false);
	return lpoIter;
}



// ������
//		����������ȡ�˵���
IEinkuiIterator* CEvPopupMenu::GetItemByIndex(
	IN UINT niIndex
)
{
	IEinkuiIterator* lpoIter = NULL;
	do
	{
		if (niIndex < (UINT)moMenuItemVec.Size())
			lpoIter = moMenuItemVec.GetEntry(niIndex)->GetIterator();

	} while (false);
	return lpoIter;
}