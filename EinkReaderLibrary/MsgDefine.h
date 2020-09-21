/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once
//�����������Ҫ����Ϣ

// �Զ�����Ϣ
#define ER_TYPE_BASE	0xEA
#define EMSG_TS_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(ER_TYPE_BASE,_MJ_NUM,_MN_NUM)

// ���ļ��Ի���
#define EEVT_ER_OPEN_FILE		EMSG_TS_REQUEST(1,1)			
// input none
// output none
// return none

// ��ʾ��������ʾ
#define EEVT_ER_SHOW_TIP		EMSG_TS_REQUEST(1,2)			
// input none
// output none
// return none

// ǰ̨/��̨
#define EEVT_HB_ACTIVITE		EMSG_TS_REQUEST(1,3)			
// input ULONG  wparam 0��̨ 1ǰ̨
// output none
// return none

// list����
#define EEVT_ER_LIST_CLICK		EMSG_TS_REQUEST(1,4)			
// input wchat* ·��
// output none
// return none

// ��ҳ��Ի���
#define EEVT_ER_OPEN_PAGE_JUMP	EMSG_TS_REQUEST(1,5)			
// input none
// output none
// return none

// ���������棬��ת��ָ��ҳ��
#define EEVT_ER_PAGE_JUMP	EMSG_TS_REQUEST(1,6)			
// input int
// output none
// return none

// ���������棬Ҫ�򿪵��ļ�·��
#define EEVT_ER_OPEN_FILE_PATH	EMSG_TS_REQUEST(1,7)			
// input none
// output wchar_t* 
// return none

// homebar״̬�����仯
#define EEVT_ER_HOMEBAR_CHANGED	EMSG_TS_REQUEST(1,8)			
// input ULONG
// output none
// return none

//��ҳ��ť�����
#define EEVT_ER_PRE_NEXT_CLICKED	EMSG_TS_REQUEST(1,9)			
// input ULONG ���ĸ���ť����һҳ/��һҳ/�м���ʾ������
// output none
// return none

//���Ź�������ť�����
#define EEVT_ER_ZOOM_TOOLBAR_CLICKED	EMSG_TS_REQUEST(1,10)			
// input ULONG 
// output none
// return none

// ��������ģʽ
#define EEVT_ER_ENTER_ZOOM	EMSG_TS_REQUEST(1,11)			
// input bool true���룬false�˳�
// output none
// return none

// �������ű���
#define EEVT_ER_SET_ZOOM	EMSG_TS_REQUEST(1,12)			
// input float
// output RECT 4������0��ʾ�����ƶ��ˣ�1��ʾ�����ƶ� 
// return none

// ˫��������ʾ
#define EEVT_ER_TWO_SCREEN	EMSG_TS_REQUEST(1,13)			
// input bool true˫�� false����
// output none
// return none

// ����ҳ���ƶ�
#define EEVT_ER_SET_PAGE_MOVE	EMSG_TS_REQUEST(1,14)			
// input POINT ������ʾ�������ƶ���������ʾ�������ƶ�
// output RECT 4������0��ʾ�����ƶ��ˣ�1��ʾ�����ƶ� 
// return none

// �������ģʽ
#define EEVT_ER_ENTER_SNAPSHOT	EMSG_TS_REQUEST(1,15)			
// input none
// output none
// return none

// ���������а�
#define EEVT_ER_SNAPSHOT_TO_CLIPBRD	EMSG_TS_REQUEST(1,16)			
// input D2D1_RECT_F
// output none
// return none

// ����TXT���ű���
#define EEVT_ER_SET_TXT_ZOOM	EMSG_TS_REQUEST(1,17)			
// input DWORD
// output none
// return none

// ��ʼҳ���Ű�
#define EEVT_TXT_ARRANGED_START		EMSG_TS_REQUEST(1,20)			
// input none
// output none
// return none

// ȫ��ҳ���Ű��У�ÿ��ҳ�淢��һ��
#define EEVT_TXT_ARRANGED_DOING		EMSG_TS_REQUEST(1,21)			
// input ULONG  current count of pages loaded
// output none
// return none

// ȫ��ҳ���Ű���ϣ�ע�⣬���ܼ��衰�յ�EEVT_TXT_ARRANGED_START����һ�������յ�EEVT_ARRANGED_COMPLETE��Ϣ����ִ�й����п��ܱ����
#define EEVT_ARRANGED_COMPLETE		EMSG_TS_REQUEST(1,22)			
// input ULONG  count of all pages loaded
// output none
// return none

