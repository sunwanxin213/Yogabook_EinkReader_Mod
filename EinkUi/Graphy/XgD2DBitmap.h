/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _LW_D2DBITMAPIMP
#define _LW_D2DBITMAPIMP

// ͸������ֵ��������ֵ���ж�Ϊ͸��
#define  TRANSPARENT_THRESHOLD_VALUE	12




// ����bitmap��
// ����䶯���������㣺
// 1�������һ�γ�ʼ����ʱ��ֻ��ʼ�����ݲ��֡����ݳ�����������ͣ�λͼ�Ͷ������ݣ������Ͷ������ݣ������Ͷ������ݣ�ICON�Ͷ������ݡ�
// 2����λͼ����ʹ��ʱ��������ƽ̨��Դ��صĳ�ʼ����

enum BitmapType{
	CommonBitmap,
	IconBitmap,
	TextBitmap,
	MemoryBitmap,
	None,
};



DECLARE_BUILTIN_NAME(CXD2dBitmap)
class CXD2dBitmap : public cmmBaseObject<CXD2dBitmap, IEinkuiBitmap, GET_BUILTIN_NAME(CXD2dBitmap)>
{

//////////////////////////////////////////////////////////////////////////
// ��ʼ������
//////////////////////////////////////////////////////////////////////////
public:
	CXD2dBitmap();
	~CXD2dBitmap();

	// ͨ��λͼ�ļ�ʵ����λͼ����
	DEFINE_CUMSTOMIZE_CREATE(CXD2dBitmap,(const wchar_t *npFileName),(npFileName))
	ULONG InitOnCreate(IN const wchar_t *npFileName);

	//// ͨ������ʵ����λͼ����		�������û���ù��������� ax nov.28,2017
	//// �ṩ�������£�
	////	npText -- ���ַ����ִ���Ŀǰ��֧�ֻ��У�
	////	dwColor -- RGB��ʽ����ɫֵ
	////	npFont -- ָ������������"Tahoma"
	////	dwFontSize -- ָ�������С
	//DEFINE_CUMSTOMIZE_CREATE(CXD2dBitmap, (const wchar_t *npText, DWORD dwColor, const wchar_t *npFont, DWORD dwFontSize), (npText, dwColor, npFont, dwFontSize));
	//ULONG InitOnCreate(IN const wchar_t *npText, IN DWORD dwTextColor, IN const wchar_t *npFont, IN DWORD dwFontSize);

	// ͨ��Icon����λͼ����
	DEFINE_CUMSTOMIZE_CREATE(CXD2dBitmap, (wchar_t *npPeFileName, int nwIconIdx, int niXSize, int niYSize), (npPeFileName, nwIconIdx, niXSize, niYSize))
	ULONG InitOnCreate(wchar_t *npPeFileName, int niIconIdx, int niXSize, int niYSize);

	// ָ��λͼ��С���Լ�λͼ���ݣ�����һ��λͼ����
	DEFINE_CUMSTOMIZE_CREATE(CXD2dBitmap, (LONG nuWidth, LONG nuHeight,LONG PixelSize, LONG Stride, void* npRawData), (nuWidth, nuHeight,PixelSize,Stride,npRawData))
	ULONG InitOnCreate(LONG nuWidth, LONG nuHeight,LONG PixelSize,LONG Stride,void* npRawData);

	// �ͷŶ���
	virtual int __stdcall Release(void);

	
	// ���ú����ģ�͡�����ʹ��ʱ����Ҫ����λͼ��Դ����ƽ̨�����Դ������ʱ��ͨ��ƽ̨�����߰Ѳ���������ʼ������

	// ��ʼ����ͨλͼ����
	ERESULT __stdcall InitCommonBitmap(IWICImagingFactory* npWICFactory);		
	
	//// ��ʼ��������λͼ����
	//ERESULT __stdcall InitTextBitmap(IWICImagingFactory* npWICFactory, ID2D1Factory* npD2D1Factory);	

	// ��ʼ��ICON��λͼ����
	ERESULT __stdcall InitIconBitmap(IWICImagingFactory* npWICFactory);
	
	// ��ʼ���ڴ������Ͷ���
	ERESULT __stdcall InitMemoryBitmap(ID2D1RenderTarget* npRenderTarget);	

	// ��ʼ��λͼ������λͼ���ͣ���ӵĵ��ò�ͬ���͵ĳ�ʼ������
	ERESULT __stdcall InitBitmap(IWICImagingFactory* npWICFactory, ID2D1Factory* npD2D1Factory, ID2D1RenderTarget* npRenderTarget);

//////////////////////////////////////////////////////////////////////////
// ʵ�ֻ���ӿ�
//////////////////////////////////////////////////////////////////////////
public:

	// ��ȡָ�������͸����
	virtual ERESULT __stdcall GetPixel(IN DWORD x, IN DWORD y, DWORD &nPixel);

	virtual ERESULT __stdcall GetBmpInfo(OUT BITMAP* npBmpInfo);

	// ��ȡλͼ�����أ�
	virtual UINT __stdcall GetWidth();
	// ��ȡλͼ�ߣ����أ�
	virtual UINT __stdcall GetHeight();
	//������չ��
	virtual void __stdcall SetExtendLine(IN LONG nlX,IN LONG nlY);
	//��ȡ������չ��
	virtual LONG __stdcall GetExtnedLineX(void);
	//��ȡ������չ��
	virtual LONG __stdcall GetExtnedLineY(void);

	// ��λͼ�������ţ�ʹ������Ҫ���µ���GetWICObject/GetD2DObject�Ի�ȡ
	// ���ź��λͼ�����ӿ�
	virtual ERESULT __stdcall Scale(UINT npWidth, UINT npHeight);

	// ��ȡIWICBitmap�ӿ�ָ�룬ʹ������Ҫ�����ͷŴ˽ӿ�ָ��
	virtual ERESULT __stdcall GetWICObject(OUT IWICBitmap** ppWicBitmap);

	// ��ϵͳ�ڲ����ã�����Direct2D���豸�����Դ
	virtual void __stdcall DiscardsBitmapResource(void);

	// ���Ƴ�һ���µĶ���,���Ƴ����Ķ�����Ҫʹ�����Լ��ͷ�
	virtual IEinkuiBitmap* __stdcall DuplicateBitmap(void);

public:
	// ��ȡָ�������λͼ����
	virtual bool __stdcall GetBitmapBuffer(IN D2D1_RECT_F ndSrcRect, OUT void* npRawData);

	// ���������ͼ
	virtual bool __stdcall SaveAsThumbnail(IN LONG nuWidth, IN LONG nuHeight, IN D2D1_RECT_F ndSampleRect, const wchar_t* nswOutputFilePath);


public:

	// ��ȡID2D1Bitmap�ӿ�ָ�룬ʹ������Ҫ�����ͷŴ˽ӿ�ָ��
	ERESULT __stdcall GetD2DObject(IN ID2D1RenderTarget *npRT, OUT ID2D1Bitmap **nppParentBitmap);

	ERESULT CreateD2DObjFromBmpSource(IN ID2D1RenderTarget *npRT, IN IWICBitmapSource *npSourceBmp, OUT ID2D1Bitmap **nppParentBitmap);

	// ��ȡλͼ�Ŀ����Ϣ
	ULONG GetBitmapDimension();

	ERESULT GenTransparentInfo();

protected:
	//private:
	HANDLE m_hBitmap;
	ID2D1Bitmap* mpBitmap2D;
	IWICBitmapDecoder *m_pIDecoder;
	IWICBitmapFrameDecode *m_pIDecoderFrame;
	IWICBitmapScaler *m_pBmpScaler;

	HRESULT m_hResult;
	UINT m_nWidth;
	UINT m_nHeight;
	UINT m_nScaleWidth;
	UINT m_nScaleHeight;
	LONG m_lExtendLineX;
	LONG m_lExtendLineY;

	IWICBitmap *m_pWICBitmap;

	CBitArray *mpTransparentInfo;

	// ��Ⱦ����,��ʼ��λͼ��ʱ���ݴ���������Ҫ��Ϊ����������ͼ��ʱ�����øò�����ȡԭʼͼ��D2Dλͼ����
	// ����������ã�Ӧ�ÿ�����������
	ID2D1RenderTarget* mpTempRenderTarget;


// ����λͼ��������ݶ��壬������λͼ�Ĳ�ͬ��ʽ����Ϊ���֣�λͼ�Ͷ������ݣ������Ͷ������ݣ������Ͷ������ݣ�ICON�Ͷ�������
protected:
	// ��ʶλͼ��������
	BitmapType meBitmapType;

	// ָʾ�Ƿ��Ѿ���ʼ��
	bool	mcIsInit;		

	// λͼ�Ͷ�������
	wchar_t mswFileName[MAX_PATH];

	//// �����Ͷ�������
	//wchar_t* mpText;		// ��Ϊ�������ݳ��Ȳ��̶�������Ҫ��̬�����ڴ�
	//wchar_t* mpFont;
	//DWORD	 mdwFontSize;
	//DWORD	 mdwTextColor;


};

#endif // _ELBITMAPIMP_H