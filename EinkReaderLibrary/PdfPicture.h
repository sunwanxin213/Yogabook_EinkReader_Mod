/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once
#include "EdDoc.h"
#include "EdViewCtl.h"
#include "Loading.h"
#include "cmmstruct.h"
#include "RedoUndoManager.h"
#include "MsgDefine.h"
#include "tchar.h"
#include "cmmString.h"
#include "cmmBaseObj.h"
#include "cmmPath.h"

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
	ULONG OpenFile(wchar_t* npszPath, PPAGE_PDF_CONTEXT npPageContext=NULL, wchar_t* npszSource=NULL);
	//�ر��ĵ�
	void CloseFile(bool nbUpdateView=true);
	//�ǲ���Txt�ĵ��������ĵ���ʽ����ͨ�����������С�������ĵ���չʾ��
	bool IsTxtDocument(void);
	//ҳ����ת
	bool GoToPage(ULONG nulPageNumber, bool renderPage);
	//ҳ����ת
	bool GoToPage(IEdPage_ptr pageGoto, bool renderPage);
	//ҳ��ǰ������
	bool PageFoward(bool forward, bool renderPage);
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
	//�ƶ���ĳλ��
	bool MovePageTo(int niX, int niY);
	//��Ļ������ת
	void SetRotation(ULONG nulRotation);
	// ���ý���˫ҳ��ʾ
	void EnableDuopageView(bool nbEnable);
	//����������������
	bool CopyToClipboard(const D2D1_RECT_F& rRegion);
	//�����ַ�����������
	bool CopyToClipboard(const wchar_t* npszText);
	// ��ȡFat״̬�ķŴ���
	float GetFatRatio(void);
	// ��ȡ��ǰҳ����������ڵ�ǰ�Ŵ�����µ�Դͼ�е�λ�ã��Լ���ӦԴͼ�Ĵ�С
	void GetRectOfViewportOnPage(D2D1_SIZE_F& nrImageSize, D2D1_RECT_F& nrViewPort);

	//ҳ�����
	void OnPageLoaded(int32 loadingStep, int32 pagecount);
	//����ͼ����
	void OnThumbnailLoaded(int32 loadingStep);

	void SetLoadingPageIndex(LONG nlPageIndex);

	//��ȡ�ĵ�����
	int GetDocType(void);
	//������������
	ED_RECTF CalImageContentRect();

	//�����¼�
	void TouchDown(PEI_TOUCHINPUT_POINT npPoint, ULONG nulPenMode,bool nbIsHand);
	void TouchMove(PEI_TOUCHINPUT_POINT npPoint, ULONG nulPenMode, bool nbIsHand);
	void TouchUp(PEI_TOUCHINPUT_POINT npPoint, ULONG nulPenMode, bool nbIsHand);
	void PenLeave(ULONG nulPenMode);
	//��¼��
	void SaveTouchPoint(PEI_TOUCHINPUT_POINT npPoint, ULONG nulPenMode, bool nbIsHand);

	//�����߿�
	void SetPenWidth(float nfPenWidth);
	//������ɫ
	void SetPenColor( ULONG nulPenColor);
	//undo �� redo����
	int RedoUndoProc(bool nbIsRedo);
	//��ȡ��ǰҳ���ע����
	int GetCurrentPageInkCount(void);
	//��յ�ǰҳ�����б�ע
	bool ClearCurrentPageAllInk(void);
	//���������ƶ�����
	void CalcMovalbe(OUT RECT& rMove);
	//��ȡҳ��Ŵ����
	float32 GetRealRatio();
	float32 GetBaseRatio();
	//ѡ��ָ�����������,�������ֿ���
	int SelectHighlight(const D2D1_RECT_F& rRegion, SELECT_HIGHLIGHT& rdHighltght);
	// ��ȡѡ��������Ϣ
	void GetSelectHighlightInfo(SELECT_HIGHLIGHT& rdHighltght, ED_RECTF& rdRect);
	//������������Ϣ
	void HighlightEvent(int niEvent);
	//������־����
	void SetLogStatus(bool nbIsEnable);
	//���ñ�״̬
	void SetPenmode(ULONG nulPenMode);
	//��ȡ�ĵ�����
	IEdDocument_ptr GetDoc(void);
	//�����־���ļ���debugview
	void SvcDebugOutFmt(char * nswString, ...);
	//��������ͼ
	void LoadThumbnails(const wchar_t* npszFilePath,bool nbIsOpen = true);
	//����PDF����ͼ
	void RefreshThumbnail();
	void RefreshThumbnail(IEdPage_ptr npPage1, IEdPage_ptr npPage2);
	bool32 GetThumbanilsPath(wchar_t* npszPathBuffer, int niLen);
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
	int miDocType;
	float32 mTitleBarHeight;	// txt��ʽ�£�����������ʾ���򣬿ճ�titlebar
	IEinkuiBrush* mpLineBrush;	// �豸���
	IEinkuiBrush* mpHigilightBrush;	// �豸���

	uint32 mFontSize;
	bool mbIsPageChange;

	volatile LONG mLoading;
	CLoadingView* mpLoadingView;

	cmmVector<ED_POINTF> mStrokePoints; //���¼
	cmmVector<ED_POINTF> mStrokePoints2; //���¼,˫ҳģʽ�µ�
	//PAGE_PDF_CONTEXT mPageContext;


	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
	// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
	virtual ERESULT OnElementDestroy();

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
	

	bool32 GetPageSize(IEdPage_ptr pagePtr, ED_SIZE& sizeInit);

	bin_ptr PrepareCanvas(int32 totalBytes);

	//����redo undo��ť״̬
	void SetRedoUndoStatus(void);
	//����ҳ���ȡҳ�����ָ��
	IEdPage_ptr GetPageByPageNumber(int niPageNumber);
	//��Ƥ����
	void EarsePro(void);
	//�ʼ�����Ƥˢ��
	void InkInputUpdate(void);
	//ɾ������
	void DeleteHighlight(IEdAnnotList_ptr npSelectAnnotList,CRedoUndoStrokeList* npRedoUndoStrokeList);
	//��ȡ����ͼ·��
	bool GetThumbnailPath(const wchar_t* npDocPathName,bool nbIsOpen);
	//����
	bool HashBuffer(cmmStringW& input, cmmStringW& output);

	void SvcDebugOut(char* nswString);
	CExclusiveAccess moLogFileLock;
	HANDLE mhandleLogFile;
	bool mbIsLog;
