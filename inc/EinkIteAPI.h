/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */

#ifndef _EINKITEAPI_H_
#define _EINKITEAPI_H_
/*
	��DLLΪӦ�ó����ṩ�˷���Eink����ȫ���ӿڡ�
	//This Dll will provide all interface to access eink .
	ע�⣺
		����ͬһ���߳��з��ʴ�SDK�ṩ��API�ӿڡ�
	Caution:
		please call these apis in single thread .

*/





#pragma pack(1)

typedef struct _TRSP_SYSTEM_INFO_DATA
{
	unsigned int uiStandardCmdNo; // Standard command number2T-con Communication Protocol
	unsigned int uiExtendCmdNo; // Extend command number
	unsigned int uiSignature; // 31 35 39 38h (8951)
	unsigned int uiVersion; // command table version
	unsigned int uiWidth; // Panel Width
	unsigned int uiHeight; // Panel Height
	unsigned int uiUpdateBufBase; // Update Buffer Address
	unsigned int uiImageBufBase; // Image Buffer Address
	unsigned int uiTemperatureNo; // Temperature segment number
	unsigned int uiModeNo; // Display mode number
	unsigned int uiFrameCount[8]; // Frame count for each mode(8).
	unsigned int uiNumImgBuf;
	unsigned int uiWbfSFIAddr;
	unsigned int uiwaveforminfo;//low byte:A2 mode index
	unsigned int uiMultiPanelIndex;//High two byte for Y-axis, low two byte for X-axis
	unsigned int uiTpXMax;  // Tp resolution
	unsigned int uiTpYMax;
	unsigned char TPVersion[4]; //e.g. v.1.0.9  TPVersion[] = {0x00,0x01,0x00,x09}
	unsigned char ucEPDType;    //0-old (needs 180 rotation), 1 - New(no need to 180 rotation)
	unsigned char ucReserved[3];
	unsigned int uiReserved[2];

	//	void* lpCmdInfoDatas[1]; // Command table pointer
} TRSP_SYSTEM_INFO_DATA;

#pragma pack()

#pragma pack(4)

#define GIHW_OWNER_DRAW	0		// Owner-draw mode. If you need handwriting , call EiSetHandwritingRect
#define GIHW_WACOM_PASS_THROUGH	3		// WACOM Pass through mode


#define	GIR_NONE 0
#define GIR_90  90
#define GIR_180 180
#define GIR_270 270

//Laptop Tent Tablet
#define GIR_MODE_LAPTOP  2
#define GIR_MODE_TENT 3
#define GIR_MODE_TABLET 4

// waveform mode
#define GI_WAVEFORM_INIT      0
#define GI_WAVEFORM_DU2       1
#define GI_WAVEFORM_GC16      2
#define GI_WAVEFORM_GL16      3

//Homebar status
#define GI_HOMEBAR_HIDE      0
#define GI_HOMEBAR_SHOW      1
#define GI_HOMEBAR_EXPAND    2
#define GI_HOMEBAR_COLLAPSE  3
#define GI_HOMEBAR_UP_SHOW   4		//���»�����ʾhomebar

typedef struct _EI_SYSTEM_INFO{
	unsigned long ulWidth; // physical Screen width
	unsigned long ulHeight; // physical Screen height
	unsigned long ulModeNo; // display mode number
	unsigned long ulOrient;	// current system upward orientation (GIR_NONE\GIR_90\GIR_180\GIR_270)
	unsigned long ulDpiX;	// DPI of X-axis
	unsigned long ulDpiY;	// DPI of Y-axis
	unsigned char ucEPDType;    //0-old (needs 180 rotation), 1 - New(no need to 180 rotation)
	unsigned int uiReserved[10];
}EI_SYSTEM_INFO, *PEI_SYSTEM_INFO;

typedef struct _EI_APP_CONTEXT {
	unsigned long ulOrient;	// current upward orientation setting by APP
	unsigned long ulWidth;	// width of screen in display-coordinate(APP-coordinate)
	unsigned long ulHeight;	// height of screen in display-coordinate(APP-coordinate)
	unsigned long ulScreenOrient;	// current B cover orientation
}EI_APP_CONTEXT,* PEI_APP_CONTEXT;

