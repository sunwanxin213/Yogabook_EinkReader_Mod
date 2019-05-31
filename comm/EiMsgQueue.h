/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */

#pragma once
// ���ļ�ּ�ڽ����������̵���Ϣ����
// ͨ��һ�鹲���ڴ��һ������������ƽ����Ŀ������Ϣ���У��ܹ����������̼����ͨѶ��
// ��Ϣ�����ɼ����������ȳ�ʼ���������Ž������ӵ���Ϣ���У����������ڳ�ʼ��ʱ��Ҫ�ṩһ���ص�����������ص��������ڼ����߳��б����á�
// ͬһ���ڴ�ֻ����һ����������

#pragma pack(4)

typedef struct _EI_MSGQUE_HEAD {
	ULONG HeadSize;
	volatile ULONG BufSize;
	volatile ULONG MsgSize;
	volatile ULONG MsgCapacity;
	volatile ULONG FirstMsg;
	volatile ULONG LastMsg;
	volatile ULONG MsgCount;
}EI_MSGQUE_HEAD,* PEI_MSGQUE_HEAD;
#pragma pack()


// ��Ϣ����
template<class CEiMsgMessage>
class CEiMsgQueue
{
private:
	PEI_MSGQUE_HEAD mpQueueHead;
	CEiMsgMessage* mpMsgBuffer;
	HANDLE mhNewArrived;
	HANDLE mhMutex;
	BOOL mbStop;

public:
	CEiMsgQueue() {
		mpQueueHead = NULL;
		mpMsgBuffer = NULL;
		mhNewArrived = NULL;
		mhMutex = NULL;
		mbStop = FALSE;
	}
	~CEiMsgQueue() {
	}

	BOOL GetStopState(void) {
		return mbStop;
	}

	// �ӹ����ڴ潨�����У�����ERROR_SUCCESS�����ߴ�����
	ULONG CreateQueue(
		const wchar_t* nusMutexName,
		const wchar_t* nusSemaphoreName,
		void* npBuffer,
		ULONG nuBufferSize,
		BOOL nbListener	// TURE: ������
	);

	// ����һ����Ϣ������ERROR_SUCCESS�����ߴ�����
	ULONG Push(
		const CEiMsgMessage& nrMsg
		);

	// ȡһ����Ϣ������ERROR_SUCCESS�����ߴ�����
	// �����Ϣ����Ϊ�գ�������ȴ�
	// �������������
	ULONG Pop(
		CEiMsgMessage& nrMsg
		);

	// ����һ����Ϣ���������д�����Ϣȫ������
	// ������ô˺�������ҪCEiMsgMessageʵ��bool IsTypeOf(const CEiMsgMessage& nrRefTo)�������ĺ�������true��ʾͬ�࣬����false��ʾ��ͬ��
	// ��һ����Ϣ������ʱ�������CEiMsgMessage��Recall()���������ڸú�����ʵ�ֶ���Ϣ���ز���
	void Recall(
		const CEiMsgMessage& nrMsg	
	);

	// ��ȡ��Ϣ��
	ULONG GetCount(void);

	// �ر�
	void RealseQueue(void);

	// ��õ�ǰδȡ�ߵ�����һ����Ϣ�ѷ���ʱ�䣬��λΪ����
	ULONGLONG GetMaxElapsedTimeOfMsg(void);

};

