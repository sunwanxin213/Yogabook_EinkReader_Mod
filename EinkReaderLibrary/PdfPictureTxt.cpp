/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "StdAfx.h"
#include "PdfPictureTxt.h"
#include "XCtl.h"
#include "stdio.h"
#include "cmmStrHandle.h"
#include "cmmPath.h"
#include "MsgDefine.h"

DEFINE_BUILTIN_NAME(PdfPictureTxt)

CPdfPictureTxt::CPdfPictureTxt(void)
{
	mulPageCount = 0;

	pdfDoc = NULL;
	pdfCrtPage = NULL;
	pdfCrtPage2 = NULL;
	pdfImage = NULL;
	pdfImage2 = NULL;
	mpElBitmap = NULL;
	duopageBuf = NULL;
	landScope = true;
	duopageMode = false;
	pageGap = 0;// 20;
	pageNo = 0;

	auto libModule = LoadLibrary(L"Edpdf.dll");
	if (libModule != NULL)
	{
		auto proc = (GetModule_Proc)GetProcAddress(libModule, "EdGetModule");
		pdfModule = proc();
	}
}


CPdfPictureTxt::~CPdfPictureTxt(void)
{
	CloseFile(false);
}

//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
ERESULT CPdfPictureTxt::OnElementCreate(IEinkuiIterator* npIterator)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		if(CXuiElement::OnElementCreate(npIterator) != ERESULT_SUCCESS)
			break;

		mpIterator->ModifyStyles(EITR_STYLE_POPUP);
		
		lResult = ERESULT_SUCCESS;

	}while(false);

	return lResult;
}

ULONG CPdfPictureTxt::InitOnCreate(
	IN IEinkuiIterator* npParent,	// ������ָ��
	IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
	IN ULONG nuEID	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;
	ICfKey* lpSubKey = NULL;

	do 
	{
		//���ȵ��û���
		leResult = 	CXuiElement::InitOnCreate(npParent,npTemplete,nuEID);
		if(leResult != ERESULT_SUCCESS)
			break;


		//��ȡ������
		/*mpIterBtShowTip = mpIterator->GetSubElementByID(PP_BT_SHOW_TIP);
		BREAK_ON_NULL(mpIterBtShowTip);*/
		

		// ������ˢ
		mpXuiBrush = EinkuiGetSystem()->CreateBrush(XuiSolidBrush, D2D1::ColorF(0.0f, 0.0f, 0.0f));
		BREAK_ON_NULL(mpXuiBrush);

		mpXuiBrush->SetStrokeWidth(2.0f);
		mpXuiBrush->SetStrokeType(
			D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_MITER,
				10.0f,
				D2D1_DASH_STYLE_DOT,
				0.0f),
			0,
			NULL);

		leResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpSubKey);

	// ��ϵͳע����Ҫ�յ�����Ϣ
	return leResult;
}

//��ť�����¼�
ERESULT CPdfPictureTxt::OnCtlButtonClick(IEinkuiIterator* npSender)
{
	ERESULT lResult = ERESULT_UNSUCCESSFUL;

	do
	{
		ULONG llBtnID = npSender->GetID();
		switch (llBtnID)
		{
		case PP_BT_SHOW_TIP:
		{
			PostMessageToParent(EEVT_ER_SHOW_TIP, CExMessage::DataInvalid);
			break;
		}
		default:
			break;
		}


		lResult = ERESULT_SUCCESS;
	} while (false);

	return lResult;
}


////��Ϣ������
//ERESULT CPdfPictureTxt::ParseMessage(IEinkuiMessage* npMsg)
//{
//	ERESULT luResult = ERESULT_UNEXPECTED_MESSAGE;
//
//	switch (npMsg->GetMessageID())
//	{
//	case EMSG_MODAL_ENTER:
//	{
//		//// ����Ҫ�����ĶԻ���
//		//mpIterator->SetVisible(true);
//		luResult = ERESULT_SUCCESS;
//		break;
//	}
//	default:
//		luResult = ERESULT_NOT_SET;
//		break;
//	}
//
//	if (luResult == ERESULT_NOT_SET)
//	{
//		luResult = CXuiElement::ParseMessage(npMsg); // ���û����ͬ��������ע�⣺һ��Ҫ��������ֱ�ӻ���
//	}
//
//	return luResult;
//}

