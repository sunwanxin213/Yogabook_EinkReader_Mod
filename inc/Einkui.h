/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */
#ifndef _XUI_H__
#define _XUI_H__
#include "cmmBaseObj.h"
#include "EResult.h"
#include "CfgIface.h"
#include "d2d1.h"
#include "EinkIteAPI.h"

// XUI����ں����������ļ�β����EinkUiStart EinkuiGetSystem


// Ĭ�϶���
#ifndef IN
#define IN
#endif//IN
#ifndef OUT
#define OUT
#endif//OUT
#define ES_ELEMENT_RES_TEXT_MAX_LEN 200		//Ԫ�������ļ�������󳤶�

#define ES_BIT_SET(_Off) (1<<(_Off-1))

// Ĭ����ЧID
#define EID_INVALID 0


//////////////////////////////////////////////////////////////////////////
// ǰ�������������ע
__interface IEinkuiBitmap;	// λͼ���ṩͼ���װ�롢���ʡ��ı��Լ����ƹ��ܣ������ܹ�ֱ���ṩ����Ⱦ����ʹ�ã�
__interface IEinkuiBrush;	// ��ˢ����װ��Brush��Pen�ĸ���
__interface IEinkuiMessage;	// ��Ϣ�����ɡ����á����ʺ��ͷ�
__interface IEinkuiPaintBoard;	// ��ͼ��
__interface IEinkuiSystem;	// XUI�����ӿ���
__interface IXelManager;	// ���������
__interface IXsElement;		// Ԫ��
__interface ILsModule;		// ģ��
__interface ICfKey;			// profile�ļ��������ļ�CfgIface.h
__interface IConfigFile;	// profile�Ľӿ���
__interface IEinkuiIterator;	// Ԫ�ص�����
__interface IXsWidget;		// ΢���ӿڣ�΢����XUIϵͳ�е�һ�������ͬһ���������ֶ���չ���ܲ�����һ�ֽ綨��ʽ��һ����չ���ܲ���������ϵͳ�����ж��ʵ����ÿ��ʵ������һ��΢�����������ݼ����崦
__interface IElementFactory; // ���й����඼�����ṩ���ӿڣ�һ�����������ʵ�������ָ����͵Ķ���
__interface IXelAllocator;
__interface IEinkuiPaintResouce;

__interface IWICImagingFactory;
__interface ID3D10Device1;
__interface IDWriteFactory;

// ǰ����������
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// 

typedef ERESULT (__stdcall IBaseObject::*PXUI_CALLBACK)(ULONG nuFlag,LPVOID npContext);
typedef ERESULT (__stdcall IBaseObject::*PWINMSG_CALLBACK)(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam,LRESULT& Result);
typedef ERESULT(__stdcall *PXUI_CUSTOM_DRAW_CALLBACK)(unsigned char* npPixels, ULONG nuWidth, ULONG nuHeight,bool nbRefresh);//nbRefresh���ã�������ύ����
typedef ERESULT(__stdcall *PXUI_EINKUPDATING_CALLBACK)(ULONGLONG nxNumber,LPVOID npContext);	// nuNumber�Ǹ�����ţ�ÿ�θ��¼�һ���ﵽ���ֵ��ص���


// {EA3270AA-3D8B-4246-AE7F-0D65F3D7BD60}
DEFINE_GUID(EGUID_EMGR_DROP_TEST, 
	0xea3270aa, 0x3d8b, 0x4246, 0xae, 0x7f, 0xd, 0x65, 0xf3, 0xd7, 0xbd, 0x60);
//IsEqualGUID





#pragma pack(4)

//////////////////////////////////////////////////////////////////////////
// ����ĺ������趨��Ϣ����,����TYPE����Ϣ�����߻����ǽ�����(������65535��
// _MJ_NUM����Ϣ�������ܺţ�������127����_MN_NUM����Ϣ���ӹ��ܺ�(511���������ܺ���64����Ĭ�����ڶ���Ԫ���趨����
// ��Ϣ���Ƶ�����Լ���ǣ��ԡ�EMSG_����ͷ���������������������������壬�磺EMSG_ELEMENT_MOUSE_ON��������һ��Element��Element��������Ϣ
//////////////////////////////////////////////////////////////////////////
#define EMSG_DEFINE(_TYPE,_MJ_NUM,_MN_NUM) (((_TYPE&0xFFFF)<<16)|((_MJ_NUM&0x7F)<<9)|(_MN_NUM&0x1FF))
// TYPE ��Ҫ��������XUIϵͳ���õĸ��ֿؼ�������1��1023�����ÿؼ���ʹ�ã���չ�ؼ�ʹ��1024���ϵ�����
#define LMSG_TP_ITERATOR	1	// �����������������Ϣ��Ϊ���⣬�����������ڲ�ʹ��
#define LMSG_TP_ELEMGR	2	// ���������
#define LMSG_TP_SYSTEM	3	// XUIϵͳ
#define LMSG_TP_BITMAP	4	// λͼ������
#define LMSG_TP_RENDER	5	// ��Ⱦ��
#define LMSG_TP_WIN_INPUT	6	// Windowsϵͳֱ���ṩ��������Ϣ

#define LMSG_TP_MOUSE		10	// ���
#define LMSG_TP_KEYBOARD	11	// ����
#define LMSG_TP_TOUCH_SCREEN 12 // ������
#define LMSG_TP_TOUCH_PAD	13 // ������
#define LMSG_TP_INPUT		18	// �߼�������Ϣ��ͨ�����ɳ���������Ϊ�������

#define LMSG_TP_OTHERS	19	// ��ȷ��������

#define LMSG_ELE_BASE	20	// ���ӻ�����ʼ����
#define LMSG_TP_ELEMENT		LMSG_ELE_BASE+1

#define LMSG_TP_BUTTON		LMSG_ELE_BASE+11
#define LMSG_TP_LIST			LMSG_ELE_BASE+12
#define LMSG_TP_COMBO			LMSG_ELE_BASE+13
#define LMSG_TP_MENU			LMSG_ELE_BASE+14
#define LMSG_TP_EDIT			LMSG_ELE_BASE+15
#define LMSG_TP_SCROLLBAR		LMSG_ELE_BASE+16
#define LMSG_TP_PROGRESS		LMSG_ELE_BASE+17
#define LMSG_TP_SLIDERBTN		LMSG_ELE_BASE+18
#define LMSG_TP_ANIMATOR		LMSG_ELE_BASE+19
#define LMSG_TP_PICTUREFRAME	LMSG_ELE_BASE+20
#define LMSG_TP_STATICTEXT	LMSG_ELE_BASE+21
#define LMSG_TP_SLIDEREBAR	LMSG_ELE_BASE+22
#define LMSG_TP_LABEL			LMSG_ELE_BASE+23

#define LMSG_TP_MENUITEMN		LMSG_ELE_BASE+24

#define LMSG_TP_POPUPMENU		LMSG_ELE_BASE+25

#define LMSG_TP_MENUBUTTON	LMSG_ELE_BASE+26

#define LMSG_TP_MENUBAR		LMSG_ELE_BASE+27

#define LMSG_TP_IMAGEBUTTON	LMSG_ELE_BASE+28

#define LMSG_TP_TOOLBAR		LMSG_ELE_BASE+29

#define LMSG_TP_WHIRLANGLE	LMSG_ELE_BASE+30

#define LMSG_TP_SPINBUTTON	LMSG_ELE_BASE+31

#define LMSG_TP_SELECT		LMSG_ELE_BASE+32

#define LMSG_TP_RADIO_BUTTON_GROUP LMSG_ELE_BASE+33

#define LMSG_TP_BI_SLIDERBAR LMSG_ELE_BASE+34

#define LMSG_TP_TIMESPINBUTTON LMSG_ELE_BASE+35



#define LMSG_TP_EXTENDED	1024	// ��չ����Ϣ��𣬿�����չ��ʱ����������ʹ��1024 - 65535֮����������֣�ʵ�ʴ�����չ����Ϣʱ��Ҫ���Ӷ���Ϣ�����ߵ��ж�
									// Ϊ�˱�����ֳ�ͻ��һ���򵥵İ취�ǣ�ʹ��GUIID���ɹ��ߣ��������ɵ�GUID�磺// {09BF0B66-916E-4506-B41C-30D0AB2ECC8D}�����4λ���������չID��CC8D = 52365��ֻ��ȥ��С��1024��ֵ


// ͨ����ϢIDȡ����
#define LMSG_GET_TYPE(_X) ((_X>>16)&0xFFFF)
// ͨ����ϢIDȡMajor Number
#define LMSG_GET_MJNUM(_X) ((_X>>9)&0x7F)
// ͨ����ϢID��ȡMinor number
#define LMSG_GET_MNNUM(_X) (_X&0x1FF)

// ��Ч����ϢID
#define EMSG_MSGID_INVALID MAXUINT32






//////////////////////////////////////////////////////////////////////////
// ���涨�����ϵͳ���ܷ��ͻ���յ���Ϣ
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ϵͳ������Ϣ������ҪӦ�𣬽���������ȡ�����������ERESULT_SUCCESS
#define EMSG_TEST EMSG_DEFINE(LMSG_TP_SYSTEM,1,1)
// input bool
// output bool

// �����߳��쳣����һ�������̷߳����쳣ʱ�����̼߳�ᱻ�رգ����Ҹ��̵߳�ӵ���ߣ�Widget����HomePage���ʹ���Ϣ��ͬʱ������ϢҲ�ᱻ���͸�SystemWidget��HomePage
#define EMSG_WORK_THREAD_EXCEPTION EMSG_DEFINE(LMSG_TP_SYSTEM,2,1)
// input
//struct _STEMS_WORKTHREAD_EXCEPTION{
//	ULONG 
//};
//typedef _STEMS_WORKTHREAD_EXCEPTION STEMS_WORKTHREAD_EXCEPTION,* PSTEMS_WORKTHREAD_EXCEPTION;
// output none
// result na



//////////////////////////////////////////////////////////////////////////
// �ػ���Ϣ��ϵͳ�����Զ��������������ǽ������������
#define EMSG_GRAPHICS_UPDATE EMSG_DEFINE(LMSG_TP_RENDER,1,1)
// input none 
// output none

//////////////////////////////////////////////////////////////////////////
//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
#define EMSG_CREATE EMSG_DEFINE(LMSG_TP_ELEMENT,1,1)
// input IEinkuiIterator* : The interface pointer which assigned to current element
// output none

//////////////////////////////////////////////////////////////////////////
// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��һ���ܵ�����Ϣ�����˳�����Ϣ�Ĵ�����̲������и����Ӷ���Ҳ���յ�Destroy��Ϣ�󣬶���ͻᱻ���٣���Ӧ��Iterator*Ҳ�ᱻ����
#define EMSG_DESTROY EMSG_DEFINE(LMSG_TP_ELEMENT,1,2)
// input none
// output none

//////////////////////////////////////////////////////////////////////////
// ����׼����֪ͨԪ��׼�����ƣ���������ǰ��ǰһ�λ��Ƶ�ʱ�������߼�Ԫ��Ҳ���յ�������Ϣ��
// ����������Ϣʱ�������ܱ���ִ�к�ʱ̫�����Ϊ����������µ�Ԫ�ػ���װ���µ���Դ��
// �����߼�Ԫ�أ����յ�����Ϣ��ʱ�򣬴���������Ԫ�صĸı���Ϊ�����ƶ������ŵȣ����ڿ���Ԫ�أ�ͨ����������
// ��Hide��Ԫ�ز����ܵ�����Ϣ��!!!ע��!!!ÿ��Ԫ��ֻ��Ҫ����׼�����ƣ�ϵͳ�����Զ���������Ԫ�ط���׼��������Ϣ
#define EMSG_PREPARE_PAINT EMSG_DEFINE(LMSG_TP_RENDER,2,1)
// input IEinkuiPaintBoard*
// output none

//////////////////////////////////////////////////////////////////////////
// ���ƣ�֪ͨԪ�ػ�������
// ��Hide��Ԫ�ز����ܵ�����Ϣ��!!!ע��!!!ÿ��Ԫ��ֻ��Ҫ�����������ϵͳ�����Զ���������Ԫ�ط��ͻ�����Ϣ
#define EMSG_PAINT EMSG_DEFINE(LMSG_TP_RENDER,2,2)
// input IEinkuiPaintBoard*
// output none
// result na

//////////////////////////////////////////////////////////////////////////
// ���ƽ�������һ��Ԫ�ص���Ԫ�ض������������Ϣ�����
#define EMSG_POST_PAINT EMSG_DEFINE(LMSG_TP_RENDER,2,3)


//////////////////////////////////////////////////////////////////////////
// ��ǿ��׼����֪ͨ��ǿ��������ǿ�����ƽ׶Σ������ʹ�÷������ο���ǿ����Ƶ��������
// ��ǿ��������Ŀ�������ǿ����������ʱ�������յ�����Ϣ��!!!Ŀǰ��ʱ��֧��3D��ǿ���ص�ʹ�ã�һ���ص���ֻ�����������
#define EMSG_PREPARE_ENHANCER EMSG_DEFINE(LMSG_TP_RENDER,3,1)
// intput 
struct _STEMS_ENHANCER_RENDER{
	IEinkuiPaintBoard* mpPaintBoard;
	IEinkuiIterator* mpDestElement;
};
typedef _STEMS_ENHANCER_RENDER STEMS_ENHANCER_RENDER,* PSTEMS_ENHANCER_RENDER;
// output none
// result ����ERESULT_SKIP_RENDER_CONTENT��������Ŀ�꼰����Ԫ�صĻ���

//////////////////////////////////////////////////////////////////////////
// ��ǿ��Ⱦ��������Ŀ��Ԫ��������Ԫ����ɻ��ƺ�֪ͨ��ǿ����ʼ��Ⱦ��
#define EMSG_RENDER_ENHANCER EMSG_DEFINE(LMSG_TP_RENDER,3,2)
// input STEMS_ENHANCER_RENDER	�������ǿ����EMSG_PREPARE_ENHANCER��Ϣ�������滻��Direct2d��RenderTarget����ʱϵͳ�Ѿ��Զ�����ָ�Ϊδ�ı�֮ǰ��Target
// output none
// result ����ERESULT_REDO_RENDER_CONTENT���ٴλ��ƶ������Ӷ�����Ч��Ҳ���ٴ�ִ�У���������ֵ�޶���

