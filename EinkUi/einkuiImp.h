/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _XUIMAIN_H_
#define _XUIMAIN_H_
#include "ImeContext.h"
#include "XsThreadBlock.h"

class CXD2dEngine;
class CXelManager;
class CXuiIterator;
class CXsWidget;
class CXelAllocator;
class CEleMgrProxy;

struct _STES_START_UP {
	D2D1_SIZE_F Dpi;
	const wchar_t* ModulePathName;		// System Widget��ʵ��ģ����ļ�·����
	const wchar_t* HomeTempleteName;	// System Widget��Home Page��templete key������
	const wchar_t* ClassesRegPath;
	const wchar_t* WindowTitle;
	HANDLE WaitingCaller;
	ERESULT Result;
	PEINKUI_CUSTOMDRAW_SETTINGS CustomDraw;
	unsigned long AutoRotate;
};
typedef _STES_START_UP STES_START_UP,* PSTES_START_UP;



// ��ʱ��
struct _STES_TIMER{
	ULONG muID;
	ULONG muBaseTick;	// the tickcount when timer was set
	ULONG muRecentTick;	// the tickcount when the timer was kicked recently
	ULONG muEndTick;	// 1/1000 second same as windows' tickcount
	ULONG muDuration;	// 1/1000 second
	ULONG muRepeat;		// ��Ҫ�ظ������Ĵ�����MAXULONG32��ʾ��Զ�ظ�
	ULONG muKicked;		// �Ѿ������Ĵ���
	void* mpContext;	// �û�ָ����������
	CXuiIterator* mpIterator;	// ��ʱ����������
};
typedef _STES_TIMER STES_TIMER,* PSTES_TIMER;

class CEsTimerCriterion	// Ĭ�ϵ��ж�׼��
{
public:
	bool operator () (const PSTES_TIMER Obj1,const PSTES_TIMER Obj2)const // һ��Ҫ����������
	{
		// ������Ob1С�ڶ���Obj2ʱ������True�����򷵻�false
		return (Obj1->muEndTick <Obj2->muEndTick || Obj1->muEndTick == Obj2->muEndTick && Obj1 < Obj2);
	}
};

typedef cmmSequence<PSTES_TIMER,CEsTimerCriterion> TEsTimerSequence;

typedef cmmVector<CXsWidget*,64,64> TEsWidgetVector;

class CEsThreadNode{
public:
	ULONG muThreadID;
	IXsWidget* mpOwnerWidget;
	void operator=(const class CEsThreadNode& src){
		muThreadID = src.muThreadID;
		mpOwnerWidget = src.mpOwnerWidget;
	}
};

class CEsThreadNodeCriterion	// Ĭ�ϵ��ж�׼��
{
public:
	bool operator () (const CEsThreadNode& Obj1,const CEsThreadNode& Obj2)const // һ��Ҫ����������
	{
		// ������Ob1С�ڶ���Obj2ʱ������True�����򷵻�false
		return (Obj1.muThreadID < Obj2.muThreadID);
	}
};

// ����ID������̶߳���
typedef cmmSequence<CEsThreadNode,CEsThreadNodeCriterion> TEsThreadSequence;

// ��ʱ��ID
#define ELMGR_TIMERID_RENDER 1	// ����
#define ELMGR_TIMERID_LAZY 2	// ��ˢ��


// Windows User �̻߳ص���Windows��Ϣ����
#define ES_WINUI_CUSTOM_MSG L"{7F139C67-035E-43D7-A18C-D4ADCEF08B2E}"

// Windows User �̻߳ص�����
typedef struct _STES_WINTHREAD_CALLBACK{
	ULONG Signature;			// must be 'WgCc'
	ULONG Size;					// size of this struct
	ULONG BlockID;
	//HANDLE WaitEvent;
	IBaseObject* Applicant;		// �ص�����
	PXUI_CALLBACK ProcedureToCall;	//�ص����̣����̵ķ���ֵ���᷵�ظ�������
	ULONG Flag;			// ���ݸ��ص������Ĳ���
	LPVOID ConText;		// ���ݸ��ص������Ĳ���
}STES_WINTHREAD_CALLBACK,* PSTES_WINTHREAD_CALLBACK;


