/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once


DECLARE_BUILTIN_NAME(System_SelectFrame)


#define SF_POINT_MAX 8	//�������

class CSelectFrame :
	public CXuiElement<CSelectFrame,GET_BUILTIN_NAME(System_SelectFrame)>
{
	friend CXuiElement<CSelectFrame,GET_BUILTIN_NAME(System_SelectFrame)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=0					// �����Ϊ0����ָ����Ԫ�ص�EID������ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ��Ǹ�����
		) ;

protected:
	CSelectFrame(void);
	virtual ~CSelectFrame(void);

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
	// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	//������Ϣ
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);
	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize);
	// ��������
	virtual ERESULT OnMouseOwnerTest(const D2D1_POINT_2F& rPoint);

	//Ԫ����ק
	virtual ERESULT OnDragging(const STMS_DRAGGING_ELE* npInfo);

	//��ק��ʼ
	virtual ERESULT OnDragBegin(const STMS_DRAGGING_ELE* npInfo);

	//��ק����
	virtual ERESULT OnDragEnd(const STMS_DRAGGING_ELE* npInfo);
	//���û�����
	virtual ERESULT OnElementEnable(bool nbIsEnable);
	//������Ϣ
	virtual ERESULT OnKeyPressed(const STEMS_KEY_PRESSED* npInfo);

protected:
	
	// ���ö�Ӧ������״̬
	void SetCursor(D2D1_POINT_2F ndPoint, D2D1_RECT_F ndNormalRect, int niIndex);

	// ���ð˸������ʾ״̬
	void SetPointVisible(bool nbFlag);

	//�����˸����λ��
	void Relocation(void);

	//����˸����϶�
	ERESULT OnPointDrag(IEinkuiIterator* npDragItem,D2D1_SIZE_F* npOffset);

	// �ȱ�����
	ERESULT OnProportionalScaling(IEinkuiIterator* npDragItem, STCTL_CHANGE_POSITION_SIZE& noChangePositionSize);

	// ���������
	ERESULT OnNormalScaling(IEinkuiIterator* npDragItem, STCTL_CHANGE_POSITION_SIZE& noChangePositionSize);

private:
	IEinkuiIterator* mpArrayPoint[SF_POINT_MAX];
	D2D1_POINT_2F mdOriginPosition;		// ԭʼλ��
	D2D1_SIZE_F	mdOriginSize;			// ԭʼ��С
	IEinkuiBrush*	mpXuiBrush;				// ���߻�ˢ
	bool 		mbModifying;			// ָʾ��ǰ�༭״̬

	HCURSOR mhCursorNwse;		// �����ʽ
	HCURSOR mhCursorNesw;		// �����ʽ

	bool	mcLastVTurn;	// ��һ�εĴ�ֱ��ת״̬	
	bool	mcLastHTurn;	// ��һ�ε�ˮƽ��ת״̬

	bool	mcVTurn;	// �Ƿ�����ֱ��ת
	bool	mcHTurn;	// �Ƿ���ˮƽ��ת


	// �ȱ�������ص�����
	bool	mcProportionalScaling;	// �Ƿ���ǿ�Ʊ�������
	bool	mcProportionalScalingByKey; // �ȱ�״̬��������shift����ʱ��Ϊtrue

	// �Ƿ�����༭�����������༭,�����е�ê��״̬��Ϊ�ƶ�״̬
	bool	mcIsEdit;


};


