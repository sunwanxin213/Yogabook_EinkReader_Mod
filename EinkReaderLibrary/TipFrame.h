/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	��ʾ/���ع�����ҳ��
*/

DECLARE_BUILTIN_NAME(TipFrame)

class CTipFrame:
	public CXuiElement<CTipFrame,GET_BUILTIN_NAME(TipFrame)>
{
	friend CXuiElement<CTipFrame,GET_BUILTIN_NAME(TipFrame)>;

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
	CTipFrame(void);
	~CTipFrame(void);

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
	IEinkuiIterator* mpIterBtClose;	//����رս���
	IEinkuiIterator* mpIterPicLeft;	//���ͼƬ
	IEinkuiIterator* mpIterPicRight;	//�ұ�ͼƬ
	IEinkuiIterator* mpIterPicPic;	//��ͼƬ
	IEinkuiIterator* mpIterPicBack;	//����ͼƬ
	IEinkuiIterator* mpIterText;	//����
};

#define TIP_F_BT_CLOSE 6 //�رհ�ť