//////////////////////////////////////////////////////////////////////////
// ͼ���豸�����Դ����������Ԫ�ض����յ�������Ϣ������ֱ��ӵ��D2d/D3d��Դ��Ԫ�ش��������Դ�������󣬽���EMSG_PREPARE_ENHANCERʱ����װ��
#define EMSG_DISCARD_DEVICE_RESOURCE EMSG_DEFINE(LMSG_TP_RENDER,3,5)
// input none
// output none


//////////////////////////////////////////////////////////////////////////
// ��ʱ����Ϣ
#define EMSG_TIMER EMSG_DEFINE(LMSG_TP_ELEMENT,2,1)
// input
struct _STEMS_TIMER{
	ULONG TimerID;		// ��ʱ����ID
	ULONG BaseTick;		// �趨��ʱ��ʱ��Tickֵ
	ULONG CurrentTick;	// ��ǰ��TickCountֵ
	ULONG ElapsedTick;	// ������һ����ʱ����Ϣ���ԣ�������Ϣ����һ����Ϣ��ȥ�˶���Tick
	ULONG Kicked;		// ����ʱ���������Ĵ�������1��ʼ����һ�δ�������1
	void* Context;		// �趨��ʱ��ʱָ����������
};
typedef _STEMS_TIMER STEMS_TIMER,* PSTEMS_TIMER;
// output none

//////////////////////////////////////////////////////////////////////////
// ��ˢ����Ϣ��ϵͳÿ���Ӳ���1����ˢ����Ϣ�����͸�ϣ��������ˢ����Ϣ�Ĵ��ڣ�������ˢ��������EITR_STYLE_LAZY_UPDATE
// �뽫���л����仯����Ч���Ķ�ʱ�������������Ϣ�Ĵ��������
#define EMSG_LAZY_UPATE EMSG_DEFINE(LMSG_TP_ELEMENT,2,10)
// input 
struct _STEMG_LAZY_UPDATE{
	ULONG Updated;	// ϵͳ�Ѿ���������LazeUpdate��Ϣ�������ڴ�Χ�ڣ������Ϣ��1��5�Σ���ÿ��֮��ļ����һ����200ms��ֻ�ܱ�֤��ķ�Χ���Ȳ�ƫ��1s
					// ����ʹ����������������ض���Ҫ��ˢ����
	ULONG CurrentTick;	// ��ǰ��Windows TickCountֵ
};
typedef _STEMG_LAZY_UPDATE STEMG_LAZY_UPDATE,* PSTEMG_LAZY_UPDATE;
// output none
// result na

//////////////////////////////////////////////////////////////////////////
// ת����Hook����Ϣ��
#define EMSG_HOOKED_MESSAGE EMSG_DEFINE(LMSG_TP_ELEMENT,2,20)
// input 
struct _STEMS_HOOKED_MSG{
	IEinkuiIterator* OriginalElement;	// ԭʼ����Ϣ����Ԫ��
	IEinkuiMessage* OriginalMessage;	// ���������Ϣ
};
typedef _STEMS_HOOKED_MSG STEMS_HOOKED_MSG,* PSTEMS_HOOKED_MSG;
// output none
// result ����ERESULT_MSG_SENDTO_NEXT����Ϣ�������ݸ���һ��hook����û����һ��hookʱֱ�ӷ��͸�Ŀ�꣬��������κ�����ֵ��������ֹ��Ϣ�ļ������ݣ�ֱ�ӷ��ظ���Ϣ�ķ�����


//////////////////////////////////////////////////////////////////////////
// ������Ϣ
#define EMSG_RESPONSE_MESSAGE EMSG_DEFINE(LMSG_TP_ELEMENT,2,30)
// input 
struct _STEMS_RESPONSE_MSG{
	IEinkuiIterator* OriginalElement;	// ԭʼ��Ϣ�Ľ���Ԫ��
	IEinkuiMessage* OriginalMessage;	// ����������Ϣ
	void* Context;	// ������Ϣ����ʱ��ָ����������
};
typedef _STEMS_RESPONSE_MSG STEMS_RESPONSE_MSG,*PSTEMS_RESPONSE_MSG;
// output none

//////////////////////////////////////////////////////////////////////////
//  ����������Ϣ
//	ԭ��
//		XUIϵͳ��Ҫ��λ��ǰ��괦����һ��Element֮�ڣ��������Ϊ���������⡯��Ϊ�˼�������㣬��Ҫÿһ��Element���XUIϵͳ��ʵʱ��⡣
//	ʵʱ����ʵ�ַ����ǣ�ϵͳ����Element���Ų�ǰ�󣬴�ZOrder��߲���ǰ���Ԫ��ѯ���Ƿ�ӵ�������㣬�����ĳ��Ԫ������ӵ�������㣬����ɼ�⣻
//	ѯ�ʵ�ͨ����Ϣ��ɣ�����ϢЯ����һ�����������ֵ����������������Ԫ�صľֲ�����ϵ�ģ�
//		��һ��Ԫ�ر���Ⱦ��Ч���ҽӣ���ô��������������Ϣǰ�󶼻ᷢ�������Ч��������
//  �ر�ע�⣬������е�ֻ���������⣬ĳ��Ԫ�ط���ERESULT_MOUSE_OWNERSHIP�����������Ѿ����߽�Ҫӵ����ꣻ����Ԫ�ض��ԣ��������������
//	�ж����ڽ��ܵ������룬�Ƴ���Ϣ��
#define EMSG_MOUSE_OWNER_TEST EMSG_DEFINE(LMSG_TP_ELEMGR,1,1)
// input D2D1_POINT_2F
// output none
// result ����ERESULT_MOUSE_OWNERSHIP��ʾ��괦������������������ش�����ֹ������Ԫ�ؽ��м�⣬���������Ӧ�÷���ERESULT_SUCCESS

// ��Ч����׼������⣬
#define EMSG_ENHANCER_PRE_MOUSE_TEST EMSG_DEFINE(LMSG_TP_ELEMGR,1,5)
// input D2D1_POINT_2F
// output D2D1_POINT_2F	��Ч�����¶�λ�ļ��ο�ֵ
// result ����ERESULT_STOP_ENUM_CHILDֹͣѯ����Ԫ�أ�����ERESULT_SUCCESS�����������Ԫ��

// ����ģ̬�Ի�״̬
#define EMSG_MODAL_ENTER EMSG_DEFINE(LMSG_TP_ELEMGR,1,10)
// none
// none
// result  return ERESULT_SUCCESS to enter the modal mode,otherwise refuse to change mode



//////////////////////////////////////////////////////////////////////////
// ���涨�����Ϣ����ͨ�õ�Ԫ��֪ͨ��Ϣ
//////////////////////////////////////////////////////////////////////////

// ��Ծ�ı䣬������EITR_STYLE_POPUP����EITR_STYLE_ACTIVE��Ԫ�أ����յ���Ծ�ı���Ϣ��
// !!!ע��!!!������Ԫ�ػ���˼�����Ϣʱ�����
#define EMSG_ELEMENT_ACTIVATED	EMSG_DEFINE(LMSG_TP_ELEMENT,10,1)
// input
typedef struct _STEMS_ELEMENT_ACTIVATION{
	LONG State;		// ����ֻ��2��״̬�ģ����ʾʧȥ״̬�������ʾ���״̬�����ڶ�״̬��ʹ�ó��ϣ�0��ʾ��״̬���߳�ʼ��״̬������ֵ��ʾ���õ�״̬
	IEinkuiIterator* Activated;	// ��ü����Ԫ�أ��յ�����Ϣ�������Ԫ���п�������Ϊĳ���Ӷ��󱻼��������˲�����ӳ�ľ��Ǽ�������ϲ�Ԫ�أ���Ԫ�أ���������������ڴ�����Ϣ�Ĺ�������Ч����Ҫ���ڳ���
	IEinkuiIterator* InActivated;	// ʧȥ�����Ԫ�أ�������������ڴ�����Ϣ�Ĺ�������Ч����Ҫ���ڳ���
}STEMS_ELEMENT_ACTIVATION,* PSTEMS_ELEMENT_ACTIVATION;
// output none
// result na



#define EMSG_ELEMENT_MOVED EMSG_DEFINE(LMSG_TP_ELEMENT,3,1)		//Ԫ��λ�÷����˸ı�
//Input		D2D1_POINT_2F
//Output	��
//Result	

#define EMSG_ELEMENT_RESIZED EMSG_DEFINE(LMSG_TP_ELEMENT,3,3)			//Ԫ�زο��ߴ緢���ı�
//Input		D2D1_SIZE_F
//Output	none
//Result	

#define EMSG_SHOW_HIDE EMSG_DEFINE(LMSG_TP_ELEMENT,3,5)		//��ʾ������
//Input		bool ,true��ʾ��ʾ��false����
//Output	None
//Result	

#define EMSG_ENALBE_DISABLE EMSG_DEFINE(LMSG_TP_ELEMENT,3,8)	// �����û��߱�����
//input bool, true for it's enable, the otherwise for disable
//output none
//result


#define EMSG_MOUSE_BUTTON EMSG_DEFINE(LMSG_TP_MOUSE,1,1)		//��갴��
//Input		STEMS_MOUSE_BUTTON
struct _STEMS_MOUSE_BUTTON{
	bool  Presssed;			//true��ʾ���£���̧֮��
	ULONG ActKey;		// �ĸ���갴ť�����仯��MK_LBUTTON/MK_RBUTTON/MK_MBUTTON/MK_XBUTTON1/MK_XBUTTON2���κ�һ��ʱ��ֻ��һ����ť�ᷢ���仯
	ULONG KeyFlag;		// ��ʱ��������ذ�ť��״�������ñ�ʾΪ���£����������ǵ�������ϣ�MK_LBUTTON/MK_RBUTTON/MK_MBUTTON/MK_XBUTTON1/MK_XBUTTON2/MK_CONTROL/MK_SHIFT
	ULONG TickCount;	// ��������ʱ��TickCount
	D2D1_POINT_2F Position;		//��������ʱ�����ָ������λ��
};
typedef _STEMS_MOUSE_BUTTON STEMS_MOUSE_BUTTON,*PSTEMS_MOUSE_BUTTON;
//Output	None
//Result

#define EMSG_MOUSE_DBCLICK EMSG_DEFINE(LMSG_TP_MOUSE,1,2)		//���˫��
//Input		STEMS_MOUSE_BUTTON , STEMS_MOUSE_BUTTON::Presssed is unused
//Output	None
//Result


#define EMSG_MOUSE_MOVING EMSG_DEFINE(LMSG_TP_MOUSE,1,3)		//����ƶ�
//Input		STEMS_MOUSE_MOVING
struct _STEMS_MOUSE_MOVING{
	ULONG KeyFlag;		// ��ʱ��������ذ�ť��״�������ñ�ʾΪ���£����������ǵ�������ϣ�MK_LBUTTON/MK_RBUTTON/MK_MBUTTON/MK_XBUTTON1/MK_XBUTTON2/MK_CONTROL/MK_SHIFT
	ULONG TickCount;	// ��ǰTickCount
	D2D1_POINT_2F Position;		//���ָ�뵱ǰλ��
};
typedef _STEMS_MOUSE_MOVING STEMS_MOUSE_MOVING,*PSTEMS_MOUSE_MOVING;
//Output	None
//Result

#define EMSG_MOUSE_WHEEL EMSG_DEFINE(LMSG_TP_MOUSE,1,4)		//������ת��
//intput
struct _STEMS_MOUSE_WHEEL{
	short Delta;
	ULONG KeyFlag;		// ��ʱ��������ذ�ť��״�������ñ�ʾΪ���£����������ǵ�������ϣ�MK_LBUTTON/MK_RBUTTON/MK_MBUTTON/MK_XBUTTON1/MK_XBUTTON2/MK_CONTROL/MK_SHIFT
	ULONG TickCount;	// ��ǰTickCount
	IEinkuiIterator* MouseFocus;	// ��ǰ����꽹��
	//D2D1_POINT_2F Position;		//���ָ�뵱ǰλ��
};
typedef _STEMS_MOUSE_WHEEL STEMS_MOUSE_WHEEL,* PSTEMS_MOUSE_WHEEL;
//output none
// result �����������Ϣ����ط���ERESULT_UNEXPECTED_MESSAGE


#define EMSG_MOUSE_FOCUS EMSG_DEFINE(LMSG_TP_MOUSE,1,8)		//��꽹��仯
//Input	STEMS_STATE_CHANGE
struct _STEMS_STATE_CHANGE{
	LONG State;		// ����ֻ��2��״̬�ģ����ʾʧȥ״̬�������ʾ���״̬�����ڶ�״̬��ʹ�ó��ϣ�0��ʾ��״̬���߳�ʼ��״̬������ֵ��ʾ���õ�״̬
	IEinkuiIterator* Related;	// ������Ԫ�أ�����Ӵ�Ԫ�ػ�ã����߱���Ԫ�ض�ȡ������û�й���Ԫ�أ���ʱΪNULL��������������ڴ�����Ϣ�Ĺ�������Ч����Ҫ���ڳ���
};
typedef _STEMS_STATE_CHANGE STEMS_STATE_CHANGE,* PSTEMS_STATE_CHANGE;
//Output None
//Result

#define EMSG_MOUSE_HOVER EMSG_DEFINE(LMSG_TP_MOUSE,1,20)		//�����ͣ
//Input		none
//Output	None
//Result


#define EMSG_KEYBOARD_FOCUS EMSG_DEFINE(LMSG_TP_KEYBOARD,1,1)		//���̽���仯
//Input	STEMS_STATE_CHANGE
//Output None
//Result

#define EMSG_TOUCH  EMSG_DEFINE(LMSG_TP_TOUCH_SCREEN,1 ,1)	// ��������Ϣ
// input EI_TOUCHINPUT_POINT
//#define EI_TOUCHEVENTF_DOWN 0 refer to file EinkIteApi.h
//#define EI_TOUCHEVENTF_MOVE 1
//#define EI_TOUCHEVENTF_UP 2
//typedef struct _EI_TOUCHINPUT_POINT {
//	unsigned long x;
//	unsigned long y;
//	unsigned long z;		//pressure
//	unsigned long Flags;	//0:down  1:move  2:up
//}EI_TOUCHINPUT_POINT, *PEI_TOUCHINPUT_POINT;

#define EMSG_KEYBOARD_SCREEN_KEYBOARD EMSG_DEFINE(LMSG_TP_KEYBOARD,1,2)		//��������Ļ����
//Input	None
//Output None
//Result


