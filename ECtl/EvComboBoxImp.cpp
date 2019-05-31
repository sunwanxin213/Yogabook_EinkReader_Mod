/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "stdafx.h"
#include "cmmstruct.h"
#include "Einkui.h"
#include "ElementImp.h"
#include "XCtl.h"
#include "EvComboBoxImp.h"


//�����ֵ
#define CB_KEY_DEFAULT_CTRL						L"DefaultCtrl"
#define CB_KEY_COMBOBOX							L"ComboBox"

#define CB_KEY_DEFAULT_EDIT						L"DefaultEdit"
#define CB_KEY_DEFAULT_LIST						L"DefaultList"
#define CB_KEY_DEFAULT_BUTTON					L"DefaultButton"
#define CB_KEY_DEFAULT_DROP_BUTTON				L"DefaultDropButton"
#define CB_KEY_DEFAULT_IMAGE_BUTTON				L"DefaultImageButton"
#define CB_KEY_DEFAULT_PICTURE_FRAME			L"DefaultPictureFrame"
//#define CB_KEY_DEFAULT_UNDER_PICTURE_FRAME		L"DefaultUnderPictureFrame"

#define CB_KEY_UPPER_PICTURE					L"UpperPicture"
//#define CB_KEY_UNDER_PICTURE					L"UnderPicture"
#define CB_KEY_CURRENT_ITEM_BUTTON				L"CurrentItemButton"
#define CB_KEY_CURRENT_ITEM_EDIT				L"CurrentItemEdit"
#define CB_KEY_DROP_DOWN_BUTTON					L"DropDownButton"
//#define CB_KEY_LIST								L"List"

#define CB_KEY_STYLE							L"Style"

#define CB_KEY_X								L"X"
#define CB_KEY_Y								L"Y"
#define CB_KEY_WIDTH							L"Width"
#define CB_KEY_HEIGHT							L"Height"
#define CB_KEY_BACKGROUND						L"BackGround"
#define CB_KEY_LIST_HEIGHT						L"ListHeight"
//#define CB_KEY_DEFAULT_VALUE_LIST				L"DefaultValueList"
#define CB_KEY_DEFAULT_CURRENT_VALUE			L"DefaultCurrentValue"

#define CB_KEY_CHILDREN							L"Children"
#define CB_KEY_MENUITEM							L"MenuItem"
#define CB_KEY_MENUITEM_ID						L"ID"
#define CB_KEY_MENUITEM_TEXT					L"Text"

//��������
#define CB_KEY_ONLY_ACCEPT_NUM					L"OnlyAcceptNum"
#define CB_KEY_MAX_INPUT_NUM					L"MaxInputNum"
#define CB_KEY_MIN_INPUT_NUM					L"MinInputNum"
#define CB_KEY_MAX_INPUT_LEN					L"MaxInputLen"

//�ؼ����
#define CB_STYLE_EDITABLE						L"0"
#define CB_STYLE_UNEDITABLE						L"1"


//�����ӿؼ�
#define	CB_ID_POPMENU							10		//�����˵�

//��Ϣ���
#define CB_KEY_MESSAGE_INFO							L"MessageInfo"
#define CB_KEY_MESSAGE_INFO_ID						L"ID"
#define CB_KEY_MESSAGE_INFO_MESSAGE_TYPE			L"MessageType"
#define CB_KEY_MESSAGE_INFO_MESSAGE_TYPE_NONE		L"0"
#define CB_KEY_MESSAGE_INFO_MESSAGE_TYPE_BOOL		L"1"
#define CB_KEY_MESSAGE_INFO_MESSAGE_TYPE_INT		L"2"
#define CB_KEY_MESSAGE_INFO_MESSAGE_TYPE_FLOAT		L"3"
#define CB_KEY_MESSAGE_INFO_MESSAGE_TYPE_STRING		L"4"


DEFINE_BUILTIN_NAME(ComboBox)


// ֻ���ڱ������ó�ʼֵ����ָ����ΪNULL�����п���ʧ�ܵ������֮������㶼Ӧ����InitOnCreate�н���
CEvComboBox::CEvComboBox() : 
			mpUpperPicture(NULL),
			mpCurrentItemEdit(NULL),
			mpCurrentItemButton(NULL),
			mpDropDownButton(NULL),
			//mpCurItem(NULL),
			mpIterPopMenu(NULL),
			mpComboBoxKey(NULL),

			mnStyle(-1),

			mbOnlyAcceptNum(false),
			mnMaxInputNum(0),
			mnMinInputNum(0)
{	
	//mfMaxWidth = 0.0f;
}

CEvComboBox::~CEvComboBox()
{
	CMM_SAFE_RELEASE(mpComboBoxKey);
}

