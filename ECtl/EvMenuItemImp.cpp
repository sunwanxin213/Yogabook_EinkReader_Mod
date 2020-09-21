#include "StdAfx.h"
#include "Einkui.h"
#include "XCtl.h"
#include "ElementImp.h"
#include "EvListImp.h"
#include "EvPopupMenuImp.h"
#include "EvMenuItemImp.h"




DEFINE_BUILTIN_NAME(MenuItem)

CEvMenuItem::CEvMenuItem(void)
{
	meMemuItemType = ENUM_MENUITEM_TYPE_NORMAL;
	miCommandID = -1;
	miShotcutKey = -1;

	mpoFocusTextImage = NULL;
	mpoFocusHotKeyImage = NULL;
	mpoTextImage = NULL;
	mpoLeftImage = NULL;
	mpoRightImage = NULL;
	mpoSeparation = NULL;
	mpoCheckedImage = NULL;
	mpoHotKeyImage = NULL;
	mpoItemBgBmp = NULL;

	miItemFrameCount = -1;

	miNameAreaWidth = 0;
	miHotKeyAreaWidth = 0;

	mbIsChecked = false;
	mbIsShowExtendMenu = false;

	miExtendMenuID = -1;

	mpoMenuItemInfo = NULL;

	mpoSubPopupMenu = NULL;

	miDuration = 0;

	mbIsFocused = false;

	mbIsSubMenuShow = false;

	ZeroMemory(mswMenuTittle, MAX_TITTLE_LENGTH * sizeof(wchar_t));
	ZeroMemory(mswHotKey, MAX_HOTKEY_LENGTH * sizeof(wchar_t));
}


CEvMenuItem::~CEvMenuItem(void)
{
	CMM_SAFE_RELEASE(mpoTextImage);

	CMM_SAFE_RELEASE(mpoLeftImage);

	CMM_SAFE_RELEASE(mpoRightImage);

	CMM_SAFE_RELEASE(mpoSeparation);

	CMM_SAFE_RELEASE(mpoCheckedImage);

	CMM_SAFE_RELEASE(mpoHotKeyImage);

	CMM_SAFE_RELEASE(mpoFocusHotKeyImage);

	CMM_SAFE_RELEASE(mpoFocusTextImage);

	CMM_SAFE_DELETE(mpoMenuItemInfo);

	mpoItemBgBmp = NULL;
}


ULONG CEvMenuItem::InitOnCreate(
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

		leResult = ERESULT_SUCCESS;
	} while (false);


	return leResult;
}
//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEvMenuItem::OnElementCreate(IEinkuiIterator* npIterator)
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