#define EMSG_KEY_PRESSED EMSG_DEFINE(LMSG_TP_KEYBOARD,1,8)		//���̰���
//Input		STEMS_KEY_PRESSED
struct _STEMS_KEY_PRESSED{
	bool IsPressDown;			//true��ʾ���£���̧֮��
	ULONG VirtualKeyCode;		//ͬWidows��WM_KEYDOWN��Ϣ����wParam
	ULONG ExtendedKeyFlag;		//ͬWidows��WM_KEYDOWN��Ϣ����lParam
};
typedef _STEMS_KEY_PRESSED STEMS_KEY_PRESSED,*PSTEMS_KEY_PRESSED;
//Output	None
//Result

//////////////////////////////////////////////////////////////////////////
//�ַ�����
#define EMSG_CHAR_INPUT EMSG_DEFINE(LMSG_TP_KEYBOARD,1,12)
// input 
struct _STEMS_CHAR_INPUT{
	wchar_t CharIn;		// ������ַ�
	ULONG Flags;		// ͬWM_CHAR��Ϣ��lParamһ��
};
typedef _STEMS_CHAR_INPUT STEMS_CHAR_INPUT,* PSTEMS_CHAR_INPUT;
// output none
// result na


// ��ݼ�������
#define EMSG_HOTKEY_PRESSED EMSG_DEFINE(LMSG_TP_KEYBOARD,10,1)
// input 
struct _STEMS_HOTKEY{
	IEinkuiIterator* Focus;	//��ǰ���̽��㣬����һ���ǽ��ܵ�ǰ��Ϣ��Ԫ��
	ULONG HotKeyID;		//ע��ʱ�ṩ��ID
	ULONG VkCode;	// ��ݼ�����ͨ����
	bool Control;	// Control���Ƿ���
	bool Shift;		// Shift���Ƿ���
	bool Alt;		// Alt���Ƿ���
};
typedef _STEMS_HOTKEY STEMS_HOTKEY,* PSTEMS_HOTKEY;
// output none
// result na


// Ԫ����ק�������߿���ѡ���ƶ�Ԫ�ػ������н����ק������
#define EMSG_DRAGGING_ELEMENT EMSG_DEFINE(LMSG_TP_INPUT,1,1)
// intput
struct _STMS_DRAGGING_ELE{
	IEinkuiIterator* DragOn;	// ��ק���õ�Ԫ��
	ULONG ActKey;		// �ĸ���갴ť���½�����ק��MK_LBUTTON/MK_RBUTTON/MK_MBUTTON���κ�һ��ʱ��ֻ��һ����ť�ἤ����ק
	ULONG KeyFlag;		// ��ʱ��������ذ�ť��״�������ñ�ʾΪ���£����������ǵ�������ϣ�MK_LBUTTON/MK_RBUTTON/MK_MBUTTON/MK_XBUTTON1/MK_XBUTTON2/MK_CONTROL/MK_SHIFT
	D2D1_POINT_2F Offset;		// ��ǰ���λ���뿪ʼ��ק��λ�õ�ƫ��ֲ�����ϵ
	D2D1_POINT_2F CurrentPos;	// ��ǰ���λ�û�����ָλ�ã��ֲ�����ϵ
};
typedef _STMS_DRAGGING_ELE STMS_DRAGGING_ELE,* PSTMS_DRAGGING_ELE;
// output none

// ��ק��ʼ
#define EMSG_DRAG_BEGIN EMSG_DEFINE(LMSG_TP_INPUT,1,2)
// input STMS_DRAGGING_ELE
// output none or ��������Dragdrop����Ĳ�ͬ����
struct _STMS_EDRGDRP_REQUEST{	// Xui drag&drop�Ĳ���
	IEinkuiIterator* Host;	// DragDrop�ķ����ߣ��������ã�ϵͳ���Զ������趨Ϊ����Ϣ�ķ�����
	ULONG KeyFlag;		// ��ʱ��������ذ�ť��״�������ñ�ʾΪ���£����������ǵ�������ϣ�MK_LBUTTON/MK_RBUTTON/MK_MBUTTON/MK_XBUTTON1/MK_XBUTTON2/MK_CONTROL/MK_SHIFT
	D2D1_POINT_2F CurrentPos;	// ��ǰ�����㣬���ܴ���Ϣ����ľֲ�����ϵ
	GUID Reason;		// Ŀ��
	ULONG Flags;		// ����Я��4�ֽڵı�־�������������жϣ����ϣ����ø���ϸ����Ϣ����Ҫ�����������߷���ѯ����Ϣ
	UCHAR Attachment[128];	// 128���ֽڵĲ����������������ʹ����Ϣ�໥����
};
typedef _STMS_EDRGDRP_REQUEST STMS_EDRGDRP_REQUEST,* PSTMS_EDRGDRP_REQUEST;
// result ��ʾ�Ƿ�����Drag&Drap����������ERESULT_EDRAGDROP_BEGIN����XUI Drag&drop���̣�����ERESULT_WDRAGDROP_BEGIN����Windows Drag&Drop���̣�����Windows Drag&Drop�Ĺ��ܸ���Xui Drag&Drop
//		  ����RESULT_SUCCESS��ִ����ͨ����ק��Ϊ�����ش���ֵ�򲻽�����ק����


// ��ק����
#define EMSG_DRAG_END EMSG_DEFINE(LMSG_TP_INPUT,1,3)
// inputSTMS_DRAGGING_ELE
// output none
// result na


// Dropѯ�ʣ�Ԫ��������Style EITR_STYLE_XUIDRAGDROP�Ż��յ�
#define EMSG_XUI_DROP_TEST EMSG_DEFINE(LMSG_TP_INPUT,2,1)
// input STMS_EDRGDRP_REQUEST
// output none
// result ERESULT_EDRAGDROP_ACCEPT��ʾ����Drop�����ݣ���������ֵ��ʾ������

// Dropѯ�ʽ��룬��Dragging point����ĳ��Elementʱ�����͸�Ŀ�꣬Ŀ��������EITR_STYLE_XUIDRAGDROP
#define EMSG_DRAGDROP_ENTER EMSG_DEFINE(LMSG_TP_INPUT,2,5)
// input none
// output none
// na

// Dropѯ�ʽ�������Dragging point�뿪��ǰElementʱ�����͸�Ŀ�꣬Ŀ��������EITR_STYLE_XUIDRAGDROP
#define EMSG_DRAGDROP_LEAVE EMSG_DEFINE(LMSG_TP_INPUT,2,6)
// input none
// output none
// na

// Drag off�����͸�XUI Drag&Drog�ķ����ߣ�������Drag&Drop׼�����
#define EMSG_XUI_DRAG_OFF EMSG_DEFINE(LMSG_TP_INPUT,2,2)
// input IEinkuiIterator* the accepter
// output optional STMS_EDRGDRP_REQUEST
// result ERESULT_SUCCESS to confirm the DROP,others to deny the DROP

// Drop down��to inform the accepter to complete the DROP
#define EMSG_XUI_DROP_DOWN EMSG_DEFINE(LMSG_TP_INPUT,2,3)
// input STMS_EDRGDRP_REQUEST
// output none
// result na

////////////////////////////////////////////////////////////////////////////
//// ����
//// ϵͳֻ�������EITR_STYLE_PAGE�Ĵ��ڷ��ͱ���Ϣ
#define EMSG_COMMAND EMSG_DEFINE(LMSG_TP_INPUT,4,1)
// input nes_command::ESCOMMAND
namespace nes_command{
	enum ESCOMMAND{
		eInvalid=0,
		eCopy=1,
		eCut=2,
		ePaste=3,
		eDelete=4,
		eSelAll=5,
		eEnter=6,
		eEsc=7,
		eAltF4=8
	};
}
// output none
// result ERESULT_SUCCESS ��ʾ����������Ϣ��������Ϣ����ת������һ���Ľ���command��Ԫ��


//////////////////////////////////////////////////////////////////////////
// �������Ϣ������System Widget��HomePage ���յ�

// ��ʾXUIϵͳ��MessageBox
#define EMSG_SWGT_MESSAGE_BOX EMSG_DEFINE(LMSG_TP_SYSTEM,10,1)
// input STEMS_MESSAGE_BOX
namespace nse_msgbox{

	enum EBUTTON{
		eNoneBt=0,
		eOk,
		eCancel,
		eYes,
		eNo,
		eAbort,
		eRetry,
		eIgnore,
		eContinue
	};

	enum EICON {
		eNoneIc=0,
		eQuestionIc,
		eCheckIc,
		eWarningIc,
		eErrorIc
	};

};
struct _STEMS_MESSAGE_BOX{
	nse_msgbox::EBUTTON Buttons[3];	// ���� �Ի����ϵ�������ť�����ݣ����ֻ��Ҫһ����ť��������������Ԫ����ΪeNoneBt
	nse_msgbox::EICON  Icon;
	const wchar_t* Title;
	const wchar_t* Text;
};
typedef _STEMS_MESSAGE_BOX STEMS_MESSAGE_BOX,* PSTEMS_MESSAGE_BOX;
// output none
// result the answer from user or error code if anything is wrong

// ��ʾTip
#define EMSG_SWGT_SHOW_TIP EMSG_DEFINE(LMSG_TP_SYSTEM,11,1)
// input
struct _STEMS_SWGT_SHOWTIP{
	const wchar_t* Text;	// to show as tooltip
	D2D1_POINT_2F Position;
};
typedef _STEMS_SWGT_SHOWTIP STEMS_SWGT_SHOWTIP,* PSTEMS_SWGT_SHOWTIP;
// output none
// result na

// ����Tip
#define EMSG_SWGT_HIDE_TIP EMSG_DEFINE(LMSG_TP_SYSTEM,11,2)
// input none
// output none
// result na

// ���ڳߴ�ı�֪ͨ
#define EMSG_SWGT_MW_RESIZED EMSG_DEFINE(LMSG_TP_SYSTEM,11,10)
// input ULONG[2], first is width followed height
// output none
// result na

// ϵͳ��Ļ������ת��Ӧ��ϵͳ�Ƿ�ת������
#define EMSG_QUERY_SWGT_ROTATED EMSG_DEFINE(LMSG_TP_SYSTEM,11,14)
// input ULONG as screen-orientation
// output ULONG orientation to set. MAXULONG32 indicate not rotate orientation

// ���巢����ת
#define EMSG_SWGT_ROTATED EMSG_DEFINE(LMSG_TP_SYSTEM,11,15)
// input ULONG: new Orientation of EINK SDK
// output none
// result na

// ��ˢ���Ͷ���
enum XuiBrushType{
	XuiSolidBrush,
	XuiLinearGradientBrush,
	XuiRadialGradientBrush,
	XuiBitmapBrush,
	XuiStroke,
};

//ͨ���ַ���ʵ����һ��IEinkuiBitmap����ʱ��Ҫ�Ľṹ�嶨�壬����IXelAllocator�ӿ��ṩ��CreateImageByText��������
typedef struct _STETXT_BMP_INIT
{
	enum eSIZELIMIT{
		EL_FREESIZE,
		EL_FIXEDWIDTH,
		EL_FIXEDSIZE
	};

	enum eTEXTALIGN{
		EL_TEXTALIGN_LEADING,
		EL_TEXTALIGN_TRAILING,
		EL_TEXTALIGN_CENTER
	} ;

	enum ePARAALIGN{
		EL_PARAALIGN_NEAR,
		EL_PARAALIGN_FAR,
		EL_PARAALIGN_CENTER
	} ;

	enum eFONTWEIGHT {
		EL_FW_NORMAL,
		EL_FW_BOLD
	};

	DWORD StructSize;			//�ṹ���С
	const wchar_t* Text;		//Ҫ����ͼƬ���ַ���
	const wchar_t* FontName;	//��������
	DWORD FontSize;				//�����С
	DWORD TextColor;			//������ɫ
	DWORD Width;				//ָ�����
	DWORD Height;				//ָ���߶�
	eSIZELIMIT Limit;
	eTEXTALIGN Talign;
	ePARAALIGN Palign;
	eFONTWEIGHT FontWeight;		//����weight��Ŀǰֻ֧����ͨ�ʹ���
}STETXT_BMP_INIT,*PSTETXT_BMP_INIT;

// �趨�Ի�Ĳ���
typedef struct _EINKUI_CUSTOMDRAW_SETTINGS
{
	ULONG Width;	// ��Panel����ʾʱ�Ŀ��
	ULONG Height;
	PXUI_CUSTOM_DRAW_CALLBACK CustomDrawFun;	// �Ի溯��
}EINKUI_CUSTOMDRAW_SETTINGS,* PEINKUI_CUSTOMDRAW_SETTINGS;


