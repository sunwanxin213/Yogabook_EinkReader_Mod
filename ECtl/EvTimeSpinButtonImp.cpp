/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "stdafx.h"
#include "cmmstruct.h"
#include "Einkui.h"
#include "ElementImp.h"
#include "XCtl.h"
#include "EvTimeSpinButtonImp.h"


//�����ֵ
#define TSB_KEY_DEFAULT_CTRL					L"DefaultCtrl"
#define TSB_KEY_TIME_SPIN_BUTTON				L"TimeSpinButton"
#define TSB_KEY_DEFAULT_EDIT					L"DefaultEdit"
#define TSB_KEY_DEFAULT_LABEL					L"DefaultLabel"
#define TSB_KEY_DEFAULT_IMAGE_BUTTON_UP			L"DefaultImageButtonUp"
#define TSB_KEY_DEFAULT_IMAGE_BUTTON_DOWN		L"DefaultImageButtonDown"

#define TSB_KEY_ARROW							L"Arrow"
#define TSB_KEY_UP_ARROW						L"UpArrow"
#define TSB_KEY_DOWN_ARROW						L"DownArrow"
#define TSB_KEY_COLON_ONE						L"LabelColonOne"
#define TSB_KEY_COLON_TWO						L"LabelColonTwo"
#define TSB_KEY_EDIT_MINUTES					L"EditMinutes"
#define TSB_KEY_EDIT_SECONDS					L"EditSeconds"
#define TSB_KEY_EDIT_MILLISECONDS				L"EditMilliseconds"

#define TSB_KEY_WIDTH							L"Width"
#define TSB_KEY_HEIGHT							L"Height"
#define TSB_KEY_X								L"X"
#define TSB_KEY_Y								L"Y"

//����
#define TSB_MAX_MINUTES							60
#define TSB_MAX_SECONDS							60
#define TSB_MAX_MILLISECONDS					1000


DEFINE_BUILTIN_NAME(TimeSpinButton)


// ֻ���ڱ������ó�ʼֵ����ָ����ΪNULL�����п���ʧ�ܵ������֮������㶼Ӧ����InitOnCreate�н���
CEvTimeSpinButton::CEvTimeSpinButton() : 
	mpEditMinutes(NULL),
	mpEditSeconds(NULL),
	mpEditMilliseconds(NULL),
	mpBtnUpArrow(NULL),
	mpBtnDownArrow(NULL),
	mpLabelColonOne(NULL),
	mpLabelColonTwo(NULL),
	mnCurrentEditID(-1),
	mbSendModifyMsgWhenEditModify(false)
{	
}

CEvTimeSpinButton::~CEvTimeSpinButton() {}