ULONG CEvComboBox::InitOnCreate(
	IN IEinkuiIterator* npParent,	// ������ָ��
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID				// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;
	ICfKey* lpRootKey = NULL;
	ICfKey* lpDefaultCtrlKey = NULL;

	do 
	{
		//���ȵ��û���
		leResult = 	CXuiElement::InitOnCreate(npParent,npTemplete,nuEID);
		if(leResult != ERESULT_SUCCESS)
			break;

		//����ComboBox���ӿؼ�
		IConfigFile * lpConfigFile = EinkuiGetSystem()->GetCurrentWidget()->GetDefaultFactory()->GetTempleteFile();
		if (lpConfigFile == NULL) break;
		lpRootKey = lpConfigFile->GetRootKey();
		if (lpRootKey == NULL) break;
		lpDefaultCtrlKey = lpRootKey->GetSubKey(CB_KEY_DEFAULT_CTRL);
		if (lpDefaultCtrlKey == NULL) break;
		mpComboBoxKey = lpDefaultCtrlKey->GetSubKey(CB_KEY_COMBOBOX);
		if (mpComboBoxKey == NULL) break;

		//�����༭����ı������ϱ�����
		ICfKey * lpUpperPictureKey = mpComboBoxKey->GetSubKey(CB_KEY_DEFAULT_PICTURE_FRAME);
		if(lpUpperPictureKey)
		{
			mpUpperPicture = CEvPictureFrame::CreateInstance(mpIterator, lpUpperPictureKey, COMBO_ID_CTRL_UPPER_PICTURE);			
		}

		//������Ͽ��е�ǰ��༭ģʽ��
		ICfKey * lpCurrentItemEditKey = mpComboBoxKey->GetSubKey(CB_KEY_DEFAULT_EDIT);
		if(lpCurrentItemEditKey)
		{
			mpCurrentItemEdit = CEvEditImp::CreateInstance(mpIterator, lpCurrentItemEditKey, COMBO_ID_CTRL_CURRENT_ITEM_EDIT);			
		}

		//������Ͽ��е�ǰ��Ǳ༭ģʽ��
		ICfKey * lpCurrentItemButtonKey = mpComboBoxKey->GetSubKey(CB_KEY_DEFAULT_DROP_BUTTON);
		if(lpCurrentItemButtonKey)
		{
			mpCurrentItemButton = CEvButton::CreateInstance(mpIterator, lpCurrentItemButtonKey, COMBO_ID_CTRL_CURRENT_ITEM_BUTTON);	
		}

		//������Ͽ��е�������ť
		ICfKey * lpDropDownButtonKey = mpComboBoxKey->GetSubKey(CB_KEY_DEFAULT_IMAGE_BUTTON);
		if(lpDropDownButtonKey)
		{
			mpDropDownButton = CEvImageButton::CreateInstance(mpIterator, lpDropDownButtonKey, COMBO_ID_CTRL_DROP_DOWN_BUTTON);			
		}

		//�����ļ���ȡ��Ϣ����
		ICfKey* lpMessageInfoKey = mpTemplete->GetSubKey(CB_KEY_MESSAGE_INFO);
		if (lpMessageInfoKey)
		{
			unsigned int		lnID = 0;
			wchar_t lpMessageType[BUF_SIZE] = {0};

			lpMessageInfoKey->QuerySubKeyValue(CB_KEY_MESSAGE_INFO_ID, &lnID, sizeof(unsigned int));
			lpMessageInfoKey->QuerySubKeyValue(CB_KEY_MESSAGE_INFO_MESSAGE_TYPE, lpMessageType, BUF_SIZE * sizeof(wchar_t));

			if (lnID != 0)
			{
				mMsgInfo.mnCtrlID = lnID;
			}

			if (wcscmp(lpMessageType, CB_KEY_MESSAGE_INFO_MESSAGE_TYPE_NONE) == 0)
			{
				mMsgInfo.mnMsgParaType	= COMBOBOX_TMPT_NONE;
				mMsgInfo.mpMsgBuf		= NULL;
			}
			else if (wcscmp(lpMessageType, CB_KEY_MESSAGE_INFO_MESSAGE_TYPE_BOOL) == 0)
			{
				mMsgInfo.mnMsgParaType = COMBOBOX_TMPT_BOOL;
			}
			else if (wcscmp(lpMessageType, CB_KEY_MESSAGE_INFO_MESSAGE_TYPE_INT) == 0)
			{
				mMsgInfo.mnMsgParaType = COMBOBOX_TMPT_INT;
			}
			else if (wcscmp(lpMessageType, CB_KEY_MESSAGE_INFO_MESSAGE_TYPE_FLOAT) == 0)
			{
				mMsgInfo.mnMsgParaType = COMBOBOX_TMPT_FLOAT;
			}
			else if (wcscmp(lpMessageType, CB_KEY_MESSAGE_INFO_MESSAGE_TYPE_STRING) == 0)
			{
				mMsgInfo.mnMsgParaType = COMBOBOX_TMPT_STRING;
			}
			else
			{
				mMsgInfo.mnMsgParaType = COMBOBOX_TMPT_OTHERS;
			}
		}

		////	��ʼ��UnificSetting
		//mpUnificSetting = GetUnificSetting();

		
		//����List
		ICfKey * lpListKey = mpComboBoxKey->GetSubKey(CB_KEY_DEFAULT_LIST);
		if(lpListKey != NULL)
		{
			CEvList* lpIterList = CEvList::CreateInstance(mpIterator, lpListKey,0);		
			BREAK_ON_NULL(lpIterList);
			mpIterList = lpIterList->GetIterator();
		}

		//mpIterator->ModifyStyles(EITR_STYLE_ALL_MWHEEL|EITR_STYLE_KEYBOARD);
		leResult = ERESULT_SUCCESS;

	} while (false);


	CMM_SAFE_RELEASE(lpRootKey);
	CMM_SAFE_RELEASE(lpDefaultCtrlKey);
	return leResult;
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEvComboBox::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		SetChildCtrlPara();

		SetDefaultValueList();

		mpIterator->ModifyStyles(EITR_STYLE_POPUP);

		mpIterPopMenu = mpIterator->GetSubElementByID(CB_ID_POPMENU);
		if (mpIterPopMenu != NULL)
		{
			//mpIterPopMenu->SetPosition(mpIterPopMenu->GetPositionX() - 5.0f, mpIterPopMenu->GetPositionY());
			
			if (mpIterList != NULL)
			{
				mpIterList->SetPosition(-5.0f,mpIterPopMenu->GetPositionY());
				//mpIterPopMenu->SetPosition(0.0f, 0.0f);
				mpIterList->SetSize(mpIterPopMenu->GetSizeX(),(float)mpTemplete->QuerySubKeyValueAsLONG(CB_KEY_LIST_HEIGHT,(LONG)mpIterPopMenu->GetSizeY()));
				EinkuiGetSystem()->GetElementManager()->SetParentElement(mpIterList,mpIterPopMenu);	//�Ѳ˵��ŵ�List��
				CExMessage::SendMessage(mpIterList,mpIterator,EACT_LIST_ADD_ELEMENT,mpIterPopMenu);
				mpIterList->SetVisible(false);
			}
			//mpIterPopMenu->SetVisible(false);
		}
		if (NULL != mpIterPopMenu)
		{
			bool lbEnable = false;
			CExMessage::SendMessageW(mpIterPopMenu, mpIterator,
				EACT_POPUPMENU_IS_MANAGER_MENUITEM_ENABLE,
				lbEnable);
		}

		lResult = ERESULT_SUCCESS;
	}while(false);

	return lResult;
}