typedef struct  _EI_BUFFER{
	unsigned long ulWidth; // horizontal size of display area
	unsigned long ulHeight; // vertical size of display area
	unsigned long ulBufferSize;// buffer size
	BYTE Buf[1];
}EI_BUFFER,* PEI_BUFFER;

//Touch�¼�
#define EI_TOUCHEVENTF_DOWN 0
#define EI_TOUCHEVENTF_MOVE 1
#define EI_TOUCHEVENTF_UP 2
#define EI_TOUCHEVENTF_HOVERING 3
#define EI_TOUCHEVENTF_HOVERING_LEAVE 4

//Touchʱ�ʰ���
#define EI_TOUCH_PEN_BUTTON_NONE 0
#define EI_TOUCH_PEN_BUTTON_ABOVE 1
#define EI_TOUCH_PEN_BUTTON_BELOW 2
#define EI_TOUCH_EVENT_FINGER 1
#define EI_TOUCH_EVENT_PEN 2
typedef struct _EI_TOUCHINPUT_POINT {
	unsigned long x;
	unsigned long y;
	unsigned long z;		//pressure
	unsigned long Flags;	//0:down  1:move  2:up 3:hovering
	unsigned long FingerOrPen;	//1:Finger 2:Pen
	unsigned long PenButton;	//0:no 1:above 2:below
}EI_TOUCHINPUT_POINT,* PEI_TOUCHINPUT_POINT;

typedef struct _EI_POINT {
	unsigned long x;
	unsigned long y;
}EI_POINT,* PEI_POINT;

typedef struct _EI_SIZE {
	unsigned long w;
	unsigned long h;
}EI_SIZE, *PEI_SIZE;

typedef struct _EI_RECT {
	unsigned long x;		// x-coordinate of upper-left corner
	unsigned long y;		// y-coordinate of upper-left corner
	unsigned long w;	// widht
	unsigned long h;	// height
}EI_RECT,* PEI_RECT;

// 
//Gesture Message
#define WM_EI_GESTURE	WM_USER + 0x101
//WParam The identifier of the gesture command
#define		EI_GID_BEGIN		1	//A gesture is starting.
#define		EI_GID_END			2	//A gesture is ending.
#define 	EI_GID_ZOOM			3	//The zoom gesture.
#define 	EI_GID_ROTATE		4	//The rotation gesture.
#define		EI_GID_PAN			5	//The pan gesture 
#define 	EI_GID_TWOFINGERTAP 6	//The two-finger tap gesture.
#define		EI_GID_PRESSANDTAP	7	//Indicates the press and tap gesture 
//LParam a pointer to a structure defined below. This pointer will be invalid when this message return.
typedef struct _EI_GESTUREINFO {	// as same as WM_GESTURE defined within the normal windows message WM_GESTURE
	UINT      cbSize;
	DWORD     dwFlags;
	DWORD     dwID;
	HWND      hwndTarget;
	POINTS    ptsLocation;
	DWORD     dwInstanceID;
	DWORD     dwSequenceID;
	ULONGLONG ullArguments;
	UINT      cbExtraArgs;
} EI_GESTUREINFO, *PEI_GESTUREINFO;



#define KEYBOARD_STYLE_CLASSIC_BLACK 1
#define KEYBOARD_STYLE_CLASSIC_WHITE 2
#define KEYBOARD_STYLE_MODERN_BLACK 3
#define KEYBOARD_STYLE_MODERN_WHITE 4
typedef struct _EI_KEYBOARD_STYLE {
	unsigned long Langid;	// Language id
	unsigned long Style;	// 
}EI_KEYBOARD_STYLE, *PEI_KEYBOARD_STYLE;

