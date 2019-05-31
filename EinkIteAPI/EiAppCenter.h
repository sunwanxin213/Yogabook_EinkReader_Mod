/* Copyright 2019 - present Lenovo */
/* License: COPYING.GPLv3 */
#pragma once
#include "EiMsgQueue.h"
#include "SvrMsg.h"


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

	// �ص���ں���
	static void __stdcall AppCenterCallBack(CEiSvrMsgItem& nrMsg, void* npContext);

	// ���ַ�����
	void AppDispatch(CEiSvrMsgItem& nrMsg);

	// ������Ϣ��Service�����ȴ�
	ULONG SendMessageToService(CEiSvrMsgItem& nrMsg);


	// ������Ϣ��Service�����ȴ�
	ULONG PostMessageToService(CEiSvrMsgItem& nrMsg);

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
	//��˽����״̬�����仯
	void PrivacyStatusChanged(CEiSvrMsgItem& nrMsg);
private:
	//��ȡGUID�ַ���
	void GetGUIDString(OUT const wchar_t* npszBuffer, int niLen);
	//���Լ����ڴ�ӳ���ļ�
	bool OpenJasonFile(const wchar_t* nszFileName);
	//���Լ����ڴ�ӳ���ļ�
	bool OpenServerJasonFile(const wchar_t* nszFileName);


	//�Լ�����������
	HANDLE mhFile;
	DWORD muFileLength;
	HANDLE mhFileMap;
	const char* mpMappedBase;

	//����������������
	HANDLE mhServerFile;
	DWORD muServerFileLength;
	HANDLE mhServerFileMap;
	const char* mpServerMappedBase;

	REG_APP_INFO mdRegAppInfo;

	//����windows��Ϣ�Ĵ��ھ��
	HWND mhWnd;
};

