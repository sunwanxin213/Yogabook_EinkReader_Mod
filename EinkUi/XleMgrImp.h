#ifndef _ELEMGRIMP_H_
#define _ELEMGRIMP_H_
/*
	���ļ�����Ԫ�ع�����ʵ����

*/
#include "BpTree.h"
#include "XsSysElement.h"

class CXelManager;
class CXuiIterator;

#define ELMGR_MAX_FREE_MESSAGE 64
#define ELMGR_HOVER_DURATION 800
#define ELMGR_TIP_DURATION 5000
#define ELMSG_MAX_MOUSE_TRACK 36


#pragma pack(4)


// �����̻߳�����Ϣ������Ӧ��
#define EMSG_WAKE_UP EMSG_DEFINE(LMSG_TP_SYSTEM,100,1)
// input none
// output none

// ϵͳ�˳���Ϣ�����͸�Ԫ�ع�������֪ͨ�˳�
#define EMSG_QUIT_XUI EMSG_DEFINE(LMSG_TP_SYSTEM,100,2)
// input none
// output none

// ϵͳ��Ļ��ת
#define EMSG_SCREEN_ROTATION EMSG_DEFINE(LMSG_TP_SYSTEM,100,5)
// input ULONG as screen-orientation
// output NONE

// ��Ⱦ����
#define EMSG_SYSTEM_RENDER EMSG_DEFINE(LMSG_TP_SYSTEM,100,10)
// input ULONG : 0 render,1 stop discarding render message
// output none

// Windows �����̻߳ص����
#define EMSG_WINCALL_COMPLETED EMSG_DEFINE(LMSG_TP_SYSTEM,101,1)
// input ULONG[2]  CallBackID and ERESULT
// output none
// result na

// ģ̬�Ի����
#define EMSG_MODAL_COMPLETED EMSG_DEFINE(LMSG_TP_SYSTEM,101,2)
// input ULONG��ERESULT produce by the MODAL
// output none
// result na

// ��ס�����߳�
#define EMSG_OPTHREAD_LOCK EMSG_DEFINE(LMSG_TP_SYSTEM,101,3)
// input HEVENT[2],the first the OPThread is waiting to proceed
struct _STEMS_OPT_LOCK {
	HANDLE OptToRelease;
	HANDLE OptToWait;
};
typedef _STEMS_OPT_LOCK STEMS_OPT_LOCK, *PSTEMS_OPT_LOCK;
// output 
// result 

// �ر�Widget
#define EMSG_CLOSE_WIDGET EMSG_DEFINE(LMSG_TP_SYSTEM,101,10)
// input IXsWidget* the pointer to the widget to close
// output none 
// result na

// ��������Ԫ�أ�Sender�����������ٵ�Ԫ��
#define EMSG_APPLY_DESTROY  EMSG_DEFINE(LMSG_TP_SYSTEM,102,1)
// input none
// output none
// result na



// ��ü��̽���
#define EMSG_SET_KEYBOARD_FOCUS EMSG_DEFINE(LMSG_TP_SYSTEM,105,1)
// input IEinkuiIterator*, Element to get keyboard focus
// output none
// result na

// �ͷż��̽���
#define EMSG_RELEASE_KEYBOARD_FOCUS EMSG_DEFINE(LMSG_TP_SYSTEM,105,2)
// input 
struct _ST_RELEASE_KEYFOCUS {
	IEinkuiIterator* CrtFocus;	// Element to release keyboard focus
	bool ShiftTab;	// ת�Ƶ���һ�����ܼ��̵�Ŀ�꣬��ͬ����Tab��
};
typedef _ST_RELEASE_KEYFOCUS ST_RELEASE_KEYFOCUS, *PST_RELEASE_KEYFOCUS;
// output none
// result na

// ���ü���
#define EMSG_SET_ACTIVE	EMSG_DEFINE(LMSG_TP_SYSTEM,105,5)
// input IEinkuiIterator*
// output none
// result na


//////////////////////////////////////////////////////////////////////////
// ���ת��
#define EMSG_MOUSE_FORWARD		EMSG_DEFINE(LMSG_TP_WIN_INPUT,1,1)
//intput
struct _STELEMGR_WINMSG_FORWARD {
	ULONG WinMsgID;
	WPARAM wParam;
	LPARAM lParam;
};
typedef _STELEMGR_WINMSG_FORWARD STELEMGR_WINMSG_FORWARD, *PSTELEMGR_WINMSG_FORWARD;
//output none

