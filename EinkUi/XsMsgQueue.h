#pragma once

typedef CXuiMessage* PElMessage;
typedef cmmFastList<PElMessage, 128, 64> CEsMessageFastList;

class CXuiMessageQueue : protected CEsMessageFastList {
	friend CXelManager;
public:
	CXuiMessageQueue() {
		miHistroy = 0;
	};
	~CXuiMessageQueue() {};

	// ���뱾����ǰ��
	bool Push_Front(IEinkuiMessage* npMsg);

	// ���뱾����β��
	bool Push_Back(IEinkuiMessage* npMsg);

	//// ���뱾����β������ִ��Reduce����
	//bool Push_Back_Reduce(IEinkuiMessage* npMsg);

	IEinkuiMessage* GetMessage(void);

	// ��ȡ������
	int Size(void);

	// ���ȫ����Ϣ
	void Clear(void);

	//// ������Ϣ����������������Ϣ���η��뵽һ���µ���Ϣ������
	//void DispartMessage(
	//	IN ULONG nuMsgID,
	//	IN IEinkuiIterator* npItr,	// ��Ϣ�ķ���Ŀ���Ǵ˶������Ӷ���
	//	OUT CXuiMessageQueue& rSaveTo	// ���ڽ�����Ϣ���������У����������Ӧ�ñ����
	//	);

	// ��ȡ�ض���Ϣ
	IEinkuiMessage* GetMessage(
		IN ULONG nuMsgID,
		IN IEinkuiIterator* npItr	// ��Ϣ�ķ���Ŀ���Ǵ˶������Ӷ���
	);

	//// ���ָ��Ҫ���ȫ����Ϣ
	//int RemoveMessages(
	//	unsigned short nusType,
	//	unsigned short nusMajNum,
	//	unsigned short nusMinNum,
	//	unsigned long nusMask	// combination of the values that they are defined below
	//	);

	// �����ָ���Ķ�ʱ����Ϣ
	int RemoveTimerMessage(
		const IEinkuiIterator* npTarget,	// null for all targets
		unsigned long nuTimerID		// 0 for all timer-message sending to the target
	);


private:
	CExclusiveAccess moLock;
	int miHistroy;



};

#define XSMSG_REMOVE_TYPE	1
#define XSMSG_REMOVE_MAJ	2
#define XSMSG_REMOVE_MIN	4


