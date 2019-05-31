/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

#include "cmmstruct.h"

//ҳ����ת�Ի���

DECLARE_BUILTIN_NAME(JumpPage)

class CJumpPage:
	public CXuiElement<CJumpPage,GET_BUILTIN_NAME(JumpPage)>
{
public:
	// ��������캯���趨Ϊprotected������Ҫ����仰; ���򣬲���Ҫ�������
	friend CXuiElement<CJumpPage,GET_BUILTIN_NAME(JumpPage)>;

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	// �������༰����������ʱ�����ر�ע��!!! һ��Ҫ���ȵ��û���ķ���
	// �����������ڽ�����Ԫ�ض��󡢳�ʼ���������ݣ�λ�á���С������ͼ�ȣ�
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent,	// ������ָ��
		IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32		// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��EUIϵͳ�Զ�����
		);

	//��ʱ��
	virtual void OnTimer(
		PSTEMS_TIMER npStatus
		);

public:
	CJumpPage();
	~CJumpPage(void);

	// ģ̬��ʾ�öԻ���
	void DoModal();

	void ExitModal();
	//���õ�ǰҳ��
	void SetCurrentPage(int niPage, int niMaxPage);

protected:
	

	//��Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	//��ť�����¼�
	virtual ERESULT OnCtlButtonClick(IEinkuiIterator* npSender);
	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize);
	

private:
	IEinkuiIterator* mpIteratorClose;
	IEinkuiIterator* mpIteratorOk;
	IEinkuiIterator* mpIteratorBackspace; //ɾ����ť
	IEinkuiIterator* mpIteratorCurrentPage;//��ǰλ��
	IEinkuiIterator* mpIteratorPageMax; //����ҳ��
	IEinkuiIterator* mpIteratorPageMaxText; //���ҳ�뷶Χ
	IEinkuiIterator* mpIteratorInput; //�����ҳ��
	IEinkuiIterator* mpIteratorCur; //����ʱ��˸�Ĺ��
	wchar_t mszInputNumber[MAX_PATH];	//���������
	int miMaxPage;
	int miInputPage; //�����ҳ��

	//�������������
	void InputNumber(ULONG nulNumber);
	//ɾ��һλ����
	void DeleteNumber();
	//��յ�ǰ����
	void ClearNumber();
};

#define JP_ID_BT_OK 8
#define JP_ID_BT_CANCAL 7
#define JP_ID_BT_BACKSPACE 6

#define JP_ID_BT_ZERO 10
#define JP_ID_BT_ONE 11
#define JP_ID_BT_TWO 12
#define JP_ID_BT_THREE 13
#define JP_ID_BT_FOR 14
#define JP_ID_BT_FIVE 15
#define JP_ID_BT_SIX 16
#define JP_ID_BT_SEVEN 17
#define JP_ID_BT_EIGHT 18
#define JP_ID_BT_NINE 19




