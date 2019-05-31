/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */



#include "stdafx.h"

#include "cmmstruct.h"
#include "Einkui.h"
#include "ElementImp.h"
#include "XCtl.h"
#include "EvImageButtonImp.h"

//���峣��
#define BUF_SIZE			256

//�����ֵ

#define IB_KEY_DEFAULT_CTRL						L"DefaultCtrl"
#define IB_KEY_IMAGE_BUTTON						L"ImageButton"
#define IB_KEY_DEFAULT_PICTURE_FRAME			L"DefaultPictureFrame"

#define IB_KEY_STYLE							L"Style"
#define IB_KEY_SELECT_FRAME						L"SelectFrame"
#define IB_KEY_FRAME_COUNT						L"FrameCount"
#define IB_KEY_LEFT_PICTURE						L"LeftPicture"
#define IB_KEY_RIGHT_PICTURE					L"RightPicture"

#define IB_KEY_X								L"X"
#define IB_KEY_Y								L"Y"
#define IB_KEY_WIDTH							L"Width"
#define IB_KEY_HEIGHT							L"Height"
#define IB_KEY_BACKGROUND						L"BackGround"
#define IB_KEY_TIP								L"Tip"
#define IB_KEY_EXPANDABLE						L"Expandable"
//#define IB_KEY_PIC_PATH_LIST					L"PicPathList"
#define IB_KEY_MENU_ITEM_LIST					L"MenuItemList"
#define IB_KEY_MENU_ITEM_ID						L"Id"
#define IB_KEY_MENU_ITEM_PIC_PATH				L"PicPath"
	//��Ϣ���
#define IB_KEY_MESSAGE_INFO							L"MessageInfo"
#define IB_KEY_MESSAGE_INFO_ID						L"ID"
#define IB_KEY_MESSAGE_INFO_MESSAGE_TYPE			L"MessageType"
#define IB_KEY_MESSAGE_INFO_MESSAGE_TYPE_NONE		L"0"
#define IB_KEY_MESSAGE_INFO_MESSAGE_TYPE_BOOL		L"1"
#define IB_KEY_MESSAGE_INFO_MESSAGE_TYPE_INT		L"2"
#define IB_KEY_MESSAGE_INFO_MESSAGE_TYPE_FLOAT		L"3"
#define IB_KEY_MESSAGE_INFO_MESSAGE_TYPE_STRING		L"4"
	//��ɫ��־
#define IB_KEY_COLOR_FLAG							L"ColorFlag"
#define IB_KEY_COLOR_FLAG_true						L"1"
#define IB_KEY_COLOR_FLAG_FLASE						L"0"


//������
#define IB_STYLE_UNCHECKABLE					L"0"
#define IB_STYLE_CHECKABLE						L"1"

#define IB_STYLE_UNEXPANDABLE					L"0"
#define IB_STYLE_EXPANDABLE						L"1"


//�����ӿؼ�
#define	IB_ID_EXPAND_CTRL						10		//��չ�ؼ�

DEFINE_BUILTIN_NAME(ImageButton)

// ֻ���ڱ������ó�ʼֵ����ָ����ΪNULL�����п���ʧ�ܵ������֮������㶼Ӧ����InitOnCreate�н���
CEvImageButton::CEvImageButton() :
	mpLeftPicture(NULL),
	mpRightPicture(NULL),
	mpBitmapSelectOrOver(NULL),
	mbIsMouseFocus(false),
	mbIsKeyboardFocus(false),
	mbIsPressed(false),
	mbChecked(false),
	mnStyle(-1),
	mbExpandable(false),
	mpIterExpandCtrl(NULL),
	mbShowExpandCtrl(false),
	mbHasColorFlag(false),
	mluRGBColor(0),
	mfRadio(1.0f),
	mbDrawShape(true)
	//mlCurrentPage(0),
	//mlPageCountMax(0)
{	
}

CEvImageButton::~CEvImageButton() 
{
}