//��ʱ��
void CPdfPictureTxt::OnTimer(
	PSTEMS_TIMER npStatus
	)
{

}

//Ԫ�زο��ߴ緢���仯
ERESULT CPdfPictureTxt::OnElementResized(D2D1_SIZE_F nNewSize)
{
	//CExMessage::SendMessage(mpIterBtFull, mpIterator, EACT_BUTTON_SET_ACTION_RECT, nNewSize);
	////mpIterLineOne->SetSize(nNewSize.width, mpIterLineOne->GetSize().height);

	//mpIterBtOk->SetPosition(nNewSize.width - 85, mpIterBtOk->GetPositionY());
	if (nNewSize.width == 0 || nNewSize.height == 0)
		return ERESULT_SUCCESS;

	ED_SIZE viewPort;
	viewPort.x = CExFloat::ToLong(nNewSize.width);
	viewPort.y = CExFloat::ToLong(nNewSize.height);

	mViewCtl.SetViewPort(viewPort);

	GoToPage(pageNo);

	return ERESULT_SUCCESS;
}

//����ҪԤ����ͼƬ,�ͽ�����Ϣ�Ķ���
bool CPdfPictureTxt::OpenFile(wchar_t* npszPath)
{
	bool lbRet = false;

	//if (pdfDoc != NULL)
	//	EinkuiGetSystem()->ExitXui(); // 4debug

	CloseFile(false);

	do 
	{
		BREAK_ON_NULL(npszPath);

		BREAK_ON_NULL(pdfModule);

		auto error = pdfModule->OpenDocument(npszPath, &pdfDoc);
		if (ERR_FAILED(error))
			break;

		//�ļ���ҳ��
		mulPageCount = pdfDoc->GetPageCount();

		lbRet = true;

	} while (false);

	return lbRet;
}

void CPdfPictureTxt::CloseFile(bool nbUpdateView)
{
	CMM_SAFE_RELEASE(pdfImage);
	CMM_SAFE_RELEASE(pdfImage2);
	CMM_SAFE_RELEASE(pdfCrtPage);
	CMM_SAFE_RELEASE(pdfCrtPage2);
	CMM_SAFE_RELEASE(pdfDoc);
	CMM_SAFE_RELEASE(mpElBitmap);

	if (nbUpdateView != false)
	{
		EinkuiGetSystem()->ClearEinkBuffer();
		EinkuiGetSystem()->UpdateView();
	}
}

//ҳ����ת
bool CPdfPictureTxt::GoToPage(ULONG nulPageNumber)
{
	bool lbRet = false;
	bool duopageOK = false;
	ULONG duopageStart;

	do
	{
		if (pdfDoc == NULL)
			break; // �ĵ�δ��

		if(nulPageNumber <= 0 || nulPageNumber > mulPageCount)
			break; //ҳ����Ч

		pageNo = 0;
		CMM_SAFE_RELEASE(pdfCrtPage);//�ͷ�
		CMM_SAFE_RELEASE(pdfCrtPage2);

		duopageStart = ((nulPageNumber - 1) & 0xFFFFFFFE) + 1;
		// �����˫ҳ��ʾģʽ���������ҳ���䣬����ҳ
		if (landScope != false && duopageMode != false && duopageStart+1 <= mulPageCount)	// �����һҳ��ʼ����û����ҳ���ݹ�չ����
		{
			ED_SIZE sizeP1, sizeP2;
			IEdPage_ptr pageNo1, pageNo2;
			do 
			{
				pageNo1 = loadPage(duopageStart-1, sizeP1);
				if (pageNo1 == NULL)
					break;
				pageNo2 = loadPage(duopageStart, sizeP2);
				if(pageNo2 == NULL)
					break;

				//if (sizeP1.x + sizeP2.x + 40> mViewCtl.GetViewPort().x)
					//break; // ��ҳƴ�Ͽ�ȴ�����Ļ�ߴ磬��ǰ�����֧��˫ҳ��ʾ

				//if(sizeP1.x != sizeP2.x || sizeP1.y != sizeP2.y)
				//	break;// ��ҳ�Ļ����Ŵ����������ߴ������ȫһ�������в�ͬ����ƴ�ϳ�˫ҳ

				duopageOK = true;
			} while (false);

			if (pageNo1 == NULL || pageNo2 == NULL)
			{
				CMM_SAFE_RELEASE(pageNo1);
				CMM_SAFE_RELEASE(pageNo2);
				break;// ���ڴ���
			}

			if (duopageOK != false)
			{
				sizeP1.x += sizeP2.x;
				pdfCrtPage = pageNo1;
				pdfCrtPage2 = pageNo2;
			}
			else
			{
				if (nulPageNumber == duopageStart)
				{
					pdfCrtPage = pageNo1;
					CMM_SAFE_RELEASE(pageNo2);
				}
				else
				{
					pdfCrtPage = pageNo2;
					CMM_SAFE_RELEASE(pageNo1);
				}

			}

			mViewCtl.SetImageInit(sizeP1);
			mViewCtl.SetCenterGap(pageGap);
		}
		else
		{
			ED_SIZE sizeInit;
			pdfCrtPage = loadPage(nulPageNumber - 1,sizeInit);
			if(pdfCrtPage == NULL)
				break;
			mViewCtl.SetImageInit(sizeInit);
			mViewCtl.SetCenterGap(0.0f);
		}

		pageNo = nulPageNumber;
		lbRet = RenderPages();
		if (lbRet != false)
		{
			EinkuiGetSystem()->ClearEinkBuffer();
			EinkuiGetSystem()->UpdateView();
		}
			

	} while (false);

	return lbRet;
}