#pragma pack()
// �ṹ�嶨�����
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// XUI���ӿڶ���
__interface IEinkuiSystem:public IBaseObject
{
	// ��õ�ǰ�Ľ������������ַ������磺��������'CN'
	virtual const wchar_t* __stdcall GetCurrentLanguage(void)=NULL;

	// ���ϵͳ��DPI����
	virtual FLOAT __stdcall GetDpiX(void)=NULL;
	virtual FLOAT __stdcall GetDpiY(void)=NULL;

	// ���û��壬��������װ����ɺ󣬵�������������û��壻�ڴ�֮ǰ���岢������Ƴ�ʵ��ͼ���Ա�������������ʱ��Eink��Ļ�������
	virtual ERESULT __stdcall EnablePaintboard(void) = NULL;

	// ���û��壬����Ļ��ת����ʱ�����ñ�������ʹϵͳ���û���
	virtual void __stdcall ResetPaintboard(void) = NULL;

	// ��û���Ĵ�С
	virtual void __stdcall GetPaintboardSize(
		OUT EI_SIZE* npSize	// ��ȡ�����С
	) = NULL;

	// ���ñ�������Eink Panel�ϵ���ʾλ�ã�ֻ�����Ի����
	virtual void __stdcall SetPositionInPanel(
		ULONG nuX,
		ULONG nuY
		) = NULL;

	// ��ȡEinkϵͳ����
	virtual PEI_SYSTEM_INFO __stdcall GetEinkSystemInfo(void) = NULL;

	// ���Windows����
	virtual HWND __stdcall GetMainWindow(void)=NULL;

	// ��ʾ��������WindowsͨѶ����
	virtual void __stdcall ShowMainWindow(bool nbShown=true)=NULL;

	// ����Ԫ�ع���
	virtual IXelManager* __stdcall  GetElementManager(void)=NULL;	// ���Բ���ʧ�ܣ������жϷ���ֵ�Ƿ�ΪNULL

	// ����һ���µ�Widget
	virtual ERESULT __stdcall LaunchWidget(
		IN const wchar_t* nswModulePathName,	// ��Widget��ģ���ļ���·��������ʵ�ִ�Widget��DLL����
		IN const wchar_t* nswHomeTempleteName,	// ��Widget��HomePage��Templete Key�����֣����Key������ProFile ��Root��
		IN const wchar_t* nswInstanceName,	// �������е�ʵ������ʵ����������ͬ�����������ͬ��ʵ������ϵͳ���᷵��ʧ��
		IN IEinkuiIterator* npFrame,	// ���ڶ�λ����ʾ��װ��Widget��Frame Element
		IN ICfKey* npInstConfig,	// Widgetʵ��ר������
		OUT IXsWidget** nppWidget	// ���Բ���д�����ڷ����½�����Widget�ӿڣ����صĽӿ���Ҫ�ͷ�
		)=NULL;

	// ��õ�ǰ΢���ӿڣ������ͷ�;���ĳ���߳�û��ʹ��CreateWidgetWorkThread��������ô���������߳��е���GetCurrentWidget�������쳣����
	virtual IXsWidget* __stdcall GetCurrentWidget(void)=NULL;	// ���Բ���ʧ�ܣ������жϷ���ֵ�Ƿ�ΪNULL

	// ��õ�ǰϵͳ�е�ĳ��Widget�ӿڣ��������NULL��ʾ�˱��֮��û��Widget��
	// �˺���ֻ�ܱ�System Widget����
	virtual IXsWidget* __stdcall ObtainWidget(
		IN int niNumber		// ��0��ʼ��ŵ�Widegt�����û�����壬ֻ��Widget�Ĵ洢λ�ã�0��λ�ô�ŵ���System Widget
		)=NULL;

	// ���ϵͳWidget
	virtual IXsWidget* __stdcall GetSystemWidget(void)=NULL;

	// ����һ�����̣߳����е�Widget��Ӧ��ʹ��������������Լ��Ķ����̣߳�����ֵ��Windows�߳̾�����������ڵ���SuspendThread/ResumeThread/GetExitCodeThread
	// ���գ����صľ���������CloseHandle�رգ��������������ͬWindows API CreateThreadһ��
	virtual HANDLE __stdcall CreateWidgetWorkThread(
		LPSECURITY_ATTRIBUTES lpThreadAttributes,
		SIZE_T dwStackSize,
		LPTHREAD_START_ROUTINE lpStartAddress,
		LPVOID lpParameter,
		DWORD dwCreationFlags,
		LPDWORD lpThreadId
		)=NULL;

	// �˳�Widget�����̣߳����������CreateWidgetWorkThread���߳��������˳�ʱ��ϵͳ���Զ�������ֹ�̲߳���
	virtual void __stdcall ExitWidgetWorkThread(DWORD dwExitCode)=NULL;

	// ��÷������������������ڽ��������򷢲���Element���󣬷��صĶ��������ͷ�
	virtual IXelAllocator* __stdcall GetAllocator(void)=NULL;

	// ��һ��Config�ļ������ڴ�һ��config�ļ���ĿǰӦ����Factory�ӿ�ʵ���У����ڴ�һ��Conponent��Ӧ��Profile
	// ���صľ����Ҫ�ͷţ������д���ݷ�ʽ�򿪣������save�����������ݵ��ļ�
	virtual IConfigFile* __stdcall OpenConfigFile(
		IN const wchar_t* nszPathName,				// �ļ�������·����
		IN ULONG nuCreationDisposition=CF_OPEN_EXISTING	// ͬCreateFile API���ƣ���������CF_CREATE_ALWAYS��CF_CREATE_NEW��CF_OPEN_ALWAYS��CF_OPEN_EXISTING�������CfgIface.h
		)=NULL;

	// �������»����Ը���������ͼ
	// �ظ����ñ����������ᵼ���ظ��Ļ��ƣ�
	virtual void __stdcall UpdateView(
		IN bool nbRefresh = false	// �����ύȫ��
		)=NULL;

	// ����Eink���ƻص���ÿ��UIϵͳ��Eink�����ύʱ����ָ���Ļص�����
	virtual ERESULT __stdcall SetEinkUpdatingCallBack(
		IN PXUI_EINKUPDATING_CALLBACK npFunction,
		IN PVOID npContext
	) = NULL;

	// ����Eink���棻����Ĺ���ԭ����ͨ��Eink�����е������������ȶԴ���ʾ���ݣ�ֻ����ͬ�Ĳ��ַ���Eink�����Eink��Ļ��Ҫȫ���ػ�
	//    ����Ҫ����Eink���棬ʹ��ȫ�����ݻ��Ƶ�Eink����Ҫ����App���»Eink��Ļ�����
	virtual void __stdcall ClearEinkBuffer(void) = NULL;

	// ��ĳ��Element��������children����
	virtual IEinkuiBitmap* __stdcall TakeSnapshot(
		IEinkuiIterator* npToShot,
		const D2D1_RECT_F& crSourceRegion,	// ��������Ŀ��Ԫ�صľֲ�����ϵ
		const D2D_SIZE_F& crBriefSize,		// ����ͼ�ߴ磬���յĽ����һ������ͼ
		const FLOAT* ColorRGBA=NULL
		)=NULL;

	// ���Direct2D�Ĺ����ӿڣ������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
	// ͨ������CXuiGraphy���ͬ������ʵ�֣�ͬ����ȫ��ͬ
	virtual ID2D1Factory* __stdcall GetD2dFactory(void)=NULL;

	// ���WIC�����������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
	// ͨ������CXuiGraphy���ͬ������ʵ�֣�ͬ����ȫ��ͬ
	virtual IWICImagingFactory* __stdcall GetWICFactory(void)=NULL;

	// ���Direct Write�����������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
	// ͨ������CXuiGraphy���ͬ������ʵ�֣�ͬ����ȫ��ͬ
	virtual IDWriteFactory* __stdcall GetDWriteFactory(void)=NULL;

	// ����ʹ��Windows�����̻߳ص������Widget��������Ҫ����Windows�Ľ���ģ�飬��ʾ�����Windows�����������Windows������صı�����Windows�����߳���ִ�еĹ��ܣ�
	// ���ù��ܴ�����ֲ���Լ�Element�����������IBaseObject�����һ�����������У�ͨ�����ñ�����������ʹ��Windows�����߳������ص��趨�Ķ������������ú����ķ���ֵҲ��
	// ͨ��������ֱ�ӷ��ظ��������ĵ����ߡ�����������ܵĹ����У������Ĵ��뽫�����ڴ˷����У�����ȷ��������������ˢ�У�����Xui����Ľ��潫������Ӧ��
	// !!!ע��!!!�����ò��������룬ͬһʱ�̽�����һ�����ô˷������������ͬʱ���ã���һ�ε��û᷵��ERESULT_ACCESS_CONFLICT�����ԣ�����ñ�������ʹ���ߣ���ؼ�鷵��ֵ��
	// �������ERESULT_ACCESS_CONFLICT����������ã���Ҫ�������³��ԡ�����������������ܷ�����
	virtual ERESULT __stdcall CallBackByWinUiThread(
		IBaseObject* npApplicant,		// �ص����󣬲���һ��Ҫ���ǵ��ñ������Ķ�����Ҳ�����Ǳ�Ķ���ָ��
		PXUI_CALLBACK npProcedureToCall,	//�ص����̣����̵ķ���ֵ���᷵�ظ�������
		ULONG nuFlag,			// ���ݸ��ص������Ĳ���
		LPVOID npConText,		// ���ݸ��ص������Ĳ���
		bool nbStall=false		// �ȴ������ڼ��Ƿ���Ҫ���������XUI��Ϣ�����nbStal��Ϊture����ֱ�ӵȴ����أ���������XUI��Ϣѭ��
								// ʹ�ô˷������Ա�����ñ������ڼ䣬�ٴ��������
		)=NULL;

	// ע��Windows��Ϣ���أ�ͨ�������ܿ�����XUIϵͳ����Windows��Ϣ֮ǰ��ȡ��ע��Windows��Ϣ
	// �����ȡ��Windows��Ϣ�Ĺ���Ҫ�����ܶ���
	// ����ERESULT_WINMSG_SENDTO_NEXT��XUI����Ϣ���ݸ���һ�������ߣ����߽���XUIϵͳ���ʹ�����������ֵ����ֹ��Windows��Ϣ�Ĵ��ݹ����Լ�XUI�Ը���Ϣ�Ĵ���
	virtual ERESULT __stdcall CaptureWindowsMessage(
		IN UINT nuiWinMsgID,	// ϣ�������Windows��Ϣ��ID
		IN IXsElement* npApplicant,	//����ص��Ķ���
		IN PWINMSG_CALLBACK npProcedure	//��Windows��Ϣ���͸��˺���
		)=NULL;

	// ����һ������˷���ͨ��ΪSystemWidget�Ĳ˵�ģ����ã�����ģ��һ���û�������ϼ���Ϊ��������ᱻ��������ǰ�ļ��̽������ϵľ���EITR_STYLE_COMMAND�Ķ���
	virtual ERESULT __stdcall GenerateCommand(
		nes_command::ESCOMMAND neCmd
		)=NULL;

	// ����ĳҳ��Ϊģ̬�Ի���ʽ�����û�������ɸöԻ�����ʱ�����ϵ��������ֶ��޷�ʹ�á�
	// XUI��ģ̬�Ի���ʽ��ȫ�ֵģ�����ģ̬�Ի��£����е�Widget������System Widget)��������Ӧ�û����룻���У������ܱ���ʹ��ģʽ�Ի����������Ǳ���ġ�
	// ʹ�÷����ǣ����ȴ���ģʽ�Ի���������Ĭ������)��Ȼ����ñ���������ģ̬�Ի���ʽ����ʱ��ģ̬�Ի�Ԫ�ض��󽫻��յ�һ��EMSG_MODAL_ENTER��Ϣ�����������������Լ���ʾ������
	// ����ģ̬�Ի������ж��û�����˶Ի��������Լ�������ExitModal�˳�ģ̬�Ի���ʽ
	// ע�⣬ģ̬�Ի���ʽ�ǿ����ص�����ģ���ģ̬�Ի��£����Դ���ģ̬�Ի�������һ����˳�
	virtual ERESULT __stdcall DoModal(
		IEinkuiIterator* npModalElement		// ��Ԫ����ģ̬�Ի���������
		)=NULL;


	// �˳�ģ̬�Ի���ʽ
	virtual void __stdcall ExitModal(
		IEinkuiIterator* npModalElement,	// ��Ԫ����ģ̬�Ի���������
		ERESULT nuResult
		)=NULL;


	// �˳�����XUIϵͳ
	virtual void __stdcall ExitXui(void)=NULL;

	// �����õ�һ�����Խӿڣ���Ҫ����
	virtual void __stdcall SystemTest(ULONG nuValue)=NULL;

	//���β����Ƿ���TOUCH����
	virtual bool __stdcall IsTouch(void) = NULL;

public:
	// �������仭ˢ
	virtual IEinkuiBrush* __stdcall CreateBrush(
		XuiBrushType niBrushType,
		D2D1_COLOR_F noColor
		) = NULL;

	// ���仭ˢʱ����Ҫ��������ɫ��
	virtual IEinkuiBrush* __stdcall CreateBrush(
		XuiBrushType niBrushType, 
		D2D1_GRADIENT_STOP* npGradientStop, ULONG nuCount, 
		D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES ndLinearGradientBrushProperties
		) = NULL;

};

//////////////////////////////////////////////////////////////////////////
// Ԫ�ع�����
//#define EMSG_POST_NORMAL	0
//#define EMSG_POST_FAST		2 // ��ʾ��Ϣ����Fast���ԣ�������ϢӦ�ð��Ⱥ���������Ϣ���е�ǰ�����ȴ�������ͬĿ�꣬��ͬ���͵���Ϣ��ֻ����ں󷢵�һ��
//#define EMSG_POST_REVERSE	3 // ����Ϣ����ͬʱ����������Ϣ��ʱ�򣬽��ᱻ����ȳ��ķ�ʽ��������Ϣ�����У����ǵ����ȼ�������ͨ��Ϣ��������Fast��Ϣ
//#define EMSG_POST_REDUCE	4 // ����Ϣ�����г��ַ���ͬһ������ĵ�������Ϣʱ��ȥ�����͸�ͳһ����ĵڶ�����Ϣ
// �����4��Post��Ϣ��ֻ��������һ
#define EMSG_POSTTYPE_FAST	2			// ������Ϣ���������˹�������Ϊ����ͨ��;��Ҫʹ��
#define EMSG_POSTTYPE_REVERSE 3			// ������Ϣ�����ᱻ����ȳ��ķ�ʽ��������ͨ��Ϣ�����У�ͨ��������Ҫ�����������ִ�е���;
#define EMSG_POSTTYPE_NORMAL 4			// ��ͨ��Ϣ�������Ƚ��ȳ��ķ�ʽ��ֹ����ͨ��Ϣ�����У�������ͨ��;
#define EMSG_POSTTYPE_REDUCE 5			// ��Լ��Ϣ���������������ٶ����У����ң��������д���2��������Ϣʱ����ɾ�������м����Ϣ
#define EMSG_POSTTYPE_QUIT	6			// ϵͳʹ��

