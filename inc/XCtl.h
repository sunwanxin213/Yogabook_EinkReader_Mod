/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */
#ifndef _ECTL_H_
#define _ECTL_H_

/*
	��DLL���XUIĬ�Ͽؼ������еĿؼ������ⶨ��
*/






#pragma pack(4)

// ͨ�ú궨��
#define ES_SHOW_TEXT_MAX_LEN 200   //�ṹ���ã���ʾ�������Buffer����




//////////////////////////////////////////////////////////////////////////
//Ԫ�����������ʽ
//////////////////////////////////////////////////////////////////////////
//Ԫ��������ID��������������ES_ETYPE_ǰ׺����������͵����ƣ����е���չ������ES_ETYPE_��ͷ������������չ������Ȼ��������������: ES_ETYPE_STOCK_CHART���Ϳ������Ϊĳ����Ʊ��չģ���ͼ��ؼ�
//Ԫ�����ʵ�������ݽṹ�����ƹ���ST_ELALC_ǰ׺��������������͵�����;�磺ST_ELALC_ELEMENT
//Ԫ���ഥ������Ϣ������ʱ����EEVT_ǰ׺
//Ԫ������ܵķ�����Ϣ������ʱ����EACT_ǰ׺

//#define ES_ETYPE_DIRECTORPH L"DirectorPh"
//#define ES_ETYPE_TURN3D L"Turn3D"

//////////////////////////////////////////////////////////////////////////
#define ES_ETYPE_BUTTON L"Button"			//��ť

//��ť��ʾ����
//struct _STEMS_BUTTON_TEXT{
//	wchar_t Text[ES_SHOW_TEXT_MAX_LEN];	    //��ť��ʾ����
//};
//typedef _STEMS_BUTTON_TEXT STEMS_BUTTON_TEXT,*PSTEMS_BUTTON_TEXT;	


//��ť�ؼ��ᴥ������Ϣ
#define EMSG_BUTTON_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(LMSG_TP_BUTTON,_MJ_NUM,_MN_NUM)

//��ť������
#define EEVT_BUTTON_PRESSED		EMSG_BUTTON_REQUEST(1,1)
//Input			TEMS_MSG_BASE �ṹ��
//Output		None
//Result

//���±��ͷ�
#define EEVT_BUTTON_RELEASE		EMSG_BUTTON_REQUEST(1,2)
//Input			TEMS_MSG_BASE �ṹ��
//Output		None
//Result

//�����µİ�ť�����������뿪�˰�ť���壬�Ӷ�����Release��Ϊ����������¼�
#define EEVT_BUTTON_ESCAPED		EMSG_BUTTON_REQUEST(1,3)
//Input			TEMS_MSG_BASE �ṹ��
//Output		None
//Result

//�����밴ť��Ч��
#define EEVT_BUTTON_MOUSE_IN	EMSG_BUTTON_REQUEST(1,4) 
//Input			IEinkuiIterator* �ð�ť
//Output		None
//Result

//����Ƴ���ť��Ч��
#define EEVT_BUTTON_MOUSE_OUT	EMSG_BUTTON_REQUEST(1,5) 
//Input			IEinkuiIterator* �ð�ť
//Output		None
//Result

//��ť������
#define EEVT_BUTTON_CLICK		EMSG_BUTTON_REQUEST(1,6)
//Input			None
//Output		None
//Result

//�Զ��嶯���������
#define EEVT_BUTTON_PLAYED		EMSG_BUTTON_REQUEST(1,7)
//Input			LONG ������ɵĶ������
//Output		None
//Result


//��ť��ѡ��
#define EEVT_BUTTON_CHECKED		EMSG_BUTTON_REQUEST(1,8)
//Input			None
//Output		None
//Result

//��ť��ȡ��ѡ��״̬
#define EEVT_BUTTON_UNCHECK		EMSG_BUTTON_REQUEST(1,9)
//Input			None
//Output		None
//Result


//��ť�ؼ�����յ���Ϣ
//���ð�ť����
#define EACT_BUTTON_SETTEXT EMSG_BUTTON_REQUEST(2,1)
//Input			wchat_t* Ҫ�ı�İ�ť����
//Output		None
//Result

//��ȡ��ť����
#define EACT_BUTTON_GETTEXT EMSG_BUTTON_REQUEST(2,2)
//Input			None
//Output		wchat_t** ���ص��ַ���ָ�벻Ӧ�ó��ڳ��У������ͷ�
//Result

//���ð�ťCheck״̬
#define EACT_BUTTON_SET_CHECKED EMSG_BUTTON_REQUEST(2,3)	
//Input			bool
//Output		None
//Result

//��ȡ��ťCheck״̬
#define EACT_BUTTON_GET_CHECKED EMSG_BUTTON_REQUEST(2,4)
//Input			None
//Output		bool
//Result

//���Ű�ť���Զ��嶯��
#define EACT_BUTTON_PLAY_OTHER_ANIMATION EMSG_BUTTON_REQUEST(2,5)
//Input			LONG,ָ�����ż��Ŷ���
//Output		NONE
//Result

//��������ͼƬ,���·��
#define EACT_BUTTON_CHANGE_PIC EMSG_BUTTON_REQUEST(2,6)
//Input			wchat_t* ͼƬ�����DLL����Ŀ¼��·��
//Output		NONE
//Result

//��������ͼƬ,ȫ·��
#define EACT_BUTTON_CHANGE_PIC_FULLPATH EMSG_BUTTON_REQUEST(2,7)
//Input			wchat_t* ͼƬ��ȫ·��
//Output		NONE
//Result

//���ü�������
#define EACT_BUTTON_SET_ACTION_RECT EMSG_BUTTON_REQUEST(2,8)
//Input			D2D1_SIZE_F Ҫ���õļ��������С
//Output		NONE
//Result


//////////////////////////////////////////////////////////////////////////
#define ES_ETYPE_RADIO_BUTTON_GROUP L"RadioButtonGroup"			//��ѡ��ť��
#define EMSG_RADIO_BUTTON_GROUP_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(LMSG_TP_RADIO_BUTTON_GROUP,_MJ_NUM,_MN_NUM)

//��ѡ������ᴥ������Ϣ
// ��ѡ�еĶ���ı�
#define EEVT_RBG_SELECTED_CHANGED EMSG_RADIO_BUTTON_GROUP_REQUEST(1,1)
//Input			ULONG ��ѡ�ж����ID���ظ�ѡ��һ�����󣬲����ظ����ʹ���Ϣ
//Output		NONE
//Result		NONE

// ��ѡ�еĶ����ٴε��
#define EEVT_RBG_SELECTED_ITEM_CLICK		 EMSG_RADIO_BUTTON_GROUP_REQUEST(1,2)
//Input			ULONG ����������ID��
//Output		NONE
//Result		NONE


//��ѡ��ť�����յ���Ϣ
//����ĳ��ѡ��
#define EACT_RBG_SET_SELECT EMSG_RADIO_BUTTON_GROUP_REQUEST(2,1)
//Input			ULONG Ҫ��ѡ�ж����ID
//Output		NONE
//Result		NONE

//��ȡ��ǰѡ�е���
#define EACT_RBG_GET_SELECT EMSG_RADIO_BUTTON_GROUP_REQUEST(2,2)
//Input			none
//Output		ULONG	��ǰ��ѡ�еĶ���ID
//Result		NONE

//��ĳ�����
#define EACT_RBG_DISABLE EMSG_RADIO_BUTTON_GROUP_REQUEST(2,3)
//Input			ULONG Ҫ��ѡ�ж����ID
//Output		NONE
//Result		NONE

//��ĳ������
#define EACT_RBG_ENABLE EMSG_RADIO_BUTTON_GROUP_REQUEST(2,4)
//Input			ULONG Ҫ��ѡ�ж����ID
//Output		NONE
//Result		NONE

//////////////////////////////////////////////////////////////////////////
#define ES_ETYPE_PICTUREFRAME L"PictureFrame"	//ͼƬԪ��
#define EMSG_PICTUREFRAME_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(LMSG_TP_PICTUREFRAME,_MJ_NUM,_MN_NUM)


//ͼƬԪ�ػᴥ������Ϣ

//ͼƬԪ�ػ���յ���Ϣ

//�л���ʾ֡
#define EACT_PICTUREFRAME_SET_INDEX EMSG_PICTUREFRAME_REQUEST(2,1)
//Input			LONG,ָ����ʾ�ڼ�֡
//Output		NONE
//Result

//������ʾͼƬ,���·��
#define EACT_PICTUREFRAME_CHANGE_PIC EMSG_PICTUREFRAME_REQUEST(2,2)
//Input			wchat_t* ͼƬ�����DLL����Ŀ¼��·��������ֻ�ܸ���Ϊֻ��1֡��ͼ
//Output		NONE
//Result

//������ʾͼƬ,ȫ·��
#define EACT_PICTUREFRAME_CHANGE_PIC_FULLPATH EMSG_PICTUREFRAME_REQUEST(2,3)
//Input			wchat_t* ͼƬ��ȫ·��������ֻ�ܸ���Ϊֻ��1֡��ͼ
//Output		NONE
//Result

//////////////////////////////////////////////////////////////////////////
#define ES_ETYPE_LIST L"List"	//ListԪ��

//����list����ʽ 
//smallItem ������SmallIcon
#define	LIST_VIEW_STYLE_SMALLITEM  0
#define	LIST_VIEW_STYLE_REPORT 1

//�����Զ�����ģʽ����ģʽ����˼���б�������Ķ��ٶ��Զ��䳤�ͱ��
//��ģʽ������ֹ�����
#define LIST_VIEW_STYLE_AUTO_FIT_X 2
#define LIST_VIEW_STYLE_AUTO_FIT_Y 3

//����List���ӿؼ�ID��List��1��2 ��ռ�ã������ؼ��������SCROLLBAR_SUBID_BASE
#define V_SCROLL_BAR_ID 1
#define H_SCROLL_BAR_ID 2
#define INSERT_MARK_ID  3
#define LIST_SUBID_BASE 10

#define EMSG_LIST_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(LMSG_TP_LIST,_MJ_NUM,_MN_NUM)



//�л���ʾ֡
#define EACT_LIST_SET_PIC_INDEX EMSG_LIST_REQUEST(2,1)
//Input			LONG,ָ����ʾ�ڼ�֡
//Output		NONE
//Result

//������ʾͼƬ
#define EACT_LIST_CHANGE_PIC EMSG_LIST_REQUEST(2,2)
//Input			wchat_t* ͼƬ��ȫ·��������ֻ�ܸ���Ϊֻ��1֡��ͼ
//Output		NONE
//Result

//Item�����Ϣ
#define EACT_LIST_ITEMCLICK EMSG_LIST_REQUEST(2,3)
//Input			Item ָ��
//Output		NONE
//Result

//��list�������Ϣ����ǿ��list�Ĺ�����λ��
#define EACT_LIST_DOCSCROLL EMSG_LIST_REQUEST(2,4)
//Input			FLOAT ��position��
//Output		NONE
//Result



//����list����ʾģʽ
#define EACT_LIST_SET_STYLE EMSG_LIST_REQUEST(2,5)
//Input			STYLE(long)
//Output		NONE
//Result
//��ȡ��ʾģʽ��Ϣ
#define EACT_LIST_GET_STYLE EMSG_LIST_REQUEST(2,6)
//Input			NONE
//Output		STYLE(long)
//Result

//��β�����Ԫ��
#define EACT_LIST_ADD_ELEMENT EMSG_LIST_REQUEST(2,7)
//Input			LONG (CXuiElement*)
//Output		NONE
//Result


//ɾ��Ԫ��
#define EACT_LIST_DELETE_ELEMENT EMSG_LIST_REQUEST(2,8)
//Input			LONG (CXuiElement*)
//Output		NONE
//Result

//ɾ������Ԫ�أ���������LIST
#define EACT_LIST_RESET EMSG_LIST_REQUEST(2,9)
//Input			NONE
//Output		NONE
//Result

//ɾ����������Ԫ�أ���������LIST
#define EACT_LIST_DELETE_ELEMENT_INDEX EMSG_LIST_REQUEST(2,10)
//Input			Index
//Output		NONE
//Result

//֪ͨList�ǲ�����Ҫ���æ�ͷ���Ԫ��
#define EACT_LIST_SET_MEMORY_MANAGER EMSG_LIST_REQUEST(2,11)
//Input			bool
//Output		NONE
//Result
//��ͷ�����Ԫ��
#define EACT_LIST_ADD_ELEMENT_HEAD EMSG_LIST_REQUEST(2,12)
//Input			LONG (CXuiElement*)
//Output		NONE
//Result
//֪ͨ�����ڹ�������ʾ������
#define EACT_LIST_VSCROLLBAR_SHOW EMSG_LIST_REQUEST(2,13)
//Input			bool (true for show /false for hide)
//Output		NONE
//Result

//��ȡ������Χ
#define EACT_LIST_GET_SCROLL_RANGE EMSG_LIST_REQUEST(2,14)
//Input			NONE
//Output		FLOAT 
//Result
//��ȡ��ǰ����λ��
#define EACT_LIST_GET_SCROLL_POSITION EMSG_LIST_REQUEST(2,15)
//Input			NONE
//Output		FLOAT 
//Result

//����list�ĸ����ڣ�˵����List���ڹ���
#define EACT_LIST_SCROLLING EMSG_LIST_REQUEST(2,16)
//Input			FLOAT��POSITION��
//Output		
//Result


//������������������List�����ڵ�λ�úʹ�С
#define EACT_LIST_SET_VSCROLLBAR_RECT EMSG_LIST_REQUEST(2,17)
//Input			D2D1_RectF��rect��
//Output		
//Result

//����list Itemλ��
#define EACT_LIST_GET_ITEM_POSITION EMSG_LIST_REQUEST(2,18)
//Input			Item Iterator
//Output		FLOAT��POSITION��
//Result
//֪ͨList ���¼��������λ�úͲ���
#define EACT_LIST_RECACULATE EMSG_LIST_REQUEST(2,19)
//Input			Item Iterator
//Output		FLOAT��POSITION��
//Result
//֪ͨList�ĸ����� list�Ĳ��ַ����˱仯
#define EACT_LIST_LAYOUT_CHANGE EMSG_LIST_REQUEST(2,20)
//Input			Item Iterator
//Output		FLOAT��POSITION��
//Result
//����list Itemλ������ 0 1 2 3
#define EACT_LIST_GET_ITEM_INDEX EMSG_LIST_REQUEST(2,21)
//Input			Item Iterator
//Output		int��index��
//Result


//��list Itemλ������ 0 1 2 3 ����
#define EACT_LIST_INSER_ITEM_INDEX EMSG_LIST_REQUEST(2,22)
//Input			_STCTL_LIST_INSERT
//Output		int��index��
//Result
struct _STCTL_LIST_INSERT{
	int mnIndex;		// index
	IEinkuiIterator*  mpElement;	//�����IEinkuiIterator
	
};
//ɾ��index���Ԫ��
#define EACT_LIST_DELETE_ELEMENT_BIGGER_OR_EQUAL_INDEX EMSG_LIST_REQUEST(2,23)
//Input			Index
//Output		NONE
//Result

static const GUID EGUID_LIST_DROP_ITEM = 
{ 0x81efb6da, 0xea4b, 0x4e5f, { 0xa4, 0xcd, 0x9c, 0xa4, 0x3e, 0x6c, 0x45, 0xf0 } };
//���������Ƿ����ITEM DROP����Ϣ����Ҫ������ͬ���͵�����֮�����ק
#define EACT_LIST_SET_ACCEPT_DROP_ITEM EMSG_LIST_REQUEST(2,24)
//Input			bool
//Output		NONE
//Result

//��ʼ�ƶ�Ԫ��
#define EACT_LIST_DRAG_ITEM_START EMSG_LIST_REQUEST(2,25)
//Input			Iterator*
//Output		NONE
//Result

//��ʼ�ƶ�Ԫ��
#define EACT_LIST_DRAG_ITEM_DRAGING EMSG_LIST_REQUEST(2,26)
//Input			STMS_DRAGGING_ELE
//Output		NONE
//Result

//�����ƶ�Ԫ��
#define EACT_LIST_DRAG_ITEM_END EMSG_LIST_REQUEST(2,27)
//Input			Iterator*
//Output		NONE
//Result

//���ø�������ƶ���Iterator
#define EACT_LIST_SET_DRAG_FOR_MOUSER_ITERATOR EMSG_LIST_REQUEST(2,28)
//Input			Iterator*
//Output		NONE
//Result
//���ò����ʶIterator
#define EACT_LIST_SET_DROP_MARK_ITERATOR EMSG_LIST_REQUEST(2,29)
//Input			Iterator*
//Output		NONE
//Result


struct _STCTL_LIST_ITEM_CLICK{
	int mnFlag;                   //������
	D2D1_POINT_2F mPosWorld;		// �������λ��
	IEinkuiIterator*  mpElement;	//�����IEinkuiIterator

};
//Item�Ҽ������Ϣ
#define EACT_LIST_ITEMCLICK_RBUTTON EMSG_LIST_REQUEST(2,30)
//Input			_STCTL_LIST_ITEM_CLICK
//Output		NONE
//Result
//ɾ��index���Ԫ��,���ǲ�Close��ЩԪ��
#define EACT_LIST_DELETE_ELEMENT_BIGGER_OR_EQUAL_INDEX_NO_CLOSE EMSG_LIST_REQUEST(2,31)
//Input			Index
//Output		NONE
//Result

struct _STCTL_LIST_SWAP{
	IEinkuiIterator*  mpFromElement;		// ����FROM
	IEinkuiIterator*  mpToElement;	//����TO

};
//����Ԫ��
#define EACT_LIST_SWAP_ELEMENT EMSG_LIST_REQUEST(2,32)
//Input			_STCTL_LIST_SWAP
//Output		NONE
//Result

//ɾ��Ԫ�ص��ǲ�Close
#define EACT_LIST_DELETE_ELEMENT_NO_CLOSE EMSG_LIST_REQUEST(2,33)
//Input			IEinkuiIterator
//Output		NONE
//Result

//����list Item������
#define EACT_LIST_GET_ITEM_NUM EMSG_LIST_REQUEST(2,34)
//Input			NULL 
//Output		int����
//Result

