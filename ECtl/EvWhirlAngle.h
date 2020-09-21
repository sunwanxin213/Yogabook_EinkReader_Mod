#pragma once

DECLARE_BUILTIN_NAME(WhirlAngle)
class CEvWhirlAngleImp :
	public CXuiElement<CEvWhirlAngleImp, GET_BUILTIN_NAME(WhirlAngle)>
{
	friend CXuiElement<CEvWhirlAngleImp, GET_BUILTIN_NAME(WhirlAngle)>;
public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	);

protected:
	CEvWhirlAngleImp(void);
	~CEvWhirlAngleImp(void);

	//������Ϣ
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);

	ERESULT ParseMessage(IEinkuiMessage* npMsg);

	// ��������
	virtual ERESULT OnMouseOwnerTest(const D2D1_POINT_2F& rPoint);

	//virtual ERESULT OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo);

	//Ԫ����ק
	virtual ERESULT OnDragging(const STMS_DRAGGING_ELE* npInfo);

	//��ק��ʼ
	virtual ERESULT OnDragBegin(const STMS_DRAGGING_ELE* npInfo);

	//��ק����
	virtual ERESULT OnDragEnd(const STMS_DRAGGING_ELE* npInfo);

	//��������뿪
	virtual void OnMouseFocus(PSTEMS_STATE_CHANGE npState);

	//���û�����
	virtual ERESULT OnElementEnable(bool nbIsEnable);

	// ������
	//		����ͼƬ֡��
	void SetPicIndex(
		IN size_t niIndex			// ����
	);

	// ������
	//		ͨ����ǰλ�ã�������ת�˵Ļ���ֵ
	double GetRadian(
		IN D2D1_POINT_2F nfCrtPoint
	);

	// ������
	//		��ȡ����֮��ľ���
	double GetDistance(
		IN D2D1_POINT_2F ndfPointA,
		IN D2D1_POINT_2F ndfPointB
	);

	// ������
	//		������ת��λ��
	void UpdateDotPosition(
		IN double ndbRadian
	);

	// ������
	//		����ת���ɽǶ�
	double RadianToAngle(
		IN double ndbRandian
	);

	// ������
	//		�Ƕ�ת���ɻ���
	double AngleToRadian(
		IN double ndbAngle
	);

private:
	void LoadResource();

	IEinkuiIterator* mpoWhirlBg;			// ��ת����
	IEinkuiIterator* mpoWhirlDot;			// ��ת��

	D2D1_POINT_2F mdWhirlCenter;		// ��ת����	
	D2D1_POINT_2F mdDotPoint;			// ��ת�����꣨Ĭ��Ϊ��ת����������꣩

	double mdbRadian;						// ��ǰ��ת�Ļ���
};

#define CONST_VALUE_PI					3.141592


#define ID_OF_SUB_WHIRL_BG				1
#define ID_OF_SUB_WHIRL_DOT				2

#define TF_ID_WHIRLANGLE_WHIRL_CENTER_X			L"WhirlCenterX"			// ��ת����X
#define TF_ID_WHIRLANGLE_WHIRL_CENTER_Y			L"WhirlCenterY"			// ��ת����Y
#define TF_ID_WHIRLANGLE_DOT_POINT_X			L"DotPointX"			// ��ת��X
#define TF_ID_WHIRLANGLE_DOT_POINT_Y			L"DotPointY"			// ��ת��Y




