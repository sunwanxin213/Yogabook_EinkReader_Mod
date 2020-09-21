#ifndef _EVSCROLLBARIMP_H_
#define _EVSCROLLBARIMP_H_

//������

// Element�Ļ���ʵ�֣���������Elementʱ�����Դ�Ϊ������Ҫ���ԴӴ����ֽ��������࣬��Ϊ��������������ʵ�������Ľӿڣ�ֱ�����������CEvScrollBar���������ṩIXsElement�ӿ�
// ���ʵ�ֵ�����ͬ�ӿڵ���𣬾Ϳ���ֱ�Ӵ�ĳ��ʵ�������������ࡣ
DECLARE_BUILTIN_NAME(ScrollBar)
class CEvScrollBar :
	public CXuiElement<CEvScrollBar, GET_BUILTIN_NAME(ScrollBar)>
{
	friend CXuiElement<CEvScrollBar, GET_BUILTIN_NAME(ScrollBar)>;
public:
	// �������༰����������ʱ�����ر�ע��!!! һ��Ҫ���ȵ��û���ķ���
	// �����������ڽ�����Ԫ�ض��󡢳�ʼ���������ݣ�λ�á���С������ͼ�ȣ�
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent,	// ������ָ��
		IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32		// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	);

protected:
	CEvScrollBar();
	virtual ~CEvScrollBar();

	//����
	//virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);
	// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
	// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
	// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
	virtual ERESULT OnElementDestroy();
	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);

	//���button��Ϣ
	virtual ERESULT OnCtlButtonClick(IEinkuiIterator* npSender);
	//������Ϣ
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);

	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize);

	//���û�����
	virtual ERESULT OnElementEnable(bool nbIsEnable);

	//��갴��;
	ERESULT OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo);

	// ��������
	ERESULT OnMouseOwnerTest(const D2D1_POINT_2F& rPoint);

	ERESULT LoadResource();
public:
	//���û�����Ի����ľ��룬
	bool SetDeltaSize(FLOAT nnSize);
	//���û���λ�����ϻ�����
	bool SetPosition(bool bUp);
	//��ȡ����������Ŀ��
	FLOAT GetVScrollBarWidth();
	//��ȡ����������ĸ߶�
	FLOAT GetHScrollBarHeigth();
	//�޸�Ԫ�ز���
	void Relocation(void);
private:
	bool mbVertical;
	IEinkuiIterator* mpBtUp;    //���ϻ�����
	IEinkuiIterator* mpBtDrag;	//����
	IEinkuiIterator* mpBtDown;  //���»�����
	D2D1_RECT_F  mRectSlider;//���Ի����Ĵ�С
	D2D1_POINT_2F  mSliderPoint;//�����λ��
	FLOAT mfDeltaSize; //Ŀ����Ҫ���������ش�С
	FLOAT mfMinBarSize; //��������С����Сֵ������Ǵ�ֱ���Ǹ߶ȣ������ˮƽ���ǿ��
	FLOAT mfDestPixelPerScrollPix;//���黬��һ�����ض�Ӧ��Ŀ�������������

	FLOAT mfPosition;   //���鵱ǰ���ڵ�����λ��
	FLOAT mfSVScrollBarWidth; //�������Ŀ�ȣ�ȡ����Ŀ��
	FLOAT mfSHScrollBarHeigth; //�������ĸ߶ȣ�ȡ����Ŀ��

	//װ��������Դ
	//virtual ERESULT LoadResource();

};

#define SCB_BT_UP 1		//���ϻ�����ť
#define SCB_BT_DRAG 2	//�϶���
#define SCB_BT_DOWN 3	//���»����Ұ�ť

#define SCROLLBAR_HOR L"Hor"

#endif//_EVSCROLLBARIMP_H_