ULONG CEvImageButton::InitOnCreate(
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

		//����ImageButton���ӿؼ�
		IConfigFile * lpConfigFile = EinkuiGetSystem()->GetCurrentWidget()->GetDefaultFactory()->GetTempleteFile();
		if (lpConfigFile == NULL) break;
		ICfKey* lpRootKey = lpConfigFile->GetRootKey();
		if (lpRootKey == NULL) break;
		ICfKey* lpDefaultCtrlKey = lpRootKey->GetSubKey(IB_KEY_DEFAULT_CTRL);
		if (lpDefaultCtrlKey == NULL) break;
		ICfKey* lpImageButtonKey = lpDefaultCtrlKey->GetSubKey(IB_KEY_IMAGE_BUTTON);
		if (lpImageButtonKey == NULL) break;

		ICfKey * lpDefaultPictureFrameKey = lpImageButtonKey->GetSubKey(IB_KEY_DEFAULT_PICTURE_FRAME);
		if (lpDefaultPictureFrameKey == NULL) break;

		//	��ʼ��UnificSetting
		//mpUnificSetting = GetUnificSetting();

		//������벿��PictureFrame
		mpLeftPicture = CEvPictureFrame::CreateInstance(mpIterator, lpDefaultPictureFrameKey, IB_ID_CTRL_LEFT_PICTURE);			

		//�����Ұ벿��PictureFrame
		mpRightPicture = CEvPictureFrame::CreateInstance(mpIterator, lpDefaultPictureFrameKey, IB_ID_CTRL_RIGHT_PICTURE);			
		
		//mpLeftPicture->GetIterator()->ModifyStyles(NULL, EITR_STYLE_MOUSE);
		//mpLeftPicture->GetIterator()->ModifyStyles(NULL, EITR_STYLE_MOUSE);
		//mpIterator->ModifyStyles(NULL, EITR_STYLE_MOUSE);
		//mpIterator->ModifyStyles(EITR_STYLE_ALL_MWHEEL|EITR_STYLE_KEYBOARD);

		ICfKey* lpValue = mpTemplete->GetSubKey(IB_KEY_TIP); //Tip1
		wchar_t* lpszTip = NULL;

		do 
		{
			BREAK_ON_NULL(lpValue);
			LONG llLen = lpValue->GetValueLength();
			lpszTip = new wchar_t[llLen];
			BREAK_ON_NULL(lpszTip);

			lpValue->GetValue(lpszTip,lpValue->GetValueLength());
			
			mpIterator->SetToolTip(lpszTip);

		} while (false);

		CMM_SAFE_DELETE(lpszTip);
		CMM_SAFE_RELEASE(lpValue);
		//SetImageButtonEnable(false);

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEvImageButton::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		SetChildCtrlPara();
		//LoadResource();

		if (mbExpandable == true)
		{
			mpIterExpandCtrl = mpIterator->GetSubElementByID(IB_ID_EXPAND_CTRL);
			if (mpIterExpandCtrl)
			{
				mpIterator->ModifyStyles(EITR_STYLE_POPUP);
				mpIterExpandCtrl->SetVisible(mbShowExpandCtrl);
			}
		}
		
		lResult = ERESULT_SUCCESS;
	} while(false);

	return lResult;
}

//��������뿪
void CEvImageButton::OnMouseFocus(PSTEMS_STATE_CHANGE npState)
{
	if (npState->State != 0)
	{
		//������
		if(mbIsMouseFocus == false) mbIsMouseFocus = true;
	}
	else
	{
		//����Ƴ�
		if(mbIsMouseFocus != false) mbIsMouseFocus = false;

		if(mbIsPressed != false)  //�����������ˣ���ȥ������״̬
			mbIsPressed = false;
	}

	EinkuiGetSystem()->UpdateView();
}

