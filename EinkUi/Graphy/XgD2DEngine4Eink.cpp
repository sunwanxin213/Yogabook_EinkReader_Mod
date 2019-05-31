/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "stdafx.h"

#include "CommonHeader.h"

#include "XgD2DEngine4Eink.h"
#include <dwmapi.h>
#include <ppl.h>  
#include "EiUpdate.h"

using namespace concurrency;


DEFINE_BUILTIN_NAME(CXD2dEngine)



CXD2dEngine::CXD2dEngine()
{
	// ��ʼ���豸�޹���Դ
	mpD2dFactory = NULL;
	mpWicFactory = NULL;
	mpWriteFactory = NULL;
	mpDbgTextFormat = NULL;

	mpEinkBuf = NULL;
	mpCustomDraw = NULL;

	// ��ʼ���豸�����Դ
	mpTarget2D = NULL;
	mpTargetBitmap = NULL;
	mpDbgTextBrush = NULL;
	mpFogBrush = NULL;

	// ��ͼ������
	mpElementManager = NULL;
	mpMessage = NULL;
	mfFpsIndx = 0.0f;
	muRenderCount = 0;
	mlRenderStep = CEinkuiSystem::eRenderStop;
	mpToCapture = NULL;
	mdBackColor = D2D1::ColorF(1.0f,1.0f,1.0f,1.0f);

	mpModalTrunk = NULL;

	mbStopPainting = false;// true;

	mdScalMatrixToCapture = mdIdentyMatrix = D2D1::Matrix3x2F::Identity();

	mlResetEinkBuf = 0;
	mpEinkUpdatingCallBack = NULL;
	mxUpdatingNumber = 0;
	mpEinkUpdatingContext = NULL;
}


CXD2dEngine::~CXD2dEngine()
{

	ReleaseDeviceResource(false);

	CMM_SAFE_RELEASE(mpD2dFactory);
	CMM_SAFE_RELEASE(mpWicFactory);
	CMM_SAFE_RELEASE(mpWriteFactory);
	CMM_SAFE_RELEASE(mpDbgTextFormat);

	CMM_SAFE_RELEASE(mpMessage);

	if (mpEinkBuf != NULL)
		EiReleaseDrawBuffer(mpEinkBuf);
}

//////////////////////////////////////////////////////////////////////////
// ������ͼ��Դ
//////////////////////////////////////////////////////////////////////////

// ������һ��ɫ�Ļ�ˢ
IEinkuiBrush* __stdcall CXD2dEngine::CreateBrush(
	XuiBrushType niBrushType,
	D2D1_COLOR_F noColor
	)
{
	IEinkuiBrush* lpBrush = NULL;

	do 
	{
		lpBrush = CXD2DBrush::CreateInstance(niBrushType, noColor);
		BREAK_ON_NULL(lpBrush);

		CEinkuiSystem::gpXuiSystem->GetBrushList().RegisteBrush(lpBrush);

	} while (false);

	return lpBrush;

}

// ���仭ˢʱ����Ҫ��������ɫ��
IEinkuiBrush* __stdcall CXD2dEngine::CreateBrush(
	XuiBrushType niBrushType, 
	D2D1_GRADIENT_STOP* npGradientStop, 
	ULONG nuCount, 
	D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES ndLinearGradientBrushProperties
	)
{
	IEinkuiBrush* lpBrush = NULL;

	do 
	{
		lpBrush = CXD2DBrush::CreateInstance(niBrushType, npGradientStop, nuCount, ndLinearGradientBrushProperties);
		BREAK_ON_NULL(lpBrush);

		CEinkuiSystem::gpXuiSystem->GetBrushList().RegisteBrush(lpBrush);

	} while (false);

	return lpBrush;

}



//////////////////////////////////////////////////////////////////////////
// ʵ����������
//////////////////////////////////////////////////////////////////////////

ULONG CXD2dEngine::InitOnCreate(ULONG Width, ULONG Height)
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;

	mpElementManager = dynamic_cast<CXelManager*>(CEinkuiSystem::gpXuiSystem->GetElementManager());

	do 
	{
		muFixedW = Width;
		muFixedH = Height;
		RelocationPainboard();

		//�����豸�޹���Դ
		luResult = CreateIndependentResource();
		BREAK_ON_FAILED(luResult);

		//�����豸�����Դ
		luResult = CreateDeviceResource();
		BREAK_ON_FAILED(luResult);

		// ��ʼ֡��ͳ��
		muLastTick = GetCurrentTickCount();

		luResult = ERESULT_SUCCESS;
	} while (false);

	
	return luResult;

}

// �����豸�޹���Դ
ERESULT CXD2dEngine::CreateIndependentResource()
{
	static const WCHAR msc_fontName[] = L"Tahoma";
	static const FLOAT msc_fontSize = 13;
	HRESULT hr;

	do 
	{
		// Create D2D factory
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED/*D2D1_FACTORY_TYPE_SINGLE_THREADED*/, &mpD2dFactory);
		if(FAILED(hr))
			break;

		// Create a WIC factory
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWICImagingFactory,
			reinterpret_cast<void **>(&mpWicFactory)
			);

		// Create DWrite factory
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(mpWriteFactory),
			reinterpret_cast<IUnknown **>(&mpWriteFactory)
			);
		if(FAILED(hr))
			break;

		hr = mpWriteFactory->CreateTextFormat(
			msc_fontName,
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			msc_fontSize,
			L"", 
			&mpDbgTextFormat
			);

	} while (false);

	return SUCCEEDED(hr)?ERESULT_SUCCESS:ERESULT_UNSUCCESSFUL;

}

// �����豸�����Դ
ERESULT CXD2dEngine::CreateDeviceResource()
{
	if (mpTarget2D != NULL)
		return ERESULT_SUCCESS;

	HRESULT hr = S_OK;
	ERESULT luResult = ERESULT_SUCCESS;
	D2D1_RENDER_TARGET_PROPERTIES ld2DTargetProt;
	HDC lhDefaultDC = NULL;
	BITMAPINFO BmpInfo;
	ID2D1DCRenderTarget* lpDcTarget;

	do
	{
		lhDefaultDC = ::GetDC(NULL);
		BREAK_ON_NULL(lhDefaultDC);

		//��������λͼ
		BmpInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
		BmpInfo.bmiHeader.biWidth = muEinkPanelW;
		BmpInfo.bmiHeader.biHeight = -(int)muEinkPanelH;
		BmpInfo.bmiHeader.biPlanes = 1;
		BmpInfo.bmiHeader.biBitCount = 32;
		BmpInfo.bmiHeader.biCompression = BI_RGB;
		BmpInfo.bmiHeader.biSizeImage = 0;
		BmpInfo.bmiHeader.biXPelsPerMeter = 1;
		BmpInfo.bmiHeader.biYPelsPerMeter = 1;
		BmpInfo.bmiHeader.biClrUsed = 0;
		BmpInfo.bmiHeader.biClrImportant = 0;
		mhForeBmp = CreateDIBSection(lhDefaultDC, &BmpInfo, DIB_RGB_COLORS, (void**)&mpForeBuffer \
			, NULL, 0);
		BREAK_ON_NULL(mhForeBmp);

		//��������DC
		mhForeDc = CreateCompatibleDC(lhDefaultDC);
		BREAK_ON_NULL(mhForeDc);

		//�滻λͼ����
		mhOldForeBmp = (HBITMAP)SelectObject(mhForeDc, mhForeBmp);
		BREAK_ON_NULL(mhOldForeBmp);

		//����D2d target
		ld2DTargetProt = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(
				DXGI_FORMAT_B8G8R8A8_UNORM,
				D2D1_ALPHA_MODE_PREMULTIPLIED),
			0,
			0,
			D2D1_RENDER_TARGET_USAGE_FORCE_BITMAP_REMOTING,
			D2D1_FEATURE_LEVEL_DEFAULT
		);

		hr = mpD2dFactory->CreateDCRenderTarget(&ld2DTargetProt, &lpDcTarget);
		if (FAILED(hr))
			break;

		RECT rc = { 0,0,(LONG)muEinkPanelW,(LONG)muEinkPanelH};

		lpDcTarget->BindDC(mhForeDc, &rc);

		mpTarget2D = lpDcTarget;

		//����2D�豸�����Դ
		luResult = CreateD2dResource();
		if (ERESULT_FAILED(luResult))
			break;

		hr = S_OK;

	} while (false);

	if (lhDefaultDC != NULL)
		ReleaseDC(NULL, lhDefaultDC);

	return SUCCEEDED(hr) ? ERESULT_SUCCESS : ERESULT_UNSUCCESSFUL;
}

// ����D2D�����Դ
ERESULT CXD2dEngine::CreateD2dResource()
{
	HRESULT hr = S_OK;


	hr = mpTarget2D->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Blue,0.6f),
		&mpDbgTextBrush
		);

	hr = mpTarget2D->CreateSolidColorBrush(
		D2D1::ColorF(0.0f,0.0f,0.0f,0.3f),
		&mpFogBrush
		);

	return ERESULT_SUCCESS;// SUCCEEDED(hr)?ERESULT_SUCCESS:ERESULT_UNSUCCESSFUL; ����������ˢ����Ҫ�����践�ش���
}

// �����ͷ���Դ
void CXD2dEngine::ReleaseDeviceResource(bool nbBroadcastToElement/* =true */)
{
	// ���ȸ�����Element������Ϣ��֪ͨ�����ͷ��豸�����Դ
	if(nbBroadcastToElement != false && mpTarget2D != NULL && mpElementManager != NULL) // mpTarget2D != NULL��ֹ��η����ͷ���Ϣ
		mpElementManager->EnumAllElement(
		false,
		this,
		(ERESULT (__stdcall IBaseObject::*)(IEinkuiIterator* npRecipient))&CXD2dEngine::EnterForDiscardDeviceRes,
		(ERESULT (__stdcall IBaseObject::*)(IEinkuiIterator* npRecipient))&CXD2dEngine::LeaveForDiscardDeviceRes
		);

	// �ͷ�λͼ���������е��Դ�λͼ
	CEinkuiSystem::gpXuiSystem->GetBitmapList().ReleaseDeviceResource();
	// �ͷ�ȫ��Brush����
	CEinkuiSystem::gpXuiSystem->GetBrushList().ReleaseDeviceResource();


	// �ͷ�����Device�����Դ
	CMM_SAFE_RELEASE(mpDbgTextBrush);
	CMM_SAFE_RELEASE(mpFogBrush);
	CMM_SAFE_RELEASE(mpTarget2D);
	CMM_SAFE_RELEASE(mpTargetBitmap);

	if (mhForeDc != NULL)
	{
		if (mhOldForeBmp != NULL)
			SelectObject(mhForeDc, mhOldForeBmp);
		DeleteDC(mhForeDc);
		mhForeDc = NULL;
	}
	if (mhForeBmp != NULL)
	{
		DeleteObject(mhForeBmp);
		mhForeBmp = NULL;
	}
}


//////////////////////////////////////////////////////////////////////////
// ��Ҫʵ�ֵĻ���ӿ�
//////////////////////////////////////////////////////////////////////////