// Windows��Ϣ�������
class CEsWsgCapture{
public:
	UINT muiWsgID;
	IEinkuiIterator* mpAppItr;
	IXsElement* mpApplicant;
	PWINMSG_CALLBACK mpProcedure;
	void operator=(const class CEsWsgCapture& src){
		muiWsgID = src.muiWsgID;
		mpApplicant = src.mpApplicant;
		mpProcedure = src.mpProcedure;
		mpAppItr = src.mpAppItr;
	}
};

class CEsWsgCaptureCriterion	// Ĭ�ϵ��ж�׼��
{
public:
	bool operator () (const CEsWsgCapture& Obj1,const CEsWsgCapture& Obj2)const // һ��Ҫ����������
	{
		// ������Ob1С�ڶ���Obj2ʱ������True�����򷵻�false
		return (Obj1.muiWsgID < Obj2.muiWsgID);
	}
};

// ����ID������̶߳���
typedef cmmMultiSequence<CEsWsgCapture,CEsWsgCaptureCriterion> TEsWsgCaptor;


DECLARE_BUILTIN_NAME(CEinkuiSystem)
class CEinkuiSystem: public cmmBaseObject<CEinkuiSystem,IEinkuiSystem,GET_BUILTIN_NAME(CEinkuiSystem)>
{
	friend IEinkuiSystem* __stdcall EinkuiGetSystem(void);
	friend class CEleMgrProxy;

public:
	enum ERENDER_STEP{
		eRenderBegin=1,
		eRenderParepare=2,
		eRenderRender=3,
		eRenderEnd=4,
		eRenderStop=5
	};

	// ���Ψһ����
	static CEinkuiSystem* GetUniqueObject(void);

	// ����Ԫ�ع���
	virtual IXelManager* __stdcall  GetElementManager(void);	// ���Բ���ʧ�ܣ������жϷ���ֵ�Ƿ�ΪNULL

	// ��õ�ǰ΢���ӿڣ������ͷ�;���ĳ���߳�û��ʹ��CreateWidgetWorkThread��������ô���������߳��е���GetCurrentWidget�������쳣����
	virtual IXsWidget* __stdcall GetCurrentWidget(void);	// ���Բ���ʧ�ܣ������жϷ���ֵ�Ƿ�ΪNULL

	// ��÷������������������ڽ��������򷢲���Element���󣬷��صĶ��������ͷ�
	virtual IXelAllocator* __stdcall GetAllocator(void);

	// ��õ�ǰ�Ľ������������ַ������磺��������'chn'
	virtual const wchar_t* __stdcall GetCurrentLanguage(void);

	// ���ϵͳ��DPI����
	virtual FLOAT __stdcall GetDpiX(void);
	virtual FLOAT __stdcall GetDpiY(void);

	// ���û��壬��������װ����ɺ󣬵�������������û��壻�ڴ�֮ǰ���岢������Ƴ�ʵ��ͼ���Ա�������������ʱ��Eink��Ļ�������
	virtual ERESULT __stdcall EnablePaintboard(bool nbIsDisable = false);

	// ���û��壬����Ļ��ת����ʱ�����ñ�������ʹϵͳ���û���
	virtual void __stdcall ResetPaintboard(void);

	// ��������ڵĴ�С
	virtual void __stdcall GetPaintboardSize(
		OUT EI_SIZE* npSize	// ��ȡ�����С
	);

	// ���ñ�������Eink Panel�ϵ���ʾλ�ã�ֻ�����Ի����
	virtual void __stdcall SetPositionInPanel(
		ULONG nuX,
		ULONG nuY
	);



	// ��ȡEinkϵͳ����
	virtual PEI_SYSTEM_INFO __stdcall GetEinkSystemInfo(void) {
		return &mdEinkInfo;	
	}

	// ���Windows����
	virtual HWND __stdcall GetMainWindow(void){
		return mhMainWnd;
	}

	// ��ʾ��������WindowsͨѶ����
	virtual void __stdcall ShowMainWindow(bool nbShown=true);

	// ����Xui
	int Startup(
		STES_START_UP& nrStart
	);