//��갴��
ERESULT CEvImageButton::OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npInfo);

		if(mpIterator->IsEnable() == false)
			break;	//����ǽ���״̬���Ͳ���������

		if((npInfo->ActKey & MK_LBUTTON) == 0)  //��������������Ͳ�����
			break;

		if ( (npInfo->Presssed == false) && (mbIsPressed == true) )
		{
			//���̧��
			mbIsPressed = false;	

			if (mnStyle == 1) mbChecked = !mbChecked;		//���ð�ť ѡ�� / ȡ��ѡ��

			//���������һ��Click
			PostMessageToParent(EEVT_IMAGEBUTTON_CLICK, CExMessage::DataInvalid);

			if (mnStyle == 1)
			{
				if (mbChecked == true)
				{
					PostMessageToParent(EEVT_IMAGEBUTTON_CHECKED, CExMessage::DataInvalid);
				}
				else
				{
					PostMessageToParent(EEVT_IMAGEBUTTON_UNCHECKED, CExMessage::DataInvalid);
				}
			}

			if (mbExpandable == true)
			{
				if (mpIterExpandCtrl)
				{
					//mbShowExpandCtrl = ( (mbExpandable == true) ? false : true);
					if (mbShowExpandCtrl == true)
						mbShowExpandCtrl = false;
					else
						mbShowExpandCtrl = true;

					mpIterExpandCtrl->SetVisible(mbShowExpandCtrl);

				}
			}

			if (mMsgInfo.mnMsgParaType == TMPT_NONE)
			{
				//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(
				//	mpIterator->GetParent()->GetParent(), EEVT_TOOLBARITEM_CLICK, &mMsgInfo, sizeof(TOOLBAR_MSG));
				CExMessage::PostMessage(mpIterator->GetParent()->GetParent(),mpIterator,EEVT_TOOLBARITEM_CLICK,mMsgInfo,EMSG_POSTTYPE_FAST);
			}

			if (mMsgInfo.mnMsgParaType == TMPT_BOOL)
			{
				mMsgInfo.mbBool = mbChecked;

				//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(
				//	mpIterator->GetParent()->GetParent(), EEVT_TOOLBARITEM_CLICK, &mMsgInfo, sizeof(TOOLBAR_MSG));
				CExMessage::PostMessage(mpIterator->GetParent()->GetParent(),mpIterator,EEVT_TOOLBARITEM_CLICK,mMsgInfo,EMSG_POSTTYPE_FAST);
			}

			/*if (mMsgInfo.mnMsgParaType == TMPT_INT)	
			{
				EinkuiGetSystem()->GetElementManager()->SimplePostMessage(
					mpIterator->GetParent(), EEVT_TOOLBARITEM_CLICK, &mMsgInfo, sizeof(TOOLBAR_MSG));
			}*/
		}
		else
		{
			//��갴��
			mbIsPressed = true;
		}

		EinkuiGetSystem()->UpdateView();

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

// ��������
ERESULT CEvImageButton::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	ERESULT luResult = ERESULT_SUCCESS;

	if(!(rPoint.x < 0.0f || rPoint.x >= mpIterator->GetSizeX()
		|| rPoint.y < 0.0f || rPoint.y >= mpIterator->GetSizeY()))
	{
		luResult = ERESULT_MOUSE_OWNERSHIP;
	}

	return luResult;
}