ERESULT __stdcall CXD2dEngine::DrawBitmap( IN const D2D1_RECT_F& rDestRect, IN const D2D1_RECT_F& rSrcRect, IN IEinkuiBitmap* npBitmap, IN ULONG nuMethod,IN float nfAlpha)
{

	ID2D1Bitmap* lpBmp=NULL;
	ERESULT luResult = ERESULT_UNSUCCESSFUL;
	CXD2dBitmap* lpXuiD2DBitmap = dynamic_cast<CXD2dBitmap*>(npBitmap);


	do 
	{
		BREAK_ON_NULL(lpXuiD2DBitmap);

		// ��ʼ��λͼ����
		luResult = lpXuiD2DBitmap->InitBitmap(mpWicFactory, mpD2dFactory, mpTarget2D);
		BREAK_ON_FAILED(luResult);

		luResult = lpXuiD2DBitmap->GetD2DObject(mpTarget2D, &lpBmp);
		if(luResult != ERESULT_SUCCESS)
			break;


		if (nuMethod == ESPB_DRAWBMP_EXTEND && 
			((rDestRect.right - rDestRect.left - 1.0f > rSrcRect.right - rSrcRect.left) || 
			(rDestRect.bottom - rDestRect.top - 1.0f > rSrcRect.bottom - rSrcRect.top)))
		{
			//��չ�߷�ʽ
			//���Ҫ��չ�Ķ���ֻ�п���ĳһ���ԭͼ����չ�㷨�ͻ���� 2018-5-2 jaryee ????
			FLOAT lfExtendLineX = CExFloat::Round((FLOAT)npBitmap->GetExtnedLineX());	//��ȡ��չ�����꣬���Ϊ-1��ʾʹ������
			FLOAT lfExtendLineY = CExFloat::Round((FLOAT)npBitmap->GetExtnedLineY());

			if(lfExtendLineX <= 0)	//�����չ������С��0����ͼƬ����Ϊ��չ��
				lfExtendLineX = CExFloat::Round((rSrcRect.right - rSrcRect.left) / 2.0f);
			if(lfExtendLineY <= 0)
				lfExtendLineY = CExFloat::Round((rSrcRect.bottom - rSrcRect.top) / 2.0f);

			D2D1_RECT_F lDestRect,lSrcRect;
			D2D1_SIZE_F lDestSize,lSrcSize;

			lDestSize.width = CExFloat::Round(rDestRect.right - rDestRect.left);	//����Ŀ���Դͼ�Ŀ�͸�
			lDestSize.height = CExFloat::Round(rDestRect.bottom - rDestRect.top);
			lSrcSize.width = CExFloat::Round(rSrcRect.right - rSrcRect.left);
			lSrcSize.height = CExFloat::Round(rSrcRect.bottom - rSrcRect.top);

			//////////////////////////////////////////////////////////////////////////
			//���Ͻ� ���ﲻ����Ϊ��չ�Ĳ�ͬ���仯
			lDestRect.left = rDestRect.left;
			lDestRect.right = lfExtendLineX + lDestRect.left;
			lDestRect.top = rDestRect.top;
			lDestRect.bottom = lfExtendLineY + lDestRect.top;

			lSrcRect.left = rSrcRect.left;
			lSrcRect.right = lfExtendLineX + rSrcRect.left;
			lSrcRect.top = rSrcRect.top;
			lSrcRect.bottom = lfExtendLineY + rSrcRect.top;
			
			if (nfAlpha >= 0.999f)
				mpTarget2D->DrawBitmap(lpBmp,&lDestRect,moRenderState.Top().mfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)ESPB_DRAWBMP_LINEAR,&lSrcRect);
			else
				mpTarget2D->DrawBitmap(lpBmp,&lDestRect,moRenderState.Top().mfAlpha * nfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)ESPB_DRAWBMP_LINEAR,&lSrcRect);

			//////////////////////////////////////////////////////////////////////////
			//���ϽǺ����Ͻ��м�Ĳ���
			if (lDestSize.width > lSrcSize.width)
			{
				//����Ǳ������Ҫִ������
				lDestRect.left = lDestRect.right;
				lDestRect.right = lDestSize.width - lSrcSize.width + lDestRect.left + 1.0f;
				//lDestRect.top = rDestRect.top;
				//lDestRect.bottom = lfExtendLineY;

				lSrcRect.left = lSrcRect.right;
				lSrcRect.right ++;
				lSrcRect.top = rSrcRect.top;
				lSrcRect.bottom = lfExtendLineY + rSrcRect.top;

				if (nfAlpha >= 0.999f)
					mpTarget2D->DrawBitmap(lpBmp,&lDestRect,moRenderState.Top().mfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)ESPB_DRAWBMP_LINEAR,&lSrcRect);
				else
					mpTarget2D->DrawBitmap(lpBmp,&lDestRect,moRenderState.Top().mfAlpha*nfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)ESPB_DRAWBMP_LINEAR,&lSrcRect);
			}


			//////////////////////////////////////////////////////////////////////////
			//���Ͻ�
			lDestRect.left = lDestRect.right;
			lDestRect.right = rDestRect.right;
			//lDestRect.top = rDestRect.top;
			//lDestRect.bottom = lfExtendLineY;

			lSrcRect.left = lSrcRect.right;
			lSrcRect.right = rSrcRect.right;
			lSrcRect.top = rSrcRect.top;
			lSrcRect.bottom = lfExtendLineY + rSrcRect.top;

			if (nfAlpha >= 0.999f)
				mpTarget2D->DrawBitmap(lpBmp,&lDestRect,moRenderState.Top().mfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)ESPB_DRAWBMP_LINEAR,&lSrcRect);
			else
				mpTarget2D->DrawBitmap(lpBmp,&lDestRect,moRenderState.Top().mfAlpha*nfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)ESPB_DRAWBMP_LINEAR,&lSrcRect);

			//////////////////////////////////////////////////////////////////////////
			if (lDestSize.height > lSrcSize.height)
			{
				//�����Ҫ��߲�ִ������
				//////////////////////////////////////////////////////////////////////////
				//���Ϻ������м䲿��
				lDestRect.left = rDestRect.left;
				lDestRect.right = lfExtendLineX + lDestRect.left;
				lDestRect.top = lDestRect.bottom;
				lDestRect.bottom = lDestSize.height - lSrcSize.height + lDestRect.top + 1.0f;

				lSrcRect.left = rSrcRect.left;
				lSrcRect.right = lfExtendLineX + rSrcRect.left;
				lSrcRect.top = lSrcRect.bottom;
				lSrcRect.bottom ++;

				if (nfAlpha >= 0.999f)
					mpTarget2D->DrawBitmap(lpBmp,&lDestRect,moRenderState.Top().mfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)ESPB_DRAWBMP_LINEAR,&lSrcRect);
				else
					mpTarget2D->DrawBitmap(lpBmp,&lDestRect,moRenderState.Top().mfAlpha*nfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)ESPB_DRAWBMP_LINEAR,&lSrcRect);

				//////////////////////////////////////////////////////////////////////////
				//�м�һ���
				if (lDestSize.width > lSrcSize.width && lDestSize.height > lSrcSize.height)
				{
					lDestRect.left = lDestRect.right;
					lDestRect.right = lDestRect.left + (lDestSize.width - lSrcSize.width) + 1.0f;
					//lDestRect.top = lfExtendLineY;
					//lDestRect.bottom = lfExtendLineY + (lDestSize.height - lSrcSize.height);

					lSrcRect.left = lfExtendLineX + rSrcRect.left;
					lSrcRect.right = lSrcRect.left + 1.0f;
					lSrcRect.top = lfExtendLineY + rSrcRect.top;
					lSrcRect.bottom = lSrcRect.top + 1.0f;

					if (nfAlpha >= 0.999f)
						mpTarget2D->DrawBitmap(lpBmp,&lDestRect,moRenderState.Top().mfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)ESPB_DRAWBMP_LINEAR,&lSrcRect);
					else
						mpTarget2D->DrawBitmap(lpBmp,&lDestRect,moRenderState.Top().mfAlpha*nfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)ESPB_DRAWBMP_LINEAR,&lSrcRect);
				}

				//////////////////////////////////////////////////////////////////////////
				//���Ϻ����µ��м䲿��
				lDestRect.left = lDestRect.right;
				lDestRect.right = rDestRect.right;


				lSrcRect.left = lSrcRect.right;
				lSrcRect.right = rSrcRect.right;

				if (nfAlpha >= 0.999f)
					mpTarget2D->DrawBitmap(lpBmp,&lDestRect,moRenderState.Top().mfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)ESPB_DRAWBMP_LINEAR,&lSrcRect);
				else
					mpTarget2D->DrawBitmap(lpBmp,&lDestRect,moRenderState.Top().mfAlpha*nfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)ESPB_DRAWBMP_LINEAR,&lSrcRect);


			}

			//////////////////////////////////////////////////////////////////////////
			//���½�
			lDestRect.left = rDestRect.left;
			lDestRect.right = lfExtendLineX + lDestRect.left;
			lDestRect.top = lDestRect.bottom;
			lDestRect.bottom = rDestRect.bottom;

			lSrcRect.left = rSrcRect.left;
			lSrcRect.right = lfExtendLineX + rSrcRect.left;
			lSrcRect.top = lSrcRect.bottom;
			lSrcRect.bottom = rSrcRect.bottom;


			if (nfAlpha >= 0.999f)
				mpTarget2D->DrawBitmap(lpBmp,&lDestRect,moRenderState.Top().mfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)ESPB_DRAWBMP_LINEAR,&lSrcRect);
			else
				mpTarget2D->DrawBitmap(lpBmp,&lDestRect,moRenderState.Top().mfAlpha*nfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)ESPB_DRAWBMP_LINEAR,&lSrcRect);

			//////////////////////////////////////////////////////////////////////////
			//���½Ǻ����½��м�Ĳ���
			if (lDestSize.width > lSrcSize.width)
			{
				//����Ǳ������Ҫִ������
				lDestRect.left = lDestRect.right;
				lDestRect.right = lDestRect.left + (lDestSize.width - lSrcSize.width) + 1.0f;

				lSrcRect.left = lSrcRect.right;
				lSrcRect.right ++;

				if (nfAlpha >= 0.999f)
					mpTarget2D->DrawBitmap(lpBmp,&lDestRect,moRenderState.Top().mfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)ESPB_DRAWBMP_LINEAR,&lSrcRect);
				else
					mpTarget2D->DrawBitmap(lpBmp,&lDestRect,moRenderState.Top().mfAlpha*nfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)ESPB_DRAWBMP_LINEAR,&lSrcRect);

			}

			//////////////////////////////////////////////////////////////////////////
			//���½�
			lDestRect.left = lDestRect.right;
			lDestRect.right = rDestRect.right;

			lSrcRect.left = lSrcRect.right;
			lSrcRect.right = rSrcRect.right; 

			if (nfAlpha >= 0.999f)
				mpTarget2D->DrawBitmap(lpBmp,&lDestRect,moRenderState.Top().mfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)ESPB_DRAWBMP_LINEAR,&lSrcRect);
			else
				mpTarget2D->DrawBitmap(lpBmp,&lDestRect,moRenderState.Top().mfAlpha*nfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)ESPB_DRAWBMP_LINEAR,&lSrcRect);

		}
		else
		{
			if(nuMethod == ESPB_DRAWBMP_EXTEND)
				nuMethod = ESPB_DRAWBMP_LINEAR;	//���Ǳ���ʹ����չ�㷨������ǰ�����ǲ���Ҫ�Ŵ�ģ�����ʹ������򵥻��ƣ���Method�����޸�Ϊ��ͨ��

			if (nfAlpha >= 0.999f)	
				mpTarget2D->DrawBitmap(lpBmp,&rDestRect,moRenderState.Top().mfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)nuMethod,&rSrcRect);
			else
				mpTarget2D->DrawBitmap(lpBmp,&rDestRect,moRenderState.Top().mfAlpha*nfAlpha,(D2D1_BITMAP_INTERPOLATION_MODE)nuMethod,&rSrcRect);

		}

		luResult = ERESULT_SUCCESS;

	} while (false);


	CMM_SAFE_RELEASE(lpBmp);

	return luResult;


}