ULONG CEvTimeSpinButton::InitOnCreate(
	IN IXuiIterator* npParent,	// ������ָ��
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

		//����TimeSpinButton���ӿؼ�

		IConfigFile * lpConfigFile = EinkuiGetSystem()->GetCurrentWidget()->GetDefaultFactory()->GetTempleteFile();
		if (lpConfigFile == NULL) break;
		ICfKey* lpRootKey = lpConfigFile->GetRootKey();
		if (lpRootKey == NULL) break;
		ICfKey* lpDefaultCtrlKey = lpRootKey->GetSubKey(TSB_KEY_DEFAULT_CTRL);
		if (lpDefaultCtrlKey == NULL) break;
		ICfKey* lpTimeSpinButtonKey = lpDefaultCtrlKey->GetSubKey(TSB_KEY_TIME_SPIN_BUTTON);
		if (lpTimeSpinButtonKey == NULL) break;
		//�����ϼ�ͷ
		ICfKey * lpButtonUpKey = lpTimeSpinButtonKey->GetSubKey(TSB_KEY_DEFAULT_IMAGE_BUTTON_UP);
		if(lpButtonUpKey)
		{
			mpBtnUpArrow = CEvImageButton::CreateInstance(mpIterator, lpButtonUpKey, TSB_ID_CTRL_BUTTON_UP);			
		}
		//�����¼�ͷ
		ICfKey * lpButtonDownKey = lpTimeSpinButtonKey->GetSubKey(TSB_KEY_DEFAULT_IMAGE_BUTTON_DOWN);
		if(lpButtonDownKey)
		{
			mpBtnDownArrow = CEvImageButton::CreateInstance(mpIterator, lpButtonDownKey, TSB_ID_CTRL_BUTTON_DOWN);			
		}
		//����ð��1
		ICfKey * lpLabelColonOneKey = lpTimeSpinButtonKey->GetSubKey(TSB_KEY_DEFAULT_LABEL);
		if(lpLabelColonOneKey)
		{
			mpLabelColonOne = CEvLabelImp::CreateInstance(mpIterator, lpLabelColonOneKey, TSB_ID_CTRL_LABEL_COLON_ONE);			
		}
		//����ð��2
		ICfKey * lpLabelColonTwoKey = lpTimeSpinButtonKey->GetSubKey(TSB_KEY_DEFAULT_LABEL);
		if(lpLabelColonTwoKey)
		{
			mpLabelColonTwo = CEvLabelImp::CreateInstance(mpIterator, lpLabelColonTwoKey, TSB_ID_CTRL_LABEL_COLON_TWO);			
		}
		//�����༭���
		ICfKey * lpEditMinutesKey = lpTimeSpinButtonKey->GetSubKey(TSB_KEY_DEFAULT_EDIT);
		if (lpEditMinutesKey)
		{
			mpEditMinutes = CEvEditImp::CreateInstance(mpIterator, lpEditMinutesKey, TSB_ID_CTRL_EDIT_MINUTES);			
		}
		//�����༭����
		ICfKey * lpEditSecondsKey = lpTimeSpinButtonKey->GetSubKey(TSB_KEY_DEFAULT_EDIT);
		if (lpEditSecondsKey)
		{
			mpEditSeconds = CEvEditImp::CreateInstance(mpIterator, lpEditSecondsKey, TSB_ID_CTRL_EDIT_SECONDS);			
		}
		//�����༭�����
		ICfKey * lpEditMillisecondsKey = lpTimeSpinButtonKey->GetSubKey(TSB_KEY_DEFAULT_EDIT);
		if (lpEditMillisecondsKey)
		{
			mpEditMilliseconds = CEvEditImp::CreateInstance(mpIterator, lpEditMillisecondsKey, TSB_ID_CTRL_EDIT_MILLISECONDS);			
		}

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEvTimeSpinButton::OnElementCreate(IXuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		SetChildCtrlPara();

		mpIterator->ModifyStyles(EITR_STYLE_POPUP);

		//mpIterator->SetEnable(false);

		lResult = ERESULT_SUCCESS;
	}while(false);

	return lResult;
}