ERESULT CEvComboBox::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ULONG luCtrlId = npSender->GetID();

	ERESULT luResult = ERESULT_SUCCESS;

	switch (luCtrlId)
	{
	case COMBO_ID_CTRL_CURRENT_ITEM_BUTTON:
	//case COMBO_ID_CTRL_DROP_DOWN_BUTTON:
		{
// 			if (mpIterPopMenu)
// 			{
// 				mpIterPopMenu->SetVisible(mpIterPopMenu->IsVisible() ? false : true);
// 			}
			if(mpIterList != NULL && mpIterPopMenu != NULL)
			{
				mpIterList->SetVisible(mpIterList->IsVisible()?false:true);
				mpIterPopMenu->SetVisible(mpIterList->IsVisible());

			}

			break;
		}
	default:
		{
			////�б�����
			//if (luCtrlId >= COMBO_ID_CTRL_OTHERS)
			//{
			//	wchar_t* lswBuf = NULL;
			//	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(npSender,
			//		EACT_BUTTON_GETTEXT, NULL, 0, &lswBuf, sizeof(wchar_t*));
			//	mpCurItem = lswBuf;

			//	if (mnStyle == 0)
			//	{
			//		EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
			//			mpCurrentItemEdit->GetIterator(),
			//			EACT_EDIT_SET_TEXT,
			//			(void*)lswBuf,
			//			(wcslen(lswBuf) + 1) * sizeof(wchar_t), NULL, 0);
			//	}
			//	else if (mnStyle == 1)
			//	{
			//		EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
			//			mpIterator->GetSubElementByID(COMBO_ID_CTRL_CURRENT_ITEM_BUTTON),
			//			EACT_BUTTON_SETTEXT,
			//			(void*)lswBuf,
			//			(wcslen(lswBuf) + 1) * sizeof(wchar_t), NULL, 0);
			//	}
			//}
		}
	}

	return luResult;
}

//���û�����
ERESULT CEvComboBox::OnElementEnable(bool nbIsEnable)
{
	SetComboBoxEnable(nbIsEnable);

	return ERROR_SUCCESS;
}

// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
ERESULT CEvComboBox::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������

	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	do 
	{
		BREAK_ON_NULL(npMsg);

		switch (npMsg->GetMessageID())
		{
		case EEVT_IMAGEBUTTON_CLICK:
// 			if (mpIterPopMenu)
// 			{
// 				mpIterPopMenu->SetVisible(mpIterPopMenu->IsVisible() ? false : true);
// 			}
			if(mpIterList != NULL && mpIterPopMenu != NULL)
			{
				mpIterList->SetVisible(mpIterList->IsVisible()?false:true);
				mpIterPopMenu->SetVisible(mpIterList->IsVisible());

			}
			break;

		case EMSG_ELEMENT_ACTIVATED:
			{
				//����״̬�ı�
				STEMS_ELEMENT_ACTIVATION* lpActive;
				luResult = CExMessage::GetInputDataBuffer(npMsg,lpActive);
				if(luResult != ERESULT_SUCCESS)
					break;

// 				if (lpActive->State == 0 && mpIterPopMenu->IsVisible() != false)			// �ɼ�״̬��ʧȥ����״̬����Ҫ���ص�ǰչ���Ĳ˵�
// 				{
// 					bool lbShow = false; 
// 					mpIterPopMenu->SetVisible(lbShow);
// 				}

				if (lpActive->State == 0 && mpIterList->IsVisible() != false)			// �ɼ�״̬��ʧȥ����״̬����Ҫ���ص�ǰչ���Ĳ˵�
				{
					bool lbShow = false; 
					mpIterList->SetVisible(lbShow);
				}
			}
			break;

		case EEVT_MENUITEM_CLICK:			//��չ�ؼ��Ĳ˵�����
			{	
				//��ȡID
				if(npMsg->GetInputDataSize()!=sizeof(int) || npMsg->GetInputData()==NULL)
					luResult = ERESULT_WRONG_PARAMETERS;
				int lItemID = *(int*)(npMsg->GetInputData());
				PostMessageToParent(EEVT_COMBOBOX_LIST_ITEM_CLICK, lItemID);

				if (mpVecComboMenuItem.Size() != 0)
				{
					int i = 0;
					for (i = 0; i <mpVecComboMenuItem.Size(); i++)
					{
						if (mpVecComboMenuItem[i].mnID == lItemID)
						{
							PostMessageToParent(EEVT_COMBOBOX_ITEM_CLICK_WITH_TEXT, mpVecComboMenuItem[i].mpText);		// ֪ͨ������ add by colin
							PostMessageToParent(EEVT_COMBOBOX_ITEM_CLICK_WITH_INDEX, i);

							if (mnStyle == 0)
							{
								EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
									mpCurrentItemEdit->GetIterator(),
									EACT_EDIT_SET_TEXT,
									(void*)mpVecComboMenuItem[i].mpText,
									BUF_SIZE * sizeof(wchar_t), NULL, 0);
							}
							else if (mnStyle == 1)
							{
								EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
									mpIterator->GetSubElementByID(COMBO_ID_CTRL_CURRENT_ITEM_BUTTON),
									EACT_BUTTON_SETTEXT,
									(void*)mpVecComboMenuItem[i].mpText,
									BUF_SIZE * sizeof(wchar_t), NULL, 0);

								
							}
							break;
						}
					}

					if(mpIterList != NULL && mpIterPopMenu != NULL)
					{
						mpIterList->SetVisible(false);
						mpIterPopMenu->SetVisible(false);
					}

					mToolbarMsgInfo.mnCtrlID = mMsgInfo.mnCtrlID;
					mToolbarMsgInfo.mnMsgParaType = (TOOLBAR_MSG_PARA_TYPE)mMsgInfo.mnMsgParaType;

					if(mMsgInfo.mnMsgParaType == COMBOBOX_TMPT_INT)
					{
						int lnVal = _wtoi(mpVecComboMenuItem[i].mpText);
						mToolbarMsgInfo.mlInterge = lnVal;
					}
					else if(mMsgInfo.mnMsgParaType == COMBOBOX_TMPT_FLOAT)
					{
						FLOAT lfVal = (FLOAT)_wtof(mpVecComboMenuItem[i].mpText);
						mToolbarMsgInfo.mfFloat = lfVal;
					}
					else if (mMsgInfo.mnMsgParaType == COMBOBOX_TMPT_STRING)
					{
						memset(mToolbarMsgInfo.mswString, 0, MAX_PATH);

						wcscpy_s(mToolbarMsgInfo.mswString, MAX_PATH, mpVecComboMenuItem[i].mpText);
						//mToolbarMsgInfo.mswString = mpVecComboMenuItem[i].mpText;
					}

					CExMessage::PostMessage(mpIterator->GetParent()->GetParent(),
						mpIterator, EEVT_TOOLBARITEM_CLICK, mToolbarMsgInfo, EMSG_POSTTYPE_FAST);
				}
			}
			break;
		case EACT_COMBOBOX_ADD_ITEM:
			{
				//if(npMsg->GetInputDataSize() != sizeof(wchar_t*))
				//{
				//	luResult = ERESULT_WRONG_PARAMETERS;
				//	break;
				//}
				//wchar_t* lpValue = (wchar_t*)npMsg->GetInputData();
				//wchar_t* lpValue = *(wchar_t**)npMsg->GetInputData();

				InsertItem((wchar_t*)npMsg->GetInputData());

				break;
			}

		case EACT_COMBOBOX_DELETE_ITEM_BY_INDEX:		// ɾ���� add by colin
			{
				if(npMsg->GetInputDataSize() != sizeof(int))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				int* lpiInex = (int*)npMsg->GetInputData();
				BREAK_ON_FALSE(DeleteItem(*lpiInex));

				luResult = ERESULT_SUCCESS;
			}
			break;

		case EACT_COMBOBOX_GET_CURRENT_ITEM_TEXT:			// ��ȡ��ǰѡ������ı�	add by colin
			{
				// ��ȡ��ǰѡ�����ı�����
				IEinkuiIterator* lpoCrtItemIter = NULL;
				wchar_t lswBuffer[BUF_SIZE] = {0};
				if (mnStyle == 0)
				{
					lpoCrtItemIter = mpCurrentItemEdit->GetIterator();
					BREAK_ON_NULL(lpoCrtItemIter);
					luResult = CExMessage::SendMessage(lpoCrtItemIter,mpIterPopMenu,EACT_EDIT_GET_TEXT, NULL, npMsg->GetOutputBuffer(), npMsg->GetOutputBufferSize());
				}
				else if (mnStyle == 1)
				{
					lpoCrtItemIter = mpIterator->GetSubElementByID(COMBO_ID_CTRL_CURRENT_ITEM_BUTTON);
					BREAK_ON_NULL(lpoCrtItemIter);
					luResult = CExMessage::SendMessage(lpoCrtItemIter,mpIterator,EACT_BUTTON_GETTEXT, NULL, npMsg->GetOutputBuffer(), npMsg->GetOutputBufferSize());
				}

				luResult = ERESULT_SUCCESS;
			}
			break;

		case EACT_COMBOBOX_SET_ITEM_SELECTED_BY_INDEX:		// ������������ѡ�� add by colin
			{
				// ��ȡѡ��������
				ULONG* lpulItemIndex = NULL;
				if(ERESULT_SUCCESS != CExMessage::GetInputDataBuffer(npMsg,lpulItemIndex))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				BREAK_ON_FALSE(lpulItemIndex);

				if((ULONG)mpVecComboMenuItem.Size() > (*lpulItemIndex))
				{
					CExMessage::PostMessage(mpIterator, mpIterator, EEVT_MENUITEM_CLICK, mpVecComboMenuItem[*lpulItemIndex].mnID);
				}

				luResult = ERESULT_SUCCESS;
			}
			break;

		case EEVT_PANE_ITEM_SET_VALUE:
			{
				SetValue();
			}
			break;

		case EEVT_GET_UNIFIC_SETTING_ID:
			{
				if(npMsg->GetOutputBufferSize() != sizeof(int))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}

				int* lpValue = (int*)npMsg->GetOutputBuffer();
				
				*lpValue = mMsgInfo.mnCtrlID;

				//IUnificSetting* pSetting = GetUnificSetting();
				//if (NULL == pSetting) break;
				bool lbEnable = true;//pSetting->GetItemEnable(mMsgInfo.mnCtrlID);
				SetComboBoxEnable(lbEnable);
			}
			break;

		case EACT_COMBOBOX_SET_ITEM_SELECTED:
			{
				if(npMsg->GetInputDataSize() != sizeof(int))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				int* lpValue = (int*)npMsg->GetInputData();

				SetItemSelected(*lpValue);

				luResult = ERESULT_SUCCESS;
			}
			break;

		case EEVT_EDIT_CONTENT_COMPLETION:
			{
				wchar_t* lswContent = (wchar_t*)npMsg->GetInputData();
				int lnNum = _wtoi(lswContent);
				if (lnNum < mnMinInputNum) 
				{
					lnNum = mnMinInputNum;
					wchar_t lwBuf[BUF_SIZE] = {0};
					_itow_s(lnNum, lwBuf, 10);
					CExMessage::SendMessageWithText(mpCurrentItemEdit->GetIterator(), 
						mpIterator, EACT_EDIT_SET_TEXT, lwBuf, NULL, 0);
				}

				if (lnNum > mnMaxInputNum) 
				{
					lnNum = mnMaxInputNum;
					wchar_t lwBuf[BUF_SIZE] = {0};
					_itow_s(lnNum, lwBuf, 10);
					CExMessage::SendMessageWithText(mpCurrentItemEdit->GetIterator(), 
						mpIterator, EACT_EDIT_SET_TEXT, lwBuf, NULL, 0);
				}
				//wchar_t lwBuf[BUF_SIZE] = {0};
				//_itow_s(lnNum, lwBuf, 10);

				mToolbarMsgInfo.mnCtrlID = mMsgInfo.mnCtrlID;
				mToolbarMsgInfo.mnMsgParaType = (TOOLBAR_MSG_PARA_TYPE)mMsgInfo.mnMsgParaType;
				//memset(mToolbarMsgInfo.mswString, 0, MAX_PATH);
				//wcscpy_s(mToolbarMsgInfo.mswString, MAX_PATH, lwBuf);
				mToolbarMsgInfo.mlInterge = lnNum;

				CExMessage::PostMessage(mpIterator->GetParent()->GetParent(),
					mpIterator, EEVT_TOOLBARITEM_CLICK, mToolbarMsgInfo, EMSG_POSTTYPE_FAST);
			}
			break;

		case EACT_COMBOBOX_SET_ENABLE:
			{
				if(npMsg->GetInputDataSize() != sizeof(int))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				int* lpID = (int*)npMsg->GetInputData();

				SetItemEnable(*lpID);
			}
			break;

		case EACT_COMBOBOX_SET_DISABLE:
			{
				if(npMsg->GetInputDataSize() != sizeof(int))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				int* lpID = (int*)npMsg->GetInputData();

				SetItemDisable(*lpID);
			}
			break;

		case EEVT_EDIT_KEYBOARD_FOCUS:
			{
				if(npMsg->GetInputDataSize() != sizeof(LONG))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				LONG* lpFocus = (LONG*)npMsg->GetInputData();
				if ( (*lpFocus != 0) && (mpCurrentItemEdit != NULL) )
				{
					_STCTL_EDIT_SELECTION sel;
					sel.SelBegin = 0;
					sel.SelCount = -1;
					CExMessage::PostMessageW(mpCurrentItemEdit->GetIterator(),
						mpIterator, EACT_EDIT_SET_SELECTION, sel);
				}

			}
			break;

		default:
			luResult = ERESULT_NOT_SET;
			break;
		}

		if(luResult == ERESULT_NOT_SET)
		{
			luResult = CXuiElement::ParseMessage(npMsg); // ���û����ͬ��������ע�⣺һ��Ҫ��������ֱ�ӻ���
		}

	} while (false);

	return luResult;
}