ERESULT __stdcall CXD2dEngine::DrawBitmap( IN const D2D1_RECT_F& rDestRect, IN IEinkuiBitmap* npBitmap, IN ULONG nuMethod,	IN float nfAlpha)
{


	D2D1_RECT_F ldSrcRect;
	ID2D1Bitmap* lpBmp=NULL;
	ERESULT luResult = ERESULT_UNSUCCESSFUL;

	if(npBitmap == NULL)
		return ERESULT_WRONG_PARAMETERS;

	CXD2dBitmap* lpXuiD2DBitmap = dynamic_cast<CXD2dBitmap*>(npBitmap);

	do 
	{
		BREAK_ON_NULL(lpXuiD2DBitmap);

		// ��ʼ��λͼ����
		luResult = lpXuiD2DBitmap->InitBitmap(mpWicFactory, mpD2dFactory, mpTarget2D);
		BREAK_ON_FAILED(luResult);

		luResult = lpXuiD2DBitmap->GetD2DObject(mpTarget2D,&lpBmp);
		if(ERESULT_SUCCEEDED(luResult) && lpBmp != NULL)
		{
			ldSrcRect.left = ldSrcRect.top = 0;
			ldSrcRect.right = lpBmp->GetSize().width;
			ldSrcRect.bottom = lpBmp->GetSize().height;

			luResult = DrawBitmap(rDestRect,ldSrcRect,npBitmap,nuMethod, nfAlpha);
		}

	} while (false);

	CMM_SAFE_RELEASE(lpBmp);

	return luResult;

}


ERESULT __stdcall CXD2dEngine::DrawBitmap( IN const D2D1_POINT_2F& rDestLeftTop, IN IEinkuiBitmap* npBitmap,IN float nfAlpha)
{

	D2D1_RECT_F ldDstRect;
	D2D1_RECT_F ldSrcRect;
	ID2D1Bitmap* lpBmp=NULL;
	ERESULT luResult = ERESULT_UNSUCCESSFUL;
	CXD2dBitmap* lpXuiD2DBitmap = dynamic_cast<CXD2dBitmap*>(npBitmap);

	if(npBitmap == NULL)
		return ERESULT_WRONG_PARAMETERS;

	do 
	{
		BREAK_ON_NULL(lpXuiD2DBitmap);

		// ��ʼ��λͼ����
		luResult = lpXuiD2DBitmap->InitBitmap(mpWicFactory, mpD2dFactory, mpTarget2D);
		BREAK_ON_FAILED(luResult);

		luResult = lpXuiD2DBitmap->GetD2DObject(mpTarget2D, &lpBmp);
		BREAK_ON_FAILED(luResult);
		BREAK_ON_NULL(lpBmp);

		ldSrcRect.left = ldSrcRect.top = 0;
		ldSrcRect.right = lpBmp->GetSize().width;
		ldSrcRect.bottom = lpBmp->GetSize().height;

		ldDstRect.left = rDestLeftTop.x;
		ldDstRect.top = rDestLeftTop.y;
		ldDstRect.right = rDestLeftTop.x + ldSrcRect.right;
		ldDstRect.bottom = rDestLeftTop.y + ldSrcRect.bottom;

		// ���α�ʾ��ʽ��ת��
		luResult = DrawBitmap(ldDstRect,ldSrcRect,npBitmap,D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, nfAlpha);

	} while (false);

	CMM_SAFE_RELEASE(lpBmp);

	return luResult;
}


ERESULT __stdcall CXD2dEngine::DrawLine( IN const D2D1_POINT_2F& noPointArray, IN const D2D1_POINT_2F& noEndPoint, IN IEinkuiBrush* npBrush )
{

	ERESULT luResult = ERESULT_UNSUCCESSFUL;

	ID2D1Brush*	lpD2DBrush = NULL;
	ID2D1StrokeStyle* lpStrokeStyle = NULL;
	CXD2DBrush* lpXuiD2DBrush = dynamic_cast<CXD2DBrush*>(npBrush);

	do 
	{
		BREAK_ON_NULL(lpXuiD2DBrush);

		luResult = lpXuiD2DBrush->InitBrush(mpTarget2D, mpD2dFactory);
		BREAK_ON_FAILED(luResult);

		// ��ȡbrush
		luResult = lpXuiD2DBrush->GetBrushObject(&lpD2DBrush);
		BREAK_ON_FAILED(luResult);

		// ���Ի�ȡstroke
		luResult = lpXuiD2DBrush->GetStrokeObject(&lpStrokeStyle);
		BREAK_ON_FAILED(luResult);


		// ����D2D�Ļ��ߺ���
		if (lpStrokeStyle != NULL)
		{
			mpTarget2D->DrawLine(noPointArray, noEndPoint, lpD2DBrush, lpXuiD2DBrush->GetStrokeWidth(), lpStrokeStyle);
		}
		else
		{
			mpTarget2D->DrawLine(noPointArray, noEndPoint, lpD2DBrush, lpXuiD2DBrush->GetStrokeWidth());
		}


		luResult = ERESULT_SUCCESS;

	} while (false);

	return luResult;
}


ERESULT __stdcall CXD2dEngine::DrawPlogon( IN const D2D1_POINT_2F* noPointArray, IN INT niCount, IN IEinkuiBrush* npBrush )
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;
	HRESULT	hr;	

	ID2D1PathGeometry*	lpPathGeometry = NULL;
	ID2D1GeometrySink*	lpSink = NULL;
	ID2D1Brush*	lpD2DBrush = NULL;
	ID2D1StrokeStyle* lpStrokeStyle = NULL;
	CXD2DBrush* lpXuiD2DBrush = dynamic_cast<CXD2DBrush*>(npBrush);


	do 
	{
		BREAK_ON_NULL(lpXuiD2DBrush);

		luResult = lpXuiD2DBrush->InitBrush(mpTarget2D, mpD2dFactory);
		BREAK_ON_FAILED(luResult);

		// ��ȡbrush
		luResult = lpXuiD2DBrush->GetBrushObject(&lpD2DBrush);
		BREAK_ON_FAILED(luResult);

		// ���Ի�ȡstroke
		luResult = lpXuiD2DBrush->GetStrokeObject(&lpStrokeStyle);
		BREAK_ON_FAILED(luResult);


		hr = mpD2dFactory->CreatePathGeometry(&lpPathGeometry);
		BREAK_ON_FAILED(hr);

		hr = lpPathGeometry->Open(&lpSink);
		BREAK_ON_FAILED(hr);

		{
			lpSink->SetFillMode(D2D1_FILL_MODE_WINDING);
			lpSink->BeginFigure(
				noPointArray[0],
				D2D1_FIGURE_BEGIN_FILLED
				);

			lpSink->AddLines(noPointArray, niCount);
			lpSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		}

		lpSink->Close();

		if (lpStrokeStyle == NULL)
		{
			mpTarget2D->DrawGeometry(lpPathGeometry, lpD2DBrush, lpXuiD2DBrush->GetStrokeWidth());
		}
		else
		{
			mpTarget2D->DrawGeometry(lpPathGeometry, lpD2DBrush, lpXuiD2DBrush->GetStrokeWidth(), lpStrokeStyle);
		}

	} while (false);

	CMM_SAFE_RELEASE(lpPathGeometry);
	CMM_SAFE_RELEASE(lpSink);

	return luResult;
}


ERESULT __stdcall CXD2dEngine::FillPlogon( IN const D2D1_POINT_2F* noPointArray, IN INT niCount, IN IEinkuiBrush* npBrush )
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;
	HRESULT	hr;	

	ID2D1PathGeometry*	lpPathGeometry = NULL;
	ID2D1GeometrySink*	lpSink = NULL;

	ID2D1Brush*	lpD2DBrush;
	CXD2DBrush* lpXuiD2DBrush = dynamic_cast<CXD2DBrush*>(npBrush);


	do 
	{
		BREAK_ON_NULL(lpXuiD2DBrush);

		luResult = lpXuiD2DBrush->InitBrush(mpTarget2D, mpD2dFactory);
		BREAK_ON_FAILED(luResult);

		// ��ȡbrush
		luResult = lpXuiD2DBrush->GetBrushObject(&lpD2DBrush);
		BREAK_ON_FAILED(luResult);

		hr = mpD2dFactory->CreatePathGeometry(&lpPathGeometry);
		BREAK_ON_FAILED(hr);

		hr = lpPathGeometry->Open(&lpSink);
		BREAK_ON_FAILED(hr);

		{
			lpSink->SetFillMode(D2D1_FILL_MODE_WINDING);
			lpSink->BeginFigure(
				noPointArray[0],
				D2D1_FIGURE_BEGIN_FILLED
				);

			lpSink->AddLines(noPointArray, niCount);
			lpSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		}

		lpSink->Close();

		mpTarget2D->FillGeometry(lpPathGeometry, lpD2DBrush);
		
	} while (false);

	CMM_SAFE_RELEASE(lpPathGeometry);
	CMM_SAFE_RELEASE(lpSink);

	return luResult;
}


ERESULT __stdcall CXD2dEngine::DrawEllipse( IN const D2D1_RECT_F& noRect, IN IEinkuiBrush* npBrush )
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;
	HRESULT	hr;

	ID2D1Brush*	lpD2DBrush = NULL;
	ID2D1StrokeStyle* lpStrokeStyle = NULL;
	ID2D1EllipseGeometry *lpEllipseGeometry = NULL;
	CXD2DBrush* lpXuiD2DBrush = dynamic_cast<CXD2DBrush*>(npBrush);

	do 
	{
		BREAK_ON_NULL(lpXuiD2DBrush);

		luResult = lpXuiD2DBrush->InitBrush(mpTarget2D, mpD2dFactory);
		BREAK_ON_FAILED(luResult);

		hr = mpD2dFactory->CreateEllipseGeometry(
			D2D1::Ellipse(D2D1::Point2F(noRect.left + (noRect.right - noRect.left)/2 , noRect.top +(noRect.bottom - noRect.top)/2 ), (noRect.right - noRect.left)/2, (noRect.bottom - noRect.top)/2),
			&lpEllipseGeometry
			);
		BREAK_ON_FAILED(hr);


		// ��ȡbrush
		luResult = lpXuiD2DBrush->GetBrushObject(&lpD2DBrush);
		BREAK_ON_FAILED(luResult);

		// ���Ի�ȡstroke
		luResult = lpXuiD2DBrush->GetStrokeObject(&lpStrokeStyle);
		BREAK_ON_FAILED(luResult);

	
		if (lpStrokeStyle == NULL)
		{
			mpTarget2D->DrawGeometry(lpEllipseGeometry, lpD2DBrush, lpXuiD2DBrush->GetStrokeWidth());
		}
		else
		{
			mpTarget2D->DrawGeometry(lpEllipseGeometry, lpD2DBrush, lpXuiD2DBrush->GetStrokeWidth(), lpStrokeStyle);
		}

		luResult = ERESULT_SUCCESS;

	} while (false);

	// �ͷ���Դ
	CMM_SAFE_RELEASE(lpEllipseGeometry);

	return luResult;
}


