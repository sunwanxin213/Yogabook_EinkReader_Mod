/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	�ʼ��˵�
*/

DECLARE_BUILTIN_NAME(MenuPen)

class CMenuPen:
	public CXuiElement<CMenuPen,GET_BUILTIN_NAME(MenuPen)>
{
	friend CXuiElement<CMenuPen,GET_BUILTIN_NAME(MenuPen)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	// ģ̬��ʾ�öԻ���
	void DoModal();

	void ExitModal();
	// ���ó�ʼ������
	void SetData(DWORD ndwPenWidthIndex, DWORD ndwPenColorIndex);

protected:
	CMenuPen(void);
	~CMenuPen(void);

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
	//��갴��
	virtual ERESULT OnMousePressed(const STEMS_MOUSE_BUTTON* npInfo);

private:

	IEinkuiIterator* mpIterMenuBase;
	IEinkuiIterator* mpIterPenWidthGroup;
	IEinkuiIterator* mpIterPenColorGroup;
	bool mbIsPressed;
};

#define MT_BT_SHAPSHOT 2

#define MP_CBT_LINE1 1
#define MP_CBT_LINE2 2
#define MP_CBT_LINE3 3
#define MP_CBT_LINE4 4

#define MP_CBT_RED 100
#define MP_CBT_YELLOW 101
#define MP_CBT_BLUE 102


