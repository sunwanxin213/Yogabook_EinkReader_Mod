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