__interface IXelManager:public IBaseObject
{
	// ��ϵͳ������ע��һ��Element�����ص���������ʧ�ܷ���NULL
	// �ɹ����صĽӿڶ���
	virtual IEinkuiIterator* __stdcall  RegisterElement(
		IN IEinkuiIterator* npParent,	// ��Ԫ�صĵ�����
		IN IXsElement* npElement,	// ��ע�����Ԫ��
		IN ULONG nuEID=0	// ��Ԫ�ص�EID����ͬһ����Ԫ���£�����Ԫ�ص�EID����Ψһ�����������EID��������=0��ϵͳ�Զ�����
		)=NULL;

	// ��ϵͳ������ע��һ��Element���˹��ܽ�Ӧ�ñ���ע��Element�������XUIϵͳ���ã���������Ѿ�����
	virtual ERESULT __stdcall UnregisterElement(
		IN IEinkuiIterator* npElementIterator	// ��Ԫ�ض�Ӧ�ĵ�����
		)=NULL;

	// ��֤һ��Iterator�Ƿ���Ч������ERESULT_SUCCESS��Ч������ERESULT_ITERATOR_INVALID��Ч
	virtual ERESULT __stdcall VerifyIterator(
		IN IEinkuiIterator* npIterator	// ������
		)=NULL;

	// �ڶ���������в���һ��Element�����ظ�Element�ĵ���������ʧ�ܷ���NULL
	// �ɹ����صĽӿڶ��� ���������Ҫͨ��Ԫ�ػ������ע����������뱣���������ָ�룬��Ϊ���ñ�����ʹ��ȫ���������һ�ȡ���������󣬺�ʱ�ϴ�
	virtual IEinkuiIterator* __stdcall FindElement(
		IN IXsElement* npElement
		)=NULL;

	// ��ø�Ԫ�أ������Ϊ�˸����������������Ϣ��Ҳ����ֱ��ʹ��NULLָ���ʾ���������
	// �ɹ����صĽӿڶ���
	virtual IEinkuiIterator* __stdcall GetRootElement(void)=NULL;

	// ��ò˵�ҳ�����еĲ˵������������ҳ
	virtual IEinkuiIterator* __stdcall GetMenuPage(void)=NULL;

	// ���ToolTipƽ�棬�����ߵ�ҳ
	virtual IEinkuiIterator* __stdcall GetToolTipPage(void)=NULL;

	// �������Ԫ�أ�����Ԫ��������XUI����ʱָ��������Ԫ��ʵ�־���Ӧ�õ�ȫ�ֹ��ܣ��磺Idealife��ȫ��Ӧ����"Idealife"Ԫ�����ʵ���ṩ��
	// ͨ��������Idealife��Ӧ����Ϣִ��Idealife��ϵͳ����
	// �ɹ����صĽӿڶ���
	virtual IEinkuiIterator* __stdcall GetDesktop(void)=NULL;

	// �����趨��Ԫ�أ�nbZTop==true������Zoder���㣬false�����ڵײ�
	virtual ERESULT __stdcall SetParentElement(IEinkuiIterator* npParent,IEinkuiIterator* npChild,bool nbZTop=true)=NULL;

	// �����꽹�㣬!!!ע��!!!�����صĶ���һ��Ҫ����ReleaseIterator�ͷţ�
	// ��Ϊ��꽹����ʱ���ܸı䣬���У����صĶ���һ������ȫ��ʵ�ķ�Ӧ��ǰ�������
	virtual IEinkuiIterator* __stdcall GetMouseFocus(void)=NULL;

	// ��ü��̽��㣬!!!ע��!!!�����صĶ���һ��Ҫ����ReleaseIterator�ͷţ�
	// ��Ϊ���̽�����ʱ���ܸı䣬���У����صĶ���һ������ȫ��ʵ�ķ�Ӧ��ǰ�������
	virtual IEinkuiIterator* __stdcall GetKeyboardFocus(void)=NULL;

	// ������ק��㣬����ϵͳ��������ק��Ϊ��ʱ�����Խ���תת�Ƹ�����
	// �����ͼת�Ƶ���Ŀ��Ԫ�ز���֧����ק��Ϊ����ǰ����ק��ΪҲ����ֹ����ǰ��ק��Ŀ��Ԫ�ػ��յ�Drag_end��Ϣ
	virtual ERESULT __stdcall ResetDragBegin(IEinkuiIterator* npToDrag)=NULL;

	// ����������ϢLMSG_GET_TYPE(MsgId) == LMSG_TP_WIN_INPUT
	virtual void __stdcall CleanHumanInput(bool nbStallInput=false) = NULL;

	// ����һ����Ϣ
	// ��Ϣ���ͺ󣬷�������Ȼ��Ҫ�ͷ�
	virtual IEinkuiMessage* __stdcall  AllocateMessage(void)=NULL;

	// ����һ����Ϣ����ʼ����ز���
	// ��Ϣ���ͺ󣬷�������Ȼ��Ҫ�ͷ�
	virtual IEinkuiMessage* __stdcall AllocateMessage(
		IN ULONG nuMsgID,	// ��Ϣ����
		IN const void* npInputBuffer,	// �������ݵĻ�����
		IN int niInputSize,	// �������ݵĴ�С
		OUT void* npOutputBuffer,	// ���������(���ػ�����)
		IN int niOutputSize,	// �����������С
		IN bool nbInputVolatile=true	// ���뻺�����Ƿ�����ʧ�ģ��μ�IXuiMessage::SetInputData��ø�����Ϣ
		)=NULL;

	// ��ָ��Ԫ�ط���һ����Ϣ������ģʽ��Send
	// ��Ϣ���ͺ󣬷�������Ȼ��Ҫ�ͷţ������ܱ����Windows�����߳�������������Send���͵���Ϣ
	virtual ERESULT __stdcall SendMessage(
		IEinkuiIterator* npDestElement,	// ������Ϣ��Ŀ��Ԫ��
		IEinkuiMessage* npMsg
		)=NULL;

	// ��ָ��Ԫ�ط���һ����Ϣ������ģʽ��Post
	// ��Ϣ���ͺ󣬷�������Ȼ��Ҫ�ͷ�
	virtual ERESULT __stdcall PostMessage(
		IEinkuiIterator* npDestElement,	// ������Ϣ��Ŀ��Ԫ��
		IEinkuiMessage* npMsg,
		IN ULONG nuPostType=EMSG_POSTTYPE_NORMAL	// ��Ϣ�����ȼ���EMSG_POST_FAST,EMSG_POST_REVERSE
		)=NULL;

	// �򵥵ظ�ָ��Ԫ�ط���һ����Ϣ������ģʽ��Send���˺����ķ��سɹ�������Ϣ����ķ���ֵ�������ԭ��Ͳ�һ������Ϣ����ķ���ֵ����������Ϣ����ʧ��
	// �����ܱ����Windows�����߳�������������Send���͵���Ϣ
	virtual ERESULT __stdcall SimpleSendMessage(
		IEinkuiIterator* npDestElement,	// ������Ϣ��Ŀ��Ԫ��
		IN ULONG nuMsgID,	// ��Ϣ����
		IN const void* npInputBuffer,	// �������ݵĻ�������ע�⣬���ϣ�����ݵĲ�����һ��ָ�뱾���磺lpAnybody��Ӧ����˵���SetInputData(&lpAnybody,sizeof(lpAnybody),ture/false);
		IN int niInputSize,	// �������ݵĴ�С
		OUT void* npOutputBuffer,	// ���������(���ػ�����)
		IN int niOutputSize	// �����������С
		)=NULL;

	// �򵥵ظ�ָ��Ԫ�ط���һ����Ϣ������ģʽ��Post���޷������Ϣ����ķ���ֵ���˺����ķ���ֵ����ʾ��Ϣ�Ƿ񱻳ɹ�������Ϣ����
	virtual ERESULT __stdcall SimplePostMessage(
		IEinkuiIterator* npDestElement,	// ������Ϣ��Ŀ��Ԫ��
		IN ULONG nuMsgID,	// ��Ϣ����
		IN const void* npInputBuffer,	// �������ݵĻ�������ע�⣬���ϣ�����ݵĲ�����һ��ָ�뱾���磺lpAnybody��Ӧ����˵���SetInputData(&lpAnybody,sizeof(lpAnybody),ture/false);
		IN int niInputSize,	// �������ݵĴ�С
		IN ULONG nuPostType=EMSG_POSTTYPE_NORMAL	// ��Ϣ�����ȼ���EMSG_POST_FAST,EMSG_POST_REVERSE
		)=NULL;

	// ö��ȫ��Ԫ�أ�ÿ������һ��Elementʱ����ö���������ṩ��ElementEnter��������һ��Ԫ��û����Ԫ��ʱ���������ṩ��ElementLeave
	// ��Ϊ���ڵ���XUIϵͳ���������ö�ٲ��ᴥ�����ڵ�
	virtual ERESULT __stdcall EnumAllElement(
		bool nbReverse,				// ����ָ����ö���ӽڵ�ʱ������Zorder��˳��ö�٣����߰���Zorder������ö��
		IBaseObject* npApplicant,	// �������
		ERESULT (__stdcall IBaseObject::*ElementEnter)(IEinkuiIterator* npRecipient),//�������ERESULT_ENUM_CHILD����ö�٣�����ERESULT_STOP_ENUM_CHILD����������ֵ��ֹͣö�ٴ�Ԫ�صĴ�Ԫ�ص���Ԫ��
		ERESULT (__stdcall IBaseObject::*ElementLeave)(IEinkuiIterator* npRecipient) //����ֵERESULT_EXIT_ENUM���ߴ����룬ֹͣȫ��ö����Ϊ��ERESULT_REDO_ENUM���ٴζԴ˶������Ӷ���ִ��ö�٣�����ֵ����ִ��
		)=NULL;

	// ����Iterator�����ã�����XUI�Ŀͻ�������ܻ���©��Iterator���ͷź����ò���������Ĭ�ϵ�Iterator->AddRef()��Iterator->Release()�����Ǽٵģ����������ʵ�ʵĵ��ã���Element��Close�󣬶�Ӧ��Iteratorһ��
	// �ᱻ�ͷţ��ڱ��ӿ����ṩ����ʵ�����ú��ͷŵķ�������Iterator�����м�Ҫ����������������ͷŽ��ᵼ��XUI������
	// ����Iterator����
	virtual int __stdcall AddRefIterator(IEinkuiIterator* npIterator)=NULL; 

	// �ͷ�Iterator
	virtual int __stdcall ReleaseIterator(IEinkuiIterator* npIterator)=NULL;

	// ע���ݼ�������ݼ���������ע���ݼ���Element�����ܵ���
	// �����ͨ������ϣ�������Alt��)���µĵ�ʱ�����ڼ��̽��㣬������Ϣ�����ȷ��͸����̽��㣬������㷵��ERESULT_KEY_UNEXPECTED�Ż��ж��Ƿ���ڿ�ݼ���Ϊ
	virtual bool __stdcall RegisterHotKey(
		IN IEinkuiIterator* npApplicant,	// ע���Ԫ�أ��������յ�ע���ǿ�ݼ���Ϣ
		IN ULONG nuHotKeyID,	// ���ȶ���õĳ�������������Hotkey��ϵͳ������ID�Ƿ��ظ�������ʹ��ͬһ��ID��Ӧ��ͬ�İ�����ϣ�
								// �û�ʹ�õ�ID��Ҫ����0x7FFFFFFF�����ڵ���0x80000000��ID������ϵͳԤ��Ŀ��ʹ�ã�
		IN ULONG nuVkNumber,	// �������
		IN bool nbControlKey,	// �Ƿ���ҪControl���
		IN bool nbShiftKey,		// �Ƿ���ҪShift���
		IN bool nbAltKey,		// �Ƿ���ҪAlt���
		IN IEinkuiIterator* npFocus=NULL	// ָ�����㷶Χ��������Ԫ�ؼ�����Ԫ�ػ�ü��̽���ʱ���Żᴥ������ע��Ŀ�ݼ�;
										// ʹ��NULL��Ϊ��������ָ�����㷶Χ�������ۼ��̽����ںδ����ܹ��յ�ע��Ŀ�ݼ�����Ϣ��
		)=NULL;

	// ע����ݼ�
	virtual bool __stdcall UnregisterHotKey(
		IN IEinkuiIterator* npApplicant,	// ע����
		IN ULONG UnuKeyNumber
		)=NULL;

};
// ���涨��Ĭ�Ͽ�ݼ�ID
#define EHOTKEY_MAKE_ID(_X) (_X+0x80000000)
#define EHOTKEY_COPY	EHOTKEY_MAKE_ID(1)
#define EHOTKEY_CUT		EHOTKEY_MAKE_ID(2)
#define EHOTKEY_PASTE	EHOTKEY_MAKE_ID(3)

#define EHOTKEY_SELECT_ALL	EHOTKEY_MAKE_ID(10)
#define EHOTKEY_ESC		EHOTKEY_MAKE_ID(20)
#define EHOTKEY_ALTF4	EHOTKEY_MAKE_ID(21)

#define EHOTKEY_ENTER	EHOTKEY_MAKE_ID(25)
#define EHOTKEY_DELETE	EHOTKEY_MAKE_ID(30)



//////////////////////////////////////////////////////////////////////////
// ��Ϣ�ӿ�
__interface IEinkuiMessage:public IBaseObject
{

	// �����Ϣ�Ľ���Ŀ���Iterator�ӿ�
	virtual IEinkuiIterator* __stdcall GetDestination(void)=NULL;

	// ������Ϣ�Ľ���Ŀ�꣬��ϵͳ����Ϣ����ģ����ã��Զ����÷���Ŀ��
	virtual ERESULT __stdcall SetDestination(IEinkuiIterator* npMsgDestItr)=NULL;

	// �����ϢID
	virtual ULONG __stdcall GetMessageID(void)=NULL;

	// ������ϢID
	virtual ERESULT __stdcall SetMessageID(ULONG nuMsgID)=NULL;

	// ��÷���ֵ
	virtual ERESULT __stdcall GetResult(void)=NULL;

	// ���÷���ֵ���������ص��Ǳ������ĳɹ����
	virtual ERESULT __stdcall SetResult(ERESULT nuResult)=NULL;

	// ������Ϣ�����ߣ�����Ϣ�����ڴӿؼ���ؼ�ʹ���߷����¼����ؼ���������Ϣ��ʱ����Ҫ������Ϣ�ķ����ߣ�Iterator�ӿ��ṩ��PostMessageToParent�������Զ����÷�����
	virtual void __stdcall SetMessageSender(IEinkuiIterator* npSender)=NULL;

	// �����Ϣ�ķ�����
	virtual IEinkuiIterator* __stdcall GetMessageSender(void)=NULL;

	// ����Input���ݣ������Ϣ�Ǳ�Post�ģ�nbVolatile����Ϊture�������Ϣ�Ǳ�Send��Ŀ��ģ������ղ���nbVolatile�����Ƿ������뻺��
	virtual ERESULT __stdcall SetInputData(
		const void* npBuffer,	// ע�⣬���ϣ�����ݵĲ�����һ��ָ�뱾���磺lpAnybody��Ӧ����˵���SetInputData(&lpAnybody,sizeof(lpAnybody),ture/false);
		int niSize,
		bool nbVolatile=true	// true:�˻���������ʧ�ģ���Ҫ�������ݵ��ڲ�����; false �˻������Ƿ���ʧ�ģ�����Ϣ���ͺͷ��صĹ�������Ч
		)=NULL;

	// ���Input����ָ�룬ע�⣬��õ�ָ����ڳ�����Ϣ������û�з����ı�ʱ��Ч��һ������Ϣ���ͳ�ȥ�����ߵ�������Ϣ���趨ֵ�ķ������������¸�ָ��ʧЧ
	// ע�⣬�˷�����õ�ָ�벢��һ��ͬǰһ�ε���SetInputData�趨��ָ����ͬ
	virtual const void* __stdcall GetInputData(void)=NULL;

	// ������뻺�����Ĵ�С
	virtual int __stdcall GetInputDataSize(void)=NULL;

	// ����Output�����������������Ϣ����Output Buffer�������Ҫ��Ϣ���ش������ݵģ�Ӧ��Send������Ϣ��������Post�������ȷʵ��ҪPost������Ϣ����ô��ο���������÷�����Ϣ�ķ���;
	// ���ѡ��Post������Ϣ����ǧ��֤�趨��Output�����������޸ĺ��ͷţ�������Ϣ�Ľ��ܷ��Ըû��������ʲ�������
	// ��������������Ϣ������֮ǰ����
	virtual ERESULT __stdcall SetOutputBuffer(
		OUT void* npBuffer,
		IN int niSize	// �������ܳߴ�,�������ݴ�С,������������ʱ,������Ӧ���ǿյ�
		)=NULL;

	// ���Output������ָ�룬ע�⣬��õ�ָ����ڳ��и���Ϣʱ��Ч��
	virtual void* __stdcall GetOutputBuffer(void)=NULL;

	// ���Output�������Ĵ�С
	virtual int __stdcall GetOutputBufferSize(void)=NULL;

	// ��������Output�����������ݴ�С
	// ��������������Ϣ�����ͺ���ã�����Ϣ�Ľ����ߵ���
	virtual void SetOutputDataSize(int niSize)=NULL;
	// ���Output�����������ݴ�С
	// ��������������Ϣ�����ͺ���ã�����Ϣ�Ľ����ߵ���
	virtual int GetOutputDataSize(void)=NULL;

	// ���÷�����Ϣ������Ϣ��Ŀ���������Ϻ�ϵͳ���Զ�����һ���µ���Ϣ���ظ������ߣ�����µ���Ϣ����������Ϣ
	virtual ERESULT __stdcall SetResponseMessage(
		IN IEinkuiIterator* npReceiver,	// ���ܷ�����Ϣ��Ŀ��
		IN void* npContext=NULL	// ���������ģ������������ú�ʹ�ã�������Ϣ������ʱ�����ݸ�������
		)=NULL;

	// ��Ϣ����������ȷ����ǰ��Ϣ�Ƿ�Send����������true,the message was sent from source element; false, the message was posted by the sender;
	virtual bool __stdcall IsSent(void)=NULL;
};