//ERESULT CEvMenuItem::OnElementDestroy()
//{
//	ERESULT lResult = ERESULT_UNSUCCESSFUL;
//
//	return lResult;
//}
//
//������Ϣ
ERESULT CEvMenuItem::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npPaintBoard);
		if (NULL == mpoMenuItemInfo)
			break;

		// �ָ���
		if (ENUM_MENUITEM_TYPE_SEPARATION == meMemuItemType)
		{
			if (mpoSeparation == NULL)
				break;

			// ��ʾ�ָ���
			npPaintBoard->DrawBitmap(D2D1::RectF((FLOAT)mpoMenuItemInfo->LeftWidth, 0,
				(FLOAT)(mpIterator->GetSizeX()), (FLOAT)mpIterator->GetSizeY()),
				mpoSeparation, ESPB_DRAWBMP_EXTEND);

			lResult = ERESULT_SUCCESS;
			break;
		}


		FLOAT lfL = mpIterator->GetSizeX();
		// ��ʾ����
		if (mpoItemBgBmp != NULL)
		{
			// �����ǰ�������꽹�㣬���õڶ�֡������ͼ
			FLOAT lfOrgX = (FLOAT)(false != mbIsFocused ? CExFloat::Round((FLOAT)mpoItemBgBmp->GetWidth() / miItemFrameCount) : 0);
			npPaintBoard->DrawBitmap(D2D1::RectF(0, 0, (FLOAT)mpIterator->GetSizeX(), (FLOAT)mpIterator->GetSizeY()),
				D2D1::RectF(lfOrgX, 0, CExFloat::Round(lfOrgX + mpoItemBgBmp->GetWidth() / miItemFrameCount), (FLOAT)(mpoItemBgBmp->GetHeight())),
				mpoItemBgBmp, ESPB_DRAWBMP_EXTEND);
		}

		// �Ǹ�������
		if (ENUM_MENUITEM_TYPE_SEPARATION != meMemuItemType)
		{
			// ��ʾ����
			if (NULL != mpoTextImage)
			{
				FLOAT lfX = (FLOAT)mpoMenuItemInfo->LeftWidth;
				FLOAT lfY = CExFloat::Round((mpIterator->GetSizeY() - mpoTextImage->GetHeight()) / 2.0f);		// ��ֱ������ʾ
				npPaintBoard->DrawBitmap(D2D1::RectF(lfX, lfY,
					(FLOAT)mpoTextImage->GetWidth() + lfX, (FLOAT)mpoTextImage->GetHeight() + lfY),
					false != mbIsFocused ? mpoFocusTextImage : mpoTextImage, ESPB_DRAWBMP_NEAREST);
			}

			// ��ʾ�ȼ�
			if (NULL != mpoHotKeyImage)
			{
				FLOAT lfX = (FLOAT)(mpoMenuItemInfo->LeftWidth + miNameAreaWidth + mpoMenuItemInfo->MiddleWidth
					+ miHotKeyAreaWidth - mpoHotKeyImage->GetWidth());
				FLOAT lfY = CExFloat::Round((mpIterator->GetSizeY() - mpoHotKeyImage->GetHeight()) / 2.0f);		// ��ֱ������ʾ
				npPaintBoard->DrawBitmap(D2D1::RectF(lfX, lfY,
					(FLOAT)mpoHotKeyImage->GetWidth() + lfX, (FLOAT)mpoHotKeyImage->GetHeight() + lfY),
					false != mbIsFocused ? mpoFocusHotKeyImage : mpoHotKeyImage, ESPB_DRAWBMP_NEAREST);
			}



			// ����Ҫ���ݲ�ͬ��������ʾ��ͬ������
			FLOAT lfX = 0.0f;
			FLOAT lfY = 0.0f;
			FLOAT lfRight = 0.0f;
			FLOAT lfBottom = 0.0f;
			// ��ͼ����
			if (ENUM_MENUITEM_TYPE_ICON == meMemuItemType
				&& NULL != mpoLeftImage)
			{
				lfX = CExFloat::Round((mpoMenuItemInfo->LeftWidth - mpoMenuItemInfo->LeftIconWidth) / 2.0f);
				lfY = CExFloat::Round((mpIterator->GetSizeY() - mpoMenuItemInfo->LeftIconHeight) / 2.0f);
				lfRight = (FLOAT)mpoMenuItemInfo->LeftIconWidth + lfX;
				lfBottom = (FLOAT)mpoMenuItemInfo->LeftIconHeight + lfY;
				npPaintBoard->DrawBitmap(D2D1::RectF(lfX, lfY, lfRight, lfBottom),
					mpoLeftImage, ESPB_DRAWBMP_EXTEND);
			}
			else if (ENUM_MENUITEM_TYPE_CHECK == meMemuItemType
				&& NULL != mpoLeftImage && NULL != mpoCheckedImage)
			{
				lfX = CExFloat::Round((mpoMenuItemInfo->LeftWidth - mpoMenuItemInfo->LeftIconWidth) / 2.0f);
				lfY = CExFloat::Round((mpIterator->GetSizeY() - mpoMenuItemInfo->LeftIconHeight) / 2.0f);
				lfRight = (FLOAT)mpoMenuItemInfo->LeftIconWidth + lfX;
				lfBottom = (FLOAT)mpoMenuItemInfo->LeftIconHeight + lfY;

				npPaintBoard->DrawBitmap(D2D1::RectF(lfX, lfY, lfRight, lfBottom),
					false != mbIsChecked ? mpoCheckedImage : mpoLeftImage, ESPB_DRAWBMP_EXTEND);
			}
			else if (ENUM_MENUITEM_TYPE_EXTEND == meMemuItemType
				&& NULL != mpoRightImage)
			{
				lfX = (FLOAT)(mpoMenuItemInfo->LeftWidth + miNameAreaWidth + mpoMenuItemInfo->MiddleWidth + miHotKeyAreaWidth
					+ CExFloat::Round((mpoMenuItemInfo->RightWidth - mpoMenuItemInfo->RightIconWidth) / 2.0f));
				lfY = CExFloat::Round((mpIterator->GetSizeY() - mpoMenuItemInfo->RightIconHeight) / 2.0f);
				lfRight = (FLOAT)mpoMenuItemInfo->RightIconWidth + lfX;
				lfBottom = (FLOAT)mpoMenuItemInfo->RightIconHeight + lfY;


				npPaintBoard->DrawBitmap(D2D1::RectF(lfX, lfY, lfRight, lfBottom),
					mpoRightImage, ESPB_DRAWBMP_EXTEND);
			}
			else if (ENUM_MENUITEM_TYPE_APPLE_EXTEND == meMemuItemType
				&& NULL != mpoRightImage)
			{
				lfX = (FLOAT)(mpoMenuItemInfo->LeftWidth + miNameAreaWidth + mpoMenuItemInfo->MiddleWidth
					+ miHotKeyAreaWidth - mpoMenuItemInfo->RightIconWidth);
				lfY = CExFloat::Round((mpIterator->GetSizeY() - mpoMenuItemInfo->RightIconHeight) / 2.0f);
				lfRight = (FLOAT)mpoMenuItemInfo->RightIconWidth + lfX;
				lfBottom = (FLOAT)mpoMenuItemInfo->RightIconHeight + lfY;

				npPaintBoard->DrawBitmap(D2D1::RectF(lfX, lfY, lfRight, lfBottom),
					mpoRightImage, ESPB_DRAWBMP_EXTEND);
			}

		}
		lResult = ERESULT_SUCCESS;

	} while (false);

	return lResult;
}

