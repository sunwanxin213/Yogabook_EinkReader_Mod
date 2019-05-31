/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	��������
*/

DECLARE_BUILTIN_NAME(SnapShot)


class CSnapShot:
	public CXuiElement<CSnapShot,GET_BUILTIN_NAME(SnapShot)>
{
	friend CXuiElement<CSnapShot,GET_BUILTIN_NAME(SnapShot)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	// ģ̬��ʾ�öԻ���
	void DoModal();
	void ExitModal();

protected:
	CSnapShot(void);
	~CSnapShot(void);

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

private:
	IEinkuiIterator* mpIterSelectFrame;	
	IEinkuiIterator* mpIterBtSnap;		
	IEinkuiIterator* mpIterBtReturn;	


	D2D1_POINT_2F mdDropBeginPos;
	D2D1_SIZE_F mdBeginSize; //�϶�ǰѡ����С

	//����������ťλ��
	void SetButtonPos();
	//��ʼ����ͼ����
	void InitSnapRect(D2D1_SIZE_F ndParentSize);

};



#define SS_BT_SNAP 110
#define SS_BT_RETURN 111


