/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "stdafx.h"
#include "cmmstruct.h"
#include "Einkui.h"
#include "ElementImp.h"
#include "XCtl.h"
#include "EvSpinButtonImp.h"


//�����ֵ
#define SB_KEY_DEFAULT_CTRL						L"DefaultCtrl"
#define SB_KEY_SPINBUTTON						L"SpinButton"
#define SB_KEY_DEFAULT_EDIT						L"DefaultEdit"
#define SB_KEY_DEFAULT_IMAGE_BUTTON_UP			L"DefaultImageButtonUp"
#define SB_KEY_DEFAULT_IMAGE_BUTTON_DOWN		L"DefaultImageButtonDown"
#define SB_KEY_DEFAULT_PICTURE_FRAME			L"DefaultPictureFrame"

#define SB_KEY_EDIT								L"Edit"
#define SB_KEY_ARROW							L"Arrow"
#define SB_KEY_UP_ARROW							L"UpArrow"
#define SB_KEY_DOWN_ARROW						L"DownArrow"
#define SB_KEY_BACK_IMAGE						L"BackImage"
#define SB_KEY_DEFAULT_VALUE					L"DefaultValue"

#define SB_KEY_WIDTH							L"Width"
#define SB_KEY_HEIGHT							L"Height"
#define SB_KEY_X								L"X"
#define SB_KEY_Y								L"Y"
#define SB_KEY_BACKGROUND						L"BackGround"


DEFINE_BUILTIN_NAME(SpinButton)


// ֻ���ڱ������ó�ʼֵ����ָ����ΪNULL�����п���ʧ�ܵ������֮������㶼Ӧ����InitOnCreate�н���
CEvSpinButton::CEvSpinButton() : 
	mpEdit(NULL),
	mpBtnUpArrow(NULL),
	mpBtnDownArrow(NULL),
	mpBkg(NULL)
{	
	miMinValue = SPINBUTTON_MIN_VALUE;
	miMaxValue = SPINBUTTON_MAX_VALUE;
}

CEvSpinButton::~CEvSpinButton() {}

ULONG CEvSpinButton::InitOnCreate(
	IN IEinkuiIterator* npParent,	// ������ָ��
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID				// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do 
	{
		//���ȵ��û���
		leResult = 	CXuiElement::InitOnCreate(npParent,npTemplete,nuEID);
		if(leResult != ERESULT_SUCCESS)
			break;

		//����SpinButton���ӿؼ�
		IConfigFile * lpConfigFile = EinkuiGetSystem()->GetCurrentWidget()->GetDefaultFactory()->GetTempleteFile();
		if (lpConfigFile == NULL) break;
		ICfKey* lpRootKey = lpConfigFile->GetRootKey();
		if (lpRootKey == NULL) break;
		ICfKey* lpDefaultCtrlKey = lpRootKey->GetSubKey(SB_KEY_DEFAULT_CTRL);
		if (lpDefaultCtrlKey == NULL) break;
		ICfKey* lpSpinButtonKey = lpDefaultCtrlKey->GetSubKey(SB_KEY_SPINBUTTON);
		if (lpSpinButtonKey == NULL) break;

		//��������ͼ
		ICfKey * lpBkgKey = lpSpinButtonKey->GetSubKey(SB_KEY_DEFAULT_PICTURE_FRAME);
		if(lpBkgKey)
		{
			mpBkg = CEvPictureFrame::CreateInstance(mpIterator, lpBkgKey, SB_ID_CTRL_BACKGROUND);			
		}

		//�����༭��
		ICfKey * lpEditKey = lpSpinButtonKey->GetSubKey(SB_KEY_DEFAULT_EDIT);
		if(lpEditKey)
		{
			mpEdit = CEvEditImp::CreateInstance(mpIterator, lpEditKey, SB_ID_CTRL_EDIT);			
		}

		
		//�����ϼ�ͷ
		ICfKey * lpButtonUpKey = lpSpinButtonKey->GetSubKey(SB_KEY_DEFAULT_IMAGE_BUTTON_UP);
		if(lpButtonUpKey)
		{
			mpBtnUpArrow = CEvImageButton::CreateInstance(mpIterator, lpButtonUpKey, SB_ID_CTRL_BUTTON_UP);			
		}

		//�����¼�ͷ
		ICfKey * lpButtonDownKey = lpSpinButtonKey->GetSubKey(SB_KEY_DEFAULT_IMAGE_BUTTON_DOWN);
		if(lpButtonDownKey)
		{
			mpBtnDownArrow = CEvImageButton::CreateInstance(mpIterator, lpButtonDownKey, SB_ID_CTRL_BUTTON_DOWN);			
		}

		//mpIterator->ModifyStyles(EITR_STYLE_ALL_MWHEEL|EITR_STYLE_KEYBOARD);
		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEvSpinButton::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		SetChildCtrlPara();

		//mpIterator->ModifyStyles(EITR_STYLE_POPUP);

		mpIterator->ModifyStyles(EITR_STYLE_POPUP);

		lResult = ERESULT_SUCCESS;
	}while(false);

	return lResult;
}

// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
ERESULT CEvSpinButton::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������

	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	do 
	{
		BREAK_ON_NULL(npMsg);

		switch (npMsg->GetMessageID())
		{
		case EEVT_IMAGEBUTTON_CLICK:
			{
				IEinkuiIterator* lpIter = npMsg->GetMessageSender();
				//SetCurrentValueByDisplay();
				int lnPreValue = GetCurrentValue();
				if (lpIter == mpBtnUpArrow->GetIterator())
				{
					++lnPreValue;
				}
				else if (lpIter == mpBtnDownArrow->GetIterator())
				{
					--lnPreValue;
				}

				if(lnPreValue <= miMaxValue && lnPreValue >= miMinValue)
				{
					SetCurrentValue(lnPreValue);

					// ֪ͨ��������Ϊ����ȥ���ñ༭���ֵ�����ᷴ����Ϣ��
					PostMessageToParent(EEVT_SPINBUTTON_CONTENT_COMPLETION, lnPreValue);

				}

				break;
			}
			

		/*case EEVT_EDIT_CONTENT_MODIFIED:
			{
				SetCurrentValueByDisplay();
				break;
			}*/

		case EACT_SPINBUTTON_GET_CURRENT_VALUE:
			{
				int* lpValue = (int*)npMsg->GetOutputBuffer();
				//SetCurrentValueByDisplay();
				*lpValue = GetCurrentValue();

				break;
			}

		case EACT_SPINBUTTON_SET_CURRENT_VALUE:
			{
				const int* lpValue = (const int*)npMsg->GetInputData();
				SetCurrentValue(*lpValue);

				break;
			}

		case EEVT_EDIT_CONTENT_MODIFIED:
			{
				PostMessageToParent(EEVT_SPINBUTTON_CONTENT_MODIFING, CExMessage::DataInvalid);
				break;
			}

		case EEVT_EDIT_CONTENT_COMPLETION:				// �༭���������
			{
				wchar_t* lswContent = (wchar_t*)npMsg->GetInputData();
				if(NULL != lswContent)
				{
					int liValue = 0;
					if(0 == _wcsicmp(lswContent, L"-"))		// ֻ��һ������
					{
						CExMessage::SendMessageWithText(npMsg->GetMessageSender(), mpIterator, EACT_EDIT_SET_TEXT, L"0");
					}
					else
						swscanf_s(lswContent, L"%d", &liValue);

					// �ж��Ƿ�Խ��
					if(liValue < miMinValue || liValue > miMaxValue)
					{
						SetCurrentValue(liValue);
					}

					// ֪ͨ������
					PostMessageToParent(EEVT_SPINBUTTON_CONTENT_COMPLETION, liValue);
				}
				
				break;
			}

		case EACT_SPINBUTTON_SET_MIN_VALUE:			// ������Сֵ
			{
				int* lpiMinValue = NULL;
				if(ERESULT_SUCCESS != CExMessage::GetInputDataBuffer(npMsg, lpiMinValue))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}

				miMinValue = *lpiMinValue;
				// �����ǰֵС����Сֵ��������Ϊ��Сֵ
				if(mnCurrentValue < miMinValue)
				{
					SetCurrentValue(miMinValue);
				}
			}
			break;

		case EACT_SPINBUTTON_SET_MAX_VALUE:			// �������ֵ
			{
				int* lpiMaxValue = NULL;
				if(ERESULT_SUCCESS != CExMessage::GetInputDataBuffer(npMsg, lpiMaxValue))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}

				miMaxValue = *lpiMaxValue;
				// �����ǰֵ�������ֵ��������Ϊ���ֵ
				if(mnCurrentValue > miMaxValue)
				{
					SetCurrentValue(miMaxValue);
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

//���û�����
ERESULT CEvSpinButton::OnElementEnable(bool nbIsEnable)
{
	if (mpEdit) mpEdit->GetIterator()->SetEnable(nbIsEnable);
	if (mpBtnUpArrow) mpBtnUpArrow->GetIterator()->SetEnable(nbIsEnable);
	if (mpBtnDownArrow) mpBtnDownArrow->GetIterator()->SetEnable(nbIsEnable);

	if (nbIsEnable)
	{
		if (mpBkg) mpBkg->GetIterator()->SetAlpha(1.0f);
	}
	else
	{
		if (mpBkg) mpBkg->GetIterator()->SetAlpha(0.5f);
	}

	return ERROR_SUCCESS;
}

BOOL CEvSpinButton::SetChildCtrlPara()
{
	if (mpTemplete == NULL) return FALSE;

	if (mpBkg)
	{
		//���ñ���
		IEinkuiIterator* pIter = mpBkg->GetIterator();
		ICfKey* lpKeyBkg = mpTemplete->GetSubKey(SB_KEY_BACK_IMAGE);

		if (lpKeyBkg)
		{
			/*wchar_t lszBkgImage[SPINBUTTON_BUF_SIZE] = {0};
			lpKeyBkg->QuerySubKeyValue(SB_KEY_BACKGROUND, lszBkgImage, SPINBUTTON_BUF_SIZE * sizeof(wchar_t));
			EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
				pIter, EACT_PICTUREFRAME_CHANGE_PIC, lszBkgImage,
				(wcslen(lszBkgImage) + 1) * sizeof(wchar_t), NULL, 0);*/

			ULONG lPosX = lpKeyBkg->QuerySubKeyValueAsLONG(SB_KEY_X);
			ULONG lPosY = lpKeyBkg->QuerySubKeyValueAsLONG(SB_KEY_Y);
			pIter->SetPosition((FLOAT)lPosX, (FLOAT)lPosY);

			ULONG lWidth = lpKeyBkg->QuerySubKeyValueAsLONG(SB_KEY_WIDTH);
			ULONG lHeight = lpKeyBkg->QuerySubKeyValueAsLONG(SB_KEY_HEIGHT);
			pIter->SetSize((FLOAT)lWidth, (FLOAT)lHeight);
		}
	}

	if (mpEdit)
	{
		//���ñ༭��
		IEinkuiIterator* pIter = mpEdit->GetIterator();
		ICfKey* lpKeyEdit = mpTemplete->GetSubKey(SB_KEY_EDIT);

		if (lpKeyEdit)
		{
			ULONG lPosX = lpKeyEdit->QuerySubKeyValueAsLONG(SB_KEY_X);
			ULONG lPosY = lpKeyEdit->QuerySubKeyValueAsLONG(SB_KEY_Y);
			pIter->SetPosition((FLOAT)lPosX - 5.0f, (FLOAT)lPosY);

			ULONG lWidth = lpKeyEdit->QuerySubKeyValueAsLONG(SB_KEY_WIDTH);
			ULONG lHeight = lpKeyEdit->QuerySubKeyValueAsLONG(SB_KEY_HEIGHT);
			pIter->SetSize((FLOAT)lWidth, (FLOAT)lHeight);
		}

		//����ֻ��������
		LONG lMode = 1;
		EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
			pIter, EACT_EDIT_NUMBER_ONLY, 
			&lMode, sizeof(LONG), NULL, 0);

		//������������
		LONG lLimit = 4;
		EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
			pIter, EACT_EDIT_SET_LENGTH_LIMIT, 
			&lLimit, sizeof(LONG), NULL, 0);
	}

	ICfKey* lpKeyArrow = mpTemplete->GetSubKey(SB_KEY_ARROW);
	if (lpKeyArrow == NULL) return FALSE;

	if (mpBtnUpArrow)
	{
		//�����ϼ�ͷ��ť
		IEinkuiIterator* pIter = mpBtnUpArrow->GetIterator();
		ICfKey* lpKeyUpArrow = lpKeyArrow->GetSubKey(SB_KEY_UP_ARROW);
		if (lpKeyUpArrow)
		{
			ULONG lPosX = lpKeyUpArrow->QuerySubKeyValueAsLONG(SB_KEY_X);
			ULONG lPosY = lpKeyUpArrow->QuerySubKeyValueAsLONG(SB_KEY_Y);
			pIter->SetPosition((FLOAT)lPosX, (FLOAT)lPosY);

			ULONG lWidth = lpKeyUpArrow->QuerySubKeyValueAsLONG(SB_KEY_WIDTH);
			ULONG lHeight = lpKeyUpArrow->QuerySubKeyValueAsLONG(SB_KEY_HEIGHT);
			pIter->SetSize((FLOAT)lWidth, (FLOAT)lHeight);
		}
	}

	if (mpBtnDownArrow)
	{
		//�����¼�ͷ��ť
		IEinkuiIterator* pIter = mpBtnDownArrow->GetIterator();
		ICfKey* lpKeyDownArrow = lpKeyArrow->GetSubKey(SB_KEY_DOWN_ARROW);
		if (lpKeyDownArrow)
		{
			ULONG lPosX = lpKeyDownArrow->QuerySubKeyValueAsLONG(SB_KEY_X);
			ULONG lPosY = lpKeyDownArrow->QuerySubKeyValueAsLONG(SB_KEY_Y);
			pIter->SetPosition((FLOAT)lPosX, (FLOAT)lPosY);

			ULONG lWidth = lpKeyDownArrow->QuerySubKeyValueAsLONG(SB_KEY_WIDTH);
			ULONG lHeight = lpKeyDownArrow->QuerySubKeyValueAsLONG(SB_KEY_HEIGHT);
			pIter->SetSize((FLOAT)lWidth, (FLOAT)lHeight);
		}
	}

	//��ȡĬ��ֵ
	int lDefaultValue = 0;
	ICfKey* lpKeyDefaultValue = mpTemplete->GetSubKey(SB_KEY_DEFAULT_VALUE);
	if (lpKeyDefaultValue)	lpKeyDefaultValue->GetValue(&lDefaultValue, sizeof(int));
	SetCurrentValue(lDefaultValue);

	//// ���ð�ť��Ӧ����
	//if(mpUpperPicture)
	//{
	//	D2D1_SIZE_F ldfSize;
	//	ldfSize = mpUpperPicture->GetIterator()->GetSize();
	//	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(mpCurrentItemButton->GetIterator(),
	//		EACT_BUTTON_SET_ACTION_RECT, &ldfSize,  sizeof(D2D1_SIZE_F), NULL, 0);

	//}

	return TRUE;
}

