/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */
#pragma once
#include "EiMsgQueue.h"
#include "SvrMsg.h"
#include "itetcon.h"

// ����SDK��App������Service�������Ϣ��������
// ����Initialize��ʼ����ִ������ڽ��߳̾ͻ�����������������ӦService����������Ϣ������һ����Ч��תΪWindows��Ϣ���͸�App������
// �˳�ʱ������Relaese�����ͷ�

class CEiAppCenter
{
public:
	CEiMsgQueueListener<CEiSvrMsgItem> moAppListener;
	CEiMsgQueueConnector<CEiSvrMsgItem> moConnectToHost;
	ULONG muAppID;

	CEiAppCenter();
	~CEiAppCenter();

	// ��ʼ��
	ULONG Initialize(TRSP_SYSTEM_INFO_DATA& rSystemInfoData);

	// �ͷ�
	void Release(void);

	// �������ݻص���ں���
	static void __stdcall AppReciveCallBack(const char* npData, ULONG nSize, void* npContext);
	// ��������Ԥ������
	void AppPushReciveDataToQueue(const char* npData, ULONG nSize);

	// �ص���ں���
	static void __stdcall AppCenterCallBack(CEiSvrMsgItem& nrMsg, void* npContext);
	// ���ַ�����
	void AppDispatch(CEiSvrMsgItem& nrMsg);

	// ������Ϣ��Service�����ȴ�
	ULONG SendMessageToService(CEiSvrMsgItem& nrMsg);

	// ������Ϣ�ͻ������ݸ�Service�����ȴ�
	ULONG SendMessageToService(CEiSvrMsgItem& nrMsg, EI_BUFFER* npBuffer);

	// ������Ϣ��Service�����ȴ�
	ULONG PostMessageToService(CEiSvrMsgItem& nrMsg);

	// ������Ϣ��Service�����ȴ�
	ULONG PostMessageToService(CEiSvrMsgItem& nrMsg, EI_BUFFER* npBuffer);

	// ����һ����Ϣ���������д�����Ϣȫ������
	void RecallMessage(const CEiSvrMsgItem& nrMsg);

	//���ý���windows��Ϣ�Ĵ��ھ��
	void SetHwnd(HWND nHwnd);

	//////////////////////////////////////////////////////////////////////////
	// ����Ϊ������Ϣ����Ӧ����
	void MsgBack(CEiSvrMsgItem& nrMsg);

	//��ȡ�������õ�Buffer��ַ
	//����ֵΪ��ַ��ʼ��ַ��rulBufferSizeΪBuffer��С
	BYTE* GetBufferBase(ULONG& rulBufferSize);

	// ��ָ������Ϣ
	void InputMsg(CEiSvrMsgItem& nrMsg);
	// ȫ���ػ�
	void ReDraw(CEiSvrMsgItem& nrMsg);
	// Z�ᷢ���仯
	void ZOrderChange(CEiSvrMsgItem& nrMsg);
	// Eink��Ļ�������仯
	void EinkScreenOrientationChange(CEiSvrMsgItem& nrMsg);
	// ������̬�����仯
	void LaptopModeChange(CEiSvrMsgItem& nrMsg);
	// ����֪ͨ�¼�
	void ServiceMsg(CEiSvrMsgItem& nrMsg);
	// homebar״̬�����仯
	void HomebarChanged(CEiSvrMsgItem& nrMsg);
	// ������ʽ�л����
	void KeyboardStyleChangeComplete(CEiSvrMsgItem& nrMsg);
	// ��������tp area
	void ResetTPArea(CEiSvrMsgItem& nrMsg);
	// ��˽����״̬�����仯
	void PrivacyStatusChanged(CEiSvrMsgItem& nrMsg);
	// ��ӦService�˵ļ��
	void CheckAppAlive(CEiSvrMsgItem& nrMsg);
	// ��鷢��Ϣ���ʱ���Ƿ��㹻��������20�뷢��������������Ϣ
	void CheckAndSendNormalRunMsg();
	// smartinfo���÷����˱仯
	void SmartInfoSettingChange(CEiSvrMsgItem& nrMsg);

private:
	//��ȡGUID�ַ���
	void GetGUIDString(wchar_t* npszBuffer, int niLen);
	//������״̬
	bool ProtectService(void);
	// ��������
	void CreateAppBuffer();
	// �ͷŻ���
	void ReleaseAppBuffer();
	// ��ע�������
	LSTATUS GetRegSZ(LPCWSTR lpValueName, wchar_t* value);

private:
	// �������ݻ���
	DWORD muSendBufferLength;
	char* mSendBuffer;

	// add �������ݻ���
	DWORD muReciveBufferLength;
	char* mpReciveBuffer;

	REG_APP_INFO mdRegAppInfo;

	//����windows��Ϣ�Ĵ��ھ��
	HWND mhWnd;
};

