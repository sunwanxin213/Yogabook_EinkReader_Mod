/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	Loading����
*/

DECLARE_BUILTIN_NAME(LoadingView)

class CLoadingView:
	public CXuiElement<CLoadingView,GET_BUILTIN_NAME(LoadingView)>
{
	friend CXuiElement<CLoadingView,GET_BUILTIN_NAME(LoadingView)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	//�����ļ���
	void SetData(wchar_t* npszFileName);
	//�����Ѽ���ҳ��
	void SetPage(LONG niIndex);

	// ģ̬��ʾ�öԻ���
	void DoModal(volatile LONG* nplStep,HANDLE nWaitHandle);
	void ExitModal();

protected:
	CLoadingView(void);
	~CLoadingView(void);

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//��ʱ��
	virtual void OnTimer(
		PSTEMS_TIMER npStatus
		);
	//������Ϣ
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);
	//��Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	//��ť�����¼�
	virtual ERESULT OnCtlButtonClick(IEinkuiIterator* npSender);
	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize);
	//֪ͨԪ�ء���ʾ/���ء������ı�
	virtual ERESULT OnElementShow(bool nbIsShow);

private:
	IEinkuiIterator* mpIterFileName;
	IEinkuiIterator* mpIterDi;
	IEinkuiIterator* mpIterDd;
	IEinkuiIterator* mpIterIndex;
	volatile LONG* mplStep;
	HANDLE mWaitHandle;
};

#define LOAD_TIMER_MOVE 1