void CEvMenuItem::LoadResource()
{
	bool lbResult = false;
	do
	{
		if (NULL == mpTemplete)
			break;

		if (NULL == mpoMenuItemInfo)
			break;

		// ��ȡID
		miCommandID = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_MENUITEM_COMMAND_ID, -1);

		// ��ȡ����
		int liType = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_MENUITEM_TYPE, 1);
		meMemuItemType = (ENUM_MENUITEM_TYPE)liType;

		if (ENUM_MENUITEM_TYPE_SEPARATION == meMemuItemType)			// �ָ���
		{
			wchar_t* lswSeparationImagePath = (wchar_t*)mpTemplete->QuerySubKeyValueAsBuffer(TF_ID_MENUITEM_SEPARATION);
			if (NULL != lswSeparationImagePath)
			{
				mpoSeparation = EinkuiGetSystem()->GetAllocator()->LoadImageFile(lswSeparationImagePath, lswSeparationImagePath[0] != L'.' ? true : false);
				mpTemplete->ReleaseBuffer(&lswSeparationImagePath);

				// ����Ƿָ��ߣ���Ҫ�Էָ��ߵĸ߶���Ϊ��߶�
				mpoMenuItemInfo->ItemHeight = (int)mpoSeparation->GetHeight();
			}

			lbResult = true;
			break;
		}

		// �������͹�������
		FLOAT lfHeight = 0;
		if (NULL != mpoItemBgBmp)
		{
			lfHeight = (FLOAT)mpoItemBgBmp->GetHeight();
		}

		if (meMemuItemType == ENUM_MENUITEM_TYPE_ICON
			|| meMemuItemType == ENUM_MENUITEM_TYPE_CHECK)
		{
			// ��ȡ��ͼ��
			wchar_t* lswLeftImage = (wchar_t*)mpTemplete->QuerySubKeyValueAsBuffer(TF_ID_MENUITEM_LEFT_ICON);
			if (NULL != lswLeftImage)
			{
				mpoLeftImage = EinkuiGetSystem()->GetAllocator()->LoadImageFile(lswLeftImage, lswLeftImage[0] == L'.' ? false : true);
				mpTemplete->ReleaseBuffer(&lswLeftImage);

				if (-1 == mpoMenuItemInfo->LeftIconWidth)
				{
					mpoMenuItemInfo->LeftIconWidth = mpoLeftImage->GetWidth();
				}
				if (-1 == mpoMenuItemInfo->LeftIconHeight)
				{
					mpoMenuItemInfo->LeftIconHeight = mpoLeftImage->GetHeight();
				}
			}


			// ��ȡChecked̬ͼ��
			if (meMemuItemType == ENUM_MENUITEM_TYPE_CHECK)
			{
				wchar_t* lswCheckedImage = (wchar_t*)mpTemplete->QuerySubKeyValueAsBuffer(TF_ID_MENUITEM_LEFT_CHECKED_ICON);
				if (NULL != lswCheckedImage)
				{
					mpoCheckedImage = EinkuiGetSystem()->GetAllocator()->LoadImageFile(lswCheckedImage, lswCheckedImage[0] == L'.' ? false : true);
					mpTemplete->ReleaseBuffer(&lswCheckedImage);
				}
			}
		}
		else if (meMemuItemType == ENUM_MENUITEM_TYPE_APPLE_EXTEND ||
			meMemuItemType == ENUM_MENUITEM_TYPE_EXTEND)
		{
			// ��ȡ��ͼ��
			wchar_t* lswRightImage = (wchar_t*)mpTemplete->QuerySubKeyValueAsBuffer(TF_ID_MENUITEM_RIGHT_ICON);
			if (NULL != lswRightImage)
			{
				mpoRightImage = EinkuiGetSystem()->GetAllocator()->LoadImageFile(lswRightImage, lswRightImage[0] == L'.' ? false : true);

				mpTemplete->ReleaseBuffer(&lswRightImage);

				if (-1 == mpoMenuItemInfo->RightIconWidth)
				{
					mpoMenuItemInfo->RightIconWidth = mpoRightImage->GetWidth();
				}
				if (-1 == mpoMenuItemInfo->RightIconHeight)
				{
					mpoMenuItemInfo->RightIconHeight = mpoRightImage->GetHeight();
				}
			}

			// ��ȡ�����˵�ID
			miExtendMenuID = (int)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_MENUITEM_POPUPMENU_ID, -1);


			// ��ȡ����
			miDuration = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_MENUITEM_DURATION, DEFAULT_TIMER_DURATION);

		}

		// ��ȡ�˵�����
		wchar_t* lswMenuText = (wchar_t*)mpTemplete->QuerySubKeyValueAsBuffer(TF_ID_MENUITEM_TEXT);
		long llResult = LoadTextImage(lswMenuText);
		mpTemplete->ReleaseBuffer(&lswMenuText);
		if (-1 != llResult && llResult > lfHeight)
			lfHeight = (FLOAT)llResult;


		// ���ò˵��ȼ�
		wchar_t* lswHotKey = (wchar_t*)mpTemplete->QuerySubKeyValueAsBuffer(TF_ID_MENUITEM_HOTKEY);
		if (NULL != lswHotKey && UNICODE_NULL != lswHotKey[0])
		{
			STCTL_MENUITEM_HOTKEY ldHotKeyInfo;
			RtlZeroMemory(&ldHotKeyInfo, sizeof(STCTL_MENUITEM_HOTKEY));
			StringCchCopyW(ldHotKeyInfo.HotKeyToShow, MAX_PATH, lswHotKey);
			mpTemplete->ReleaseBuffer(&lswHotKey);

			// VirtualKey
			ldHotKeyInfo.VirtualKey = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_MENUITEM_HOTKEY_VIRTUAL_KEY, -1);
			if (-1 != ldHotKeyInfo.VirtualKey)
			{
				// Ctrl
				ICfKey* lpoKey = mpTemplete->OpenKey(TF_ID_MENUITEM_HOTKEY_CTRL_KEY);
				if (NULL != lpoKey)
					ldHotKeyInfo.NeedCtrl = true;

				// Shift
				CMM_SAFE_RELEASE(lpoKey);
				lpoKey = mpTemplete->OpenKey(TF_ID_MENUITEM_HOTKEY_SHAFT_KEY);
				if (NULL != lpoKey)
					ldHotKeyInfo.NeedShift = true;

				// Alt
				CMM_SAFE_RELEASE(lpoKey);
				lpoKey = mpTemplete->OpenKey(TF_ID_MENUITEM_HOTKEY_ALT_KEY);
				if (NULL != lpoKey)
					ldHotKeyInfo.NeedAlt = true;

				CMM_SAFE_RELEASE(lpoKey);

				llResult = LoadHotkeyImage(&ldHotKeyInfo);
				if (-1 != llResult && llResult > lfHeight)
					lfHeight = (FLOAT)llResult;
			}
		}

		// ���ò˵���߶�
		if (-1 == mpoMenuItemInfo->ItemHeight)
			mpoMenuItemInfo->ItemHeight = (int)lfHeight;

		lbResult = true;
	} while (false);


	if (false == lbResult)
	{
		PrintDebugString(L"MenuItem_LoadResource ʧ��.");
	}
}

