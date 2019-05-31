/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _EOB_FACTORY_MGR_
#define _EOB_FACTORY_MGR_


// �����������
// ���ڼ���DLL������DLL�����ע��DLL������DLL


DECLARE_BUILTIN_NAME(CXelFactoryMgr)
class CXelFactoryMgr : public cmmBaseObject<CXelFactoryMgr, IBaseObject, GET_BUILTIN_NAME(CXelFactoryMgr)>
{
public:
	CXelFactoryMgr();
	virtual ~CXelFactoryMgr();
	
public:
	// ���ع�����DLL
	HMODULE	__stdcall LoadDll(const wchar_t* nswDllPath);
	// ж�ع�����DLL
	BOOL __stdcall UnLoadDll(HMODULE nhDllHandle);


	// ʵ���������࣬��ȡ��ӿڡ��䱾���ǣ����ù�����DLL��һ��������������������ʵ����������
	// nuDllHandle1��DLLģ����
	IElementFactory* __stdcall GetFactInstance(const wchar_t* nswDllPath);

private:

};





#endif