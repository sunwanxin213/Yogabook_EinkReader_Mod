/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "stdafx.h"
#include <math.h>
#include "shlwapi.h"
#include <string>
#include "CommonHeader.h"

#include "XgD2DBitmap.h"


#pragma comment(lib, "D2d1.lib")
#pragma comment(lib, "Windowscodecs.lib")

template <typename T>
inline void SafeRelease(T *&pI)
{
	if (NULL != pI)
	{
		pI->Release();
		pI = NULL;
	}
}


class CHICon {
	HICON mhIcon;
	int mnSize;
	static SIZE naPredefinedSize[6];

	BOOL GetSpecifiedSizedIcon(HINSTANCE hMod, WORD nwIconIdx, int niSize);
	BOOL GetIconByResId(wchar_t *npPeFile, int nwResId);
	BOOL GetIconByIndex(wchar_t *npPeFile, int nwResIdx);

public:
	CHICon();
	~CHICon();

	HICON GetIconHandle();
	int GetIconSize();

	// nwIconIdx >= 0��˵��Ϊͼ������
	// nwIconIdx <0�� ˵�������ֵΪͼ����ԴID
	BOOL GetSizedIcon(wchar_t *npPeFile, int nwIconIdx, int cx, int cy);

};


SIZE CHICon::naPredefinedSize[6] = {
	{0, 0},
	{256, 256},
	{128, 128},
	{64, 64},
	{32, 32},
	{16, 16} 
};

CHICon::CHICon()
{
	mhIcon = NULL;
	mnSize = 0;
}

CHICon::~CHICon()
{
	if(mhIcon) {
		DestroyIcon(mhIcon);
		mhIcon = NULL;
	}
}

HICON CHICon::GetIconHandle()
{
	return mhIcon;
}

int CHICon::GetIconSize()
{
	return mnSize;
}

BOOL CHICon::GetSizedIcon(wchar_t *npPeFile, int niIconIdx, int niXSize, int niYSize)
{
	BOOL fResult = FALSE;

	wchar_t wsRealFilePath[MAX_PATH+1] = L"";
	ExpandEnvironmentStrings(npPeFile, wsRealFilePath, MAX_PATH);

	naPredefinedSize[0].cx = niXSize;
	naPredefinedSize[0].cy = niYSize;

	if(niIconIdx < 0) {
		fResult = GetIconByResId(wsRealFilePath, -niIconIdx);
	} else {
		fResult = GetIconByIndex(wsRealFilePath, niIconIdx);
	}

	return fResult;
}

BOOL CHICon::GetIconByIndex(wchar_t *npPeFile, int nwResIdx)
{
	BOOL fReturn = FALSE;
	UINT nResId = 0;
	UINT uiCount;

	for(int i= 0; i< _countof(naPredefinedSize); i++) {
		uiCount = PrivateExtractIcons(npPeFile, nwResIdx, naPredefinedSize[i].cx, naPredefinedSize[i].cy, &mhIcon, &nResId, 1, LR_SHARED);
		if(uiCount == 1 && mhIcon != NULL) {
			fReturn = TRUE;
			break;
		}
	}

	return fReturn;
}

BOOL CHICon::GetIconByResId(wchar_t *npPeFile, int nwResId)
{
	BOOL fResult = FALSE;

	HINSTANCE hMod = LoadLibraryEx(npPeFile, NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE);
	if(hMod != NULL) {
		for(int i = 0; i< _countof(naPredefinedSize); i++) {
			if(GetSpecifiedSizedIcon(hMod, nwResId, naPredefinedSize[i].cx)) {
				mnSize = naPredefinedSize[i].cx;
				fResult = TRUE;
				break;
			}
		}
		FreeLibrary(hMod);
	}

	return fResult;
}

BOOL CHICon::GetSpecifiedSizedIcon(HINSTANCE hMod, WORD nwIconResId, int niSize)
{
	UINT nIconToExtract = 1;
	UINT nIconId = 0;

	mhIcon = (HICON)LoadImage(hMod, MAKEINTRESOURCE(nwIconResId), IMAGE_ICON, niSize, niSize, LR_SHARED);
	if(mhIcon)
		return TRUE;
	return FALSE;
}



DEFINE_BUILTIN_NAME(CXD2dBitmap)

CXD2dBitmap::CXD2dBitmap()
{
	m_hBitmap = NULL;
	m_pIDecoder = NULL;
	m_pWICBitmap = NULL;
	m_pIDecoder = NULL;
	m_pIDecoderFrame = NULL;
	m_pBmpScaler = NULL;
	mpTransparentInfo = NULL;
	m_lExtendLineX = m_lExtendLineY = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nScaleHeight = 0;
	m_nScaleWidth = 0;

	mpBitmap2D = NULL;
	meBitmapType = None;
	mcIsInit = false;

	// λͼ���ݳ�ʼ��
	memset(mswFileName, 0, MAX_PATH*sizeof(wchar_t));

	//mpText = NULL;
	//mpFont = NULL;

}