	// ����һ���µ�Widget
	virtual ERESULT __stdcall LaunchWidget(
		IN const wchar_t* nswModulePathName,	// ��Widget��ģ���ļ���·��������ʵ�ִ�Widget��DLL����
		IN const wchar_t* nswHomeTempleteName,	// ��Widget��HomePage��Templete Key�����֣����Key������ProFile ��Root��
		IN const wchar_t* nswInstanceName,		// �������е�ʵ������ʵ����������ͬ�����������ͬ��ʵ������ϵͳ���᷵��ʧ��
		IN IEinkuiIterator* npFrame,	// ���ڶ�λ����ʾ��װ��Widget��Frame Element
		IN ICfKey* npInstConfig,	// Widgetʵ��ר������
		OUT IXsWidget** nppWidget	// ���Բ���д�����ڷ����½�����Widget�ӿڣ����صĽӿ���Ҫ�ͷ�
		);

	// ��õ�ǰϵͳ�е�ĳ��Widget�ӿڣ��������NULL��ʾ�˱��֮��û��Widget��
	// �˺���ֻ�ܱ�System Widget����
	virtual IXsWidget* __stdcall ObtainWidget(
		IN int niNumber		// ��0��ʼ��ŵ�Widegt�����û�����壬ֻ��Widget�Ĵ洢λ��
		);

	// ���ϵͳWidget
	virtual IXsWidget* __stdcall GetSystemWidget(void);

	// Widgetѹջ����CXelManager���ã����ַ�һ����Ϣ��Ŀ��Elementʱ���ã���ʾ�л����µ�Widget״̬
	void PushWidget(IXsWidget* npWidget);

	// Widget��ջ����ElementManager�����һ��Element����Ϣ�ǵ��ã��ָ���֮ǰ��Widget״̬
	void PopWidget(void);

	// ����һ�����̣߳����е�Widget��Ӧ��ʹ��������������Լ��Ķ����̣߳�����ֵ��Windows�߳̾�����������ڵ���SuspendThread/ResumeThread/GetExitCodeThread
	// ���գ����صľ���������CloseHandle�رգ��������������ͬWindows API CreateThreadһ��
	virtual HANDLE __stdcall CreateWidgetWorkThread(
		LPSECURITY_ATTRIBUTES lpThreadAttributes,
		SIZE_T dwStackSize,
		LPTHREAD_START_ROUTINE lpStartAddress,
		LPVOID lpParameter,
		DWORD dwCreationFlags,
		LPDWORD lpThreadId
		);

	// �˳�Widget�����̣߳����������CreateWidgetWorkThread���߳��������˳�ʱ��ϵͳ���Զ�������ֹ�̲߳���
	virtual void __stdcall ExitWidgetWorkThread(DWORD dwExitCode);

	// ��һ��Config�ļ������ڴ�һ��config�ļ���ĿǰӦ����Factory�ӿ�ʵ���У����ڴ�һ��Conponent��Ӧ��Profile
	virtual IConfigFile* __stdcall OpenConfigFile(
		IN const wchar_t* nszPathName,				// �ļ�������·����
		IN ULONG nuCreationDisposition=CF_OPEN_EXISTING	// ͬCreateFile API���ƣ���������CF_CREATE_ALWAYS��CF_CREATE_NEW��CF_OPEN_ALWAYS��CF_OPEN_EXISTING�������CfgIface.h
		);

	// ��ĳ��Element��������children����
	virtual IEinkuiBitmap* __stdcall TakeSnapshot(
		IEinkuiIterator* npToShot,
		const D2D1_RECT_F& crSourceRegion,	// ��������Ŀ��Ԫ�صľֲ�����ϵ
		const D2D_SIZE_F& crBriefSize,		// ����ͼ�ߴ磬���յĽ����һ������ͼ
		const FLOAT* ColorRGBA=NULL
		);

	// �������»����Ը���������ͼ��Idealife��ģʽ��������ͼ�ػ棬ֻҪ����һ�ξͻ�������������ͼ��
	// �ظ����ñ����������ᵼ���ظ��Ļ��ƣ�
	virtual void __stdcall UpdateView(
		IN bool nbRefresh = false	// �����ύȫ��
		);