// �������ع�������ʱ��
#define EEVT_RESET_HIDE_TIME		EMSG_TS_REQUEST(1,23)			
// input bool true���ö�ʱ����false�رն�ʱ��
// output none
// return none


// ��ʾ�����ع�����
#define EEVT_SHOW_TOOLBAR		EMSG_TS_REQUEST(1,24)			
// input bool
// output none
// return none


// ����Ķ���ʷ
#define EEVT_DELETE_READ_HISTORY		EMSG_TS_REQUEST(1,25)			
// input none
// output none
// return none

// �˵�����
#define EEVT_MENU_ITEM_CLICKED		EMSG_TS_REQUEST(1,26)			
// input ULONG ��ťid
// output none


// �����ҳ��ע
#define EEVT_MENU_ERASER_ALL	EMSG_TS_REQUEST(1,27)			
// input none
// output none
// return none


// �����߿�
#define EEVT_SET_PEN_WIDTH	EMSG_TS_REQUEST(1,28)			
// input ULONG �ʿ������±�
// output none
// return none

// ������ɫ
#define EEVT_SET_PEN_COLOR	EMSG_TS_REQUEST(1,29)			
// input ULONG 0��ɫ 1��ɫ 2��ɫ
// output none
// return none

// �����¼�
#define EEVT_TOUCH_INPUT		EMSG_TS_REQUEST(1,29)			
// input ������Ϣ
// output none
// return none

// ���ñ�״̬
#define EEVT_PEN_MODE		EMSG_TS_REQUEST(1,30)			
// input ULONG
// output none
// return none

// �����ֻ��߿���
#define EEVT_HAND_WRITE		EMSG_TS_REQUEST(1,31)			
// input bool
// output none
// return none

// undo
#define EEVT_UNDO		EMSG_TS_REQUEST(1,32)			
// input none
// output none
// return none

// redo
#define EEVT_REDO		EMSG_TS_REQUEST(1,33)			
// input none
// output none
// return none

// ֪ͨ����������ҳ��״̬
#define EEVT_UPDATE_PAGE_STATUS		EMSG_TS_REQUEST(1,34)	
typedef struct _PAGE_STATUS {
	int RedoCount; //redo���м���
	int UndoCount; //undo���м���
	int InkCount; //�ж��ٱʼ�����
}PAGE_STATUS, *PPAGE_STATUS;

// input PAGE_STATUS
// output none
// return none


// ֪ͨ��������ҳ�뷢���仯
#define EEVT_PAGE_CHANGED		EMSG_TS_REQUEST(1,35)			
// input none
// output none
// return none

// �ļ����޸�
#define EEVT_FILE_MODIFY		EMSG_TS_REQUEST(1,36)			
// input none
// output none
// return none

// ���������棬����ʷ��¼�е��ļ�
#define EEVT_ER_OPEN_HISTORY_FILE_PATH	EMSG_TS_REQUEST(1,37)			
// input ULONT
// output bool 
// return none

// ���û�������
#define EEVT_SET_FW_LINE_RECT	EMSG_TS_REQUEST(1,38)			
// input ED_RECT
// output none 
// return none

// ��ȡҳ���ƶ�״̬��ť
#define EEVT_GET_MOVE_BUTTON_STATUS	EMSG_TS_REQUEST(1,39)			
// input none
// output RECT 
// return none

// ��ʾ�����ط�ҳ������
#define EEVT_SHOW_PAGE_PROCESS	EMSG_TS_REQUEST(1,40)			
// input bool
// output none 
// return none

// ��Ĭ��ʽ�����ֻ��߿��أ�����toast
#define EEVT_HAND_WRITE_MUTE		EMSG_TS_REQUEST(1,41)			
// input bool
// output none
// return none

// highlight toolbar��ť�¼�
#define EEVT_HIGHLIGHT_BT_EVENT		EMSG_TS_REQUEST(1,42)			
// input int
// output none
// return none
//��ť����
#define HIGHLIGHT_BT_COPY 1
#define HIGHLIGHT_BT_HIGHLIGHT 2
#define HIGHLIGHT_BT_DELETE_LINE 3
#define HIGHLIGHT_BT_UNDER_LINE 4
#define HIGHLIGHT_BT_DELETE 5
#define HIGHLIGHT_BT_TRANSLATE 6