ERESULT __stdcall CXD2dEngine::FillEllipse( IN const D2D1_RECT_F& noRect, IN IEinkuiBrush* npBrush )
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;
	HRESULT	hr;

	ID2D1Brush*	lpD2DBrush = NULL;
	ID2D1EllipseGeometry *lpEllipseGeometry = NULL;
	CXD2DBrush* lpXuiD2DBrush = dynamic_cast<CXD2DBrush*>(npBrush);

	do 
	{

		BREAK_ON_NULL(lpXuiD2DBrush);

		luResult = lpXuiD2DBrush->InitBrush(mpTarget2D, mpD2dFactory);
		BREAK_ON_FAILED(luResult);

		hr = mpD2dFactory->CreateEllipseGeometry(
			D2D1::Ellipse(D2D1::Point2F(noRect.left + (noRect.right - noRect.left)/2 , noRect.top +(noRect.bottom - noRect.top)/2 ), (noRect.right - noRect.left)/2, (noRect.bottom - noRect.top)/2),
			&lpEllipseGeometry
			);
		BREAK_ON_FAILED(hr);

		// ��ȡbrush
		luResult = lpXuiD2DBrush->GetBrushObject(&lpD2DBrush);
		BREAK_ON_FAILED(luResult);

		// �����Բ����
		mpTarget2D->FillGeometry(lpEllipseGeometry, lpD2DBrush);

		luResult = ERESULT_SUCCESS;

	} while (false);

	// �ͷ���Դ
	CMM_SAFE_RELEASE(lpEllipseGeometry);

	return luResult;
}


ERESULT __stdcall CXD2dEngine::DrawRect( IN const D2D1_RECT_F& noRect, IN IEinkuiBrush* npBrush )
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;
	HRESULT	hr;

	ID2D1Brush*	lpD2DBrush = NULL;
	ID2D1StrokeStyle* lpStrokeStyle = NULL;
	ID2D1RectangleGeometry *lpRectGeometry = NULL;
	CXD2DBrush* lpXuiD2DBrush = dynamic_cast<CXD2DBrush*>(npBrush);

	do 
	{
		BREAK_ON_NULL(lpXuiD2DBrush);

		luResult = lpXuiD2DBrush->InitBrush(mpTarget2D, mpD2dFactory);
		BREAK_ON_FAILED(luResult);

		hr = mpD2dFactory->CreateRectangleGeometry(noRect,
			&lpRectGeometry);
		BREAK_ON_FAILED(hr);

		// ��ȡbrush
		luResult = lpXuiD2DBrush->GetBrushObject(&lpD2DBrush);
		BREAK_ON_FAILED(luResult);

		// ���Ի�ȡstroke
		luResult = lpXuiD2DBrush->GetStrokeObject(&lpStrokeStyle);
		BREAK_ON_FAILED(luResult);


		if (lpStrokeStyle == NULL)
		{
			mpTarget2D->DrawGeometry(lpRectGeometry, lpD2DBrush, lpXuiD2DBrush->GetStrokeWidth());
		}
		else
		{
			mpTarget2D->DrawGeometry(lpRectGeometry, lpD2DBrush, lpXuiD2DBrush->GetStrokeWidth(), lpStrokeStyle);
		}

		luResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpRectGeometry);

	return luResult;
}


ERESULT __stdcall CXD2dEngine::FillRect( IN const D2D1_RECT_F& noRect, IN IEinkuiBrush* npBrush )
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;
	HRESULT	hr;

	ID2D1Brush*	lpD2DBrush = NULL;
	ID2D1RectangleGeometry *lpRectGeometry = NULL;
	CXD2DBrush* lpXuiD2DBrush = dynamic_cast<CXD2DBrush*>(npBrush);

	do 
	{
		BREAK_ON_NULL(lpXuiD2DBrush);

		luResult = lpXuiD2DBrush->InitBrush(mpTarget2D, mpD2dFactory);
		BREAK_ON_FAILED(luResult);

		hr = mpD2dFactory->CreateRectangleGeometry(noRect,
			&lpRectGeometry);
		BREAK_ON_FAILED(hr);

		// ��ȡbrush
		luResult = lpXuiD2DBrush->GetBrushObject(&lpD2DBrush);
		BREAK_ON_FAILED(luResult);


		// �����Բ����
		mpTarget2D->FillGeometry(lpRectGeometry, lpD2DBrush);

		luResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpRectGeometry);

	return luResult;
}


ERESULT __stdcall CXD2dEngine::DrawRoundRect( IN const D2D1_RECT_F& noRect, FLOAT radiusX, FLOAT radiusY, IN IEinkuiBrush* npBrush )
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;
	HRESULT	hr;

	ID2D1Brush*	lpD2DBrush = NULL;
	ID2D1StrokeStyle* lpStrokeStyle = NULL;
	ID2D1RoundedRectangleGeometry* lpRoundedRectGeometry = NULL;
	CXD2DBrush* lpXuiD2DBrush = dynamic_cast<CXD2DBrush*>(npBrush);

	do 
	{
		BREAK_ON_NULL(lpXuiD2DBrush);

		luResult = lpXuiD2DBrush->InitBrush(mpTarget2D, mpD2dFactory);
		BREAK_ON_FAILED(luResult);

		hr = mpD2dFactory->CreateRoundedRectangleGeometry(
			D2D1::RoundedRect
			(
				noRect,
				radiusX,
				radiusY
			),
			&lpRoundedRectGeometry);
		BREAK_ON_FAILED(hr);

		// ��ȡbrush
		luResult = lpXuiD2DBrush->GetBrushObject(&lpD2DBrush);
		BREAK_ON_FAILED(luResult);

		// ���Ի�ȡstroke
		luResult = lpXuiD2DBrush->GetStrokeObject(&lpStrokeStyle);
		BREAK_ON_FAILED(luResult);


		if (lpStrokeStyle == NULL)
		{
			mpTarget2D->DrawGeometry(lpRoundedRectGeometry, lpD2DBrush, lpXuiD2DBrush->GetStrokeWidth());
		}
		else
		{
			mpTarget2D->DrawGeometry(lpRoundedRectGeometry, lpD2DBrush, lpXuiD2DBrush->GetStrokeWidth(), lpStrokeStyle);
		}

		luResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpRoundedRectGeometry);

	return luResult;
}

ERESULT __stdcall CXD2dEngine::FillRoundRect( IN const D2D1_RECT_F& noRect, FLOAT radiusX, FLOAT radiusY, IN IEinkuiBrush* npBrush )
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;
	HRESULT	hr;

	ID2D1Brush*	lpD2DBrush = NULL;
	ID2D1RoundedRectangleGeometry* lpRoundedRectGeometry = NULL;
	CXD2DBrush* lpXuiD2DBrush = dynamic_cast<CXD2DBrush*>(npBrush);

	do 
	{
		BREAK_ON_NULL(lpXuiD2DBrush);

		luResult = lpXuiD2DBrush->InitBrush(mpTarget2D, mpD2dFactory);
		BREAK_ON_FAILED(luResult);

		hr = mpD2dFactory->CreateRoundedRectangleGeometry(
			D2D1::RoundedRect
			(
				noRect,
				radiusX,
				radiusY
			),
			&lpRoundedRectGeometry);
		BREAK_ON_FAILED(hr);

		// ��ȡbrush
		luResult = lpXuiD2DBrush->GetBrushObject(&lpD2DBrush);
		BREAK_ON_FAILED(luResult);


		// �����Բ����
		mpTarget2D->FillGeometry(lpRoundedRectGeometry, lpD2DBrush);

		luResult = ERESULT_SUCCESS;

	} while (false);

	CMM_SAFE_RELEASE(lpRoundedRectGeometry);

	return luResult;
}



//////////////////////////////////////////////////////////////////////////
// �������ܺ���
//////////////////////////////////////////////////////////////////////////

// ���Direct2D��RenderTarget�������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
// ��Ӧ����EMSG_PAINT/EMSG_RENDER_ENHANCER��Ϣ�����ڼ�ִ�л��ƶ�����������Prepare��Ϣ�ڼ���ƣ������ƻ���Ⱦ������ȶ�
ID2D1RenderTarget* __stdcall CXD2dEngine::GetD2dRenderTarget(void)
{
	return mpTarget2D;
}

// ���Direct2D�Ĺ����ӿڣ������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
ID2D1Factory* __stdcall CXD2dEngine::GetD2dFactory(void)
{
	return mpD2dFactory;
}

// ���WIC�����������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
IWICImagingFactory* __stdcall CXD2dEngine::GetWICFactory(void)
{
	return mpWicFactory;
}

// ���Direct Write�����������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
IDWriteFactory* __stdcall CXD2dEngine::GetDWriteFactory(void)
{
	return mpWriteFactory;
}


// ���Eink���Ĵ�С
void CXD2dEngine::GetPaintboardSize(
	OUT EI_SIZE* npSize	// ��ȡ�����С
)	
{
	if (npSize != NULL)
	{
		npSize->w = (LONG)muEinkPanelW;
		npSize->h = (LONG)muEinkPanelH;
	}
}

// ��õ�ǰ��D2d�����ã��ֲ����굽���������ת������
const D2D1::Matrix3x2F& __stdcall CXD2dEngine::GetCurrent2DWorldMatrix(void)
{
	if(moRenderState.Size()>0)
		return moRenderState.Top().mdWorld;
	else 
		return mdIdentyMatrix;
}

// ��õ�ǰ���������ã��������������������������D2dTarget���л��������������л����Target��Ӧ����������
// ����ERESULT_SUCCESS��ǰ���ڿ��������ã����ҳɹ�ȡ�ã�ERESULT_UNSUCCESS�����ڿ��������ã�����ֵ��ʾ����
ERESULT __stdcall CXD2dEngine::GetVisibleRegion(
	OUT D2D1_RECT_F* npRegion
	)
{
	int i;
	ERESULT luResult = ERESULT_UNSUCCESSFUL;
	D2D1_POINT_2F loPoint;

	for (i=moRenderState.Size()-1;i>=0;i--)
	{
		if(moRenderState[i].mpD2dTarget != NULL)	// ����л�����2DTarget�Ͳ���Ҫ����������
			break;
		if(moRenderState[i].mbHasClipRect != false)	// ���ڿ�����
		{
			luResult = ERESULT_SUCCESS;
			if(npRegion != NULL)
			{
				loPoint.x = moRenderState[i].mdClipRect.left;
				loPoint.y = moRenderState[i].mdClipRect.top;
				loPoint = loPoint * moRenderState[i].mdWorld;

				npRegion->left = loPoint.x;
				npRegion->top= loPoint.y;

				loPoint.x = moRenderState[i].mdClipRect.right;
				loPoint.y = moRenderState[i].mdClipRect.bottom;
				loPoint = loPoint * moRenderState[i].mdWorld;

				npRegion->right = loPoint.x;
				npRegion->bottom = loPoint.y;
			}
			break;
		}
	}

	return luResult;
}