bool CPdfPictureTxt::PageFoward(bool foward)
{
#ifdef _AX_TEST_MEMLEAK
	if (foward == false)
	{
		EinkuiGetSystem()->ExitXui();
		return false;
	}
#endif//_AX_TEST_MEMLEAK

	if (pdfCrtPage2 == NULL)
	{
		if (foward == false)
			return GoToPage(pageNo - 1);
		else
			return GoToPage(pageNo + 1);
	}
	else
	{
		auto pageStart = ((pageNo - 1) & 0xFFFFFFFE) + 1;
		if (foward == false)
			return GoToPage(pageStart - 2);
		else
			return GoToPage(pageStart + 2);
	}
}

ULONG CPdfPictureTxt::GetCurrentPageNumber(ULONG& secondPage)
{
	if (pdfCrtPage == NULL)
		return 0;

	if (pdfCrtPage2 != NULL)
		secondPage = pdfCrtPage2->GetPageIndex() + 1;
	else
		secondPage = 0;

	return pdfCrtPage->GetPageIndex() + 1;
}

//��������
//rMove4������0��ʾ�����ƶ��ˣ�1��ʾ�����ƶ�
bool CPdfPictureTxt::SetScaleRatio(float nfScale, OUT RECT& rMove)
{
	bool lbRet = false;

	do
	{
		mViewCtl.SetUserRatio(nfScale);

		if (pdfImage == NULL)
			break;

		CalcMovalbe(rMove);

		lbRet = RenderPages();
		if (lbRet != false)
		{
			EinkuiGetSystem()->ClearEinkBuffer();
			EinkuiGetSystem()->UpdateView();
		}
			

	} while (false);

	return lbRet;
}

//�ƶ�ҳ��
//niXOffset,niYOffset������ʾ�������ƶ���������ʾ�������ƶ�
//rMove4������0��ʾ�����ƶ��ˣ�1��ʾ�����ƶ�
bool CPdfPictureTxt::MovePage(int niXOffset, int niYOffset, OUT RECT& rMove)
{
	bool lbRet = false;

	do
	{
		ED_POINT moveOffset;
		moveOffset.x = (float32)niXOffset;
		moveOffset.y = (float32)niYOffset;
		mViewCtl.Move(moveOffset);

		CalcMovalbe(rMove);

		EinkuiGetSystem()->ClearEinkBuffer();
		EinkuiGetSystem()->UpdateView(true);
		lbRet = true;

	} while (false);

	return lbRet;

}


//��ȡ��ҳ��
ULONG CPdfPictureTxt::GetPageCount(void)
{
	return mulPageCount;
}

//��Ļ������ת
void CPdfPictureTxt::SetRotation(ULONG nulRotation)
{
	do 
	{
		//mpIterPicture->SetPosition(mdOldPos);
		//

		if (nulRotation == GIR_NONE || nulRotation == GIR_180)
		{
			landScope = true;
		}
		else if (nulRotation == GIR_90 || nulRotation == GIR_270)
		{
			landScope = false;
		}

		if (pdfCrtPage != NULL)
			GoToPage(pageNo);

	} while (false);

}

