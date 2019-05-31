/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _XS_D2DENGINE
#define _XS_D2DENGINE
#include "eiupdate.h"



// ��Ⱦ״̬
class CErdState{
public:
	CErdState(){
		mbRotated = false;
		mpD2dTarget = NULL;
		mbHasClipRect = false;
		mpEnhancer = NULL;
		mbPaintItself = true;
		//mbTopDraw = false;
		//mlCrtPtLevel = -1;
		//mlPlLevelHost = -1;
	}
	~CErdState(){}

	D2D1::Matrix3x2F mdTransLation;
	D2D1::Matrix3x2F mdRotation;
	D2D1::Matrix3x2F mdWorld;
	D2D1_POINT_2F mdOffset;
	FLOAT mfAlpha;
	IEinkuiIterator* mpEnhancer;
	bool mbRotated;
	bool mbHasClipRect;
	bool mbPaintItself;
	//bool mbTopDraw;
	LONG mlCrtPtLevel;
	LONG mlPlLevelHost;
	ID2D1RenderTarget* mpD2dTarget;
	D2D1_RECT_F mdClipRect;

	void operator=(const class CErdState& src){
		mdTransLation = src.mdTransLation;
		mdRotation = src.mdRotation;
		mdWorld = src.mdWorld;
		mfAlpha = src.mfAlpha;
		mpEnhancer = src.mpEnhancer;
		mbRotated = src.mbRotated;
		//mbTopDraw = src.mbTopDraw;
		mlCrtPtLevel = src.mlCrtPtLevel;
		mlPlLevelHost = src.mlPlLevelHost;
		mbPaintItself = src.mbPaintItself;
		mpD2dTarget = src.mpD2dTarget;
		mbHasClipRect = src.mbHasClipRect;
		if(mbHasClipRect != false)
			mdClipRect = src.mdClipRect;
		mdOffset = src.mdOffset;
	}
};

typedef cmmStack<CErdState,23,32> TRrdStateStack;

#pragma pack(1)
typedef struct _EI_LINE_DIFF {
	unsigned short Begin;
	unsigned short End;
}EI_LINE_DIFF, *PEI_LINE_DIFF;
#pragma pack()



//////////////////////////////////////////////////////////////////////////
// D2D ͼ������
//////////////////////////////////////////////////////////////////////////


DECLARE_BUILTIN_NAME(CXD2dEngine)
class CXD2dEngine: public cmmBaseObject<CXD2dEngine,IEinkuiPaintBoard, GET_BUILTIN_NAME(CXD2dEngine)>, public IEinkuiPaintResouce
{

//////////////////////////////////////////////////////////////////////////
// �����ʼ������
//////////////////////////////////////////////////////////////////////////
public:

	// Ĭ�Ϲ��캯��
	CXD2dEngine();
	// Ĭ����������
	~CXD2dEngine();

	// �Զ����ʼ������
	DEFINE_CUMSTOMIZE_CREATE(CXD2dEngine,(ULONG Width,ULONG Height),(Width, Height))
	ULONG InitOnCreate(ULONG Width, ULONG Height);

	// �����豸�޹���Դ
	virtual ERESULT CreateIndependentResource();

	// �����豸�����Դ
	virtual ERESULT CreateDeviceResource();

	// �ͷ�ȫ����Դ
	virtual void ReleaseDeviceResource(bool nbBroadcastToElement=true);

	// ������
	virtual void Clear();

	// �ύ����Ļ
	virtual HRESULT Present(
		IN bool nbRefresh = false	// �����ύȫ��
		);

	//// ����D3d��Դ
	//ERESULT CreateD3dResource();

	// ����D2d��Դ
	ERESULT CreateD2dResource();

	//��ȡ��ǰ����ͼ�񣬻�ȡ��HBITMAP�����ɵ��������ͷ�
	virtual HBITMAP GetCurrentBitmap(LONG nlWidth,LONG nlHeight);
//////////////////////////////////////////////////////////////////////////
// ������ͼ��Դ��ʵ��IXuiPaintResouce�Ľӿ�
//////////////////////////////////////////////////////////////////////////
public:
	// �������仭ˢ
	virtual IEinkuiBrush* __stdcall CreateBrush(
		XuiBrushType niBrushType,
		D2D1_COLOR_F noColor
		);