// ����ֱ�ӷ���D3d��D2d�����Ԫ�أ�ʹ�ñ��ӿ���ϵͳ�㱨�豸����ϵͳ����ERESULT_SUCCESS��ʾ���Լ���ִ�У�����ֵ�����ERESULT_FAILED����ֹ����ִ��
ERESULT __stdcall CXD2dEngine::CheckDeviceResult(
	IN HRESULT nhrDeviceResult
	)
{
	if(nhrDeviceResult != S_OK)
	{
		//����OK��DirectX����ֵ
		/*if(nhrDeviceResult == DXGI_STATUS_OCCLUDED)
			StopPainting(true);*/ //close by niu 2017-12-5 �豸��ʧ��Ҫֹͣ���´λ���ʱ���ؽ��豸

		if (
			nhrDeviceResult == DXGI_STATUS_OCCLUDED ||
			nhrDeviceResult == DXGI_ERROR_DEVICE_REMOVED ||
			nhrDeviceResult == DXGI_ERROR_INVALID_CALL ||
			nhrDeviceResult == DXGI_ERROR_DEVICE_RESET ||
			nhrDeviceResult == D2DERR_RECREATE_TARGET)
		{
			//��ʾ�豸��ʧ

			ReleaseDeviceResource();
			return ERESULT_DIRECTX_ERROR;
		}

	}
	return ERESULT_SUCCESS;
}


// ��õ�ǰ֡��
ULONG __stdcall CXD2dEngine::GetCurrentFps(void)
{
	return (ULONG)mfFpsIndx;
}

// ��õ�ǰ����Ⱦ���кţ��������к�������Э���ͼ��㵱ǰ����Ⱦ������ÿִ��һ����Ⱦ����ż�һ��ĳЩ����£�Ԫ�ؿ��ԻᱻҪ�����2�Σ������ﵽ���ֵ����0��ʼ
ULONG __stdcall CXD2dEngine::GetRenderNumber(void)
{
	return muRenderCount;
}

// ��õ�ǰ��TickCount
ULONG __stdcall CXD2dEngine::GetCurrentTickCount()
{
	return muCrtTick;
}

// ������
void CXD2dEngine::Clear()
{
	mpTarget2D->Clear(D2D1::ColorF(D2D1::ColorF::White, 0.0f));
}

// �ύ����Ļ
HRESULT CXD2dEngine::Present(
	IN bool nbRefresh	// �����ύȫ��
	)
{
	if (mbStopPainting != false)
		return S_OK;

	if (mpCustomDraw != NULL)
		mpCustomDraw(mpForeBuffer, muEinkPanelW, muEinkPanelH,nbRefresh);
	else
		DrawEink(nbRefresh);

	return S_OK;
}

void CXD2dEngine::DrawEink(
	IN bool nbRefresh	// �����ύȫ��
	)
{
	// ��ȡEink���ƻ�����
	if (mpEinkBuf == NULL)
	{
		mpEinkBuf = EiGetDrawBuffer(FALSE, FALSE);
		if (mpEinkBuf == NULL)
			return;

		ClearEinkBuffer(true);
	}

	//DWORD luTick = GetTickCount();

	//for(int k=0;k<100;k++)
	//{

	moEiUpdate.SetPanel(muEinkPanelW,muEinkPanelH);

	// ��RGB32ת��Ϊ�Ҷ�ͼ��
	// Gray = R*0.299 + G*0.587 + B*0.114
#ifdef EI_PARALLEL
	parallel_for(size_t(0), (size_t)muEinkPanelH, [&](size_t i)
#else
	for (ULONG i = 0; i < muEinkPanelH; i++)
#endif//EI_PARALLEL
	{

		register unsigned char* lpDst = mpEinkBuf->Buf + muEinkPanelW*(ULONG)i;
		register unsigned char* lpSrc = mpForeBuffer + muEinkPanelW * 4 * (ULONG)i;
		register unsigned char lucNew;
		register ULONG luBegin;
		register ULONG luEnd;
		int j;
		//int liWidthMin = (int)(mpEinkBuf->ulWidth < muEinkPanelW ? mpEinkBuf->ulWidth : muEinkPanelW);// ???ax δ���������
		luBegin = (ULONG)muEinkPanelW;// liWidthMin;
		luEnd = 0;
		// ת����ɫ�ռ䣬ͬʱ����Ѱ��һ�������
		for (j = 0; j < muEinkPanelW; j++, lpSrc += 4, lpDst++)
		{
			//lucNew = (((USHORT(lpSrc[0]) * 19 + USHORT(lpSrc[1]) * 38 + USHORT(lpSrc[2]) * 7) >> 6) & 0xF0);
			lucNew = ((USHORT(lpSrc[0]) * 19 + USHORT(lpSrc[1]) * 38 + USHORT(lpSrc[2]) * 7) >> 6);
			if (lucNew != *lpDst)
			{
				luBegin = j;
				j = muEinkPanelW; // �����˳���ѭ��
				*lpDst = lucNew;
			}
		}

		// �������в��ȵ����
		if (luBegin < (ULONG)muEinkPanelW)
		{
			j = luBegin + 1; // �ص��ղŵ�λ��
			luEnd = luBegin;

			// ת����ɫ�ռ䣬ͬʱ����¼����ȵ�ĩβ
			for (; j < muEinkPanelW; j++, lpSrc += 4, lpDst++)
			{
				//lucNew = (((USHORT(lpSrc[0]) * 19 + USHORT(lpSrc[1]) * 38 + USHORT(lpSrc[2]) * 7) >> 6) & 0xF0);
				lucNew = ((USHORT(lpSrc[0]) * 19 + USHORT(lpSrc[1]) * 38 + USHORT(lpSrc[2]) * 7) >> 6);
				if (lucNew != *lpDst)
				{
					luEnd = (short)j; // ��¼���һ��λ��
					*lpDst = lucNew;
				}
			}
			
			luEnd = luEnd + 1;	// ��Endָ�򡮲�ͬ����֮���һ������
		}

		if (luEnd > luBegin)
			moEiUpdate.SaveLineDiff((int)i, luBegin, luEnd); // ����������̰߳�ȫ 
	}
#ifdef EI_PARALLEL
	);
#endif//EI_PARALLEL

	//}
	//luTick = GetTickCount() - luTick;

	//////////////////////////////////////////////////////////////////////////
	// д���ļ����������ݸı�
	//static int gliTraceNumber = 0;
	//wchar_t glszTrackName[256];

	//swprintf_s(glszTrackName,256, L"c:\\test\\t%04d.bin", gliTraceNumber++);

	//ULONG luWritten;
	//HANDLE lhFile = CreateFile(glszTrackName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);

	//if (lhFile != NULL)
	//{
	//	WriteFile(lhFile,mpEinkBuf->Buf, mpEinkBuf->ulWidth*mpEinkBuf->ulHeight, &luWritten, NULL);
	//	CloseHandle(lhFile);
	//}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	////EI_RECT ldUpdateRect = { 0,0,muEinkPanelW,muEinkPanelH};
	//EI_RECT ldUpdateRect = { muEinkPanelW,muEinkPanelH,0,0 };

	//for (ULONG i = 0; i < muHeight; i++)
	//{
	//	if (mpDiff[i].Begin < mpDiff[i].End)
	//	{
	//		if (mpDiff[i].Begin < ldUpdateRect.x)
	//			ldUpdateRect.x = mpDiff[i].Begin;
	//		if (mpDiff[i].End > ldUpdateRect.w)
	//			ldUpdateRect.w = mpDiff[i].End;
	//		if (i < ldUpdateRect.y)
	//			ldUpdateRect.y = i;
	//		if (i > ldUpdateRect.h)
	//			ldUpdateRect.h = i;
	//	}
	//}

	//if (ldUpdateRect.w > ldUpdateRect.x && ldUpdateRect.h > ldUpdateRect.y)
	//{
	//	ldUpdateRect.w -= ldUpdateRect.x;
	//	ldUpdateRect.h -= ldUpdateRect.y;

	//	EiDraw(&ldUpdateRect, mpEinkBuf);
	//}
	//////////////////////////////////////////////////////////////////////////

	if (mpEinkUpdatingCallBack != NULL)
		mpEinkUpdatingCallBack(++mxUpdatingNumber,mpEinkUpdatingContext);

	if (mlResetEinkBuf != 0)
	{
		EI_RECT ldUpdateRect = { 0,0,muEinkPanelW,muEinkPanelH};
		EiDraw(&ldUpdateRect, mpEinkBuf);

		ClearEinkBuffer(false);
	}
	else
	{
		CEiBlocks loBlocks;

		moEiUpdate.GetDiffBlocks(loBlocks);
		moEiUpdate.Reset();

		for (int j = 0; j < loBlocks.Size(); j++)
		{
			EI_RECT ldUpdateRect;

			ldUpdateRect.x = loBlocks[j].x1;
			ldUpdateRect.w = loBlocks[j].x2 - loBlocks[j].x1;
			ldUpdateRect.y = loBlocks[j].y1;
			ldUpdateRect.h = loBlocks[j].y2 - loBlocks[j].y1;

			EiDraw(&ldUpdateRect, mpEinkBuf);
		}
	}

	//EI_RECT ldUpdateRect = { 0,0,mpEinkBuf->ulWidth,100 };

	//while (ldUpdateRect.y < muHeight)
	//{
	//	if (ldUpdateRect.y + ldUpdateRect.h > muHeight)
	//		ldUpdateRect.h = muHeight - ldUpdateRect.y;

	//	EiDraw(&ldUpdateRect, mpEinkBuf);

	//	ldUpdateRect.y += ldUpdateRect.h;

	//}
}