// ΢���ӿڣ�΢����ͬһ���������ֶ���չ���ܲ�����һ�ֽ綨��ʽ��һ����չ���ܲ���������ϵͳ�����ж��ʵ����ÿ��ʵ������һ��΢����
// ͬ����ͬ�Ĺ��ܲ�����ÿһ������ʵ�����Ƕ�����΢����΢������������ʵ�����������ƣ�΢�������һ�������Ĺ��ܲ�����
// ϵͳ�������ӵ��һ�������Ĺ��ܲ�����ֻ��Ψһ��ʵ����ʵ����һ���ǡ�System��,���΢��Ҳ������ϵͳ΢����
__interface IXsWidget : public IBaseObject 
{
	// ��ȡ΢�����ڰ�װĿ¼�������к�׺��'\'
	virtual const wchar_t* __stdcall GetWidgetDefaultPath(void)=NULL;

	// ��Widget��ģ���ļ�������ʵ�ִ�Widget��DLL���ƣ���"IdeaMain.dll"
	virtual const wchar_t* __stdcall GetModuleName(void)=NULL;

	// ��ñ�΢��ʵ������ϵͳ����ʱ�����ĵ�һ��΢��������system��
	virtual const wchar_t* __stdcall GetInstanceName(void)=NULL;

	// ���΢������Module�Ĺ����ӿ�
	virtual IElementFactory* __stdcall GetDefaultFactory(void)=NULL;

	// ���΢����ʵ��ר�����ã�һ��΢��Module������һ��������ͬʱ���ж��ʵ�������ͬʱ��һ̨�����ϣ�ÿһ��Windows�û��ʻ��£�����������һ��Idealife���̣�
	//		������ҪΪ΢����ÿһ������̬ʵ���ṩһ��ר�����ã������Խ���Ҫ���ڱ������������Ϣ��ŵ����ר��������
	virtual ICfKey* __stdcall GetInstanceConfig(void)=NULL;

	//��ȡ΢�����������ʱ�ļ���Ŀ¼,���صĵ�ַ������\\��β,ͬһ����΢���Ĳ�ͬʵ���õ����ǲ�ͬ���ļ���·��
	virtual bool __stdcall GetInstanceTempPath(OUT wchar_t* npswPath,IN LONG nlBufferLen) = NULL;

	// ���΢��ʵ����Home Page
	virtual IEinkuiIterator* __stdcall GetHomePage(void)=NULL;

	// �˳�Widget���˹�����Ҫ��SystemWidget���ùر�ָ����Widget����SystemWidget���ô˷���������������XUI�����˳���
	// ���ܼٶ����ô˷�����ָ���رյ�Widgetʵ���������˳��������ô˷�����Ӧ�÷�ֹ�ٴε���ָ��widgetʵ�����κη�����������䷢��������Ϣ��
	// ���ô˷�����Widgetʵ�����ں��ʵ�ִ��״̬��ʱ�˳������ڸ�Widgetʵ�����ԣ��˷��������ú�����HomePage�����ʵ���ʱ���յ�Destroy��Ϣ��
	virtual void __stdcall Close(void)=NULL;
};


//////////////////////////////////////////////////////////////////////////
// ����λͼ�Ľӿ�
//////////////////////////////////////////////////////////////////////////

__interface IEinkuiBitmap : public IBaseObject {

public:

	// ��ȡָ��λ������ֵ, ARGB
	virtual ERESULT __stdcall GetPixel(IN DWORD x, IN DWORD y, DWORD &nPixel) = NULL;  

	// ��ȡλͼ������Ϣ
	virtual ERESULT __stdcall GetBmpInfo(OUT BITMAP* npBmpInfo) = NULL;	

	// ��ȡλͼ���
	virtual UINT __stdcall GetWidth(void) = NULL; 

	// ��ȡλͼ�߶�
	virtual UINT __stdcall GetHeight(void) = NULL; 

	// ������չ��
	virtual void __stdcall SetExtendLine(IN LONG nlX,IN LONG nlY) = NULL;

	// ��ȡ������չ��
	virtual LONG __stdcall GetExtnedLineX(void) = NULL;

	// ��ȡ������չ��
	virtual LONG __stdcall GetExtnedLineY(void) = NULL;

	// ����λͼ
	virtual ERESULT __stdcall Scale(UINT npWidth, UINT npHeight) = NULL; 

	// ��ȡIWICBitmap�ӿ�ָ�룬ʹ������Ҫ�����ͷŴ˽ӿ�ָ��
	virtual ERESULT __stdcall GetWICObject(OUT IWICBitmap** ppWicBitmap) = NULL;

	// ��ϵͳ�ڲ����ã�����Direct2D���豸�����Դ
	virtual void __stdcall DiscardsBitmapResource(void) = NULL;

	// ���Ƴ�һ���µĶ���,���Ƴ����Ķ�����Ҫʹ�����Լ��ͷ�
	virtual IEinkuiBitmap* __stdcall DuplicateBitmap(void) = NULL;

public:
	// ��ȡָ�������λͼ����
	virtual bool __stdcall GetBitmapBuffer(IN D2D1_RECT_F ndSrcRect, OUT void* npRawData);

	// ���������ͼ
	virtual bool __stdcall SaveAsThumbnail(IN LONG nuWidth, IN LONG nuHeight, IN D2D1_RECT_F ndSampleRect, const wchar_t* nswOutputFilePath);

};