//////////////////////////////////////////////////////////////////////////
// ������Ϣת��
#define EMSG_KEYBOARD_FORWARD		EMSG_DEFINE(LMSG_TP_WIN_INPUT,1,2)
// input STELEMGR_WINMSG_FORWARD
// output none
// result na

//////////////////////////////////////////////////////////////////////////
// ������������ϢLMSG_GET_TYPE() == LMSG_TP_WIN_INPUT
#define EMSG_INPUT_ENABLE			EMSG_DEFINE(LMSG_TP_WIN_INPUT,2,1)
// INPUT NONE
// OUTPUT NONE
// RESULT NA

// �޸Ĵ˴��룬��������Ϣ���������Ϣת���������߼�δ������Ax.2017.08.16
////////////////////////////////////////////////////////////////////////////
//// ��������Ϣת��
//#define EMSG_TOUCH_FORWARD			EMSG_DEFINE(LMSG_TP_WIN_INPUT,1,4)
//// input STEMS_TOUCH
//// output none
//// result na

// ��ȡһ����ǰ�Ļ�����״
#define EMSG_MAIN_GET_CURRENT_BITMAP EMSG_DEFINE(LMSG_TP_SYSTEM,103,2)
// intput LPARAM ͬ�����Ϣ
// output HBITMAP
// result na


#pragma pack()


#define ES_ETYPE_MANAGER L"ElementManager"


typedef bplustree<IEinkuiIterator*> TEIteratorVerification;

typedef cmmStack<int, 32, 16>  TElEnumIndexStack;


typedef cmmQueue<IEinkuiMessage*, 64, 64> TElMessageQueue;
typedef cmmStack<IEinkuiMessage*, ELMGR_MAX_FREE_MESSAGE, ELMGR_MAX_FREE_MESSAGE> TElMessageStack;

class CElMouseFootPrint {
public:
	D2D1_POINT_2F Point;
	ULONG KeyFlag;		// ��ʱ��������ذ�ť��״�������ñ�ʾΪ���£����������ǵ�������ϣ�MK_LBUTTON/MK_RBUTTON/MK_MBUTTON/MK_XBUTTON1/MK_XBUTTON2/MK_CONTROL/MK_SHIFT
	ULONG TickCount;
	void operator=(const CElMouseFootPrint& src) {
		Point.x = src.Point.x;
		Point.y = src.Point.y;
		KeyFlag = src.KeyFlag;
		TickCount = src.TickCount;
	}
};
typedef cmmQueue<CElMouseFootPrint, ELMSG_MAX_MOUSE_TRACK, 16> TElMouseTrace;

class CElMouseTestState {
public:
	D2D1_POINT_2F Point;
	IEinkuiIterator* mpElement;
	LONG mlCrtLevel;
	bool mbSeekInLevels;
	void operator=(const CElMouseTestState& src) {
		Point.x = src.Point.x;
		Point.y = src.Point.y;
		mpElement = src.mpElement;
		mlCrtLevel = src.mlCrtLevel;
		mbSeekInLevels = src.mbSeekInLevels;
	}
};


//////////////////////////////////////////////////////////////////////////
// Ԫ�ع�����
DECLARE_BUILTIN_NAME(CXelManager)
class CXelManager : public cmmBaseObject<CXelManager, IXelManager, GET_BUILTIN_NAME(CXelManager)>
{
	friend class CEinkuiSystem;
public:
	CXelManager();
	~CXelManager();

	DEFINE_CUMSTOMIZE_CREATE(CXelManager, (), ())


		// ��ϵͳ������ע��һ��Element�����ص���������ʧ�ܷ���NULL
		// �ɹ����صĽӿڶ���
		virtual IEinkuiIterator* __stdcall  RegisterElement(
			IN IEinkuiIterator* npParent,	// ��Ԫ�صĵ�����
			IN IXsElement* npElement,	// ��ע�����Ԫ��
			IN ULONG nuEID = 0	// ��Ԫ�ص�EID����ͬһ����Ԫ���£�����Ԫ�ص�EID����Ψһ�����������EID��������=0��ϵͳ�Զ�����
		);

