/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "stdafx.h"
#include "cmmBaseObj.h"
#include "Einkui.h"
#include "ElementImp.h"
#include "EvListImp.h"
#include "XCtl.h"
#include "EvButtonImp.h"
#include   <algorithm>  
//using namespace D2D1;


DEFINE_BUILTIN_NAME(List)

// ֻ���ڱ������ó�ʼֵ����ָ����ΪNULL�����п���ʧ�ܵ������֮������㶼Ӧ����InitOnCreate�н���
CEvList::CEvList()
{
	mlShowStyle = LIST_VIEW_STYLE_REPORT;
	mbShowVerScroll = true;
	mbShowHorScroll = true;
	mlDocumentHeight = 0;
	mlDocumentWidth = 0;
	mlDocToView0PositionY = 0;
	mbNeedMemoryManager = true;
	mpVScrollBar = NULL;
	mpHScrollBar = NULL;
	mbSetScrollBarPositionAndSize = false;
	mbAcceptDrop = false;
	mpIterMoveItem = NULL;
	mpIterPicMoveForMouse = NULL;
	mpIterInsertMark = NULL;
}

// �����ͷų�Ա����
CEvList::~CEvList()
{
	//ResetList();

	//CMM_SAFE_RELEASE(mpVScrollBar);
	//CMM_SAFE_RELEASE(mpHScrollBar);

}

ULONG CEvList::InitOnCreate(
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
		//�������������
		ICfKey * lpVScrollKey = npTemplete->GetSubKey(L"VScroll");
		if(lpVScrollKey)
		{
			mpVScrollBar = CEvScrollBar::CreateInstance(mpIterator,lpVScrollKey,V_SCROLL_BAR_ID);			
		}
		//�������������
		ICfKey * lpHScrollKey = npTemplete->GetSubKey(L"HScroll");
		if(lpHScrollKey)
		{
			mpHScrollBar = CEvScrollBar::CreateInstance(mpIterator,lpHScrollKey,H_SCROLL_BAR_ID);			
		}
		
		mpIterator->ModifyStyles(EITR_STYLE_ALL_MWHEEL);
		//����InsertMark
		ICfKey * lpMarkKey = npTemplete->GetSubKey(L"InsertMark");
		if(lpMarkKey)
		{
			mpIterInsertMark = EinkuiGetSystem()->GetAllocator()->CreateElement (mpIterator,lpMarkKey,INSERT_MARK_ID);	
			if(mpIterInsertMark)
				mpIterInsertMark->SetVisible(false);
		}
		
		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CEvList::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		//װ��һЩ��Ҫ��������Դ
		LoadResource();

		//���ù�����λ�úʹ�С
		//������ʾ����
		if(mlShowStyle != LIST_VIEW_STYLE_AUTO_FIT_X && mlShowStyle != LIST_VIEW_STYLE_AUTO_FIT_Y)
		{
			D2D1_RECT_F lRectVisibel;
			lRectVisibel.left = 0;
			lRectVisibel.top = 0;
			lRectVisibel.right = lRectVisibel.left + mpIterator->GetSizeX();
			lRectVisibel.bottom = lRectVisibel.top + mpIterator->GetSizeY();
			mpIterator->SetVisibleRegion(lRectVisibel);
			//�������ù�������λ�úʹ�С
			SetScrollBarPositionAndSize();
		}
		else
		{
			if(mpVScrollBar != NULL)
				mpVScrollBar->GetIterator()->SetVisible(false);
		}
		lResult = ERESULT_SUCCESS;
	}while(false);

	return lResult;
}

//����ScrollBar��λ��
bool CEvList::SetScrollBarPositionAndSize()
{
	//if(mbSetScrollBarPositionAndSize == false)
	{
		//����ScrollBarλ��
		if(mpVScrollBar)
		{
			//mpVScrollBar->GetIterator()->SetVisible(false);
			FLOAT lfScrollBarWidth = mpVScrollBar->GetVScrollBarWidth() ;
			FLOAT lfX = mpIterator->GetSizeX();
			FLOAT lfY = mpIterator->GetSizeY();
			mpVScrollBar->GetIterator()->SetPosition(lfX-lfScrollBarWidth-1,1 );
			mpVScrollBar->GetIterator()->SetSize(lfScrollBarWidth,lfY-2);
			mpVScrollBar->Relocation();
			mbSetScrollBarPositionAndSize = true;
		}

	}
	return true;
}

// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
ERESULT CEvList::OnElementDestroy()
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		CXuiElement::OnElementDestroy();	//���û���

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