//////////////////////////////////////////////////////////////////////////
// Ԫ�ص����ӿڣ�XUIϵͳΪÿһ��Ԫ�ض�������Ӧ�ĵ�����
//////////////////////////////////////////////////////////////////////////
__interface IEinkuiIterator:public IBaseObject
{
	// ����Iterator��׼��������Ϣ���������������Element���Ȼ��յ�EMSG_CREATE��Ϣ���������ͨ����Element��ʵ���������˳�ǰ����
	virtual void __stdcall Start(void)=NULL;

	// �ر�һ��Ԫ�أ�����Close֮�󣬴�Iterator��Ӧ�Ķ�����յ�Destroy��Ϣ���������Close����֮�󣬴�Iterator�Լ�����ָ���Element���󶼲����ڱ�ʹ�ã����ң�����Ա�Iterator����Release����
	virtual void __stdcall Close(void)=NULL;

	// ��ñ�Element������Etype
	virtual const wchar_t* __stdcall  GetType(void)=NULL;

	// ��ñ�Element��EID
	virtual ULONG __stdcall  GetID(void)=NULL;

	// ��ñ���������Ӧ��Element����
	virtual IXsElement* __stdcall GetElementObject(void)=NULL;

	// ��ø�����
	virtual IEinkuiIterator* __stdcall  GetParent(void)=NULL;

	// �����һ����Ӷ��������
	virtual int __stdcall GetSubElementCount(void);

	// ͨ��ZOder�����д������ӽڵ�
	virtual IEinkuiIterator* __stdcall  GetSubElementByZOder(
		int niPos	// zero base index value to indicate the position in z-order array
		)=NULL;

	// ͨ��ID����ӽڵ�
	virtual IEinkuiIterator* __stdcall  GetSubElementByID(ULONG nuEid)=NULL;

	// ѯ��ĳ��Iterator�ڲ�νṹ���ǲ��ǵ�ǰIterator������
	virtual bool __stdcall FindAncestor(const IEinkuiIterator* npIsAncestor)=NULL;

	// ����Ԫ�ط���һ����Ϣ������ģʽ��Send
	// ��Ϣ���ͺ󣬷�������Ȼ��Ҫ�ͷţ����ϣ���Ը��Ӽ򵥵ķ�ʽ������Ϣ���ο�IXelManager��SimpleSendMessage����
	virtual ERESULT __stdcall SendMessage(IEinkuiMessage* npMsg)=NULL;

	// ����Ԫ�ط���һ����Ϣ������ģʽ��Post
	// ��Ϣ���ͺ󣬷�������Ȼ��Ҫ�ͷţ����ϣ���Ը��Ӽ򵥵ķ�ʽ������Ϣ���ο�IXelManager��SimplePostMessage����
	virtual ERESULT __stdcall PostMessage(IEinkuiMessage* npMsg)=NULL;

	// ����Ԫ�صĸ�Ԫ�ط���һ����Ϣ�����͵�ģʽ��FastPost
	// ��Ϣ���ͺ󣬷�������Ȼ��Ҫ�ͷ�
	// !!!ע��!!! �����ڷ������Ǳ���������Ӧ��Ԫ��֮���
	virtual ERESULT __stdcall PostMessageToParent(IEinkuiMessage* npMsg)=NULL;

	// ������̽��㣬�����Ԫ�ؾ���popup���ԣ�Ҳ�������������ʵ��ϲ�
	virtual ERESULT __stdcall SetKeyBoardFocus(void)=NULL;

	// �ͷż��̽��㣬���nbShiftFocus==true���⽫����Tab Order����һ�����̽����߻�ý���
	virtual void __stdcall ReleaseKeyBoardFocus(bool nbShiftFocus=false)=NULL;

	// ����Ԫ��Ϊ��Ծ�������Ԫ�ز�����EITR_STYLE_POPUP����EITR_STYLE_ACTIVABLE�����ô��͵�һ������EITR_STYLE_POPUP����EITR_STYLE_ACTIVABLE�����ϲ�Ԫ�ؽ�������
	virtual void __stdcall SetActive(void)=NULL;

	// ����Style���⽫�޸�ȫ����Styleѡ��ΪnuStyles
	virtual void __stdcall SetStyles(ULONG nuStyles)=NULL;

	// �޸�Style��ǰһ��������ϣ�����ӵ�Style����һ��������ϣ���Ƴ���Style����ǰ�����������а�����ͬStyleʱ����Style���ᱻ�Ƴ� 
	virtual void __stdcall ModifyStyles(ULONG nuSet,ULONG nuClear=0)=NULL;

	// ��ȡStyle
	virtual ULONG __stdcall GetStyles(void)=NULL;

	// ���붨ʱ�����������ô����Ķ�ʱ������Ҫע��
	virtual ERESULT __stdcall SetTimer(
		IN ULONG nuID,	  // ��ʱ��ID
		IN ULONG nuRepeat,// ��Ҫ�ظ������Ĵ�����MAXULONG32��ʾ��Զ�ظ�
		IN ULONG nuDuration,	// ��������
		IN void* npContext//�����ģ������Ŷ�ʱ����Ϣ���͸�������
		)=NULL;

	// ���ٶ�ʱ��
	virtual ERESULT __stdcall KillTimer(
		IN ULONG nuID	  // ��ʱ��ID
		)=NULL;

	// HookĿ�꣬��ǰ��֧�ֵ���ε�Hook������һ��Ԫ����ͬһʱ�̽���һ��Ԫ��Hook����ͼHookһ���Ѿ���Hook��Ԫ��ʱ�����᷵��ʧ��ERESULT_ACCESS_CONFLICT
	virtual ERESULT __stdcall SetHook(
		IN IEinkuiIterator* npHooker,	// Hook�����ߣ�һ��������Hook���������������Ϣ��EMSG_HOOKED_MESSAGE���ᱻת�����������ȷ��͸�Hooker����Hooker�����޸��������Ϣ��Ҳ������ֹ��Ϣ���͸�������
		IN bool nbSet		// true to set ,false to remove
		)=NULL;

	// ���Hooker����ȡ��Ԫ�ر�˭Hook
	virtual IEinkuiIterator* __stdcall GetHooker(void)=NULL;

	// ������Ⱦ��Ч������Ч�����ڸ�ĳ��Ԫ�غ�������Ԫ���ṩ�ض�����Ⱦ����Ч������ѡ��Direct2D��Direct3D��������XUIϵͳ����Ⱦ
	// ͬһ��Ԫ����ͬһʱ��ֻ����һ����Ч���ڹ��������ң�ͨ����Ч�����Ƕ��丸Ԫ�ط�������
	// ����ERESULT_ACCESS_CONFLICT��ʾ�����Ч��������ͻ����Ч�����ã����ڽ��յ�EMSG_PREPARE_PAINTʱ���������ط������ã��п��ܵ������ش���
	virtual ERESULT __stdcall SetEnhancer(
		IN IEinkuiIterator* npEnhancer,
		IN bool nbEnable		// true ���ã�false ȡ��
		)=NULL;

	// �����Ч��
	virtual IEinkuiIterator* __stdcall GetEnhancer(void)=NULL;

	//////////////////////////////////////////////////////////////////////////
	// ��������������ʾ��λ����صķ���

	// ������ʾ/����
	virtual void __stdcall SetVisible(bool nbVisible)=NULL;

	// ��ȡ��ʾ/����״̬
	virtual bool __stdcall IsVisible(void)=NULL;

	// �趨����͸����
	virtual void __stdcall SetAlpha(FLOAT nfAlpha)=NULL;

	// ��ȡ����͸����
	virtual FLOAT __stdcall GetAlpha(void)=NULL;

	// ����ƽ������
	virtual void __stdcall SetPosition(FLOAT nfX,FLOAT nfY)=NULL;
	virtual void __stdcall SetPosition(const D2D1_POINT_2F& rPosition)=NULL;

	// ��ȡƽ������
	virtual FLOAT __stdcall GetPositionX(void)=NULL;
	virtual FLOAT __stdcall GetPositionY(void)=NULL;
	virtual D2D1_POINT_2F __stdcall GetPosition(void)=NULL;
	virtual void __stdcall GetRect(D2D1_RECT_F& rRect)=NULL;

	// ���ÿ�������
	virtual void __stdcall SetVisibleRegion(
		IN const D2D1_RECT_F& rRegion		// �����������Ŀ������򣬴�����֮�ⲻ����ʾ��Ԫ�ؼ���Ԫ�ص����ݣ����rRegion.left > region.right ��ʾȡ������������
		)=NULL;

	// ��ȡ�������򣬷���false��ʾû�����ÿ�������
	virtual bool __stdcall GetVisibleRegion(
		OUT D2D1_RECT_F& rRegion	// ���ؿ����������û�����ÿ��������򲻻��޸��������
		)=NULL;

	// ����ƽ��ת��
	virtual void __stdcall SetRotation(
		FLOAT nfAngle,			// �Ƕȵ�λ -359 -> +359��
		D2D1_POINT_2F ndCenter
		)=NULL;
	// ����ƽ��ת�ǣ���Ԫ������Ϊ��ת����
	virtual void __stdcall SetRotation(
		FLOAT nfAngle			// �Ƕȵ�λ -359 -> +359��
		)=NULL;

	// ��ȡƽ��ת��
	virtual FLOAT __stdcall GetRotationAngle(void)=NULL;
	virtual D2D1_POINT_2F __stdcall GetRotationCenter(void)=NULL;
	virtual FLOAT __stdcall GetRotation(D2D1_POINT_2F& rCenter)=NULL;

	// ���òο��ߴ�
	virtual void __stdcall SetSize(FLOAT nfCx,FLOAT nfCy)=NULL;
	virtual void __stdcall SetSize(const D2D1_SIZE_F& rSize)=NULL;

	// ��ȡ�ο��ߴ�
	virtual FLOAT __stdcall GetSizeX(void)=NULL;
	virtual FLOAT __stdcall GetSizeY(void)=NULL;
	virtual D2D1_SIZE_F __stdcall GetSize(void)=NULL;

	// �����Ƿ���Ч
	virtual void __stdcall SetEnable(bool nbSet)=NULL;

	// ��ȡ�Ƿ���Ч
	virtual bool __stdcall IsEnable(void)=NULL;

	// ����Ԫ�ص�����ͬ��������߲�
	virtual void __stdcall BringToTop(void)=NULL;

	// ����Ԫ���ڸ�Ԫ�ص�Z Order�����У������ƶ�һλ
	virtual bool __stdcall MoveDown(void)=NULL;

	// ����Ԫ���ڸ�Ԫ�ص�Z Order�����У������ƶ�һλ
	virtual bool __stdcall MoveUp(void)=NULL;

	// ����������Ԫ�صĵ��Ŵ���Ϊԭʼ���ã��������ļ��ƶ��Ĵ���
	virtual bool __stdcall ResetChildrenByDefualtZOrder(void)=NULL;

	// �������ñ�Ԫ�ص�Z Order��ֵ����������ֵ�Element������ͬ��Z Order���޷�ȷ�����ǵ��Ⱥ�ѭ�򣬵�ϵͳ�������в������
	virtual void __stdcall SetDefaultZOrder(const LONG nlDefaultZOrder)=NULL;

	// ���Ĭ�ϵ�ZOrderֵ
	virtual LONG __stdcall GetDefaultZOrder(void)=NULL;

	//����ToolTip������ڱ���������ͣ�������Զ���ʾ�ĵ��м��������ʾ�����һ���ƿ���ʾ��ToolTip�����Զ���ʧ
	virtual void __stdcall SetToolTip(const wchar_t* nswTip)=NULL;

	//����IME���봰��λ�ã�ndPosition�Ǳ�Ԫ�ؾֲ������е�λ��; ��ԭԪ�ؾ���EITR_STYLE_DISABLE_IME����ʱ��Ч
	virtual void __stdcall SetIMECompositionWindow(D2D1_POINT_2F ndPosition)=NULL;

	// �Ӿֲ���ַ�������ַ
	virtual bool __stdcall LocalToWorld(const D2D1_POINT_2F& crLocalPoint,D2D1_POINT_2F& rWorldPoint);

	// �������ַת��Ϊ�ֲ���ַ
	virtual bool __stdcall WorldToLocal(const D2D1_POINT_2F& crWorldPoint,D2D1_POINT_2F& rLocalPoint);

	// ����Ԫ�ؽ������Ʋ㣬���Ʋ���һ���ı���Ԫ�ػ���ʱ���Ŵ���ķ�����ͨ������Ԫ�ذ��մ�����ϵ����Ϊ���νṹ������ʱҲ�ǰ������νṹ�ȸ�����ִ�У�
	//		������Ʋ㼼������Ԫ�ؽ��ڲ�ͬ������λ��ƣ�ͬһ�������Ԫ�أ���Ȼ���մ�����ϵ�����νṹ�ȸ�������
	//		�����趨����Ļ��Ʋ㣬�����ܴ���max-ulong����������ж�Ҳ�յ����Ʋ��Ӱ�죬��θߵ�Ԫ�����ȱ��ж�������
	//		�����һ���Ѿ��趨�˻��Ʋ�ε������е�ĳ��Ԫ���ٴν������Ʋ�Σ���ô���������е�������Ԫ�ؽ����ܵ���һ�����Ʋ�ε�Ӱ�죬�����µĲ�ι�����
	//		Ϊ�˱�������뾡���ܱ���ʹ��Ƕ�׵Ļ��Ʋ��
	virtual ERESULT __stdcall CreatePaintLevel(LONG nlLevelCount)=NULL;

	// �����Ԫ�ػ��Ʋ�����������0��ʾû�����û��Ʋ��
	virtual LONG __stdcall GetPaintLevelCount(void)=NULL;

	// ɾ�����Ʋ���趨
	virtual ERESULT __stdcall DeletePaintLevel(void
		//bool nbClearAll=true		// �����������Ԫ�صĻ��Ʋ���趨
		)=NULL;

	// �趨����Ļ��Ʋ��
	virtual ERESULT __stdcall SetPaintLevel(LONG nlLevel)=NULL;

	// �������Ļ��Ʋ��
	virtual LONG __stdcall GetPaintLevel(void)=NULL;

};

// Ԫ��Style��ֵ��ע�⣬�����ǿ��Ա����ʹ�õģ�ÿһ��ֵ���Ƕ����Ƶ�һλ������
#define EITR_STYLE_NONE			0
#define EITR_STYLE_ACTIVABLE	ES_BIT_SET(1)		// ���ܼ������������PopUp�Ĵ���һ�����б�����
#define EITR_STYLE_POPUP		ES_BIT_SET(2)|ES_BIT_SET(1)		// ����Ԫ�أ������ܸ�Ԫ���趨�Ŀ�����������ƣ�ͬʱ��ͬ����PopupԪ�صĵ��Ŵ������Ϊ���̺���꽹��Ļ�ö��Զ���������ý����PopupԪ�ؽ��Զ����������ϲ�
#define EITR_STYLE_CENTER		ES_BIT_SET(4)		// ʼ�շ����ڸ�����Ĳο��ߴ�����
#define EITR_STYLE_TOP			ES_BIT_SET(5)		// ����ͬ��Ԫ�ص���߲㣬���TOP�����������Ⱥ�ߵ����ţ������ϣ������Ҳ����������Ӱ��
//#define EITR_STYLE_TOPDRAW		ES_BIT_SET(6)		// ��Ҫ�ڽ��涥��ִ�л��Ʋ���
#define EITR_STYLE_SNAPSHOT_HIDE	ES_BIT_SET(6)		// ������ʱ������ʾ
#define EITR_STYLE_LAZY_UPDATE	ES_BIT_SET(11)		// ����ÿ��5�ε�Update��Ϣ
#define EITR_STYLE_MOUSE		ES_BIT_SET(12)	// ���������Ϣ
#define EITR_STYLE_DRAG			(ES_BIT_SET(13)|ES_BIT_SET(12))	// ������ק��Ϣ
#define EITR_STYLE_ALL_DRAG		(ES_BIT_SET(14)|ES_BIT_SET(13)|ES_BIT_SET(12))	// ��ȫ��ק����ק��ʼ�ڱ�Ԫ�صĲ�֧����ק��Ԫ��ʱ�����߸�Ԫ�ط�����קʧ��ʱ����ק��Ϣ���Զ����͸���Ԫ��
#define EITR_STYLE_ALL_MWHEEL	(ES_BIT_SET(15)|ES_BIT_SET(12))	// �����ǰ��꽹�㴦��ĳ���Ӷ��󣬸��Ӷ��󲻴����ȫ�����Wheel��Ϣ������ת���������󣬾���popup���Ե��Ӷ���������Ӷ��󶼲�������ת��������Ϣ

#define EITR_STYLE_KEYBOARD  ES_BIT_SET(18)		// ���ܼ�����Ϣ
#define EITR_STYLE_ALL_KEY	 ES_BIT_SET(19)|ES_BIT_SET(18)		// �����Լ������в����ܼ��̽�����Ӷ�����ܼ��̽���
#define EITR_STYLE_COMMAND	ES_BIT_SET(20)	// ��������������Ϣ����EMSG_COMMAND�������ǰ���̽��㲻�������ϵͳ��������������ϼ�������
#define EITR_STYLE_DISABLE_IME			ES_BIT_SET(22)	// ��ʹ��IME

#define EITR_STYLE_WINDRAGDROP		ES_BIT_SET(25)	// ����Windows��Drag&Drop��Ϊ��ͨ��Xuiϵͳ�ṩ����Ӧ����ʵ�ֶ�Windows��֧��
#define EITR_STYLE_XUIDRAGDROP		ES_BIT_SET(26)	// ����XUIϵͳ��Drag&Drop��Ϊ



//////////////////////////////////////////////////////////////////////////
// Ԫ�ض���Ľӿڣ�Ԫ�ص�ʵ��ģ���ṩ��Ԫ�ض���ӿڣ�����Ԫ�ر����Ԫ�ع������������ⲿʹ��Ԫ�ص��������Ԫ�ؽ���
//////////////////////////////////////////////////////////////////////////
__interface IXsElement:public IBaseObject
{
	// ��ñ�Ԫ�ص�Ԫ������
	virtual const wchar_t* __stdcall  GetType(void)=NULL;

	// ��ȫ�ַ�Χ����֤�˶����Ƿ���nswTypeָ�������ͣ�BaseObject�ṩ��IsKindOf����ֻ����֤ͬһ��ģ���ڵĶ���
	virtual bool __stdcall  GlobleVerification(const wchar_t* nswType)=NULL;

	// Ĭ����Ϣ��ں��������ڽ���������Ϣ
	virtual ERESULT __stdcall MessageReceiver(IEinkuiMessage* npMsg)=NULL;

	// ��ñ�Ԫ�صĵ������ӿ�
	virtual IEinkuiIterator* __stdcall GetIterator(void)=NULL;

	// ���Tab Order, -1 δ����
	virtual LONG __stdcall GetTabOrder(void)=NULL;

	// ���Z Order��-1 δ����
	virtual LONG __stdcall GetZOrder(void)=NULL;
};


//////////////////////////////////////////////////////////////////////////
//Ԫ�����������ʽ
//////////////////////////////////////////////////////////////////////////
//Ԫ��������ID��������������ES_ETYPE_ǰ׺����������͵����ƣ����е���չ������ES_ETYPE_��ͷ������������չ������Ȼ��������������: ES_ETYPE_STOCK_CHART���Ϳ������Ϊĳ����Ʊ��չģ���ͼ��ؼ�
//Ԫ�����ʵ�������ݽṹ�����ƹ���ST_ELALC_ǰ׺��������������͵�����;�磺ST_ELALC_ELEMENT
//Ԫ���ഥ������Ϣ������ʱ����EEVT_ǰ׺
//Ԫ������ܵķ�����Ϣ������ʱ����EACT_ǰ׺



// �������ͣ�ʵ����XUI.DLL�У�������Ԫ�ض���Ļ�������ӵ��ID�����ͣ��ܹ��ж����ͣ��ܹ���ϵͳע���ע�����ܹ�������Ϣ
#define ES_ETYPE_ELEMENT L"Element"


// ��������Ϣ
#define EEVT_ELEMENT_MOUSE_ON		EMSG_DEFINE(EMSG_CLASS_ELEMENT,1,1)
#define EEVT_ELEMENT_MOUSE_OFF		EMSG_DEFINE(EMSG_CLASS_ELEMENT,1,2)
#define EEVT_ELEMENT_MOUSE_HOVER	EMSG_DEFINE(EMSG_CLASS_ELEMENT,1,3)	// ��ʾ���������ؼ�����ͣ����ͣ��0.4��


//////////////////////////////////////////////////////////////////////////