//��ָ�����ƶ�������ʾ����
#define EACT_LIST_SHOW_BY_INDEX EMSG_LIST_REQUEST(2,35)
//Input			LONG 
//Output		NULL
//Result

//��ͼ
//#define EACT_PICTUREFRAME_CHANGE_PIC EMSG_BUTTON_REQUEST(2,2)
////Input			LONG,ָ����ʾ�ڼ�֡
//struct _STEMS_PICTURE_TEXT{
//	wchar_t Text[ES_SHOW_TEXT_MAX_LEN];	    //��ť��ʾ����
//};
//typedef _STEMS_BUTTON_TEXT STEMS_BUTTON_TEXT,*PSTEMS_BUTTON_TEXT;	
//
////Output		NONE
////Result

// ��������Ϣ

//////////////////////////////////////////////////////////////////////////
#define ES_ETYPE_STATICTEXT L"StaticText"	//�ı�����
#define EMSG_STATICTEXT_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(LMSG_TP_STATICTEXT,_MJ_NUM,_MN_NUM)
//�ı�Ԫ�ػᴥ������Ϣ


//�ı�Ԫ�ؽ��յ���Ϣ
//������ʾ����
#define EACT_STATICTEXT_SET_TEXT EMSG_STATICTEXT_REQUEST(2,1)
//Input			wchat* Ҫ��ʾ�����ı�
//Output		NONE
//Result

//��ȡ��ʾ����
#define EACT_STATICTEXT_GET_TEXT EMSG_STATICTEXT_REQUEST(2,2)
//Input			none
//Output		wchat_t** ���ص��ַ���ָ�벻Ӧ�ó��ڳ��У������ͷ�
//Result


//�ı�Ԫ�ص���ɫ
//������ʾ����
#define EACT_STATICTEXT_SET_TEXT_COLOR EMSG_STATICTEXT_REQUEST(2,3)
//Input			RGBA(LONG)
//Output		NONE
//Result

//////////////////////////////////////////////////////////////////////////
#define ES_ETYPE_SLIDERBUTTON L"SliderButton"	//������ť
#define EMSG_SLIDERBUTTON_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(LMSG_TP_SLIDERBTN,_MJ_NUM,_MN_NUM)

//����Sliderbutton��style��
#define ES_SLIDER_BUTTON_STYLE_HOR 1
#define ES_SLIDER_BUTTON_STYLE_VER 2
#define ES_SLIDER_BUTTON_STYLE_ANYWAY 3

//���ظ�������ק��Ϣ
#define EACT_SLIDERBUTTON_DRAG_START EMSG_SLIDERBUTTON_REQUEST(2,1)
#define EACT_SLIDERBUTTON_DRAG_END EMSG_SLIDERBUTTON_REQUEST(2,2)
#define EACT_SLIDERBUTTON_DRAGING EMSG_SLIDERBUTTON_REQUEST(2,3)
//Input			STMS_DRAGGING_ELE
//output        NONE
//Result

//����sliderbutton�Ĺ������򣬲����������style��
#define EACT_SLIDERBUTTON_SET_STYLE EMSG_SLIDERBUTTON_REQUEST(2,4)
//Input			nStyle(LONG)
//Output		NONE
//Result

//����sliderbutton�Ĺ�������
#define EACT_SLIDERBUTTON_SET_SLIDERRECT EMSG_SLIDERBUTTON_REQUEST(2,5)
//Input			D2D1_RECT_F
//Output		NONE
//Result

//////////////////////////////////////////////////////////////////////////
#define ES_ETYPE_SCROLLBAR L"ScrollBar"	//������
#define EMSG_SCROLLBAR_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(LMSG_TP_SCROLLBAR,_MJ_NUM,_MN_NUM)

//���ظ������������λ��+1 �� -1 ��Ϣ
#define EACT_SCROLLBAR_VSCROLL EMSG_SCROLLBAR_REQUEST(2,1)
//Input			0/UP �� 1/DOWN
//Output		NONE
//Result

//���ظ����ڣ�����������������λ��
#define EACT_SCROLLBAR_VSCROLL_THUMB_POSITION EMSG_SCROLLBAR_REQUEST(2,2)
//Input			fPositin(FLOAT)
//Output		NONE
//Result

//���ظ����ں��������λ��+1 �� -1 ��Ϣ
#define EACT_SCROLLBAR_HSCROLL EMSG_SCROLLBAR_REQUEST(2,2)
//Input			0/Lefe �� 1/Right
//Output		NONE
//Result

//���ظ����ڣ����������������λ��
#define EACT_SCROLLBAR_HSCROLL_THUMB_POSITION EMSG_SCROLLBAR_REQUEST(2,4)
//Input			fPositin(FLOAT)
//Output		NONE
//Result

//�����������������λ��
#define EACT_SCROLLBAR_VSCROLL_SET_SLIDER_POSTION EMSG_SCROLLBAR_REQUEST(2,5)
//Input			fPositin(FLOAT)
//Output		NONE
//Result

//���ú������������λ��
#define EACT_SCROLLBAR_HSCROLL_SET_SLIDER_POSTION EMSG_SCROLLBAR_REQUEST(2,6)
//Input			fPositin(FLOAT)
//Output		NONE
//Result

//���ù������Ĺ�����Χ
#define EACT_SCROLLBAR_HVSCROLL_SET_DELTA_SIZE EMSG_SCROLLBAR_REQUEST(2,7)
//Input			fSize(FLOAT)
//Output		NONE
//Result

//��ȡ����������ĸ߶�
#define EACT_SCROLLBAR_HSCROLL_GET_HEIGTH EMSG_SCROLLBAR_REQUEST(2,8)
//Input			NULL
//Output		fSize(FLOAT)
//Result

//��ȡ����������Ŀ��
#define EACT_SCROLLBAR_VSCROLL_GET_WIDTH EMSG_SCROLLBAR_REQUEST(2,9)
//Input			NULL
//Output		fSize(FLOAT)
//Result

//Relactio������
#define EACT_SCROLLBAR_HVSCROLL_RELACATION EMSG_SCROLLBAR_REQUEST(2,10)
//Input			NULL
//Output		NULL
//Result

//ģ�������°�ť
#define EACT_SCROLLBAR_BT_CLICK EMSG_SCROLLBAR_REQUEST(2,11)
//Input			bool 
//Output		NULL
//Result


//////////////////////////////////////////////////////////////////////////
#define ES_ETYPE_SLIDERBAR L"SliderBar"	//������
#define EMSG_SLIDERBAR_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(LMSG_TP_SLIDEREBAR,_MJ_NUM,_MN_NUM)


// ������ı��˻����λ��
#define EACT_SLIDERBAR_THUMB_CLICK EMSG_SLIDERBAR_REQUEST(2,1)
//Input			fPositin(FLOAT)
//Output		NONE
//Result


//���û��鷶Χ,��СĬ����0��Input �������Χ
#define EACT_SLIDERBAR_SET_RANGE EMSG_SLIDERBAR_REQUEST(2,2)
//Input			fPositin(FLOAT)�����Χ
//Output		NONE
//Result

//���û���λ�ã�������Range ������λ��
#define EACT_SLIDERBAR_SET_POSITION EMSG_SLIDERBAR_REQUEST(2,3)
//Input			fPositin(FLOAT)��
//Output		NONE
//Result

//��ʼ�϶�����
#define EACT_SLIDERBAR_DRAG_START EMSG_SLIDERBAR_REQUEST(2,4)
//Input			fPositin(FLOAT)
//Output		NONE
//Result

//�����϶�������
#define EACT_SLIDERBAR_DRAGING EMSG_SLIDERBAR_REQUEST(2,4)
//Input			fPositin(FLOAT)
//Output		NONE
//Result

//�����϶�����
#define EACT_SLIDERBAR_DRAG_END EMSG_SLIDERBAR_REQUEST(2,5)
//Input			fPositin(FLOAT)
//Output		NONE
//Result

//////////////////////////////////////////////////////////////////////////
#define ES_ETYPE_EDIT L"Edit"	//�༭��

// �༭��ؼ����ܴ�����Ϣ ����������������������������������������������������������������������������������������

// ���ݱ��޸�
#define EEVT_EDIT_CONTENT_MODIFIED EMSG_DEFINE(LMSG_TP_EDIT,1,1)
// input none
// output none
// result na

// �����޸���ɣ�����ʱ����"Enter"���������Ƴ����̽��㣬�����������������Ϣ������
// Ӧ���յ�"��������"��Ϣ�����µ������޸ģ����ᷢ��������Ϣ
#define EEVT_EDIT_CONTENT_COMPLETION EMSG_DEFINE(LMSG_TP_EDIT,1,5)
// input wchar[] the content string with a tail UNICODE_NULL
// output none
// result na

// ��꽹��仯
#define EEVT_EDIT_MOUSE_FOCUS EMSG_DEFINE(LMSG_TP_EDIT,2,1)
// input LONG  zero if the focus is lost,nonzero if we get the mousefocus
// output none
// result na

// ����϶��������������κ�һ����ťȻ���ڽ������϶�
#define EEVT_EDIT_DRAGING	EMSG_DEFINE(LMSG_TP_EDIT,2,5)
// input none
// output none
// result na