// �����Ӳ˵��б�
void CEvMenuItem::LoadSubPopupMenu()
{
	if (-1 == miExtendMenuID)
	{
		return;
	}

	// ���жϸ������Ƿ����
	if (NULL == mpIterator->GetParent())
		return;

	// �����Ӳ˵�ģ�壬������
	ICfKey* lpoPopupMenuKey = mpTemplete->GetParentsKey();
	while (NULL != lpoPopupMenuKey->GetParentsKey())
		lpoPopupMenuKey = lpoPopupMenuKey->GetParentsKey();
	lpoPopupMenuKey = lpoPopupMenuKey->GetSubKey(L"PopupMenu");
	if (NULL == lpoPopupMenuKey)
		return;

	// ��Ѱ��IDΪmiExtendMenuID���Ӳ˵�
	lpoPopupMenuKey = lpoPopupMenuKey->MoveToSubKey();
	while (NULL != lpoPopupMenuKey)
	{
		if (miExtendMenuID == lpoPopupMenuKey->QuerySubKeyValueAsLONG(TF_ID_POPUPMENU_MAIN_ID, -1))
			break;
		else
			lpoPopupMenuKey = lpoPopupMenuKey->MoveToNextKey();
	}

	if (NULL == lpoPopupMenuKey)
		return;

	mpoSubPopupMenu = CEvPopupMenu::CreateInstance(mpIterator, lpoPopupMenuKey, 0);
	if (NULL == mpoSubPopupMenu)
		return;

	mpoSubPopupMenu->GetIterator()->SetPosition(mpIterator->GetSizeX() - 5, 0);
	mpoSubPopupMenu->GetIterator()->SetVisible(false);
}


// ������
//		���ó�ʼ���ṹ���ָ�루����֮����˵������ָ�룬��Ϊ�˱�ʾ����Ҫȥ�ͷţ�
void CEvMenuItem::SetMenuItemInfo(IN PST_MENUITEM_INFO npoMenuItemInfo)
{
	CMM_SAFE_DELETE(mpoMenuItemInfo);
	mpoMenuItemInfo = new ST_MENUITEM_INFO;
	// CheckMarx fix by zhuhl5
	memcpy_s(mpoMenuItemInfo, sizeof(ST_MENUITEM_INFO), npoMenuItemInfo, sizeof(ST_MENUITEM_INFO));

	LoadResource();

}

// ��갴��
ERESULT CEvMenuItem::OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		// ����������
		if (npInfo->ActKey & MK_LBUTTON
			&& false == npInfo->Presssed && false != mpIterator->IsEnable())
		{
			if (ENUM_MENUITEM_TYPE_NORMAL == meMemuItemType
				|| ENUM_MENUITEM_TYPE_ICON == meMemuItemType
				|| ENUM_MENUITEM_TYPE_CHECK == meMemuItemType)
			{
				// �򸸴��ڷ�����Ϣ
				PostMessageToParent(EEVT_MENUITEM_CLICK, miCommandID);

				// �ָ�������ʾ״̬
				mbIsFocused = false;
			}
		}

		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}