private:

	//IEinkuiIterator* mpIterBtShowTip;	//��ʾ/���ع�����
	IEinkuiBrush*	mpXuiBrush;				// ���߻�ˢ
	ULONG mulPageCount; //�ļ���ҳ��
	wchar_t mpszSourcePath[MAX_PATH]; //�ļ�����ʵ·��
	float mfPenWidth;
	ED_COLOR mdPenColor;
	bool mbIsModify; //�Ƿ��޸Ĺ��ļ�
	ED_RECT mdFwLineRect; //FW��������
	float mfMiddleLineX; //˫ҳģʽ������λ��
	CFilePathName mCachePath; //����ͼ·��

	CRedoUndoManager mcRedoUndoManager; //����redo undo����
	CRedoUndoStrokeList* mpEarseRedoUndoStrokeList; //���ڼ�¼��Ƥ��¼����Ϊ��Ƥ��ʱʱˢ�£���Ҫһ��undo,�������⴦��
	

	IEdPage_ptr mpSelectPage; //ѡ���������ĸ�ҳ��
	IEdAnnotList_ptr mpSelectAnnotList; //ѡ������ĸ�������list
	IEdStextQuadList_ptr mpSelectStextList; //ѡ������ĸ��������
};

#define PP_BT_SHOW_TIP 2 

#define PP_TIMER_ID_UPDATE 1 //ˢ��ҳ����ʾ
#define PP_TIMER_ID_ENABLE_FRAME 2 //��������ˢ��
#define PP_TIMER_ID_SAVE_FILE 3 //д���ļ