//����
ERESULT CEvTimeSpinButton::OnPaint(IXuiPaintBoard* npPaintBoard)
{

	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npPaintBoard);

		if (mpBgBitmap != NULL)
		{
			npPaintBoard->DrawBitmap(D2D1::RectF(0,0,mpIterator->GetSizeX(),mpIterator->GetSizeY()),
				D2D1::RectF(0.0f,0.0f,(float)mpBgBitmap->GetWidth(),(float)mpBgBitmap->GetHeight()),
				mpBgBitmap,
				ESPB_DRAWBMP_EXTEND
				);
		}

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
ERESULT CEvTimeSpinButton::ParseMessage(IXuiMessage* npMsg)
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
				IXuiIterator* lpIter = npMsg->GetMessageSender();

				int lnValue = 0;

				if (lpIter == mpBtnUpArrow->GetIterator())
				{
					if (mnCurrentEditID == 0)
					{
						mCurrentTime.nMinutes++;
						if (mCurrentTime.nMinutes >= TSB_MAX_MINUTES) 
							mCurrentTime.nMinutes = 0;
					}
					else if (mnCurrentEditID == 1)
					{
						mCurrentTime.nSeconds++;
						if (mCurrentTime.nSeconds >= TSB_MAX_SECONDS) 
							mCurrentTime.nSeconds = 0;
					}
					else if (mnCurrentEditID == 2)
					{
						mCurrentTime.nMillisecond++;
						if (mCurrentTime.nMillisecond >= TSB_MAX_MILLISECONDS) 
							mCurrentTime.nMillisecond = 0;
					}
				}
				else if (lpIter == mpBtnDownArrow->GetIterator())
				{
					if (mnCurrentEditID == 0)
					{
						mCurrentTime.nMinutes--;
						if (mCurrentTime.nMinutes < 0) 
							mCurrentTime.nMinutes = TSB_MAX_MINUTES - 1;
					}
					else if (mnCurrentEditID == 1)
					{
						mCurrentTime.nSeconds--;
						if (mCurrentTime.nSeconds < 0) 
							mCurrentTime.nSeconds = TSB_MAX_SECONDS - 1;
					}
					else if (mnCurrentEditID == 2)
					{
						mCurrentTime.nMillisecond--;
						if (mCurrentTime.nMillisecond < 0) 
							mCurrentTime.nMillisecond = TSB_MAX_MILLISECONDS - 1;
					}
				}
				
				UpdateView();

				CExMessage::PostMessage(mpIterator->GetParent(),
					mpIterator, EEVT_TIMESPINBUTTON_TIME_MODIFIED, mCurrentTime);
			}
			break;

		case EACT_TIMESPINBUTTON_SET_TIME:
			{
				XuiTimeFormat* lpValue = (XuiTimeFormat*)npMsg->GetInputData();
				SetCurrentTime(*lpValue);
			}
			break;

		case EACT_TIMESPINBUTTON_GET_TIME:
			{
				XuiTimeFormat* lpValue = (XuiTimeFormat*)npMsg->GetOutputBuffer();
				//SetCurrentValueByDisplay();
				*lpValue = GetCurrentTime();
			}
			break;

		case EEVT_EDIT_CONTENT_COMPLETION:
			{
				wchar_t* lswContent = new wchar_t[128];

				if (npMsg->GetMessageSender() == mpEditMinutes->GetIterator())
				{
					memset(lswContent, 0, 128);
					CExMessage::SendMessage(mpEditMinutes->GetIterator(), 
						mpIterator, EACT_EDIT_GET_TEXT, NULL, lswContent, 128 * sizeof(wchar_t));
					int nMinutes = _wtoi(lswContent);
					if (nMinutes != mCurrentTime.nMinutes)
					{
						if ( (nMinutes < TSB_MAX_MINUTES) && (nMinutes >= 0) )
						{
							mCurrentTime.nMinutes = nMinutes;
						}
						UpdateView();
					}
				}
				else if (npMsg->GetMessageSender() == mpEditSeconds->GetIterator())
				{
					memset(lswContent, 0, 128);
					CExMessage::SendMessage(mpEditSeconds->GetIterator(), 
						mpIterator, EACT_EDIT_GET_TEXT, NULL, lswContent, 128 * sizeof(wchar_t));
					int nSeconds = _wtoi(lswContent);
					if (nSeconds != mCurrentTime.nSeconds)
					{
						if ( (nSeconds < TSB_MAX_SECONDS) && (nSeconds >= 0) )
						{
							mCurrentTime.nSeconds = nSeconds;
						}
						UpdateView();
					}
				}
				else if (npMsg->GetMessageSender() == mpEditMilliseconds->GetIterator())
				{
					memset(lswContent, 0, 128);
					CExMessage::SendMessage(mpEditMilliseconds->GetIterator(), 
						mpIterator, EACT_EDIT_GET_TEXT, NULL, lswContent, 128 * sizeof(wchar_t));
					int nMilliseconds = _wtoi(lswContent);
					if (nMilliseconds != mCurrentTime.nMillisecond)
					{
						if ( (nMilliseconds < TSB_MAX_MILLISECONDS) && (nMilliseconds >= 0) )
						{
							mCurrentTime.nMillisecond = nMilliseconds;
						}
						UpdateView();
					}
				}

				CExMessage::PostMessage(mpIterator->GetParent(),
					mpIterator, EEVT_TIMESPINBUTTON_TIME_MODIFIED, mCurrentTime);
			}
			break;

		case EEVT_EDIT_KEYBOARD_FOCUS:
			{
				if (npMsg->GetInputDataSize() != sizeof(LONG))
				{
					luResult = ERESULT_UNEXPECTED_MESSAGE;
					break;
				}
				LONG llFlag = *(LONG*)npMsg->GetInputData();

				IXuiIterator* iter = npMsg->GetMessageSender();

				if(llFlag == 0)	//ʧȥ����
				{
				}
				else
				{
					if (iter == mpEditMinutes->GetIterator())
					{
						mnCurrentEditID = 0;
					}
					else if (iter == mpEditSeconds->GetIterator())
					{
						mnCurrentEditID = 1;
					}
					else if (iter == mpEditMilliseconds->GetIterator())
					{
						mnCurrentEditID = 2;
					}
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
ERESULT CEvTimeSpinButton::OnElementEnable(bool nbIsEnable)
{
	SetTimeSpinButtonEnable(nbIsEnable);

	return ERROR_SUCCESS;
}

bool CEvTimeSpinButton::SetCurrentTime(XuiTimeFormat time)
{
	mCurrentTime = time;

	UpdateView();

	return true;
}

XuiTimeFormat CEvTimeSpinButton::GetCurrentTime() const
{
	return mCurrentTime;
}

void CEvTimeSpinButton::UpdateView()
{
	wstring lwsMinutes;
	wstring lwsSeconds;
	wstring lwsMilliseconds;

	wchar_t lwBuf[128];

	memset(lwBuf, 0, sizeof(wchar_t) * 128);
	_itow_s(mCurrentTime.nMinutes, lwBuf, 10);
	if (mCurrentTime.nMinutes < 10)
	{
		lwsMinutes = L"0";
	}
	lwsMinutes += lwBuf;

	memset(lwBuf, 0, sizeof(wchar_t) * 128);
	_itow_s(mCurrentTime.nSeconds, lwBuf, 10);
	if (mCurrentTime.nSeconds < 10)
	{
		lwsSeconds = L"0";
	}
	lwsSeconds += lwBuf;

	memset(lwBuf, 0, sizeof(wchar_t) * 128);
	_itow_s(mCurrentTime.nMillisecond, lwBuf, 10);
	if (mCurrentTime.nMillisecond < 10)
	{
		lwsMilliseconds = L"00";
	}
	else if (mCurrentTime.nMillisecond < 100)
	{
		lwsMilliseconds = L"0";
	}
	lwsMilliseconds += lwBuf;

	//���½���ؼ�
	CExMessage::SendMessageWithText(mpEditMinutes->GetIterator(), mpIterator, 
		EACT_EDIT_SET_TEXT, lwsMinutes.c_str());
	CExMessage::SendMessageWithText(mpEditSeconds->GetIterator(), mpIterator, 
		EACT_EDIT_SET_TEXT, lwsSeconds.c_str());
	CExMessage::SendMessageWithText(mpEditMilliseconds->GetIterator(), mpIterator, 
		EACT_EDIT_SET_TEXT, lwsMilliseconds.c_str());
}

bool CEvTimeSpinButton::SetTimeSpinButtonEnable(bool nbIsEnable)
{
	if (nbIsEnable)
	{
		if (mpIterator) mpIterator->ModifyStyles(EITR_STYLE_MOUSE, NULL);
	}
	else
	{
		if (mpIterator) mpIterator->ModifyStyles(NULL, EITR_STYLE_MOUSE);
	}

	if (mpBtnUpArrow) mpBtnUpArrow->GetIterator()->SetEnable(nbIsEnable);
	if (mpBtnDownArrow) mpBtnDownArrow->GetIterator()->SetEnable(nbIsEnable);

	if (mpEditMinutes) mpEditMinutes->GetIterator()->SetEnable(nbIsEnable);
	if (mpEditSeconds) mpEditSeconds->GetIterator()->SetEnable(nbIsEnable);
	if (mpEditMilliseconds) mpEditMilliseconds->GetIterator()->SetEnable(nbIsEnable);

	return true;
}

BOOL CEvTimeSpinButton::SetChildCtrlPara()
{
	if (mpTemplete == NULL) return FALSE;


	ICfKey* lpKeyArrow = mpTemplete->GetSubKey(TSB_KEY_ARROW);
	if (lpKeyArrow == NULL) return FALSE;

	//�����ϼ�ͷ��ť
	if (mpBtnUpArrow)
	{
		IXuiIterator* pIter = mpBtnUpArrow->GetIterator();
		ICfKey* lpKeyUpArrow = lpKeyArrow->GetSubKey(TSB_KEY_UP_ARROW);
		if (lpKeyUpArrow)
		{
			ULONG lPosX = lpKeyUpArrow->QuerySubKeyValueAsLONG(TSB_KEY_X);
			ULONG lPosY = lpKeyUpArrow->QuerySubKeyValueAsLONG(TSB_KEY_Y);
			pIter->SetPosition((FLOAT)lPosX, (FLOAT)lPosY);

			ULONG lWidth = lpKeyUpArrow->QuerySubKeyValueAsLONG(TSB_KEY_WIDTH);
			ULONG lHeight = lpKeyUpArrow->QuerySubKeyValueAsLONG(TSB_KEY_HEIGHT);
			pIter->SetSize((FLOAT)lWidth, (FLOAT)lHeight);
		}
	}

	//�����¼�ͷ��ť
	if (mpBtnDownArrow)
	{
		IXuiIterator* pIter = mpBtnDownArrow->GetIterator();
		ICfKey* lpKeyDownArrow = lpKeyArrow->GetSubKey(TSB_KEY_DOWN_ARROW);
		if (lpKeyDownArrow)
		{
			ULONG lPosX = lpKeyDownArrow->QuerySubKeyValueAsLONG(TSB_KEY_X);
			ULONG lPosY = lpKeyDownArrow->QuerySubKeyValueAsLONG(TSB_KEY_Y);
			pIter->SetPosition((FLOAT)lPosX, (FLOAT)lPosY);

			ULONG lWidth = lpKeyDownArrow->QuerySubKeyValueAsLONG(TSB_KEY_WIDTH);
			ULONG lHeight = lpKeyDownArrow->QuerySubKeyValueAsLONG(TSB_KEY_HEIGHT);
			pIter->SetSize((FLOAT)lWidth, (FLOAT)lHeight);
		}
	}

	ICfKey* lpKeyColonOne = mpTemplete->GetSubKey(TSB_KEY_COLON_ONE);
	if (lpKeyColonOne == NULL) return FALSE;
	ICfKey* lpKeyColonTwo = mpTemplete->GetSubKey(TSB_KEY_COLON_TWO);
	if (lpKeyColonTwo == NULL) return FALSE;
	//����ð��1
	if (mpLabelColonOne)
	{
		ULONG lPosX = lpKeyColonOne->QuerySubKeyValueAsLONG(TSB_KEY_X);
		ULONG lPosY = lpKeyColonOne->QuerySubKeyValueAsLONG(TSB_KEY_Y);
		mpLabelColonOne->GetIterator()->SetPosition((FLOAT)lPosX, (FLOAT)lPosY);
	}
	//����ð��2
	if (mpLabelColonTwo)
	{
		ULONG lPosX = lpKeyColonTwo->QuerySubKeyValueAsLONG(TSB_KEY_X);
		ULONG lPosY = lpKeyColonTwo->QuerySubKeyValueAsLONG(TSB_KEY_Y);
		mpLabelColonTwo->GetIterator()->SetPosition((FLOAT)lPosX, (FLOAT)lPosY);
	}

	ICfKey* lpKeyMinutesEdit = mpTemplete->GetSubKey(TSB_KEY_EDIT_MINUTES);
	if (lpKeyMinutesEdit == NULL) return FALSE;
	ICfKey* lpKeySecendsEdit = mpTemplete->GetSubKey(TSB_KEY_EDIT_SECONDS);
	if (lpKeySecendsEdit == NULL) return FALSE;
	ICfKey* lpKeyMillisecondsEdit = mpTemplete->GetSubKey(TSB_KEY_EDIT_MILLISECONDS);
	if (lpKeyMillisecondsEdit == NULL) return FALSE;
	//���÷ֱ༭��
	if (mpEditMinutes)
	{
		ULONG lPosX = lpKeyMinutesEdit->QuerySubKeyValueAsLONG(TSB_KEY_X);
		ULONG lPosY = lpKeyMinutesEdit->QuerySubKeyValueAsLONG(TSB_KEY_Y);
		mpEditMinutes->GetIterator()->SetPosition((FLOAT)lPosX, (FLOAT)lPosY);

		ULONG lWidth = lpKeyMinutesEdit->QuerySubKeyValueAsLONG(TSB_KEY_WIDTH);
		ULONG lHeight = lpKeyMinutesEdit->QuerySubKeyValueAsLONG(TSB_KEY_HEIGHT);
		mpEditMinutes->GetIterator()->SetSize((FLOAT)lWidth, (FLOAT)lHeight);
	}
	//������༭��
	if (mpEditSeconds)
	{
		ULONG lPosX = lpKeySecendsEdit->QuerySubKeyValueAsLONG(TSB_KEY_X);
		ULONG lPosY = lpKeySecendsEdit->QuerySubKeyValueAsLONG(TSB_KEY_Y);
		mpEditSeconds->GetIterator()->SetPosition((FLOAT)lPosX, (FLOAT)lPosY);

		ULONG lWidth = lpKeySecendsEdit->QuerySubKeyValueAsLONG(TSB_KEY_WIDTH);
		ULONG lHeight = lpKeySecendsEdit->QuerySubKeyValueAsLONG(TSB_KEY_HEIGHT);
		mpEditSeconds->GetIterator()->SetSize((FLOAT)lWidth, (FLOAT)lHeight);
	}
	//���ú���༭��
	if (mpEditMilliseconds)
	{
		ULONG lPosX = lpKeyMillisecondsEdit->QuerySubKeyValueAsLONG(TSB_KEY_X);
		ULONG lPosY = lpKeyMillisecondsEdit->QuerySubKeyValueAsLONG(TSB_KEY_Y);
		mpEditMilliseconds->GetIterator()->SetPosition((FLOAT)lPosX, (FLOAT)lPosY);

		ULONG lWidth = lpKeyMillisecondsEdit->QuerySubKeyValueAsLONG(TSB_KEY_WIDTH);
		ULONG lHeight = lpKeyMillisecondsEdit->QuerySubKeyValueAsLONG(TSB_KEY_HEIGHT);
		mpEditMilliseconds->GetIterator()->SetSize((FLOAT)lWidth, (FLOAT)lHeight);
	}

	//��������򳤶�
	LONG lLimit = 2;
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(mpEditMinutes->GetIterator(),
		EACT_EDIT_SET_LENGTH_LIMIT, &lLimit, sizeof(LONG), NULL, 0);
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(mpEditSeconds->GetIterator(),
		EACT_EDIT_SET_LENGTH_LIMIT, &lLimit, sizeof(LONG), NULL, 0);
	lLimit = 3;
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(mpEditMilliseconds->GetIterator(),
		EACT_EDIT_SET_LENGTH_LIMIT, &lLimit, sizeof(LONG), NULL, 0);

	//ֻ��������
	int liAccept = 1;
	CExMessage::PostMessageW(mpEditMinutes->GetIterator(), mpIterator, EACT_EDIT_NUMBER_ONLY, liAccept);
	CExMessage::PostMessageW(mpEditSeconds->GetIterator(), mpIterator, EACT_EDIT_NUMBER_ONLY, liAccept);
	CExMessage::PostMessageW(mpEditMilliseconds->GetIterator(), mpIterator, EACT_EDIT_NUMBER_ONLY, liAccept);

	UpdateView();

	return TRUE;
}