	// ���仭ˢʱ����Ҫ��������ɫ��
	virtual IEinkuiBrush* __stdcall CreateBrush(
		XuiBrushType niBrushType, 
		D2D1_GRADIENT_STOP* npGradientStop, ULONG nuCount, 
		D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES ndLinearGradientBrushProperties
		);	


//////////////////////////////////////////////////////////////////////////
// ��Ҫʵ�ֵ�ͼ���������ӿ�
//////////////////////////////////////////////////////////////////////////

// ��װλͼ�Ļ��Ʋ���
public:

	// ����һ��ͼ�񵽻�ͼ��
	// ��Ҫָ��Ŀ���Դ���򣻽���EMSG_PAINT/EMSG_RENDER_ENHANCER��Ϣ�����ڼ���Ч
	virtual ERESULT __stdcall DrawBitmap(
		IN const D2D1_RECT_F& rDestRect,	// Ŀ������
		IN const D2D1_RECT_F& rSrcRect,	// λͼ�Ĳ�������
		IN IEinkuiBitmap* npBitmap,		// �����Ƶ�λͼ
		IN ULONG nuMethod,			// ���������������Ķ���
		IN float nfAlpha = 1.0f		
		);

	// ����һ��ͼ�񵽻�ͼ��
	// ��ָ��Ŀ�����򣬲�������������λͼ������EMSG_PAINT/EMSG_RENDER_ENHANCER��Ϣ�����ڼ���Ч
	virtual ERESULT __stdcall DrawBitmap(
		IN const D2D1_RECT_F& rDestRect,	// Ŀ������
		IN IEinkuiBitmap* npBitmap,		// �����Ƶ�λͼ
		IN ULONG nuMethod,			// ���������������Ķ���
		IN float nfAlpha = 1.0f			
		);

	// ����һ��ͼ�񵽻�ͼ��
	// ��ȫ����ͼ��Ĵ�С���Ƶ�Ŀ�ꣻ����EMSG_PAINT/EMSG_RENDER_ENHANCER��Ϣ�����ڼ���Ч
	virtual ERESULT __stdcall DrawBitmap(
		IN const D2D1_POINT_2F& rDestLeftTop,	// Ŀ����������Ͻ�
		IN IEinkuiBitmap* npBitmap,			// �����Ƶ�λͼ
		IN float nfAlpha = 1.0f			
		);


// ��װ���㻭�ߵĲ����ӿ�
public:

	// �����ߺ���
	virtual ERESULT __stdcall DrawLine(
		IN const D2D1_POINT_2F&	noStartPoint,	// ��ʼ��
		IN const D2D1_POINT_2F& noEndPoint,		// ������
		IN IEinkuiBrush* npBrush
		);

	// �������
	virtual ERESULT __stdcall DrawPlogon(
		IN const D2D1_POINT_2F*	noPointArray,	// �����б�
		IN INT	niCount,
		IN IEinkuiBrush* npBrush 
		);

	// �������
	virtual ERESULT __stdcall FillPlogon(
		IN const D2D1_POINT_2F*	noPointArray,	// �����б�
		IN INT	niCount,
		IN IEinkuiBrush* npBrush 
		);

	// ����Բ
	virtual ERESULT __stdcall DrawEllipse(
		IN const D2D1_RECT_F& noRect,			
		IN IEinkuiBrush* npBrush
		);

	// �����Բ
	virtual ERESULT __stdcall FillEllipse(
		IN const D2D1_RECT_F& noRect,			
		IN IEinkuiBrush* npBrush
		);


	// ������
	virtual ERESULT __stdcall DrawRect(
		IN const D2D1_RECT_F& noRect,
		IN IEinkuiBrush* npBrush
		);

	// ������
	virtual ERESULT __stdcall FillRect(
		IN const D2D1_RECT_F& noRect,
		IN IEinkuiBrush* npBrush
		);


	// ��Բ�Ǿ���
	virtual ERESULT __stdcall DrawRoundRect(
		IN const D2D1_RECT_F& noRect,
		FLOAT radiusX,
		FLOAT radiusY,
		IN IEinkuiBrush* npBrush
		);


