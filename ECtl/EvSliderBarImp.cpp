/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "stdafx.h"
#include "cmmBaseObj.h"
#include "Einkui.h"
#include "ElementImp.h"
#include "XCtl.h"
#include "EvSliderBarImp.h"


//using namespace D2D1;

DEFINE_BUILTIN_NAME(SliderBar)

CEvSliderBar::CEvSliderBar()
{
	mulMethod = ESPB_DRAWBMP_LINEAR;	//���ŷ�ʽ
//	mpBarButton = NULL;
	mbIsMouseFocus = false;	
	mbIsPressed = false;
	mnVertical = 0;

}
CEvSliderBar::~CEvSliderBar()
{
	//if(mpBarButton)
	//	mpBarButton->Release();
}

// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
ERESULT CEvSliderBar::OnElementDestroy()
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;
	do
	{
		CXuiElement::OnElementDestroy();	//���û���
		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

ULONG CEvSliderBar::InitOnCreate(
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

		lpKey = npTemplete->GetSubKey(SLIDER_BAR_LEFT_PICTURE);			
		if(lpKey)
			mpLeftBarPicture = EinkuiGetSystem()->GetAllocator()->CreateElement(mpIterator,lpKey);

		lpKey = npTemplete->GetSubKey(SLIDER_BAR_SLIDERBUTTON);			
		if(lpKey)
			mpDragButton = EinkuiGetSystem()->GetAllocator()->CreateElement(mpIterator,lpKey);
		BREAK_ON_NULL(mpDragButton);
		//װ��һЩ��Ҫ��������Դ
		LoadResource();

	

		leResult = ERESULT_SUCCESS;

	} while (false);
	return leResult;
}
//Ԫ��Enbale
ERESULT CEvSliderBar::OnElementEnable(bool nbIsEnable)
{
	if(mpDragButton) mpDragButton->SetEnable(nbIsEnable);
	
	return ERESULT_SUCCESS;
}
//װ��������Դ
ERESULT CEvSliderBar::LoadResource()
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	ICfKey* lpValue = NULL;
	do 
	{	
		//װ��LeftOrBack��ͼƬ 

		//װ��Min Max

		//����Bar��Rect
		if (mpIterator->GetSizeX() > 0)
		{
			mpBarPicture->SetSize(mpIterator->GetSizeX(), mpBarPicture->GetSizeY());
			mpLeftBarPicture->SetSize(mpIterator->GetSizeX(), mpLeftBarPicture->GetSizeY());
		}

		// ��ȡ�����Ч���� add by colin
		mdEffectiveRect.left = (FLOAT)this->mpTemplete->QuerySubKeyValueAsLONG(TF_ID_SLIDERBAR_EFFECTIVERECT_LEFT, 0);
		mdEffectiveRect.top = (FLOAT)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_SLIDERBAR_EFFECTIVERECT_TOP, 0);
		mdEffectiveRect.right = (FLOAT)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_SLIDERBAR_EFFECTIVERECT_RIGHT, mpBarPicture->GetSizeX());
		mdEffectiveRect.bottom = (FLOAT)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_SLIDERBAR_EFFECTIVERECT_BOTTOM, mpBarPicture->GetSizeY());

		if (mpDragButton)
		{
			LONG lnStyle = ES_SLIDER_BUTTON_STYLE_HOR;
			if (mnVertical)
				lnStyle = ES_SLIDER_BUTTON_STYLE_VER;
			//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpDragButton,EACT_SLIDERBUTTON_SET_STYLE,&lnStyle,sizeof(LONG));
			CExMessage::PostMessage(mpDragButton, mpIterator, EACT_SLIDERBUTTON_SET_STYLE, lnStyle);
			//��ȡBarRect
			mBarRect.left = mpBarPicture->GetPositionX();
			mBarRect.top = mpBarPicture->GetPositionY();
			mBarRect.right = mBarRect.left + mpBarPicture->GetSizeX();
			mBarRect.bottom = mBarRect.top + mpBarPicture->GetSizeY();

			//���ÿ��Ի����ķ�Χ
			BREAK_ON_NULL(mpBarPicture);
			mSliderRect.left = mpBarPicture->GetPositionX();
			mSliderRect.top = mpBarPicture->GetPositionY();
			mSliderRect.right = mSliderRect.left + mpBarPicture->GetSizeX();
			mSliderRect.bottom = mSliderRect.top + mpBarPicture->GetSizeY();

			////�����1/2 dragButton�Ĵ���
			//if(mnVertical)
			//{		
			//	mSliderRect.top -= mpDragButton->GetSizeY() /2 ;
			//	mSliderRect.bottom += mpDragButton->GetSizeY() / 2;
			//}
			//else
			//{	
			//	mSliderRect.left -= mpDragButton->GetSizeX() /2 ;
			//	mSliderRect.right += mpDragButton->GetSizeX() /2 ;
			//}
			//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpDragButton,EACT_SLIDERBUTTON_SET_SLIDERRECT,&mSliderRect,sizeof(D2D1_RECT_F));
			CExMessage::PostMessage(mpDragButton, mpIterator, EACT_SLIDERBUTTON_SET_SLIDERRECT, mSliderRect);
		}
		//�����LeftOrUP Picture���С��Ϊ0
		if (mpLeftBarPicture)
		{
			if (mnVertical)
				mpLeftBarPicture->SetSize(mpLeftBarPicture->GetSizeX(), 0.0);
			else
				mpLeftBarPicture->SetSize(0.0, mpLeftBarPicture->GetSizeY());
		}

		leResult = ERESULT_SUCCESS;
	} while (false);

	return leResult;
}

// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
ERESULT CEvSliderBar::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������

	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;
	do 
	{
		BREAK_ON_NULL(npMsg);
		switch (npMsg->GetMessageID())
		{
		case EACT_SLIDERBUTTON_DRAG_START:
			{
				FLOAT lfPosition;
				if(mnVertical)
					lfPosition = mpDragButton->GetPositionY() - mBarRect.top ;		
				else
					lfPosition = mpDragButton->GetPositionX() - mBarRect.left ;
				lfPosition *= mfDestPixelPerScrollPix;

				CExMessage::PostMessage(mpIterator->GetParent(),mpIterator, EACT_SLIDERBAR_DRAG_START, lfPosition);

				break;
			}
		case EACT_SLIDERBUTTON_DRAG_END:
			{
				// ����֪������ק������Ӧ�ý���ʱ��λ��Ҳ���͹�ȥ  add by colin
				FLOAT lfPosition;
				if(mnVertical)
					lfPosition = mpDragButton->GetPositionY() - mBarRect.top ;		
				else
					lfPosition = mpDragButton->GetPositionX() - mBarRect.left ;
				lfPosition *= mfDestPixelPerScrollPix;

				CExMessage::PostMessage(mpIterator->GetParent(),mpIterator, EACT_SLIDERBAR_DRAG_END, lfPosition);
				break;
			}
		case EACT_SLIDERBUTTON_DRAGING:
			{
				//����Position,�������ڷ���λ��Ϣ
				//��λDragButton�����ĵ�λ��
				if(mpDragButton)
				{
					FLOAT lfPosition ;
					if(mnVertical)
						lfPosition = mpDragButton->GetPositionY() - mBarRect.top ;		
					else
						lfPosition = mpDragButton->GetPositionX() - mBarRect.left ;

					//�����LeftorUP picture ��Ӧ�÷Ŵ����С��ͼ��
					if(mpLeftBarPicture)
					{
						if(mnVertical)
							mpLeftBarPicture->SetSize(mpLeftBarPicture->GetSizeX(),lfPosition);
						else
							mpLeftBarPicture->SetSize(lfPosition,mpLeftBarPicture->GetSizeY());
					}
					lfPosition *= mfDestPixelPerScrollPix;

					CExMessage::PostMessage(mpIterator->GetParent(),mpIterator, EACT_SLIDERBAR_DRAGING, lfPosition);

				}
				break;
			}
		case EACT_SLIDERBAR_SET_RANGE:
			{
				if(npMsg->GetInputDataSize() != sizeof(FLOAT))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				FLOAT* lpValue = (FLOAT*)npMsg->GetInputData();
				//FLOAT lfy = (*lpValue)/mfDestPixelPerScrollPix;
				SetDeltaSize(*lpValue);
				break;
			}
		case EACT_SLIDERBAR_SET_POSITION:
			{
				if(npMsg->GetInputDataSize() != sizeof(FLOAT))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				FLOAT* lpValue = (FLOAT*)npMsg->GetInputData();

				FLOAT lfPosition ;
				FLOAT lfDelta = (*lpValue) / mfDestPixelPerScrollPix;
				if(mnVertical)
				{
					lfPosition = mBarRect.top + lfDelta;	
					mpDragButton->SetPosition(mpDragButton->GetPositionX(),lfPosition);
				}
				else
				{
					lfPosition =  mBarRect.left + lfDelta ;
					mpDragButton->SetPosition(lfPosition ,mpDragButton->GetPositionY());
				}

				//�����LeftorUP picture ��Ӧ�÷Ŵ����С��ͼ��
				if(mpLeftBarPicture)
				{
					if(mnVertical)
						mpLeftBarPicture->SetSize(mpLeftBarPicture->GetSizeX(),lfPosition - mBarRect.top);
					else
						mpLeftBarPicture->SetSize(lfPosition - mBarRect.left,mpLeftBarPicture->GetSizeY());
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
bool CEvSliderBar:: SetDeltaSize(FLOAT nfSize)
{
	mfMax = nfSize;

	//�ȵõ����Ŀɹ�������
	FLOAT mfMaxScrollPixel = 0.0;
	if(mnVertical)
		mfMaxScrollPixel = mBarRect.bottom - mBarRect.top - mpDragButton->GetSizeY() ;
	else
		mfMaxScrollPixel = mBarRect.right - mBarRect.left - mpDragButton->GetSizeX();

	//����Ŀ�����غ͹������صĶ�Ӧ��ϵ
	mfDestPixelPerScrollPix = mfMax / mfMaxScrollPixel;

	


	return true;
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEvSliderBar::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;		

		
		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}


// ��������
ERESULT CEvSliderBar::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	ERESULT luResult = ERESULT_SUCCESS;

	do 
	{	
		if(mpIterator->IsVisible() == false)
			break;

		////����ǻ�������
		//if(rPoint.x < 0.0f ||
		//	(UINT)rPoint.x >= mpIterator->GetSizeX() ||
		//	rPoint.y < 0.0f || 
		//	(UINT)rPoint.y >= mpIterator->GetSizeY())
		//	break;

		// ������Ч����
		if(rPoint.x < mdEffectiveRect.left ||
			rPoint.x > mdEffectiveRect.right ||
			rPoint.y < mdEffectiveRect.top ||
			rPoint.y > mdEffectiveRect.bottom)
			break;
	
		luResult = ERESULT_MOUSE_OWNERSHIP;

	} while (false);

	return luResult;
}

bool CEvSliderBar::SetRange(FLOAT nMin,FLOAT nMax)
{
	mfMin = nMin;
	mfMax = nMin;
	return true;
}
bool CEvSliderBar::SetPos(FLOAT nPos)
{
	mfPos = nPos;
	return false;
}


//��갴��
ERESULT CEvSliderBar::OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npInfo);
		if(mpIterator->IsEnable() == false)
			break;	//����ǽ���״̬���Ͳ���������

		if(MOUSE_LB(npInfo->ActKey) == false)  //��������������Ͳ�����
			break;

		if (npInfo->Presssed != false)
		{
			//��갴�£����������mBarRect ����
			if(npInfo->Position.x < mBarRect.left ||
					npInfo->Position.x > mBarRect.right ||
					npInfo->Position.y < mBarRect.top ||
					npInfo->Position.y > mBarRect.bottom)
					break;
			FLOAT lf = 0.0;
			if(mnVertical)
			{
				lf = (npInfo->Position.y - mBarRect.top) * mfDestPixelPerScrollPix;
				//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpIterator,EACT_SLIDERBAR_SET_POSITION,&lf,sizeof(FLOAT));
				CExMessage::PostMessage(mpIterator,mpIterator,EACT_SLIDERBAR_SET_POSITION,lf);
			}
			else
			{
				lf = (npInfo->Position.x - mBarRect.left) * mfDestPixelPerScrollPix;
				//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpIterator,EACT_SLIDERBAR_SET_POSITION,&lf,sizeof(FLOAT));
				CExMessage::PostMessage(mpIterator,mpIterator,EACT_SLIDERBAR_SET_POSITION,lf);
			}

			//EinkuiGetSystem()->GetElementManager()->SimplePostMessage(mpIterator->GetParent(),EACT_SLIDERBAR_THUMB_POSITION,&lf,sizeof(FLOAT));
			CExMessage::PostMessage(mpIterator->GetParent(),mpIterator,EACT_SLIDERBAR_THUMB_CLICK,lf);
		}



		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

//������Ϣ
ERESULT CEvSliderBar::OnPaint(IEinkuiPaintBoard* npPaintBoard)
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

//Ԫ�زο��ߴ緢���仯
ERESULT CEvSliderBar::OnElementResized(D2D1_SIZE_F nNewSize)
{
	return LoadResource();
}