//װ��������Դ
ERESULT CEvList::LoadResource()
{

	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	ICfKey* lpValue = NULL;

	do 
	{
		//��ȡ֡��Ϣ
		mlMaxFrame = mpTemplete->QuerySubKeyValueAsLONG(TF_ID_LIST_BACKIAMGE_FRAME_COUNT,1);

		if (mlMaxFrame > 0 && mpBgBitmap != NULL)
		{
			//����ÿ֡��С
			UINT luiWidth = mpBgBitmap->GetWidth();
			UINT luiHeight = mpBgBitmap->GetHeight();
			mlCurrentIndex = 0;

			mpIterator->SetSize(float(luiWidth / mlMaxFrame),(float)luiHeight);
		}
		//��ȡ��ʾģʽ

		//��ȡ�Ƿ���Ҫ��ʾ������������

		// ��ȡ�������
		mlShowStyle = (LONG)mpTemplete->QuerySubKeyValueAsLONG(TF_ID_LIST_STYLE,1);

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}
//����Resize ����������ʾ����
ERESULT CEvList::OnElementResized(D2D1_SIZE_F nNewSize)
{
	
	if(mlShowStyle != LIST_VIEW_STYLE_AUTO_FIT_X && mlShowStyle != LIST_VIEW_STYLE_AUTO_FIT_Y)
	{
		//������ʾ����
		D2D1_RECT_F lRectVisibel;
		lRectVisibel.left = 0;
		lRectVisibel.top = 0;
		lRectVisibel.right = lRectVisibel.left + mpIterator->GetSizeX();
		lRectVisibel.bottom = lRectVisibel.top + mpIterator->GetSizeY();
		mpIterator->SetVisibleRegion(lRectVisibel);
		//�������ù�������λ�úʹ�С
		SetScrollBarPositionAndSize();

		//���¼����������ù�����С
		CalcElementPosition();
		//
		CheckVScrollBarNeedToShow();
	}
	return ERESULT_SUCCESS;
}
// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
ERESULT CEvList::ParseMessage(IEinkuiMessage* npMsg)
{
	// ʵ��ԭ�����ȵ�������ķֽ⹦�ܣ����󽫲��������Ϣ��������

	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;

	do 
	{
		BREAK_ON_NULL(npMsg);

		switch (npMsg->GetMessageID())
		{
			
	
			//���������Ϣ
		case EACT_SCROLLBAR_VSCROLL_THUMB_POSITION:
			{
				if(npMsg->GetInputDataSize() != sizeof(FLOAT))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				//FLOAT* lpValue = (FLOAT*)npMsg->GetInputData();			
				FLOAT lTempPos = *(FLOAT*)npMsg->GetInputData();
				if(lTempPos > (mlDocumentHeight - mpIterator->GetSizeY()))
					lTempPos = mlDocumentHeight - mpIterator->GetSizeY();
				else if(lTempPos < 0.0f)
					lTempPos = 0.0f;

				mlDocToView0PositionY = lTempPos;
				CalcElementPosition();

				//�践�ظ������ڣ��������ڹ���
				PostMessageToParent(EACT_LIST_SCROLLING,mlDocToView0PositionY);
				luResult = ERESULT_SUCCESS;
				break;
			}
		case EACT_LIST_SET_PIC_INDEX:	
			{
				//�л���ʾ֡
				if(npMsg->GetInputDataSize() != sizeof(LONG))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				LONG* lpValue = (LONG*)npMsg->GetInputData();
				luResult = OnChangeBackImageIndex(*lpValue);

				break;
			}
		case EACT_LIST_SET_MEMORY_MANAGER:	
			{
				//�л���ʾ֡
				if(npMsg->GetInputDataSize() != sizeof(bool))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				bool* lpValue = (bool*)npMsg->GetInputData();
				mbNeedMemoryManager = *lpValue;
				luResult = ERESULT_SUCCESS;

				break;
			}
		case EACT_LIST_CHANGE_PIC:	
			{
				//������ʾͼƬ			

				wchar_t* lpValue = (wchar_t*)npMsg->GetInputData();

				luResult = OnChangeBackImagePic(lpValue);
				break;
			}
		case EACT_LIST_ITEMCLICK:
		case EACT_LIST_ITEMCLICK_RBUTTON:
		case EEVT_MENUITEM_CLICK:
			{
				//���򸸴���
				mpIterator->PostMessageToParent(npMsg);
				luResult = ERESULT_SUCCESS;
				break;
			}
		case EACT_LIST_DOCSCROLL:
			{
				if(npMsg->GetInputDataSize() != sizeof(FLOAT))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				if(mpVScrollBar->GetIterator()->IsEnable() == false || mpVScrollBar->GetIterator()->IsVisible() == false)
					break;
				FLOAT* lpValue = (FLOAT*)npMsg->GetInputData();
				FLOAT lTempPos = *lpValue;
				if(lTempPos > (mlDocumentHeight - mpIterator->GetSizeY()))
					lTempPos = mlDocumentHeight - mpIterator->GetSizeY();
				else if(lTempPos < 0.0f)
					lTempPos = 0.0f;
				mlDocToView0PositionY = lTempPos;
				CalcElementPosition();
				CheckVScrollBarNeedToShow();
				//��Ҫ���ù������Ļ���λ��
				if(mpVScrollBar)
					EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(mpVScrollBar->GetIterator(),
					EACT_SCROLLBAR_VSCROLL_SET_SLIDER_POSTION,&mlDocToView0PositionY,sizeof(FLOAT),NULL,0);
				luResult = ERESULT_SUCCESS;

				break;
			}
		case EACT_LIST_GET_SCROLL_RANGE:
			{
				FLOAT lfScrollRange = mlDocumentHeight - mpIterator->GetSizeY();
				if(npMsg->GetOutputBufferSize() == sizeof(FLOAT) && npMsg->GetOutputBuffer() != NULL)
				{
					//RtlCopyMemory(npMsg->GetOutputBuffer(),&lfScrollRange,sizeof(FLOAT));
					*(FLOAT*)npMsg->GetOutputBuffer() = lfScrollRange;
					luResult = ERESULT_SUCCESS;
				}
				else
				{
					luResult = ERESULT_WRONG_PARAMETERS;
				}
				//npMsg->SetOutputBuffer(&lfScrollRange,sizeof(FLOAT));

				break;
			}
		case EACT_LIST_GET_SCROLL_POSITION:
			{
				FLOAT lfScrollPosition = mlDocToView0PositionY;

				if(npMsg->GetOutputBufferSize() == sizeof(FLOAT) && npMsg->GetOutputBuffer() != NULL)
				{
					//RtlCopyMemory(npMsg->GetOutputBuffer(),&lfScrollPosition,sizeof(FLOAT));
					*(FLOAT*)npMsg->GetOutputBuffer() = lfScrollPosition;
					luResult = ERESULT_SUCCESS;
				}
				else
				{
					luResult = ERESULT_WRONG_PARAMETERS;
				}
				break;
			}	
		case EACT_LIST_ADD_ELEMENT:
			{
				if(npMsg->GetInputDataSize() != sizeof(IEinkuiIterator*))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				IEinkuiIterator* lpElement = *(IEinkuiIterator**)npMsg->GetInputData();
				bool bRet = AddElement(lpElement,mpElementVector.Size());
				
				/*if(npMsg->GetOutputBufferSize() == sizeof(bool))
					*(bool*)npMsg->GetOutputBuffer() = bRet;*/
				luResult = ERESULT_SUCCESS;
				break;
			}
		case EACT_LIST_ADD_ELEMENT_HEAD:
			{
				if(npMsg->GetInputDataSize() != sizeof(IEinkuiIterator*))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				IEinkuiIterator* lpElement = *(IEinkuiIterator**)npMsg->GetInputData();
				bool bRet = AddElement(lpElement,0);
				npMsg->SetOutputDataSize(sizeof(bool));
				//npMsg->SetOutputBuffer(&bRet,sizeof(bool));
				if(npMsg->GetOutputBufferSize() == sizeof(bool))
					*(bool*)npMsg->GetOutputBuffer() = bRet;
				luResult = ERESULT_SUCCESS;
				break;
			}
		case EACT_LIST_DELETE_ELEMENT_NO_CLOSE:
			{
				if(npMsg->GetInputDataSize() != sizeof(IEinkuiIterator*))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				IEinkuiIterator* lpElement = *(IEinkuiIterator**)npMsg->GetInputData();
				bool bRet = false;
				for(int i = 0; i < mpElementVector.Size();i++)
				{
					if(lpElement == mpElementVector[i])
					{
						bRet = DeleteElement(i,false);
						break;
					}
				}
				npMsg->SetOutputDataSize(sizeof(bool));
				//npMsg->SetOutputBuffer(&bRet,sizeof(bool));
				if(npMsg->GetOutputBufferSize() == sizeof(bool))
					*(bool*)npMsg->GetOutputBuffer() = bRet;
				luResult = ERESULT_SUCCESS;
				break;
			}
		case EACT_LIST_DELETE_ELEMENT:
			{
				if(npMsg->GetInputDataSize() != sizeof(IEinkuiIterator*))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				IEinkuiIterator* lpElement = *(IEinkuiIterator**)npMsg->GetInputData();
				bool bRet = DeleteElement(lpElement);
				npMsg->SetOutputDataSize(sizeof(bool));
				//npMsg->SetOutputBuffer(&bRet,sizeof(bool));
				if(npMsg->GetOutputBufferSize() == sizeof(bool))
					*(bool*)npMsg->GetOutputBuffer() = bRet;

				
				luResult = ERESULT_SUCCESS;
				break;
			}
		case EACT_LIST_DELETE_ELEMENT_INDEX:
			{
				if(npMsg->GetInputDataSize() != sizeof(long))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				long* lpValue = (long*)npMsg->GetInputData();
				bool bRet = DeleteElement(*lpValue);
				npMsg->SetOutputDataSize(sizeof(bool));				
				if(npMsg->GetOutputBufferSize() == sizeof(bool))
					*(bool*)npMsg->GetOutputBuffer() = bRet;
				luResult = ERESULT_SUCCESS;
				break;
			}
		case EACT_LIST_DELETE_ELEMENT_BIGGER_OR_EQUAL_INDEX_NO_CLOSE:
			{

				if(npMsg->GetInputDataSize() != sizeof(long))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				long* lpValue = (long*)npMsg->GetInputData();
				bool bRet = false;
				for(int i = mpElementVector.Size()-1;  i >= *lpValue;i--)
					bRet = DeleteElement(i,false);

				npMsg->SetOutputDataSize(sizeof(bool));				
				if(npMsg->GetOutputBufferSize() == sizeof(bool))
					*(bool*)npMsg->GetOutputBuffer() = bRet;
				luResult = ERESULT_SUCCESS;
				break;
			}
		case EACT_LIST_DELETE_ELEMENT_BIGGER_OR_EQUAL_INDEX:
			{
				if(npMsg->GetInputDataSize() != sizeof(long))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				long* lpValue = (long*)npMsg->GetInputData();
				bool bRet = false;
				for(int i = mpElementVector.Size()-1;  i >= *lpValue;i--)
					bRet = DeleteElement(i);
				
				npMsg->SetOutputDataSize(sizeof(bool));				
				if(npMsg->GetOutputBufferSize() == sizeof(bool))
					*(bool*)npMsg->GetOutputBuffer() = bRet;
				luResult = ERESULT_SUCCESS;
				break;
			}
		case  EACT_LIST_RESET:
			{
				ResetList();
				luResult = ERESULT_SUCCESS;
				break;
			}
		case EACT_LIST_SET_STYLE:
			{
				if(npMsg->GetInputDataSize() != sizeof(LONG))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				LONG* lpValue = (LONG*)npMsg->GetInputData();
				SetListViewStyle(*lpValue);
				luResult = ERESULT_SUCCESS;
			}
		case EEVT_BUTTON_CLICK:
			{
				//����ת��
				if(npMsg->IsSent())
				{					
					if(mpIterator->GetParent())
						mpIterator->GetParent()->SendMessageW(npMsg);
				}
				else
					mpIterator->GetParent()->PostMessageW(npMsg);
				//mpIterator->PostMessageToParent(npMsg);
			}
			luResult = ERESULT_SUCCESS;
			break;
		case EACT_LIST_SET_VSCROLLBAR_RECT:
			{
				if(npMsg->GetInputDataSize() != sizeof(D2D1_RECT_F))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				D2D1_RECT_F* lpRect = (D2D1_RECT_F*)npMsg->GetInputData();
				if(lpRect && mpVScrollBar)
				{
					luResult = ERESULT_SUCCESS;
					FLOAT lfScrollBarWidth = mpVScrollBar->GetVScrollBarWidth() ;
					FLOAT lfX = lpRect->left;
					FLOAT lfY = lpRect->top;
					mpVScrollBar->GetIterator()->SetPosition(lpRect->left,lpRect->top );
					mpVScrollBar->GetIterator()->SetSize(lfScrollBarWidth,lpRect->bottom - lpRect->top);
					mpVScrollBar->Relocation();
				}				
				luResult = ERESULT_SUCCESS;
				break;
			}
		case EACT_LIST_GET_ITEM_POSITION:
			{
				if(npMsg->GetInputDataSize() != sizeof(IEinkuiIterator*))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				IEinkuiIterator* lpElement = *(IEinkuiIterator**)npMsg->GetInputData();

				break;
			}
		case EACT_LIST_GET_ITEM_INDEX:
			{
				if(npMsg->GetInputDataSize() != sizeof(IEinkuiIterator*))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				if(npMsg->GetOutputBufferSize() != sizeof(int))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				IEinkuiIterator* lpElement = *(IEinkuiIterator**)npMsg->GetInputData();
				int i = 0;
				for(i = 0 ; i < mpElementVector.Size();i++)
				{
					if(lpElement == mpElementVector[i])
						break;
				}
				// �����������
				int* lpOut = (int*)npMsg->GetOutputBuffer();
				*lpOut = i;
				npMsg->SetOutputDataSize(sizeof(int));
				break;
			}
		case EACT_LIST_GET_ITEM_NUM:
			{
				
				if(npMsg->GetOutputBufferSize() != sizeof(int))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}				
				int lnNum = mpElementVector.Size();
				
				// �����������
				int* lpOut = (int*)npMsg->GetOutputBuffer();
				*lpOut = lnNum;
				npMsg->SetOutputDataSize(sizeof(int));
				break;
			}
		case EACT_LIST_RECACULATE:
			{
				
				Recaculate();
				
				luResult = ERESULT_SUCCESS;
				break;
				
			}
		case EACT_LIST_INSER_ITEM_INDEX:
			{
				_STCTL_LIST_INSERT *lIns = NULL;
				if(ERESULT_SUCCESS != CExMessage::GetInputDataBuffer(npMsg, lIns))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				BREAK_ON_NULL(lIns);
				bool bRet = AddElement(lIns->mpElement,lIns->mnIndex);

				if(npMsg->GetOutputBufferSize() == sizeof(bool))
					*(bool*)npMsg->GetOutputBuffer() = bRet;
				luResult = ERESULT_SUCCESS;				
				break;
			}
		case EACT_LIST_SWAP_ELEMENT:
			{
				if(npMsg->GetInputDataSize() != sizeof(_STCTL_LIST_SWAP*) )
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				_STCTL_LIST_SWAP *lSwap = (_STCTL_LIST_SWAP*)npMsg->GetInputData();
				BREAK_ON_NULL(lSwap);
				BREAK_ON_NULL(lSwap->mpFromElement);
				BREAK_ON_NULL(lSwap->mpToElement);
				int lPos1,lPos2;
				lPos2 = lPos1 = -1;
				for(int i = 0;i < mpElementVector.Size();i++)
				{
					if(mpElementVector[i] == lSwap->mpFromElement)
						lPos1 = i;
					if(mpElementVector[i] == lSwap->mpToElement)
						lPos2 = i;
				}

				if(lPos2 == -1 || lPos1 == -1)
					break;
				mpElementVector[lPos1] = lSwap->mpToElement;
				mpElementVector[lPos2] = lSwap->mpFromElement;
				CalcElementPosition();
				
				bool lbRet = true;
				if(npMsg->GetOutputBufferSize() == sizeof(bool))
					*(bool*)npMsg->GetOutputBuffer() = lbRet;
				luResult = ERESULT_SUCCESS;		
				break;
			}
		case EACT_LIST_SET_ACCEPT_DROP_ITEM:
			{
				if(npMsg->GetInputDataSize() != sizeof(bool) )
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				bool *lbAccept = (bool*)npMsg->GetInputData();
				mbAcceptDrop = *lbAccept;
				if(mbAcceptDrop)
					mpIterator->ModifyStyles(EITR_STYLE_XUIDRAGDROP);
				else
					mpIterator->ModifyStyles(0,EITR_STYLE_XUIDRAGDROP);
				luResult = ERESULT_SUCCESS;	
				break;
			}
		case EMSG_DRAGDROP_ENTER://Drag����
			{
				
				break;
			}
		case EMSG_DRAGDROP_LEAVE://drag�뿪
			{
				OutputDebugString(L"Drop leave");
				if(mpIterInsertMark)
					mpIterInsertMark->SetVisible(false);
				break;
			}
		
		case EMSG_XUI_DROP_TEST:
			{	
			
				if(npMsg->GetInputData()==NULL || npMsg->GetInputDataSize()!=sizeof(STMS_EDRGDRP_REQUEST))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				PSTMS_EDRGDRP_REQUEST lpReq = (PSTMS_EDRGDRP_REQUEST)npMsg->GetInputData();

				if(mbAcceptDrop && IsEqualGUID(lpReq->Reason,EGUID_LIST_DROP_ITEM) != false )
				{				
					
					//mpIterator->SetAlpha(0.4);
					//�����θ��ǣ���ʾ�����ʶ
					int nIndex = CheckPos(lpReq->CurrentPos);
					if(nIndex != -1 && mpIterInsertMark)
					{						
						//mpIterInsertMark->ModifyStyles(EITR_STYLE_TOPDRAW);
						if(nIndex == mpElementVector.Size()) //����ﵽ���
						{
							IEinkuiIterator * lpLastIte = mpElementVector[nIndex-1];
							mpIterInsertMark->SetPosition(lpLastIte->GetPositionX(),
								lpLastIte->GetPositionY() + lpLastIte->GetSizeY() - 6);
						}
						else
							mpIterInsertMark->SetPosition(mpElementVector[nIndex]->GetPositionX() ,
									mpElementVector[nIndex]->GetPositionY() - 6);
						mpIterInsertMark->SetSize(mpElementVector[nIndex]->GetSizeX(),mpIterInsertMark->GetSizeY());
						mpIterInsertMark->SetVisible(true);
						luResult = ERESULT_EDRAGDROP_ACCEPT;
					}
				}					
					
			
				break;
			}
		case EMSG_XUI_DROP_DOWN:
			{
				OutputDebugString(L"Drop down");
				if(mpIterInsertMark)
					mpIterInsertMark->SetVisible(false);
				//Ŀ�����Լ���������
				if(npMsg->GetInputData()==NULL || npMsg->GetInputDataSize()!=sizeof(STMS_EDRGDRP_REQUEST))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				PSTMS_EDRGDRP_REQUEST lpReq = (PSTMS_EDRGDRP_REQUEST)npMsg->GetInputData();

				if(IsEqualGUID(lpReq->Reason,EGUID_LIST_DROP_ITEM)==false/* || lpReq->Flags != ICON_TYPE_WIDGET*/)
					break;

				break;
			}
		case EACT_LIST_SET_DRAG_FOR_MOUSER_ITERATOR:
			{
				if(npMsg->GetInputDataSize() != sizeof(IEinkuiIterator*))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				mpIterPicMoveForMouse = *(IEinkuiIterator**)npMsg->GetInputData();
				//if(mpIterPicMoveForMouse)
				//	mpIterPicMoveForMouse->ModifyStyles(EITR_STYLE_TOPDRAW);
				luResult = ERESULT_SUCCESS;	
				break;
			}
		case EACT_LIST_SET_DROP_MARK_ITERATOR:
			{
				if(npMsg->GetInputDataSize() != sizeof(IEinkuiIterator*))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				mpIterInsertMark = *(IEinkuiIterator**)npMsg->GetInputData();
				//if(mpIterInsertMark)
				//	mpIterInsertMark->ModifyStyles(EITR_STYLE_TOPDRAW);
				luResult = ERESULT_SUCCESS;	
				break;
			}	
		case EACT_LIST_DRAG_ITEM_START:
			{
				if(npMsg->GetInputDataSize() != sizeof(IEinkuiIterator*))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}

				mpIterMoveItem = *(IEinkuiIterator**)npMsg->GetInputData();
				if(mpIterMoveItem)
					mpIterMoveItem->SetAlpha(0.5);
				
				luResult = ERESULT_SUCCESS;	
				break;
			}
		case EACT_LIST_DRAG_ITEM_DRAGING:
			{

				if(npMsg->GetInputDataSize() != sizeof(STMS_DRAGGING_ELE))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				STMS_DRAGGING_ELE * lpDrageEle = (STMS_DRAGGING_ELE*)npMsg->GetInputData();
				if(lpDrageEle && mpIterPicMoveForMouse)
				{
					mpIterPicMoveForMouse->SetVisible(true);
					mpIterPicMoveForMouse->SetPosition(lpDrageEle->CurrentPos.x,
						lpDrageEle->CurrentPos.y);					
				}
			

				luResult = ERESULT_SUCCESS;	
				break;
			}
		case EACT_LIST_DRAG_ITEM_END:
			{
				if(npMsg->GetInputDataSize() != sizeof(IEinkuiIterator*))
				{
					luResult = ERESULT_WRONG_PARAMETERS;
					break;
				}
				IEinkuiIterator * lpIte = *(IEinkuiIterator**)npMsg->GetInputData();
				if(lpIte)
					lpIte->SetAlpha(1.0);
				if(mpIterPicMoveForMouse)
					mpIterPicMoveForMouse->SetVisible(false);
			
				luResult = ERESULT_SUCCESS;	
				break;
			}
		case EACT_LIST_SHOW_BY_INDEX:
			{
				////��ָ�����ƶ�������ʾ����
				LONG llIndex = -1;
				if(CExMessage::GetInputData(npMsg,llIndex) != ERESULT_SUCCESS)
					break;

				ShowByIndex(llIndex);

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
ERESULT CEvList::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{

	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		BREAK_ON_NULL(npPaintBoard);

		if (mpBgBitmap != NULL)
		{
			float lfX = mpIterator->GetSizeX() * mlCurrentIndex;

			npPaintBoard->DrawBitmap(D2D1::RectF(0,0,mpIterator->GetSizeX(),mpIterator->GetSizeY()),
				D2D1::RectF(lfX,0,lfX + (float)mpBgBitmap->GetWidth(),(float)mpBgBitmap->GetHeight()),
				mpBgBitmap,
				ESPB_DRAWBMP_EXTEND
				);
		}

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}


//�л���ʾ֡,��һ֡Ϊ1
ERESULT CEvList::OnChangeBackImageIndex(LONG nlIndex)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(nlIndex <= 0 || nlIndex > mlMaxFrame)
			break;

		mlCurrentIndex = nlIndex - 1;

		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

//������ʾͼƬ
ERESULT CEvList::OnChangeBackImagePic(wchar_t* npswPicPath)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;

	do 
	{
		if(npswPicPath == NULL || npswPicPath[0] == UNICODE_NULL)
			break;

		CMM_SAFE_RELEASE(mpBgBitmap);	//ȥ��ԭ����ͼƬ

		mpBgBitmap = EinkuiGetSystem()->GetAllocator()->LoadImageFile(npswPicPath);
		BREAK_ON_NULL(mpBgBitmap);

		mlMaxFrame = 1;	//Ŀǰֻ֧���л�Ϊһ֡��ͼ

		leResult = ERESULT_SUCCESS;

	} while (false);

	return leResult;
}

// ��������
ERESULT CEvList::OnMouseOwnerTest(const D2D1_POINT_2F& rPoint)
{
	ERESULT luResult = ERESULT_SUCCESS;

	do 
	{
		if(rPoint.x < 0.0f || (UINT)rPoint.x >= mpIterator->GetSizeX() || 
			rPoint.y < 0.0f || (UINT)rPoint.y >= mpIterator->GetSizeY())
			break;

		luResult = ERESULT_MOUSE_OWNERSHIP;

	} while (false);

	return luResult;
}
bool CEvList::CheckVScrollBarNeedToShow()
{
	bool lbRet = false;
	if(mlShowStyle == LIST_VIEW_STYLE_AUTO_FIT_Y || mlShowStyle == LIST_VIEW_STYLE_AUTO_FIT_X)
	{
		if(mpVScrollBar) mpVScrollBar->GetIterator()->SetVisible(false);
		//if(mp)
		return true;
	}


	FLOAT lfDeata = mlDocumentHeight - mpIterator->GetSizeY();

	if(lfDeata > 0)
	{
		if(mpVScrollBar)
		{
			if (mpTemplete->QuerySubKeyValueAsLONG(L"ShowOrDisable",0) == 0)
			{
				if(!mpVScrollBar->GetIterator()->IsEnable())
				{
					mpVScrollBar->GetIterator()->SetEnable(true);			
				}
			}
			else
			{
				if(!mpVScrollBar->GetIterator()->IsVisible())
				{
					mpVScrollBar->GetIterator()->SetVisible(true);
					bool lbShow = true;
					EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(mpIterator->GetParent(),
						EACT_LIST_VSCROLLBAR_SHOW,&lbShow,sizeof(bool),NULL,0);
				}
			}

			lbRet = true;
			mpVScrollBar->SetDeltaSize(lfDeata);
		}
	}
	else
	{
		if(mpVScrollBar)
		{
			if (mpTemplete->QuerySubKeyValueAsLONG(L"ShowOrDisable",0) == 0)
			{
				if(mpVScrollBar->GetIterator()->IsEnable())
				{
					mpVScrollBar->GetIterator()->SetEnable(false);				
				}
			}
			else
			{
				if(mpVScrollBar->GetIterator()->IsVisible())
				{
					mpVScrollBar->GetIterator()->SetVisible(false);
					bool lbShow = false;
					EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(mpIterator->GetParent(),
						EACT_LIST_VSCROLLBAR_SHOW,&lbShow,sizeof(bool),NULL,0);
				}
			}

			lbRet = true;
		}
	}

	return lbRet;
}
//����λ�ã������Ա������λ��
bool CEvList::CalcElementPosition()
{
	bool lbRet = false;
	if(mlShowStyle == LIST_VIEW_STYLE_REPORT && mpElementVector.Size() > 0)
	{
		IEinkuiIterator * lpIterator = NULL;
		IEinkuiIterator * lpPreIterator = NULL;

		//λ�ô�-mlDocToView0PositionY��ʼ
		FLOAT lfPositionY = -mlDocToView0PositionY; 
		mlDocumentHeight = 0.0f;
		for(int i = 0; i < mpElementVector.Size();i++)
		{
			lpIterator = mpElementVector[i];
			if(lpIterator && lpIterator->IsVisible())
			{
				mlDocumentHeight += lpIterator->GetSizeY();
				//���ǰһ������գ���ǰ��λ��Ϊ��ǰһ��PositionY�� + ��ǰһ��sizeY��			
				if(lpPreIterator)
					lfPositionY += lpPreIterator->GetSizeY();

				lpIterator->SetPosition(0,lfPositionY);
				lpPreIterator = lpIterator;
			}

		}
	}
	else if(mlShowStyle == LIST_VIEW_STYLE_SMALLITEM && mpElementVector.Size() > 0)
	{
		//��ȡView��size ����Ҫ��ȥ�������Ŀ��
		FLOAT lfViewSizeX = mpIterator->GetSizeX();
		FLOAT lfViewSizeY = mpIterator->GetSizeY();
		//һ��һ���Ų�����ͳ��һ�е����߶ȣ��������docmentHeigth
		FLOAT lfCurrentLineMaxHeigth = 0;
		FLOAT lfCurrentLinePositionX = 0;
		FLOAT lfCurrentLinePositionY = -mlDocToView0PositionY;
		FLOAT lfCurrentLineUsedX = 0; //��ʱ��������¼һ���Ѿ����˶���
		IEinkuiIterator * lpIterator = NULL;
		IEinkuiIterator * lpPreIterator = NULL;
		mlDocumentHeight = 0.0;
		for(int i = 0; i < mpElementVector.Size();i++)
		{
			lpIterator = mpElementVector[i];
			if(lpIterator && lpIterator->IsVisible())
			{
				//�����ܲ������ڵ�ǰ�У����оͻ���
				bool lbIsChangeLine = false;
				if(lfViewSizeX - lfCurrentLineUsedX < lpIterator->GetSizeX())
				{
					//������о�Ҫ���У��������õ�ǰ�б���
					mlDocumentHeight += lfCurrentLineMaxHeigth;
					lfCurrentLineUsedX = 0;
					lfCurrentLinePositionX = 0;
					lfCurrentLinePositionY+= lfCurrentLineMaxHeigth;
					lfCurrentLineMaxHeigth = 0;
					lbIsChangeLine = true;

				}

				//����check �����ܲ��ܷ��뵱ǰ�У������µ��У�
				//����ⶼ���У�˵�����element�Ŀ�ȳ���view�Ŀ�ȣ����账��
				if(lfViewSizeX - lfCurrentLineUsedX >= lpIterator->GetSizeX())
				{
					//���Է��ڵ�ǰ��,�޸ĵ�ǰ�б���
					lfCurrentLineUsedX += lpIterator->GetSizeX();
					if(lfCurrentLineMaxHeigth < lpIterator->GetSizeY())
						lfCurrentLineMaxHeigth = lpIterator->GetSizeY();

					if(!lbIsChangeLine && lpPreIterator)
						lfCurrentLinePositionX += lpPreIterator->GetSizeX();
					lpIterator->SetPosition(lfCurrentLinePositionX,lfCurrentLinePositionY);

				}			
			}//if(lpIterator)
			lpPreIterator = lpIterator;
		}//for
		//�������һ�еĸ߶�
		if(lpIterator)
		   mlDocumentHeight += lfCurrentLineMaxHeigth;
	}
	else if(mlShowStyle == LIST_VIEW_STYLE_AUTO_FIT_Y && mpElementVector.Size() > 0)
	{
		IEinkuiIterator * lpIterator = NULL;
		IEinkuiIterator * lpPreIterator = NULL;

		//λ�ô�-mlDocToView0PositionY��ʼ
		FLOAT lfPositionY = 0; 
		mlDocumentHeight = 0.0;
		FLOAT lfMaxSizeX = 0;
		for(int i = 0; i < mpElementVector.Size();i++)
		{
			lpIterator = mpElementVector[i];
			if(lpIterator && lpIterator->IsVisible())
			{
				mlDocumentHeight += lpIterator->GetSizeY();
				//��ȡ�����
				if(lpIterator->GetSizeX() > lfMaxSizeX)
					lfMaxSizeX = lpIterator->GetSizeX();
				//���ǰһ������գ���ǰ��λ��Ϊ��ǰһ��PositionY�� + ��ǰһ��sizeY��			
				if(lpPreIterator)
					lfPositionY += lpPreIterator->GetSizeY();

				lpIterator->SetPosition(0,lfPositionY);
				lpPreIterator = lpIterator;
			}
		}
		mpIterator->SetSize(lfMaxSizeX,mlDocumentHeight);
	}
	return lbRet;
}
//��������
//������ʾģʽ
bool CEvList::SetListViewStyle(LONG nlShowStyle)
{
	mlShowStyle = nlShowStyle;
	return true;
}
//�����Ƿ���ʾ���������Ĺ�����
bool CEvList::ShowScrollBar(bool nbShowVer,bool nbShowHor)
{
	mbShowHorScroll = nbShowHor;
	mbShowVerScroll = nbShowVer;
	return true;
}

//���һ��Ԫ�ص�List������list�����������ͷŸ�Ԫ��
bool CEvList::AddElement(IEinkuiIterator * npElement,int nIndex ) 
{
	if(npElement == NULL)
		return false;

	bool lbRet = false;
	mpElementVector.Insert(nIndex,npElement);
	lbRet = CalcElementPosition();
	lbRet = CheckVScrollBarNeedToShow();
	
	return lbRet;
}

//���List�����ͷ�ԭ�е�Ԫ��
bool CEvList::ResetList()
{
	//�ͷŶ�Ӧ���ڴ�
	if(mbNeedMemoryManager)
	{
		for(int i = 0; i < mpElementVector.Size();i++)
		{
			mpElementVector[i]->Close();
			//mpElementVector[i]->Release();
		}
	}

	mpElementVector.Clear();
	mlDocumentHeight = mlDocumentWidth = 0;
	//���ع�����
	CheckVScrollBarNeedToShow();
	if(mlShowStyle == LIST_VIEW_STYLE_AUTO_FIT_Y )
		mpIterator->SetSize(mpIterator->GetSizeX(),0.0f);
	if(mlShowStyle == LIST_VIEW_STYLE_AUTO_FIT_X)
		mpIterator->SetSize(0.0f,mpIterator->GetSizeY());

	return true;
}

//��������ɾ��Ԫ��
bool CEvList::DeleteElement(LONG nlIndex,bool nbClostElement)
{
	bool lbRet = false;
	if(nlIndex >= 0 && nlIndex < mpElementVector.Size())
	{
		FLOAT lfElementHeigth = mpElementVector[nlIndex]->GetSizeY();

		if(nbClostElement)
			mpElementVector[nlIndex]->Close();
		

		lbRet = mpElementVector.RemoveByIndex(nlIndex);
		if(lbRet)
		{
			//����Doc�߶�			
			lbRet = CalcElementPosition();
		}

		//check docunment�ĸ߶�,���ù�����Χ
		lbRet = CheckVScrollBarNeedToShow();

	}

	return lbRet;
}

//����Ԫ��ָ��ɾ��Ԫ��
bool CEvList::DeleteElement(IEinkuiIterator * npElement)
{
	int liShow = 0;
	for(int i = 0; i < mpElementVector.Size();i++)
	{
		if(npElement == mpElementVector[i])
		{
			DeleteElement(i);
			liShow = i-2;
			ShowByIndex(liShow>0?liShow:0);
			return true;
		}
	}

	return false;
}

//��껬��
ERESULT CEvList::OnMouseWheel(const STEMS_MOUSE_WHEEL* npInfo)
{
	if(npInfo && mpVScrollBar->GetIterator()->IsVisible() && mpElementVector.Size() != 0)
	{	
		float lfPosition = -mlDocToView0PositionY;
		float lfOffset = ( (FLOAT)npInfo->Delta * (mlDocumentHeight/mpElementVector.Size()) )/120.0f;
	    //Ϊ��ͳһ��ֻ����20������
		lfPosition += (lfOffset < 0 ? -LIST_MIN_WHEEL_OFFSET :LIST_MIN_WHEEL_OFFSET);
		if(lfPosition <= -(mlDocumentHeight - mpIterator->GetSizeY()))
			lfPosition = -(mlDocumentHeight - mpIterator->GetSizeY());
		if(lfPosition > 0.0f)
			lfPosition = 0.0f;

		mlDocToView0PositionY = -lfPosition;
		/*TCHAR tcBuf[MAX_PATH]= {0};
		swprintf(tcBuf,L"mldoctovir %f i = %f",mlDocToView0PositionY,mlDocumentHeight);
		OutputDebugString(tcBuf);*/
		EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
			mpIterator,
			EACT_LIST_DOCSCROLL,
			&mlDocToView0PositionY,
			sizeof(float),NULL,0);
	}
	return ERESULT_SUCCESS;
}


//������Ϣ
ERESULT CEvList::OnKeyPressed(const STEMS_KEY_PRESSED* npInfo)
{
	ERESULT luResult = ERESULT_SUCCESS;

	bool lbSet = false;
	float lfPosition = -mlDocToView0PositionY;
	switch (npInfo->VirtualKeyCode )
	{
	case VK_LEFT:		
		break;
	case VK_RIGHT:		
		break;
	case VK_UP:	
		if(mpVScrollBar->GetIterator()->IsVisible()
			&& mpElementVector.Size() != 0 
			&& npInfo->IsPressDown)
		{
			lfPosition += mlDocumentHeight/(mpElementVector.Size());
			lbSet = true;
		}
		break;
	case VK_DOWN:
		if(mpVScrollBar->GetIterator()->IsVisible()
			&& mpElementVector.Size() != 0
			&& npInfo->IsPressDown )
		{
			lfPosition -=  mlDocumentHeight/(mpElementVector.Size());
			lbSet = true;
		}
		break;

	default:
		luResult = ERESULT_SUCCESS;
	}
	if(lbSet)
	{	
	
		if(lfPosition <= -(mlDocumentHeight - mpIterator->GetSizeY()))
			lfPosition = -(mlDocumentHeight - mpIterator->GetSizeY());
		if(lfPosition > 0.0f)
			lfPosition = 0.0f;

		mlDocToView0PositionY = -lfPosition;
		EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
			mpIterator,
			EACT_LIST_DOCSCROLL,
			&mlDocToView0PositionY,
			sizeof(float),NULL,0);
	}

	return luResult;
}

