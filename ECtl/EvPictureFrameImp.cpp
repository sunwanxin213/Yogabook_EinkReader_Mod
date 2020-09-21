/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "stdafx.h"
#include "cmmBaseObj.h"
#include "Einkui.h"

#include "ElementImp.h"
#include "XCtl.h"
#include "EvPictureFrameImp.h"


//using namespace D2D1;

DEFINE_BUILTIN_NAME(PictureFrame)

CEvPictureFrame::CEvPictureFrame()
{
	mlMaxFrame = mlCurrentIndex = 0;
	mdFrameSize.width = mdFrameSize.height = 0.0f;
	mulMethod = ESPB_DRAWBMP_LINEAR;	//���ŷ�ʽ
	mfBeginPos = 0.0f;
	mbIsAutoPlay = false;
	mbIsPlayLoop = false;
	mlPlayTimerElapse = 0;
}
CEvPictureFrame::~CEvPictureFrame()
{

}

// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
ERESULT CEvPictureFrame::OnElementDestroy()
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		CXuiElement::OnElementDestroy();	//���û���

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

ULONG CEvPictureFrame::InitOnCreate(
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

		//װ��һЩ��Ҫ��������Դ
		LoadResource();

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}


//װ��������Դ
ERESULT CEvPictureFrame::LoadResource()
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	ICfKey* lpValue = NULL;

	do 
	{
		//��ȡ֡��Ϣ
		mlMaxFrame = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_PIC_FRAME_COUNT,1);
		mbIsAutoPlay = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_PIC_AUTO_PLAY, 0)==0?false:true;
		mbIsPlayLoop = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_PIC_PLAY_LOOP, 0) == 0 ? false : true;
		mlPlayTimerElapse = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_PIC_PLAY_TIMER_ELAPSE, 0);

		BREAK_ON_FALSE(Resize());

		if (mbIsAutoPlay != false && mlMaxFrame > 1)
		{
			//˵��Ҫ�Զ����Ŷ���
			mpIterator->SetTimer(TF_ID_PIC_TIMER_ID_PLAY, MAXULONG64 , mlPlayTimerElapse, NULL);
		}

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

//���¼���֡��С,nbResizeΪ����������ô�С
bool CEvPictureFrame::Resize(bool nbResize)
{
	bool lbret = false;

	do 
	{
		if (mlMaxFrame <= 0 || mpBgBitmap == NULL)
			break;

		//����ÿ֡��С
		UINT luiWidth = mpBgBitmap->GetWidth();
		UINT luiHeight = mpBgBitmap->GetHeight();
		mlCurrentIndex = 0;

		mdFrameSize.width = float(luiWidth / mlMaxFrame);
		mdFrameSize.height = (float)luiHeight;

		if (nbResize != false)
		{
			//��ȡ�ο��ߴ�����
			LONG llWidth = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_ELEMENT_WIDTH,0);
			LONG llHeight = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_ELEMENT_HEIGHT,0);

			if (llWidth == 0 || llHeight == 0)
				mpIterator->SetSize(mdFrameSize.width,mdFrameSize.height); //���û�����òο��ߴ磬���Լ��������Ϊ׼
			else
				mpIterator->SetSize((FLOAT)llWidth,(FLOAT)llHeight);

		}

		if((mpBgBitmap->GetExtnedLineX() != 0 && mpBgBitmap->GetExtnedLineY() != 0)/* && ((mpIterator->GetSizeX() - mdFrameSize.width > 1) || mpIterator->GetSizeY() - mdFrameSize.height > 1)*/)
			mulMethod = ESPB_DRAWBMP_EXTEND;	//�����������չ�߲��Ҳο��ߴ����ʵ�ʳߴ磬�Ǿ�ʹ����չ��ʽ

		mfBeginPos = mdFrameSize.width * mlCurrentIndex;	//����ʱ��X������ʼ��

		lbret = true;

	} while (false);

	return lbret;
}

// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
ERESULT CEvPictureFrame::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������

	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	do 
	{
		BREAK_ON_NULL(npMsg);

		switch (npMsg->GetMessageID())
		{
		case EACT_PICTUREFRAME_SET_INDEX:	
			{
				//�л���ʾ֡
				if(npMsg->GetInputDataSize() != sizeof(LONG))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}

				LONG* lpValue = (LONG*)npMsg->GetInputData();

				luResult = OnChangeIndex(*lpValue);

				break;
			}
		case EACT_PICTUREFRAME_CHANGE_PIC:	
			{
				//������ʾͼƬ,���·��
				wchar_t* lpValue = (wchar_t*)npMsg->GetInputData();

				luResult = OnChangePic(lpValue,false);

				if (mpBgBitmap != NULL && npMsg->GetOutputBufferSize() == sizeof(D2D1_SIZE_F))
				{
					//�����������Ҫ�õ�֡��С
					D2D1_SIZE_F* lpOut = (D2D1_SIZE_F*)npMsg->GetOutputBuffer();
					*lpOut = mdFrameSize;
					npMsg->SetOutputDataSize(sizeof(D2D1_SIZE_F));
				}

				break;
			}
		case EACT_PICTUREFRAME_CHANGE_PIC_FULLPATH:	
			{
				//������ʾͼƬ,ȫ·��
				wchar_t* lpValue = (wchar_t*)npMsg->GetInputData();

				luResult = OnChangePic(lpValue,true);

				if (mpBgBitmap != NULL && npMsg->GetOutputBufferSize() == sizeof(D2D1_SIZE_F))
				{
					//�����������Ҫ�õ�֡��С
					D2D1_SIZE_F* lpOut = (D2D1_SIZE_F*)npMsg->GetOutputBuffer();
					(*lpOut).width = mpBgBitmap->GetWidth();
					(*lpOut).height = mpBgBitmap->GetHeight();
					npMsg->SetOutputDataSize(sizeof(D2D1_SIZE_F));
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

//����
ERESULT CEvPictureFrame::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npPaintBoard);

		if (mpBgBitmap != NULL)
		{
			npPaintBoard->DrawBitmap(D2D1::RectF(0,0,mpIterator->GetSizeX(),mpIterator->GetSizeY()),
				D2D1::RectF(mfBeginPos,0,mfBeginPos + mdFrameSize.width,mdFrameSize.height),
				mpBgBitmap,
				mulMethod
				);
		}

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEvPictureFrame::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		mpIterator->ModifyStyles(/*EITR_STYLE_CONTROL*/NULL,EITR_STYLE_MOUSE);

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

//�л���ʾ֡,��һ֡Ϊ1
ERESULT CEvPictureFrame::OnChangeIndex(LONG nlIndex)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(nlIndex <= 0 || nlIndex > mlMaxFrame)
			break;

		mlCurrentIndex = nlIndex - 1;

		mfBeginPos = mdFrameSize.width * mlCurrentIndex;	//����ʱ��X������ʼ��

		EinkuiGetSystem()->UpdateView();

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

//������ʾͼƬ
ERESULT CEvPictureFrame::OnChangePic(wchar_t* npswPicPath,bool nbIsFullPath)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do 
	{
		if(npswPicPath == NULL || npswPicPath[0] == UNICODE_NULL)
			break;

		CMM_SAFE_RELEASE(mpBgBitmap);	//ȥ��ԭ����ͼƬ

		mpBgBitmap = EinkuiGetSystem()->GetAllocator()->LoadImageFile(npswPicPath,nbIsFullPath);
		BREAK_ON_NULL(mpBgBitmap);

		mlMaxFrame = 1;	//Ŀǰֻ֧���л�Ϊһ֡��ͼ

		Resize(false);

		EinkuiGetSystem()->UpdateView();

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

// ��������
ERESULT CEvPictureFrame::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	ERESULT luResult = ERESULT_SUCCESS;

	do 
	{
		BREAK_ON_NULL(mpBgBitmap);

		float lfX = mpIterator->GetSizeX() * mlCurrentIndex;  	//���ĸ�λ�ÿ�ʼ��ʾ

		if(rPoint.x < 0.0f || (UINT)rPoint.x >= mpIterator->GetSizeX() || rPoint.y < 0.0f || (UINT)rPoint.y >= mpIterator->GetSizeY())
			break;

		//????ͨ������Alphaֵ���

		luResult = ERESULT_MOUSE_OWNERSHIP;

	} while (false);

	return luResult;
}

//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CEvPictureFrame::OnElementShow(bool nbIsShow)
{
	if (mbIsAutoPlay != false)
	{
		//������Զ����Ŷ�����Ԫ��
		if (nbIsShow == false)
		{
			//�����������״̬���͹ص���ʱ��
			mpIterator->KillTimer(TF_ID_PIC_TIMER_ID_PLAY);
		}
		else
		{
			//��ʼ������������ʾ��1֡
			mpIterator->SetTimer(TF_ID_PIC_TIMER_ID_PLAY, MAXULONG64, mlPlayTimerElapse, NULL);
			mlCurrentIndex = 1;
		}
	}
	

	return ERESULT_SUCCESS;
}

//��ʱ��
void CEvPictureFrame::OnTimer(PSTEMS_TIMER npStatus)
{
	do
	{
		if (npStatus->TimerID == TF_ID_PIC_TIMER_ID_PLAY)
		{
			//���Ŷ���

			if (mlCurrentIndex < mlMaxFrame - 1)
			{
				mlCurrentIndex++;
			}
			else
			{
				mlCurrentIndex = 1;	//�������ҳ�󣬷��ص�һ֡��ʾ
				if (mbIsPlayLoop == false)
					mpIterator->KillTimer(TF_ID_PIC_TIMER_ID_PLAY); //����Ƿ�ѭ������ֻ����һ��
			}
			OnChangeIndex(mlCurrentIndex);

		}

	} while (false);

}