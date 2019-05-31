/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once
#include "EdDoc.h"
#include "EdViewCtl.h"

/*
	PDFͼƬ��ʾ
*/

DECLARE_BUILTIN_NAME(PdfPicturePDF)

class CPdfPicturePDF:
	public CXuiElement<CPdfPicturePDF,GET_BUILTIN_NAME(PdfPicturePDF)>
{
	friend CXuiElement<CPdfPicturePDF,GET_BUILTIN_NAME(PdfPicturePDF)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	//���ĵ�
	bool OpenFile(wchar_t* npszPath);
	//�ر��ĵ�
	void CloseFile(bool nbUpdateView=true);
	//ҳ����ת
	bool GoToPage(ULONG nulPageNumber);
	//ҳ��ǰ������
	bool PageFoward(bool foward);
	//��õ�ǰҳ��
	ULONG GetCurrentPageNumber(ULONG& secondPage);
	//��������
	//rMove4������0��ʾ�����ƶ��ˣ�1��ʾ�����ƶ�
	bool SetScaleRatio(float nfScale,OUT RECT& rMove);
	//��ȡ��ҳ��
	ULONG GetPageCount(void);
	//�ƶ�ҳ��
	//niXOffset,niYOffset������ʾ�������ƶ���������ʾ�������ƶ�
	//rMove4������0��ʾ�����ƶ��ˣ�1��ʾ�����ƶ�
	bool MovePage(int niXOffset,int niYOffset, OUT RECT& rMove);
	//��Ļ������ת
	void SetRotation(ULONG nulRotation);
	// ���ý���˫ҳ��ʾ
	void EnableDuopageView(bool nbEnable);
	//����������������
	bool CopyToClipboard(const D2D1_RECT_F& rRegion);
	// ��ȡFat״̬�ķŴ���
	float GetFatRatio(void);
	// ��ȡ��ǰҳ����������ڵ�ǰ�Ŵ�����µ�Դͼ�е�λ�ã��Լ���ӦԴͼ�Ĵ�С
	void GetRectOfViewportOnPage(D2D1_SIZE_F& nrImageSize, D2D1_RECT_F& nrViewPort);
	

protected:
	CPdfPicturePDF(void);
	~CPdfPicturePDF(void);

	IEdModule_ptr pdfModule;
	IEdDocument_ptr pdfDoc;
	IEdPage_ptr pdfCrtPage;
	IEdPage_ptr pdfCrtPage2;
	IEdBitmap_ptr pdfImage;
	IEdBitmap_ptr pdfImage2;
	IEinkuiBitmap* mpElBitmap;
	ED_SIZE imageSize;
	bin_ptr duopageBuf;
	bool32 landScope;
	bool32 duopageMode;
	int32 pageGap;
	int32 pageNo;
	CEdViewCtl mViewCtl;
	D2D1_RECT_F mRecentDst;
	D2D1_RECT_F mRecentSrc;


	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//������Ϣ
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);

	//��ʱ��
	virtual void OnTimer(
		PSTEMS_TIMER npStatus
		);
	////��Ϣ������
	//virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	//��ť�����¼�
	virtual ERESULT OnCtlButtonClick(IEinkuiIterator* npSender);
	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize);
	//֪ͨԪ�ء���ʾ/���ء������ı�
	virtual ERESULT OnElementShow(bool nbIsShow);

	IEdPage_ptr loadPage(int32 pageNo, ED_SIZE& sizeInit);

	bool RenderPages(void);
	void CalcMovalbe(OUT RECT& rMove);

private:

	IEinkuiIterator* mpIterBtShowTip;	//��ʾ/���ع�����
	IEinkuiBrush*	mpXuiBrush;				// ���߻�ˢ
	ULONG mulPageCount; //�ļ���ҳ��
};

#define PP_BT_SHOW_TIP 2 