	// ���Բ�Ǿ���
	virtual ERESULT __stdcall FillRoundRect(
		IN const D2D1_RECT_F& noRect,
		FLOAT radiusX,
		FLOAT radiusY,
		IN IEinkuiBrush* npBrush
		);	

	// ??���½ӿ���Ҫȷ��

	// ���Direct2D��RenderTarget�������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
	// ��Ӧ����EMSG_PAINT/EMSG_RENDER_ENHANCER��Ϣ�����ڼ�ִ�л��ƶ�����������Prepare��Ϣ�ڼ���ƣ������ƻ���Ⱦ������ȶ�
	virtual ID2D1RenderTarget* __stdcall GetD2dRenderTarget(void);

	// ���Direct2D�Ĺ����ӿڣ������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
	virtual ID2D1Factory* __stdcall GetD2dFactory(void);

	// ���WIC�����������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
	virtual IWICImagingFactory* __stdcall GetWICFactory(void);

	// ���Direct Write�����������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
	virtual IDWriteFactory* __stdcall GetDWriteFactory(void);


	// ��õ�ǰ��D2d�����ã��ֲ����굽���������ת������
	virtual const D2D1::Matrix3x2F& __stdcall GetCurrent2DWorldMatrix(void);

	// ��õ�ǰ���������ã��������������������������D2dTarget���л��������������л����Target��Ӧ����������
	// ����ERESULT_SUCCESS��ǰ���ڿ��������ã����ҳɹ�ȡ�ã�ERESULT_UNSUCCESS�����ڿ��������ã�����ֵ��ʾ����
	virtual ERESULT __stdcall GetVisibleRegion(
		OUT D2D1_RECT_F* npRegion
		);

	// ����ֱ�ӷ���D3d��D2d�����Ԫ�أ�ʹ�ñ��ӿ���ϵͳ�㱨�豸����ϵͳ����ERESULT_SUCCESS��ʾ���Լ���ִ�У�����ֵ�����ERESULT_FAILED����ֹ����ִ��
	virtual ERESULT __stdcall CheckDeviceResult(
		IN HRESULT nhrDeviceResult
		);


	// ��õ�ǰ��TickCount
	virtual ULONG __stdcall GetCurrentTickCount();

	// ��õ�ǰ֡��
	virtual ULONG __stdcall GetCurrentFps(void);

	// ��õ�ǰ����Ⱦ���кţ��������к�������Э���ͼ��㵱ǰ����Ⱦ������ÿִ��һ����Ⱦ����ż�һ��ĳЩ����£�Ԫ�ؿ��ԻᱻҪ�����2�Σ������ﵽ���ֵ����0��ʼ
	virtual ULONG __stdcall GetRenderNumber(void);


//////////////////////////////////////////////////////////////////////////
// �������ܺ���
//////////////////////////////////////////////////////////////////////////
public:


	// ִ�л�������
	ERESULT DoRender(
		IN ULONG nuCrtTick,	// ��ǰ��TickCount
		IN bool nbRefresh=false,	// �����ύȫ��
		IN IEinkuiIterator* npToCapture=NULL	// ��ֵ����������ץȡ��Ԫ������Ԫ�ص�ͼ�񣬲�����Ӱ����Ļ��ʾ
		);

	// ִ����������
	IEinkuiBitmap* TakeSnapshot(
		IEinkuiIterator* npToShot,
		const D2D1_RECT_F& crSourceRegion,	// ��������Ŀ��Ԫ�صľֲ�����ϵ
		const D2D_SIZE_F& crBriefSize,		// ����ͼ�ߴ磬���յĽ����һ������ͼ
		const FLOAT* ColorRGBA = NULL
	);

	// ��Windows���ڹ��̵��ã����洰����С����Ϣ
	void StopPainting(bool nbMin);

	// ���軭���С
	void ResetPaintboard(void);

	// ��������ڵĴ�С
	void GetPaintboardSize(
		OUT EI_SIZE* npSize	// ��ȡ�����С
		);

	// ����Prepare PaintǰԤ����
	ERESULT __stdcall EnterForPrepare(IEinkuiIterator* npRecipient);

	// ����Prepare Paint����
	ERESULT __stdcall LeaveForPrepare(IEinkuiIterator* npRecipient);

	// ����PaintǰԤ����
	ERESULT __stdcall EnterForPaint(IEinkuiIterator* npRecipient);