//����C����д����
typedef struct  _EI_SET_TP_AREA {
	EI_RECT Rect; //Ҫ���õ�����
	BYTE Index; //0 ~ 7   ��������8�鲻ͬ�����rect,rect�����ص�
	BYTE Flag; //BYTE ID; //SET_SP_AREA_NO_REPORT��SET_SP_AREA_TOUCH_ONLY��SET_SP_AREA_PEN_ONLY��SET_SP_AREA_TOUCH_PEN
}SET_TP_AREA, *PSET_TP_AREA;
#define SET_SP_AREA_NO_REPORT 0x00
#define SET_SP_AREA_TOUCH_ONLY 0x01
#define SET_SP_AREA_PEN_ONLY 0x02
#define SET_SP_AREA_TOUCH_PEN 0x03


// Touch
#define WM_EI_TOUCH		WM_USER + 0x103
// WParam: NA	
// LParam: EI_TOUCHINPUT_POINT��ָ������ṹ���󣬵���Ϣ�����غ����ָ�뽫ʧЧ
// LParam: EI_TOUCHINPUT_POINT, point to a EI_TOUCHINPUT_POINT , when message return , this pointer will be invalid .

// panel-Rotate
// ֪ͨApp��ƽ�巢����ת���յ�ת����Ϣ�󣬵���EiSetScreenOrient���õ�ǰAPP����Ļ��ʾ����
// Inform app that panel have been rotated. When an app receive a panel-rotation message , it can call EiSetScreenOrient to set the screen-orientation for itself.
#define WM_EI_ROTATION WM_USER + 0x104
// WParam: GIR_NONE\GIR_90\GIR_180\GIR_270
// LParam: NA

// ����Ӧ�û���Eink������ϵͳ�ж���Ҫ���»���ʱ���ʹ���Ϣ��APP��ͨ������EiInvalidPanel API��ʹϵͳ���ʹ���Ϣ����APP
// Ask app to re-draw Eink screen , when system need application to re-draw , system will send message to App . 
// by calling EiInvalidPanel API will urge system to send this message to app .
#define WM_EI_DRAW		WM_USER + 0x110
//WParam: NA
//LParam: a pointer of type PEI_RECT to indicate the area to be update

// ��ԾӦ���л� // active app switch
#define WM_EI_ACTIVATE WM_USER + 0x115
//WParam: �����ʾ��Ӧ�ñ���ʾ��ǰ̨��Ϊ���ʾ��Ӧ�ñ�ת����̨	
//WParam: none zero indicate this app is show at front , otherwise this app is switch to background .
//LParam: NA


// ������̬�л� // Lattop mode switch
#define WM_EI_LAPTOP_MODE_CHANGE WM_USER + 0x116
//WParam: ULONG GIR_MODE_LAPTOP/GIR_MODE_TENT/GIR_MODE_TABLET
//LParam: NA

// Homebar��̬�л� // Homebar mode switch
#define WM_EI_HOMEBAR_MODE_CHANGE WM_USER + 0x117
//WParam: ULONG GI_HOMEBAR_EXPAND/GI_HOMEBAR_COLLAPSE
//LParam: NA

// ������ʽ�л���� // Change Keyboard style to complete.
#define WM_EI_CHANGE_KEYBOARD_STYLE_COMPLETE WM_USER + 0x118
//WParam: BOOL true for success
//LParam: NA

// ��������Tp area // Reset TP area.
#define WM_EI_RESET_TP_AREA WM_USER + 0x119
//WParam: NA
//LParam: NA

//��˽����״̬�仯 //Privacy Status switch.
#define WM_EI_PRIVACY_CHANGE WM_USER + 0x120
//WParam: 0:off 1:on
//LParam: NA


