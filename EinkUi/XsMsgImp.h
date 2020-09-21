#ifndef _ELMSGIMP_H_
#define _ELMSGIMP_H_
/*
	���ļ�����Message�����ʵ��

*/

// �������û�������С�����Բ���С��64���ֽ�
#define ELMSG_BUILDIN_BUFSIZE 512



DECLARE_BUILTIN_NAME(CXuiMessage)
class CXuiMessage : public cmmBaseObject<CXuiMessage, IEinkuiMessage, GET_BUILTIN_NAME(CXuiMessage)>
{
	friend class CXelManager;
	friend class CXuiMessageQueue;
public:

	DEFINE_CUMSTOMIZE_CREATE(CXuiMessage, (), ())

		// �ͷŶ���
		virtual int __stdcall Release(void);

	// �����Ϣ�Ľ���Ŀ���Iterator�ӿ�
	virtual IEinkuiIterator* __stdcall GetDestination(void);

	// ������Ϣ�Ľ���Ŀ��
	virtual ERESULT __stdcall SetDestination(IEinkuiIterator* npMsgDestItr);

	// �����ϢID
	virtual ULONG __stdcall GetMessageID(void);

	// ������ϢID
	virtual ERESULT __stdcall SetMessageID(ULONG nuMsgID);

	// ��÷���ֵ
	virtual ERESULT __stdcall GetResult(void);

	// ���÷���ֵ���������ص��Ǳ������ĳɹ����
	virtual ERESULT __stdcall SetResult(ERESULT nuResult);

	// ������Ϣ�����ߣ�����Ϣ�����ڴӿؼ���ؼ�ʹ���߷����¼����ؼ���������Ϣ��ʱ����Ҫ������Ϣ�ķ����ߣ�Iterator�ӿ��ṩ��PostMessageToParent�������Զ����÷�����
	virtual void __stdcall SetMessageSender(IEinkuiIterator* npSender);

	// �����Ϣ�ķ�����
	virtual IEinkuiIterator* __stdcall GetMessageSender(void);

	// ����Input���ݣ������Ϣ�Ǳ�Post�ģ�nbVolatile����Ϊture�������Ϣ�Ǳ�Send��Ŀ��ģ������ղ���nbVolatile�����Ƿ������뻺��
	virtual ERESULT __stdcall SetInputData(
		const void* npBuffer,	// ע�⣬���ϣ�����ݵĲ�����һ��ָ�뱾���磺lpAnybody��Ӧ����˵���SetInputData(&lpAnybody,sizeof(lpAnybody),ture/false);
		int niSize,
		bool nbVolatile = true	// true:�˻���������ʧ�ģ���Ҫ�������ݵ��ڲ�����; false �˻������Ƿ���ʧ�ģ�����Ϣ���ͺͷ��صĹ�������Ч
	);

	// ���Input����ָ�룬ע�⣬��õ�ָ����ڳ�����Ϣ������û�з����ı�ʱ��Ч��һ������Ϣ���ͳ�ȥ�����ߵ�������Ϣ���趨ֵ�ķ������������¸�ָ��ʧЧ
	// ע�⣬�˷�����õ�ָ�벢��һ��ͬǰһ�ε���SetInputData�趨��ָ����ͬ
	virtual const void* __stdcall GetInputData(void);

	// ������뻺�����Ĵ�С
	virtual int __stdcall GetInputDataSize(void);

	// ����Output�����������������Ϣ����Output Buffer�������Ҫ��Ϣ���ش������ݵģ�Ӧ��Send������Ϣ��������Post�������ȷʵ��ҪPost������Ϣ����ô��ο���������÷�����Ϣ�ķ���;
	// ���ѡ��Post������Ϣ����ǧ��֤�趨��Output�����������޸ĺ��ͷţ�������Ϣ�Ľ��ܷ��Ըû��������ʲ�������
	virtual ERESULT __stdcall SetOutputBuffer(void* npBuffer, int niSize);

	// ���Output������ָ�룬ע�⣬��õ�ָ����ڳ��и���Ϣʱ��Ч��
	virtual void* __stdcall GetOutputBuffer(void);

	// ���Output�������Ĵ�С
	virtual int __stdcall GetOutputBufferSize(void);


	// ��������Output�����������ݴ�С
	// ��������������Ϣ�����ͺ���ã�����Ϣ�Ľ����ߵ���
	virtual void SetOutputDataSize(int niSize);
	// ���Output�����������ݴ�С
	virtual int GetOutputDataSize(void);

	// ���÷�����Ϣ������Ϣ��Ŀ���������Ϻ�ϵͳ���Զ�����һ���µ���Ϣ���ظ������ߣ�����µ���Ϣ����������Ϣ
	virtual ERESULT __stdcall SetResponseMessage(
		IN IEinkuiIterator* npReceiver,	// ���ܷ�����Ϣ��Ŀ��
		IN void* npContext = NULL	// ���������ģ������������ú�ʹ�ã�������Ϣ������ʱ�����ݸ�������
	);

	// ��Ϣ����������ȷ����ǰ��Ϣ�Ƿ�Send����������true,the message was sent from source element; false, the message was posted by the sender;
	virtual bool __stdcall IsSent(void);

protected:

	CXuiMessage();
	~CXuiMessage();

	// ���ñ��ṹ
	void Reuse();

protected:
	// ��Ϣ�����ߵĵ�����ָ��
	IEinkuiIterator* mpMsgDestItr;
	// ��Ϣ������
	IEinkuiIterator* mpSender;

	// ��ָ��ָ����Ϣ������,����������post��Ϣʱ����Ҫ������Ϣ������	
	void* mpInputData;

	// ��Ϣ�������Ĵ�С	
	int	 miInputDataSize;

	// ��Ϣ��outputBuffer������ָ��
	void* mpMsgOutputBuffer;
	// ��Ϣ��outputBuffer�������Ĵ�С
	int miOutputBufferSize;
	// ��¼OutputBuffer�����ݵĶ���
	int miOutputDataSize;

	// ��ǰ��Ϣ�������ϢID	
	ULONG muMsgID;

	// ���ĳһ�����ķ���ֵ
	ERESULT muResult;

	// ��Ϣ��ɣ���ϵͳ��ͬ���¼�
	HANDLE mhCompleteEvent;

	// 1K��С�����뻺����
	UCHAR mbufInputBuffer[ELMSG_BUILDIN_BUFSIZE];

	// �����û��ṩ����������
	__inline ERESULT CopyInputData(
		const void* npBuffer,
		int niSize	// �������0
	);

	__inline void FreeInputBuffer();

};


#define	ELMSG_FLAG_INPUT_ALLOCATED  0 		// ��ʾInput��������Ҫ�ͷ�
#define ELMSG_FLAG_SEND		1		// ��ʾ��Ϣ��Send������δ���ñ�ʾ��Ϣ�Ǳ�Post����
//#define ELMSG_FLAG_FAST		2		// ��ʾ��Ϣ����Fast���ԣ�������ϢӦ�ð��Ⱥ���������Ϣ���е�ǰ�����ȴ�����
//#define ELMSG_FLAG_REVERSE	3		// ����Ϣ����ͬʱ����������Ϣ��ʱ�򣬽��ᱻ����ȳ��ķ�ʽ��������Ϣ�����У����ǵ����ȼ�������ͨ��Ϣ��������Fast��Ϣ
//
//#if (EMSG_POST_FAST != ELMSG_FLAG_FAST || ELMSG_FLAG_REVERSE != EMSG_POST_REVERSE)
//#error "Must set the same value"
//#endif







































#endif//_ELMSGIMP_H_