CXD2dBitmap::~CXD2dBitmap()
{
	CMM_SAFE_RELEASE(mpBitmap2D);
	CMM_SAFE_RELEASE(m_pIDecoder);
	CMM_SAFE_RELEASE(m_pIDecoderFrame);
	CMM_SAFE_RELEASE(m_pWICBitmap);
	CMM_SAFE_RELEASE(m_pBmpScaler);
	
	if(mpTransparentInfo)
		delete mpTransparentInfo;
	
	//if (mpText != NULL)
	//	delete mpText;
	//if (mpFont != NULL)
	//	delete mpFont;

}


ERESULT CXD2dBitmap::InitBitmap(IWICImagingFactory* npWICFactory, ID2D1Factory* npD2D1Factory, ID2D1RenderTarget* npRenderTarget)
{

	ERESULT luStatus = ERESULT_UNSUCCESSFUL;
	
	// ����Ѿ���ʼ������ֱ�ӷ��سɹ�
	if (mcIsInit == true)
		return ERESULT_SUCCESS;

	// �ݴ���Ⱦ����
	mpTempRenderTarget = npRenderTarget;

	UNREFERENCED_PARAMETER(npWICFactory);
	UNREFERENCED_PARAMETER(npD2D1Factory);
	UNREFERENCED_PARAMETER(npRenderTarget);

	switch(meBitmapType)
	{
	case CommonBitmap:
		{
//			luStatus = InitCommonBitmap(npWICFactory);
			luStatus = ERESULT_SUCCESS;
			break;
		}
	case IconBitmap:
		{
//			luStatus = InitIconBitmap(npWICFactory);
			luStatus = ERESULT_SUCCESS;
			break;
		}
	//case TextBitmap:
	//	{
	//		// ͨ������ʵ����λͼ����Ҫ���ú����ģ��
	//		luStatus = InitTextBitmap(npWICFactory, npD2D1Factory);
	//		break;
	//	}
	case MemoryBitmap:
		{
//			luStatus = InitMemoryBitmap(npRenderTarget);
			luStatus = ERESULT_SUCCESS;
			break;
		}
	case None:
		{
			luStatus = ERESULT_SUCCESS;
			break;
		}
	}
	
	// �����ʼ���ɹ�������λ
	if (ERESULT_SUCCEEDED(luStatus))
		mcIsInit = true;

	return luStatus;

}

// ���Ƴ�һ���µĶ���,���Ƴ����Ķ�����Ҫʹ�����Լ��ͷ�
IEinkuiBitmap* __stdcall CXD2dBitmap::DuplicateBitmap(void)
{

	IEinkuiBitmap* lpNewBitmap = NULL;
	void*	lpBitmapBuffer = NULL;
	bool	lbStatus = false;

	// �������������͵�λͼ����ͳһת�����ڴ���λͼ����
	do 
	{
		lpBitmapBuffer = new char[m_nWidth * m_nHeight*4+4];
		BREAK_ON_NULL(lpBitmapBuffer);

		lbStatus = GetBitmapBuffer(D2D1::RectF(0.0f, 0.0f, (float)m_nWidth, (float)m_nHeight), lpBitmapBuffer);
		BREAK_ON_FALSE(lbStatus);

		lpNewBitmap =  CXD2dBitmap::CreateInstance(m_nWidth, m_nHeight,4,m_nWidth*4,lpBitmapBuffer);

	} while (false);

	CMM_SAFE_DELETE(lpBitmapBuffer);

	return lpNewBitmap;

}


// ָ��λͼ��С���Լ�λͼ���ݣ�����һ��λͼ����
ULONG CXD2dBitmap::InitOnCreate(LONG nuWidth, LONG nuHeight,LONG PixelSize, LONG Stride, void* npRawData)
{
	ERESULT luStatus = ERESULT_UNSUCCESSFUL;
	meBitmapType = MemoryBitmap;
	IWICImagingFactory* lpWicFactory = NULL; 

	do 
	{
		// ��ϵͳע�������������ͳһ�ͷ��豸�����Դ
		CEinkuiSystem::gpXuiSystem->GetBitmapList().RegisteBitmap(this);

		BREAK_ON_NULL(npRawData);

		if (PixelSize < 3 || PixelSize > 4)
		{
			luStatus = ERESULT_BMP_IVLDFORMAT;
			break;
		}

		// ��ȡWIC������
		lpWicFactory = EinkuiGetSystem()->GetWICFactory();
		BREAK_ON_NULL(lpWicFactory);

		// ��WIC�ӿڴ��ڴ�����ֱ������λͼ
		luStatus = EinkuiGetSystem()->GetWICFactory()->CreateBitmapFromMemory(
			static_cast<unsigned int>(nuWidth), static_cast<unsigned int>(nuHeight),
			(PixelSize==3?GUID_WICPixelFormat24bppBGR:GUID_WICPixelFormat32bppPBGRA),
			Stride, nuWidth*nuHeight*PixelSize, (PBYTE)npRawData,
			&m_pWICBitmap);

		BREAK_ON_FAILED(luStatus);
		BREAK_ON_NULL(m_pWICBitmap);

		// ����WIC�ӿڻ�ȡͼƬ���
		GetBitmapDimension();
		GenTransparentInfo();

		luStatus = ERESULT_SUCCESS;

	} while (false);


	return luStatus;
}

