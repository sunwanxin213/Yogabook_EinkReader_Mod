#pragma once
#include "AcWMI.h"
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")


// reset tp area
#define WM_BIOS_EVENT_RESET_TP_AREA WM_USER + 0x917
//WParam: NA
//LParam: NA

// �ָ�C��״̬
#define WM_BIOS_EVENT_8951_STATUS_CHANGE WM_USER + 0x918
//WParam: NA
//LParam: NA

class EventSink : public IWbemObjectSink {
	LONG m_lRef;
	bool bDone;

public:
	EventSink(HANDLE PenBtEvt);
	~EventSink();

	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT
		STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);

	virtual HRESULT STDMETHODCALLTYPE Indicate(
		LONG lObjectCount,
		IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray
	);

	virtual HRESULT STDMETHODCALLTYPE SetStatus(
		/* [in] */ LONG lFlags,
		/* [in] */ HRESULT hResult,
		/* [in] */ BSTR strParam,
		/* [in] */ IWbemClassObject __RPC_FAR *pObjParam
	);

private:
	HANDLE m_PenBtEvt;

};

class EventResetTpArea : public IWbemObjectSink {
	LONG m_lRef;
	bool bDone;

public:
	EventResetTpArea(HANDLE PenBtEvt);
	~EventResetTpArea();

	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT
		STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);

	virtual HRESULT STDMETHODCALLTYPE Indicate(
		LONG lObjectCount,
		IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray
	);

	virtual HRESULT STDMETHODCALLTYPE SetStatus(
		/* [in] */ LONG lFlags,
		/* [in] */ HRESULT hResult,
		/* [in] */ BSTR strParam,
		/* [in] */ IWbemClassObject __RPC_FAR *pObjParam
	);

private:
	HANDLE m_PenResettpEvt;

};

//8951 wake up
class Event8951Wakeup : public IWbemObjectSink {
	LONG m_lRef;
	bool bDone;

public:
	Event8951Wakeup(HANDLE PenBtEvt);
	~Event8951Wakeup();

	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT
		STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);

	virtual HRESULT STDMETHODCALLTYPE Indicate(
		LONG lObjectCount,
		IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray
	);

	virtual HRESULT STDMETHODCALLTYPE SetStatus(
		/* [in] */ LONG lFlags,
		/* [in] */ HRESULT hResult,
		/* [in] */ BSTR strParam,
		/* [in] */ IWbemClassObject __RPC_FAR *pObjParam
	);

private:
	HANDLE m_PenResettpEvt;

};


//8951 sleep
class Event8951Sleep : public IWbemObjectSink {
	LONG m_lRef;
	bool bDone;

public:
	Event8951Sleep(HANDLE PenBtEvt);
	~Event8951Sleep();

	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT
		STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);

	virtual HRESULT STDMETHODCALLTYPE Indicate(
		LONG lObjectCount,
		IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray
	);

	virtual HRESULT STDMETHODCALLTYPE SetStatus(
		/* [in] */ LONG lFlags,
		/* [in] */ HRESULT hResult,
		/* [in] */ BSTR strParam,
		/* [in] */ IWbemClassObject __RPC_FAR *pObjParam
	);

private:
	HANDLE m_PenResettpEvt;

};

class BCoverCtrl {
public:
	BCoverCtrl();
	~BCoverCtrl();

	//�����𶯼���
	HRESULT SetHapticLevel(int niLevel);
	//���ر���
	HRESULT BackLightControl(BOOL DoEnable);
	//����touch
	HRESULT TouchControl(BOOL enable);
	//����knock knock
	HRESULT KnockKncokControl(BOOL enable);
	//����8951
	HRESULT Reset8951();
	//��ʼ��
	HRESULT initHighTempEvt(HWND nhWnd);
	//��ʼ��reset tp area
	HRESULT initResetTpAreaEvt(HWND nhWnd);
	//��ʼ��8951 wake up�¼�
	HRESULT init8951WakeupEvt(HWND nhWnd);
	//��ʼ��8951 sleep�¼�
	HRESULT init8951SleepEvt(HWND nhWnd);

	HRESULT InitBCoverMethod();
	HRESULT UninitBCoverMethod();

private:
	HANDLE m_PenBtEvt;
	HANDLE m_PenResettpEvt;
	HANDLE m_PenResetCCoverEvt;
	HANDLE m_Pen8951SleepEvt;
	CAccessWmi m_AcWmi;


	CString m_strPath;
	IUnsecuredApartment* m_pUnsecApp;
	EventSink *m_EvtSink;
	EventResetTpArea *m_EventResetTpArea;
	Event8951Wakeup *m_Event8951Wakeup;
	Event8951Sleep *m_Event8951Sleep;
	IUnknown* m_pStubUnk;
	IWbemObjectSink* m_pStubSink;

	wchar_t mszName[MAX_PATH];
};


