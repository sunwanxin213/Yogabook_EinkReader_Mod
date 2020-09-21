/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	Txt�˵�
*/

DECLARE_BUILTIN_NAME(MenuTxt)

class CMenuTxt:
	public CXuiElement<CMenuTxt,GET_BUILTIN_NAME(MenuTxt)>
{
	friend CXuiElement<CMenuTxt,GET_BUILTIN_NAME(MenuTxt)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	// ģ̬��ʾ�öԻ���
	void DoModal();

	void ExitModal();
	//����txt�ֺ�
	void SetTxtFontSizeIndex(DWORD ndwIndex);
	//������Ļ����
	void SetScreenOritent(ULONG nulIndex);
protected:
	CMenuTxt(void);
	~CMenuTxt(void);

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
	IEinkuiIterator* mpIterFontSizeGroup;
	bool mbIsPressed;
};

#define MT_BT_SHAPSHOT 2
#define MP_BT_LCD_CONTROL 7