template<class CEiMsgMessage>
ULONG CEiMsgQueue<CEiMsgMessage>::CreateQueue(
	const wchar_t* nusMutexName,
	const wchar_t* nusSemaphoreName,
	void* npBuffer,
	ULONG nuBufferSize,
	BOOL nbListener	// TURE: ������
)
{
	ULONG luResult = ERROR_SUCCESS;

	if (nuBufferSize < sizeof(EI_MSGQUE_HEAD) || npBuffer == NULL)
		return ERROR_NOT_ENOUGH_MEMORY;

	if (nbListener != FALSE)
	{	// ������
		// ������Ϣ����
		//��Ҫ����Ȩ�ޣ�������񴴽��Ķ�����ͨ�����޷���
		SECURITY_DESCRIPTOR lsd;
		InitializeSecurityDescriptor(&lsd, SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(&lsd, TRUE, (PACL)NULL, FALSE);
		SECURITY_ATTRIBUTES	lsa;
		lsa.nLength = sizeof(SECURITY_ATTRIBUTES);
		lsa.bInheritHandle = TRUE;
		lsa.lpSecurityDescriptor = &lsd;

		mhNewArrived = CreateSemaphore(&lsa, 0, 10000, nusSemaphoreName);
		if (mhNewArrived == NULL)
			return ERROR_OPEN_FAILED;

		// �����������
		mhMutex = CreateMutex(&lsa, TRUE, nusMutexName);
		if (mhMutex == NULL)
			return ERROR_OPEN_FAILED;

		// ��ʼ����Ϣ���й����ڴ�
		mpQueueHead = (PEI_MSGQUE_HEAD)npBuffer;
		mpMsgBuffer = (CEiMsgMessage*)(mpQueueHead + 1);

		mpQueueHead->HeadSize = sizeof(EI_MSGQUE_HEAD);
		mpQueueHead->BufSize = nuBufferSize - sizeof(EI_MSGQUE_HEAD);
		mpQueueHead->MsgSize = sizeof(CEiMsgMessage);
		mpQueueHead->MsgCapacity = mpQueueHead->BufSize / mpQueueHead->MsgSize;
		mpQueueHead->FirstMsg = 0;
		mpQueueHead->LastMsg = 0;
		mpQueueHead->MsgCount = 0;

		luResult = ERROR_SUCCESS;

		ReleaseMutex(mhMutex);

	}
	else
	{	// �Ǽ�����
		// �����������
		mhNewArrived = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, nusSemaphoreName);
		if (mhNewArrived == NULL)
			return ERROR_OPEN_FAILED;

		// ���ӻ������
		mhMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, nusMutexName);
		if (mhMutex == NULL)
			return ERROR_OPEN_FAILED;
		
		if (WaitForSingleObject(mhMutex, INFINITE) != WAIT_OBJECT_0)
			return ERROR_NOT_READY;

		// �ӹ����ڴ���װ������
		if (((PEI_MSGQUE_HEAD)npBuffer)->MsgSize == sizeof(CEiMsgMessage) &&
			((PEI_MSGQUE_HEAD)npBuffer)->HeadSize == sizeof(EI_MSGQUE_HEAD))
		{
			mpQueueHead = (PEI_MSGQUE_HEAD)npBuffer;
			mpMsgBuffer = (CEiMsgMessage*)(mpQueueHead + 1);
			luResult = ERROR_SUCCESS;
		}

		ReleaseMutex(mhMutex);
	}

	return luResult;
}

// ����һ����Ϣ������ERROR_SUCCESS�����ߴ�����
template<class CEiMsgMessage>
ULONG CEiMsgQueue<CEiMsgMessage>::Push(
	const CEiMsgMessage& nrMsg
)
{
	ULONG luResult = ERROR_SUCCESS;

	if (mbStop != FALSE || WaitForSingleObject(mhMutex, INFINITE) != WAIT_OBJECT_0)
		return ERROR_NOT_READY;

	do 
	{
		if (mpQueueHead->MsgCount >= mpQueueHead->MsgCapacity)
		{
			luResult = ERROR_NOT_ENOUGH_MEMORY;
			break;
		}

		if(mpQueueHead->MsgCount > 0)
			mpQueueHead->LastMsg = ((mpQueueHead->LastMsg + 1) % mpQueueHead->MsgCapacity);

		mpMsgBuffer[mpQueueHead->LastMsg] = nrMsg;
		mpMsgBuffer[mpQueueHead->LastMsg].SaveTickCount();

		mpQueueHead->MsgCount++;

		ReleaseSemaphore(mhNewArrived, 1, NULL);
	} while (false);

	ReleaseMutex(mhMutex);

	return luResult;
}

// ȡһ����Ϣ������ERROR_SUCCESS�����ߴ�����
// �����Ϣ����Ϊ�գ�������ȴ�
template<class CEiMsgMessage>
ULONG CEiMsgQueue<CEiMsgMessage>::Pop(
	CEiMsgMessage& nrMsg
)
{
	ULONG luResult = ERROR_SUCCESS;

	if (mhNewArrived== NULL || WaitForSingleObject(mhNewArrived,INFINITE) != WAIT_OBJECT_0)
		return ERROR_NOT_READY;

	if (mbStop != FALSE)
		return ERROR_NOT_READY;

	if (WaitForSingleObject(mhMutex, INFINITE) != WAIT_OBJECT_0)
		return ERROR_NOT_READY;

	do
	{
		if (mpQueueHead->MsgCount == 0 || mpQueueHead->FirstMsg >= mpQueueHead->MsgCapacity)
		{
			luResult = ERROR_NOT_READY;
			break;
		}

		nrMsg = mpMsgBuffer[mpQueueHead->FirstMsg];
		mpQueueHead->MsgCount--;

		if (mpQueueHead->MsgCount == 0)
			mpQueueHead->FirstMsg = mpQueueHead->LastMsg = 0;
		else
			mpQueueHead->FirstMsg = ((mpQueueHead->FirstMsg + 1) % mpQueueHead->MsgCapacity);

	} while (false);

	ReleaseMutex(mhMutex);

	return luResult;
}