ERESULT CXD2dBitmap::InitMemoryBitmap(ID2D1RenderTarget* npRenderTarget)
{

	ERESULT luStatus = ERESULT_UNSUCCESSFUL;

	// �ڴ���λͼ��û��Ҫ���ú����ģ��
	do 
	{
		UNREFERENCED_PARAMETER(npRenderTarget);

		luStatus = ERESULT_SUCCESS;
	} while (false);

	return luStatus;
}

// ͨ��Icon����λͼ����
ULONG CXD2dBitmap::InitOnCreate(wchar_t *npPeFileName, int niIconIdx, int niXSize, int niYSize)
{

	HRESULT luResult;
	CHICon oIcon;

	do 
	{
		// ��ϵͳע�������������ͳһ�ͷ��豸�����Դ
		CEinkuiSystem::gpXuiSystem->GetBitmapList().RegisteBitmap(this);

		BREAK_ON_FALSE(oIcon.GetSizedIcon(npPeFileName, niIconIdx, niXSize, niYSize));
		
		// ͨ��WIC�ӿڣ���ICONͼ�괴��λͼ
		luResult = EinkuiGetSystem()->GetWICFactory()->CreateBitmapFromHICON(oIcon.GetIconHandle(), &m_pWICBitmap);
		BREAK_ON_FAILED(luResult);

		luResult = m_pWICBitmap->GetSize(&m_nWidth, &m_nHeight);
		BREAK_ON_FAILED(luResult);

		luResult = GenTransparentInfo();

	} while (false);

	return luResult;

}

// ��ʼ��ICON��λͼ����
ERESULT CXD2dBitmap::InitIconBitmap(IWICImagingFactory* npWICFactory)
{

	ERESULT luStatus = ERESULT_UNSUCCESSFUL;

	// ICON��λͼ��û��Ҫ���ú����ģ��
	do 
	{

		luStatus = ERESULT_SUCCESS;
	} while (false);

	return luStatus;

}

// ͨ��λͼ�ļ�ʵ����λͼ����
ULONG CXD2dBitmap::InitOnCreate(IN const wchar_t *npFileName) 
{
	// ��ϵͳע�������������ͳһ�ͷ��豸�����Դ
	CEinkuiSystem::gpXuiSystem->GetBitmapList().RegisteBitmap(this);

	meBitmapType = CommonBitmap;
	memcpy(mswFileName, npFileName, (int)wcslen(npFileName)*sizeof(wchar_t));

	return InitCommonBitmap(EinkuiGetSystem()->GetWICFactory());

}

// ��ʼ����ͨλͼ����
ERESULT CXD2dBitmap::InitCommonBitmap(IWICImagingFactory* npWICFactory)
{

	m_nScaleHeight = 0;
	m_nScaleWidth = 0;

	ULONG ulResult = ERESULT_UNSUCCESSFUL;
	//////////////////////////////////////////////////////////////////////////
	// ���ļ�����IWICBitmapDecoder�ӿ�
	SafeRelease(m_pIDecoderFrame);
	m_hResult = npWICFactory->CreateDecoderFromFilename(
		mswFileName, 
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnDemand,
		&m_pIDecoder);

	if(SUCCEEDED(m_hResult)) {
		//////////////////////////////////////////////////////////////////////////
		// ��IWICBitmapDecoder�л�ȡ��һ֡
		SafeRelease(m_pIDecoderFrame);
		m_hResult = m_pIDecoder->GetFrame(0, &m_pIDecoderFrame);
		if(SUCCEEDED(m_hResult)) {
			//////////////////////////////////////////////////////////////////////////
			// ��IWICBitmapDecoderת��ΪIWICBitmap�ӿ�
			SafeRelease(m_pWICBitmap);
			m_hResult = npWICFactory->CreateBitmapFromSource(
				m_pIDecoderFrame,
				WICBitmapCacheOnDemand,
				&m_pWICBitmap); 
			if(SUCCEEDED(m_hResult)) 
			{
				//////////////////////////////////////////////////////////////////////////
				// ��ȡIWICBitmapLock����ȡλͼ�����Ϣ
				m_hResult = m_pWICBitmap->GetSize(&m_nWidth, &m_nHeight);
				if(SUCCEEDED(m_hResult)) 
				{
					ulResult = GenTransparentInfo();
				}
				else
					ulResult = ERESULT_BMP_ERRGETSIZE;
			} else
				ulResult = ERESULT_BMP_CREATEWICBMP;
		} else 
		{
			ulResult = ERESULT_BMP_GETFRAME;
		}
	} else {
		ulResult = ERESULT_BMP_CREATEDECODER;
	}

	return ulResult;

}