// ѡ��ָ�������ڵ�����
#define EEVT_SELECT_HIGHLIGHT		EMSG_TS_REQUEST(1,43)			
// input D2D1_RECT_F
// output SELECT_HIGHLIGHT
// return none
typedef struct _SELECT_HIGHLIGHT {
	int QuadCount; //�м���ѡ�п�
	int AnnotCount; //�м������и����Ķ���
	D2D1_POINT_2F PointA;
	D2D1_POINT_2F PointB;
}SELECT_HIGHLIGHT, *PSELECT_HIGHLIGHT;

// ��ȡѡ��������Ϣ
#define EEVT_GET_SELECT_HIGHLIGHT_INFO		EMSG_TS_REQUEST(1,44)			
// input none
// output SELECT_HIGHLIGHT
// return none


// ����ҳ�淽��
#define EEVT_SET_SCREEN_STATUS	EMSG_TS_REQUEST(1,45)			
// input ULONG
// output none
// return none
#define SCREEN_STATUS_AUTO 10 //�Զ�
#define SCREEN_STATUS_H 11	//����
#define SCREEN_STATUS_V 12	//����	


// �򿪻�ر�partaul
#define EEVT_PARTIAL_ENABLE		EMSG_TS_REQUEST(1,46)			
// input bool
// output none
// return none


// ��������ͼ����
#define EEVT_ENTER_THUMBNAIL_DLG	EMSG_TS_REQUEST(1,47)			
// input none
// output none
// return none

//����ͼĳҳ�����
#define EEVT_THUMBNAIL_CLICK	EMSG_TS_REQUEST(1,48)			
// input int
// output none
// return none

// ����ҳ�淽��
#define EEVT_THUMBNAIL_SELECT	EMSG_TS_REQUEST(1,49)			
// input ULONG
// output none
// return none
#define THUMBNAIL_SELECT_ALL 1
#define THUMBNAIL_SELECT_ANNOT 2

// ȫ������ͼ�������
#define EEVT_THUMBNAIL_COMPLETE		EMSG_TS_REQUEST(1,50)			
// input ULONG  count of all pages loaded
// output none
// return none

// ����ͼĿ¼����
#define EEVT_UPDATE_THUMBNAIL_PATH		EMSG_TS_REQUEST(1,51)			
// input wchat*
// output none
// return none

// �򿪻�ر�B����Ļ
#define EEVT_CLOSE_B_SCREEN	EMSG_TS_REQUEST(1,52)			
// input bool true���� falseϨ��
// output none
// return none

// ������̬�ı仯
#define EEVT_HB_MODE_CHANGE		EMSG_TS_REQUEST(1,53)			
// input ULONG  wparam :
// GIR_MODE_LAPTOP  2
// GIR_MODE_TENT 3
// GIR_MODE_TABLET 4
// output none
// return none

// [zhuhl5@20200116:ZoomRecovery]
// ֪ͨ��ǰ���ű�����λ
#define EEVT_ER_SET_ZOOMLEVEL	EMSG_TS_REQUEST(1, 54)			
// input int
// output none
// return none

// [zhuhl5@20200116:ZoomRecovery]
// ת������Office�ĵ�
#define EEVT_CONVERT_OPEN_OFFICE_FILE EMSG_TS_REQUEST(1, 55)			
// input int
// output none
// return none
// ȡ������ѡ��
#define EEVT_HIDE_HIGHLIGHT		EMSG_TS_REQUEST(1,56)			
// input none
// output none
// return none


// ��ȫ��
#define EEVT_CLEAR_FULL_SCREEN	EMSG_TS_REQUEST(1,57)			
// input none
// output none
// return none

// �Զ�������Ļ
#define EEVT_ER_AUTO_ZOOM EMSG_TS_REQUEST(1,58)

//�ĵ�����
#define DOC_TYPE_PDF 0
#define DOC_TYPE_EPUB 1
#define DOC_TYPE_MOBI 2
#define DOC_TYPE_TXT 3

//����ɫ
#define PEN_COLOR_BLACK 0
#define PEN_COLOR_RED 1
#define PEN_COLOR_BLUE 2

//��״̬
#define PEN_MODE_NONE 0
#define PEN_MODE_PEN 1
#define PEN_MODE_ERASER 2
#define PEN_MODE_SELECT 3