	// ����Paint����
	ERESULT __stdcall LeaveForPaint(IEinkuiIterator* npRecipient);

	// ����CaptureǰԤ����
	ERESULT __stdcall EnterForCapture(IEinkuiIterator* npRecipient);

	// ����Capture����
	ERESULT __stdcall LeaveForCapture(IEinkuiIterator* npRecipient);

	// ����EMSG_DISCARD_DEVICE_RESOURCEǰԤ����
	ERESULT __stdcall EnterForDiscardDeviceRes(IEinkuiIterator* npRecipient);

	// ����EMSG_DISCARD_DEVICE_RESOURCE����
	ERESULT __stdcall LeaveForDiscardDeviceRes(IEinkuiIterator* npRecipient);

	// �趨�Ի溯�����趨��Xuiϵͳ���ڵ���Eink���ƣ�������rgb32�Ļ������ṩ���˴��趨�Ļص�����
	void SetCustomDraw(PXUI_CUSTOM_DRAW_CALLBACK CustomDraw);

	// ����Eink����
	void ClearEinkBuffer(bool nbClear) {
		InterlockedExchange(&mlResetEinkBuf, (LONG)nbClear);
	}

	//HRESULT SaveToImageFile(D3D10_MAPPED_TEXTURE2D* npMapped); for test

private:
	void DrawEink(
		IN bool nbRefresh = false	// �����ύȫ��
	);

	// ���¶�λ
	void RelocationPainboard(void);

//////////////////////////////////////////////////////////////////////////
// �������ͼ�йص��豸��ػ��豸�޹���Դ
//////////////////////////////////////////////////////////////////////////
public:
	CXelManager* mpElementManager;
	ULONG muEinkPanelW;
	ULONG muEinkPanelH;
	ULONG muFixedW;
	ULONG muFixedH;
	volatile bool mbStopPainting;
	PXUI_CUSTOM_DRAW_CALLBACK mpCustomDraw;
	ULONGLONG mxUpdatingNumber;
	PXUI_EINKUPDATING_CALLBACK mpEinkUpdatingCallBack;
	LPVOID mpEinkUpdatingContext;

	//////////////////////////////////////////////////////////////////////////fs
	// �豸�޹���Դ
	ID2D1Factory* mpD2dFactory;
	IWICImagingFactory* mpWicFactory;
	IDWriteFactory* mpWriteFactory;
	IDWriteTextFormat* mpDbgTextFormat;

	//////////////////////////////////////////////////////////////////////////
	// �豸�����Դ
	ID2D1RenderTarget* mpTarget2D;		// Direct2D����ȾĿ��
	ID2D1Bitmap* mpTargetBitmap;
	ID2D1SolidColorBrush* mpDbgTextBrush;
	ID2D1SolidColorBrush* mpFogBrush;

	// ����������
	volatile LONG mlRenderStep;	//���ƽ׶�
	IEinkuiMessage* mpMessage;
	ULONG muCrtTick;
	TRrdStateStack moRenderState;
	D2D1::Matrix3x2F mdIdentyMatrix;
	IEinkuiIterator* mpToCapture;
	bool mbCapturing;
	D2D1_RECT_F  mdCaptureRegion;
	D2D_SIZE_F	 mdCaptureBriefSize;
	D2D1::Matrix3x2F mdScalMatrixToCapture;
	D2D1_COLOR_F mdBackColor;

	// ģ̬�Ի�����˸����
	IEinkuiIterator* mpModalTrunk;
	ULONG muFlashModalTick;

	// Layered Window ��
	HBITMAP mhForeBmp;			// ǰ̨��ͼ���õ�GDIλͼ��������Layered window ģʽ
	HDC mhForeDc;				// ����Layered window ģʽ�£�������UpdateLayeredWindowʱʹ��
	HBITMAP mhOldForeBmp;			// �������滻����Ĭ��λͼ
	UCHAR* mpForeBuffer;			// ǰ̨�û�ͼλͼ�Ļ�����������Layered window ģʽ
	PEI_BUFFER mpEinkBuf;
	CEiUpdate moEiUpdate;
	LONG mlResetEinkBuf;



	// ͳ��֡��
	volatile ULONG muRenderCount;
	volatile ULONG muLastTick;
	volatile FLOAT mfFpsIndx;


};





#endif