//	�������û���ù��������� ax nov.28,2017
//ULONG CXD2dBitmap::InitOnCreate(IN const wchar_t *npText, IN DWORD dwTextColor, IN const wchar_t *npFont, IN DWORD dwFontSize) 
//{
//	// ��ϵͳע�������������ͳһ�ͷ��豸�����Դ
//	CEinkuiSystem::gpXuiSystem->GetBitmapList().RegisteBitmap(this);
//
//	if(npText == NULL || npFont == NULL)
//		return ERESULT_BMP_CREATEDWRITEFAC;
//
//	meBitmapType = TextBitmap;
//
//	// ��������������ռ�õ��ڴ�
//	mpText = new wchar_t[(int)wcslen(npText)];
//	memcpy(mpText, npText, (int)wcslen(npText)*sizeof(wchar_t));
//
//	mpFont = new wchar_t[(int)wcslen(npFont)];
//	memcpy(mpFont, npFont, (int)wcslen(npFont)*sizeof(wchar_t));
//
//	mdwTextColor = dwTextColor;
//	mdwFontSize = dwFontSize;
//		
//	return 0;
////	return InitTextBitmap(EinkuiGetSystem()->GetWICFactory(), EinkuiGetSystem()->GetD2dFactory());
//
//}

//// ��ʼ��������λͼ����			�������û���ù��������� ax nov.28,2017
//ERESULT CXD2dBitmap::InitTextBitmap(IWICImagingFactory* npWICFactory, ID2D1Factory* npD2D1Factory)
//{
//	ID2D1RenderTarget* lpBitmapRT = NULL;
//	IDWriteTextFormat* lpTextFormat = NULL;
//	IDWriteTextLayout* lpTextLayout = NULL;
//	IDWriteFactory* lpDWriteFactory = NULL;
//	ID2D1SolidColorBrush *lpBlackBrush = NULL;
//
//
//	__try {
//		// Create a DirectWrite factory.
//		m_hResult = DWriteCreateFactory(
//			DWRITE_FACTORY_TYPE_SHARED,
//			__uuidof(lpDWriteFactory),
//			reinterpret_cast<IUnknown **>(&lpDWriteFactory)
//			);
//		if(FAILED(m_hResult))
//			return ERESULT_BMP_CREATEDWRITEFAC;
//		
//		FLOAT lfDIPs = 0.0f;
//		FLOAT dpiX = 96.0f;
//		FLOAT dpiY = 96.0F;
//
//		//EinkuiGetSystem()->GetD2dFactory()->GetDesktopDpi(&dpiX, &dpiY); Modified by Ax
//		lfDIPs = mdwFontSize / (dpiX / 96.0f);
//		m_hResult = lpDWriteFactory->CreateTextFormat(
//			mpFont,
//			NULL,
//			DWRITE_FONT_WEIGHT_NORMAL,
//			DWRITE_FONT_STYLE_NORMAL,
//			DWRITE_FONT_STRETCH_NORMAL,
//			lfDIPs,
//			L"", //locale
//			&lpTextFormat);
//		if(FAILED(m_hResult))
//			return ERESULT_BMP_CREATETEXTFMT;
//		lpTextFormat->SetTextAlignment(/*DWRITE_TEXT_ALIGNMENT_CENTER*/DWRITE_TEXT_ALIGNMENT_LEADING);
//		lpTextFormat->SetParagraphAlignment(/*DWRITE_PARAGRAPH_ALIGNMENT_CENTER*/DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
//
//		m_hResult = lpDWriteFactory->CreateTextLayout(mpText, (UINT32)wcslen(mpText),
//			lpTextFormat, 500, 500, &lpTextLayout);
//		if(FAILED(m_hResult))
//			return ERESULT_BMP_CREATETEXTLAYOUT;
//
//		DWRITE_TEXT_METRICS dtm;
//		m_hResult = lpTextLayout->GetMetrics(&dtm);
//		if(FAILED(m_hResult))
//			return ERESULT_BMP_GETMETRICS;
//
//		m_hResult = npWICFactory->CreateBitmap(static_cast<UINT>(dtm.width+1),
//			static_cast<UINT>(dtm.height + 1), // ȡ��
//			GUID_WICPixelFormat32bppPBGRA,
//			WICBitmapCacheOnLoad,
//			&m_pWICBitmap);
//		if(FAILED(m_hResult)) {
//			return ERESULT_BMP_CREATEWICBMP;
//		}
//
//		D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(
//			DXGI_FORMAT_B8G8R8A8_UNORM,
//			D2D1_ALPHA_MODE_PREMULTIPLIED);
//		m_hResult = npD2D1Factory->CreateWicBitmapRenderTarget(
//			m_pWICBitmap,
//			D2D1::RenderTargetProperties(),
//			&lpBitmapRT);
//		if(FAILED(m_hResult))
//			return ERESULT_BMP_CREATEWICBMPRT;
//
//		m_hResult = lpBitmapRT->CreateSolidColorBrush(
//			D2D1::ColorF(/*D2D1::ColorF::Red*/mdwTextColor),
//			&lpBlackBrush
//			);
//
//		//lpBitmapRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE); ??? �Ƿ���Ҫ��ӣ�������Ǹ�˭�õģ�Jun.14,2012
//
//		D2D1_SIZE_F renderTargetSize = lpBitmapRT->GetSize();
//		lpBitmapRT->BeginDraw();
//		lpBitmapRT->DrawText(mpText, 
//			(UINT32)wcslen(mpText),
//			lpTextFormat,
//			D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
//			lpBlackBrush);
//		lpBitmapRT->EndDraw();
//
//		GetBitmapDimension();
//
//		GenTransparentInfo();
//	}
//	__finally {
//		CMM_SAFE_RELEASE(lpBitmapRT);
//		CMM_SAFE_RELEASE(lpTextFormat);
//		CMM_SAFE_RELEASE(lpTextLayout);
//		CMM_SAFE_RELEASE(lpDWriteFactory);
//		CMM_SAFE_RELEASE(lpBlackBrush);
//	}
//
//	return ERESULT_SUCCESS;
//
//}