	// ����Eink���ƻص���ÿ��UIϵͳ��Eink�����ύʱ����ָ���Ļص�����
	virtual ERESULT __stdcall SetEinkUpdatingCallBack(
		IN PXUI_EINKUPDATING_CALLBACK npFunction,
		IN PVOID npContext
	);

	// ����Eink���棻����Ĺ���ԭ����ͨ��Eink�����е������������ȶԴ���ʾ���ݣ�ֻ����ͬ�Ĳ��ַ���Eink�����Eink��Ļ��Ҫȫ���ػ�
	//    ����Ҫ����Eink���棬ʹ��ȫ�����ݻ��Ƶ�Eink����Ҫ����App���»Eink��Ļ�����
	virtual void __stdcall ClearEinkBuffer(void);

	// ���Direct2D�Ĺ����ӿڣ������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
	// ͨ������CXuiGraphy���ͬ������ʵ�֣�ͬ����ȫ��ͬ
	virtual ID2D1Factory* __stdcall GetD2dFactory(void);

	// ���WIC�����������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
	// ͨ������CXuiGraphy���ͬ������ʵ�֣�ͬ����ȫ��ͬ
	virtual IWICImagingFactory* __stdcall GetWICFactory(void);

	// ���Direct Write�����������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
	// ͨ������CXuiGraphy���ͬ������ʵ�֣�ͬ����ȫ��ͬ
	virtual IDWriteFactory* __stdcall GetDWriteFactory(void);

	// ����ʹ��Windows�����̻߳ص������Widget��������Ҫ����Windows�Ľ���ģ�飬��ʾ�����Windows�����������Windows������صı�����Windows�����߳���ִ�еĹ��ܣ�
	// ���ù��ܴ�����ֲ���Լ�Element�����������IBaseObject�����һ�����������У�ͨ�����ñ�����������ʹ��Windows�����߳������ص��趨�Ķ������������ú����ķ���ֵҲ��
	// ͨ��������ֱ�ӷ��ظ��������ĵ����ߡ�����������ܵĹ����У������Ĵ��뽫�����ڴ˷����У�����ȷ��������������ˢ�У�����Xui����Ľ��潫������Ӧ��
	virtual ERESULT __stdcall CallBackByWinUiThread(
		IBaseObject* npApplicant,		// �ص����󣬲���һ��Ҫ���ǵ��ñ������Ķ�����Ҳ�����Ǳ�Ķ���ָ��
		PXUI_CALLBACK npProcedureToCall,	//�ص����̣����̵ķ���ֵ���᷵�ظ�������
		ULONG nuFlag,			// ���ݸ��ص������Ĳ���
		LPVOID npConText,		// ���ݸ��ص������Ĳ���
		bool nbStall=false		// �ȴ������ڼ��Ƿ���Ҫ���������XUI��Ϣ�����nbStal��Ϊture����ֱ�ӵȴ����أ���������XUI��Ϣѭ��
		// ʹ�ô˷������Ա�����ñ������ڼ䣬�ٴ��������
		);

	// ע��Windows��Ϣ���أ�ͨ�������ܿ�����XUIϵͳ����Windows��Ϣ֮ǰ��ȡ��ע��Windows��Ϣ
	// �����ȡ��Windows��Ϣ�Ĺ���Ҫ�����ܶ��ݡ�
	// ����ERESULT_WINMSG_SENDTO_NEXT��XUI����Ϣ���ݸ���һ�������ߣ����߽���XUIϵͳ���ʹ�����������ֵ����ֹ��Windows��Ϣ�Ĵ��ݹ����Լ�XUI�Ը���Ϣ�Ĵ���
	virtual ERESULT __stdcall CaptureWindowsMessage(
		IN UINT nuiWinMsgID,	// ϣ�������Windows��Ϣ��ID
		IN IXsElement* npApplicant,	//����ص��Ķ���
		IN PWINMSG_CALLBACK npProcedure	//��Windows��Ϣ���͸��˺���
		);

	// ����һ������˷���ͨ��ΪSystemWidget�Ĳ˵�ģ����ã�����ģ��һ���û�������ϼ���Ϊ��������ᱻ��������ǰ�ļ��̽������ϵľ���EITR_STYLE_COMMAND�Ķ���
	virtual ERESULT __stdcall GenerateCommand(
		nes_command::ESCOMMAND neCmd
		);