BOOL CEvSpinButton::SetCurrentValue(const int nValue)
{
	BOOL lbResult = FALSE;
	int lnValue = nValue;

	if (nValue > miMaxValue)		lnValue = miMaxValue;
	if (nValue < miMinValue)		lnValue = miMinValue;
	
	mnCurrentValue = lnValue;

	// ��������ֵ����������Ӱ�ť
	if(mnCurrentValue == miMaxValue)
	{
		mpBtnUpArrow->GetIterator()->SetEnable(false);
	}
	else
	{
		if(false == mpBtnUpArrow->GetIterator()->IsEnable())
			mpBtnUpArrow->GetIterator()->SetEnable(true);
	}

	// �������Сֵ������ü�С��ť
	if(mnCurrentValue == miMinValue)
	{
		mpBtnDownArrow->GetIterator()->SetEnable(false);
	}
	else
	{
		if(false == mpBtnDownArrow->GetIterator()->IsEnable())
			mpBtnDownArrow->GetIterator()->SetEnable(true);
	}

	return UpdateEditView();
}

BOOL CEvSpinButton::SetCurrentValueByDisplay()
{
	BOOL lbResult = FALSE;

	wchar_t buf[SPINBUTTON_BUF_SIZE] = { 0 };
	
	if (mpEdit)
	{
		EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
			mpEdit->GetIterator(), EACT_EDIT_GET_TEXT, NULL, 0,
			buf, SPINBUTTON_BUF_SIZE * sizeof(wchar_t));

		mnCurrentValue = _wtoi(buf);

		lbResult = TRUE;
	}


	return lbResult;
}

//���±༭��ͼ
BOOL CEvSpinButton::UpdateEditView()
{
	BOOL lbResult = FALSE;

	wchar_t buf[SPINBUTTON_BUF_SIZE] = { 0 };
	_itow_s(mnCurrentValue, buf, SPINBUTTON_BUF_SIZE, 10);

	if (mpEdit)
	{
		EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
			mpEdit->GetIterator(), EACT_EDIT_SET_TEXT, 
			buf, SPINBUTTON_BUF_SIZE * sizeof(wchar_t), NULL, 0);

		lbResult = TRUE;
	}

	return lbResult;
} 

int	CEvSpinButton::GetCurrentValue()
{
	SetCurrentValueByDisplay();
	return mnCurrentValue;
}