// ����һ����Ϣ���������д�����Ϣȫ������
// ������ô˺�������ҪCEiMsgMessageʵ��bool IsTypeOf(const CEiMsgMessage& nrRefTo)�������ĺ�������true��ʾͬ�࣬����false��ʾ��ͬ��
// ��һ����Ϣ������ʱ�������CEiMsgMessage��Recall()���������ڸú�����ʵ�ֶ���Ϣ���ز���
template<class CEiMsgMessage>
void CEiMsgQueue<CEiMsgMessage>::Recall(
	const CEiMsgMessage& nrMsg
)
{
	ULONG luIndex, luCount;
	if (WaitForSingleObject(mhMutex, INFINITE) != WAIT_OBJECT_0)
		return;

	do
	{
		if (mpQueueHead->MsgCount == 0 || mpQueueHead->FirstMsg >= mpQueueHead->MsgCapacity)
		{
			break;
		}

		luIndex = mpQueueHead->FirstMsg;
		luCount = mpQueueHead->MsgCount;

		while (luCount > 0)
		{
			if (mpMsgBuffer[luIndex].IsTypeOf(nrMsg) != false)
				mpMsgBuffer[luIndex].Recall();

			luIndex = ((luIndex + 1) % mpQueueHead->MsgCapacity);
			luCount--;
		}
	} while (false);

	ReleaseMutex(mhMutex);
}

// ��õ�ǰδȡ�ߵ�����һ����Ϣ�ѷ���ʱ�䣬��λΪ����
template<class CEiMsgMessage>
ULONGLONG CEiMsgQueue<CEiMsgMessage>::GetMaxElapsedTimeOfMsg(void)
{
	ULONGLONG TickCount = 0;

	if (mbStop != FALSE)
		return 0;

	if (WaitForSingleObject(mhMutex, INFINITE) != WAIT_OBJECT_0)
		return 0;

	do
	{
		if(mpQueueHead == NULL || mpMsgBuffer == NULL)
			break;

		if (mpQueueHead->MsgCount == 0 || mpQueueHead->FirstMsg >= mpQueueHead->MsgCapacity)
			break;

		CEiMsgMessage& nrMsg = mpMsgBuffer[mpQueueHead->FirstMsg];

		TickCount = nrMsg.GetElapsedTick();

	} while (false);

	ReleaseMutex(mhMutex);

	return TickCount;
}


// ��ȡ��Ϣ��
template<class CEiMsgMessage>
ULONG CEiMsgQueue<CEiMsgMessage>::GetCount(void)
{
	ULONG luCount = 0;

	if (WaitForSingleObject(mhMutex, INFINITE) == WAIT_OBJECT_0)
	{
		luCount = mpQueueHead->MsgCount;
		ReleaseMutex(mhMutex);
	}

	return luCount;
}

// �ر�
template<class CEiMsgMessage>
void CEiMsgQueue<CEiMsgMessage>::RealseQueue(void)
{
	if(mhNewArrived!=NULL)
	{ 
		SetEvent(mhNewArrived);
		CloseHandle(mhNewArrived);
		mhNewArrived = NULL;
	}

	mbStop = TRUE;
	CloseHandle(mhMutex);
	mhMutex = NULL;
}




// �����࣬��ʼ�����Զ�����һ�������̣߳�������Ϣ����ʱ������ó�ʼ��ʱ�����Ļص�����
template<class CEiMsgMessage>
class CEiMsgQueueListener
{
typedef void (__stdcall *PEI_MSG_LISTENER)(CEiMsgMessage& nrMsg,void* npContext);
private:
	PEI_MSG_LISTENER mpCallBack;
	void* mpContext;
	HANDLE mhListenerThread;
	CEiMsgQueue<CEiMsgMessage> moQueue;
	
	static ULONG WINAPI Listener(CEiMsgQueueListener<CEiMsgMessage>* npThis);

public:
	CEiMsgQueueListener() {
		mhListenerThread = NULL;
	}
	~CEiMsgQueueListener(){}

