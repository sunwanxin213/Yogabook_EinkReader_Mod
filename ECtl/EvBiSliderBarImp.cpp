/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "stdafx.h"
#include "cmmBaseObj.h"
#include "Einkui.h"
#include "ElementImp.h"
#include "XCtl.h"
#include "EvBiSliderBarImp.h"


//using namespace D2D1;

DEFINE_BUILTIN_NAME(BiSliderBar)

CEvBiSliderBar::CEvBiSliderBar()
{
	mulMethod = ESPB_DRAWBMP_LINEAR;	//���ŷ�ʽ
//	mpBarButton = NULL;
	mbIsMouseFocus = false;	
	mbIsPressed = false;
	mnVertical = 0;
	mfLabelLength = 40.0f;
}
CEvBiSliderBar::~CEvBiSliderBar()
{
	//if(mpBarButton)
	//	mpBarButton->Release();
}

// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
ERESULT CEvBiSliderBar::OnElementDestroy()
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;
	do
	{
		CXuiElement::OnElementDestroy();	//���û���
		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

ULONG CEvBiSliderBar::InitOnCreate(
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

		//��������SliderButton/BAR picture /Bar left Picture
		ICfKey * lpKey = npTemplete->GetSubKey(SLIDER_BAR_PICTURE);			
		if(lpKey)
			mpBarPicture = EinkuiGetSystem()->GetAllocator()->CreateElement(mpIterator,lpKey);
		BREAK_ON_NULL(mpBarPicture);

		lpKey = npTemplete->GetSubKey(SLIDER_BAR_LEFT_SLIDERBUTTON);			
		if(lpKey)
			mpDragButtonLeft = EinkuiGetSystem()->GetAllocator()->CreateElement(mpIterator,lpKey);
		BREAK_ON_NULL(mpDragButtonLeft);

		lpKey = npTemplete->GetSubKey(SLIDER_BAR_RIGHT_SLIDERBUTTON);			
		if(lpKey)
			mpDragButtonRigth = EinkuiGetSystem()->GetAllocator()->CreateElement(mpIterator,lpKey);
		BREAK_ON_NULL(mpDragButtonRigth);

		lpKey = npTemplete->GetSubKey(BI_SLIDER_BAR_LEFT_PICTURE);			
		if(lpKey)
			mpLeftBarPicture = EinkuiGetSystem()->GetAllocator()->CreateElement(mpIterator,lpKey);

		lpKey = npTemplete->GetSubKey(SLIDER_BAR_RIGHT_PICTURE);			
		if(lpKey)
			mpRightBarPicture = EinkuiGetSystem()->GetAllocator()->CreateElement(mpIterator,lpKey);

		lpKey = npTemplete->GetSubKey(SLIDER_BAR_MID_SLIDERBUTTON);			
		if(lpKey)
			mpDragButtonMid = EinkuiGetSystem()->GetAllocator()->CreateElement(mpIterator,lpKey);

		lpKey = npTemplete->GetSubKey(SLIDER_BAR_MID_LABEL);			
		if(lpKey)
			mpDragButtonMidLable = EinkuiGetSystem()->GetAllocator()->CreateElement(mpIterator,lpKey);

		//װ��һЩ��Ҫ��������Դ
		LoadResource();

	

		leResult = ERESULT_SUCCESS;

	} while (false);
	return leResult;
}
//Ԫ��Enbale
ERESULT CEvBiSliderBar::OnElementEnable(bool nbIsEnable)
{
	if(mpDragButtonLeft) mpDragButtonLeft->SetEnable(nbIsEnable);
	if(mpDragButtonRigth) mpDragButtonRigth->SetEnable(nbIsEnable);	
	if(mpDragButtonMid) mpDragButtonMid->SetEnable(nbIsEnable);
//	if(mpDragButtonMid) mpDragButtonMid->SetEnable(nbIsEnable);

	//if (nbIsEnable)
	//{
	//	if (mpIterator) mpIterator->ModifyStyles(EITR_STYLE_MOUSE, NULL);
	//}
	//else
	//{
	//	if (mpIterator) mpIterator->ModifyStyles(NULL, EITR_STYLE_MOUSE);
	//}

	return ERESULT_SUCCESS;
}
//װ��������Դ
ERESULT CEvBiSliderBar::LoadResource()
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	ICfKey* lpValue = NULL;
	do 
	{	
		//װ��LeftOrBack��ͼƬ 

		//װ��Min Max

		//����Bar��Rect
		leResult = ERESULT_SUCCESS;
	} while (false);

	return leResult;
}

// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
ERESULT CEvBiSliderBar::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������

	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;
	do 
	{
		BREAK_ON_NULL(npMsg);
		switch (npMsg->GetMessageID())
		{
		case EACT_BISLIDERBAR_SET_MIDLABEL_LEGTH:
			{
				if(npMsg->GetInputDataSize() != sizeof(FLOAT))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				FLOAT* lpValue = (FLOAT*)npMsg->GetInputData();
				mfLabelLength = * lpValue;
				break;
			}
		
		case EACT_BISLIDERBAR_SET_MIDLABEL_VALUE:
			{
				wchar_t* lpswText = (wchar_t*)npMsg->GetInputData();
				CExMessage::SendMessageWithText(mpDragButtonMidLable,mpIterator,EACT_LABEL_SET_TEXT,lpswText,NULL,0);
				break;
			}
		case EACT_SLIDERBUTTON_DRAG_START:
			{
				//�������ҵĻ�������
				if(mpDragButtonLeft && mpDragButtonRigth && mpDragButtonMid)
				{
					FLOAT lfPositionLeft,lfPositionRight,lfPositionMid ;
					lfPositionLeft = mpDragButtonLeft->GetPositionX()+mpDragButtonLeft->GetSizeX()/2 - mBarRect.left ;
					lfPositionRight = mpDragButtonRigth->GetPositionX()+mpDragButtonRigth->GetSizeX()/2 - mBarRect.left ;
					lfPositionMid = mpDragButtonMid->GetPositionX() - mBarRect.left ;
					D2D1_RECT_F lLeftSliderRect;
					lLeftSliderRect.left = mSliderRect.left;
					lLeftSliderRect.top = mSliderRect.top;
					lLeftSliderRect.bottom = mSliderRect.bottom;
					lLeftSliderRect.right = lfPositionRight+mpDragButtonLeft->GetSizeX()/2;
					CExMessage::PostMessage(mpDragButtonLeft,mpIterator,EACT_SLIDERBUTTON_SET_SLIDERRECT,lLeftSliderRect);

					D2D1_RECT_F lRightSliderRect;
					lRightSliderRect.left = lfPositionLeft-mpDragButtonRigth->GetSizeX()/2;
					lRightSliderRect.top = mSliderRect.top;
					lRightSliderRect.bottom = mSliderRect.bottom;
					lRightSliderRect.right = mSliderRect.right;
					CExMessage::PostMessage(mpDragButtonRigth,mpIterator,EACT_SLIDERBUTTON_SET_SLIDERRECT,lRightSliderRect);
		
					//����label��λ��
					if(mpDragButtonMidLable && !mnVertical)
					{
						if(mpDragButtonMid->GetPositionX() < (mBarRect.right-mBarRect.left) / 2)
						{
							mpDragButtonMidLable->SetPosition(mpDragButtonMid->GetPositionX() + 5,mpDragButtonMidLable->GetPositionY());
						}
						else
						{
							mpDragButtonMidLable->SetPosition(mpDragButtonMid->GetPositionX()-mfLabelLength- 5 ,mpDragButtonMidLable->GetPositionY());
						}
					}
				}
				CExMessage::PostMessage(mpIterator->GetParent(),mpIterator, EACT_BISLIDERBAR_DRAG_START, 0);
				break;
			}
		case EACT_SLIDERBUTTON_DRAG_END:
			{
				CExMessage::PostMessage(mpIterator->GetParent(),mpIterator, EACT_BISLIDERBAR_DRAG_END, 0);
				break;
			}
		case EACT_SLIDERBUTTON_DRAGING:
			{
				//����Position,�������ڷ���λ��Ϣ
				//��λDragButton�����ĵ�λ��
				if(mpDragButtonLeft && mpDragButtonRigth && mpDragButtonMid)
				{
					FLOAT lfPositionLeft,lfPositionRight,lfPositionMid ;
					if(mnVertical)
					{
						lfPositionLeft = mpDragButtonLeft->GetPositionY() - mBarRect.top ;	
						lfPositionRight = mpDragButtonRigth->GetPositionY() - mBarRect.top ;	
						lfPositionMid = mpDragButtonMid->GetPositionY() - mBarRect.top ;	
					}
					else
					{
						lfPositionLeft = mpDragButtonLeft->GetPositionX()+mpDragButtonLeft->GetSizeX()/2 - mBarRect.left ;
						lfPositionRight = mpDragButtonRigth->GetPositionX()+mpDragButtonRigth->GetSizeX()/2 - mBarRect.left ;
						lfPositionMid = mpDragButtonMid->GetPositionX() - mBarRect.left ;

					}

					//�����LeftorUP picture ��Ӧ�÷Ŵ����С��ͼ��
					if(mpLeftBarPicture)
					{
						if(mnVertical)
							mpLeftBarPicture->SetSize(mpLeftBarPicture->GetSizeX(),lfPositionLeft);
						else
						{
							mpLeftBarPicture->SetSize(lfPositionLeft,mpLeftBarPicture->GetSizeY());
							mpLeftBarPicture->SetPosition(mBarRect.left,mpLeftBarPicture->GetPositionY());
						}
					}
					lfPositionLeft *= mfDestPixelPerScrollPixLeft;


	                //�����LeftorUP picture ��Ӧ�÷Ŵ����С��ͼ��
					if(mpRightBarPicture)
					{
						if(mnVertical)
							mpRightBarPicture->SetSize(mpRightBarPicture->GetSizeX(),lfPositionRight);
						else
						{
							mpRightBarPicture->SetSize(mBarRect.right- lfPositionRight,mpRightBarPicture->GetSizeY());
							mpRightBarPicture->SetPosition(lfPositionRight+mBarRect.left,mpRightBarPicture->GetPositionY());
						}
					}
					lfPositionRight *= mfDestPixelPerScrollPixRight;

					//����label��λ��
					if(mpDragButtonMidLable && !mnVertical)
					{
						if(mpDragButtonMid->GetPositionX() < (mBarRect.right-mBarRect.left) / 2)
						{
							mpDragButtonMidLable->SetPosition(mpDragButtonMid->GetPositionX() + 5,mpDragButtonMidLable->GetPositionY());
						}
						else
						{
							mpDragButtonMidLable->SetPosition(mpDragButtonMid->GetPositionX()-mfLabelLength -5 ,mpDragButtonMidLable->GetPositionY());
						}
					}
					lfPositionMid *= mfDestPixelPerScrollPixMid;
					_sBiSliderBarStruct lsPos;
					lsPos.mfLeftPos = lfPositionLeft;
					lsPos.mfRightPos = lfPositionRight;
					lsPos.mfMidPos = lfPositionMid;
					//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpIterator->GetParent(),EACT_BISLIDERBAR_THUMB_POSITION,&lsPos,sizeof(_sBiSliderBarStruct));
					//CExMessage::PostMessage(mpIterator->GetParent(),mpIterator,EACT_BISLIDERBAR_THUMB_POSITION,lsPos);
					CExMessage::SendMessageW(mpIterator->GetParent(),mpIterator,EACT_BISLIDERBAR_THUMB_POSITION,lsPos);
				}
				break;
			}
		case EACT_BISLIDERBAR_SET_RANGE:
			{
				if(npMsg->GetInputDataSize() != sizeof(_sBiSliderBarStruct))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				_sBiSliderBarStruct* lpValue = (_sBiSliderBarStruct*)npMsg->GetInputData();
				BREAK_ON_NULL(lpValue);
				//FLOAT lfy = (*lpValue)/mfDestPixelPerScrollPix;
				SetDeltaSizeLeft(lpValue->mfLeftPos);
				SetDeltaSizeRigth(lpValue->mfRightPos);
				SetDeltaSizeMid(lpValue->mfMidPos);
				break;
			}
		case EACT_BISLIDERBAR_SET_POS:
			{
				if(npMsg->GetInputDataSize() != sizeof(_sBiSliderBarStruct))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				_sBiSliderBarStruct* lpValue = (_sBiSliderBarStruct*)npMsg->GetInputData();
				BREAK_ON_NULL(lpValue);

				FLOAT lfPositionLeft;
				FLOAT lfDeltaLeft = lpValue->mfLeftPos / mfDestPixelPerScrollPixLeft;
				if(mnVertical)
				{
					lfPositionLeft = mBarRect.top + lfDeltaLeft;	
					mpDragButtonLeft->SetPosition(mpDragButtonLeft->GetPositionX(),lfPositionLeft - mpDragButtonLeft->GetSizeY()/2);
				}
				else
				{
					lfPositionLeft =  mBarRect.left + lfDeltaLeft ;
					mpDragButtonLeft->SetPosition(lfPositionLeft-mpDragButtonLeft->GetSizeX()/2,mpDragButtonLeft->GetPositionY());
				}
				//�����LeftorUP picture ��Ӧ�÷Ŵ����С��ͼ��
				if(mpLeftBarPicture)
				{
					if(mnVertical)
						mpLeftBarPicture->SetSize(mpLeftBarPicture->GetSizeX(),lfPositionLeft - mBarRect.top);
					else
						mpLeftBarPicture->SetSize(lfPositionLeft - mBarRect.left,mpLeftBarPicture->GetSizeY());
				}
			
				FLOAT lfPositionRight;
				FLOAT lfDeltaRight = lpValue->mfRightPos / mfDestPixelPerScrollPixRight;
				if(mnVertical)
				{
					lfPositionRight = mBarRect.top + lfDeltaRight;	
					mpDragButtonRigth->SetPosition(mpDragButtonRigth->GetPositionX(),lfDeltaRight - mpDragButtonRigth->GetSizeY()/2);
				}
				else
				{
					lfPositionRight =  mBarRect.left + lfDeltaRight ;
					mpDragButtonRigth->SetPosition(lfDeltaRight-mpDragButtonLeft->GetSizeX()/2,mpDragButtonRigth->GetPositionY());
				}
				//�����LeftorUP picture ��Ӧ�÷Ŵ����С��ͼ��
				if(mpRightBarPicture)
				{

					if(mnVertical)
						mpRightBarPicture->SetSize(mpDragButtonRigth->GetSizeX(),lfPositionRight - mBarRect.top);
					else
					{
						mpRightBarPicture->SetSize(mBarRect.right - lfPositionRight,mpRightBarPicture->GetSizeY());
					    mpRightBarPicture->SetPosition(lfPositionRight+mBarRect.left,mpRightBarPicture->GetPositionY());
					}
				}

				FLOAT lfPositionMid;
				FLOAT lfDeltaMid = lpValue->mfMidPos / mfDestPixelPerScrollPixMid;
				if(mnVertical)
				{
					lfPositionMid = mBarRect.top + lfDeltaMid;	
					mpDragButtonMid->SetPosition(mpDragButtonMid->GetPositionX(),lfDeltaRight);
				}
				else
				{
					lfPositionMid =  mBarRect.left + lfDeltaMid ;
					mpDragButtonMid->SetPosition(lfDeltaMid,mpDragButtonMid->GetPositionY());
				}

				//����label��λ��
				if(mpDragButtonMidLable && !mnVertical)
				{
					if(mpDragButtonMid->GetPositionX() < (mBarRect.right-mBarRect.left) / 2)
					{
						mpDragButtonMidLable->SetPosition(mpDragButtonMid->GetPositionX() + 5,mpDragButtonMidLable->GetPositionY());
					}
					else
					{
						mpDragButtonMidLable->SetPosition(mpDragButtonMid->GetPositionX()-mfLabelLength -5 ,mpDragButtonMidLable->GetPositionY());
					}
				}
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

	} while (false);
	
	return luResult;
}

//���ù����ķ�Χ
bool CEvBiSliderBar:: SetDeltaSizeLeft(FLOAT nfSize)
{
	mfMaxLeft = nfSize;

	//�ȵõ����Ŀɹ�������
	FLOAT mfMaxScrollPixelLeft = 0.0;
	if(mnVertical)
		mfMaxScrollPixelLeft = mBarRect.bottom - mBarRect.top  ;
	else
		mfMaxScrollPixelLeft = mBarRect.right - mBarRect.left ;

	//����Ŀ�����غ͹������صĶ�Ӧ��ϵ
	mfDestPixelPerScrollPixLeft= mfMaxLeft / mfMaxScrollPixelLeft;

	return true;
}


//���ù����ķ�Χ
bool CEvBiSliderBar:: SetDeltaSizeRigth(FLOAT nfSize)
{
	mfMaxRight = nfSize;

	//�ȵõ����Ŀɹ�������
	FLOAT mfMaxScrollPixelRight= 0.0;
	if(mnVertical)
		mfMaxScrollPixelRight = mBarRect.bottom - mBarRect.top  ;
	else
		mfMaxScrollPixelRight = mBarRect.right - mBarRect.left ;

	//����Ŀ�����غ͹������صĶ�Ӧ��ϵ
	mfDestPixelPerScrollPixRight = mfMaxRight / mfMaxScrollPixelRight;

	return true;
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEvBiSliderBar::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;		

		if(mpDragButtonLeft && mpDragButtonRigth)
		{
			LONG lnStyle = ES_SLIDER_BUTTON_STYLE_HOR;
			if(mnVertical)
				lnStyle = ES_SLIDER_BUTTON_STYLE_VER;
			//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpDragButton,EACT_SLIDERBUTTON_SET_STYLE,&lnStyle,sizeof(LONG));
			CExMessage::PostMessage(mpDragButtonLeft,mpIterator,EACT_SLIDERBUTTON_SET_STYLE,lnStyle);
			CExMessage::PostMessage(mpDragButtonRigth,mpIterator,EACT_SLIDERBUTTON_SET_STYLE,lnStyle);
			if(mpDragButtonMid)
				CExMessage::PostMessage(mpDragButtonMid,mpIterator,EACT_SLIDERBUTTON_SET_STYLE,lnStyle);
			//��ȡBarRect
			mBarRect.left = mpBarPicture->GetPositionX() ;
			mBarRect.top = mpBarPicture->GetPositionY();
			mBarRect.right = mBarRect.left + mpBarPicture->GetSizeX();
			mBarRect.bottom =mBarRect.top + mpBarPicture->GetSizeY();

			//���ÿ��Ի����ķ�Χ
			BREAK_ON_NULL(mpBarPicture);
			mSliderRect.left = mpBarPicture->GetPositionX() -  mpDragButtonLeft->GetSizeX()/2 ;
			mSliderRect.top = mpBarPicture->GetPositionY();
			mSliderRect.right = mpBarPicture->GetPositionX() + mpBarPicture->GetSizeX() + mpDragButtonLeft->GetSizeX()/2;
			mSliderRect.bottom =mSliderRect.top + mpBarPicture->GetSizeY();
			CExMessage::PostMessage(mpDragButtonRigth,mpIterator,EACT_SLIDERBUTTON_SET_SLIDERRECT,mSliderRect);
			CExMessage::PostMessage(mpDragButtonLeft,mpIterator,EACT_SLIDERBUTTON_SET_SLIDERRECT,mSliderRect);
			if(mpDragButtonMid)
				CExMessage::PostMessage(mpDragButtonMid,mpIterator,EACT_SLIDERBUTTON_SET_SLIDERRECT,mBarRect);
			//�����LeftOrUP Picture���С��Ϊ0
			if(mpLeftBarPicture)
			{
				if(mnVertical)
					mpLeftBarPicture->SetSize(mpLeftBarPicture->GetSizeX(),0.0);
				else
					mpLeftBarPicture->SetSize(0.0,mpLeftBarPicture->GetSizeY());
			}	
			if(mpRightBarPicture)
			{
				if(mnVertical)
					mpRightBarPicture->SetSize(mpRightBarPicture->GetSizeX(),0.0);
				else
					mpRightBarPicture->SetSize(0.0,mpRightBarPicture->GetSizeY());
			}	
		}
		
		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}


// ��������
ERESULT CEvBiSliderBar::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	ERESULT luResult = ERESULT_SUCCESS;

	do 
	{	
		if(mpIterator->IsVisible() == false)
			break;

		//����ǻ�������
		if(rPoint.x < 0.0f ||
			(UINT)rPoint.x >= mpIterator->GetSizeX() ||
			rPoint.y < 0.0f || 
			(UINT)rPoint.y >= mpIterator->GetSizeY())
			break;
	
		luResult = ERESULT_MOUSE_OWNERSHIP;

	} while (false);

	return luResult;
}
bool CEvBiSliderBar::SetRangMid(FLOAT nMin,FLOAT nMax)
{
	mfMinMid = nMin;
	mfMaxMid = nMax;
	return true;
}
bool CEvBiSliderBar::SetPosMid(FLOAT nPos)
{
	mfPosMid = nPos;
	return true;
}
bool CEvBiSliderBar::SetDeltaSizeMid(FLOAT nfSize)
{
	mfMaxMid = nfSize;

	//�ȵõ����Ŀɹ�������
	FLOAT mfMaxScrollPixelMid= 0.0;
	if(mnVertical)
		mfMaxScrollPixelMid = mBarRect.bottom - mBarRect.top  ;
	else
		mfMaxScrollPixelMid = mBarRect.right - mBarRect.left ;

	//����Ŀ�����غ͹������صĶ�Ӧ��ϵ
	mfDestPixelPerScrollPixMid = mfMaxMid / mfMaxScrollPixelMid;

	return true;
}
bool CEvBiSliderBar::SetRangeLeft(FLOAT nMin,FLOAT nMax)
{
	mfMinLeft = nMin;
	mfMaxLeft = nMax;
	return true;
}
bool CEvBiSliderBar::SetRangeRight(FLOAT nMin,FLOAT nMax)
{
	mfMinRigth = nMin;
	mfMaxRight = nMax;
	return true;
}