// ִ�л�������
ERESULT CXD2dEngine::DoRender(
	IN ULONG nuCrtTick,	// ��ǰ��TickCount
	IN bool nbRefresh,	// �����ύȫ��
	IN IEinkuiIterator* npToCapture	// ��ֵ����������ץȡ��Ԫ������Ԫ�ص�ͼ�񣬲�����Ӱ����Ļ��ʾ
	)
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;

	 //����Ƿ��ڲ����߳�
	if (CEinkuiSystem::gpXuiSystem->IsRunningInOperationThread() == false)
	{
		return ERESULT_WRONG_THREAD;
	}

	// �жϴ����Ƿ�����
	if(mbStopPainting != false && npToCapture == NULL)
		return ERESULT_NOT_PAINT;
	
	//��ʼ����

	InterlockedExchange(&mlRenderStep,CEinkuiSystem::eRenderBegin);
	InterlockedExchangePointer(reinterpret_cast<PVOID*>(&mpToCapture),npToCapture);

	try
	{
		if(mpElementManager == NULL)
		{
			mpElementManager = dynamic_cast<CXelManager*>(CEinkuiSystem::gpXuiSystem->GetElementManager());
			if(mpElementManager == NULL)
				THROW_NULL;
		}

		luResult = CreateDeviceResource();
		if(luResult != ERESULT_SUCCESS)
			THROW_FALSE;

		//�������׼����Ϣ
		CMM_SAFE_RELEASE(mpMessage);
		mpMessage = mpElementManager->AllocateMessage();
		if(mpMessage == NULL)
			THROW_NULL;

		//����״̬��գ���ʾ�����Ǵ���׼�����ơ���Ϣ�ڼ�
		moRenderState.Clear();
		muCrtTick = nuCrtTick;

		//�㲥����׼����Ϣ
		InterlockedExchange(&mlRenderStep, CEinkuiSystem::eRenderParepare);

		luResult = mpElementManager->EnumAllElement(
				false,
				this,
				(ERESULT (__stdcall IBaseObject::*)(IEinkuiIterator* npRecipient))&CXD2dEngine::EnterForPrepare,
				(ERESULT (__stdcall IBaseObject::*)(IEinkuiIterator* npRecipient))&CXD2dEngine::LeaveForPrepare
				);
		if(ERESULT_FAILED(luResult))
		{
			//׼������ʧ��
			THROW_FALSE;
		}

		//���������Ϣ
		CMM_SAFE_RELEASE(mpMessage);
		mpMessage = mpElementManager->AllocateMessage();
		if(mpMessage == NULL)
			THROW_NULL;

		//��ʼ������״̬
		{
			CErdState loState;
			loState.mdWorld = mdIdentyMatrix;
			loState.mdTransLation = mdIdentyMatrix;
			loState.mdRotation = mdIdentyMatrix;
			loState.mdOffset = D2D1::Point2F(0.0f,0.0f);
			loState.mfAlpha = 1.0f;
			loState.mpEnhancer = NULL;
			loState.mbRotated = false;
			loState.mlPlLevelHost = -1;
			loState.mlCrtPtLevel = 0;

			moRenderState.Push(loState);
		}

		if (mpTarget2D == NULL)
			EinkuiGetSystem()->ExitXui(); //???? niu��ʱ��֪��ʲôԭ���������Ϊ��������������ֱ���˳�����

		mpTarget2D->BeginDraw();
		//mbD2dBeginCalled = true;

		// ����
		Clear();
	
		//�㲥������Ϣ
		InterlockedExchange(&mlRenderStep, CEinkuiSystem::eRenderRender);

		if(mpToCapture == NULL)
		{
			luResult = mpElementManager->EnumAllElement(
									false,
									this,
									(ERESULT (__stdcall IBaseObject::*)(IEinkuiIterator* npRecipient))&CXD2dEngine::EnterForPaint,
									(ERESULT (__stdcall IBaseObject::*)(IEinkuiIterator* npRecipient))&CXD2dEngine::LeaveForPaint
									);
		}
		else
		{
			mbCapturing = false;
			luResult = mpElementManager->EnumAllElement(
				false,
				this,
				(ERESULT (__stdcall IBaseObject::*)(IEinkuiIterator* npRecipient))&CXD2dEngine::EnterForCapture,
				(ERESULT (__stdcall IBaseObject::*)(IEinkuiIterator* npRecipient))&CXD2dEngine::LeaveForCapture
				);
		}

		// ����֡��
		{

			FLOAT lfTick = (FLOAT)(nuCrtTick - muLastTick);
			if(lfTick < 0.005f)
				lfTick = 0.005f;
			mfFpsIndx = mfFpsIndx*((1000.0f- lfTick*2.0f)/1000.0f) + (1000.0f/lfTick)*(lfTick*2.0f/1000.0f);
			muLastTick = nuCrtTick;
		}

		if(mpToCapture == NULL && mpElementManager->GetProbeMode()==4)
		{
			wchar_t lswFps[64];

			swprintf_s(lswFps,64,L"%2d [fps]",(ULONG)mfFpsIndx);

			mpTarget2D->SetTransform(mdIdentyMatrix);

			mpTarget2D->DrawText(
				lswFps,
				9,//wcslen(lswFps),
				mpDbgTextFormat,
				D2D1::RectF(0.0f, 0.0f,150.0f,30.0f),
				mpDbgTextBrush
				);

		}

		if(mpElementManager->GetProbeMode() == 2)
		{
			wchar_t lswDump[512]=L"";
			const wchar_t* lswEleType;
			IEinkuiIterator* lpKeyFocus = mpElementManager->GetMouseFocus();
			IEinkuiIterator* lpItr = lpKeyFocus;
			if(lpItr != NULL)
			{
				while(lpItr != lpItr->GetParent())
				{
					lswEleType = lpItr->GetElementObject()->GetType();
					wcscat_s(lswDump,512,lswEleType);
					wcscat_s(lswDump,512,L"\n\r");
					lpItr = lpItr->GetParent();
				}

				UINT luSize = (UINT)wcslen(lswDump);
				D2D1_POINT_2F ldPos = mpElementManager->GetCurrentMousePosition();
				if(ldPos.x+300.0f > (FLOAT)muEinkPanelW)
					ldPos.x = (FLOAT)muEinkPanelW - 300.0f;
				if(ldPos.y + 300.0f > (FLOAT)muEinkPanelH)
					ldPos.y = (FLOAT)muEinkPanelH- 300.0f;

				mpTarget2D->SetTransform(D2D1::Matrix3x2F::Identity());
				mpTarget2D->DrawText(lswDump,luSize,mpDbgTextFormat,
					D2D1::RectF(ldPos.x+30.0f,ldPos.y+10.0f,ldPos.x+300.0f,ldPos.y+300.0f),
					mpDbgTextBrush);

				mpElementManager->ReleaseIterator(lpKeyFocus);
			}
		}


		luResult = CheckDeviceResult(mpTarget2D->EndDraw());

		InterlockedExchange(&mlRenderStep,CEinkuiSystem::eRenderEnd);

		if(ERESULT_FAILED(luResult))
		{
			//����ʧ��
			THROW_FALSE;
		}
	}
	catch(int)
	{
		if(ERESULT_SUCCEEDED(luResult))
			luResult = ERESULT_UNSUCCESSFUL;
	}
	catch (...)
	{
		//StopPainting(true);
		ReleaseDeviceResource();
		luResult = ERESULT_DIRECTX_ERROR;
	}

	if( mpToCapture == NULL && ERESULT_SUCCEEDED(luResult))
		luResult = CheckDeviceResult(Present(nbRefresh));

	if(ERESULT_FAILED(luResult))
	{
		//����ʧ��

		if(ERESULT_SUCCEEDED(luResult))
			luResult = ERESULT_UNSUCCESSFUL;
	}
	else
	{
		// ���ڵ��ñ�����DoRender֮����ⲿ��ͨ��WindowsUI�̵߳���
		////��Ϊ���ǵĽ������»����ˣ�������Ҫ����ϵͳ�´���Ȼ��������Ҫ��ʾ��Ԥ��ͼ��
		//DwmInvalidateIconicBitmaps(mhWindow);
		//�������
	}

	InterlockedIncrement((LONG*)&muRenderCount);
	InterlockedCompareExchange((LONG*)&muRenderCount,0,MAXULONG32);
	InterlockedExchange(&mlRenderStep,CEinkuiSystem::eRenderStop);
	InterlockedExchangePointer(reinterpret_cast<PVOID*>(&mpToCapture),NULL);

	return luResult;

}

// ��Windows���ڹ��̵��ã����洰����С����Ϣ
void CXD2dEngine::StopPainting(bool nbMin){

	if(mbStopPainting != nbMin)
		mbStopPainting = nbMin;
}

// �ı䴰�ڴ�С
void CXD2dEngine::ResetPaintboard(void)
{
	// ����Ƿ��ڲ����߳�
	CMMASSERT(CEinkuiSystem::gpXuiSystem->IsRunningInOperationThread() != false);

	ClearEinkBuffer(true);
	ReleaseDeviceResource();

	RelocationPainboard();
}

// ���¶�λ
void CXD2dEngine::RelocationPainboard(void)
{
	EI_APP_CONTEXT ldContext;
	EiGetAppContext(&ldContext);
	if (ldContext.ulWidth == 0)
		return;	// ʧ��

	muEinkPanelW = ldContext.ulWidth;
	muEinkPanelH = ldContext.ulHeight;

	//muEinkPanelW = CEinkuiSystem::gpXuiSystem->GetEinkSystemInfo()->ulWidth;
	//muEinkPanelH = CEinkuiSystem::gpXuiSystem->GetEinkSystemInfo()->ulHeight;
	if (muFixedW != 0)
		muEinkPanelW = muFixedW;
	if (muFixedH != 0)
		muEinkPanelH = muFixedH;
}

// ����Prepare PaintǰԤ����
ERESULT __stdcall CXD2dEngine::EnterForPrepare(IEinkuiIterator* npRecipient)
{
	if(npRecipient->IsVisible()==false)
		return ERESULT_STOP_ENUM_CHILD;

	PVOID lpThis = this;
	mpMessage->SetMessageID(EMSG_PREPARE_PAINT);
	mpMessage->SetInputData(&lpThis,sizeof(lpThis));

	ERESULT luResult = mpElementManager->SendMessage(npRecipient,mpMessage);

	if(ERESULT_FAILED(luResult))	// ��������ˣ��Ͳ�����ö���ˣ����ش���
		return luResult;

	return ERESULT_ENUM_CHILD;
}

// ����Prepare Paint����
ERESULT __stdcall CXD2dEngine::LeaveForPrepare(IEinkuiIterator* npRecipient)
{
	return ERESULT_SUCCESS;
}