	// ��ϵͳ������ע��һ��Element���˹��ܽ�Ӧ�ñ���ע��Element�������XUIϵͳ���ã���������Ѿ�����
	virtual ERESULT __stdcall UnregisterElement(
		IN IEinkuiIterator* npElementIterator	// ��Ԫ�ض�Ӧ�ĵ�����
	);

	// ��֤һ��Iterator�Ƿ���Ч������ERESULT_SUCCESS��Ч������ERESULT_ITERATOR_INVALID��Ч
	virtual ERESULT __stdcall VerifyIterator(
		IN IEinkuiIterator* npIterator	// ������
	);

	// �ڶ���������в���һ��Element�����ظ�Element�ĵ���������ʧ�ܷ���NULL
	// �ɹ����صĽӿڶ��󣬲�ʹ��ʱ��Ҫ�ͷţ� ���������Ҫͨ��Ԫ�ػ������ע����������뱣���������ָ�룬��Ϊ���ñ�����ʹ��ȫ���������һ�ȡ���������󣬺�ʱ�ϴ�
	virtual IEinkuiIterator* __stdcall FindElement(
		IN IXsElement* npElement
	);

	// ��ø�Ԫ�أ������Ϊ�˸����������������Ϣ��Ҳ����ֱ��ʹ��NULLָ���ʾ���������
	// �ɹ����صĽӿڶ��󣬲�ʹ��ʱ��Ҫ�ͷ�
	virtual IEinkuiIterator* __stdcall GetRootElement(void);

	// ��ò˵�ҳ�����еĲ˵������������ҳ
	virtual IEinkuiIterator* __stdcall GetMenuPage(void);

	// ���ToolTipƽ�棬�����ߵ�ҳ
	virtual IEinkuiIterator* __stdcall GetToolTipPage(void);

	// �������Ԫ�أ�����Ԫ��������XUI����ʱָ��������Ԫ��ʵ�־���Ӧ�õ�ȫ�ֹ��ܣ��磺Idealife��ȫ��Ӧ����"Idealife"Ԫ�����ʵ���ṩ��
	// ͨ��������Idealife��Ӧ����Ϣִ��Idealife��ϵͳ����
	// �ɹ����صĽӿڶ��󣬲�ʹ��ʱ��Ҫ�ͷ�
	virtual IEinkuiIterator* __stdcall GetDesktop(void);

	// �����趨��Ԫ�أ�nbZTop==true������Zoder���㣬false�����ڵײ�
	virtual ERESULT __stdcall SetParentElement(IEinkuiIterator* npParent, IEinkuiIterator* npChild, bool nbZTop);

	// ����һ����Ϣ
	// ��Ϣ���ͺ󣬷�������Ȼ��Ҫ�ͷ�
	virtual IEinkuiMessage* __stdcall  AllocateMessage(void);

	// ����һ����Ϣ����ʼ����ز���
	// ��Ϣ���ͺ󣬷�������Ȼ��Ҫ�ͷ�
	virtual IEinkuiMessage* __stdcall AllocateMessage(
		IN ULONG nuMsgID,	// ��Ϣ����
		IN const void* npInputBuffer,	// �������ݵĻ�����
		IN int niInputSize,	// �������ݵĴ�С
		OUT void* npOutputBuffer,	// ���������(���ػ�����)
		IN int niOutputSize,	// �����������С
		IN bool nbInputVolatile = true	// ���뻺�����Ƿ�����ʧ�ģ��μ�IXuiMessage::SetInputData��ø�����Ϣ
	);

	// ��ָ��Ԫ�ط���һ����Ϣ������ģʽ��Send
	// ��Ϣ���ͺ󣬷�������Ȼ��Ҫ�ͷ�
	virtual ERESULT __stdcall SendMessage(
		IEinkuiIterator* npDestElement,	// ������Ϣ��Ŀ��Ԫ��
		IEinkuiMessage* npMsg
	);

	// ��ָ��Ԫ�ط���һ����Ϣ������ģʽ��Post
	// ��Ϣ���ͺ󣬷�������Ȼ��Ҫ�ͷ�
	virtual ERESULT __stdcall PostMessage(
		IEinkuiIterator* npDestElement,	// ������Ϣ��Ŀ��Ԫ��
		IEinkuiMessage* npMsg,
		IN ULONG nuPostType = EMSG_POSTTYPE_NORMAL	// ��Ϣ�����ȼ���EMSG_POST_FAST,EMSG_POST_REVERSE
	);