//�����µ�һ��
BOOL CEvComboBox::InsertItem(wchar_t* lswName)
{
	BOOL lbRet = FALSE;

	do 
	{
		BREAK_ON_NULL(lswName);

		_STCTL_POPUPMENU_MENUITEMINFO lsPopItem = {0};
		lsPopItem.Type = 1;
		lsPopItem.Index = -1;
		lsPopItem.UniqueMenuItemId = mpVecComboMenuItem.Size()+1;
		lsPopItem.MenuItemId = mpVecComboMenuItem.Size()+1;
		wcscpy_s(lsPopItem.MenuText,lswName);

		if(CExMessage::SendMessage(mpIterPopMenu,mpIterator,EACT_POPUPMENU_INSERT_MENUITEM_BY_CREATE,lsPopItem) != ERESULT_SUCCESS)
			break;

		//��������List�Ĵ�С
		mpIterList->SetSize(mpIterPopMenu->GetSizeX(),(float)mpTemplete->QuerySubKeyValueAsLONG(CB_KEY_LIST_HEIGHT,(LONG)mpIterPopMenu->GetSizeY()));
		//mpIterPopMenu->SetPosition(0.0f,0.0f);
		//CExMessage::SendMessage(mpIterList,mpIterator,EACT_LIST_RECACULATE,mpIterPopMenu);

		ComboMenuItem ldMenuItem;
		ldMenuItem.mnID = lsPopItem.MenuItemId;
		wcscpy_s(ldMenuItem.mpText,BUF_SIZE,lswName);
		mpVecComboMenuItem.Insert(-1, ldMenuItem);

		//// �ڷǱ༭ģʽ�£������ǰֻ�иղ����һ���Ҫ���ø���ΪĬ��ѡ�е���  add by colin
		//if(mnStyle == 1 && mpVecComboMenuItem.Size() == 1)
		//{
		//	CExMessage::PostMessage(mpIterator, mpIterator, EEVT_MENUITEM_CLICK, ldMenuItem.mnID);
		//}

		lbRet = TRUE;

	} while (FALSE);
	
	return lbRet;
}

// ɾ��ָ���С��0��ʾɾ��ȫ���� ��������Add by colin
bool CEvComboBox::DeleteItem(IN int niIndexToDel)
{
	bool lbResult = false;
	do 
	{
		if(mpVecComboMenuItem.Size() > 0)
		{
			if(niIndexToDel >= mpVecComboMenuItem.Size())
				break;

			// ɾ����
			CExMessage::SendMessage(mpIterPopMenu, mpIterPopMenu, EACT_POPUPMENU_DELETE_MENUITEM_BY_INDEX, niIndexToDel);

			// ��ȡ��ǰѡ�����ı�����
			IEinkuiIterator* lpoCrtItemIter = NULL;
			wchar_t lswBuffer[BUF_SIZE] = {0};
			if (mnStyle == 0)
			{
				lpoCrtItemIter = mpCurrentItemEdit->GetIterator();
				BREAK_ON_NULL(lpoCrtItemIter);
				if(ERESULT_SUCCESS != CExMessage::SendMessage(lpoCrtItemIter,mpIterPopMenu,EACT_EDIT_GET_TEXT, NULL, (void*)&lswBuffer, sizeof(wchar_t) * BUF_SIZE))
					break;
			}
			else if (mnStyle == 1)
			{
				lpoCrtItemIter = mpIterator->GetSubElementByID(COMBO_ID_CTRL_CURRENT_ITEM_BUTTON);
				BREAK_ON_NULL(lpoCrtItemIter);
				CExMessage::SendMessage(lpoCrtItemIter,mpIterator,EACT_BUTTON_GETTEXT, NULL, (void*)&lswBuffer,BUF_SIZE * sizeof(wchar_t));
			}
			else
				break;

			// ɾ����¼��
			if(niIndexToDel < 0)		// ɾ��ȫ��
			{
				wchar_t lswEmpty[] = L"";
				if(mnStyle == 0)
					CExMessage::SendMessageWithText(lpoCrtItemIter,mpIterator,EACT_EDIT_SET_TEXT, lswEmpty);
				else if(mnStyle == 1)
					CExMessage::SendMessageWithText(lpoCrtItemIter,mpIterator,EACT_BUTTON_SETTEXT, lswEmpty);

				mpVecComboMenuItem.Clear();

				// ��PopmMenu���óɸպ�û��List�Ĵ�С
				mpIterPopMenu->SetSize(mpIterPopMenu->GetSizeX(), mpIterList->GetSizeY());
			}
			else		// ɾ��ĳ��
			{
				// ���Ҫɾ����������ݺ͵�ǰѡ�����������ͬ�����л�һ��
				if(_wcsicmp(lswBuffer, mpVecComboMenuItem[niIndexToDel].mpText) == 0)
				{
					// ����������һ���ʹ����һ�����ʹ�õ�һ��
					int liNextItemIndex = (niIndexToDel + 1) < mpVecComboMenuItem.Size()? (niIndexToDel + 1) : 0;
					if(mnStyle == 0)
						CExMessage::SendMessageWithText(lpoCrtItemIter,mpIterator,EACT_EDIT_SET_TEXT, mpVecComboMenuItem[liNextItemIndex].mpText);
					else if(mnStyle == 1)
						CExMessage::SendMessageWithText(lpoCrtItemIter,mpIterator,EACT_BUTTON_SETTEXT, NULL, mpVecComboMenuItem[liNextItemIndex].mpText);
					else
						break;
				}

				mpVecComboMenuItem.RemoveByIndex(niIndexToDel);
			}


			// ����List�Ĵ�С
			CExMessage::SendMessage(mpIterList,mpIterator,EACT_LIST_RECACULATE,CExMessage::DataInvalid);

		}
		

		lbResult = true;
	} while (false);

	return lbResult;
}

