#include "stdafx.h"

#include "CommonHeader.h"
#include "XsSysElement.h"

DEFINE_BUILTIN_NAME(CEleMgrProxy)




const wchar_t* CEleMgrProxy::GetType(void)
{
	return GetObjectName();
}

bool CEleMgrProxy::GlobleVerification(const wchar_t* nswType)	// ��֤�˶����Ƿ���nswTypeָ��������
{
	return (_wcsicmp(nswType, GetObjectName()) == 0);
}

// Ĭ����Ϣ��ں��������ڽ���������Ϣ
ERESULT __stdcall CEleMgrProxy::MessageReceiver(IEinkuiMessage* npMsg)
{
	return CEinkuiSystem::gpXuiSystem->SystemMessageReceiver(npMsg);
}

// ��ñ�Ԫ�صĵ������ӿ�
IEinkuiIterator* __stdcall CEleMgrProxy::GetIterator(void)
{
	return NULL;
}