ERESULT CEvMenuItem::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������
	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	switch (npMsg->GetMessageID())
	{
	case EEVT_MENUITEM_CLICK:
	{
		this->GetIterator()->PostMessageToParent(npMsg);

		luResult = ERESULT_SUCCESS;
	}
	break;

	case EACT_MENUITEM_SET_CHECK_STATE:
	{
		if (ENUM_MENUITEM_TYPE_CHECK != meMemuItemType)
			break;

		if (npMsg->GetInputDataSize() != sizeof(bool))
			break;
		mbIsChecked = *(bool*)npMsg->GetInputData();

		// ��ȡ�˵����ƺ��ȼ�
		wchar_t* lswMenuText = NULL;
		if (false == mbIsChecked)
		{
			lswMenuText = (wchar_t*)mpTemplete->QuerySubKeyValueAsBuffer(TF_ID_MENUITEM_TEXT);
		}
		else
		{
			lswMenuText = (wchar_t*)mpTemplete->QuerySubKeyValueAsBuffer(TF_ID_MENUITEM_CHECKED_TEXT);
		}
		long llResult = LoadTextImage(lswMenuText);
		if (-1 == mpoMenuItemInfo->ItemHeight
			&& mpIterator->GetSizeY() < (FLOAT)llResult)
		{
			mpoMenuItemInfo->ItemHeight = llResult;
			mpIterator->SetSize(mpIterator->GetSizeX(), (FLOAT)llResult);
		}

		luResult = ERESULT_SUCCESS;
	}
	break;

	case EACT_MENUITEM_CHANGE_TEXT:
	{
		//if(npMsg->GetInputDataSize() != sizeof(wchar_t*) )
			//break;

		wchar_t* lswMenuText = (wchar_t*)npMsg->GetInputData();

		// ���ò˵�����
		long llResult = LoadTextImage(lswMenuText);
		if (-1 == llResult)
			break;
		if (-1 == mpoMenuItemInfo->ItemHeight
			&& mpIterator->GetSizeY() < (FLOAT)llResult)
		{
			mpoMenuItemInfo->ItemHeight = llResult;
			mpIterator->SetSize(mpIterator->GetSizeX(), (FLOAT)llResult);
		}

		luResult = ERESULT_SUCCESS;
	}
	break;

	case EACT_MENUITEM_CHANGE_HOTKEY:
	{
		PSTCTL_MENUITEM_HOTKEY lpdHotKeyInfo = NULL;
		if (ERESULT_SUCCESS != CExMessage::GetInputDataBuffer(npMsg, lpdHotKeyInfo))
		{
			luResult = ERESULT_UNSUCCESSFUL;
			break;
		}

		// �����ȼ�
		long llResult = LoadHotkeyImage(lpdHotKeyInfo);
		if (-1 == llResult)
			break;
		if (-1 == mpoMenuItemInfo->ItemHeight
			&& mpIterator->GetSizeY() < (FLOAT)llResult)
		{
			mpoMenuItemInfo->ItemHeight = llResult;
			mpIterator->SetSize(mpIterator->GetSizeX(), (FLOAT)llResult);
		}

		luResult = ERESULT_SUCCESS;
	}
	break;

	//mpIterator->SetToolTip(L"ksdfjlsdjkf");

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
void CEvMenuItem::OnTimer(
	PSTEMS_TIMER npStatus
)
{
	if (npStatus->TimerID == TIMER_ID_SHOW_POPUPMENU)
	{
		if (NULL != mpoSubPopupMenu)
		{
			// ���ص�ʱ��Ҫȷ����ǰ��겻�ڸò˵������ڵ�PopupMenu��
			mpoSubPopupMenu->GetIterator()->SetVisible(mbIsSubMenuShow);
			EinkuiGetSystem()->UpdateView();
		}
	}
}

//��������뿪
void CEvMenuItem::OnMouseFocus(PSTEMS_STATE_CHANGE npState)
{
	if (ENUM_MENUITEM_TYPE_SEPARATION == meMemuItemType || false == mpIterator->IsEnable())
		return;

	// �������зǷָ������͵Ĳ˵���
	// ֻҪ�����룬����뽹��̬��
	if (0 != npState->State)
	{
		mbIsFocused = true;

		// ͬʱ֪ͨPopupMenu��ȥ��ȡ������Ľ���
		if (NULL != mpIterator->GetParent()
			&& false != mpIterator->GetParent()->GetElementObject()->IsKindOf(GET_BUILTIN_NAME(PopupMenu))
			)
		{
			PostMessageToParent(EEVT_MENUITEM_GET_FOCUS, CExMessage::DataInvalid);

			// ���ͬʱ������ǰ�˵����Ӧ��PopupMenu��һ�������˵�������Ҫ���ø�PopupMenu������Ϊ��ȡ����״̬
			IEinkuiIterator* lpPopupMenuParent = mpIterator->GetParent()->GetParent();
			if (NULL != lpPopupMenuParent
				&& false != lpPopupMenuParent->GetElementObject()->IsKindOf(GET_BUILTIN_NAME(MenuItem))
				)
			{
				STEMS_STATE_CHANGE ldStateChange;
				ldStateChange.Related = NULL;
				ldStateChange.State = 1;

				// ������Ϣ����Ӧ�ĸ�MenuItem���趨��Ϊѡ��״̬��
				EinkuiGetSystem()->GetElementManager()->SimplePostMessage(
					lpPopupMenuParent, EMSG_MOUSE_FOCUS, &ldStateChange, sizeof(STEMS_STATE_CHANGE));
			}
		}
	}
	// ��������뿪�˲��Ҳ��Ǳ���������ȡʱ����ʧȥ����
	else if (NULL != npState->Related && false == npState->Related->FindAncestor(mpIterator))
	{
		mbIsFocused = false;
	}

	// �����ǰ�˵���Ϊ��չ����
	if (ENUM_MENUITEM_TYPE_EXTEND == meMemuItemType || ENUM_MENUITEM_TYPE_APPLE_EXTEND == meMemuItemType)
	{
		// ȷ���Ӳ˵��Ѿ�����
		if (NULL == mpoSubPopupMenu)
		{
			LoadSubPopupMenu();
			if (NULL == mpoSubPopupMenu)
				return;
		}

		// ���������ʱ��
		if (0 != npState->State)
		{
			// ����Ҫ�رն�ʱ�������������뿪�ͽ���ʱ�������������Ӳ˵��Ķ�ʱ��
			mpIterator->KillTimer(TIMER_ID_SHOW_POPUPMENU);

			// ����Ӳ˵�������״̬��������ʱ��������ʾ�Ӳ˵���
			if (false == mpoSubPopupMenu->GetIterator()->IsVisible())
			{
				// ������ʱ�������Ӳ˵�
				mbIsSubMenuShow = true;
				mpIterator->SetTimer(TIMER_ID_SHOW_POPUPMENU, 1, miDuration, NULL);
			}

		}
		else		// ����뿪��ʱ��
		{
			// ����������ʱ�����Ա���ղŽ������ʱ��������ʾ�Ӳ˵��Ķ�ʱ����������
			mpIterator->KillTimer(TIMER_ID_SHOW_POPUPMENU);

			// ����ȡ����Ĳ��ǵ�ǰ��������������ʱ�������ض����˵���
			if (NULL != npState->Related && false == npState->Related->FindAncestor(mpIterator))
			{
				mbIsSubMenuShow = false;
				// �Ӳ˵�����ʾ״̬��
				if (false != mpoSubPopupMenu->GetIterator()->IsVisible())
					mpIterator->SetTimer(TIMER_ID_SHOW_POPUPMENU, 1, miDuration, NULL);
			}
			else			// �Ǳ������ȡ�˽��㣬����Ȼ��ʾѡ��״̬
			{
				mbIsFocused = true;
			}
		}
	}

	EinkuiGetSystem()->UpdateView();
}

// ��������
ERESULT CEvMenuItem::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	ERESULT luResult = ERESULT_SUCCESS;

	if (!(rPoint.x < 0.0f || rPoint.x >= mpIterator->GetSizeX() || rPoint.y < 0.0f || rPoint.y >= mpIterator->GetSizeY()))
	{
		luResult = ERESULT_MOUSE_OWNERSHIP;
	}

	return luResult;
}