BOOL CEvComboBox::SetChildCtrlPara()
{
	if (mpTemplete == NULL) return FALSE;

	if (mpUpperPicture)
	{
		//�����ϱ���
		ICfKey* pKeyUpperPicture = mpTemplete->GetSubKey(CB_KEY_UPPER_PICTURE);
		if(NULL != pKeyUpperPicture)		// ��ֵ����ʱ�ٸ��ģ�������Ȼʹ��Ĭ�ϵ�  ����add by colin
		{
			IEinkuiIterator* pIter = mpUpperPicture->GetIterator();
			wchar_t lpUpperPicture[BUF_SIZE] = {0};
			pKeyUpperPicture->QuerySubKeyValue(CB_KEY_BACKGROUND, lpUpperPicture, BUF_SIZE * sizeof(wchar_t));
			EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
				pIter, EACT_PICTUREFRAME_CHANGE_PIC, lpUpperPicture,
				(wcslen(lpUpperPicture) + 1) * sizeof(wchar_t), NULL, 0);

			ULONG lPosX = pKeyUpperPicture->QuerySubKeyValueAsLONG(CB_KEY_X);
			ULONG lPosY = pKeyUpperPicture->QuerySubKeyValueAsLONG(CB_KEY_Y);
			pIter->SetPosition((FLOAT)(lPosX), (FLOAT)(lPosY));

			ULONG lWidth = pKeyUpperPicture->QuerySubKeyValueAsLONG(CB_KEY_WIDTH);
			ULONG lHeight = pKeyUpperPicture->QuerySubKeyValueAsLONG(CB_KEY_HEIGHT);
			pIter->SetSize((FLOAT)(lWidth), (FLOAT)(lHeight));
		}
	}

	if (mpCurrentItemEdit)
	{
		//������Ͽ��е�ǰ��༭ģʽ��
		ICfKey* pKeyCurrentItemEdit = mpTemplete->GetSubKey(CB_KEY_CURRENT_ITEM_EDIT);
		if(NULL != pKeyCurrentItemEdit)		// ��ֵ����ʱ�ٸ��ģ�������Ȼʹ��Ĭ�ϵ�  ����add by colin
		{
			IEinkuiIterator* pIter = mpCurrentItemEdit->GetIterator();
			ULONG lPosX = pKeyCurrentItemEdit->QuerySubKeyValueAsLONG(CB_KEY_X);
			ULONG lPosY = pKeyCurrentItemEdit->QuerySubKeyValueAsLONG(CB_KEY_Y);
			pIter->SetPosition((FLOAT)(lPosX), (FLOAT)(lPosY));

			ULONG lWidth = pKeyCurrentItemEdit->QuerySubKeyValueAsLONG(CB_KEY_WIDTH);
			ULONG lHeight = pKeyCurrentItemEdit->QuerySubKeyValueAsLONG(CB_KEY_HEIGHT);
			pIter->SetSize((FLOAT)(lWidth), (FLOAT)(lHeight));
		}
	}

	if (mpCurrentItemButton)
	{
		//������Ͽ��е�ǰ��Ǳ༭ģʽ��
		ICfKey* pKeyCurrentItemButton = mpTemplete->GetSubKey(CB_KEY_CURRENT_ITEM_BUTTON);
		if(NULL != pKeyCurrentItemButton)	// ��ֵ����ʱ�ٸ��ģ�������Ȼʹ��Ĭ�ϵ�  ����add by colin
		{
			IEinkuiIterator* pIter = mpCurrentItemButton->GetIterator();

			ULONG lPosX = pKeyCurrentItemButton->QuerySubKeyValueAsLONG(CB_KEY_X);
			ULONG lPosY = pKeyCurrentItemButton->QuerySubKeyValueAsLONG(CB_KEY_Y);
			pIter->SetPosition((FLOAT)(lPosX), (FLOAT)(lPosY));

			ULONG lWidth = pKeyCurrentItemButton->QuerySubKeyValueAsLONG(CB_KEY_WIDTH);
			ULONG lHeight = pKeyCurrentItemButton->QuerySubKeyValueAsLONG(CB_KEY_HEIGHT);
			pIter->SetSize((FLOAT)(lWidth), (FLOAT)(lHeight));

			D2D1_RECT_F ldfRect;
			ldfRect.left = 0.0f;
			ldfRect.top = 0.0f;
			ldfRect.right = (FLOAT)(lWidth);
			ldfRect.bottom = (FLOAT)(lHeight);

			pIter->SetVisibleRegion(ldfRect);
		}


	}
	
	if (mpDropDownButton)
	{
		//������Ͽ��е�������ť
		ICfKey* pKeyDropDownButton = mpTemplete->GetSubKey(CB_KEY_DROP_DOWN_BUTTON);
		if(NULL != pKeyDropDownButton)	// ��ֵ����ʱ�ٸ��ģ�������Ȼʹ��Ĭ�ϵ�  ����add by colin
		{
			IEinkuiIterator* pIter = mpDropDownButton->GetIterator();

			ULONG lPosX = pKeyDropDownButton->QuerySubKeyValueAsLONG(CB_KEY_X);
			ULONG lPosY = pKeyDropDownButton->QuerySubKeyValueAsLONG(CB_KEY_Y);
			pIter->SetPosition((FLOAT)(lPosX), (FLOAT)(lPosY));

			ULONG lWidth = pKeyDropDownButton->QuerySubKeyValueAsLONG(CB_KEY_WIDTH);
			ULONG lHeight = pKeyDropDownButton->QuerySubKeyValueAsLONG(CB_KEY_HEIGHT);
			pIter->SetSize((FLOAT)(lWidth), (FLOAT)(lHeight));
		}
	}

	// ���ð�ť��Ӧ����
	if(mpUpperPicture)
	{
		D2D1_SIZE_F ldfSize;
		ldfSize = mpUpperPicture->GetIterator()->GetSize();
		EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(mpCurrentItemButton->GetIterator(),
			EACT_BUTTON_SET_ACTION_RECT, &ldfSize,  sizeof(D2D1_SIZE_F), NULL, 0);

	}
	
	//�����ļ���ȡ�ؼ����
	wchar_t lpStyle[BUF_SIZE] = {0};
	mpTemplete->QuerySubKeyValue(CB_KEY_STYLE, lpStyle, BUF_SIZE * sizeof(wchar_t));
	if (wcscmp(lpStyle, CB_STYLE_EDITABLE) == 0)
	{
		//�ɱ༭���
		mnStyle = 0;
		mpCurrentItemButton->GetIterator()->SetVisible(false);

		//��ȡ�༭����
		ICfKey* pKeyCurrentItemEdit = mpTemplete->GetSubKey(CB_KEY_CURRENT_ITEM_EDIT);
		if (pKeyCurrentItemEdit != NULL)
		{
			wchar_t lpBuf[BUF_SIZE] = {0};
			pKeyCurrentItemEdit->QuerySubKeyValue(CB_KEY_ONLY_ACCEPT_NUM, lpBuf, BUF_SIZE * sizeof(wchar_t));
			if (wcscmp(lpBuf, L"1") == 0)
			{
				mbOnlyAcceptNum = true;

				ULONG lulAcceptNumOnly = 1;
				CExMessage::SendMessage(mpCurrentItemEdit->GetIterator(), 
					mpIterator, EACT_EDIT_NUMBER_ONLY, lulAcceptNumOnly, NULL, 0);
				// ��ȡ��������
				memset(lpBuf, 0, sizeof(wchar_t) * BUF_SIZE);
				pKeyCurrentItemEdit->QuerySubKeyValue(CB_KEY_MAX_INPUT_LEN, 
					lpBuf, BUF_SIZE * sizeof(wchar_t));
				ULONG lulLengthLimit = _wtoi(lpBuf);
				CExMessage::SendMessage(mpCurrentItemEdit->GetIterator(), 
					mpIterator, EACT_EDIT_SET_LENGTH_LIMIT, lulLengthLimit, NULL, 0);
				// �����ֵ
				memset(lpBuf, 0, sizeof(wchar_t) * BUF_SIZE);
				pKeyCurrentItemEdit->QuerySubKeyValue(CB_KEY_MAX_INPUT_NUM, 
					lpBuf, BUF_SIZE * sizeof(wchar_t));
				mnMaxInputNum = _wtoi(lpBuf);
				// ��С��ֵ
				memset(lpBuf, 0, sizeof(wchar_t) * BUF_SIZE);
				pKeyCurrentItemEdit->QuerySubKeyValue(CB_KEY_MIN_INPUT_NUM, 
					lpBuf, BUF_SIZE * sizeof(wchar_t));
				mnMinInputNum = _wtoi(lpBuf);
			}
		}
	}
	else
	{
		//���ɱ༭���
		mnStyle = 1;
		mpCurrentItemEdit->GetIterator()->SetVisible(false);
	}

	//���ò˵����б�
	ICfKey*  lpChildrenKey = NULL;
	ICfKey*	lpMenuKey = NULL;
	ICfKey*	lpMenuItemKey = NULL;

	lpChildrenKey = mpTemplete->GetSubKey(CB_KEY_CHILDREN);
	if (lpChildrenKey)		lpMenuKey = lpChildrenKey->GetSubKey(CB_ID_POPMENU);
	if (lpMenuKey)			lpMenuItemKey = lpMenuKey->GetSubKey(CB_KEY_MENUITEM);
	if (lpMenuItemKey)
	{
		ICfKey* lpItemKey = lpMenuItemKey->MoveToSubKey();	
		int nCount = 0;
		while (lpItemKey != NULL)	//ѭ����ȡ�˵�����Ϣ
		{
 			ComboMenuItem MenuItem;
 			lpItemKey->QuerySubKeyValue(CB_KEY_MENUITEM_ID, &(MenuItem.mnID), sizeof(int));
 			lpItemKey->QuerySubKeyValue(CB_KEY_MENUITEM_TEXT, MenuItem.mpText, sizeof(wchar_t) * BUF_SIZE);
			mpVecComboMenuItem.Insert(nCount++, MenuItem);

			lpItemKey = lpItemKey->MoveToNextKey();
		}
	}
	
	return TRUE;
}