	// �򵥵ظ�ָ��Ԫ�ط���һ����Ϣ������ģʽ��Send���˺����ķ��سɹ�������Ϣ����ķ���ֵ�������ԭ��Ͳ�һ������Ϣ����ķ���ֵ����������Ϣ����ʧ��
	virtual ERESULT __stdcall SimpleSendMessage(
		IEinkuiIterator* npDestElement,	// ������Ϣ��Ŀ��Ԫ��
		IN ULONG nuMsgID,	// ��Ϣ����
		IN const void* npInputBuffer,	// �������ݵĻ�����
		IN int niInputSize,	// �������ݵĴ�С
		OUT void* npOutputBuffer,	// ���������(���ػ�����)
		IN int niOutputSize	// �����������С
	);

	// �򵥵ظ�ָ��Ԫ�ط���һ����Ϣ������ģʽ��Post���޷������Ϣ����ķ���ֵ���˺����ķ���ֵ����ʾ��Ϣ�Ƿ񱻳ɹ�������Ϣ����
	virtual ERESULT __stdcall SimplePostMessage(
		IEinkuiIterator* npDestElement,	// ������Ϣ��Ŀ��Ԫ��
		IN ULONG nuMsgID,	// ��Ϣ����
		IN const void* npInputBuffer,	// �������ݵĻ�����
		IN int niInputSize,	// �������ݵĴ�С
		IN ULONG nuPostType = EMSG_POSTTYPE_NORMAL	// ��Ϣ�����ȼ���EMSG_POST_FAST,EMSG_POST_REVERSE
	);

	// ö��ȫ��Ԫ�أ�ÿ������һ��Elementʱ����ö���������ṩ��ElementEnter��������һ��Ԫ��û����Ԫ��ʱ���������ṩ��ElementLeave
	// ��Ϊ���ڵ���XUIϵͳ���������ö�ٲ��ᴥ�����ڵ�
	virtual ERESULT __stdcall EnumAllElement(
		bool nbReverse,				// ����ָ����ö���ӽڵ�ʱ������Zorder��˳��ö�٣����߰���Zorder������ö��
		IBaseObject* npApplicant,	// �������
		ERESULT(__stdcall IBaseObject::*ElementEnter)(IEinkuiIterator* npRecipient),//�������ERESULT_ENUM_CHILD����ö�٣�����ERESULT_STOP_ENUM_CHILD����������ֵ��ֹͣö�ٴ�Ԫ�صĴ�Ԫ�ص���Ԫ��
		ERESULT(__stdcall IBaseObject::*ElementLeave)(IEinkuiIterator* npRecipient) //����ֵ������
	);

	// ����Iterator�����ã�����XUI�Ŀͻ�������ܻ���©��Iterator���ͷź����ò���������Ĭ�ϵ�Iterator->AddRef()��Iterator->Release()�����Ǽٵģ����������ʵ�ʵĵ��ã���Element��Close�󣬶�Ӧ��Iteratorһ��
	// �ᱻ�ͷţ��ڱ��ӿ����ṩ����ʵ�����ú��ͷŵķ�������Iterator�����м�Ҫ����������������ͷŽ��ᵼ��XUI������
	// ����Iterator����
	virtual int __stdcall AddRefIterator(IEinkuiIterator* npIterator);

	// �ͷ�Iterator
	virtual int __stdcall ReleaseIterator(IEinkuiIterator* npIterator);

	// �����꽹�㣬!!!ע��!!!�����صĶ���һ��Ҫ����ReleaseIterator�ͷţ�
	// ��Ϊ��꽹����ʱ���ܸı䣬���У����صĶ���һ������ȫ��ʵ�ķ�Ӧ��ǰ�������
	virtual IEinkuiIterator* __stdcall GetMouseFocus(void);

	// ��ü��̽��㣬!!!ע��!!!�����صĶ���һ��Ҫ����ReleaseIterator�ͷţ�
	// ��Ϊ���̽�����ʱ���ܸı䣬���У����صĶ���һ������ȫ��ʵ�ķ�Ӧ��ǰ�������
	virtual IEinkuiIterator* __stdcall GetKeyboardFocus(void);