//
////�ַ�������Ϣ
//ERESULT CEvList::OnChar(const PSTEMS_CHAR_INPUT npChar)
//{
//	ERESULT luResult;
//
//	bool lbSet = false;
//	float lfPosition = -mlDocToView0PositionY;
//	//�ж�
//	if(npChar->CharIn == VK_UP)
//	{
//		if(mpVScrollBar->GetIterator()->IsVisible()
//			&& mpElementVector.Size() != 0 
//			)
//		{
//			lfPosition += mlDocumentHeight/(mpElementVector.Size());
//			lbSet = true;
//		}
//	}
//	else if(npChar->CharIn == VK_DOWN)
//	{
//		if(mpVScrollBar->GetIterator()->IsVisible()
//			&& mpElementVector.Size() != 0
//			)
//		{
//			lfPosition -=  mlDocumentHeight/(mpElementVector.Size());
//			lbSet = true;
//		}
//	}
//	if(lbSet)
//	{	
//
//		if(lfPosition <= -(mlDocumentHeight - mpIterator->GetSizeY()))
//			lfPosition = -(mlDocumentHeight - mpIterator->GetSizeY());
//		if(lfPosition > 0.0f)
//			lfPosition = 0.0f;
//
//		mlDocToView0PositionY = -lfPosition;
//		EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(
//			mpIterator,
//			EACT_LIST_DOCSCROLL,
//			&mlDocToView0PositionY,
//			sizeof(float),NULL,0);
//	}
//	return ERESULT_SUCCESS;
//}