//����
ERESULT CEvImageButton::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{

	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npPaintBoard);

		if (mbIsMouseFocus == true)
		{
			ULONG lulMethod = ESPB_DRAWBMP_LINEAR;
			npPaintBoard->DrawBitmap(
				D2D1::RectF(0.0f, 0.0f, mpIterator->GetSize().width, mpIterator->GetSize().height),

				D2D1::RectF(
				(FLOAT)(mpBitmapSelectOrOver->GetWidth()) * 2.0f / 4.0f, 
				0.0f, 
				(FLOAT)(mpBitmapSelectOrOver->GetWidth()) * 3.0f / 4.0f, 
				(FLOAT)(mpBitmapSelectOrOver->GetHeight())
				),

				mpBitmapSelectOrOver,
				lulMethod
				);
		}

		if (mnStyle == 0)
		{
			if (mbIsPressed == true)
			{
				ULONG lulMethod = ESPB_DRAWBMP_LINEAR;
				npPaintBoard->DrawBitmap(
					D2D1::RectF(0.0f, 0.0f, mpIterator->GetSize().width, mpIterator->GetSize().height),

					D2D1::RectF(
					(FLOAT)(mpBitmapSelectOrOver->GetWidth()) * 3.0f / 4.0f, 
					0.0f, 
					(FLOAT)(mpBitmapSelectOrOver->GetWidth()), 
					(FLOAT)(mpBitmapSelectOrOver->GetHeight())
					),

					mpBitmapSelectOrOver,
					lulMethod
					);
			}
		}
		else if (mnStyle == 1)
		{
			if (mbChecked != false)
			{
				ULONG lulMethod = ESPB_DRAWBMP_LINEAR;
				npPaintBoard->DrawBitmap(
					D2D1::RectF(0, 0, mpIterator->GetSize().width, mpIterator->GetSize().height),

					D2D1::RectF(
					(FLOAT)(mpBitmapSelectOrOver->GetWidth()) * 3.0f / 4.0f, 
					0.0f, 
					(FLOAT)(mpBitmapSelectOrOver->GetWidth()), 
					(FLOAT)(mpBitmapSelectOrOver->GetHeight())
					),

					mpBitmapSelectOrOver,
					lulMethod
					);
			}
		}

		if ( (mbHasColorFlag != false) && (mbDrawShape != false) )
		{
			ID2D1SolidColorBrush *lpBrush;
			HRESULT hr = npPaintBoard->GetD2dRenderTarget()->CreateSolidColorBrush(
				//D2D1::ColorF(D2D1::ColorF::Red, 1.0f), &lpBrush);
				//D2D1::ColorF((float)(GetRValue(mluRGBColor)), (float)(GetGValue(mluRGBColor)), (float)GetBValue(mluRGBColor)), &lpBrush);
				D2D1::ColorF(RGB(GetBValue(mluRGBColor), GetGValue(mluRGBColor), GetRValue(mluRGBColor)), 1.0f), &lpBrush);
			npPaintBoard->GetD2dRenderTarget()->FillRectangle(
				D2D1::RectF(5.0f, 20.0f, 20.0f, 24.0f), lpBrush);	
		}
	
		lResult = ERESULT_SUCCESS;
		
	} while(false);

	return lResult;
}

// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
ERESULT CEvImageButton::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������

	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	do 
	{
		BREAK_ON_NULL(npMsg);

		switch (npMsg->GetMessageID())
		{
		case EEVT_MENUITEM_CLICK:			//��չ�ؼ��Ĳ˵�����
			{	
				//if (mpVecExpandMenuItem.Size() != 0)
				{
					//��ȡID
					if(npMsg->GetInputDataSize()!=sizeof(int) || npMsg->GetInputData()==NULL)
						luResult = ERESULT_WRONG_PARAMETERS;
					int lItemID = *(int*)(npMsg->GetInputData());

					wchar_t* pPicPath = NULL;
					for (int i = 0; i < mpVecExpandMenuItem.Size(); ++i)
					{
						if (mpVecExpandMenuItem[i].id == lItemID)
						{
							pPicPath = mpVecExpandMenuItem[i].pPicPath;

							if (pPicPath && (*pPicPath != '\0'))
							{	
								//������ť��벿��ͼƬ
								if (mpLeftPicture)
								{
									EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
										mpLeftPicture->GetIterator(), EACT_PICTUREFRAME_CHANGE_PIC, pPicPath,
										(wcslen(pPicPath) + 1) * sizeof(wchar_t), NULL, 0);
								}
							}

							break;
						}
					}

					//����toolbar��������Ϣ
					if (mMsgInfo.mnMsgParaType == TMPT_INT)
					{
						//mMsgInfo.mpMsgBuf = (int*)(npMsg->GetInputData());
						mMsgInfo.mlInterge = (LONG)*(int*)(npMsg->GetInputData());

						//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(
						//	mpIterator->GetParent()->GetParent(), EEVT_TOOLBARITEM_CLICK, &mMsgInfo, sizeof(TOOLBAR_MSG));

						//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(
						//	mpIterator->GetParent(), EEVT_TOOLBARITEM_CLICK, &mMsgInfo, sizeof(TOOLBAR_MSG));

						CExMessage::PostMessage(mpIterator->GetParent()->GetParent(),mpIterator,EEVT_TOOLBARITEM_CLICK,mMsgInfo,EMSG_POSTTYPE_FAST);
						CExMessage::PostMessage(mpIterator->GetParent(),mpIterator,EEVT_TOOLBARITEM_CLICK,mMsgInfo,EMSG_POSTTYPE_FAST);

					}

					mbShowExpandCtrl = false;
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

				if (lpActive->State == 0 && mpIterExpandCtrl->IsVisible() != false)			// �ɼ�״̬��ʧȥ����״̬����Ҫ���ص�ǰչ���Ĳ˵�
				{
					mbShowExpandCtrl = false;
					if (mpIterExpandCtrl)
					{
						mpIterExpandCtrl->SetVisible(mbShowExpandCtrl);
					}
				}

				luResult = ERESULT_SUCCESS;
			}
			break;

		case EACT_IMAGEBUTTON_SET_COLOR:
			{
				if(npMsg->GetInputDataSize() != sizeof(ULONG))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				
				mluRGBColor = *(ULONG*)npMsg->GetInputData();
				if((mluRGBColor & 0xFF000000) != 0)
					mbDrawShape = true;
				else
					mbDrawShape = false;

				EinkuiGetSystem()->UpdateView();

				luResult = ERESULT_SUCCESS;
			}
			break;

		case EACT_IMAGEBUTTON_CHANGE_LEFT_IMAGE_BKG:
			{
				
				wchar_t* lpValue = (wchar_t*)npMsg->GetInputData();

				EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
					mpLeftPicture->GetIterator(), EACT_PICTUREFRAME_CHANGE_PIC, lpValue,
					(wcslen(lpValue) + 1) * sizeof(wchar_t), NULL, 0);

				luResult = ERESULT_SUCCESS;
			}
			break;

		case EACT_IMAGEBUTTON_CHANGE_LEFT_IMAGE_BKG_FULL_PATH:
			{
			
				wchar_t* lpValue = (wchar_t*)npMsg->GetInputData();

				EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
					mpLeftPicture->GetIterator(), EACT_PICTUREFRAME_CHANGE_PIC_FULLPATH, lpValue,
					(wcslen(lpValue) + 1) * sizeof(wchar_t), NULL, 0);

				luResult = ERESULT_SUCCESS;
			}
			break;

		case EACT_IMAGEBUTTON_SET_CHECKED:
			{
				if(npMsg->GetInputDataSize() != sizeof(bool))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				// ��ȡ��������
				bool* lpValue = (bool*)npMsg->GetInputData();
				SetChecked(*lpValue);

				luResult = ERESULT_SUCCESS;
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
				bool lbEnable = true;// pSetting->GetItemEnable(mMsgInfo.mnCtrlID);
				SetImageButtonEnable(lbEnable);

				luResult = ERESULT_SUCCESS;
			}
			break;

		case EEVT_PANE_ITEM_SET_VALUE:
			{
				SetValue();

				luResult = ERESULT_SUCCESS;
			}
			break;

		case EACT_IMAGEBUTTON_SET_RATIO:
			{
				if(npMsg->GetInputDataSize() != sizeof(FLOAT))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				FLOAT* lpValue = (FLOAT*)npMsg->GetInputData();

				mfRadio = *lpValue;

				luResult = ERESULT_SUCCESS;
			}
			break;

		case EACT_IMAGEBUTTON_SET_ITEM_SELECTED:
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

		case EACT_IMAGEBUTTON_DRAW_SHAPE:
			{
				if(npMsg->GetInputDataSize() != sizeof(bool))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				// ��ȡ��������
				bool* lpValue = (bool*)npMsg->GetInputData();
				mbDrawShape = *lpValue;
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


BOOL CEvImageButton::SetChildCtrlPara()
{
	if (mpLeftPicture && mpRightPicture)
	{
		ICfKey * lpLeftPictureKey		= mpTemplete->GetSubKey(IB_KEY_LEFT_PICTURE);
		ICfKey * lpRightPictureKey		= mpTemplete->GetSubKey(IB_KEY_RIGHT_PICTURE);

		if (lpLeftPictureKey)
		{
			ULONG lPosX = lpLeftPictureKey->QuerySubKeyValueAsLONG(IB_KEY_X);
			ULONG lPosY = lpLeftPictureKey->QuerySubKeyValueAsLONG(IB_KEY_Y);
			mpLeftPicture->GetIterator()->SetPosition((FLOAT)(lPosX), (FLOAT)(lPosY));

			ULONG lHeight = lpLeftPictureKey->QuerySubKeyValueAsLONG(IB_KEY_HEIGHT);
			ULONG lWidth = lpLeftPictureKey->QuerySubKeyValueAsLONG(IB_KEY_WIDTH);
			mpLeftPicture->GetIterator()->SetSize((FLOAT)(lWidth), (FLOAT)(lHeight));

			//������ͼ
			wchar_t lpPicBackGround[BUF_SIZE] = {0};
			lpLeftPictureKey->QuerySubKeyValue(IB_KEY_BACKGROUND, lpPicBackGround, BUF_SIZE * sizeof(wchar_t));
			EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
				mpLeftPicture->GetIterator(), EACT_PICTUREFRAME_CHANGE_PIC, lpPicBackGround,
				(wcslen(lpPicBackGround) + 1) * sizeof(wchar_t), NULL, 0);
		}

		if (lpRightPictureKey)
		{
			ULONG lPosX = lpRightPictureKey->QuerySubKeyValueAsLONG(IB_KEY_X);
			ULONG lPosY = lpRightPictureKey->QuerySubKeyValueAsLONG(IB_KEY_Y);
			mpRightPicture->GetIterator()->SetPosition((FLOAT)(lPosX), (FLOAT)(lPosY));

			ULONG lHeight = lpRightPictureKey->QuerySubKeyValueAsLONG(IB_KEY_HEIGHT);
			ULONG lWidth = lpRightPictureKey->QuerySubKeyValueAsLONG(IB_KEY_WIDTH);
			mpRightPicture->GetIterator()->SetSize((FLOAT)(lWidth), (FLOAT)(lHeight));

			//������ͼ
			wchar_t lpPicBackGround[BUF_SIZE] = {0};
			lpRightPictureKey->QuerySubKeyValue(IB_KEY_BACKGROUND, lpPicBackGround, BUF_SIZE * sizeof(wchar_t));
			EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
				mpRightPicture->GetIterator(), EACT_PICTUREFRAME_CHANGE_PIC, lpPicBackGround,
				(wcslen(lpPicBackGround) + 1) * sizeof(wchar_t), NULL, 0);
		}

		//������
		wchar_t lpSelectFrame[BUF_SIZE] = {0};
		mpTemplete->QuerySubKeyValue(IB_KEY_SELECT_FRAME, lpSelectFrame, BUF_SIZE * sizeof(wchar_t));
		mpBitmapSelectOrOver = EinkuiGetSystem()->GetAllocator()->LoadImageFile(lpSelectFrame);

		//�����ļ���ȡ�ؼ����
		wchar_t lpStyle[BUF_SIZE] = {0};
		mpTemplete->QuerySubKeyValue(IB_KEY_STYLE, lpStyle, BUF_SIZE * sizeof(wchar_t));
		if (wcscmp(lpStyle, IB_STYLE_UNCHECKABLE) == 0)
		{
			// UNCHECKABLE ���
			mnStyle = 0;
		}
		else
		{
			// CHECKABLE ���
			mnStyle = 1;
		}

		//�����ļ���ȡ��չ����
		wchar_t lpExpandable[BUF_SIZE] = {0};
		mpTemplete->QuerySubKeyValue(IB_KEY_EXPANDABLE, lpExpandable, BUF_SIZE * sizeof(wchar_t));
		if (wcscmp(lpExpandable, IB_STYLE_EXPANDABLE) == 0)
		{			
			//֧����չ
			mbExpandable = true;
			
			//��ȡ����ͼƬ·���б�
			ICfKey* lpExpandMenuItemListKey = mpTemplete->GetSubKey(IB_KEY_MENU_ITEM_LIST);
			ICfKey* lpItemKey = NULL;

			if (lpExpandMenuItemListKey)
			{
				lpItemKey = lpExpandMenuItemListKey->MoveToSubKey();	
				int nCount = 0;
				while (lpItemKey != NULL)	//ѭ����ȡ����ͼƬ·���б�
				{
					ExpandMenuItem MenuItem;
					lpItemKey->QuerySubKeyValue(IB_KEY_MENU_ITEM_ID, &(MenuItem.id), sizeof(int));
					lpItemKey->QuerySubKeyValue(IB_KEY_MENU_ITEM_PIC_PATH, MenuItem.pPicPath, sizeof(wchar_t) * MAX_PATH);
						mpVecExpandMenuItem.Insert(nCount++, MenuItem);

					lpItemKey = lpItemKey->MoveToNextKey();
				}
			}
		}
		else
		{
			// ��֧����չ
			mbExpandable = false;
		}

		//�����ļ���ȡ��Ϣ����
		ICfKey* lpMessageInfoKey = mpTemplete->GetSubKey(IB_KEY_MESSAGE_INFO);
		if (lpMessageInfoKey)
		{
			unsigned int		lnID = 0;
			wchar_t lpMessageType[BUF_SIZE] = {0};

			lpMessageInfoKey->QuerySubKeyValue(IB_KEY_MESSAGE_INFO_ID, &lnID, sizeof(unsigned int));
			lpMessageInfoKey->QuerySubKeyValue(IB_KEY_MESSAGE_INFO_MESSAGE_TYPE, lpMessageType, BUF_SIZE * sizeof(wchar_t));

			if (lnID != 0)
			{
				mMsgInfo.mnCtrlID = lnID;
			}

			if (wcscmp(lpMessageType, IB_KEY_MESSAGE_INFO_MESSAGE_TYPE_NONE) == 0)
			{
				mMsgInfo.mnMsgParaType	= TMPT_NONE;
				//mMsgInfo.mpMsgBuf		= NULL;
			}
			else if (wcscmp(lpMessageType, IB_KEY_MESSAGE_INFO_MESSAGE_TYPE_BOOL) == 0)
			{
				mMsgInfo.mnMsgParaType = TMPT_BOOL;
			}
			else if (wcscmp(lpMessageType, IB_KEY_MESSAGE_INFO_MESSAGE_TYPE_INT) == 0)
			{
				mMsgInfo.mnMsgParaType = TMPT_INT;
			}
			else if (wcscmp(lpMessageType, IB_KEY_MESSAGE_INFO_MESSAGE_TYPE_FLOAT) == 0)
			{
				mMsgInfo.mnMsgParaType = TMPT_FLOAT;
			}
			else if (wcscmp(lpMessageType, IB_KEY_MESSAGE_INFO_MESSAGE_TYPE_STRING) == 0)
			{
				mMsgInfo.mnMsgParaType = TMPT_STRING;
			}
			else
			{
				mMsgInfo.mnMsgParaType = TMPT_OTHERS;
			}
		}

		//�����ļ���ȡ��ɫ�������
		wchar_t lpColorFlag[BUF_SIZE] = {0};
		mpTemplete->QuerySubKeyValue(IB_KEY_COLOR_FLAG, lpColorFlag, BUF_SIZE * sizeof(wchar_t));
		if (wcscmp(lpColorFlag, IB_KEY_COLOR_FLAG_true) == 0)
		{
			mbHasColorFlag = true;
		}
		

		return true;
	}
	else
	{
		return false;
	}	
}


