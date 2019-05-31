/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once
//�����������Ҫ����Ϣ

// �Զ�����Ϣ
#define EMSG_HB_TYPE_BASE	0xAE
#define EMSG_TS_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(EMSG_HB_TYPE_BASE,_MJ_NUM,_MN_NUM)

#define EI_POWER_ENTER_CONNECTED_STANDBY 0
#define EI_POWER_RETURN_CONNECTED_STANDBY 1
#define EI_POWER_ENTER_SHUTDOWN 2
// ��Դ�¼�
#define EEVT_HB_POWER_CHANGE		EMSG_TS_REQUEST(1,1)			
// input ULONG  �¼�ID������
// output none
// return none

// ������̬�仯
#define EEVT_HB_MODE_CHANGE		EMSG_TS_REQUEST(1,2)			
// input ULONG  ��̬ID
// output none
// return none

// �ػ���Ϊ
#define EEVT_HB_SHUT_DOWN	EMSG_TS_REQUEST(1,3)			
// NONE
// output none
// return none

// �û�session�¼�
#define EEVT_HB_SESSION_CHANGE		EMSG_TS_REQUEST(1,4)			
// input ULONG  wparam
// output none
// return none

// ǰ̨/��̨
#define EEVT_HB_ACTIVITE		EMSG_TS_REQUEST(1,5)			
// input ULONG  wparam 0��̨ 1ǰ̨
// output none
// return none

// ����֪ͨ�¼�
#define EEVT_HB_SERVICE_NOTIFY		EMSG_TS_REQUEST(1,6)			
// input ULONG  wparam ��ϢID
// output none
// return none

// ����֪ͨ������̬�����仯
#define EEVT_HB_LAPTOP_CHANGED		EMSG_TS_REQUEST(1,7)			
// input ULONG  ��̬���� GIR_MODE_LAPTOP/GIR_MODE_TENT/GIR_MODE_TABLET
// output none
// return none


// ѡ�������
#define EEVT_TS_SELECT		EMSG_TS_REQUEST(1,8)			
// input ULONG  ������
// output none
// return none

// ѡ��ļ�����ʽ
#define EEVT_KEYBOARD_SELECT		EMSG_TS_REQUEST(1,9)			
// input ULONG  ѡ��ĵڼ���
// output none
// return none


// ����ѡ��仯
#define EEVT_COVER_SELECT		EMSG_TS_REQUEST(1,10)			
// input ULONG  1��ʾͼƬ 2��ʾ���
// output none
// return none


// Ҫ��Ԥ��ͼƬ
#define EEVT_PICTURE_PREVIEW		EMSG_TS_REQUEST(1,11)			
// input wchar_t*  ͼƬ�����·��
// output none
// return none

// Ԥ��ͼƬ����OK��ť
#define EEVT_PICTURE_PREVIEW_EXIT		EMSG_TS_REQUEST(1,12)			
// input ULONG 1��ʾȷ�� 0��ʾȡ��
// output none
// return none

// ��ȡ�����ļ�ָ��
#define EEVT_GET_SETTING_FILE		EMSG_TS_REQUEST(1,13)			
// input none
// output XmlDocument*
// return none