	// ������ק��㣬����ϵͳ��������ק��Ϊ��ʱ�����Խ���תת�Ƹ�����
	// �����ͼת�Ƶ���Ŀ��Ԫ�ز���֧����ק��Ϊ����ǰ����ק��ΪҲ����ֹ����ǰ��ק��Ŀ��Ԫ�ػ��յ�Drag_end��Ϣ
	virtual ERESULT __stdcall ResetDragBegin(IEinkuiIterator* npToDrag);

	// ����������ϢLMSG_GET_TYPE(MsgId) == LMSG_TP_WIN_INPUT
	virtual void __stdcall CleanHumanInput(bool nbStallInput = false);

	// ע���ݼ�������ݼ���������ע���ݼ���Element�����ܵ���
	// �����ͨ������ϣ�������Alt��)���µĵ�ʱ�����ڼ��̽��㣬������Ϣ�����ȷ��͸����̽��㣬������㷵��ERESULT_KEY_UNEXPECTED�Ż��ж��Ƿ���ڿ�ݼ���Ϊ
	virtual bool __stdcall RegisterHotKey(
		IN IEinkuiIterator* npApplicant,	// ע���Ԫ�أ��������յ�ע���ǿ�ݼ���Ϣ
		IN ULONG nuHotKeyID,	// ���ȶ���õĳ�������������Hotkey�����ܳ�����ͬ��ID����ͼע�����е�Hotkey����ʧ��
		IN ULONG nuVkNumber,	// �������
		IN bool nbControlKey,	// �Ƿ���ҪControl���
		IN bool nbShiftKey,		// �Ƿ���ҪShift���
		IN bool nbAltKey,		// �Ƿ���ҪAlt���
		IN IEinkuiIterator* npFocus = NULL	// ָ�����㷶Χ��������Ԫ�ؼ�����Ԫ�ػ�ü��̽���ʱ���Żᴥ������ע��Ŀ�ݼ�;
		// ʹ��NULL��Ϊ��������ָ�����㷶Χ�������ۼ��̽����ںδ����ܹ��յ�ע��Ŀ�ݼ�����Ϣ��
	);

	// ע����ݼ�
	virtual bool __stdcall UnregisterHotKey(
		IN IEinkuiIterator* npApplicant,	// ע����
		IN ULONG UnuKeyNumber
	);

	// ����XUIԪ�������������룬��Ҫ��UnlockIterators���
	void LockIterators(void);

	// �������XUIԪ����
	void UnlockIterators(void);

	// ����Ϣ��������ã��ͷſ���
	void ReleaseMessage(IEinkuiMessage* npMsg);

	// ����һ��Iterator����Ϣ����
	void StartMessageReceiver(IEinkuiIterator* npIterator);

	// ������̽��㣬�����Ԫ�ؾ���popup���ԣ�Ҳ�������������ʵ��ϲ�
	ERESULT ApplyKeyBoard(IEinkuiIterator* npIterator);

	// �ͷż��̽��㣬�⽫����Tab Order����һ�����̽����߻�ý���
	void ReleaseKeyBoard(PST_RELEASE_KEYFOCUS npRelease);

	LONG GetProbeMode(void) {
		return mlProbeMode;
	}

	// ��ü��̽��㣬��Ҫ�ͷ�
	CXuiIterator* InnerGetKeyFocus(void);

	// �����꽹�㣬��Ҫ�ͷ�
	CXuiIterator* InnerGetMouseFocus(void);

	// ��û�ԾԪ��
	CXuiIterator* GetActiveElement(void);

	// ����Active PopupԪ��
	ERESULT AssignActivation(IEinkuiIterator* npToSet);

	D2D1_POINT_2F GetCurrentMousePosition(void) {
		D2D1_POINT_2F ldPos;
		if (moMouseTrace.Size() != 0)
		{
			ldPos = moMouseTrace.Back().Point;
		}
		else
		{
			ldPos.x = 0.0f;
			ldPos.y = 0.0f;
		}

		return ldPos;
	}

	// ����Ԫ��
	ERESULT DestroyElement(
		IN IEinkuiIterator* npElementIterator	// ��Ԫ�ض�Ӧ�ĵ�����
	);

	// ����������ʵ�Ԫ��
	ERESULT SendCommand(nes_command::ESCOMMAND neCmd);