// �������ӿ�
__interface IXelAllocator:public IBaseObject
{
public:
	// �������ļ��д�������
	virtual IEinkuiIterator* __stdcall CreateElement(
		IN IEinkuiIterator* npParent,		// ������ָ��
		IN ICfKey* npTemplete,			// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		) = NULL;

	// ͨ����������������,�������Ķ�����Я������
	virtual IEinkuiIterator* __stdcall CreateElement(
		IN IEinkuiIterator* npParent,		// ������ָ��
		IN const wchar_t* nswClassName,		// ����
		IN ULONG nuEID=MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		) = NULL;


	// ����һ��ͼƬ�ļ�
	virtual IEinkuiBitmap* __stdcall LoadImageFile(
		IN wchar_t* nswRelativePathName,		//��ֵΪ���·��ʱ�����·��Ϊ��ģ��Dll����Ŀ¼
		IN bool nbIsFullPath = false			//�õ����ʾnswRelativePathNameΪȫ·��������Ϊ���·��
		) = NULL;

	// ��ָ����PE�ļ��У���ȡλͼ��Դ������һ��λͼ����
	// �������ȡ��ָ����ߵ�ͼ�꣬���������ͼ������λͼ
	virtual IEinkuiBitmap* __stdcall LoadImageFromPE(
		IN wchar_t *npPeFileName,				// ָ��PE�ļ�ȫ·��
		IN int niDummy,							// ��Դ����
		IN int niXSize = 64,					// ָ��Ŀ��ͼ��Ŀ��
		IN int niYSize = 64						// ָ��Ŀ��ͼ��ĸ߶�
		) = NULL;

	//����������ͼƬ��STETXT_BMP_INIT������ýӿڶ����Ϸ�
	virtual IEinkuiBitmap* __stdcall CreateImageByText(STETXT_BMP_INIT& rdBmpInit) = NULL;

	//���ڴ潨��һ��λͼ�����򷵻غ�npRawData�����е������ͷ�
	virtual IEinkuiBitmap* __stdcall CreateBitmapFromMemory(
		LONG nuWidth,					// λͼ���
		LONG nuHeight,					// λͼ�߶�
		LONG PixelSize,					// ���ص�λ��3 or 4
		LONG Stride,
		void* npRawData					// λͼԭʼ����
		) = NULL;


};


//////////////////////////////////////////////////////////////////////////
// ���幤����ӿ�
// ���й����඼�����ṩ���ӿڣ�һ�����������ʵ�������ָ����͵Ķ���

__interface IElementFactory : public IBaseObject
{

public:
	// �������ļ��д�������
	virtual IEinkuiIterator* __stdcall CreateElement(
		IN IEinkuiIterator* npParent,		// ������ָ��
		IN ICfKey* npTemplete,			// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		) = NULL;

	// ͨ����������������
	virtual IEinkuiIterator* __stdcall CreateElement(
		IN IEinkuiIterator* npParent,		// ������ָ��
		IN const wchar_t*		nswClassName,	// ����
		IN ULONG nuEID=MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		) = NULL;

	// ������Module���׵�Profile�ļ��ӿڣ����صĽӿڵ�����ʹ��ʱ����ҪRelease
	virtual IConfigFile* __stdcall GetTempleteFile(void)=NULL;

};


//////////////////////////////////////////////////////////////////////////
// ��ˢ�ӿ�
//////////////////////////////////////////////////////////////////////////

// �ýӿڳ�����D2D��Stroke��Brush����Լ�GDIPlus��Pen��Brush�ĸ���
// �����ǻ��߻�����䣬���ó�����IXuiBrush�ӿڣ������ʵ�����ݲ�ͬƽ̨��ʵ�ִ������


__interface IEinkuiBrush : public IBaseObject {


	// ����D2D��GDIPlushƽ̨�Ĺ�����Ϊ����
public:
	// ����SOLID��ˢ����ɫ
	virtual void __stdcall SetColor(IN D2D1_COLOR_F noColor) = NULL;

	// ���ý��仭ˢ����ɫ
	virtual void __stdcall SetLinearBrushProperties(
		const D2D1_GRADIENT_STOP* npGradientStop, 
		ULONG nuCount, 
		D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES ndLinearGradientBrushProperties
		) = NULL;

	// �������� �����������ʱ����
	virtual bool __stdcall SetStrokeType(const D2D1_STROKE_STYLE_PROPERTIES &strokeStyleProperties, const FLOAT *dashes, UINT dashesCount) = NULL;

	// �����߿�
	virtual void __stdcall SetStrokeWidth(IN float nfWidth) = NULL;

	// ��ȡ�ֿ�
	virtual float __stdcall GetStrokeWidth() = NULL;

	// ��ϵͳ�ڲ����ã�����Direct2D���豸�����Դ
	virtual void __stdcall DiscardsBrushResource(void) = NULL;

	// ���Ƴ�һ���µĶ��󣬸��Ƴ����Ķ�����Ҫʹ�����Լ��ͷ�
	virtual IEinkuiBrush* __stdcall DuplicateBrush(void) = NULL;

	// ����D2D�����������Ϊ
	//public:
	//	// ��ȡ��ˢ����
	//	virtual ERESULT GetBrushObject(OUT ID2D1Brush** npBrushObject) = NULL;
	//
	//	// ��ȡ�������͵�Stroke����
	//	virtual ERESULT GetStrokeObject(OUT ID2D1StrokeStyle** npStrokeObject) = NULL;


	// ����GDIPlush�����������Ϊ
	//public:
	//	// ��ȡ��ˢ����
	//	virtual ERESULT GetBrushObject(OUT Brush** npBrushObject) = NULL;
	//
	//	// ��ȡ���ʶ���
	//	virtual ERESULT GetStrokeObject(OUT Pen** npPenObject) = NULL;




};


//////////////////////////////////////////////////////////////////////////
// �滭��Դ�Ĵ���
//////////////////////////////////////////////////////////////////////////

__interface IEinkuiPaintResouce{

// �������໭ˢ�Ľӿ�
public:

	// �������仭ˢ
	virtual IEinkuiBrush* __stdcall CreateBrush(
		XuiBrushType niBrushType,
		D2D1_COLOR_F noColor
		) = NULL;

	// ���仭ˢʱ����Ҫ��������ɫ��
	virtual IEinkuiBrush* __stdcall CreateBrush(
		XuiBrushType niBrushType, 
		D2D1_GRADIENT_STOP* npGradientStop, ULONG nuCount, 
		D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES ndLinearGradientBrushProperties
		) = NULL;

};


//////////////////////////////////////////////////////////////////////////
// ����ӿ�
//////////////////////////////////////////////////////////////////////////

// �ýӿڳ�����D2D��GDIPlus�Ļ�ͼ�ӿ�

__interface IEinkuiPaintBoard : public IBaseObject {


	// ��װλͼ�Ļ��Ʋ���
public:

	// ����һ��ͼ�񵽻�ͼ��
	// ��Ҫָ��Ŀ���Դ���򣻽���EMSG_PAINT/EMSG_RENDER_ENHANCER��Ϣ�����ڼ���Ч
	virtual ERESULT __stdcall DrawBitmap(
		IN const D2D1_RECT_F& rDestRect,	// Ŀ������
		IN const D2D1_RECT_F& rSrcRect,	// λͼ�Ĳ�������
		IN IEinkuiBitmap* npBitmap,			// �����Ƶ�λͼ
		IN ULONG nuMethod,				// ���������������Ķ���
		IN float nfAlpha = 1.0f
		)=NULL;

	// ����һ��ͼ�񵽻�ͼ��
	// ��ָ��Ŀ�����򣬲�������������λͼ������EMSG_PAINT/EMSG_RENDER_ENHANCER��Ϣ�����ڼ���Ч
	virtual ERESULT __stdcall DrawBitmap(
		IN const D2D1_RECT_F& rDestRect,	// Ŀ������
		IN IEinkuiBitmap* npBitmap,			// �����Ƶ�λͼ
		IN ULONG nuMethod,				// ���������������Ķ���
		IN float nfAlpha = 1.0f
		)=NULL;

	// ����һ��ͼ�񵽻�ͼ��
	// ��ȫ����ͼ��Ĵ�С���Ƶ�Ŀ�ꣻ����EMSG_PAINT/EMSG_RENDER_ENHANCER��Ϣ�����ڼ���Ч
	virtual ERESULT __stdcall DrawBitmap(
		IN const D2D1_POINT_2F& rDestLeftTop,	// Ŀ����������Ͻ�
		IN IEinkuiBitmap* npBitmap,				// �����Ƶ�λͼ
		IN float nfAlpha = 1.0f
		)=NULL;


	// ��װ���㻭�ߵĲ����ӿ�
public:

	// �����ߺ���
	virtual ERESULT __stdcall DrawLine(
		IN const D2D1_POINT_2F&	noStartPoint,	// ��ʼ��
		IN const D2D1_POINT_2F& noEndPoint,		// ������
		IN IEinkuiBrush* npBrush
		)=NULL;

	// �������
	virtual ERESULT __stdcall DrawPlogon(
		IN const D2D1_POINT_2F*	noStartPoint,	// �����б�
		IN INT	niCount,
		IN IEinkuiBrush* npBrush 
		)=NULL;

	// �������
	virtual ERESULT __stdcall FillPlogon(
		IN const D2D1_POINT_2F*	noStartPoint,	// �����б�
		IN INT	niCount,
		IN IEinkuiBrush* npBrush 
		)=NULL;

	// ����Բ
	virtual ERESULT __stdcall DrawEllipse(
		IN const D2D1_RECT_F& noRect,			
		IN IEinkuiBrush* npBrush
		)= NULL;

	// �����Բ
	virtual ERESULT __stdcall FillEllipse(
		IN const D2D1_RECT_F& noRect,
		IN IEinkuiBrush* npBrush
		)= NULL;


	// ������
	virtual ERESULT __stdcall DrawRect(
		IN const D2D1_RECT_F& noRect,
		IN IEinkuiBrush* npBrush
		)= NULL;

	// ������
	virtual ERESULT __stdcall FillRect(
		IN const D2D1_RECT_F& noRect,
		IN IEinkuiBrush* npBrush
		)= NULL;


	// ��Բ�Ǿ���
	virtual ERESULT __stdcall DrawRoundRect(
		IN const D2D1_RECT_F& noRect,
		FLOAT radiusX,
		FLOAT radiusY,
		IN IEinkuiBrush* npBrush
		)= NULL;


	// ���Բ�Ǿ���
	virtual ERESULT __stdcall FillRoundRect(
		IN const D2D1_RECT_F& noRect,
		FLOAT radiusX,
		FLOAT radiusY,
		IN IEinkuiBrush* npBrush
		)= NULL;	

	// ��D2D�йص�һЩ��������ӿ�
public:

	// ���Direct2D��RenderTarget�������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
	// ��Ӧ����EMSG_PAINT/EMSG_RENDER_ENHANCER��Ϣ�����ڼ�ִ�л��ƶ�����������Prepare��Ϣ�ڼ���ƣ������ƻ���Ⱦ������ȶ�
	virtual ID2D1RenderTarget* __stdcall GetD2dRenderTarget(void)= NULL;

	// ���Direct2D�Ĺ����ӿڣ������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
	virtual ID2D1Factory* __stdcall GetD2dFactory(void)= NULL;

	// ���WIC�����������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
	virtual IWICImagingFactory* __stdcall GetWICFactory(void)= NULL;

	// ���Direct Write�����������ͷţ�ֻ�ܹ��ڻ�ñ�IXuiPaintBoard�ӿڵ���Ϣ��Ӧ�ڼ�ʹ�÷��ص�RenderTarget�ӿڣ���Ҫ���ڱ���˴����صĽӿڣ�����ʧЧ���δ֪����
	virtual IDWriteFactory* __stdcall GetDWriteFactory(void)= NULL;

	// ��õ�ǰ��D2d�����ã��ֲ����굽���������ת������
	virtual const D2D1::Matrix3x2F& __stdcall GetCurrent2DWorldMatrix(void)= NULL;

	// ��õ�ǰ���������ã��������������������������D2dTarget���л��������������л����Target��Ӧ����������
	// ����ERESULT_SUCCESS��ǰ���ڿ��������ã����ҳɹ�ȡ�ã�ERESULT_UNSUCCESS�����ڿ��������ã�����ֵ��ʾ����
	virtual ERESULT __stdcall GetVisibleRegion(
		OUT D2D1_RECT_F* npRegion
		)= NULL;

	// ����ֱ�ӷ���D3d��D2d�����Ԫ�أ�ʹ�ñ��ӿ���ϵͳ�㱨�豸����ϵͳ����ERESULT_SUCCESS��ʾ���Լ���ִ�У�����ֵ�����ERESULT_FAILED����ֹ����ִ��
	virtual ERESULT __stdcall CheckDeviceResult(
		IN HRESULT nhrDeviceResult
		)= NULL;

	// ��õ�ǰ��TickCount
	virtual ULONG __stdcall GetCurrentTickCount()= NULL;

	// ��õ�ǰ֡��
	virtual ULONG __stdcall GetCurrentFps(void)= NULL;

	// ��õ�ǰ����Ⱦ���кţ��������к�������Э���ͼ��㵱ǰ����Ⱦ������ÿִ��һ����Ⱦ����ż�һ��ĳЩ����£�Ԫ�ؿ��ԻᱻҪ�����2�Σ������ﵽ���ֵ����0��ʼ
	virtual ULONG __stdcall GetRenderNumber(void)= NULL;

};
 //////////////////////////////////////////////////////////////////////////
 // λͼ���Ʋ������������������IXuiPaintBoard::DrawBitmap
#define ESPB_DRAWBMP_NEAREST D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR //�ڽ��㷨���ʺ�ѹ��ͼ��
#define ESPB_DRAWBMP_LINEAR D2D1_BITMAP_INTERPOLATION_MODE_LINEAR	// ���Բ�ֵ���ʺϷ���ͼ�񣻶���ϣ���ﵽ�ϺõķŴ�Ч������ʹ��IEinkuiBitmap�ķŴ��㷨����
#define ESPB_DRAWBMP_EXTEND 0xF0000001	// ��չ������ͨ��ָ����չ�ߣ��ص�������չ�ߵ���������չ���㲿�֣���������ʺ�����2Dͼ�ν���ı����ȣ�





































#ifdef __cplusplus
extern "C" {
#endif

// XUI�������������ϣ���޸�DPI��������ڵ��ô˺���ǰ����Win API SetProcessDPIAware
int __stdcall EinkuiStart(
	IN const wchar_t* nswModulePath,	// System Widget��ʵ��ģ����ļ�·����
	IN const wchar_t* nswHomeTempleteName,	// System Widget��Home Page��templete key������
	IN const wchar_t* nswClassesRegPath,	// Xuiע�������ڵ�ע���·������:Software\\Lenovo\\Veriface5\\PublicClasses
	IN ULONG nuAutoRotate=0,				// ����֧���Զ���ת
	IN PEINKUI_CUSTOMDRAW_SETTINGS npCustomDraw=NULL,	// �Ի�Eink
	IN const wchar_t* nswWndTittle = NULL		// �����ڱ���
	);

// ���XUIϵͳ�ӿڣ����Բ���ʧ�ܣ������жϷ���ֵ�Ƿ�ΪNULL
IEinkuiSystem* __stdcall EinkuiGetSystem(void);


#ifdef __cplusplus
}
#endif

#endif//_XUI_H_