// ���˫��
#define EEVT_EDIT_MOUSE_DCLICK	EMSG_DEFINE(LMSG_TP_EDIT,2,11)
// input LONG the number of the character was double-clicked
// oputput none
// result na

// ���̽���仯
#define EEVT_EDIT_KEYBOARD_FOCUS	EMSG_DEFINE(LMSG_TP_EDIT,3,1)
// input LONG  zero if the focus is lost,nonzero if we get the keyboardf
// output none
// result na

// ������Ϊ����Ԫ�ؽ��յ�������Ϣ�����ϣ���ؼ���Ҫ�������������Ϊ���뷵��ERESULT_DISCARD
#define EEVT_EDIT_KEY_STIKE		EMSG_DEFINE(LMSG_TP_EDIT,3,5)
// intput STEMS_KEY_PRESSED
// output none
// result ERESULT_DISCARD to discard the operation

// �ַ����룬��Ԫ�ؽ��յ�������Ϣ�����ϣ���ؼ���Ҫ��������ַ�������Ϊ���뷵��ERESULT_DISCARD
#define EEVT_EDIT_CHAR_INPUT	EMSG_DEFINE(LMSG_TP_EDIT,3,10)
// intput STEMS_CHAR_INPUT
// output none
// result ERESULT_DISCARD to discard the operation



// �༭��ؼ���������Ϣ ��������������������������������������������������������������������������������������������

// �����ı�����, must be send
#define EACT_EDIT_SET_TEXT EMSG_DEFINE(LMSG_TP_EDIT,20,1)
// input wchar[] must have the tail UNICODE_NULL and no more than 512 characters
// output none
// result ERESULT_SUCCESS or ERESULT_UNSUCCESSFUL

// ֻ��������
#define EACT_EDIT_NUMBER_ONLY EMSG_DEFINE(LMSG_TP_EDIT,20,5)
// input LONG, nonzero indicate that the edit shalln't accept characters except these are numbers '0' to '9'; zero set back to normal mode
// output none
// result na

// ������ʾ
#define EACT_EDIT_PASSWORD_MODE EMSG_DEFINE(LMSG_TP_EDIT,20,6)
// intput LONG, nonzero inidcate that the edit shall draw every charactor as '*';zero set back to normal mode 
// output none
// result na

// ��������
#define EACT_EDIT_SET_LENGTH_LIMIT EMSG_DEFINE(LMSG_TP_EDIT,20,7)
// input LONG, the limit of characters the edit can save. < 0 to cancel the limit
// output none
// result na


// ����ı����ݣ�must be send
#define EACT_EDIT_GET_TEXT EMSG_DEFINE(LMSG_TP_EDIT,21,1)
// intput none
// output wchar[] buffer to receive the text
// result return ERESULT_INSUFFICIENT_RESOURCES if output buffer is short than the text to obtain. 

// ����ı����ݰ������ַ���������������β��NULL
#define EACT_EDIT_GET_TEXT_LENGTH	EMSG_DEFINE(LMSG_TP_EDIT,21,2)
// input none
// output ULONG
// result ERESULT_SUCCESS

// ��ȡѡ����Ϣ
#define EACT_EDIT_GET_SELECTION EMSG_DEFINE(LMSG_TP_EDIT,22,1)
// intput none
// output
struct _STCTL_EDIT_SELECTION{
	LONG Chars;		// �ؼ��ڵ��ַ�������������β'\0'����������ϢEACT_EDIT_SET_SELECTIONʱ��Ч
	LONG SelBegin;	// ѡ����������ַ�������0��ʾ�ؼ��еĵ�һ���ַ���û���ַ�ѡ��ʱ�����壻
	LONG SelCount;	// ѡ��������ַ�����0��ʾû���ַ���ѡ�У�-1��ʾȫ��ѡ��		add by colin
};
typedef _STCTL_EDIT_SELECTION STCTL_EDIT_SELECTION,* PSTCTL_EDIT_SELECTION;
// success or failure

// ����ѡ��
#define EACT_EDIT_SET_SELECTION EMSG_DEFINE(LMSG_TP_EDIT,22,2)
// intput STCTL_EDIT_SELECTION
// output none
// success or failure







//////////////////////////////////////////////////////////////////////////
#define ES_ETYPE_LABEL L"Label"	//��ǩ

// ��ǩ�ؼ����ܴ�����Ϣ��ֻ�������˳��������ԲŻᴥ��������Ϣ ����������������������������������������������������������������������������������������

// ��꽹��仯
#define EEVT_LABEL_MOUSE_FOCUS EMSG_DEFINE(LMSG_TP_LABEL,1,1)
// input LONG  zero if the focus is lost,nonzero if we get the mousefocus
// output none
// result na

// OPEN�������������������ǻ�ü��̽����ǰ��¿ո��
#define EEVT_LABEL_OPEN	EMSG_DEFINE(LMSG_TP_LABEL,1,2)
// input none
// oputput none
// result na

// ���̽���仯
#define EEVT_LABEL_KEYBOARD_FOCUS	EMSG_DEFINE(LMSG_TP_LABEL,2,1)
// input LONG  zero if the focus is lost,nonzero if we get the keyboardf
// output none
// result na

// ������Ϊ����Ԫ�ؽ��յ�������Ϣ�����ϣ���ؼ���Ҫ�������������Ϊ���뷵��ERESULT_DISCARD
#define EEVT_LABEL_KEY_STIKE		EMSG_DEFINE(LMSG_TP_LABEL,2,2)
// intput STEMS_KEY_PRESSED
// output none
// result ERESULT_DISCARD to discard the operation



// ��ǩ�ؼ���������Ϣ ��������������������������������������������������������������������������������������������

// �����ı�����, must be send
#define EACT_LABEL_SET_TEXT EMSG_DEFINE(LMSG_TP_LABEL,20,1)
// input wchar[] must have the tail UNICODE_NULL and no more than 512 characters
// output none
// result ERESULT_SUCCESS or ERESULT_UNSUCCESSFUL

// ����ı����ݣ�must be send
#define EACT_LABEL_GET_TEXT EMSG_DEFINE(LMSG_TP_LABEL,21,1)
// intput none
// output wchar[] buffer to receive the text
// result return ERESULT_INSUFFICIENT_RESOURCES if output buffer is short than the text to obtain. 

// ����ı����ݰ������ַ���������������β��NULL
#define EACT_LABEL_GET_TEXT_LENGTH	EMSG_DEFINE(LMSG_TP_LABEL,21,2)
// input none
// output ULONG
// result ERESULT_SUCCESS

// ��������ǰ����ɫ
#define EACT_LABEL_SET_FORE_COLOR EMSG_DEFINE(LMSG_TP_LABEL,21,5)
// intput ULONG color in 4 bytes as b,g,r,a
// output none
// result ERESULT_SUCCESS or ERESULT_WRONG_PARAMETERS or ERESULT_UNSUCCESSFUL

// �������屳����ɫ
#define EACT_LABEL_SET_BACK_COLOR EMSG_DEFINE(LMSG_TP_LABEL,21,6)
// input  ULONG color in 4 bytes as b,g,r,a
// output none
// result ERESULT_SUCCESS or ERESULT_WRONG_PARAMETERS or ERESULT_UNSUCCESSFUL

// �������ǰ����ɫ
#define EACT_LABEL_GET_FORE_COLOR EMSG_DEFINE(LMSG_TP_LABEL,21,7)
// input none
// output  ULONG color in 4 bytes as b,g,r,a
// result ERESULT_SUCCESS or ERESULT_WRONG_PARAMETERS or ERESULT_UNSUCCESSFUL

// ������屳����ɫ
#define EACT_LABEL_GET_BACK_COLOR EMSG_DEFINE(LMSG_TP_LABEL,21,8)
// input none
// output  ULONG color in 4 bytes as b,g,r,a
// result ERESULT_SUCCESS or ERESULT_WRONG_PARAMETERS or ERESULT_UNSUCCESSFUL

// �����������»���
#define EACT_LABEL_SET_UNDERLINE EMSG_DEFINE(LMSG_TP_LABEL,21,10)
// input LONG, nonzero to set underline with the text,zero to remove the underline
// output none
// result na

// ���ó���������
#define EACT_LABEL_SET_HYPERLINK EMSG_DEFINE(LMSG_TP_LABEL,21,11)
// input LONG, nonzero to set the hyperlink attribute,zero to remove the hyperlink attribute
// output none
// result na

// ��������Ų���Ϣ
#define EACT_LABEL_GET_LAYOUT EMSG_DEFINE(LMSG_TP_LABEL,21,15)
// input none
//output 
struct _STCTL_LABEL_LAYOUT{
	D2D1_SIZE_F MaxSize;			// �������������
	D2D1_RECT_F Background;			// ��������
	D2D1_POINT_2F TextPos;			// ������������Ͻ�
	D2D1_SIZE_F TextSize;			// ��������Ŀ��
	LONG VisibleChars;				// �ɼ��ַ����������������滻��'.'
	LONG Lines;						// �ɼ�����
};
typedef _STCTL_LABEL_LAYOUT STCTL_LABEL_LAYOUT,* PSTCTL_LABEL_LAYOUT;
// result ERESULT_SUCCESS or ERESULT_WRONG_PARAMETERS or ERESULT_UNSUCCESSFUL