int __stdcall CXD2dBitmap::Release(void)
{

	int liCount = cmmBaseObject<CXD2dBitmap, IEinkuiBitmap, GET_BUILTIN_NAME(CXD2dBitmap)>::Release();
	// �������ֵ��0����ʾ��Ҫ��ɾ��
	if(liCount == 0)
	{
		CEinkuiSystem::gpXuiSystem->GetBitmapList().UnregisteBitmap(this);
	}

	return liCount;

}


ERESULT CXD2dBitmap::GenTransparentInfo() {
	ULONG ulResult = ERESULT_UNSUCCESSFUL;

	if(mpTransparentInfo) {
		delete mpTransparentInfo;
		mpTransparentInfo = NULL;
		return ulResult;
	}
	mpTransparentInfo = new CBitArray(m_nHeight * m_nWidth);
	if(mpTransparentInfo == NULL) {
		return ERESULT_INSUFFICIENT_RESOURCES;
	}

	WICRect rcLock = { 0, 0, (INT)m_nWidth, (INT)m_nHeight };
	IWICBitmapLock *pLocker = NULL;
	m_hResult = m_pWICBitmap->Lock(
		&rcLock, WICBitmapLockRead, 
		&pLocker);
	if(SUCCEEDED(m_hResult)) {
		WICPixelFormatGUID pf;
		m_hResult = pLocker->GetPixelFormat(&pf);
		if(SUCCEEDED(m_hResult)) {
			UINT nSize = 0;
			BYTE *pBuf = NULL;
			m_hResult = pLocker->GetDataPointer(&nSize, &pBuf);
			if(SUCCEEDED(m_hResult)) {
				// ʹ�����淽���ж�ÿ�����͸����
				// ����ÿ����ѭ�����ж�GUIDӰ��ͼ�μ���Ч��
				// �����뿴�����Ƚ�����
				if(IsEqualGUID(GUID_WICPixelFormat32bppBGRA, pf)) {
					for(UINT n=0; n< m_nWidth*m_nHeight; n++) {
						mpTransparentInfo->SetBit(n, (pBuf[n*4 + 3] > TRANSPARENT_THRESHOLD_VALUE) ? true : false);
					}
				} else if(IsEqualGUID(GUID_WICPixelFormat32bppPBGRA, pf)) {
					for(UINT n=0; n< m_nWidth*m_nHeight; n++) {
						mpTransparentInfo->SetBit(n, (pBuf[n*4 + 3] > TRANSPARENT_THRESHOLD_VALUE) ? true : false);
					}
				} else if(IsEqualGUID(GUID_WICPixelFormat32bppRGBA, pf)) {
					for(UINT n=0; n< m_nWidth*m_nHeight; n++) {
						mpTransparentInfo->SetBit(n, (pBuf[n*4 + 3] > TRANSPARENT_THRESHOLD_VALUE) ? true : false);
					}
				} else if(IsEqualGUID(GUID_WICPixelFormat32bppPRGBA, pf)) {
					for(UINT n=0; n< m_nWidth*m_nHeight; n++) {
						mpTransparentInfo->SetBit(n, (pBuf[n*4 + 3] > TRANSPARENT_THRESHOLD_VALUE) ? true : false);
					}
				} else {
					for(UINT n=0; n< m_nWidth*m_nHeight; n++) {
						mpTransparentInfo->SetBit(n, true);
					}
				}
				ulResult = ERESULT_SUCCESS;
			} else {
				ulResult = ERESULT_BMP_GETBMPBUF;
			}
		} else {
			for(UINT n=0; n< m_nWidth*m_nHeight; n++) {
				mpTransparentInfo->SetBit(n, true);
			}
		}
		pLocker->Release();
	} else {
		ulResult = ERESULT_BMP_WICLOCKBMP;
	}

	return ulResult;

}

