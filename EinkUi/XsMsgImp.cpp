#include "stdafx.h"

#include "CommonHeader.h"
#include "XsMsgImp.h"









DEFINE_BUILTIN_NAME(CXuiMessage)


CXuiMessage::CXuiMessage()
{
	// �Գ�Ա������һЩ��Ҫ�ĳ�ʼ��
	muMsgID = 0;
	miInputDataSize = 0;
	miOutputDataSize = 0;
	mpMsgDestItr = NULL;
	mpInputData = NULL;
	mpMsgOutputBuffer = NULL;
	muResult = ERESULT_UNSUCCESSFUL;
	mhCompleteEvent = NULL;
	mpSender = NULL;
	miOutputBufferSize = 0;
}
CXuiMessage::~CXuiMessage()
{
	FreeInputBuffer();
}

void CXuiMessage::FreeInputBuffer()
{
	if (mpInputData == NULL && miInputDataSize == 0)
		return;

	// ������Ϣ��־λ�ж��Ƿ���Ҫ�ͷŻ�����
	if (cmmBaseObject::TestFlag(ELMSG_FLAG_INPUT_ALLOCATED) != false && mpInputData != NULL && mpInputData != mbufInputBuffer)
		delete mpInputData;//delete mpInputData;	//hy 20190723

	mpInputData = NULL;
	miInputDataSize = 0;

	cmmBaseObject::SetFlags(ELMSG_FLAG_INPUT_ALLOCATED, false);
	//cmmBaseObject<CXuiMessage,IEinkuiMessage,GET_BUILTIN_NAME(CXuiMessage)>::SetFlags(ELMSG_FLAG_INPUT_COPIED,false);
}

// ���ñ��ṹ
void CXuiMessage::Reuse()
{

	FreeInputBuffer();

	muMsgID = 0;
	miOutputDataSize = 0;
	mpMsgDestItr = NULL;
	mpMsgOutputBuffer = NULL;
	muResult = ERESULT_UNSUCCESSFUL;
	mhCompleteEvent = NULL;
	mpSender = NULL;
	miOutputBufferSize = 0;

	cmmBaseObject<CXuiMessage, IEinkuiMessage, GET_BUILTIN_NAME(CXuiMessage)>::Reuse();
}


// �ͷŶ���
int __stdcall CXuiMessage::Release(void)
{
	static CXuiMessage* glMsg = NULL;
	if (glMsg == this)
		glMsg->Release();
	int liCount = cmmBaseObject<CXuiMessage, IEinkuiMessage, GET_BUILTIN_NAME(CXuiMessage)>::Release();
	// �������ֵ��1�����ʾ����Ԫ�ع�������һ�������⣬�Ѿ�û�б����������Ԫ���ˣ���֪ͨԪ�ع�����׼���ͷ�
	if (liCount == 1)
	{
		// ֪ͨԪ�ع�����
		CEinkuiSystem::gpXuiSystem->GetElementManagerObject()->ReleaseMessage(this);
	}

	return liCount;
}

// �����Ϣ�Ľ���Ŀ���Iterator�ӿ�
IEinkuiIterator* __stdcall CXuiMessage::GetDestination(void)
{
	return mpMsgDestItr;

}

// ������Ϣ�Ľ���Ŀ��
ERESULT __stdcall CXuiMessage::SetDestination(IEinkuiIterator* npMsgDestItr)
{
	if (npMsgDestItr != NULL)
	{
		mpMsgDestItr = npMsgDestItr;
		return ERESULT_SUCCESS;
	}
	else
	{
		return ERESULT_UNSUCCESSFUL;
	}

}

// �����ϢID
ULONG __stdcall CXuiMessage::GetMessageID(void)
{
	return muMsgID;
}

// ������ϢID
ERESULT __stdcall CXuiMessage::SetMessageID(ULONG nuMsgID)
{
	// �����ж���ϢID����Ч��
	muMsgID = nuMsgID;
	return ERESULT_SUCCESS;
}

// ��÷���ֵ
ERESULT __stdcall CXuiMessage::GetResult(void)
{
	return muResult;
}

// ���÷���ֵ
ERESULT __stdcall CXuiMessage::SetResult(ERESULT nuResult)
{
	muResult = nuResult;
	return ERESULT_SUCCESS;
}

// ������Ϣ�����ߣ�����Ϣ�����ڴӿؼ���ؼ�ʹ���߷����¼����ؼ���������Ϣ��ʱ����Ҫ������Ϣ�ķ����ߣ�Iterator�ӿ��ṩ��PostMessageToParent�������Զ����÷�����
void __stdcall CXuiMessage::SetMessageSender(IEinkuiIterator* npSender)
{
	mpSender = npSender;
}

// �����Ϣ�ķ�����
IEinkuiIterator* __stdcall CXuiMessage::GetMessageSender(void)
{
	return mpSender;
}


