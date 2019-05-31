/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	����ʱ�Ĺ�����
*/

DECLARE_BUILTIN_NAME(ToolbarH)

class CToolbarH:
	public CXuiElement<CToolbarH,GET_BUILTIN_NAME(ToolbarH)>
{
	friend CXuiElement<CToolbarH,GET_BUILTIN_NAME(ToolbarH)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	//�����ļ�����
	void SetFileName(wchar_t* npszString);
	//����ҳ���ַ���
	void SetPage(wchar_t* npszString);
	//����˫ҳ��ʾ��ť״̬
	void SetDuopageButton(bool nbSingle);
	//��ȡ��ǰ˫ҳ��ʾ״̬
	bool GetDuopageStatus(void);

protected:
	CToolbarH(void);
	~CToolbarH(void);

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
private:
	IEinkuiIterator* mpIterFileName;	//�ļ���
	IEinkuiIterator* mpIterPage;		//ҳ��
	IEinkuiIterator* mpIterBtFileOpen;	//���ļ��Ի���
	IEinkuiIterator* mpIterBtTwo;		//˫����ʾ
	IEinkuiIterator* mpIterBtOne;		//������ʾ
	IEinkuiIterator* mpIterBtJump;		//ҳ����ת
	IEinkuiIterator* mpIterBtSnap;		//����
	IEinkuiIterator* mpIterBtSuofang;		//����
	IEinkuiIterator* mpIterBackground;	//����ͼ

	bool mbIsTwoScreen;//true��ʾ˫��
	bool mbIsTxt;
};

#define TBH_BT_OPEN_FILE 8
#define TBH_BT_JUMP 3
#define TBH_BT_TWO 2
#define TBH_BT_SNAPSHOT 4
#define TBH_BT_ZOOM 5
#define TBH_BT_ONE_PIC 9