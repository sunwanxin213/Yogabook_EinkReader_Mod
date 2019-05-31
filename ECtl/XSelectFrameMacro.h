/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _LW_SELECTFRAME_MACRO
#define _LW_SELECTFRAME_MACRO


#define ES_ETYPE_SELECT L"Select"
#define EMSG_SELECTFRAME_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(LMSG_TP_SELECT,_MJ_NUM,_MN_NUM)

#define EMSG_SELECTPOINT_MOVING	EMSG_SELECTFRAME_REQUEST(1,7)	//�����ƶ�ĳһ��ê��,���ƶ���ƫ����
// input D2D1_SIZE_F
// output none
// result none

#define EMSG_SELECTPOINT_RESET_CURSOR EMSG_SELECTFRAME_REQUEST(1,10)		//���õ�������ʽ
// input HCURSOR
// output none
// result none

#endif