void CEvImageButton::SetChecked(bool nbChecked)
{
	mbChecked = nbChecked;

	if (true == nbChecked)
	{
		PostMessageToParent(EEVT_IMAGEBUTTON_CHECKED, CExMessage::DataInvalid);
	}
	else
	{
		PostMessageToParent(EEVT_IMAGEBUTTON_UNCHECKED, CExMessage::DataInvalid);
	}
}

bool CEvImageButton::SetValue()
{
	//if (NULL == mpUnificSetting) return false;

	if (false == mpIterator->IsEnable()) return false;

	//	��ȡֵ
	//eValueType ValueType;
	//wstring lwValue = mpUnificSetting->GetItemValue(mMsgInfo.mnCtrlID, &ValueType);
	wchar_t lwValue[] = L"1";
	if (1 == mnStyle)		// �� check ���
	{
		if (L"1" == lwValue)
		{
			mbChecked = true;
		}
		else if (L"0" == lwValue)
		{
			mbChecked = false;
		}
	}

	if (mbExpandable && mpIterExpandCtrl)
	{
		int lItemID = _wtoi(lwValue/*.c_str()*/);

		wchar_t* pPicPath = NULL;
		for (int i = 0; i < mpVecExpandMenuItem.Size(); ++i)
		{
			if (mpVecExpandMenuItem[i].id == lItemID)
			{
				pPicPath = mpVecExpandMenuItem[i].pPicPath;

				if (pPicPath && (*pPicPath != '\0'))
				{	
					//������ť��벿��ͼƬ
					if (mpLeftPicture)
					{
						EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
							mpLeftPicture->GetIterator(), EACT_PICTUREFRAME_CHANGE_PIC, pPicPath,
							(wcslen(pPicPath) + 1) * sizeof(wchar_t), NULL, 0);
					}
				}

				break;
			}
		}
	}

	return true;
}