// ����PaintǰԤ����
ERESULT __stdcall CXD2dEngine::EnterForPaint(IEinkuiIterator* npRecipient)
{
	ERESULT luResult = ERESULT_SUCCESS;
	CErdState loState;
	D2D1_POINT_2F ldPosition;
	D2D1_POINT_2F ldCenter;
	FLOAT lfAngle;
	FLOAT lfCrtAlpha;
	CXuiIterator* lpRecipient;
	D2D1_RECT_F ldClipRect;
	LONG llMyLevel;
	LONG llLevelCount;

	lpRecipient = dynamic_cast<CXuiIterator*>(npRecipient);

	llLevelCount = lpRecipient->GetPaintLevelCount();
	llMyLevel = lpRecipient->GetPaintLevel();

	// �����Ƿ���Ҫ���Ʊ������Ӷ���
	// ������أ���ѹ��һ����״̬��Ȼ���˳�; �½��˻��Ʋ�Σ���ǰ����������ʾʱ���������ڲ�
	if(lpRecipient->IsVisible()==false || llLevelCount > 0 && (llMyLevel != moRenderState.Top().mlCrtPtLevel && (llMyLevel != -1 || moRenderState.Top().mlCrtPtLevel != 0)))
	{
		loState.mpEnhancer = NULL;
		loState.mlCrtPtLevel = 0;
		moRenderState.Push(loState);

		return ERESULT_STOP_ENUM_CHILD;
	}

	// ���㵱ǰ����������͸���ȣ�����ѹ��״̬ջ
	ldPosition = lpRecipient->GetPosition();
	lfAngle = lpRecipient->GetRotation(ldCenter);

	// �����λ�ƾͼ���λ�Ƶľ���
	if(ldPosition.x != 0.0f || ldPosition.y != 0.0f)
	{
		//loState.mdTransLation = moRenderState.Top().mdTransLation * D2D1::Matrix3x2F::Translation(ldPosition.x,ldPosition.y);
		loState.mdOffset.x = CExFloat::Round(moRenderState.Top().mdOffset.x + ldPosition.x);
		loState.mdOffset.y = CExFloat::Round(moRenderState.Top().mdOffset.y + ldPosition.y);
		loState.mdTransLation = D2D1::Matrix3x2F::Translation(loState.mdOffset.x,loState.mdOffset.y);
	}
	else
	{
		loState.mdTransLation = moRenderState.Top().mdTransLation;
		loState.mdOffset = moRenderState.Top().mdOffset;
	}

	// ���ϲ�Ԫ�ظ��ƾ���
	loState.mbRotated = moRenderState.Top().mbRotated;
	loState.mdRotation = moRenderState.Top().mdRotation;
	loState.mdWorld = loState.mdTransLation * loState.mdRotation;	// �����ϲ�Ԫ����ת��ľ�������ϲ�Ԫ�ض�û����ת��������ǳ��Ե�λ��
	//loState.mbTopDraw = (moRenderState.Top().mbTopDraw || lpRecipient->CheckStyle(EITR_STYLE_TOPDRAW));
	// ��������������µĻ��Ʋ��
	if(llLevelCount > 0)
	{
		// ��ȡ�µĻ��Ʋ�α��
		// �ӵ�ǰ��ջȡ��ǰ�α�������Ļ��Ʋ���,�õ��µĻ��Ʋ�α��
		loState.mlCrtPtLevel = lpRecipient->GetNextPaintLevel(moRenderState.Top().mlPlLevelHost);
		if(loState.mlCrtPtLevel > 0)
			loState.mbPaintItself = false;
	}
	else
	{
		// ���ϲ㸴�ƻ��Ʋ��
		loState.mlCrtPtLevel = moRenderState.Top().mlCrtPtLevel;

		// ������������������Ƿ���Ҫ��ʾ
		if(llMyLevel >= 0)
		{
			if(llMyLevel != loState.mlCrtPtLevel)
				loState.mbPaintItself = false;
		}
		else
		{
			loState.mbPaintItself = moRenderState.Top().mbPaintItself;
		}
	}
	// Ϊ�����趨������չ���Ļ��Ʋ�εĳ�ʼֵ
	loState.mlPlLevelHost = -1;



	// �����ǰ�����µ���ת�趨
	if(lfAngle >= 0.01f)
	{
		// ���ȼ����µ�����λ��
		ldCenter = ldCenter * loState.mdWorld;
		//ldCenter = D2D1::Matrix3x2F::ReinterpretBaseType(&loState.mdWorld)->TransformPoint(ldCenter);��ת���˷�

		loState.mdRotation = loState.mdRotation * D2D1::Matrix3x2F::Rotation(lfAngle,ldCenter);	// ������ת���󵥶����
		loState.mdWorld = loState.mdTransLation * loState.mdRotation;
		loState.mbRotated = true;
	}

	lfCrtAlpha = lpRecipient->GetAlpha();
	if(lfCrtAlpha < 0.999 || moRenderState.Top().mfAlpha < 0.999)
	{
		loState.mfAlpha = moRenderState.Top().mfAlpha * lfCrtAlpha;
		if(loState.mfAlpha >= 0.999f)
			loState.mfAlpha = 1.0f;
	}
	else
		loState.mfAlpha = 1.0f;

	// ���游������Ϊ
	if(loState.mlCrtPtLevel == 0)
	{
		loState.mpEnhancer = lpRecipient->GetEnhancer();
	}

	//// �����ô��룬��ʽ����û��֧�����ŵı�Ҫ
	// 	// �ܵ����ſ���
	// 	loState.mdWorld =  loState.mdWorld * mdScalMatrixToCapture;

	moRenderState.Push(loState);

	// ���û����õ�����ת������
	mpTarget2D->SetTransform(loState.mdWorld);
	lpRecipient->SaveWorldMatrix(loState.mdWorld);

	// �ж��Ƿ�ע������Ⱦ��Ч��
	if(loState.mpEnhancer != NULL)
	{
		STEMS_ENHANCER_RENDER ldEnhanceRender;
		ldEnhanceRender.mpPaintBoard = this;
		ldEnhanceRender.mpDestElement = npRecipient;

		luResult = mpElementManager->SimpleSendMessage(loState.mpEnhancer,EMSG_PREPARE_ENHANCER,&ldEnhanceRender,sizeof(ldEnhanceRender),NULL,0);
		if(luResult == ERESULT_SKIP_RENDER_CONTENT || ERESULT_FAILED(luResult))
		{
			// �������������ƣ�ֻҪ����ERESULT_ENUM_CHILD�ͱ�Ȼ���������ݻ���
			return luResult;
		}
	}

	// ���ü�����
	if(npRecipient->GetVisibleRegion(ldClipRect)!=false)
	{
		moRenderState.Top().mbHasClipRect = true;
		moRenderState.Top().mdClipRect = ldClipRect;
		mpTarget2D->PushAxisAlignedClip(ldClipRect,D2D1_ANTIALIAS_MODE_ALIASED);
	}


	// �����Ԫ��Ҫ��TopDraw����ʱ��������Ϣ
	//if(loState.mbTopDraw == false)
	//{
		//// ���ͻ�����Ϣ
		//mpMessage->SetMessageID(EMSG_PAINT);
		//PVOID lpThis = this;
		//mpMessage->SetInputData(&lpThis,sizeof(lpThis));

		//luResult = mpElementManager->SendMessage(npRecipient,mpMessage);
	//}
	//else
	//	luResult = ERESULT_SUCCESS;
	// ���ڵ�һ�λ���ʱ�����Ʊ�
	if(loState.mbPaintItself != false)
	{
		// ���ͻ�����Ϣ
		mpMessage->SetMessageID(EMSG_PAINT);
		PVOID lpThis = this;
		mpMessage->SetInputData(&lpThis,sizeof(lpThis));

		luResult = mpElementManager->SendMessage(npRecipient,mpMessage);
	}


	if(ERESULT_SUCCEEDED(luResult))
		return ERESULT_ENUM_CHILD;

//	return luResult;
	// modified by ax ��ֹĳЩElementд�Ĳ���׼�����ش�������һ�ɸ�Ϊ���ɹ���ֹͣ����������Ԫ�أ�����������������Ļ���
	return ERESULT_SUCCESS;
}

// ����Paint����
ERESULT __stdcall CXD2dEngine::LeaveForPaint(IEinkuiIterator* npRecipient)
{
	ERESULT luResult = ERESULT_SUCCESS;
	LONG llCrtPaintLevel = -1;
	LONG llNext;

	// �ָ�������
	if(moRenderState.Top().mbHasClipRect != false)
	{
		mpTarget2D->PopAxisAlignedClip();
	}

	if(mpElementManager->GetProbeMode() == 1)
	{
		//D2D1_ANTIALIAS_MODE  ldOldMode = mpTarget2D->GetAntialiasMode();
		//mpTarget2D->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		mpTarget2D->DrawRectangle(D2D1::RectF(0.5f,0.5f,npRecipient->GetSizeX()-0.5f,npRecipient->GetSizeY()-0.5f),mpDbgTextBrush,1.0f);
		//mpTarget2D->SetAntialiasMode(ldOldMode);
	}

	llCrtPaintLevel = moRenderState.Top().mlCrtPtLevel;
	if(moRenderState.Top().mpEnhancer != NULL)
	{
		STEMS_ENHANCER_RENDER ldEnhanceRender;

		ldEnhanceRender.mpPaintBoard = this;
		ldEnhanceRender.mpDestElement = npRecipient;

		luResult = mpElementManager->SimpleSendMessage(moRenderState.Top().mpEnhancer,EMSG_RENDER_ENHANCER,&ldEnhanceRender,sizeof(ldEnhanceRender),NULL,0);
		if(luResult == ERESULT_REDO_RENDER_CONTENT)	// �ٴ�ִ�л���
			luResult = ERESULT_REDO_ENUM;
		else
			if(luResult != ERESULT_DIRECTX_ERROR)	// �����Dx���󣬾Ͳ��ܼ���ִ�л�����
				luResult = ERESULT_SUCCESS;	// ���Ե�һ���Եش������ǲ�Ӧ����ֹ����������Ļ���

	}


	moRenderState.Pop();

	// �����ǰ�������Ļ��Ʋ�Σ����һ���û��ִ����Ļ��Ʋ�Σ��򷵻�����ö��
	if(luResult != ERESULT_REDO_ENUM && ERESULT_SUCCEEDED(luResult))// ��������������������ö�٣��������ж���
	{
		llNext = ((CXuiIterator*)npRecipient)->GetNextPaintLevel(llCrtPaintLevel);
		if(llNext > llCrtPaintLevel)
		{
			luResult = ERESULT_REDO_ENUM;
			moRenderState.Top().mlPlLevelHost = llNext;
		}
	}

	return luResult;
}