// �������
#define EACT_LABEL_SET_MAX_WIDTH EMSG_DEFINE(LMSG_TP_LABEL,21,16)
// input LONG
// output none
// result na

// �������
#define EACT_LABEL_SET_MAX_HEIGHT EMSG_DEFINE(LMSG_TP_LABEL,21,17)
// input LONG
// output none
// result na


//////////////////////////////////////////////////////////////////////////
#define ES_ETYPE_ANIMATOR L"Animator"	//��ǩ

// �����ؼ��Ĵ�����Ϣ��������������������������������������������������������������������������������������������

// ׼��������Ϣ���ø������л�������������ɰ棬�յ�����Ϣ�󣬲�Ҫִ���������룬�������ɰ�����
#define EEVT_ANIMATOR_BEFORE_PAINT EMSG_DEFINE(LMSG_TP_ANIMATOR,1,1)
// input IXuiPaintBoard*
// output none
// result na

// ����������Ϣ���ø������л��᳷���������ɰ棬�յ�����Ϣ�󣬲�Ҫִ���������룬�������ɰ�����
#define EEVT_ANIMATOR_AFTER_PAINT EMSG_DEFINE(LMSG_TP_ANIMATOR,1,2)
// input IXuiPaintBoard*
// output none
// result na


// �����ؼ���������Ϣ��������������������������������������������������������������������������������������������

// ���¶���֡, must be send
#define EACT_ANIMATOR_SET_FRAME EMSG_DEFINE(LMSG_TP_ANIMATOR,20,1)
// input 
struct _STCTL_ANIMATOR_FRAME{
	ULONG PixelWidth;	// must be product of 4
	ULONG PixelHeight;
	PBYTE PixelData;	// the format must be RGB24
};
typedef _STCTL_ANIMATOR_FRAME STCTL_ANIMATOR_FRAME,* PSTCTL_ANIMATOR_FRAME;
// output none
// result ERESULT_SUCCESS or ERESULT_UNSUCCESSFUL


// ����ͼ��任, ��δʵ��
#define EACT_ANIMATOR_SET_TRANSFORM EMSG_DEFINE(LMSG_TP_ANIMATOR,21,1)
// input
struct _STCTL_ANIMATOR_TRANSFORM{
	ULONG Flag;				// ������ĺ궨�壬�����ǲ�ͬFlag�����
	FLOAT RotationAngle;	// ��ת�Ƕȣ�����˳ʱ��
	D2D1_POINT_2F RotationCenter;	// ��ת����
};
typedef _STCTL_ANIMATOR_TRANSFORM STCTL_ANIMATOR_TRANSFORM,* PSTCTL_ANIMATOR_TRANSFORM;
#define EFLAG_ANIMATOR_HORIZONTALFLIP 1 // ���ҷ�ת
#define EFLAG_ANIMATOR_VIRTICALFLIP 2 // ���·�ת
#define EFLAG_ANIMATOR_ROTATION	4		// �趨��ת�ĽǶ�
#define EFLAG_ANIMATOR_ROTATION_CENTER  8 // �趨��ת������ 
// output none
// result ERESULT_SUCCESS or ERESULT_UNSUCCESSFUL


//////////////////////////////////////////////////////////////////////////
// ѡ���
//////////////////////////////////////////////////////////////////////////

// NOTE:
// 1��ѡ����λ�úʹ�С����Ҫ������ȥ���á�
// 2����������ڲ����ã���ôѡ����λ�úʹ�С��͸������غϡ�ѡ������ȫ���ո����ڵ���С���Σ���ȫ��ס������


#define ES_ETYPE_SELECT L"Select"
#define EMSG_SELECTFRAME_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(LMSG_TP_SELECT,_MJ_NUM,_MN_NUM)


//////////////////////////////////////////////////////////////////////////
// �������ƶ�ѡ������Ϣ
//////////////////////////////////////////////////////////////////////////

#define EMSG_SELECTFRAME_BEGIN		EMSG_SELECTFRAME_REQUEST(1,2)		//ѡ���ʼ�϶�
// input  none
// output none
// result none
#define EMSG_SELECTFRAME_DRAGED		EMSG_SELECTFRAME_REQUEST(1,3)		//ѡ����϶����
// input none
// output none
// result none

#define EMSG_SELECTFRAME_MOVING		EMSG_SELECTFRAME_REQUEST(1,4)		// �ƶ�ѡ��򣬽�ƫ�ƴ��ݵ�����
// input D2D1_POINT_2F
// output none
// result none

// �յ�������Ϣ�������������Ҫʶ�����뷵��ERESULT_KEY_ACCEPTED����������ֵ��ѡ������н��͸ü�����Ϣ��Ŀǰ���ܽ��͵��Ƿ�������µ�ѡ����ƶ�
#define EMSG_SELECTFRAME_KEYBOARD	EMSG_SELECTFRAME_REQUEST(2,1)		
// input STEMS_KEY_PRESSED refer to EMSG_KEY_PRESSED message's definition
// output none
// result na

//////////////////////////////////////////////////////////////////////////
// ���µ��ƶ�ê�����Ϣ
//////////////////////////////////////////////////////////////////////////

// NOTE:�������ƶ�ĳ��ê��ʱ��ѡ�����Ҫ���͸�������һ����Ԫ��{positionƫ����,size�仯��, �Ƿ�����ת},
// �������յ���Ϣ�󣬸��������߼��ж��Ƿ���Ҫ������ת������ѡ�������͹�һ�������������������ѡ����λ�úʹ�С��

#define EMSG_SELECTPOINT_CHANGE_POSITION_SIZE		EMSG_SELECTFRAME_REQUEST(10,1)	// �ƶ�ĳ��ê�㣬�򸸴��ڷ�����Ϣ
// input PSTCTL_CHANGE_POSITION_SIZE
// output none
// result none

#define EMSG_SELECTPOINT_BEGIN	EMSG_SELECTFRAME_REQUEST(10,5)		//��ʼ�ƶ�ĳһ��ê��
// input none
// output none
// result none

#define EMSG_SELECTFPOINT_MOVED	 EMSG_SELECTFRAME_REQUEST(10,10)		//ĳ��ê���ƶ����
// input none
// output none
// result none

#define EMSG_SELECTFRAME_REGULAR		EMSG_SELECTFRAME_REQUEST(10,12)		// �����ڸ��Ӵ��ڷ��͹�һ������
// input none
// output none
// result none


#define EMSG_SET_PROPORTIONAL_SCALING		EMSG_SELECTFRAME_REQUEST(10,15)		// ������֪ͨ�Ӵ��ڣ���Ҫǿ�Ƶȱ�����
// input bool��true��ʾ��Ҫ�ȱ����ţ�false��ʾ����Ҫ
// output none
// result none

#define EMSG_SET_EDIT_STATUS				EMSG_SELECTFRAME_REQUEST(10,16)		// ��ʾ�Ƿ�����༭
// input bool��true��ʾ����༭��false��ʾ������
// output none
// result none


enum XuiSelectFrameActivePoint{
	XuiLeftTop = 1,		// ���ϵ�
	XuiMidTop = 2,		// ���ϵ�
	XuiRightTop = 3,		// ���ϵ�
	XuiRightMid = 4,		// ���е�
	XuiRightBottom = 5,	// ���µ�
	XuiMidBottom = 6,	// ���µ�
	XuiLeftBottom = 7,	// ���µ�
	XuiLeftMid = 8		// ���е�
};

typedef  struct _STCTL_CHANGE_POSITION_SIZE{
	D2D1_POINT_2F	mdPositionOffset;	// λ��ƫ����
	D2D1_SIZE_F		mdSizeVariation;	// ��С�仯��
	bool			mcHTurn;			// �ж��Ƿ�ˮƽ��ת
	bool			mcVTurn;			// �ж��Ƿ�ֱ��ת
}STCTL_CHANGE_POSITION_SIZE,*PSTCTL_CHANGE_POSITION_SIZE;


//////////////////////////////////////////////////////////////////////////
#define ES_ETYPE_MENUITEM L"MenuItem"	// �˵���

#define EMSG_MENUITEM_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(LMSG_TP_MENUITEMN,_MJ_NUM,_MN_NUM)

// �˵��������Ϣ����ֱ�ӷ��͸�PopupMenu��PopupMenu��MenuButton��MenuBar���յ������Ϣ����ֱ������������
// ��������������������������������������������������������������������������������������������

// �˵�����
#define EEVT_MENUITEM_CLICK						EMSG_MENUITEM_REQUEST(1,1)
// input LONG, ������Ĳ˵����ID

// �˵����ȡ����
#define EEVT_MENUITEM_GET_FOCUS					EMSG_MENUITEM_REQUEST(1,2)
// input Iterator** ��ȡ����Ĳ˵���

// �˵��������ͣ
#define EEVT_MENUITEM_MOUSE_HOVER				EMSG_MENUITEM_REQUEST(1,3)
// input STCTL_MENUITEM_MOUSE_HOVER
// output none
// result none

typedef struct _STCTL_MENUITEM_MOUSE_HOVER{
	UINT CommandID;
	IEinkuiIterator* MenuItemIter;
}STCTL_MENUITEM_MOUSE_HOVER,*PSTCTL_MENUITEM_MOUSE_HOVER;