int CEvList::CheckPos(D2D1_POINT_2F nPos)
{
	int lnIndex = -1;
	do 
	{
		D2D1_RECT_F lRect;
		//��check��㣬�����Ǹ�element��
		for(int i = 0;i < mpElementVector.Size();i++)
		{			
			mpElementVector[i]->GetRect(lRect);
			if(nPos.x > lRect.left && nPos.x < lRect.right
				&& nPos.y > lRect.top && nPos.y < lRect.bottom)
			{
				lnIndex = i;
				break;
			}
		}

		//check �Ƿ������°벿�֣�����°벿�֣���Index+1
		if( lnIndex != -1 && nPos.y > lRect.top + (lRect.bottom - lRect.top)/2)
			lnIndex += 1;

	} while (false);
	return lnIndex;
}

////��ָ�����ƶ�������ʾ����
void CEvList::ShowByIndex(LONG nlIndex)
{
	IEinkuiIterator* lpItem = NULL;

	do 
	{
		if(nlIndex < 0 || nlIndex >= mpElementVector.Size())
			break;	//���Ϸ������

		lpItem = mpElementVector.GetEntry(nlIndex);
		BREAK_ON_NULL(lpItem);

		if(lpItem->GetPositionX() >= 0.0f 
			&& lpItem->GetPositionY() >= 0.0f 
			&& (lpItem->GetPositionX()+lpItem->GetSizeX()) <= mpIterator->GetSizeX()
			&& (lpItem->GetPositionY()+lpItem->GetSizeY()) <= mpIterator->GetSizeY())
			break;	//�Ѿ�����ʾ��������

		D2D1_POINT_2F ldOldPos = lpItem->GetPosition();
		D2D1_POINT_2F ldNewPos = ldOldPos;
		if (ldOldPos.y < 0.0f)
		{
			//����ʾ��������
			ldNewPos.y = 0.0f;
		}
		else
		{
			//����ʾ��������
			ldNewPos.y = mpIterator->GetSizeY() - lpItem->GetSizeY();
		}

		
		mlDocToView0PositionY -= (ldNewPos.y - ldOldPos.y);

		//���¶�λԪ��λ��
		CalcElementPosition();

		//��Ҫ���ù������Ļ���λ��
		if(mpVScrollBar)
			EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(mpVScrollBar->GetIterator(),
			EACT_SCROLLBAR_VSCROLL_SET_SLIDER_POSTION,&mlDocToView0PositionY,sizeof(FLOAT),NULL,0);

	} while (false);
}

//���¶�λλ��
void CEvList::Recaculate(void)
{
	CalcElementPosition();
	CheckVScrollBarNeedToShow();
	if(mpVScrollBar )
	{
		if(mpVScrollBar->GetIterator()->IsEnable())
		{
			//����Ƿ���Ҫ����Postionλ�ã���Ϊ�䶯��ԭ�е�λ�ó����˻�����Χ
			if( ( mlDocToView0PositionY + mpIterator->GetSizeY()) > mlDocumentHeight )
				mlDocToView0PositionY = mlDocumentHeight - mpIterator->GetSizeY();	
		}
		else
			mlDocToView0PositionY = 0.0f;
		EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(mpVScrollBar->GetIterator(),
			EACT_SCROLLBAR_VSCROLL_SET_SLIDER_POSTION,&mlDocToView0PositionY,sizeof(FLOAT),NULL,0);

	}
	CalcElementPosition();
	//�践�ظ������ڣ������С�ĸı�
	//SendMessageToParent(EACT_LIST_LAYOUT_CHANGE,NULL);
	EinkuiGetSystem()->GetElementManager()->SimpleSendMessage(mpIterator->GetParent(),
		EACT_LIST_LAYOUT_CHANGE,NULL,0,NULL,0);
}