	// ִ���Ƴ�XUI����
	void EndMessageQueue(void) {
		mbExitXui = true;

		moFastMessages.Clear();
		moNormalMessages.Clear();
	}

	void SetPositionInPanel(
		ULONG x,
		ULONG y
	) {
		mdTopLeftInPanel.x = (FLOAT)x;
		mdTopLeftInPanel.y = (FLOAT)y;
	}

protected:
	// ��Ԫ�ص�����
	CXuiIterator moIteratorRoot;
	CEleMgrProxy moElementRoot;
	CExclusiveAccess moIteratorLock;

	// �������е�������������֤
	TEIteratorVerification moIteratorVerification;

	// ����Ԫ��
	CXuiIterator* mpDesktopIterator;

	// ��Ϣ����
	//CXuiMessageQueue moMessages;
	CXuiMessageQueue moFastMessages;
	CXuiMessageQueue moNormalMessages;
	// 	CXuiMessageQueue moReduceMessages;
	HANDLE mhMessageInserted;
	LONG mlMsgAllocated;
	bool mbExitXui;
	volatile LONG mlCleanHumanInput;

	// ���е���Ϣ
	TElMessageStack moFreeMessagePool;
	CExclusiveAccess moFreeMessageLock;

	// ��������
	CXuiIterator* mpMouseFocus;	// ��ǰ��꽹��
	CXuiIterator* mpActivatedElement;
	CExclusiveAccess moMFocusLock;
	bool mbHoverTest;
	bool mbTopDrawTest;
	TElMouseTrace moMouseTrace;
	CXuiIterator* mpMouseMoveOn;	// �������ƶ�������Ŀ�꣬���������򷵻أ���������һ������꽹��Ļ����
	LONG mlMouseMoveOnLevel;
	D2D1_POINT_2F mdPointRelative;	// ����ֵ��Ӧ�����λ�����������ϵ��λ��
	cmmStack<CElMouseTestState, 32> moPointToTest;
	bool mbTrackMouse;
	D2D1_POINT_2F mdTopLeftInPanel;	// ��Eink Panel�ϵ����Ͻ�

	// Tip
	CXuiIterator* mpTipOwner;
	ULONG muTickOnTipShow;
	bool mbTipHide;

	// ��ק
	bool mbDragging;
	bool mbXuiDragDrop;
	ULONG muKeyWithDragging;	// ͨ���ĸ�������ק
	D2D1_POINT_2F mdDragFrom;	// ��ʼ��קʱ������
	STMS_EDRGDRP_REQUEST mdDrgdrpRequest;
	CXuiIterator* mpDraggingElement;
	CXuiIterator* mpDragMsgReceiver;
	CXuiIterator* mpRecentAskedDrop;
	CXuiIterator* mpLastAccepter;


	// ���̿���
	CXuiIterator* mpKbFocus;	// ��ǰ���̽���
	CExclusiveAccess moKFocusLock;


	// ���ģʽ����
	LONG mlProbeMode;	// �Ƿ������ģʽ
	LONG mlProbePass;	// �������ͨ����������esi'


	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate(void);

	// ����Ϣ������ȡһ����Ϣ�����ҷַ�����
	ERESULT ProcessNextMessage(
		IEinkuiMessage* npMsg = NULL		//��Ϊ�գ���ʾֱ�Ӵ���������Ϣ����������Ϣ���ж�ȡ
	);

	// �ȴ���Ϣ����ڲ�������WaitForSingleObject�ȴ��������ֵͬWaitForSingleObjectһ��
	ULONG WaitMessageReach(ULONG nuMilliseconds);

	// ���ø�Ԫ�ص�Widget���ԣ���System Widget
	void SetRootWidget(IXsWidget* npWidget);

	// �ı���̽���
	void ChangeKeyFocus(CXuiIterator* npNewFocus);

	// �ı���꽹��
	void ChangeMouseFocus(CXuiIterator* npNewFocus);

	// ��npSeekFrom��ʼ���ҿ��Լ����Ԫ�أ������಻Ҫֱ�ӵ���
	void ChangeActiveElement(CXuiIterator* npSeekFrom);

	// ����ģ̬�Ի�״̬
	void EnterModal(void);

	// �����������ת��
	ERESULT OnMsgMouseForward(const PSTELEMGR_WINMSG_FORWARD npMouseInput);