BOOL CEvComboBox::SetDefaultValueList()
{
	//����Ĭ��ֵ
	ICfKey* lpDefaultCurrentValueKey = mpTemplete->GetSubKey(CB_KEY_DEFAULT_CURRENT_VALUE);
	if (lpDefaultCurrentValueKey)
	{
		wchar_t lswBuf[BUF_SIZE] = { 0 };
		lpDefaultCurrentValueKey->GetValue(lswBuf, sizeof(wchar_t) * BUF_SIZE);

		if (mnStyle == 0)
		{
			EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
				mpCurrentItemEdit->GetIterator(),
				EACT_EDIT_SET_TEXT,
				(void*)lswBuf,
				(wcslen(lswBuf) + 1) * sizeof(wchar_t), NULL, 0);
		}
		else if (mnStyle == 1)
		{
			EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
				mpIterator->GetSubElementByID(COMBO_ID_CTRL_CURRENT_ITEM_BUTTON),
				EACT_BUTTON_SETTEXT,
				(void*)lswBuf,
				(wcslen(lswBuf) + 1) * sizeof(wchar_t), NULL, 0);
		}
	}

	return TRUE;
}

bool CEvComboBox::SetValue()
{
	//if (NULL == mpUnificSetting) return false;

	if (false == mpIterator->IsEnable()) return false;

	//	��ȡֵ
	//eValueType ValueType;
	//wstring lwValue = mpUnificSetting->GetItemValue(mMsgInfo.mnCtrlID, &ValueType);
	wchar_t lwValue[] = L"1";
	//int lnID = _wtoi(lwID.c_str());

	//// ���õ�ǰѡ��ֵ
	//if (mpCurrentItemEdit)
	//{
	//	
	//}

	//if (mpCurrentItemButton)
	//{
	//	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
	//		mpCurrentItemButton->GetIterator(), EACT_BUTTON_SETTEXT, lwID.c_str(),
	//		(lwID.size() + 1) * sizeof(wchar_t), NULL, 0);
	//}

	if (mnStyle == 0)
	{
			EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
				mpCurrentItemEdit->GetIterator(),
				EACT_EDIT_SET_TEXT,
				lwValue/*.c_str()*/,
				(wcslen(lwValue)/*lwValue.size()*/ + 1) * sizeof(wchar_t), 
				NULL, 
				0);
	}
	else if (mnStyle == 1)
	{
		EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
			mpCurrentItemButton->GetIterator(),
			EACT_BUTTON_SETTEXT,
			lwValue/*.c_str()*/,
			(wcslen(lwValue)/*lwValue.size()*/ + 1) * sizeof(wchar_t), 
			NULL, 
			0);
	}

	return true;
}

