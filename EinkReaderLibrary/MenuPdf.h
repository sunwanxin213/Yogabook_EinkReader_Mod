/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	pdf����˵�
*/

DECLARE_BUILTIN_NAME(MenuPdf)

class CMenuPdf:
	public CXuiElement<CMenuPdf,GET_BUILTIN_NAME(MenuPdf)>
{
	friend CXuiElement<CMenuPdf,GET_BUILTIN_NAME(MenuPdf)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	// ģ̬��ʾ�öԻ���
	void DoModal();

	void ExitModal();
	//���õ�˫ҳ
	void SetPageTwo(bool nbIsDouble, bool nbIsEnable);
	//���õ�ǰҳ���ע����
	void SetCurrentPageInkCount(int niCount);
	//������Ļ����
	void SetScreenOritent(ULONG nulIndex);
protected:
	CMenuPdf(void);
	~CMenuPdf(void);

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//��ʱ��
	virtual void OnTimer(
		PSTEMS_TIMER npStatus
		);
	//��Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	//��ť�����¼�
	virtual ERESULT OnCtlButtonClick(IEinkuiIterator* npSender);
	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize);
	//֪ͨԪ�ء���ʾ/���ء������ı�
	virtual ERESULT OnElementShow(bool nbIsShow);
	// ��������
	virtual ERESULT OnMouseOwnerTest(const D2D1_POINT_2F& rPoint);
	//����ƶ�
	virtual ERESULT OnMouseMoving(const STEMS_MOUSE_MOVING* npInfo);
	//��������뿪
	//virtual void OnMouseFocus(PSTEMS_STATE_CHANGE npState);
	//��갴��
	virtual ERESULT OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo);

private:

	IEinkuiIterator* mpIterMenuBase;
	IEinkuiIterator* mpIterPageGroup;
	IEinkuiIterator* mpIterBtEraser;
	bool mbIsPressed;
};

#define MP_BT_SHAPSHOT 2
#define MP_BT_ERASER 5
#define MP_BT_CLEAR_FULL_SCREEN 10