	// ����ĳҳ��Ϊģ̬�Ի���ʽ�����û�������ɸöԻ�����ʱ�����ϵ��������ֶ��޷�ʹ�á�
	// XUI��ģ̬�Ի���ʽ��ȫ�ֵģ�����ģ̬�Ի��£����е�Widget������System Widget)��������Ӧ�û����룻���У������ܱ���ʹ��ģʽ�Ի����������Ǳ���ġ�
	// ʹ�÷����ǣ����ȴ���ģʽ�Ի���������Ĭ������)��Ȼ����ñ���������ģ̬�Ի���ʽ����ʱ��ģ̬�Ի�Ԫ�ض��󽫻��յ�һ��EMSG_MODAL_ENTER��Ϣ�����������������Լ���ʾ������
	// ����ģ̬�Ի������ж��û�����˶Ի��������Լ�������ExitModal�˳�ģ̬�Ի���ʽ
	// ע�⣬ģ̬�Ի���ʽ�ǿ����ص�����ģ���ģ̬�Ի��£����Դ���ģ̬�Ի�������һ����˳�
	virtual ERESULT __stdcall DoModal(
		IEinkuiIterator* npModalElement		// ��Ԫ����ģ̬�Ի���������
		);


	// �˳�ģ̬�Ի���ʽ
	virtual void __stdcall ExitModal(
		IEinkuiIterator* npModalElement,	// ��Ԫ����ģ̬�Ի���������
		ERESULT nuResult
		);

	// �����ڵ�Windows��Ϣ���̺���
	static LRESULT CALLBACK MainWindowProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
		);

	// XUI���潻���߳�
	static ULONG WINAPI OperationThread(PSTES_START_UP npStartup);

	// Widget�����̵߳����
	static ULONG WINAPI WidgetWorkThread(LPVOID Context);

	// ����߳�
	static ULONG WINAPI DetermineThread(LPVOID Context);


	// ���붨ʱ�����������ô����Ķ�ʱ������Ҫע��
	ERESULT SetTimer(
		IN CXuiIterator* npIterator,// ������
		IN ULONG nuID,	  // ��ʱ��ID
		IN ULONG nuRepeat,// ��Ҫ�ظ������Ĵ�����MAXULONG32��ʾ��Զ�ظ�
		IN ULONG nuDuration,	// ��������
		IN void* npContext//�����ģ������Ŷ�ʱ����Ϣ���͸�������
		);

	// ���ٶ�ʱ��
	ERESULT KillTimer(
		IN CXuiIterator* npIterator,// ������
		IN ULONG nuID	  // ��ʱ��ID
		);

	// ȷ�ϵ�ǰ�߳��Ƿ��ǲ����߳�
	bool IsRunningInOperationThread(void);

	// ȷ�ϵ�ǰ�߳��Ƿ���Windows�����߳�
	bool IsRunningInWindowsUiThread(void);

	// ��鲢��ִ��Windows�̵߳�Callback���󣬱�����ֻ�ܱ�ִ����Windows�߳��еĻ�����סWindows�̵߳ĵط�����
	// returns false if we got wm_quit message
	bool RunWindowsUICallback(void);

	// ���Ԫ�ع���������
	CXelManager* GetElementManagerObject(void){
		return mpElementManager;
	}

	// ���Ime�����Ķ���
	CXsImeContext* GetImeContext(void){
		return mpImeContext;
	}

	// �����Ⱦ�׶�
	CEinkuiSystem::ERENDER_STEP GetRenderStep(void);

	bool IsOptLocked(void){
		return mbLocked;
	}

	// �˳�����XUIϵͳ
	virtual void __stdcall ExitXui(void);

	virtual void __stdcall SystemTest(ULONG nuValue);

	CXsBmpList& GetBitmapList(void){
		return moAllBitmaps;
	}

	CXsBrushList& GetBrushList(void)
	{
		return moAllBrushes;
	}

	// �����ǰ���ģ̬����
	IEinkuiIterator* GetTopModalElement(void);

	//// ������˸Domodal�Ĵ���
	//void FlashModalElement(IEinkuiIterator* npDodal);
	//���β����Ƿ���TOUCH����
	virtual bool __stdcall IsTouch(void);
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