void CPdfPictureTxt::EnableDuopageView(bool nbEnable)
{
	bool32 changed = (duopageMode == false && nbEnable != false || duopageMode != false && nbEnable == false);

	duopageMode = nbEnable;

	if (changed == false)
		return;

	GoToPage(pageNo);
	//else
	//	GoToPage(1);
}

bool CPdfPictureTxt::CopyToClipboard(const D2D1_RECT_F& rRegion)
{
	bin_ptr bufToCopy = NULL;
	D2D1_RECT_F intersectRect;
	bool resultOK = false;
	HBITMAP bitmapHandle = NULL;

	do 
	{
		if (mpElBitmap == NULL || CExRect::Intersect(rRegion,mRecentDst,intersectRect) == false)
			break;

		int32 imageWidth = CExFloat::ToLong(ED_RECT_WIDTH(intersectRect));
		int32 imageHeight = CExFloat::ToLong(ED_RECT_HEIGHT(intersectRect));
		if (imageWidth == 0 || imageHeight == 0)
			break;

		bufToCopy = (bin_ptr)HeapAlloc(GetProcessHeap(),0,imageWidth*imageHeight*4);
		if (bufToCopy == NULL)
			break;

		intersectRect.left = CExFloat::Floor(intersectRect.left - mRecentDst.left + mRecentSrc.left);
		intersectRect.right = intersectRect.left + (float32)imageWidth;
		intersectRect.top = CExFloat::Floor(intersectRect.top - mRecentDst.top + mRecentSrc.top);
		intersectRect.bottom = intersectRect.top + (float32)imageHeight;

		if (mpElBitmap->GetBitmapBuffer(intersectRect,bufToCopy)==false)
			break;

		bitmapHandle = CreateCompatibleBitmap(GetDC(NULL), imageWidth,imageHeight);
		if(bitmapHandle == NULL)
			break;

		if(SetBitmapBits(bitmapHandle, imageWidth*imageHeight * 4,bufToCopy)<=0)
			break;

		if(OpenClipboard(NULL)==FALSE)
			break;
		EmptyClipboard();

		SetClipboardData(CF_BITMAP,bitmapHandle);

		CloseClipboard();

		resultOK = true;
	} while (false);

	if (bitmapHandle != NULL)
		DeleteObject(bitmapHandle);

	if(bufToCopy != NULL)
		HeapFree(GetProcessHeap(), 0, bufToCopy);


	return resultOK;
}

float CPdfPictureTxt::GetFatRatio(void)
{
	return mViewCtl.GetFatRatio() / mViewCtl.GetBaseRatio();
}

//֪ͨԪ�ء���ʾ/���ء������ı�
ERESULT CPdfPictureTxt::OnElementShow(bool nbIsShow)
{
	//EiSetHomebarStatus(nbIsShow == false ? GI_HOMEBAR_SHOW : GI_HOMEBAR_HIDE);

	return ERESULT_SUCCESS;
}


IEdPage_ptr CPdfPictureTxt::loadPage(int32 pageNo, ED_SIZE& sizeInit)
{
	IEdPage_ptr pageGot = NULL;
	ED_RECTF box = { 0,0,1,1 };
	IEdPage_ptr pageReturn = NULL;

	do
	{
		pageGot = pdfDoc->GetPage(pageNo);
		if (pageGot == NULL)
			break;

		if(pageGot->GetMediaBox(&box)==false)
			break;

		sizeInit.x = CExFloat::ToLong(ED_RECT_WIDTH(box));
		sizeInit.y = CExFloat::ToLong(ED_RECT_HEIGHT(box));

		pageReturn = pageGot;

	} while (false);

	return pageReturn;
}

