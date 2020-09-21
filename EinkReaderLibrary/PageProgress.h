/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	��ҳ��ת������ҳ��
*/


DECLARE_BUILTIN_NAME(PageProgress)


class CPageProgress:
	public CXuiElement<CPageProgress,GET_BUILTIN_NAME(PageProgress)>
{
	friend CXuiElement<CPageProgress,GET_BUILTIN_NAME(PageProgress)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	// ��������
	void SetData(FLOAT nfCurrentPage, FLOAT nfPageCount);
	//���ü�ͷλ��
	void SetArrowPos(D2D1_POINT_2F ndPos);

protected:
	CPageProgress(void);
	~CPageProgress(void);

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
	//��������뿪
	virtual void OnMouseFocus(PSTEMS_STATE_CHANGE npState);
private:
	IEinkuiIterator* mpIterProgress;	//����������
	IEinkuiIterator* mpIterArrow;	//�·�С��ͷ

	FLOAT mfCurrentPage;
	FLOAT mfPageCount;

};

#define PP_BT_CLOSE 1
#define PP_PROGRESS 2