// ����CaptureǰԤ����
ERESULT __stdcall CXD2dEngine::EnterForCapture(IEinkuiIterator* npRecipient)
{
	ERESULT luResult;
	CErdState loState;
	D2D1_POINT_2F ldPosition;
	D2D1_POINT_2F ldCenter;
	FLOAT lfAngle;
	CXuiIterator* lpRecipient;
	D2D1_RECT_F ldClipRect;

	// �����û�п�ʼ���������ǲ������ȣ�����Ѿ���ʼ���������Ƿ����أ��������账���״̬����ѹ��һ����״̬��Ȼ���˳�
	if(mbCapturing == false && mpToCapture->FindAncestor(npRecipient)==false || mbCapturing != false && npRecipient->IsVisible()==false)
	{
		loState.mpEnhancer = NULL;
		moRenderState.Push(loState);

		return ERESULT_STOP_ENUM_CHILD;
	}

	lpRecipient = dynamic_cast<CXuiIterator*>(npRecipient);

	if(mpToCapture == npRecipient)
	{
		mbCapturing = true;
		// ���Ծֲ�����ת����ֱ�Ӷ�λ����������ԭ��

		loState.mdOffset.x = CExFloat::Round(-mdCaptureRegion.left);
		loState.mdOffset.y = CExFloat::Round(-mdCaptureRegion.top);
		loState.mdTransLation = D2D1::Matrix3x2F::Translation(-mdCaptureRegion.left,-mdCaptureRegion.top);

		loState.mdWorld = loState.mdTransLation;
		loState.mdRotation = mdIdentyMatrix;
		loState.mfAlpha = 1.0f;
		loState.mpEnhancer = NULL;
		loState.mbRotated = false;

	}
	else
	{
		// ���㵱ǰ����������͸���ȣ�����ѹ��״̬ջ
		ldPosition = lpRecipient->GetPosition();
		lfAngle = lpRecipient->GetRotation(ldCenter);

		// �����λ�ƾͼ���λ�Ƶľ���
		if(ldPosition.x != 0.0f || ldPosition.y != 0.0f)
		{
			//loState.mdTransLation = moRenderState.Top().mdTransLation * D2D1::Matrix3x2F::Translation(ldPosition.x,ldPosition.y);
			loState.mdOffset.x = CExFloat::Round(moRenderState.Top().mdOffset.x + ldPosition.x);
			loState.mdOffset.y = CExFloat::Round(moRenderState.Top().mdOffset.y + ldPosition.y);
			loState.mdTransLation = D2D1::Matrix3x2F::Translation(loState.mdOffset.x,loState.mdOffset.y);
		}
		else
		{
			loState.mdTransLation = moRenderState.Top().mdTransLation;
			loState.mdOffset = moRenderState.Top().mdOffset;
		}

		// ���ϲ�Ԫ�ظ��ƾ���
		loState.mbRotated = moRenderState.Top().mbRotated;
		loState.mdRotation = moRenderState.Top().mdRotation;
		loState.mdWorld = loState.mdTransLation * loState.mdRotation;	// �����ϲ�Ԫ����ת��ľ�������ϲ�Ԫ�ض�û����ת��������ǳ��Ե�λ��
		//loState.mbTopDraw = (moRenderState.Top().mbTopDraw || lpRecipient->CheckStyle(EITR_STYLE_TOPDRAW));

		// �����ǰ�����µ���ת�趨
		if(lfAngle >= 0.01f)
		{
			// ���ȼ����µ�����λ��
			ldCenter = ldCenter * loState.mdWorld;
			//ldCenter = D2D1::Matrix3x2F::ReinterpretBaseType(&loState.mdWorld)->TransformPoint(ldCenter);��ת���˷�

			loState.mdRotation = loState.mdRotation * D2D1::Matrix3x2F::Rotation(lfAngle,ldCenter);	// ������ת���󵥶����
			loState.mdWorld = loState.mdTransLation * loState.mdRotation;
			loState.mbRotated = true;

		}

		loState.mfAlpha = moRenderState.Top().mfAlpha * lpRecipient->GetAlpha();
		if(loState.mfAlpha >= 0.9999f)
			loState.mfAlpha = 1.0f;

		loState.mpEnhancer = lpRecipient->GetEnhancer();
	}
	// �ܵ����ſ���
	loState.mdWorld =  loState.mdWorld * mdScalMatrixToCapture;

	moRenderState.Push(loState);

	// ���û����õ�����ת������
	mpTarget2D->SetTransform(loState.mdWorld);
	//lpRecipient->SaveWorldMatrix(loState.mdWorld); ��׽ͼ�񣬲��ܽ�����ת����������

	if(mbCapturing == false)	// ��û��ʼ��׽�������ϣ��ץȡ���� Ax Jul.09,2012
	{
		return ERESULT_ENUM_CHILD;
	}

	// ���ü�����
	if(npRecipient->GetVisibleRegion(ldClipRect)!=false)	
	{
		moRenderState.Top().mbHasClipRect = true;
		moRenderState.Top().mdClipRect = ldClipRect;
		mpTarget2D->PushAxisAlignedClip(ldClipRect,D2D1_ANTIALIAS_MODE_ALIASED);
	}


	// �����Ԫ��Ҫ������ʱ���أ���ʱ��������Ϣ
	if(lpRecipient->CheckStyle(EITR_STYLE_SNAPSHOT_HIDE) == false)
	{
		 //���ͻ�����Ϣ
		mpMessage->SetMessageID(EMSG_PAINT);
		PVOID lpThis = this;
		mpMessage->SetInputData(&lpThis,sizeof(lpThis));

		luResult = mpElementManager->SendMessage(npRecipient,mpMessage);

		if(ERESULT_SUCCEEDED(luResult))
			return ERESULT_ENUM_CHILD;
	}

	//	return luResult;
	// modified by ax ��ֹĳЩElementд�Ĳ���׼�����ش�������һ�ɸ�Ϊ���ɹ���ֹͣ����������Ԫ�أ�����������������Ļ���
	return ERESULT_SUCCESS;
}

// ����Capture����
ERESULT __stdcall CXD2dEngine::LeaveForCapture(IEinkuiIterator* npRecipient)
{
	ERESULT luResult = ERESULT_SUCCESS;

	if(mpToCapture == npRecipient)
	{
		mbCapturing = false;
	}

	// �ָ�������
	if(moRenderState.Top().mbHasClipRect != false)
	{
		mpTarget2D->PopAxisAlignedClip();
	}

	moRenderState.Pop();

	return luResult;
}

// ����EMSG_DISCARD_DEVICE_RESOURCEǰԤ����
ERESULT __stdcall CXD2dEngine::EnterForDiscardDeviceRes(IEinkuiIterator* npRecipient)
{
	mpElementManager->SimpleSendMessage(npRecipient,EMSG_DISCARD_DEVICE_RESOURCE,NULL,0,NULL,0);
	return ERESULT_ENUM_CHILD;
}

// ����EMSG_DISCARD_DEVICE_RESOURCE����
ERESULT __stdcall CXD2dEngine::LeaveForDiscardDeviceRes(IEinkuiIterator* npRecipient)
{
	return ERESULT_SUCCESS;
}

//��ȡ��ǰ����ͼ�񣬻�ȡ��HBITMAP�����ɵ��������ͷ�
HBITMAP CXD2dEngine::GetCurrentBitmap(LONG nlWidth,LONG nlHeight)
{
	BITMAPINFO BmpInfo;
	HBITMAP lhBmpHandle = NULL;
	UCHAR* lpArgb = NULL;
	HDC lhCompatibleDc = NULL;
	HBITMAP lhOldBitmap = NULL;

	ERESULT luResult;
	D2D1_COLOR_F ldOldBackColor;

	float lfWidth = 0;
	float lfHeight = 0;
	float lfW = (float)nlWidth / muEinkPanelW;
	float lfH = (float)nlHeight / muEinkPanelH;

	do
	{

		if (lfW > lfH)
		{
			lfWidth = muEinkPanelW * lfH;
			lfHeight = muEinkPanelH * lfH;
		}
		else
		{
			lfWidth = muEinkPanelW * lfW;
			lfHeight = muEinkPanelH * lfW;
		}

		// ����ִ�л��Ʋ���
		mdCaptureRegion.left = mdCaptureRegion.top = 0.0f;
		mdCaptureRegion.right = (FLOAT)muEinkPanelW;
		mdCaptureRegion.bottom = (FLOAT)muEinkPanelH;
		mdCaptureBriefSize.width = lfWidth;
		mdCaptureBriefSize.height = lfHeight;




		// �������ű�����׼��һ��ת������
		mdScalMatrixToCapture = D2D1::Matrix3x2F::Scale(mdCaptureBriefSize.width / (mdCaptureRegion.right - mdCaptureRegion.left), mdCaptureBriefSize.height / (mdCaptureRegion.bottom - mdCaptureRegion.top),
			D2D1::Point2F(0.0f, 0.0f));
		//mdScalMatrixToCapture = D2D1::Matrix3x2F::Identity();

		ldOldBackColor = mdBackColor;
		mdBackColor = D2D1::ColorF(D2D1::ColorF::Black, 0.0f);

		luResult = DoRender(GetCurrentTickCount(),false, EinkuiGetSystem()->GetSystemWidget()->GetHomePage());

		mdBackColor = ldOldBackColor;

		if (luResult != ERESULT_SUCCESS)
			break;

		ZeroMemory(&BmpInfo.bmiHeader, sizeof(BITMAPINFOHEADER));
		BmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		BmpInfo.bmiHeader.biWidth = nlWidth;
		BmpInfo.bmiHeader.biHeight = -(int)nlHeight;
		BmpInfo.bmiHeader.biPlanes = 1;
		BmpInfo.bmiHeader.biBitCount = 32;

		lhBmpHandle = CreateDIBSection(mhForeDc, &BmpInfo, DIB_RGB_COLORS, (void**)&lpArgb, NULL, 0);
		BREAK_ON_NULL(lhBmpHandle);

		lhCompatibleDc = CreateCompatibleDC(mhForeDc);
		BREAK_ON_NULL(lhCompatibleDc);

		lhOldBitmap = (HBITMAP)SelectObject(lhCompatibleDc, lhBmpHandle);
		::BitBlt(lhCompatibleDc, (nlWidth - (ULONG)lfWidth) / 2, (nlHeight - (ULONG)lfHeight) / 2, (ULONG)lfWidth, (ULONG)lfHeight, mhForeDc, 0, 0, SRCCOPY);

	} while (false);

	if (lhOldBitmap != NULL)
		SelectObject(lhCompatibleDc, lhOldBitmap);

	if (lhCompatibleDc != NULL)
		ReleaseDC(NULL, lhCompatibleDc);

	return lhBmpHandle;
}

// �趨�Ի溯�����趨��Xuiϵͳ���ڵ���Eink���ƣ�������rgb32�Ļ������ṩ���˴��趨�Ļص�����
void CXD2dEngine::SetCustomDraw(PXUI_CUSTOM_DRAW_CALLBACK CustomDraw)
{
	InterlockedExchangePointer(reinterpret_cast<PVOID*>(&mpCustomDraw), CustomDraw);
}

// ִ����������
IEinkuiBitmap* CXD2dEngine::TakeSnapshot(
	IEinkuiIterator* npToShot,
	const D2D1_RECT_F& crSourceRegion,	// ��������Ŀ��Ԫ�صľֲ�����ϵ
	const D2D_SIZE_F& crBriefSize,		// ����ͼ�ߴ磬���յĽ����һ������ͼ
	const FLOAT* ColorRGBA
)
{
	ERESULT luResult;
	D2D1_COLOR_F ldOldBackColor;
	IEinkuiBitmap* lpBitmap = NULL;
	// ����ִ�л��Ʋ���
	mdCaptureRegion = crSourceRegion;
	mdCaptureBriefSize = crBriefSize;

	// �������ű�����׼��һ��ת������
	mdScalMatrixToCapture = D2D1::Matrix3x2F::Scale(crBriefSize.width / (crSourceRegion.right - crSourceRegion.left), crBriefSize.height / (crSourceRegion.bottom - crSourceRegion.top),
		D2D1::Point2F(0.0f, 0.0f));

	if (ColorRGBA != NULL)
	{
		ldOldBackColor = mdBackColor;
		mdBackColor.r = ColorRGBA[0];
		mdBackColor.g = ColorRGBA[1];
		mdBackColor.b = ColorRGBA[2];
		mdBackColor.a = ColorRGBA[3];
	}

	luResult = DoRender(GetCurrentTickCount(),false, npToShot);

	if (ColorRGBA != NULL)
	{
		mdBackColor = ldOldBackColor;
	}

	if (luResult != ERESULT_SUCCESS)
		return NULL;

	char *	lpBitmapBuffer = NULL;
	LONG liBrfW = CExFloat::ToLong(crBriefSize.width);
	LONG liBrfH = CExFloat::ToLong(crBriefSize.height);
	LONG liStrideSrc = muEinkPanelW * 4;
	LONG liStrideDst = liBrfW * 4;

	// ��������ͼ���ڴ�
	lpBitmapBuffer = new char[liStrideDst * liBrfH + 4];
	RETURN_ON_NULL(lpBitmapBuffer,NULL);

	// ����ԭʼ����
	for (LONG Line = 0; Line < liBrfH; Line++)
		RtlCopyMemory(lpBitmapBuffer + liStrideDst*Line, mpForeBuffer + liStrideSrc*Line, liStrideDst);

	// ����λͼ����
	lpBitmap = CXD2dBitmap::CreateInstance(liBrfW,liBrfH, 4,liStrideDst, lpBitmapBuffer);

	CMM_SAFE_DELETE(lpBitmapBuffer);

	// �����½��Ķ���
	return lpBitmap;
}