// ����Input���ݣ������Ϣ�Ǳ�Post�ģ�nbVolatile����Ϊture�������Ϣ�Ǳ�Send��Ŀ��ģ������ղ���nbVolatile�����Ƿ������뻺��
ERESULT __stdcall CXuiMessage::SetInputData(
	const void* npBuffer,
	int niSize,
	bool nbVolatile	// true:�˻���������ʧ�ģ���Ҫ�������ݵ��ڲ�����; false �˻������Ƿ���ʧ�ģ�����Ϣ���ͺͷ��صĹ�������Ч
)
{
	ERESULT luResult;

	if (niSize < 0 || npBuffer == NULL && niSize != 0)
		return ERESULT_UNSUCCESSFUL;

	FreeInputBuffer();

	if (nbVolatile != false && npBuffer != NULL)
	{
		luResult = CopyInputData(npBuffer, niSize);
	}
	else
	{
		// npBuffer == NULL or nbVolatile == false
		mpInputData = (void*)npBuffer;
		miInputDataSize = niSize;

		luResult = ERESULT_SUCCESS;
	}

	return ERESULT_SUCCESS;
}

// ���Input����ָ�룬ע�⣬��õ�ָ����ڳ�����Ϣ������û�з����ı�ʱ��Ч��һ������Ϣ���ͳ�ȥ�����ߵ�������Ϣ���趨ֵ�ķ������������¸�ָ��ʧЧ
// ע�⣬�˷�����õ�ָ�벢��һ��ͬǰһ�ε���SetInputData�趨��ָ����ͬ
const void* __stdcall CXuiMessage::GetInputData(void)
{
	return mpInputData;
}

// ������뻺�����Ĵ�С
int __stdcall CXuiMessage::GetInputDataSize(void)
{
	return miInputDataSize;
}

// ����Output�����������������Ϣ����Output Buffer�������Ҫ��Ϣ���ش������ݵģ�Ӧ��Send������Ϣ��������Post�������ȷʵ��ҪPost������Ϣ����ô��ο���������÷�����Ϣ�ķ���;
// ���ѡ��Post������Ϣ����ǧ��֤�趨��Output�����������޸ĺ��ͷţ�������Ϣ�Ľ��ܷ��Ըû��������ʲ�������
ERESULT __stdcall CXuiMessage::SetOutputBuffer(void* npBuffer, int niSize)
{
	if (niSize <= 0)
		return ERESULT_UNSUCCESSFUL;

	miOutputBufferSize = niSize;

	if (npBuffer == NULL)
		return ERESULT_UNSUCCESSFUL;
	mpMsgOutputBuffer = npBuffer;

	return ERESULT_SUCCESS;

}

// ���Output������ָ�룬ע�⣬��õ�ָ����ڳ��и���Ϣʱ��Ч��
void* __stdcall CXuiMessage::GetOutputBuffer(void)
{
	return mpMsgOutputBuffer;
}

// ���Output�������Ĵ�С
int __stdcall CXuiMessage::GetOutputBufferSize(void)
{
	return miOutputBufferSize;
}

// ��������Output�����������ݴ�С
void CXuiMessage::SetOutputDataSize(int niSize)
{
	miOutputDataSize = niSize;
}

// ���Output�����������ݴ�С
int CXuiMessage::GetOutputDataSize(void)
{
	return miOutputDataSize;
}

// ���÷�����Ϣ������Ϣ��Ŀ���������Ϻ�ϵͳ���Զ�����һ���µ���Ϣ���ظ������ߣ�����µ���Ϣ����������Ϣ
ERESULT __stdcall CXuiMessage::SetResponseMessage(
	IN IEinkuiIterator* npReceiver,	// ���ܷ�����Ϣ��Ŀ��
	IN void* npContext	// ���������ģ������������ú�ʹ�ã�������Ϣ������ʱ�����ݸ�������
)
{
	return ERESULT_UNSUCCESSFUL;//???��δʵ��
}


// �����û��ṩ����������
ERESULT CXuiMessage::CopyInputData(
	const void* npBuffer,
	int niSize
)
{
	ERESULT luResult = ERESULT_SUCCESS;

	if (npBuffer != NULL)
	{
		if (niSize < ELMSG_BUILDIN_BUFSIZE)
		{
			mpInputData = mbufInputBuffer;
		}
		else
		{
			mpInputData = new char[niSize];
			cmmBaseObject::SetFlags(ELMSG_FLAG_INPUT_ALLOCATED, true);
		}

		if (mpInputData != NULL)
		{
    		// CheckMarx fix by zhuhl5
			memcpy_s(mpInputData, niSize, npBuffer, niSize);

			miInputDataSize = niSize;

			luResult = ERESULT_SUCCESS;
		}
		else
			luResult = ERESULT_INSUFFICIENT_RESOURCES;
	}

	return luResult;
}

// ��Ϣ����������ȷ����ǰ��Ϣ�Ƿ�Send����������true,the message was sent from source element; false, the message was posted by the sender;
bool __stdcall CXuiMessage::IsSent(void)
{
	return cmmBaseObject::TestFlag(ELMSG_FLAG_SEND);
}