bool CEvBiSliderBar::SetPosLeft(FLOAT nPos)
{
	mfPosLeft = nPos;
	return false;
}

bool CEvBiSliderBar::SetPosRight(FLOAT nPos)
{
	mfPosRight = nPos;
	return false;
}
//��갴��
ERESULT CEvBiSliderBar::OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npInfo);
		if(mpIterator->IsEnable() == false)
			break;	//����ǽ���״̬���Ͳ���������

		if(MOUSE_LB(npInfo->ActKey) == false)  //��������������Ͳ�����
			break;

		//if (npInfo->Presssed != false)
		//{
		//	//��갴�£����������mBarRect ����
		//	if(npInfo->Position.x < mBarRect.left ||
		//			npInfo->Position.x > mBarRect.right ||
		//			npInfo->Position.y < mBarRect.top ||
		//			npInfo->Position.y > mBarRect.bottom)
		//			break;
		//	FLOAT lf = 0.0;
		//	if(mnVertical)
		//	{
		//		lf = (npInfo->Position.y - mBarRect.top) * mfDestPixelPerScrollPix;
		//		//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpIterator,EACT_BiSliderBar_SET_POSITION,&lf,sizeof(FLOAT));
		//		CExMessage::PostMessage(mpIterator,mpIterator,EACT_SLIDERBAR_SET_POSITION,lf);
		//	}
		//	else
		//	{
		//		lf = (npInfo->Position.x - mBarRect.left) * mfDestPixelPerScrollPix;
		//		//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpIterator,EACT_BiSliderBar_SET_POSITION,&lf,sizeof(FLOAT));
		//		CExMessage::PostMessage(mpIterator,mpIterator,EACT_SLIDERBAR_SET_POSITION,lf);
		//	}

		//	//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpIterator->GetParent(),EACT_BiSliderBar_THUMB_POSITION,&lf,sizeof(FLOAT));
		//	CExMessage::PostMessage(mpIterator->GetParent(),mpIterator,EACT_SLIDERBAR_THUMB_POSITION,lf);
		//}



		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

//������Ϣ
ERESULT CEvBiSliderBar::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npPaintBoard);

		if(mpBgBitmap != NULL)
			npPaintBoard->DrawBitmap(D2D1::RectF(0,0,mpIterator->GetSizeX(),mpIterator->GetSizeY()),
			mpBgBitmap,
			ESPB_DRAWBMP_EXTEND);

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}