	// ������ʼ��
	ULONG CreateListener(
		const wchar_t* nusMutexName, 
		const wchar_t* nusSemaphoreName,
		void* npBuffer,
		ULONG nuBufferSize,
		PEI_MSG_LISTENER npCallBack,
		void* npContext
	);

	// ֹͣ����
	ULONG Stop(void);

	// ���Լ�����һ����Ϣ�������ټ����̺߳ͼ����ص������е���!!!
	ULONG PostMsgToListener(const CEiMsgMessage& ncrMsg) {
		return moQueue.Push(ncrMsg);
	}

	// �ٻط��͸���������̵߳�һ����Ϣ���������д�����Ϣȫ������
	// ������ô˺�������ҪCEiMsgMessageʵ��bool IsTypeOf(const CEiMsgMessage& nrRefTo)�������ĺ�������true��ʾͬ�࣬����false��ʾ��ͬ��
	// ��һ����Ϣ������ʱ�������CEiMsgMessage��Recall()���������ڸú�����ʵ�ֶ���Ϣ���ز���
	void Recall(
		const CEiMsgMessage& nrMsg
	)
	{
		moQueue.Recall(nrMsg);
	}


};

template<class CEiMsgMessage>
ULONG CEiMsgQueueListener<CEiMsgMessage>::CreateListener(
	const wchar_t* nusMutexName,
	const wchar_t* nusSemaphoreName,
	void* npBuffer,
	ULONG nuBufferSize,
	PEI_MSG_LISTENER npCallBack,
	void* npContext
	)
{
	ULONG luResult = moQueue.CreateQueue(nusMutexName, nusSemaphoreName, npBuffer, nuBufferSize, TRUE);
	if (luResult != ERROR_SUCCESS)
		return luResult;

	// ���������߳�
	mpCallBack = npCallBack;
	mpContext = npContext;

	mhListenerThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Listener, (LPVOID)this, 0, NULL);
	if (mhListenerThread == NULL)
		return GetLastError();

	return ERROR_SUCCESS;
}

template<class CEiMsgMessage>
ULONG WINAPI CEiMsgQueueListener<CEiMsgMessage>::Listener(CEiMsgQueueListener<CEiMsgMessage>* npThis)
{
	ULONG luResult;
	CEiMsgMessage loMsg;

	while (true)
	{
		try
		{
			while (npThis->moQueue.GetStopState() == FALSE)
			{
				luResult = npThis->moQueue.Pop(loMsg);
				if (luResult == ERROR_SUCCESS)
					npThis->mpCallBack(loMsg, npThis->mpContext);
			}
		}
		catch (...)
		{
			
		}
	}

	return ERROR_SUCCESS;
}

// ֹͣ����
template<class CEiMsgMessage>
ULONG CEiMsgQueueListener<CEiMsgMessage>::Stop(void)
{
	moQueue.RealseQueue();

	return ERROR_SUCCESS;
}


// ��Ϣ���������������ӵ���Ϣͨ��������Ϳ��Ը���Ϣͨ��������Ϣ��
template<class CEiMsgMessage>
class CEiMsgQueueConnector
{
private:
	CEiMsgQueue<CEiMsgMessage> moQueue;

public:
	CEiMsgQueueConnector() {
	}
	~CEiMsgQueueConnector() {
	}

	// ��������ʼ��
	ULONG CreateConnector(
		const wchar_t* nusMutexName,
		const wchar_t* nusSemaphoreName,
		void* npBuffer,
		ULONG nuBufferSize
	) {
		return moQueue.CreateQueue(nusMutexName, nusSemaphoreName, npBuffer, nuBufferSize, FALSE);
	}

	// ����һ����Ϣ
	ULONG PostMsg(const CEiMsgMessage& ncrMsg) {
		return moQueue.Push(ncrMsg);
	}

	// ����һ����Ϣ���������д�����Ϣȫ������
	// ������ô˺�������ҪCEiMsgMessageʵ��bool IsTypeOf(const CEiMsgMessage& nrRefTo)�������ĺ�������true��ʾͬ�࣬����false��ʾ��ͬ��
	// ��һ����Ϣ������ʱ�������CEiMsgMessage��Recall()���������ڸú�����ʵ�ֶ���Ϣ���ز���
	void Recall(
		const CEiMsgMessage& nrMsg
	)
	{
		moQueue.Recall(nrMsg);
	}

	// ��õ�ǰδȡ�ߵ�����һ����Ϣ�ѷ���ʱ�䣬��λΪ����
	ULONGLONG GetMaxElapsedTimeOfMsg(void){
		return moQueue.GetMaxElapsedTimeOfMsg();
	}
};
