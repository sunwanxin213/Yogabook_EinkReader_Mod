/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _LW_D2DBRUSH
#define _LW_D2DBRUSH



// ����D2D�Ļ�ˢ��
// ��Ҫ���ܣ�
// 1�����Դ����������͵Ļ�ˢ
// 2���������û�ˢ�Ļ�������
// 3�����Ի�ȡ��Ӧƽ̨�Ļ�ˢ����
// 4����������



DECLARE_BUILTIN_NAME(CXD2DBrush)
class CXD2DBrush: public cmmBaseObject<CXD2DBrush,IEinkuiBrush, GET_BUILTIN_NAME(CXD2DBrush)> 
{

//////////////////////////////////////////////////////////////////////////
// ��ʼ������
//////////////////////////////////////////////////////////////////////////
public:
	CXD2DBrush();
	~CXD2DBrush();

	// �����ʼ�������У�ֻʵ�������������������κ���ƽ̨��ص���Դ��ֱ���ö���ʹ��ʱ������InitBrush��ʼ����ˢ
	DEFINE_CUMSTOMIZE_CREATE(CXD2DBrush,(XuiBrushType niBrushType, D2D1_COLOR_F noColor),(niBrushType, noColor))
	ULONG InitOnCreate(XuiBrushType niBrushType, D2D1_COLOR_F noColor);

	// ���仭ˢʱ����Ҫ��������ɫ��
	DEFINE_CUMSTOMIZE_CREATE(CXD2DBrush,
		(XuiBrushType niBrushType, D2D1_GRADIENT_STOP* npGradientStop, ULONG nuCount, D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES ndLinearGradientBrushProperties),			(niBrushType, npGradientStop, nuCount, ndLinearGradientBrushProperties))
	ULONG InitOnCreate(XuiBrushType niBrushType, D2D1_GRADIENT_STOP* npGradientStop, ULONG nuCount, 
		D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES ndLinearGradientBrushProperties);


	// ����ʹ��ʱ����Ҫ������ˢ��Դ����ƽ̨�����Դ������ʱ��ͨ��ƽ̨�����߰Ѳ���������ʼ������
	ERESULT __stdcall InitBrush(ID2D1RenderTarget* npRenderTarget,  ID2D1Factory* npD2D1Factory);		


//////////////////////////////////////////////////////////////////////////
// ʵ�ֻ���ӿ�
//////////////////////////////////////////////////////////////////////////

// ����D2D��GDIPlushƽ̨�Ĺ�����Ϊ����
public:
	// ����SOLID��ˢ����ɫ
	virtual void __stdcall SetColor(IN D2D1_COLOR_F noColor);

	// ���ý��仭ˢ������
	virtual void __stdcall SetLinearBrushProperties(
		const D2D1_GRADIENT_STOP* npGradientStop, 
		ULONG nuCount, 
		D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES ndLinearGradientBrushProperties		
		);

	// �������ͣ���D2D�Ľṹ��������
	virtual bool __stdcall SetStrokeType(const D2D1_STROKE_STYLE_PROPERTIES &strokeStyleProperties, const FLOAT *dashes, UINT dashesCount);

	// �����߿�
	virtual void __stdcall SetStrokeWidth(IN float nfWidth);

	// ��ȡ�߿�
	virtual float __stdcall GetStrokeWidth();

	// ��ϵͳ�ڲ����ã�����Direct2D���豸�����Դ
	virtual void __stdcall DiscardsBrushResource(void);
	
	// ʵ������ӿں���Ҫ������������˳�ʱ��������ͷ�����
	virtual int __stdcall Release(void);

	// ���Ƴ�һ���µĶ���,���Ƴ����Ķ�����Ҫʹ�����Լ��ͷ�
	virtual IEinkuiBrush* __stdcall DuplicateBrush(void);

// ����D2D�����������Ϊ
public:
	// ��ȡ��ˢ����
	virtual ERESULT __stdcall GetBrushObject(OUT ID2D1Brush** npBrushObject);

	// ��ȡ�������͵�Stroke����
	virtual ERESULT __stdcall GetStrokeObject(OUT ID2D1StrokeStyle** npStrokeObject);


//////////////////////////////////////////////////////////////////////////
// ����������Լ���Դ
//////////////////////////////////////////////////////////////////////////
protected:
	// �̶���ɫ����仭ˢ
	ID2D1SolidColorBrush* mpSolidBrush;
	// ���Խ��仭ˢ
	ID2D1LinearGradientBrush* mpLinearGradientBrush;
	// ���򽥱仭ˢ
	ID2D1RadialGradientBrush* mpRadialGradientBrush;
	// λͼ��ˢ
	ID2D1BitmapBrush* mpBitmapBrush;

	// ��ˢ����
	XuiBrushType miBrushType;
	// SOLID��ˢ��ɫ
	D2D1_COLOR_F	moSolidBrushColor;		

	// ���仭ˢԤ����4���ɵ��ڵĵ�
	cmmVector<D2D1_GRADIENT_STOP,4> mdGradientStop;
	D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES	mdLinearGradientBrushProperties;
	ULONG	muCount;


	// ��־�Ƿ�Ҫ�������Ͷ���
	bool	mcStrokeCreate;
	// ���Ͷ���
	ID2D1StrokeStyle* mpStrokeStyle;
	// ���Ϳ��
	float	mfStrokeWidth;
	// ��������
	D2D1_STROKE_STYLE_PROPERTIES moStrokeStyleProperties;
	// �����Զ�������
	FLOAT* mpDashes;
	UINT   muDashCount;

};


#endif