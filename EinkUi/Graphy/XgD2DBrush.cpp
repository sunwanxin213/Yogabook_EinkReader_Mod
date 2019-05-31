/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#include "stdafx.h"

#include "CommonHeader.h"

#include "XgD2DBrush.h"


DEFINE_BUILTIN_NAME(CXD2DBrush)


CXD2DBrush::CXD2DBrush()
{
	mpSolidBrush = NULL;
	mpLinearGradientBrush = NULL;
	mpRadialGradientBrush = NULL;
	mpBitmapBrush = NULL;
	mpStrokeStyle = NULL;
	
	mcStrokeCreate = false;
	mfStrokeWidth = 1.0f;

	mpDashes = NULL;
	muCount = 0;



}

// ��Դ�ͷ�
CXD2DBrush::~CXD2DBrush()
{
	CMM_SAFE_RELEASE(mpSolidBrush);
	CMM_SAFE_RELEASE(mpLinearGradientBrush);
	CMM_SAFE_RELEASE(mpRadialGradientBrush);
	CMM_SAFE_RELEASE(mpBitmapBrush);
	CMM_SAFE_RELEASE(mpStrokeStyle);

	if (mpDashes != NULL)
		delete mpDashes;

}

// ʵ������ӿں���Ҫ������������˳�ʱ��������ͷ�����
int CXD2DBrush::Release()
{
	int liCount = cmmBaseObject<CXD2DBrush, IEinkuiBrush, GET_BUILTIN_NAME(CXD2DBrush)>::Release();
	// �������ֵ��0����ʾ��Ҫ��ɾ��
	if(liCount == 0)
	{
		CEinkuiSystem::gpXuiSystem->GetBrushList().UnregisteBrush(this);
	}

	return liCount;	
}


ULONG CXD2DBrush::InitOnCreate(XuiBrushType niBrushType, D2D1_COLOR_F noColor)
{
	miBrushType = niBrushType;
	moSolidBrushColor = noColor;
		
	return 0;
}

ULONG CXD2DBrush::InitOnCreate(XuiBrushType niBrushType, D2D1_GRADIENT_STOP* npGradientStop, ULONG nuCount, 
	D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES ndLinearGradientBrushProperties)
{
	miBrushType = niBrushType;

	for (ULONG liLoop = 0; liLoop < nuCount; liLoop++)
	{
		mdGradientStop.Insert(-1,npGradientStop[liLoop]);
	}

	mdLinearGradientBrushProperties = ndLinearGradientBrushProperties;

	muCount = nuCount;

	return 0;
}

