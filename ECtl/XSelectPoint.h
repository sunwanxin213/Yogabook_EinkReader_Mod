/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once


DECLARE_BUILTIN_NAME(System_SelectPoint)


#define SF_POINT_MAX 8	//�������

class CSelectPoint :
	public CXuiElement<CSelectPoint,GET_BUILTIN_NAME(System_SelectPoint)>
{
	friend CXuiElement<CSelectPoint,GET_BUILTIN_NAME(System_SelectPoint)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=0					// �����Ϊ0����ָ����Ԫ�ص�EID������ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ��Ǹ�����
		) ;

protected:
	CSelectPoint(void);
	virtual ~CSelectPoint(void);

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
	// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	// ��������
	virtual ERESULT OnMouseOwnerTest(const D2D1_POINT_2F& rPoint);
	//������Ϣ
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);

	//Ԫ����ק
	virtual ERESULT OnDragging(const STMS_DRAGGING_ELE* npInfo);

	//��ק��ʼ
	virtual ERESULT OnDragBegin(const STMS_DRAGGING_ELE* npInfo);

	//��ק����
	virtual ERESULT OnDragEnd(const STMS_DRAGGING_ELE* npInfo);

private:
	float	mfWidth;			// Ĭ�Ͽ��
	float	mfHeight;			// Ĭ�ϸ߶�

	IEinkuiBrush* mpXuiBrush;
	IEinkuiBrush* mpFillBrush;


	//�޸������ʽ
	void SetCursor(HCURSOR nhInnerCursor);


};