// �˵��������Ϣ
// ���ò˵����Check״̬
#define EACT_MENUITEM_SET_CHECK_STATE			EMSG_MENUITEM_REQUEST(2,1)
// input bool �Ƿ�����Check״̬

// ���Ĳ˵����ı�
#define EACT_MENUITEM_CHANGE_TEXT				EMSG_MENUITEM_REQUEST(2,2)
// input wchar_t*			�µĲ˵��ı�

// �����ȼ�(����ȷ���˵������ı���
#define EACT_MENUITEM_CHANGE_HOTKEY				EMSG_MENUITEM_REQUEST(2,3)
// PSTCTL_MENUITEM_HOTKEY

struct _STCTL_MENUITEM_HOTKEY
{
	wchar_t HotKeyToShow[MAX_PATH];		// ������ʾ���ȼ�����
	ULONG VirtualKey;					// �����ֵ
	bool NeedShift;						// �Ƿ���ҪShift
	bool NeedCtrl;						// �Ƿ���ҪCtrl
	bool NeedAlt;						// �Ƿ���ҪAlt
};

typedef _STCTL_MENUITEM_HOTKEY STCTL_MENUITEM_HOTKEY, *PSTCTL_MENUITEM_HOTKEY;





//////////////////////////////////////////////////////////////////////////
#define ES_ETYPE_POPUPMENU L"PopupMenu"	

#define EMSG_POPUPMENU_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(LMSG_TP_POPUPMENU,_MJ_NUM,_MN_NUM)

// �����˵�������Ϣ
// ��������������������������������������������������������������������������������������������

// �����µĲ˵��ָ��λ��
#define EACT_POPUPMENU_INSERT_MENUITEM_BY_CREATE								EMSG_POPUPMENU_REQUEST(1,1)
// input STCTL_POPUPMENU_MENUITEMINFO
struct _STCTL_POPUPMENU_MENUITEMINFO{
	LONG UniqueMenuItemId;				// MenuItem��ȫ��ΨһID
	int Index;							// �˵����λ��������-1��ʾ��ֵĩβ��
	LONG MenuItemId;					// MenuItem��PopupMenu�µ�ID��Ϊ0��ʾ����ID
	UINT Type;							// �˵�������ͣ�����MenuItemö�ٵ����ͣ�Ŀǰ��֧��1����
	wchar_t MenuText[MAX_PATH];			// �˵�����ʾ������
	PSTCTL_MENUITEM_HOTKEY* HotKeyInfo;	// �ȼ���Ϣ
};
typedef _STCTL_POPUPMENU_MENUITEMINFO STCTL_POPUPMENU_MENUITEMINFO,* PSTCTL_POPUPMENU_MENUITEMINFO;


// ����˵��ָ��λ��
#define EACT_POPUPMENU_INSERT_MENUITEM_BY_EXIST								EMSG_POPUPMENU_REQUEST(1,2)
// input PSTCTL_POPUPMENU_MENUITEMINSERT

struct _STCTL_POPUPMENU_MENUITEMINSERT{
	IEinkuiIterator* MenuItem;
	int Index;							// Ҫ���������λ��
};
typedef _STCTL_POPUPMENU_MENUITEMINSERT STCTL_POPUPMENU_MENUITEMINSERT,* PSTCTL_POPUPMENU_MENUITEMINSERT;


// ɾ��ָ��CommandID�Ĳ˵���
#define EACT_POPUPMENU_DELETE_MENUITEM_BY_COMMANDID					EMSG_POPUPMENU_REQUEST(1,4)
// input LONG commandid

// ɾ��ָ�������Ĳ˵����������-1��ʾɾ��ȫ���˵���
#define EACT_POPUPMENU_DELETE_MENUITEM_BY_INDEX						EMSG_POPUPMENU_REQUEST(1,5)
// input LONG index

// ��ȡָ��CommandID�Ĳ˵��������
#define EACT_POPUPMENU_GET_MENUITEM_BY_COMMANDID					EMSG_POPUPMENU_REQUEST(1,6)
// input LONG commandId
// output Iterator*

// ��ȡָ�������Ĳ˵���ĵ�����
#define EACT_POPUPMENU_GET_MENUITEM_BY_INDEX						EMSG_POPUPMENU_REQUEST(1,7)
// input  LONG index
// output Iterator* 

// Ҫ�����²��ֲ˵���
#define EACT_POPUPMENU_RELAYOUT_MENUITEM							EMSG_POPUPMENU_REQUEST(1,8)


// ������ʾ��ʱ����Ҫȥ����˵��������/���á� �ò˵�������ȥ����
#define EACT_POPUPMENU_IS_MANAGER_MENUITEM_ENABLE					EMSG_POPUPMENU_REQUEST(1, 9)
// input bool 

// �����˵���������Ϣ��
// ��������������������������������������������������������������������������������������������



//////////////////////////////////////////////////////////////////////////
#define ES_ETYPE_MENUBUTTON L"MenuButton"		// �˵���ť

#define EMSG_MENUBUTTON_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(LMSG_TP_MENUBUTTON,_MJ_NUM,_MN_NUM)

// �˵���ť��������Ϣ����ֱ�ӷ��͸�������
// ��������������������������������������������������������������������������������������������

// �˵���ť�����
#define EEVT_MENUBUTTON_CLICK					EMSG_MENUBUTTON_REQUEST(1,1)
// input bool ��ǰ����Ĳ˵���ť�Ƿ񵯳��Ӳ˵�


// �˵���ť������Ϣ
// ��������������������������������������������������������������������������������������������
// �����Ӳ˵�����״̬
#define EEVT_MENUBUTTON_SET_SUBMENU_VISIBLE		EMSG_MENUBUTTON_REQUEST(2,1)
// input bool ����״̬

// ����˵��PopupMenu��������MenuItem��PopupMenu)
#define EEVT_MENUBUTTON_INSERT_MENUITEM			EMSG_MENUBUTTON_REQUEST(2,2)
// input STCTL_MENUBUTTON_INSERT_MENUITEM
struct _STCTL_MENUBUTTON_INSERT_MENUITEM{
	LONG UniquePopupMenuId;				// PopupMenu��ȫ��ΨһID
	STCTL_POPUPMENU_MENUITEMINFO PopupMenuInfo;
};
typedef struct _STCTL_MENUBUTTON_INSERT_MENUITEM STCTL_MENUBUTTON_INSERT_MENUITEM,* PSTCTL_MENUBUTTON_INSERT_MENUITEM;





//////////////////////////////////////////////////////////////////////////
#define ES_ETYPE_MENUBAR L"MenuBar"		// �˵���ť

#define EMSG_MENUBAR_REQUEST(_MJ_NUM,_MN_NUM)	EMSG_DEFINE(LMSG_TP_MENUBAR,_MJ_NUM,_MN_NUM)

// �˵���������Ϣ
// ��������������������������������������������������������������������������������������������

// ѯ�ʵ�ǰ�Ƿ��������˵���ť��չ��
#define EACT_MENUBAR_ANY_SUBMENU_VISIBLE		EMSG_MENUBAR_REQUEST(1,1)
// output  bool, ture��ʾ��������ť��չ����false��ʾû�С�

// �����������һ�ε������Ӳ˵�
#define EACT_MENUBAR_HIDE_LAST_SUBMENU			EMSG_MENUBAR_REQUEST(1,2)


// Ҫ�󴴽��µ�MenuItem���뵽ָ����PopupMenu��
#define EACT_MENUBAR_INSERT_NEW_MENUITEM		EMSG_MENUBAR_REQUEST(1,3)
// input _STCTL_MENUBAR_INSERT_MENUITEM
// ����ֵ ERESULT_SUCCESS ��ʾ�ɹ��������ʾʧ��

struct _STCTL_MENUBAR_INSERT_MENUITEM{
	UINT MenuButtonID;				// Ҫ�����MenuButtonID
	STCTL_MENUBUTTON_INSERT_MENUITEM MenuButtonInfo;
};
typedef _STCTL_MENUBAR_INSERT_MENUITEM STCTL_MENUBAR_INSERT_MENUITEM,* PSTCTL_MENUBAR_INSERT_MENUITEM;


// ��ȡָ��CommandID�ĵ����˵����������ɹ�����SUCCESS������ʧ��
#define EACT_MENUBAR_GET_POPUPMENU_BY_COMMANDID	EMSG_MENUBAR_REQUEST(1, 4)
// long commandid
// output Iterator* 





//////////////////////////////////////////////////////////////////////////

#define ES_ETYPE_IMAGEBUTTON	L"ImageButton"			//ImageButtonԪ��

//����ImageButton����ʽ 
#define	IB_STYLE_NORMAL				0		//��ͨ���
#define	IB_STYLE_KEEP_CHECKED		1		//����ѡ�а��·��

//����ImageButton���ӿؼ�ID�������ؼ�������ڵ��� IB_ID_CTRL_OTHERS
#define IB_ID_CTRL_LEFT_PICTURE				1
#define IB_ID_CTRL_RIGHT_PICTURE			2

#define IB_ID_CTRL_OTHERS					3	