// ����ʹ��ʱ����Ҫ������ˢ��Դ����ƽ̨�����Դ������ʱ��ͨ��ƽ̨�����߰Ѳ���npRenderTarget������ʼ������
ULONG CXD2DBrush::InitBrush(ID2D1RenderTarget* npRenderTarget,  ID2D1Factory* npD2D1Factory)
{
	ULONG luStatus = 0;
	HRESULT hr = 0;

	do 
	{
		switch(miBrushType)
		{
		case XuiStroke:
		case XuiSolidBrush:
			{
				if (mpSolidBrush == NULL)
				{
					hr = npRenderTarget->CreateSolidColorBrush(
						moSolidBrushColor,
						&mpSolidBrush
						);

				}
				else
				{
					// �����Ѿ�������SolidBrush��������п��ܷ����ı䣬�������µ���һ��
					mpSolidBrush->SetColor(moSolidBrushColor);
				}

				break;
			}
		case XuiLinearGradientBrush:
			{
				ID2D1GradientStopCollection *lpGradientStops = NULL;
				if (mpLinearGradientBrush == NULL)
				{
					hr = npRenderTarget->CreateGradientStopCollection(
						mdGradientStop.GetBuffer(),
						mdGradientStop.Size(),
						D2D1_GAMMA_2_2,
						D2D1_EXTEND_MODE_CLAMP,
						&lpGradientStops
						);

					if (SUCCEEDED(hr))
					{
						hr = npRenderTarget->CreateLinearGradientBrush(
							mdLinearGradientBrushProperties,
							lpGradientStops,
							&mpLinearGradientBrush
							);
					}
				}
				else
				{
					// �����Ѿ�������SolidBrush��������п��ܷ����ı䣬�������µ���һ��
					mpLinearGradientBrush->SetStartPoint(mdLinearGradientBrushProperties.startPoint);
					mpLinearGradientBrush->SetEndPoint(mdLinearGradientBrushProperties.endPoint);
				}

				CMM_SAFE_RELEASE(lpGradientStops);

				break;
			}
		case XuiRadialGradientBrush:
			{
				ID2D1GradientStopCollection *lpGradientStops = NULL;
				if (mpRadialGradientBrush == NULL)
				{
					hr = npRenderTarget->CreateGradientStopCollection(
						mdGradientStop.GetBuffer(),
						mdGradientStop.Size(),
						D2D1_GAMMA_2_2,
						D2D1_EXTEND_MODE_CLAMP,
						&lpGradientStops
						);

					float lfMaxRadius;
					if (mdLinearGradientBrushProperties.startPoint.x < mdLinearGradientBrushProperties.startPoint.y)
						lfMaxRadius = mdLinearGradientBrushProperties.startPoint.y;
					else
						lfMaxRadius = mdLinearGradientBrushProperties.startPoint.x;

					if (SUCCEEDED(hr))
					{
						hr = npRenderTarget->CreateRadialGradientBrush(
							D2D1::RadialGradientBrushProperties(
							mdLinearGradientBrushProperties.startPoint,
							mdLinearGradientBrushProperties.endPoint,
							lfMaxRadius,
							lfMaxRadius),
							lpGradientStops,
							&mpRadialGradientBrush
							);
					}
				}
				else
				{
					// �����Ѿ�������SolidBrush��������п��ܷ����ı䣬�������µ���һ��
					mpRadialGradientBrush->SetCenter(mdLinearGradientBrushProperties.startPoint);
				}

				CMM_SAFE_RELEASE(lpGradientStops);

				break;
			}
		case XuiBitmapBrush:
			{
				break;
			}
		default:
			{
				luStatus = ERESULT_WRONG_PARAMETERS;
				break;
			}
		}


	} while (false);

	if (mcStrokeCreate != false)
	{
		// ���Ͷ������һ���������Ͳ��ܸı�������
		if (mpStrokeStyle == NULL)
		{
			hr = npD2D1Factory->CreateStrokeStyle(
				moStrokeStyleProperties,
				mpDashes,
				muDashCount,
				&mpStrokeStyle);
		}

	}



	if (SUCCEEDED(hr))
		luStatus = ERESULT_SUCCESS;
	else
		luStatus = ERESULT_UNSUCCESSFUL;

	return luStatus;
}

// ����SOLID��ˢ����ɫ
void CXD2DBrush::SetColor(IN D2D1_COLOR_F noColor)
{
	moSolidBrushColor = D2D1::ColorF(noColor.r, noColor.g, noColor.b, noColor.a);

}

// ���ý��仭ˢ������
void CXD2DBrush::SetLinearBrushProperties(
	const D2D1_GRADIENT_STOP* npGradientStop, 
	ULONG nuCount, 
	D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES ndLinearGradientBrushProperties
	)
{
	
	for (ULONG liLoop = 0; liLoop < nuCount; liLoop++)
	{
		mdGradientStop.Insert(-1,npGradientStop[liLoop]);
	}

	mdLinearGradientBrushProperties = ndLinearGradientBrushProperties;
	muCount = nuCount;

}

// �������ͣ���D2D�Ľṹ��������
bool CXD2DBrush::SetStrokeType(const D2D1_STROKE_STYLE_PROPERTIES &strokeStyleProperties, const FLOAT *dashes, UINT dashesCount)
{
	bool lbStatus = false;

	do 
	{
		// ����Ѿ����ù����ͣ��򲻿����ٴ�����
		BREAK_ON_FALSE(!mcStrokeCreate);

		moStrokeStyleProperties = strokeStyleProperties;
		lbStatus = true;

		// �ж��Ƿ����Զ�������
		BREAK_ON_NULL(dashes);
		// ����У�����������״̬
		lbStatus = false;
		BREAK_ON_FALSE(dashesCount != 0);
		CMM_SAFE_DELETE(mpDashes);

		mpDashes = new FLOAT[dashesCount];
		for (ULONG liLoop = 0; liLoop < dashesCount; liLoop++)
		{
			mpDashes[liLoop] = dashes[liLoop];
		}

		muDashCount = dashesCount;

		lbStatus = true;
	} while (false);

	if (lbStatus != false)
		mcStrokeCreate = true;	

	return lbStatus;
}

