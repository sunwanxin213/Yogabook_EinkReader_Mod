/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _ELBITMAPIMP_H
#define _ELBITMAPIMP_H

// ͸������ֵ��������ֵ���ж�Ϊ͸��
#define  TRANSPARENT_THRESHOLD_VALUE	12


DECLARE_BUILTIN_NAME(CElBitmap)
class CElBitmap : public cmmBaseObject<CElBitmap, ILwBitmap, GET_BUILTIN_NAME(CElBitmap)>
{
public:

	CElBitmap();
	~CElBitmap();

	// �ͷŶ���
	virtual int __stdcall Release(void);

	// ��ȡIWICBitmap�ӿ�ָ�룬ʹ������Ҫ�����ͷŴ˽ӿ�ָ��
	virtual ERESULT __stdcall GetWICObject(OUT IWICBitmap** ppWicBitmap);

	// ��ȡID2D1Bitmap�ӿ�ָ�룬ʹ������Ҫ�����ͷŴ˽ӿ�ָ��
	virtual ERESULT __stdcall GetD2DObject(IN ID2D1RenderTarget *npRT,
		OUT ID2D1Bitmap **nppParentBitmap);

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

	//// ��λͼ�������ţ�����ȡ���Ž����ID2D1Bitmap�ӿ�ָ��
	//virtual ERESULT GetScaledD2DObject(IN UINT npWidth, IN UINT npHeight,
	//	IN ID2D1RenderTarget *npRT,
	//	OUT ID2D1Bitmap **nppParentBitmap);

	// ��Ҫ�Զ������Դ����EUIϵͳ�����������λͼ��ʹ�����������Paintboard��DrawBitmap�����ж�һ��λͼ�Ƿ�����ͷ��Դ�������һ��λͼ���Դ���󲢲���ʹ����(Paintboard::DrawBitmap��������,
	// �������Լ����ض��÷�����ô�͵��ñ��������ر�λͼ���Զ�����Auto video object management;
	// ����ֵ�ǵ��ñ�����ǰ��״̬
	virtual bool __stdcall SetAVOM(
		bool nbEnable	// false to disable AVOM,true to enable AVOM
		){return false;}


	// ��ϵͳ�ڲ����ã�����Direct2D���豸�����Դ
	virtual void __stdcall Discards2DResource(void); 

	ULONG InitOnCreate(
		IN const wchar_t *npFileName);

	ULONG InitOnCreate(
		IN const wchar_t *npText,
		IN DWORD dwTextColor,
		IN const wchar_t *npFont,
		IN DWORD dwFontSize);

	ULONG InitOnCreate(wchar_t *npPeFileName, int niIconIdx, int niXSize, int niYSize);

	ULONG InitOnCreate(ID2D1RenderTarget* npRt, FLOAT nfWidth, FLOAT nfHeight);

	// ͨ��D3D Surfaceʵ����λͼ����
	DEFINE_CUMSTOMIZE_CREATE(CElBitmap, (ID2D1RenderTarget* npRt, FLOAT nfWidth, FLOAT nfHeight), (npRt, nfWidth, nfHeight))

	// ͨ��EXE�ļ���ʵ����λͼ����
	DEFINE_CUMSTOMIZE_CREATE(CElBitmap, (wchar_t *npPeFileName, int nwIconIdx, int niXSize, int niYSize), (npPeFileName, nwIconIdx, niXSize, niYSize))

	// ͨ��λͼ�ļ���ʵ����λͼ����
	DEFINE_CUMSTOMIZE_CREATE(CElBitmap,(const wchar_t *npFileName),(npFileName))

	// ͨ������ʵ����λͼ����
	// �ṩ�������£�
	//	npText -- ���ַ����ִ���Ŀǰ��֧�ֻ��У�
	//	dwColor -- RGB��ʽ����ɫֵ
	//	npFont -- ָ������������"Tahoma"
	//	dwFontSize -- ָ�������С
	DEFINE_CUMSTOMIZE_CREATE(CElBitmap, (const wchar_t *npText, DWORD dwColor, const wchar_t *npFont, DWORD dwFontSize), (npText, dwColor, npFont, dwFontSize));
protected:
//private:
	HANDLE m_hBitmap;
	ID2D1Bitmap* mpBitmap2D;
	IDXGISurface* mpDxgiSurface;
	IWICBitmapDecoder *m_pIDecoder;
	IWICBitmapFrameDecode *m_pIDecoderFrame;
	IWICBitmapScaler *m_pBmpScaler;
// 	static IWICImagingFactory *m_pWICFactory;
// 	static ID2D1Factory *m_pD2DFactory;

	HRESULT m_hResult;
	UINT m_nWidth;
	UINT m_nHeight;
	UINT m_nScaleWidth;
	UINT m_nScaleHeight;
	LONG m_lExtendLineX;
	LONG m_lExtendLineY;

	IWICBitmap *m_pWICBitmap;

	CBitArray *mpTransparentInfo;

	ULONG Load(IN const wchar_t *npFileName);

	ERESULT CreateD2DObjFromBmpSource(IN ID2D1RenderTarget *npRT, IN IWICBitmapSource *npSourceBmp,
		OUT ID2D1Bitmap **nppParentBitmap);

	ULONG GetBitmapDimension();

	ERESULT GenTransparentInfo();
};

#endif // _ELBITMAPIMP_H