//��ť�ؼ��ᴥ������Ϣ
#define EMSG_IMAGEBUTTON_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(LMSG_TP_IMAGEBUTTON,_MJ_NUM,_MN_NUM)

//��ť������
#define EEVT_IMAGEBUTTON_PRESSED		EMSG_IMAGEBUTTON_REQUEST(1,1)
//Input			TEMS_MSG_BASE �ṹ��
//Output		None
//Result

//��ť���ͷ�
#define EEVT_IMAGEBUTTON_RELEASE		EMSG_IMAGEBUTTON_REQUEST(1,2)
//Input			TEMS_MSG_BASE �ṹ��
//Output		None
//Result

//��ť������
#define EEVT_IMAGEBUTTON_CLICK			EMSG_IMAGEBUTTON_REQUEST(1,3)
//Input			None
//Output		None
//Result

//��ť�����������밴��״̬
#define EEVT_IMAGEBUTTON_CHECKED		EMSG_IMAGEBUTTON_REQUEST(1,4)
//Input			None
//Output		None
//Result

//��ť��������ȡ������״̬
#define EEVT_IMAGEBUTTON_UNCHECKED		EMSG_IMAGEBUTTON_REQUEST(1,5)
//Input			None
//Output		None
//Result

//�ı���ɫѡ��ť����ɫֵ��ֻ����ɫѡ��ť��Ч��
#define EACT_IMAGEBUTTON_SET_COLOR		EMSG_IMAGEBUTTON_REQUEST(1,6)
//Input			ULONG ����ɫֵ��Ҫ���Ƶ���ɫ�����Aͨ��Ϊ0���򲻻��� 0xARGB 
//Output		None
//Result

//�ı���ͼ�ı���ͼ�����·����
#define EACT_IMAGEBUTTON_CHANGE_LEFT_IMAGE_BKG		EMSG_IMAGEBUTTON_REQUEST(1,7)
//Input			wchat_t* ͼƬ�����DLL����Ŀ¼��·��������ֻ�ܸ���Ϊֻ��1֡��ͼ
//Output		NONE
//Result

//�ı���ͼ�ı���ͼ��ȫ·����
#define EACT_IMAGEBUTTON_CHANGE_LEFT_IMAGE_BKG_FULL_PATH		EMSG_IMAGEBUTTON_REQUEST(1,8)
//Input			wchat_t* ͼƬȫ·��������ֻ�ܸ���Ϊֻ��1֡��ͼ
//Output		NONE
//Result

//���ð�ťcheck״̬
#define EACT_IMAGEBUTTON_SET_CHECKED							EMSG_IMAGEBUTTON_REQUEST(1,9)
//Input			bool
//Output		None
//Result

//���ð�ť�����ű�������ֹ��ť����
#define EACT_IMAGEBUTTON_SET_RATIO								EMSG_IMAGEBUTTON_REQUEST(1,10)
//Input			Float ��������
//Output		NONE
//Result

//���ð�ť�Ӳ˵�ĳ�ѡ��
#define EACT_IMAGEBUTTON_SET_ITEM_SELECTED						EMSG_IMAGEBUTTON_REQUEST(1,11)
//Input			int ����ID�� ��1��ʼ��
//Output		NONE
//Result

//������ɫ��ť���ƾ���״̬�Ŀ�����ر�
#define EACT_IMAGEBUTTON_DRAW_SHAPE						EMSG_IMAGEBUTTON_REQUEST(1,12)
//Input			bool ��true: �����Զ�����״ false�������ƣ�
//Output		NONE
//Result

//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////

#define ES_ETYPE_COMBOBOX L"ComboBox"			//ComboBoxԪ��

//����ComboBox����ʽ 
#define	COMBO_STYLE_EDIT		0	//���б༭�����Ͽ�
#define	COMBO_STYLE_STATIC		1	//���о�̬�ı��ؼ�����Ͽ�

//����ComboBox���ӿؼ�ID�������ؼ�������ڵ��� COMBO_ID_CTRL_OTHERS
#define COMBO_ID_CTRL_UPPER_PICTURE				1
#define COMBO_ID_CTRL_UNDER_PICTURE				2
#define COMBO_ID_CTRL_CURRENT_ITEM_EDIT			3
#define COMBO_ID_CTRL_CURRENT_ITEM_BUTTON		4
#define COMBO_ID_CTRL_DROP_DOWN_BUTTON			5
#define COMBO_ID_CTRL_LIST						6

#define COMBO_ID_CTRL_OTHERS					7	


#define EMSG_COMBOBOX_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(LMSG_TP_COMBO, _MJ_NUM, _MN_NUM)

// ���ѡ��
#define EACT_COMBOBOX_ADD_ITEM						EMSG_COMBOBOX_REQUEST(1,1)
//Input			wchar_t*  ѡ���ַ���
//Output		None
//Result


//	�����б�����
#define EEVT_COMBOBOX_LIST_ITEM_CLICK				EMSG_COMBOBOX_REQUEST(1,2)
//Input			int (IDֵ�������ļ���ָ����
//Output		None
//Result

//	�����б�����
#define EEVT_COMBOBOX_LIST_ITEM_CLICK_COMPLEX			EMSG_COMBOBOX_REQUEST(1,3)
//Input			int (IDֵ�������ļ���ָ����
//Output		None
//Result

// ĳ����ʱ�����ͱ�ѡ������ı�����			// add by colin
#define EEVT_COMBOBOX_ITEM_CLICK_WITH_TEXT			EMSG_COMBOBOX_REQUEST(1,4)
// Input wchar_t*
// output none
// result

// ĳ����ʱ�����ͱ�ѡ���������				// add by colin
#define EEVT_COMBOBOX_ITEM_CLICK_WITH_INDEX			EMSG_COMBOBOX_REQUEST(1,5)
// Input ULONG*
// output none
// result

//����ToolBar��Ϣ�ṹ
enum COMBOBOX_MSG_PARA_TYPE
{
	COMBOBOX_TMPT_NONE,
	COMBOBOX_TMPT_BOOL,
	COMBOBOX_TMPT_INT,
	COMBOBOX_TMPT_FLOAT,
	COMBOBOX_TMPT_STRING,
	COMBOBOX_TMPT_OTHERS			//Never Use
};
struct COMBOBOX_MSG
{
	unsigned int				mnCtrlID;			//ToolBar�ӿؼ� ID
	COMBOBOX_MSG_PARA_TYPE		mnMsgParaType;		//��Ϣ���ͱ�ʶ
	void*						mpMsgBuf;			//��ϢЯ�������ݻ�����

	COMBOBOX_MSG() : mnCtrlID(0x00000000), mnMsgParaType(COMBOBOX_TMPT_OTHERS), mpMsgBuf(0) {}
	~COMBOBOX_MSG() { mpMsgBuf = 0;}
};

//���ð�ť�Ӳ˵�ĳ�ѡ��
#define EACT_COMBOBOX_SET_ITEM_SELECTED						EMSG_COMBOBOX_REQUEST(2,4)
//Input			int ����ID��
//Output		NONE
//Result

//���������˵�ĳһ����Ч
#define EACT_COMBOBOX_SET_ENABLE							EMSG_COMBOBOX_REQUEST(2,5)
//Input			int ����ID��
//Output		NONE
//Result

//���������˵�ĳһ����Ч
#define EACT_COMBOBOX_SET_DISABLE							EMSG_COMBOBOX_REQUEST(2,6)
//Input			int ����ID��
//Output		NONE
//Result

// ������ɾ���� ,���븺����ʾɾ��ȫ�� add by colin
#define EACT_COMBOBOX_DELETE_ITEM_BY_INDEX			EMSG_COMBOBOX_REQUEST(2,7)
//Input			int ����
//Output		None
//Result

// ��ȡ��ǰѡ������ʾ�ı� add by colin
#define EACT_COMBOBOX_GET_CURRENT_ITEM_TEXT			EMSG_COMBOBOX_REQUEST(2,8)
// input none
//Output		wchat_t** ���ص��ַ���ָ�벻Ӧ�ó��ڳ��У������ͷ�
// result return ERESULT_INSUFFICIENT_RESOURCES if output buffer is short than the text to obtain. 

// ������������ĳ�ѡ�� add by colin
#define EACT_COMBOBOX_SET_ITEM_SELECTED_BY_INDEX						EMSG_COMBOBOX_REQUEST(2,9)
//Input			ULONG ��index)
//Output		NONE
//Result

//////////////////////////////////////////////////////////////////////////
//BiSliderBar��Ϣ
#define EMSG_BI_SLIDERBAR_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(LMSG_TP_BI_SLIDERBAR, _MJ_NUM, _MN_NUM)
struct _sBiSliderBarStruct
{
	float mfLeftPos;
	float mfRightPos;
	float mfMidPos;
};
//SLiderBar
#define EACT_BISLIDERBAR_THUMB_POSITION						EMSG_BI_SLIDERBAR_REQUEST(1,1)
//Input			NULL
//Output		_sBiSliderBarStruct
//Result

//SLiderBar
#define EACT_BISLIDERBAR_SET_POS						EMSG_BI_SLIDERBAR_REQUEST(1,2)
//Input			_sBiSliderBarStruct
//Output		NULL
//Result