// �������Ϳ��
void CXD2DBrush::SetStrokeWidth(IN float nfWidth)
{
	mfStrokeWidth = nfWidth;

}

// ��ȡ���Ϳ��
float CXD2DBrush::GetStrokeWidth()
{
	return mfStrokeWidth;
}

// ��ȡ��ˢ����
ERESULT CXD2DBrush::GetBrushObject(OUT ID2D1Brush** npBrushObject)
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;

	// �����ж����������͵Ļ�ˢ��Ȼ�󷵻ض�Ӧ�Ļ�ˢ���
	
	switch(miBrushType)
	{
	case XuiStroke:
	case XuiSolidBrush:
		{
			if (mpSolidBrush != NULL)
			{
				*npBrushObject = mpSolidBrush;
				luResult = ERESULT_SUCCESS;
			}
			break;
		}
	case XuiLinearGradientBrush:
		{
			if (mpLinearGradientBrush != NULL)
			{
				*npBrushObject = mpLinearGradientBrush;
				luResult = ERESULT_SUCCESS;
			}
			break;
		}
	case XuiRadialGradientBrush:
		{
			if (mpRadialGradientBrush != NULL)
			{
				*npBrushObject = mpRadialGradientBrush;
				luResult = ERESULT_SUCCESS;
			}
			break;
		}
	case XuiBitmapBrush:
		{
			if (mpBitmapBrush != NULL)
			{
				*npBrushObject = mpBitmapBrush;
				luResult = ERESULT_SUCCESS;
			}
			break;
		}
	default:
		break;
	}

	return luResult;
}

// ��ȡ���Ͷ���
ERESULT CXD2DBrush::GetStrokeObject(OUT ID2D1StrokeStyle** npStrokeObject)
{
	ERESULT luResult = ERESULT_UNSUCCESSFUL;	// ��������ķ���ֵ������ûɶ�ô�


	do 
	{
		if (mpStrokeStyle != NULL)
			*npStrokeObject = mpStrokeStyle;
		else
			*npStrokeObject = NULL;
	
		luResult = ERESULT_SUCCESS;

	} while (false);

	return luResult;
}


void CXD2DBrush::DiscardsBrushResource()
{
	CMM_SAFE_RELEASE(mpStrokeStyle);
	CMM_SAFE_RELEASE(mpSolidBrush);
	CMM_SAFE_RELEASE(mpLinearGradientBrush);
	CMM_SAFE_RELEASE(mpRadialGradientBrush);
	CMM_SAFE_RELEASE(mpBitmapBrush);

}

// ���Ƴ�һ���µĶ���
IEinkuiBrush* __stdcall CXD2DBrush::DuplicateBrush(void)
{

	IEinkuiBrush* lpNewBrush = NULL;

	do 
	{
		switch(miBrushType)
		{
		case XuiStroke:
			{
				lpNewBrush = CXD2DBrush::CreateInstance(XuiStroke, moSolidBrushColor);
				break;
			}
		case XuiSolidBrush:
			{
				lpNewBrush = CXD2DBrush::CreateInstance(XuiSolidBrush, moSolidBrushColor);
				break;
			}
		case XuiLinearGradientBrush:
			{
				lpNewBrush = CXD2DBrush::CreateInstance(XuiLinearGradientBrush, mdGradientStop.GetBuffer(),mdGradientStop.Size(), mdLinearGradientBrushProperties);
				break;
			}
		case XuiRadialGradientBrush:
			{
				lpNewBrush = CXD2DBrush::CreateInstance(XuiRadialGradientBrush, mdGradientStop.GetBuffer(),mdGradientStop.Size(), mdLinearGradientBrushProperties);
				break;
			}

		}

		BREAK_ON_NULL(lpNewBrush);

		// �Ƿ���Ҫ��������
		if (mcStrokeCreate != false)
		{
			lpNewBrush->SetStrokeType(moStrokeStyleProperties, mpDashes, muDashCount);
		}


	} while (false);

	return lpNewBrush;
}