#ifdef __cplusplus
extern "C" {
#endif


// ��ʼ��
// Initialize
// ��Ӧ����������Ҫ���ñ��ӿں�����Einkϵͳע�᱾Ӧ�á�
// After app startup , need to call this function to register itself to Eink system .
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻ERROR_ALREADY_EXISTS ��ʾӦ���Ѿ�������
// return
//		zero: success
//		non-zero: error code
DWORD EiAppStart(
	HWND hInforWindow	// ���ڽ��ձ�ϵͳ��Ϣ��Windows���ھ�� 
						// windows handle to receive eink message .
	);


// ���Eink�Ļ�����Ϣ
// get eink basic information .
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiGetSystemInfo(PEI_SYSTEM_INFO pstSystemInfo);


// ���APP����Ļ�趨��Ϣ
// get APP context indicated current screen settings
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
DWORD EiGetAppContext(PEI_APP_CONTEXT pstAppContext);

// ��û��ƻ���
// Get Drawing buffer .
// ͨ�����ñ��������һ�����ƻ���,һ��Ӧ��ʵ����������������ƻ���
// to get a draw-buffer that it represent pixels on the display area of Eink cover. There are two draw-buffers for each APP 
// ��APP����EiSetScreenOrient����Ҫ�ͷ�֮ǰ����Ļ��ƻ��壬Ȼ��ͨ�����ñ��������»�ȡ����
// If an APP call EiSetScreenOrient to change current display orientation, these buffers must be release and call this function to retrieve them.
// ���أ����ؿ�ָ��NULL����ʾʧ�ܣ���Ϊ��Դ�ľ������ط�NULLL����ʾ�ɹ�������ֵ�����½��Ļ���
// return
//		NULL : fail , mostly is run out of resource .
//		none NULL: success.
EI_BUFFER* EiGetDrawBuffer(
	BOOL bInit,		// �Ƿ񽫻�������
					// whether clear buffer content to zero .
	BOOL bCopy		// �Ƿ񽫵�ǰEink�����ݸ��Ƶ�����
					// will copy current content to buffer .
	);

// �ͷŻ��ƻ���
// release drawing content .
// ������ʹ��ʱ����Ҫ���ñ������ͷ���
// please call this function to release it .
void EiReleaseDrawBuffer( 
	EI_BUFFER* pstBuffer		// ָ����ƻ��� // point to drawing buffer .
	);

// �������ݵ�Eink��
// Draw an image to display-area in Eink-panel.
// �������е����ݻ��Ƶ�Eink��Ļ�ϣ����ô˺����󣬴���Ļ��ƻ���ᱻ�Զ��ͷţ�Ӧ�ò��ܼ���ʹ����黺��
// draw content in DRAWING buffer to eink display , after call this function ,DRAWING buffer will be release , and app should not use this buffer any more .
// ����x,y,w,h��ʾ��Ҫ���µ�����������ȫ�����ƻ��Ǿֲ����ƣ����ƻ����е�����ʼ�ն�Ӧ����������Ļ
// x,y,w,h indicate region need to be redraw , no matter full screen or partial screen redraw , DRAWING content will be for whole screen .
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiDraw(
	EI_RECT* pstArea,	// indicates the area to draw
	EI_BUFFER* pstBuffer		// image buffer
	);

// ���Eink��Ļ
// Clean up E-ink screen
// return:
//		na
void EiCleanupScreen(
	unsigned char ucBackgroundColor		// the background color of E-ink screen after cleaning up
	);

// ��ʹϵͳ����һ��WM_EI_DRAW��Ϣ����ǰAPP
// indicate system to generate an WM_EI_DRAW message to current app .
// ��дAPPʱ�����Խ����еĻ��ƶ����ڴ���WM_EI_DRAW��Ϣ�Ĺ����У����APP�ڴ���������һ�λ��Ʋ�����ֻ��Ҫ���ñ���������ʹϵͳ����һ��WM_EI_DRAW
// when you write an app , please place all drawing action in WM_EI_DRAW process function , if App want an redraw , can call this function , and system will generate an WM_EI_DRAW
// ��Ϣ��APP���Ӷ����ܹ������Ʋ���������һ���ط����У�WM_EI_DRAW������̣�
// message to app , so we should centralize all drawing action in one place.
void EiInvalidPanel(
	EI_RECT* pstArea	// indicates the area to update; set NULL for full panel
	);

// ����Handwritingģʽ
// Set handwriting mode .
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiSetHandWritingMode(
	DWORD eMode // Set the handwriting-mode, please refer to GIHW_HANDWRITING
	);

// ������Ļ��ʾ����
// set the screen orientation for current app
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiSetScreenOrient(
	DWORD eOrientation // refer to GIR_NONE
	);

// ����������ϵת������ʾ����ϵ
// Convert a point from physical-coordinate to display-coordinate
void EiPhysicalToDisplay(
	IN EI_POINT* pstPointP,
	OUT EI_POINT* pstPointD
	);

// ����ʾ����ϵת������������ϵ
// Convert a point from display-coordinate to physical-coordinate
void EiDisplayToPhysical(
	IN EI_POINT* pstPointD,
	OUT EI_POINT* pstPointP
);

// Set waveform mode for epd display, e.g. GI_WAVEFORM_DU2, GI_WAVEFORM_GC16, GI_WAVEFORM_GL16
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiSetWaveformMode(
	DWORD dwMode
	);

// Enable / disable direct handwriting region, which means handwriting is handled by tcon.
// To disable it, set both width and height = 0.
// Direct handwriting can only be used in GIHW_OWNER_DRAW mode.
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiSetHandwritingRect(
	EI_RECT dRect
);

// Check DisplayArea engine status.
//
// ���أ� ����TRUE����ʾ���С�����FALSE����ʾ���Ʊ�ռ��;
// return
//		TRUE: ready
//		FALSE: not ready
BOOL EiCheckDpyReady();

// Set PartialUpdate feature
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiSetPartialUpdate(
	BOOL enable
);


// Get scenario
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiGetScenario(DWORD& rdwCurrentMode);

// OOBE complete
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiOOBEComplete();

// Set Homebar status
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiSetHomebarStatus(
	ULONG lulStatus  //GI_HOMEBAR_HIDE/ GI_HOMEBAR_SHOW / GI_HOMEBAR_EXPAND / GI_HOMEBAR_COLLAPSE
);

// Set direct handwriting setting, e.g. line width, eraser
// Value 1~5
// Return value
//		zero: success
//		non-zero: error code
DWORD EiSetHandwritingSetting(BYTE lineWidth);

// ��ȡ��ǰ������̬
// Get tablet mode
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
//rdwCurrentMode : GIR_MODE_LAPTOP/GIR_MODE_TENT/GIR_MODE_TABLET
DWORD EiGetTabletMode(DWORD& rdwCurrentMode);


// ���ü�����ʽ
// Set keyboard style
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// �õ���ֱ�ӷ��ز��ȴ������յ� WM_EI_CHANGE_KEYBOARD_STYLE_COMPLETE ��Ϣʱ����ʾ�л����
// return
//		zero: success
//		non-zero: error code
DWORD EiSetKeyboardStyle(EI_KEYBOARD_STYLE ndStyle);

// ����C���ֻ������
// Set TP Area
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiSetTpArea(SET_TP_AREA ndTpArea);

// ������˽Э�鿪��״̬
// Set the privacy protocol switch status.
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
//rdwStatus : 0 off,1 on
DWORD EiSetPrivacyStatus(DWORD& rdwStatus);

// ��ȡ��˽Э�鿪��״̬
// Get the privacy protocol switch status.
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
//rdwStatus : 0 off,1 on
DWORD EiGetPrivacyStatus(DWORD& rdwStatus);

// �ж��Լ��Ƿ���ǰ̨����
// Is it a foreground window
//
// ���أ� �����㣬��ʾ�ɹ������ط��㣬��ʾ�����룻
// return
//		zero: success
//		non-zero: error code
DWORD EiIsForeground(bool& rbIsForeground);

// ֹͣ����		// stop service .
// ��һ��Ӧ��ʵ����Ҫ�˳�ʱ�������������
// when an app exit , please call this function.
void EiEnd(void);

// ��ȡ�û���ʾ����
// ���أ� ���ط��㣬��ʾ�ɹ��������㣬��ʾʧ�ܣ�
ULONG EiGetUserLagID(
	DWORD& rdwLagID
);



#ifdef __cplusplus
}
#endif




#pragma pack()



#endif//_EINKITEAPI_H_