ULONG CXD2dBitmap::GetBitmapDimension() {
	ULONG ulResult = ERESULT_UNSUCCESSFUL;
	IWICBitmapLock *lpLock = NULL;
	m_hResult = m_pWICBitmap->Lock(NULL, WICBitmapLockRead, &lpLock);
	if(SUCCEEDED(m_hResult)) {
		m_hResult = lpLock->GetSize(&m_nWidth, &m_nHeight);
		if(SUCCEEDED(m_hResult)) {
			ulResult = ERESULT_SUCCESS;
		} else {
			m_nHeight = 0;
			m_nWidth = 0;
			ulResult = ERESULT_BMP_ERRGETSIZE;
		}
		lpLock->Release();
	} else
		ulResult = ERESULT_BMP_WICLOCKBMP;
	return ulResult;
}



UINT __stdcall CXD2dBitmap::GetWidth() 
{
	return m_nScaleWidth ? m_nScaleWidth : m_nWidth;
}

UINT __stdcall CXD2dBitmap::GetHeight() 
{
	return m_nScaleHeight ? m_nScaleHeight : m_nHeight;
}


ERESULT __stdcall CXD2dBitmap::GetWICObject(OUT IWICBitmap** ppWicBitmap)
{
	*ppWicBitmap = m_pWICBitmap;
	return ERESULT_SUCCESS;	
}

ERESULT __stdcall CXD2dBitmap::GetD2DObject(IN ID2D1RenderTarget *npRT, OUT ID2D1Bitmap **nppParentBitmap)
{

	ERESULT luResult;

	if(mpBitmap2D == NULL)
	{
		luResult = CreateD2DObjFromBmpSource(npRT, m_pWICBitmap/*m_pIDecoderFrame*/,&mpBitmap2D);
		if(ERESULT_FAILED(luResult))
			return luResult;
	}

	if(mpBitmap2D == NULL)
		return ERESULT_INSUFFICIENT_RESOURCES;

	mpBitmap2D->AddRef();

	*nppParentBitmap = mpBitmap2D;

	return ERESULT_SUCCESS;
}

ERESULT __stdcall CXD2dBitmap::GetPixel(IN DWORD x, IN DWORD y, DWORD &nPixel)
{
	ULONG ulResult = ERESULT_UNSUCCESSFUL;

	if(mpTransparentInfo) {
		bool lfIsTrans = mpTransparentInfo->GetBit(y*m_nWidth + x);
		nPixel = lfIsTrans;
		ulResult = ERESULT_SUCCESS;
	}
	return ulResult;
}


ERESULT __stdcall CXD2dBitmap::GetBmpInfo(OUT BITMAP* npBmpInfo)
{
	return ERESULT_UNSUCCESSFUL; // Not implemented yet.

	if(NULL == m_hBitmap) {
		return -1;
	}
	::GetObject(m_hBitmap, sizeof(BITMAP), npBmpInfo);
	return 0;
}


ERESULT CXD2dBitmap::CreateD2DObjFromBmpSource(IN ID2D1RenderTarget *npRT, IN IWICBitmapSource *npSourceBmp, OUT ID2D1Bitmap **nppParentBitmap) {
	ULONG ulResult = ERESULT_UNSUCCESSFUL;

	IWICBitmapSource* pWicBmpToGet = npSourceBmp;
	if(pWicBmpToGet == NULL)
		pWicBmpToGet = m_pWICBitmap;

	if(m_nScaleWidth && m_nScaleHeight && (NULL != m_pBmpScaler))
	{
		pWicBmpToGet = m_pBmpScaler;
	}
	// ����쳣����

	try
	{
		IWICFormatConverter *lpFormatConverter = NULL;

		do 
		{
			m_hResult = EinkuiGetSystem()->GetWICFactory()->CreateFormatConverter(&lpFormatConverter);
			BREAK_ON_FAILED(m_hResult);

			m_hResult = lpFormatConverter->Initialize(
				pWicBmpToGet,				    // Input bitmap to convert
				GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
				WICBitmapDitherTypeNone,         // Specified dither pattern
				NULL,                            // Specify a particular palette 
				0.0f,                             // Alpha threshold
				WICBitmapPaletteTypeCustom       // Palette translation type
				);
			BREAK_ON_FAILED(m_hResult);

			m_hResult = npRT->CreateBitmapFromWicBitmap(lpFormatConverter, NULL, nppParentBitmap);
			BREAK_ON_FAILED(m_hResult);


			ulResult = ERESULT_SUCCESS;

		} while (false);

		SafeRelease(lpFormatConverter);
		
	}
	catch (...)
	{
		*nppParentBitmap = NULL;
	}

	return ulResult;
}


// ��ϵͳ�ڲ����ã�����Direct2D���豸�����Դ
void __stdcall CXD2dBitmap::DiscardsBitmapResource(void)
{
	CMM_SAFE_RELEASE(mpBitmap2D);
	CMM_SAFE_RELEASE(m_pBmpScaler);
	CMM_SAFE_RELEASE(m_pIDecoder);
	CMM_SAFE_RELEASE(m_pIDecoderFrame);

	mcIsInit = false;
}