//���û�����
ERESULT CEvMenuItem::OnElementEnable(bool nbIsEnable)
{
	CMM_SAFE_RELEASE(mpoTextImage);
	if (GetMenuItemType() != ENUM_MENUITEM_TYPE_SEPARATION)
	{
		//�����ṹ��
		STETXT_BMP_INIT ldInit;
		ZeroMemory(&ldInit, sizeof(STETXT_BMP_INIT));
		ldInit.FontName = mpoMenuItemInfo->FontName;
		ldInit.FontSize = mpoMenuItemInfo->FontSize;

		if (false != nbIsEnable)
		{
			ldInit.TextColor = mpoMenuItemInfo->FontColor;
		}
		else
		{
			ldInit.TextColor = mpoMenuItemInfo->FontDisableColor;
		}

		ldInit.Text = mswMenuTittle;
		CMM_SAFE_RELEASE(mpoTextImage);
		mpoTextImage = EinkuiGetSystem()->GetAllocator()->CreateImageByText(ldInit);

		if (mswHotKey[0] != UNICODE_NULL)
		{
			ldInit.Text = mswHotKey;
			CMM_SAFE_RELEASE(mpoHotKeyImage);
			mpoHotKeyImage = EinkuiGetSystem()->GetAllocator()->CreateImageByText(ldInit);
		}
	}

	return ERESULT_SUCCESS;
}

//�����ͣ
ERESULT CEvMenuItem::OnMouseHover()
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if (GetMenuItemType() == ENUM_MENUITEM_TYPE_SEPARATION)
			break;

		STCTL_MENUITEM_MOUSE_HOVER ldInfo;
		ldInfo.CommandID = miCommandID;
		ldInfo.MenuItemIter = mpIterator;
		PostMessageToParent(EEVT_MENUITEM_MOUSE_HOVER, ldInfo);

		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}

//��ݼ���Ϣ
ERESULT CEvMenuItem::OnHotKey(const STEMS_HOTKEY* npHotKey)
{
	if (npHotKey->HotKeyID == mlHotKeyID)
	{
		// ģ������Ϣ
		// ����������
		if (false != mpIterator->IsEnable())
		{
			if (ENUM_MENUITEM_TYPE_NORMAL == meMemuItemType
				|| ENUM_MENUITEM_TYPE_ICON == meMemuItemType
				|| ENUM_MENUITEM_TYPE_CHECK == meMemuItemType)
			{
				// �򸸴��ڷ�����Ϣ
				PostMessageToParent(EEVT_MENUITEM_CLICK, miCommandID);
			}
		}
	}

	return ERESULT_KEY_UNEXPECTED;
}


