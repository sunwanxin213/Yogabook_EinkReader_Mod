/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	����ʾ�ķ�ҳ���м书������ʾ���ع��ܵİ�ť
*/

DECLARE_BUILTIN_NAME(PreNextButton)

class CPreNextButton:
	public CXuiElement<CPreNextButton,GET_BUILTIN_NAME(PreNextButton)>
{
	friend CXuiElement<CPreNextButton,GET_BUILTIN_NAME(PreNextButton)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

protected:
	CPreNextButton(void);
	~CPreNextButton(void);

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//��ʱ��
	virtual void OnTimer(
		PSTEMS_TIMER npStatus
		);
	//��Ϣ������
	//virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
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
	IEinkuiIterator* mpIterBtPre;		//��һҳ
	IEinkuiIterator* mpIterBtMiddle;	//�м�
	IEinkuiIterator* mpIterBtNext;		//��һҳ

	D2D1_POINT_2F mdPressPos;
	float mfLeftButton; //�����жϵ���Ƿ��Ƿ�ҳ����
	DWORD mdwClickTicount; //�����жϳ��� ����500ms�㳤��

};

#define PNB_BT_MIDDLE 100  //�м�
#define PNB_BT_PRE 101	//��һҳ
#define PNB_BT_NEXT 102 //��һҳ