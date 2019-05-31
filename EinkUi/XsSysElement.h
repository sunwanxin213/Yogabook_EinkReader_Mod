/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once


//////////////////////////////////////////////////////////////////////////
// Ԫ�ع���������Ԫ�أ����������з��͸�Ԫ�ع���������Ϣ
DECLARE_BUILTIN_NAME(CEleMgrProxy)
class CEleMgrProxy: public cmmBaseObject<CEleMgrProxy,IXsElement,GET_BUILTIN_NAME(CEleMgrProxy)>
{
public:
	// ��ñ�Ԫ�ص�Ԫ������
	virtual const wchar_t* __stdcall  GetType(void);

	// ��ȫ�ַ�Χ����֤�˶����Ƿ���nswTypeָ��������
	virtual bool __stdcall  GlobleVerification(const wchar_t* nswType);

	// Ĭ����Ϣ��ں��������ڽ���������Ϣ
	virtual ERESULT __stdcall MessageReceiver(IEinkuiMessage* npMsg);

	// ��ñ�Ԫ�صĵ������ӿ�
	virtual IEinkuiIterator* __stdcall GetIterator(void);

	// ���Tab Order, -1 δ����
	virtual LONG __stdcall GetTabOrder(void) {
		return 0;
	}

	// ���Z Order��-1 δ����
	virtual LONG __stdcall GetZOrder(void){
		return 0;
	}

};