// ������
//		���ز˵�������ͼƬ����������������ʼ����������mpoTextImage poFocusTextImage
// ����ֵ��
//		-1 ��ʾʧ�ܣ����򷵻ش�����ͼƬ�����߶�
//		ע�⣺�������ָ�����ָ����ַ���ָ�룬����������ͼƬ�����ظ߶�Ϊ0
long CEvMenuItem::LoadTextImage(IN wchar_t* nswMenuName)
{
	long lfHeight = -1;
	do
	{
		if (NULL == nswMenuName || UNICODE_NULL == nswMenuName[0])
		{
			CMM_SAFE_RELEASE(mpoTextImage);	//ȥ��ԭ����ͼƬ
			CMM_SAFE_RELEASE(mpoFocusTextImage);	//ȥ��ԭ����ͼƬ
			lfHeight = 0;
			break;
		}

		// ��������ݼ�
		wchar_t* lswShotcutKeyBegin = wcsstr(nswMenuName, L"(&");
		if (NULL != lswShotcutKeyBegin)
		{
			wchar_t* lswShotcutKeyEnd = wcsstr(lswShotcutKeyBegin, L")");
			if (NULL != lswShotcutKeyEnd)
			{
				int liShotcutKey = toupper(*(lswShotcutKeyEnd - 1));
				if (liShotcutKey >= L'A' && liShotcutKey <= L'Z')
				{
					miShotcutKey = liShotcutKey;

					// ������ʾ����
					*(++lswShotcutKeyBegin) = liShotcutKey;
					*(++lswShotcutKeyBegin) = L')';
					*lswShotcutKeyEnd = UNICODE_NULL;
				}
			}
		}

		// �����˵�����ͼƬ
		STETXT_BMP_INIT ldNormalInit;
		ZeroMemory(&ldNormalInit, sizeof(STETXT_BMP_INIT));
		ldNormalInit.FontName = mpoMenuItemInfo->FontName;
		ldNormalInit.FontSize = mpoMenuItemInfo->FontSize;
		ldNormalInit.Text = nswMenuName;
		ldNormalInit.TextColor = mpoMenuItemInfo->FontColor;

		if (mpIterator->GetSizeX() > 0.0f && mpIterator->GetSizeY() > 0.0f)
		{
			//ֻ�������˿�߲������⼸��
			ldNormalInit.Width = (DWORD)mpIterator->GetSizeX();
			ldNormalInit.Height = (DWORD)mpIterator->GetSizeY();
			ldNormalInit.Limit = STETXT_BMP_INIT::EL_FIXEDSIZE;
		}

		CMM_SAFE_RELEASE(mpoTextImage);	//ȥ��ԭ����ͼƬ
		mpoTextImage = EinkuiGetSystem()->GetAllocator()->CreateImageByText(ldNormalInit);
		BREAK_ON_NULL(mpoTextImage);
		miNameAreaWidth = mpoTextImage->GetWidth();

		if (lfHeight < (long)mpoTextImage->GetHeight())
		{
			lfHeight = (long)mpoTextImage->GetHeight();
		}

		ldNormalInit.TextColor = mpoMenuItemInfo->FontFocusColor;
		CMM_SAFE_RELEASE(mpoFocusTextImage);	//ȥ��ԭ����ͼƬ
		mpoFocusTextImage = EinkuiGetSystem()->GetAllocator()->CreateImageByText(ldNormalInit);
		StringCchCopyW(mswMenuTittle, MAX_TITTLE_LENGTH, ldNormalInit.Text);				// ����˵�����
		if (lfHeight < (long)mpoFocusTextImage->GetHeight())
		{
			lfHeight = (long)mpoFocusTextImage->GetHeight();
		}

		// ������Ϣ�������ڣ�Ҫ�����²���
		PostMessageToParent(EACT_POPUPMENU_RELAYOUT_MENUITEM, CExMessage::DataInvalid);

	} while (false);

	// ������˱�����û�п���ʾ�����ݣ������ظ���
	if (NULL == mpoFocusTextImage && NULL == mpoTextImage
		&& NULL == mpoHotKeyImage && NULL == mpoFocusHotKeyImage
		&& NULL == mpoLeftImage && NULL == mpoCheckedImage
		&& NULL == mpoRightImage)
	{
		mpIterator->SetVisible(false);
	}

	return lfHeight;
}


