/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */
#ifndef _ERESULT_H_
#define _ERESULT_H_
/* 
	���Ķ���Eiui��ϵͳ������
	Definitions for the return value from Eiui funcitons
*/



#define  ERESULT ULONG

#define ERESULT_SUCCEEDED(_X) ((_X&0x80000000)==0)
#define ERESULT_FAILED(_X) ((_X&0x80000000)!=0)


//////////////////////////////////////////////////////////////////////////
// �����붨�����
//////////////////////////////////////////////////////////////////////////
// �������Ϊ3�����������λ����ȷ������������ʣ����Ϊ1��ʾ���صĽ�������������ʱ���������룻���Ϊ0��ʾ���ص��ǷǴ���ķ�����
// ������ĵڶ������Ǵ������漰��ϵͳ����Ԫ�����ͣ�ͬXUI.H�ļ��ж������Ϣ����һ�£���LMSG_TP_SYSTEM��ϵͳ�����룬ͨ��XuiĬ�϶�����ϵͳ����
// ������ĵ��������Ƿ������ӱ��룬ȡֵ������32767

// �����붨���ú�
#define ERESULT_NORMAL_DEFINE(_TYPE,_NUMBER) ((((_TYPE)&0xFFFF)<<15)|((_NUMBER)&0x7FFF))

// �����붨���ú�
#define ERESULT_ERROR_DEFINE(_TYPE,_NUMBER) ((((_TYPE)&0xFFFF)<<15)|((_NUMBER)&0x7FFF)|0x80000000)






//////////////////////////////////////////////////////////////////////////
// �Ǵ���ϵͳ������
//////////////////////////////////////////////////////////////////////////
#define ERESULT_SUCCESS				ERESULT_NORMAL_DEFINE(LMSG_TP_SYSTEM,0)
#define ERESULT_DEFAULT				ERESULT_NORMAL_DEFINE(LMSG_TP_SYSTEM,1)

#define ERESULT_NOT_POINTOWNER		ERESULT_NORMAL_DEFINE(LMSG_TP_SYSTEM,10)	// ̽����Ļ�ϵ�һ�����Ƿ�����ĳ��ELEMENTʱ�����ز��ǣ����򣬷���success��ʾ�õ����ڸ�ELEMENT

#define ERESULT_DRGDRP_REFUSE		ERESULT_NORMAL_DEFINE(LMSG_TP_SYSTEM,20)	// �ܾ�Drag&Drop���������ڸ����ڶԿؼ���Drag&Drop�¼�����

#define ERESULT_NO_MESSAGE			ERESULT_NORMAL_DEFINE(LMSG_TP_SYSTEM,30)	// ��ǰ��Ϣ���п�

#define ERESULT_NOT_PAINT			ERESULT_NORMAL_DEFINE(LMSG_TP_SYSTEM,35)	// û�л��ƣ������Ǵ������ػ��߱��ڵ�

#define ERESULT_QUIT_OPLOOP			ERESULT_NORMAL_DEFINE(LMSG_TP_SYSTEM,50)	// �˳�Operation Thread Message Loop
#define ERESULT_BLOCK				ERESULT_NORMAL_DEFINE(LMSG_TP_SYSTEM,52)	// ��Ȼ������û�����
#define ERESULT_QUIT_XUI			ERESULT_NORMAL_DEFINE(LMSG_TP_SYSTEM,60)	// �˳�XUIϵͳ



#define ERESULT_UNEXPECTED_MESSAGE	ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,1)	// ͨ������Ϣ�����ຯ�����أ���ʾ������Ϣδ������
#define ERESULT_MSG_SENDTO_NEXT	ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,5)	// �����յ���Hook��Ϣ���ݸ���һ��Hook���߷��͵�ԭʼ������
#define ERESULT_WINMSG_SENDTO_NEXT		ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,5)	// ��Windows��Ϣ���ݸ���һ�������߻��߽���ϵͳ����
#define ERESULT_ENUM_CHILD			ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,10)	// ����ö��������Ԫ��
#define ERESULT_STOP_ENUM_CHILD		ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,11)	// ֹͣö����Ԫ��
#define ERESULT_EXIT_ENUM			ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,12)	// ֹͣȫ��ö����Ϊ
#define ERESULT_REDO_ENUM			ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,13)	// �Ե�ǰ�ڵ㼰���ӽڵ�����ö��һ��
#define ERESULT_DISCARD				ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,14)	// ͨ�÷���ֵ��������Ϣ�����߷���ĳ����Ϊ������
#define ERESULT_ACCEPT				ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,15)	// ͨ�÷���ֵ��������Ϣ�����߽���ĳ����Ϊ������

#define ERESULT_MOUSE_OWNERSHIP		ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,20)	// ����Ϊ�������ӵ���ߣ�����괦����������÷�Χ

#define ERESULT_EDRAGDROP_BEGIN		ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,30)	// �������XUI Drag&Drop���̣���������EMSG_DRAG_BEGIN��Ϣʱ��Ч
#define ERESULT_EDRAGDROP_ACCEPT	ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,32)	// XUI Drag&Drop�����У���������ӦEMSG_XUI_DROP_TEST����ʾ���Խ���Drop

#define ERESULT_WDRAGDROP_BEGIN		ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,40)	// �������Windows Drag&Drop���̣���������EMSG_DRAG_BEGIN��Ϣʱ��Ч




#define ERESULT_IDOK				ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMENT,1)	// �Ի��򷵻ص�ok���
#define ERESULT_IDCANCLE			ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMENT,2)	// �Ի��򷵻ص�Cancel���
#define ERESULT_IDYES				ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMENT,3)	// �Ի��򷵻ص�Yes���
#define ERESULT_IDNO				ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMENT,4)	// �Ի��򷵻ص�No���
#define ERESULT_IDABOUT				ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMENT,5)	// �Ի��򷵻ص�About���
#define ERESULT_IDRETRY				ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMENT,6)	// �Ի��򷵻ص�Retry���
#define ERESULT_IDIGNORE			ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMENT,7)	// �Ի��򷵻ص�Ignore���
#define ERESULT_IDCONTINUE			ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMENT,8)	// �Ի��򷵻ص�Continue���

#define ERESULT_REFUSE_MOVING		ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMENT,10)	// ELEMENT�����ƶ�
#define ERESULT_KEY_UNEXPECTED		ERESULT_DISCARD // ר�����ڼ�����Ϣ�Ĵ�����ĳ���ؼ�����ȫ��������Ϣ����ֻ�����Լ�ʶ��İ�������������İ�����Ϣһ��Ҫ����������
#define ERESULT_KEY_ACCEPTED		ERESULT_ACCEPT // ר�����ڼ�����Ϣ�Ĵ�����ĳ���ؼ�����ȫ��������Ϣ����ֻ�����Լ�ʶ��İ������յ�������İ�����Ϣ��һ��Ҫ����������

#define ERESULT_SKIP_RENDER_CONTENT	ERESULT_NORMAL_DEFINE(LMSG_TP_RENDER,5)	// ��������Ŀ�꼰����Ԫ�أ��������Ƿ���EMSG_PAINT��Ϣ
#define ERESULT_REDO_RENDER_CONTENT	ERESULT_NORMAL_DEFINE(LMSG_TP_RENDER,6)	// �ٴλ���Ŀ�꼰����Ԫ�أ�������Ч������Ҳ���ٴ�ִ��




//////////////////////////////////////////////////////////////////////////
// ϵͳ������
//////////////////////////////////////////////////////////////////////////
#define ERESULT_UNSUCCESSFUL		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,1)
#define ERESULT_ERROR_WINAPI		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,2)	// Windows API����
#define ERESULT_WRONG_PARAMETERS	ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,3)
#define ERESULT_WRONG_PARAMETERS1	ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,4)	// ��ָ�����
#define ERESULT_MESSAGE_EXCEPTION	ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,7)	// ��Ϣ������̷����쳣
#define ERESULT_UNEXPECTED_CALL		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,8)	// δԤ�ϵĵ��ã������÷����ĵ���
#define ERESULT_NOT_INITIALIZED		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,9)	// δ��ʼ��
#define ERESULT_OBJECT_LOCKED		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,10)	// ��������
#define ERESULT_ELEMENT_INVALID		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,11)	// UIԪ��δ����
#define ERESULT_UNKNOWN_ERROR		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,12)	// δ֪����
#define ERESULT_ID_CONFLICT			ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,13)	// ID��ͻ
#define ERESULT_OBJECT_EXISTED		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,14)	// �����Ѿ����ڣ������ظ�����
#define ERESULT_ITERATOR_INVALID	ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,15)	// ��Ч�ĵ���������
#define ERESULT_ACCESS_CONFLICT		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,16)	// ���ʳ�ͻ
#define ERESULT_SENDER_INVALID		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,17)	// ��Ϣ��������Ч
#define ERESULT_DRAGGING_UNSUPPORT		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,18)	// ��֧���϶�
#define ERESULT_NOT_ENOUGH_BUFFER	ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,19)	// ����������
#define ERESULT_OBJECT_NOT_FOUND	ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,20)	// ���󲻴���
#define ERETULT_WRONG_STYLE			ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,21)	// ������

#define ERESULT_INSUFFICIENT_RESOURCES	ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,30)	// �ڴ����������Դ����
#define ERESULT_FILE_NOT_FOUND		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,31)	// û���ҵ��ļ�
#define ERESULT_STRING_NOT_FOUND	ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,32)	// �����ļ��е���Ϣû���ҵ�
#define ERESULT_BUSY				ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,33)	// ϵͳæµ
#define ERESULT_TIMEOUT				ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,34)	// ϵͳ��ʱ
#define ERESULT_WRONG_THREAD		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,35)	// ������߳�
#define ERESULT_STALLED_THREAD		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,36)	// �߳�����

#define ERESULT_DIRECTX_ERROR		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,50)	// Direct2D or Direct3D��������ͨ������ֱ�Ӳ���D3dD2d��ģ�飬��ϵͳ����Paint�������ֹϵͳ�ĵ�ǰ���ƹ���



#define ERESULT_NOT_SET				ERESULT_ERROR_DEFINE(LMSG_TP_OTHERS,1)	// ͨ�����ڳ��������̣���ʾһ������ֵ��δ������

//////////////////////////////////////////////////////////////////////////
// λͼ��ش���
//////////////////////////////////////////////////////////////////////////
#define ERESULT_BMP_CREATEDECODER	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 1)	// ���ļ������������ӿ�ʧ��
#define ERESULT_BMP_GETFRAME		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 2)	// �ӽ�������ȡ֡�ӿ�ʧ��
#define ERESULT_BMP_CREATEWICBMP	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 3)	// ת��WIC BITMAP�ӿ�ʧ��
#define ERESULT_BMP_WICBMPNOTREADY	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 4)	// WIC BITMAP�ӿ�ΪNULL
#define ERESULT_BMP_WICLOCKBMP		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 5)	// WIC BITMAP�ӿ�Lockʧ��
#define ERESULT_BMP_ERRGETSIZE		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 6)	// ��ȡλͼ�����Ϣʧ��
#define ERESULT_BMP_IVLDFORMAT		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 7)	// ��Ч��λͼ��ʽ
#define ERESULT_BMP_GETBMPBUF		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 8)	// ��ȡλͼ���ݻ�����ʧ��
#define ERESULT_BMP_CREATECVTER		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 8)	// ����WIC��ʽת�����ӿ�ʧ��
#define ERESULT_BMP_INITCVTER		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 9)	// ��ʼ��WIC��ʽת�����ӿ�ʧ��
#define ERESULT_BMP_CREATED2DBMP	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 10)	// ����D2Dλͼ�ӿ�ʧ��
#define ERESULT_BMP_CREATESCALER	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 11)	// ����WIC���Žӿ�ʧ��
#define ERESULT_BMP_INITSCALER		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 12)	// ��ʼ��WIC���Žӿ�ʧ��
#define ERESULT_BMP_INVALIDXY		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 13)	// ���������ֵ��Ч
#define ERESULT_BMP_CREATEWICBMPRT	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 14)	// CreateWicBitmapRenderTargetʧ��
#define ERESULT_BMP_CREATEDWRITEFAC	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 15)	// ����DWrite Factoryʧ��
#define ERESULT_BMP_CREATETEXTFMT	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 16)	// ����DWrite ITextFormatʧ��
#define ERESULT_BMP_CREATETEXTLAYOUT	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 17)	// ����DWrite ITextLayoutʧ��
#define ERESULT_BMP_GETMETRICS		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 18)	// ����DWrite GetMetricsʧ��
#define ERESULT_BMP_EXSTRACTICON	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 19)	// ��EXE���ͷ�iconʧ��
#define ERESULT_BMP_COPYFROMRT		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 20)	// ��Rt����λͼʧ��
#define ERESULT_BMP_CREATEBMPBRUSH	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 21)	// ����λͼBrushʧ��
#define ERESULT_BMP_DXGIDESC		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 22)	// ��ȡDXGISurface����ʧ��
#define ERESULT_BMP_INVALIDBUF		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 23)	// ���뻺������Ч
#define ERESULT_BMP_DRAWRT			ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 24)	// �����RenderTargetʱ����
#define ERESULT_BMP_CREATEWICRT		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 25)	// ����WICBitmapRenderTargetʧ��
#define ERESULT_BMP_COPYFROMBUF		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 26)	// CopyFromBufferʧ��




































#endif//_ERESULT_H_