bool CPdfPictureTxt::RenderPages(void)
{
	bool lbRet = false;
	bin_ptr imageBuffer = NULL;

	do
	{
		if (pdfCrtPage == NULL)
			break;

		CMM_SAFE_RELEASE(pdfImage);
		CMM_SAFE_RELEASE(pdfImage2);
		CMM_SAFE_RELEASE(mpElBitmap);
		if (duopageBuf != NULL)
		{
			HeapFree(GetProcessHeap(), 0, duopageBuf);
			duopageBuf = NULL;
		}


		pdfImage = pdfCrtPage->Render(mViewCtl.GetRealRatio(), true);
		if (pdfImage == NULL)
			break;

		if (pdfCrtPage2 != NULL)
		{
			pdfImage2 = pdfCrtPage2->Render(mViewCtl.GetRealRatio(), true);
			if (pdfImage2 == NULL)
				break;

			imageSize.x = (pdfImage->GetWidth() <= pdfImage2->GetWidth() ? pdfImage->GetWidth() : pdfImage2->GetWidth());
			imageSize.y = (pdfImage->GetHeight() <= pdfImage2->GetHeight() ? pdfImage->GetHeight() : pdfImage2->GetHeight());

			imageBuffer = (bin_ptr)HeapAlloc(GetProcessHeap(), 0, (imageSize.x * 2 + pageGap) * 4 * imageSize.y);
			if(imageBuffer == NULL)
				break;

			bin_ptr src1, src2, dst;
			int32 stride = imageSize.x * 4;
			dst = imageBuffer;
			src1 = pdfImage->GetBuffer();
			src2 = pdfImage2->GetBuffer();

			for (int32 i = 0; i < imageSize.y; i++)
			{
				RtlCopyMemory(dst, src1, stride);
				dst += stride;
				if (pageGap >= 1.0f)
				{
	//				RtlZeroMemory(dst, pageGap * 4);
					memset(dst, 0xFF, pageGap * 4);
					dst += pageGap * 4;
				}
				RtlCopyMemory(dst, src2, stride);
				dst += stride;
				src1 += stride;
				src2 += stride;
			}
			imageSize.x = imageSize.x*2 + pageGap;

			duopageBuf = imageBuffer;
		}
		else
		{
			imageBuffer = pdfImage->GetBuffer();

			imageSize.x = pdfImage->GetWidth();
			imageSize.y = pdfImage->GetHeight();
		}

		mViewCtl.AdjustImageRealSize(imageSize);


		mpElBitmap = EinkuiGetSystem()->GetAllocator()->CreateBitmapFromMemory(imageSize.x,imageSize.y, 4,imageSize.x * 4, imageBuffer);
		if (mpElBitmap == NULL)
			break;

		lbRet = true;

	} while (false);

	return lbRet;
}

void CPdfPictureTxt::CalcMovalbe(OUT RECT& rMove)
{
	ED_RECT dstRect;
	ED_RECT srcRect;
	UCHAR edgeImpact;

	if (mViewCtl.GetViewMapArea(dstRect, srcRect,&edgeImpact) == false)
	{
		rMove.left = rMove.right = rMove.top = rMove.bottom = 0;
	}
	else
	{
		rMove.left = ((edgeImpact & 1) > 0);
		rMove.right = ((edgeImpact & 2) > 0);
		rMove.top = ((edgeImpact & 4) > 0);
		rMove.bottom = ((edgeImpact & 8) > 0);
	}
}

//����
ERESULT CPdfPictureTxt::OnPaint(IEinkuiPaintBoard* npPaintBoard)
{
	ERESULT leResult = ERESULT_UNSUCCESSFUL;
	ED_RECT dstRect;
	ED_RECT srcRect;

	do
	{
		BREAK_ON_NULL(npPaintBoard);

		leResult = CXuiElement::OnPaint(npPaintBoard);
		if (ERESULT_FAILED(leResult))
			break;

		if (mpElBitmap != NULL && mViewCtl.GetViewMapArea(dstRect, srcRect) != false)
		{
			mRecentDst.left = (float32)dstRect.left;
			mRecentDst.right = (float32)dstRect.right;
			mRecentDst.top = (float32)dstRect.top;
			mRecentDst.bottom = (float32)dstRect.bottom;
			mRecentSrc.left = (float32)srcRect.left;
			mRecentSrc.right = (float32)srcRect.right;
			mRecentSrc.top = (float32)srcRect.top;
			mRecentSrc.bottom = (float32)srcRect.bottom;

			npPaintBoard->DrawBitmap(mRecentDst,mRecentSrc,mpElBitmap, ESPB_DRAWBMP_LINEAR);
		}

		leResult = ERESULT_SUCCESS;
	} while (false);

	return leResult;
}

void CPdfPictureTxt::GetRectOfViewportOnPage(D2D1_SIZE_F& nrImageSize, D2D1_RECT_F& nrViewPort)
{
	nrImageSize.width = (FLOAT)imageSize.x;
	nrImageSize.height = (FLOAT)imageSize.y;

	nrViewPort = mRecentSrc;
}