	// �����������ת��
	ERESULT OnMsgKeyboardForward(const PSTELEMGR_WINMSG_FORWARD npKeyStrike);

	// ��������
	ERESULT OnMsgEnalbeInput(void);

	// ����Eink Touch����ת��
	//ERESULT OnMsgEinkTouchForward(const PSTEMS_TOUCH npEinkTouchInput);

	// ��������Ԥ����
	ERESULT __stdcall EnterForMouseTest(IEinkuiIterator* npRecipient);

	// �����������
	ERESULT __stdcall LeaveForMouseTest(IEinkuiIterator* npRecipient);

	// ������갴����Ϣ
	__inline void SendMouseButtonPressed(IEinkuiIterator* npFocus, bool nbPressed, ULONG nuActKey, ULONG nuKeyFlag, ULONG nuTickCount, const D2D1_POINT_2F& rPosition);

	// ������갴��˫����Ϣ
	void SendMouseButtonDbClick(IEinkuiIterator* npFocus, ULONG nuActKey, ULONG nuKeyFlag, ULONG nuTickCount, const D2D1_POINT_2F& rPosition);

	// ����Ƿ�������ק���ҷ��Ϳ�ʼ��ק��Ϣ
	__inline void DetectMouseDragBegin(CXuiIterator* npFocus, ULONG nuActKey, ULONG nuKeyFlag, const D2D1_POINT_2F& rPosition);

	// ��Ŀ��Ԫ�ش����ﵽ����ȫ��������Zorder��߲�
	__inline void BringFocusedPopupToTop(CXuiIterator* npFocus);

	// ����Ƿ���ü��̽��㣬���Ŀ��ǡ���ǵ�ǰ���̽�����ֱ�ӷ��أ������ͷŵ�ǰ���̽���
	__inline void DetectKeyboardFocus(CXuiIterator* npToDetect);

	// ���ͼ�����Ϣ��Ŀ�꣬���Ŀ�귴��ERESULT_UNEXPECTED_KEY�������ϴ��ݲ�֧�ֵļ��̰�����Ϣ����PopupԪ��Ϊֹ
	ERESULT KeyStrike(CXuiIterator* npKeyFocus, const PSTEMS_KEY_PRESSED npStrike);

	//// ��Key��Ϣת��ΪCommand
	//bool KeyToCommand(const PSTEMS_KEY_PRESSED npStrike);

	// ���̽���ת�Ƶ���һ������
	bool ShiftKeyBoardFocus(CXuiIterator* npKeyFocus);

	// ѯ���Ƿ�֧��Drop
	__inline void DropDetect(CXuiIterator* npToDetect);

	// ִ��Drop down
	__inline void TryDropDown(CXuiIterator* npToTry);

	// �ݹ�����ĳ��Ԫ�أ��������ٸ�Ԫ�أ���������Ԫ��
	void DestroyElementSubTree(IEinkuiIterator* npToDestroy);

	// ����Mouse wheel��Ϣ�������ǰ��꽹�㲻����������Ϣ����ô���ж��ڵ����һ��Popup(������һ��popup)֮ǰ�Ƿ���EITR_STYLE_ALL_MWHEEL��Ԫ�أ���Ҫ����Mouse Wheel��Ϣ
	void TransferMouseWheel(CXuiIterator* npMouseFocus, STEMS_MOUSE_WHEEL& rInfor);

	__inline void CalculateMouseMoving(IEinkuiIterator* npOwner, const D2D1_POINT_2F& rCrtPos, const D2D1_POINT_2F& rLastPos, D2D1_POINT_2F& rResult);

	// �ڶ����ϼ���ư���
	bool DetectHotKey(CXuiIterator* npHost, CXuiHotkeyEntry& rToFind);

	// ��ݼ�������false��ʾ���ǿ�ݼ�
	bool HotKeyProcessor(CXuiIterator*npKeyFocus, const PSTELEMGR_WINMSG_FORWARD npKeyStrike);

};

#define EELMGR_WINCAP_MOVING	ES_BIT_SET(1)
#define EELMGR_WINCAP_LB		ES_BIT_SET(2)
#define EELMGR_WINCAP_MB		ES_BIT_SET(3)
#define EELMGR_WINCAP_RB		ES_BIT_SET(4)
#define EELMGR_WINCAP_ALL		0xFFFF























#endif//_ELEMGRIMP_H_