ERESULT __stdcall CXD2dBitmap::Scale(UINT npWidth, UINT npHeight) {
	ULONG ulResult = ERESULT_UNSUCCESSFUL;

	m_nScaleWidth = npWidth;
	m_nScaleHeight = npHeight;

	DiscardsBitmapResource();

	IWICBitmapSource* pWicBmpToGet = m_pIDecoderFrame;
	if(pWicBmpToGet == NULL)
		pWicBmpToGet = m_pWICBitmap;

	HRESULT m_hResult = EinkuiGetSystem()->GetWICFactory()->CreateBitmapScaler(&m_pBmpScaler);
	if(SUCCEEDED(m_hResult)) {
		m_hResult = m_pBmpScaler->Initialize(
			pWicBmpToGet,
			m_nScaleWidth,
			m_nScaleHeight,
			WICBitmapInterpolationModeFant);
		if(SUCCEEDED(m_hResult)) {
			pWicBmpToGet = m_pBmpScaler;
			ulResult = ERESULT_SUCCESS;
		}
		else
			ulResult = ERESULT_BMP_INITSCALER;
	} else {
		ulResult = ERESULT_BMP_CREATESCALER;
	}
	GetBitmapDimension();
	GenTransparentInfo();

	return ulResult;
}

//������չ��
void __stdcall CXD2dBitmap::SetExtendLine(IN LONG nlX,IN LONG nlY)
{
	m_lExtendLineX = nlX;
	m_lExtendLineY = nlY;
}

//��ȡ������չ��
LONG __stdcall CXD2dBitmap::GetExtnedLineX(void)
{
	return m_lExtendLineX;
}

//��ȡ������չ��
LONG __stdcall CXD2dBitmap::GetExtnedLineY(void)
{
	return m_lExtendLineY;
}


// ��ȡָ�������λͼ����
bool __stdcall CXD2dBitmap::GetBitmapBuffer(IN D2D1_RECT_F ndSrcRect, OUT void* npRawData)
{
	bool lbStatus = false;
	HRESULT	hr;
	IWICFormatConverter *lpFormatConverter = NULL;

	do 
	{
		// ���ͼ�ζ���û�г�ʼ������ֱ�ӷ���ʧ��
		//BREAK_ON_FALSE(mcIsInit);
		BREAK_ON_NULL(m_pWICBitmap);
		BREAK_ON_NULL(npRawData);

		WICRect ldSrcWicRect;
		ldSrcWicRect.X = (INT)ndSrcRect.left;
		ldSrcWicRect.Y = (INT)ndSrcRect.top;
		ldSrcWicRect.Width = (INT)(ndSrcRect.right - ndSrcRect.left);
		ldSrcWicRect.Height = (INT)(ndSrcRect.bottom - ndSrcRect.top);

		// �ü������ܴ���ԭʼ����
		BREAK_ON_FALSE(m_nWidth >= (UINT)ldSrcWicRect.Width);
		BREAK_ON_FALSE(m_nHeight >= (UINT)ldSrcWicRect.Height);

		// ���Ƚ��룬���ʱת��BGRA�����ظ�ʽ
		hr = EinkuiGetSystem()->GetWICFactory()->CreateFormatConverter(&lpFormatConverter);
		BREAK_ON_FAILED(hr);

		hr = lpFormatConverter->Initialize(
			m_pWICBitmap,					 // Input bitmap to convert
			GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
			WICBitmapDitherTypeNone,         // Specified dither pattern
			NULL,                            // Specify a particular palette 
			0.f,                             // Alpha threshold
			WICBitmapPaletteTypeCustom       // Palette translation type
			);
		BREAK_ON_FAILED(hr);

		// �ڶ���������stride,���������)����ʾλͼ��һ�е��������Ƕ��٣��������������еķָ�
		hr = lpFormatConverter->CopyPixels(
			&ldSrcWicRect, 
			ldSrcWicRect.Width*4,		
			ldSrcWicRect.Width*ldSrcWicRect.Height*4, 
			(BYTE*)npRawData
			);
		BREAK_ON_FAILED(hr);
		
		lbStatus = true;

	} while (false);


	CMM_SAFE_RELEASE(lpFormatConverter);

	return lbStatus;
}


