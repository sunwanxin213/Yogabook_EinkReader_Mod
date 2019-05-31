/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _EVLISTIMP_H_
#define _EVLISTIMP_H_


#include "cmmstruct.h"
#include "EvScrollBarImp.h"
// Element�Ļ���ʵ�֣���������Elementʱ�����Դ�Ϊ������Ҫ���ԴӴ����ֽ��������࣬��Ϊ��������������ʵ�������Ľӿڣ�ֱ�����������CEvButton���������ṩIXsElement�ӿ�
// ���ʵ�ֵ�����ͬ�ӿڵ���𣬾Ϳ���ֱ�Ӵ�ĳ��ʵ�������������ࡣ

//CEvList��һ���򵥿ؼ������ؼ�������Ŀؼ������Լ�ָ��
//ĿǰList�ؼ�����
//    1�� �������ñ������л�����,����֧�ֶ�֡ͼƬ
//    2�� ����������ʾģʽ��Ŀǰ֧�� Report�� SmallItem(��ӦMFC ClistCtrl SmallIconģʽ)
//        Report �� smallItem ��ֻ֧������Ĺ�����
//    3�� ֧��Vert �� Hor �Ĺ�����
//    4�� ���������Ƿ���Ҫ��������Ŀǰ������Ĭ�Ϸ���list���Һ��·�

DECLARE_BUILTIN_NAME(List)
class CEvList :
	public CXuiElement<CEvList ,GET_BUILTIN_NAME(List)>
{
	friend CXuiElement<CEvList ,GET_BUILTIN_NAME(List)>;
public:
	
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		) ;

protected:
	// ֻ���ڱ������ó�ʼֵ����ָ����ΪNULL�����п���ʧ�ܵ������֮������㶼Ӧ����InitOnCreate�н���
	CEvList();
	// �����ͷų�Ա����
	virtual ~CEvList();
	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//����
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);
	
	
	
	// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
	// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);

	// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
	// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
	virtual ERESULT OnElementDestroy();
	
	//�л���ʾ֡,��һ֡Ϊ1
	virtual ERESULT OnChangeBackImageIndex(LONG nlIndex = 0);
	//������ʾͼƬ
	virtual ERESULT OnChangeBackImagePic(wchar_t* npswPicPath = NULL);

	// ��������
	virtual ERESULT OnMouseOwnerTest(const D2D1_POINT_2F& rPoint);

	//����Resize ����������ʾ����
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize);

	//����λ�ã������Ա������λ��
	bool CalcElementPosition();

	//��껬��
	virtual ERESULT OnMouseWheel(const STEMS_MOUSE_WHEEL* npInfo);

	//����windows��������
	ERESULT OnKeyPressed(const STEMS_KEY_PRESSED* npInfo);

	//�ַ�������Ϣ
	//virtual ERESULT OnChar(const PSTEMS_CHAR_INPUT npChar);
public:
	//������ʾģʽ
	bool SetListViewStyle(LONG nlShowStyle);

	//�����Ƿ���ʾ���������Ĺ�����
	bool ShowScrollBar(bool nbShowVer,bool nbShowHor);

	//���һ��Ԫ�ص�List������list�����������ͷŸ�Ԫ��
	bool AddElement(IEinkuiIterator * npElement,int nIndex ) ;

	//���List�����ͷ�ԭ�е�Ԫ��
	bool ResetList();

	//��������ɾ��Ԫ��
	bool DeleteElement(LONG nlIndex,bool nbClostElement=true);

	//����Ԫ��ָ��ɾ��Ԫ��
	bool DeleteElement(IEinkuiIterator * npElement);

	//����ScrollBar��λ��
	bool SetScrollBarPositionAndSize();

	bool CheckVScrollBarNeedToShow();

private:

	LONG mlCurrentIndex;			 //��ǰ��ʾ�ڼ�֡
	LONG mlMaxFrame;				 //���֡��

	FLOAT mlDocumentHeight;         //list�ĵ��ĸ߶ȣ������reportģʽ���൱�������ӿؼ��ĸ߶Ⱥ�
	FLOAT mlDocumentWidth;          //list�ĵ��Ŀ��

	bool mbShowVerScroll;          //ǿ���Ƿ���ʾ���������
	bool mbShowHorScroll;          //ǿ���Ƿ���ʾ���������

	LONG mlShowStyle;              //��ʾģʽ

	cmmVector<IEinkuiIterator*>     mpElementVector;//Ԫ��Vector
	bool mbNeedMemoryManager;   //�Ƿ���ҪList�������ڴ棬�����ͷ���Item

	FLOAT mlDocToView0PositionY;    //Doc λ�ã������View��0λ��


	//���������
	bool        mbSetScrollBarPositionAndSize;
	CEvScrollBar * mpVScrollBar;
	CEvScrollBar * mpHScrollBar; 
	IEinkuiIterator * mpIterInsertMark;     //�����ʶ,ÿ���б��Լ���key���洴��

	GUID    mGuidDrop;                  //����Drop��GUID
	bool      mbAcceptDrop;             //�Ƿ����DropItem
	IEinkuiIterator * mpIterMoveItem;       //��ҪϵͳItem��Iterator
	IEinkuiIterator * mpIterPicMoveForMouse;//��������ܶ���ͼ��
   
	D2D1_RECT_F mRectMove;              //ת������ϵ��
private:
	//װ��������Դ
	ERESULT LoadResource();

	//��������㣬���ز�������� 0.n ���ʧ�ܷ���-1
	int CheckPos(D2D1_POINT_2F nPos);
	
	////��ָ�����ƶ�������ʾ����
	void ShowByIndex(LONG nlIndex);

	//���¶�λλ��
	void Recaculate(void);
};


#define LIST_MIN_WHEEL_OFFSET  20
#define TF_ID_LIST_STYLE				 L"Style"			// 0��ʾSmallIcon���1��ʾReport���2��ʾAutoFitX���3��ʾAutoFitY���
#define TF_ID_LIST_BACKIAMGE_FRAME_COUNT L"BackFrameCount"	//��ͼ�м�֡





#endif//_EVLISTIMP_H_