//SLiderBar
#define EACT_BISLIDERBAR_GET_POS						EMSG_BI_SLIDERBAR_REQUEST(1,3)
//Input			NULL
//Output		_sBiSliderBarStruct
//Result

//SLiderBar
#define EACT_BISLIDERBAR_DRAG_START						EMSG_BI_SLIDERBAR_REQUEST(1,4)
//Input			_sBiSliderBarStruct
//Output		NULL
//Result

//SLiderBar
#define EACT_BISLIDERBAR_DRAG_END						EMSG_BI_SLIDERBAR_REQUEST(1,5)
//Input			_sBiSliderBarStruct
//Output		NULL
//Result
//SLiderBar
#define EACT_BISLIDERBAR_SET_RANGE						EMSG_BI_SLIDERBAR_REQUEST(1,6)
//Input			_sBiSliderBarStruct
//Output		NULL
//Result

#define EACT_BISLIDERBAR_SET_MIDLABEL_LEGTH						EMSG_BI_SLIDERBAR_REQUEST(1,7)
//Input			FLOAT
//Output		NULL
//Result

#define EACT_BISLIDERBAR_SET_MIDLABEL_VALUE						EMSG_BI_SLIDERBAR_REQUEST(1,8)
//Input			whcar_t*
//Output		NULL
//Result
//////////////////////////////////////////////////////////////////////////
#define ES_ETYPE_TOOLBAR	L"ToolBar"			// ������

#define EMSG_TOOLBARITEM_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(LMSG_TP_TOOLBAR, _MJ_NUM, _MN_NUM)


// �����������������
#define EEVT_TOOLBARITEM_CLICK					EMSG_TOOLBARITEM_REQUEST(1,1)
// input TOOLBAR_MSG
//����ToolBar��Ϣ�ṹ
enum TOOLBAR_MSG_PARA_TYPE
{
	TMPT_NONE,
	TMPT_BOOL,
	TMPT_INT,
	TMPT_FLOAT,
	TMPT_STRING,
	TMPT_HEX,

	TMPT_OTHERS			//Never Use
};
struct TOOLBAR_MSG
{
	unsigned int				mnCtrlID;			//ToolBar�ӿؼ� ID
	TOOLBAR_MSG_PARA_TYPE		mnMsgParaType;		//��Ϣ���ͱ�ʶ
	//void*						mpMsgBuf;			//��ϢЯ�������ݻ�����
	union {
		bool mbBool;
		wchar_t mswString[MAX_PATH];
		LONG mlInterge;
		FLOAT mfFloat;
		ULONG muColor;
	};

	TOOLBAR_MSG() : mnCtrlID(0x00000000), mnMsgParaType(TMPT_OTHERS)/*, mpMsgBuf(0) */{}
	//~TOOLBAR_MSG() { mpMsgBuf = 0;}
};


//	���½�����Ϣ
#define EEVT_UPDATE_UI							EMSG_TOOLBARITEM_REQUEST(1,2)
// input  none
// output none
// result none

//	����Pane
#define EEVT_UPDATE_PANE						EMSG_TOOLBARITEM_REQUEST(1,4)
//Input			int (Pane IDֵ��
//Output		None
//Result

//	֪ͨPane��Ԫ�ظ������ݽ���
#define EEVT_PANE_ITEM_SET_VALUE				EMSG_TOOLBARITEM_REQUEST(1,5)
//Input			None
//Output		None
//Result


//	��ȡ����UnificSetting ID
#define EEVT_GET_UNIFIC_SETTING_ID				EMSG_TOOLBARITEM_REQUEST(1,6)
//Input		None
//Output	int
//Result

//////////////////////////////////////////////////////////////////////////
#define ES_ETYPE_WHIRLANGLE L"WhirlAngle"		// ��ת�Ƕ�

#define EMSG_WHIRLANGLE_REQUEST(_MJ_NUM,_MN_NUM)	EMSG_DEFINE(LMSG_TP_WHIRLANGLE,_MJ_NUM,_MN_NUM)

// ������Ϣ
// ��������������������������������������������������������������������������������������������

// ��ȡ��ǰ�Ƕ�
#define EACT_WHIRLANGLE_GET_ANGLE				EMSG_WHIRLANGLE_REQUEST(1,1)
// output  double

// ���õ�ǰ�Ƕ�
#define EACT_WHIRLANGLE_SET_ANGLE				EMSG_WHIRLANGLE_REQUEST(1,2)
// input  double


// ������Ϣ
// ��������������������������������������������������������������������������������������������
// �Ƕȷ����仯��ʼ
#define EEVT_WHIRLANGEL_ANGLE_CHANGE_BEGIN		EMSG_WHIRLANGLE_REQUEST(2,1)

// �Ƕȷ����仯
#define EEVT_WHIRLANGEL_ANGLE_CHANGE			EMSG_WHIRLANGLE_REQUEST(2,2)
// input double ��ǰ�Ƕ�

// �Ƕȷ����仯����
#define EEVT_WHIRLANGEL_ANGLE_CHANGE_END		EMSG_WHIRLANGLE_REQUEST(2,3)

//////////////////////////////////////////////////////////////////////////

#define ES_ETYPE_SPINBUTTON L"SpinButton"			//SpinButtonԪ��

//����SpinButton���ӿؼ�ID�������ؼ�������ڵ��� SB_ID_CTRL_OTHERS
#define SB_ID_CTRL_BACKGROUND				1
#define SB_ID_CTRL_EDIT						2
#define SB_ID_CTRL_BUTTON_UP				3
#define SB_ID_CTRL_BUTTON_DOWN				4

#define SB_ID_CTRL_OTHERS					5	

#define EMSG_SPINBUTTON_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(LMSG_TP_SPINBUTTON, _MJ_NUM, _MN_NUM)

//��ȡ��ǰֵ
#define EACT_SPINBUTTON_GET_CURRENT_VALUE			EMSG_SPINBUTTON_REQUEST(1,1)
//Input		None
//Output	int*
//Result

//���õ�ǰֵ
#define EACT_SPINBUTTON_SET_CURRENT_VALUE			EMSG_SPINBUTTON_REQUEST(1,2)
//Input		int*  
//Output	None
//Result

// ������Сֵ
#define EACT_SPINBUTTON_SET_MIN_VALUE				EMSG_SPINBUTTON_REQUEST(1,3)
//Input		int*  
//Output	None
//Result

// �������ֵ
#define EACT_SPINBUTTON_SET_MAX_VALUE				EMSG_SPINBUTTON_REQUEST(1,4)
//Input		int*  
//Output	None
//Result


//�༭�����ݱ��޸�
#define EEVT_SPINBUTTON_CONTENT_MODIFING			EMSG_SPINBUTTON_REQUEST(2,1)
// input none
// output none
// result na

// �����޸���ɣ�����ʱ����"Enter"���������Ƴ����̽��㣬�����������������Ϣ������
// Ӧ���յ�"��������"��Ϣ�����µ������޸ģ����ᷢ��������Ϣ
#define EEVT_SPINBUTTON_CONTENT_COMPLETION			EMSG_SPINBUTTON_REQUEST(2,2)
// input ��ǰֵ
// output none
// result na
//////////////////////////////////////////////////////////////////////////

#define ES_ETYPE_TIMESPINBUTTON L"TimeSpinButton"			//TimeSpinButtonԪ��


#define EMSG_TIMESPINBUTTON_REQUEST(_MJ_NUM,_MN_NUM) EMSG_DEFINE(LMSG_TP_TIMESPINBUTTON, _MJ_NUM, _MN_NUM)

//����SpinButton���ӿؼ�ID�������ؼ�������ڵ��� SB_ID_CTRL_OTHERS
#define TSB_ID_CTRL_EDIT_MINUTES			1
#define TSB_ID_CTRL_EDIT_SECONDS			2
#define TSB_ID_CTRL_EDIT_MILLISECONDS		3
#define TSB_ID_CTRL_BUTTON_UP				4
#define TSB_ID_CTRL_BUTTON_DOWN				5
#define TSB_ID_CTRL_LABEL_COLON_ONE			6
#define TSB_ID_CTRL_LABEL_COLON_TWO			7

#define TSB_ID_CTRL_OTHERS					10	


struct XuiTimeFormat
{
	//int nHours;		//00 - 23
	int nMinutes;		//��		00 - 59
	int nSeconds;		//��		00 - 59
	int nMillisecond;	//����	000 - 999 

	XuiTimeFormat() : nMinutes(0), nSeconds(0), nMillisecond(0) {}
};

//���ÿؼ���ǰʱ��
#define EACT_TIMESPINBUTTON_SET_TIME			EMSG_TIMESPINBUTTON_REQUEST(1,1)
//Input		XuiTimeFormat 
//Output	None

//��ȡ�ؼ���ǰʱ�� (send)
#define EACT_TIMESPINBUTTON_GET_TIME			EMSG_TIMESPINBUTTON_REQUEST(1,2)
// intput none
// output XuiTimeFormat* buffer to receive the time

//֪ͨ������ʱ�䱻�޸�
#define EEVT_TIMESPINBUTTON_TIME_MODIFIED			EMSG_TIMESPINBUTTON_REQUEST(1,3)
//Input		XuiTimeFormat 
//Output	None

//////////////////////////////////////////////////////////////////////////


#pragma pack()



#endif//_ECTL_H_