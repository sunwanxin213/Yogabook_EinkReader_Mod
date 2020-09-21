/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	�ƶ�����Ĺ�����
*/
#include "cmmstruct.h"
#include "Define.h"

DECLARE_BUILTIN_NAME(MoveControlToolbar)

#define ZC_BT_UP 101
#define ZC_BT_DOWN 102
#define ZC_BT_LEFT 103
#define ZC_BT_RIGHT 104

class CMoveControlToolbar:
	public CXuiElement<CMoveControlToolbar,GET_BUILTIN_NAME(MoveControlToolbar)>
{
	friend CXuiElement<CMoveControlToolbar,GET_BUILTIN_NAME(MoveControlToolbar)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	//��ʼ���Լ�
	void initData(void);

	//����4���ƶ���ť��״̬
	void ShowMoveButton(RECT ldRect);
	//���û�������
	void SetMoveForward(MoveForward forward);

protected:
	CMoveControlToolbar(void);
	~CMoveControlToolbar(void);

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//��ʱ��
	virtual void OnTimer(PSTEMS_TIMER npStatus);
	//��Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	//��ť�����¼�
	virtual ERESULT OnCtlButtonClick(IEinkuiIterator* npSender);
	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize);
	//֪ͨԪ�ء���ʾ/���ء������ı�
	virtual ERESULT OnElementShow(bool nbIsShow);

private:
	IEinkuiIterator* mpIterBtUP = nullptr;
	IEinkuiIterator* mpIterBtDown = nullptr;
	IEinkuiIterator* mpIterBtLeft = nullptr;
	IEinkuiIterator* mpIterBtRight = nullptr;

	MoveForward mMoveForward = MoveForward::HORIZONTAL_VERTICAL;

	//���������ƶ�
	void MovePage(ULONG nulID);
};
