/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once
#include "EdDoc.h"
#include "EdViewCtl.h"
#include "Loading.h"

/*
	PDFͼƬ��ʾ
*/

DECLARE_BUILTIN_NAME(PdfPicture)

#define PAGEID_LIB(_X) (_X-1)
#define PAGEID_USER(_X) (_X+1)

class CPdfPicture:
	public CXuiElement<CPdfPicture,GET_BUILTIN_NAME(PdfPicture)>
{
	friend CXuiElement<CPdfPicture,GET_BUILTIN_NAME(PdfPicture)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	//���ĵ�
	ULONG OpenFile(wchar_t* npszPath, PPAGE_PDF_CONTEXT npPageContext=NULL);
	//�ر��ĵ�
	void CloseFile(bool nbUpdateView=true);
	//�ǲ���Txt�ĵ��������ĵ���ʽ����ͨ�����������С�������ĵ���չʾ��
	bool IsTxtDocument(void);
	//ҳ����ת
	bool GoToPage(ULONG nulPageNumber);
	//ҳ����ת
	bool GoToPage(IEdPage_ptr pageGoto);
	//ҳ��ǰ������
	bool PageFoward(bool forward);
	//��õ�ǰҳ��
	ULONG GetCurrentPageNumber(ULONG& secondPage);
	//��õ�ǰ��һҳ�������ģ���ǰҳ��ҳ���ڽṹ���pageIndex����
	bool GetCrtPageContext(PPAGE_PDF_CONTEXT npPageContext);
	//��������
	//rMove4������0��ʾ�����ƶ��ˣ�1��ʾ�����ƶ�
	bool SetScaleRatio(float nfScale,OUT RECT& rMove);
	//��ȡ��ҳ��
	ULONG GetPageCount(void);
	//����Txt�ļ��������С
	bool SetFontSize(int fontSize);
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

	void OnPageLoaded(int32 loadingStep, int32 pagecount);

	void SetLoadingPageIndex(LONG nlPageIndex);
protected:
	CPdfPicture(void);
	~CPdfPicture(void);

	IEdModule_ptr pdfModuleArr[10];
	IEdDocument_ptr pdfDoc;
	IEdPage_ptr pdfCrtPage;
	IEdPage_ptr pdfCrtPage2;
	IEdBitmap_ptr pdfImage;
	IEdBitmap_ptr pdfImage2;
	IEinkuiBitmap* mpElBitmap;
	ED_SIZE imageSize;
	bin_ptr pageCanvas;
	int32  pageCanvasSize;
	bool32 landScope;
	bool32 duopageMode;
	int32 pageGap;
	int32 pageNo;
	CEdViewCtl mViewCtl;
	D2D1_RECT_F mRecentDst;
	D2D1_RECT_F mRecentSrc;
	bool mIsTxtDoc;
	float32 mTitleBarHeight;	// txt��ʽ�£�����������ʾ���򣬿ճ�titlebar

	uint32 mFontSize;

	volatile LONG mLoading;
	CLoadingView* mpLoadingView;

	//PAGE_PDF_CONTEXT mPageContext;


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

	bool RenderPages(void);
	void CalcMovalbe(OUT RECT& rMove);

	bool32 GetPageSize(IEdPage_ptr pagePtr, ED_SIZE& sizeInit);

	bin_ptr PrepareCanvas(int32 totalBytes);

private:

	IEinkuiIterator* mpIterBtShowTip;	//��ʾ/���ع�����
	IEinkuiBrush*	mpXuiBrush;				// ���߻�ˢ
	ULONG mulPageCount; //�ļ���ҳ��
};

#define PP_BT_SHOW_TIP 2 