public:
	// Ψһʵ��
	static CEinkuiSystem* gpXuiSystem;


protected:
	// �ڲ�����
	// ͼ������ӿ�
	CXD2dEngine* mpXuiGraphics;
	// ��ͼ��Դ����ָ��
	IEinkuiPaintResouce* mpPaintResource;
	// EINK�豸��Ϣ
	EI_SYSTEM_INFO mdEinkInfo;
	ULONG muAutoRotate;

	HWND mhMainWnd;
	//bool mbWindowHide;
	D2D1_SIZE_F mdDpi;
	LONG mlNewPaintboardWidth;
	LONG mlNewPaintboardHeight;
	LONG mlNewPaintboardX;
	LONG mlNewPaintboardY;

	CXsBmpList moAllBitmaps;
	CXsBrushList moAllBrushes;

	CXelManager* mpElementManager;

	// ��ʱ��
	TEsTimerSequence moTimers;
	CExclusiveAccess moTimerListLock;
	STEMG_LAZY_UPDATE mdLazyUpdate;

	// widget����
	//TEsWidgetStack moWidgetContext;
	CXsWgtContext moWidgetContext;	// �����̵߳�Widgetִ��ջ
	TEsWidgetVector moAllWidgets;	// ȫ��Widget
	TEsThreadSequence moWidgetWorkThreads;	// ȫ�������߳�
	ULONG muOperationThreadID;	// �����߳�
	CExclusiveAccess moWidgetLock;

	// �����߳̾��
	HANDLE mhOperationThread;
	bool mbThreadKilled;
	bool mbFirstRun;
	bool mbLocked;	// ���ڼ򵥵���һ����������ʾ����Windows�����̷߳��ָñ���������ʱ�����ٶ������̱߳������߳��������ȡ������޸ģ��������߳��ж�ʱ�Ϳ��ܳ�����

	// �������
	HANDLE mhExitDetermine;

	// ������
	CXelAllocator* mpAllocator;

	// ���ƿ���
	volatile LONG miDisableRender;
	volatile LONG miToRender;
	volatile LONG miDiscardRenderMessage;
	volatile LONG miRefreshEink;
	volatile ULONG muRenderTick;		// ���һ����Ⱦ��TickCount
	volatile LONG miLostToRender;	// ��Ϊû�л���Ҫ�������һ�λ���ȱʧ��֡��
	//ULONG muAutoRenderInterval;	// �Զ����Ƽ��
	volatile LONG miMissedMouseTest;	// ���һ�λ��ƺ��û���յ������Ϣ�Ķ�ʱ���ﵽ������һ�����������Ҫ������
	//ULONG muRecentRenderTick;	// ���һ�λ��Ƶ�ʱ���
	//ULONG muNextRenderTick;		// ��һ�λ��Ƶ�ʱ���

	// Windows �̻߳ص�����
	ULONG muWinThreadID;	// Windows�����߳�
	UINT muiCustomWinMsg;// WinUI�Զ�����Ϣ
	CXsWgtContext moWinWgtContext;	// Windows�����߳��̵߳�Widgetִ��ջ
	CExclusiveAccess moWinThreadLock;
	LONG mlWinCallBack;
	//ULONG muWinUiCallBackID;
	//ERESULT muOpLoopResult;
	CXuiThreadBlock moThreadBlock;
	CXuiModalStack moModalStack;

	// Windows ��Ϣ����
	TEsWsgCaptor moCaptors;
	CExclusiveAccess moCaptorLock;

	// ���
	USHORT msuWinCap;
	// IME
	CXsImeContext* mpImeContext;

	//�Ƿ��Զ�����̵Ķ���
	bool mbIsTouch;

	//����������Ԥ��ͼ�񣬵�������С��������ʱʹ��
	HBITMAP mpSmallPreView;
	HBITMAP mpBigPreView;

	CEinkuiSystem();
	~CEinkuiSystem();

	DEFINE_CUMSTOMIZE_CREATE(CEinkuiSystem,(),())

	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate(void);

	// ע�����й�����Ϣ
	bool RegisterCommonWindowsMessage(void);

	// ��������
	ERESULT CreateMainWnd(
		IN int niX,		// ����������Ļ�ϵ����Ͻ�X����
		IN int niY,		// ����������Ļ�ϵ����Ͻ�Y����
		IN int niWidth,		// ����������Ļ�ϵĿ��
		IN int niHeight,	// ����������Ļ�ϵĸ߶�
		IN const wchar_t* nswWndTittle		// �����ڱ���
		);

	// Windows�����̣߳���ѭ��
	int HostThreadLoop();

	// �������ƶ�ʱ��
	void EnableRender(void);

	// ��ֹ���ƶ�ʱ��
	void DisableRender(void);


	// ����Windows WM_CREATE��Ϣ
	LRESULT WsgOnCreate(HWND nhWindow);

	// ���һ���ɾ��һ����ʱ��������ʱ���������ڲ�����
	bool FindTimer(
		IN CXuiIterator* npIterator,// ������
		IN ULONG nuID,	  // ��ʱ��ID������Ϊ0
		IN bool nbDelete=false	// �Ƿ�ɾ��
		);

	// �Ӷ�ʱ��������ȡ���һ�����ܼ�����ʱ������tick�����룩Ϊ��λ
	ULONG GetTickCountToKickTimer(ULONG nuCrtTick);

	// ��ʱ����������
	void KickTimers(ULONG nuCrtTick);

	// ���������߳�
	ERESULT ExecuteOperationThread(PSTES_START_UP npCreate);

	ERESULT ExecuteDetermineThread(LPVOID Context);

	// ���ڽ������з��͸�ϵͳ����Ϣ
	ERESULT SystemMessageReceiver(IEinkuiMessage* npMsg);

	// ��ʱ����Ϣ
	ERESULT OnMsgTimer(const PSTEMS_TIMER npTimerInput);

	// �������߳�����״̬
	void DetermineOPRunming(void);

	// �ص������ߵĺ������ɹ���������̷߳��ͻָ���Ϣ
	void CallClientProcedure(PSTES_WINTHREAD_CALLBACK npToCall);

	// �����߳���Ϣ����ѭ��
	ERESULT OpThreadMessageLoop(ULONG nuBlockID);

	// ��ˢ�·���Ԥ����
	ERESULT __stdcall EnterForLazyUpdate(IEinkuiIterator* npRecipient);

	// ��ˢ�·��ͺ���
	ERESULT __stdcall LeaveForLazyUpdate(IEinkuiIterator* npRecipient);

	ERESULT DoCapture(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam,
		HRESULT& rWinResult
		);

	// �յ�Windows Mouse��Ϣ
	void OnWinMouse(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	// �յ�Eink Touch��Ϣ
	void OnEinkTouch(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


	// ����windows��겶��
	void SetWinMouseCapture(USHORT nsuFlag);

	// �ͷ�windows��겶��
	void ReleaseWinMouseCapture(USHORT nsuFlag);

	void WsgOnGetMinMaxInfo(MINMAXINFO *pMinMaxInfo);

	//void WsgOnSettingChanged();	discarded by ax mar.03,2013 ȥ���Զ��ߴ�ı�Ĺ���

	//ERESULT __stdcall CorrectMainWindowSize(ULONG nuFlag,LPVOID npContext);

	ERESULT CloseWidget(
		IN IXsWidget* npWidget
		);

	void EndXui(void);

	// �����ڵ�Windows��Ϣ���̺���
	LRESULT MessageFromXuiToWindowUi(
		WPARAM wParam,
		LPARAM lParam
		);

	// ϵͳ���ƹ���
	void RenderProcedule(ULONG nuCrtTick);

};


#define ESYS_FLAG_INITOK 1




// ���������߳��������Ĳ���
struct _STES_CREATE_THREAD{
	IXsWidget* Widget;
	LPTHREAD_START_ROUTINE ThreadProc;
	LPVOID Context;
};
typedef _STES_CREATE_THREAD STES_CREATE_THREAD,* PSTES_CREATE_THREAD;













































#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif//_XUIMAIN_H_