bool CEvImageButton::SetImageButtonEnable(bool nbEnable)
{
	if (nbEnable)
	{
		if (mpIterator) mpIterator->ModifyStyles(EITR_STYLE_MOUSE, NULL);
		if (mpLeftPicture)
		{
			IEinkuiIterator* iter = mpLeftPicture->GetIterator();
			if (iter)	iter->SetAlpha(1.0f);
		}
	}
	else
	{
		if (mpIterator) mpIterator->ModifyStyles(NULL, EITR_STYLE_MOUSE);
		if (mpLeftPicture)
		{
			IEinkuiIterator* iter = mpLeftPicture->GetIterator();
			if (iter)	iter->SetAlpha(0.5f);
		}
	}

	return true;
}

//Ԫ�زο��ߴ緢���仯
ERESULT CEvImageButton::OnElementResized(D2D1_SIZE_F nNewSize)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if (mpLeftPicture)
			mpLeftPicture->GetIterator()->SetSize
			(mpIterator->GetSize().width - mpLeftPicture->GetIterator()->GetPositionX() * 2.0f * mfRadio,
			mpIterator->GetSize().height - mpLeftPicture->GetIterator()->GetPositionY() * 2.0f * mfRadio);

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

//���û�����
ERESULT CEvImageButton::OnElementEnable(bool nbIsEnable)
{
	SetImageButtonEnable(nbIsEnable);

	return ERROR_SUCCESS;
}

bool CEvImageButton::SetItemSelected(int nID)
{
	if (mbExpandable && mpIterExpandCtrl)
	{
		int lItemID = nID;

		wchar_t* pPicPath = NULL;
		for (int i = 0; i < mpVecExpandMenuItem.Size(); ++i)
		{
			if (mpVecExpandMenuItem[i].id == lItemID)
			{
				pPicPath = mpVecExpandMenuItem[i].pPicPath;

				if (pPicPath && (*pPicPath != '\0'))
				{	
					//������ť��벿��ͼƬ
					if (mpLeftPicture)
					{
						EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
							mpLeftPicture->GetIterator(), EACT_PICTUREFRAME_CHANGE_PIC, pPicPath,
							(wcslen(pPicPath) + 1) * sizeof(wchar_t), NULL, 0);
					}
				}

				break;
			}
		}
	}

	return true;
}