// ������
//		���ز˵����ȼ�ͼƬ����������������ʼ����������mpoHotKeyImage poFocusHotKeyImage
// ����ֵ��
//		-1 ��ʾʧ�ܣ����򷵻ش�����ͼƬ�����߶�
//		ע�⣺�������ָ�����ָ����ַ���ָ�룬�������ȼ�ͼƬ�����ظ߶�Ϊ0
long CEvMenuItem::LoadHotkeyImage(IN PSTCTL_MENUITEM_HOTKEY npdHotKeyInfo)
{
	long lfHeight = -1;
	do
	{
		BREAK_ON_NULL(mpoTextImage);

		if (NULL == npdHotKeyInfo->HotKeyToShow || UNICODE_NULL == npdHotKeyInfo->HotKeyToShow[0])
		{
			CMM_SAFE_RELEASE(mpoHotKeyImage);
			CMM_SAFE_RELEASE(mpoFocusHotKeyImage);
			lfHeight = 0;
			break;
		}

		// �����������ֵ
		if (0 == npdHotKeyInfo->VirtualKey)
			break;

		// �����ȼ�ͼƬ
		STETXT_BMP_INIT ldNormalInit;
		ZeroMemory(&ldNormalInit, sizeof(STETXT_BMP_INIT));
		ldNormalInit.FontName = mpoMenuItemInfo->FontName;
		ldNormalInit.FontSize = mpoMenuItemInfo->FontSize;
		ldNormalInit.TextColor = mpoMenuItemInfo->FontColor;
		ldNormalInit.Text = npdHotKeyInfo->HotKeyToShow;

		CMM_SAFE_RELEASE(mpoHotKeyImage);
		mpoHotKeyImage = EinkuiGetSystem()->GetAllocator()->CreateImageByText(ldNormalInit);

		miHotKeyAreaWidth = mpoHotKeyImage->GetWidth();

		if (lfHeight < (long)mpoHotKeyImage->GetHeight())
		{
			lfHeight = (long)mpoHotKeyImage->GetHeight();
		}

		// ��������̬�ȼ�ͼƬ
		ldNormalInit.TextColor = mpoMenuItemInfo->FontFocusColor;
		CMM_SAFE_RELEASE(mpoFocusHotKeyImage);
		mpoFocusHotKeyImage = EinkuiGetSystem()->GetAllocator()->CreateImageByText(ldNormalInit);

		// ��¼�˵��ȼ�
		StringCchCopyW(mswHotKey, MAX_HOTKEY_LENGTH, ldNormalInit.Text);

		if (lfHeight < (long)mpoFocusHotKeyImage->GetHeight())
		{
			lfHeight = (long)mpoFocusHotKeyImage->GetHeight();
		}

		// ������Ϣ�������ڣ�Ҫ�����²���
		PostMessageToParent(EACT_POPUPMENU_RELAYOUT_MENUITEM, CExMessage::DataInvalid);

		// ��ע���ȼ�
		// �����������ֵ
		mlHotKeyID = npdHotKeyInfo->VirtualKey;

		// Ctrl
		if (false != npdHotKeyInfo->NeedCtrl)
		{
			mlHotKeyID |= MENUITEM_HOTKEY_CTRL;
		}

		// Shift
		if (false != npdHotKeyInfo->NeedShift)
		{
			mlHotKeyID |= MENUITEM_HOTKEY_SHIFT;
		}

		// Alt
		if (false != npdHotKeyInfo->NeedAlt)
		{
			mlHotKeyID |= MENUITEM_HOTKEY_ATL;
		}

		// ����ע��ȫ���ȼ�
		EinkuiGetSystem()->GetElementManager()->RegisterHotKey(mpIterator, mlHotKeyID,
			npdHotKeyInfo->VirtualKey, npdHotKeyInfo->NeedCtrl, npdHotKeyInfo->NeedShift, npdHotKeyInfo->NeedAlt, NULL);



		//// ע�ᱸ���ȼ�������ʧ��
		ICfKey* lpoKey = this->mpTemplete->OpenKey(TF_ID_MENUITEM_RESERVE_HOTKEY);
		if (NULL != lpoKey)
		{
			int liReverseVk = this->mpTemplete->QuerySubKeyValueAsLONG(TF_ID_MENUITEM__RESERVE_HOTKEY_VIRTUAL_KEY, -1);
			if (-1 != liReverseVk)
			{
				// Ctrl
				CMM_SAFE_RELEASE(lpoKey);
				lpoKey = mpTemplete->OpenKey(TF_ID_MENUITEM_HOTKEY_CTRL_KEY);
				bool lbIsNeedCtrl = NULL != lpoKey ? true : false;

				// Shift
				CMM_SAFE_RELEASE(lpoKey);
				lpoKey = mpTemplete->OpenKey(TF_ID_MENUITEM_HOTKEY_SHAFT_KEY);
				bool lbIsNeedShift = NULL != lpoKey ? true : false;

				// Alt
				CMM_SAFE_RELEASE(lpoKey);
				lpoKey = mpTemplete->OpenKey(TF_ID_MENUITEM_HOTKEY_ALT_KEY);
				bool lbIsNeedAlt = NULL != lpoKey ? true : false;

				// ����ע��ȫ���ȼ�
				bool lbIsOk = EinkuiGetSystem()->GetElementManager()->RegisterHotKey(mpIterator, mlHotKeyID,
					liReverseVk, lbIsNeedCtrl, lbIsNeedShift, lbIsNeedAlt, NULL);

				lbIsNeedAlt = false;
			}

			CMM_SAFE_RELEASE(lpoKey);
		}



	} while (false);

	// ������˱�����û�п���ʾ�����ݣ������ظ���
	if (NULL == mpoFocusTextImage && NULL == mpoTextImage
		&& NULL == mpoHotKeyImage && NULL == mpoFocusHotKeyImage
		&& NULL == mpoLeftImage && NULL == mpoCheckedImage
		&& NULL == mpoRightImage)
	{
		mpIterator->SetVisible(false);
	}

	return lfHeight;
}


// ������
//		���ؼ����˵�
void CEvMenuItem::HideCascadeMenu()
{
	if (NULL != mpoSubPopupMenu)
		mpoSubPopupMenu->GetIterator()->SetVisible(false);
}