bool CEvComboBox::SetComboBoxEnable(bool lbEnalbe)
{
	IEinkuiIterator* iter = NULL;

	if (mpUpperPicture) 
	{
		iter = mpUpperPicture->GetIterator();
		if (iter)	iter->SetEnable(lbEnalbe);
	}
	if (mpCurrentItemEdit) 
	{
		iter = mpCurrentItemEdit->GetIterator();
		if (iter)	iter->SetEnable(lbEnalbe);
	}
	if (mpCurrentItemButton) 
	{
		iter = mpCurrentItemButton->GetIterator();
		if (iter)	iter->SetEnable(lbEnalbe);
	}
	if (mpDropDownButton) 
	{
		iter = mpDropDownButton->GetIterator();
		if (iter)	iter->SetEnable(lbEnalbe);
	}

	//if (mpIterPopMenu) mpIterPopMenu->SetVisible(false);

	if(mpIterList != NULL)
		mpIterList->SetVisible(false);

	if (lbEnalbe)
	{
		iter = mpUpperPicture->GetIterator();
		if (iter) iter->SetAlpha(1.0f);
	}
	else
	{
		iter = mpUpperPicture->GetIterator();
		if (iter) iter->SetAlpha(1.0f);
	}

	return true;
}

bool CEvComboBox::SetItemSelected(int nID)
{
	int lItemID = nID;

	if (mpVecComboMenuItem.Size() != 0)
	{
		int i = 0;
		for (i = 0; i < mpVecComboMenuItem.Size(); i++)
		{
			if (mpVecComboMenuItem[i].mnID == lItemID)
			{
				if (mnStyle == 0)
				{
					EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
						mpCurrentItemEdit->GetIterator(),
						EACT_EDIT_SET_TEXT,
						(void*)mpVecComboMenuItem[i].mpText,
						BUF_SIZE * sizeof(wchar_t), NULL, 0);
				}
				else if (mnStyle == 1)
				{
					EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
						mpIterator->GetSubElementByID(COMBO_ID_CTRL_CURRENT_ITEM_BUTTON),
						EACT_BUTTON_SETTEXT,
						(void*)mpVecComboMenuItem[i].mpText,
						BUF_SIZE * sizeof(wchar_t), NULL, 0);
				}
				break;
			}
		}
	}

	return true;
}

bool CEvComboBox::SetItemEnable(int nID)
{
	IEinkuiIterator* lpoPopupMenu = NULL;
	ULONG lulCommandID = (ULONG)nID;

	if (NULL == mpIterPopMenu) return false;

	if(ERESULT_SUCCESS == CExMessage::SendMessageW(mpIterPopMenu, 
		mpIterator, EACT_POPUPMENU_GET_MENUITEM_BY_COMMANDID, lulCommandID, &lpoPopupMenu, sizeof(IEinkuiIterator*)))
	{
		if (NULL != lpoPopupMenu)
		{
			lpoPopupMenu->SetEnable(true);
		}
	}

	return true;
}

bool CEvComboBox::SetItemDisable(int nID)
{
	IEinkuiIterator* lpoPopupMenu = NULL;	
	ULONG lulCommandID = (ULONG)nID;

	if (NULL == mpIterPopMenu) return false;

	if(ERESULT_SUCCESS == CExMessage::SendMessageW(mpIterPopMenu, 
		mpIterator, EACT_POPUPMENU_GET_MENUITEM_BY_COMMANDID, lulCommandID, &lpoPopupMenu, sizeof(IEinkuiIterator*)))
	{
		if (NULL != lpoPopupMenu)
		{
			lpoPopupMenu->SetEnable(false);
		}
	}
	
	return false;
}