bool __stdcall CXD2dBitmap::SaveAsThumbnail(IN LONG nuWidth, IN LONG nuHeight,  IN D2D1_RECT_F ndSampleRect, const wchar_t* nswOutputFilePath)
{
	bool lbStatus = false;
	ERESULT luResult = ERESULT_UNSUCCESSFUL;
	HRESULT hr = S_OK;
		
	IWICBitmap *lpWICBitmap = NULL;
	ID2D1RenderTarget *lpRT = NULL;
	IWICBitmapEncoder *lpEncoder = NULL;
	IWICBitmapFrameEncode *lpFrameEncode = NULL;
	IWICStream *lpStream = NULL;
	ID2D1Bitmap* lpBmp2d = NULL;	

	const UINT sc_bitmapWidth = (ULONG)nuWidth;
	const UINT sc_bitmapHeight = (ULONG)nuHeight;

	do 
	{
		// ����WIC�����RT����
		hr = EinkuiGetSystem()->GetWICFactory()->CreateBitmap(
			sc_bitmapWidth,
			sc_bitmapHeight,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapCacheOnLoad,
			&lpWICBitmap
			);
		BREAK_ON_FAILED(hr);

		hr = EinkuiGetSystem()->GetD2dFactory()->CreateWicBitmapRenderTarget(
			lpWICBitmap,
			D2D1::RenderTargetProperties(),
			&lpRT
			);
		BREAK_ON_FAILED(hr);

		// ����ԭʼͼ���WIC���󣬴���D2Dλͼ����
		luResult = CreateD2DObjFromBmpSource(lpRT, m_pWICBitmap,&lpBmp2d);
		BREAK_ON_FAILED(luResult);

		// ��ԭʼλͼ���Ƶ�RenderTarget,���ƺ�����������ͼ��Сһ��
		{
			lpRT->BeginDraw();

			// ����Ҫ����������봴����͸������Ϣ��ͼ������Ҫ��alphaȫ����0
			lpRT->Clear(D2D1::ColorF(D2D1::ColorF::White, 0.0f));

			lpRT->DrawBitmap(
				lpBmp2d,
				D2D1::RectF(0.0f,0.0f,(FLOAT)nuWidth,(FLOAT)nuHeight),		// Ŀ�������С
				1.0f,
				(D2D1_BITMAP_INTERPOLATION_MODE)ESPB_DRAWBMP_LINEAR,
				ndSampleRect		// ԭʼ�����С
				);

			lpRT->EndDraw();
		}

		// ���浽�ļ������ڣ�RenderTarget�ϵ����ݾ�������ͼ
		hr = EinkuiGetSystem()->GetWICFactory()->CreateStream(&lpStream);
		BREAK_ON_FAILED(hr);

		//WICPixelFormatGUID format = GUID_WICPixelFormat32bppPBGRA;
		GUID containerFormat = GUID_ContainerFormatBmp;
		WICPixelFormatGUID format = GUID_WICPixelFormatDontCare;

		hr = lpStream->InitializeFromFilename(nswOutputFilePath, GENERIC_WRITE);
		BREAK_ON_FAILED(hr);

		std::wstring lpFileExtention = PathFindExtension(nswOutputFilePath);
		
		// �����ļ���׺���жϱ����ʽ
		if (lpFileExtention.compare(L".jpg") == 0 ||
			lpFileExtention.compare(L".jpeg") == 0 ||
			lpFileExtention.compare(L".jpe") == 0 ||
			lpFileExtention.compare(L".jfif") == 0)
		{
			containerFormat = GUID_ContainerFormatJpeg;
		}
		else if (lpFileExtention.compare(L".tif") == 0 ||
			lpFileExtention.compare(L".tiff") == 0)
		{
			containerFormat = GUID_ContainerFormatTiff;
		}
		else if (lpFileExtention.compare(L".gif") == 0)
		{
			containerFormat = GUID_ContainerFormatGif;
		}
		else if (lpFileExtention.compare(L".png") == 0)
		{
			containerFormat = GUID_ContainerFormatPng;
		}
		else if (lpFileExtention.compare(L".wmp") == 0)
		{
			containerFormat = GUID_ContainerFormatWmp;
		}
		else if(lpFileExtention.compare(L".bmp") == 0)
		{
			containerFormat = GUID_ContainerFormatBmp;
		}

		hr = EinkuiGetSystem()->GetWICFactory()->CreateEncoder(containerFormat, NULL, &lpEncoder);
		BREAK_ON_FAILED(hr);

		hr = lpEncoder->Initialize(lpStream, WICBitmapEncoderNoCache);
		BREAK_ON_FAILED(hr);		

		hr = lpEncoder->CreateNewFrame(&lpFrameEncode, NULL);
		BREAK_ON_FAILED(hr);

		hr = lpFrameEncode->Initialize(NULL);
		BREAK_ON_FAILED(hr);		

		hr = lpFrameEncode->SetSize(sc_bitmapWidth, sc_bitmapHeight);
		BREAK_ON_FAILED(hr);	

		hr = lpFrameEncode->SetPixelFormat(&format);
		BREAK_ON_FAILED(hr);

		hr = lpFrameEncode->WriteSource(lpWICBitmap, NULL);
		BREAK_ON_FAILED(hr);

		hr = lpFrameEncode->Commit();
		BREAK_ON_FAILED(hr);	

		hr = lpEncoder->Commit();
		BREAK_ON_FAILED(hr);		
		
		lbStatus = true;

	} while (false);


	// �ͷ���Դ
	CMM_SAFE_RELEASE(lpWICBitmap);
	CMM_SAFE_RELEASE(lpRT);
	CMM_SAFE_RELEASE(lpEncoder);
	CMM_SAFE_RELEASE(lpFrameEncode);
	CMM_SAFE_RELEASE(lpStream);
	CMM_SAFE_RELEASE(lpBmp2d);

	return lbStatus;
}
