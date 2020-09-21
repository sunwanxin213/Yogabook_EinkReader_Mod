/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */
#pragma once

//��ȡ����BUFFER��С
#define USBHID_BUFFER_SIZE 1024
#define USBHID_TP_REPORT_ID 0x90
#define USBHID_EMR_REPORT_ID 0x91

class USBHIDAPI
{
public:
	USBHIDAPI(SIZE& nrPanel);
	~USBHIDAPI();

	BOOL EnumHIDDevice(WORD uVID, WORD uPID, //USB VID PID
		BOOL bPresentFlag, //�豸������ڱ�־ 0����Ҫ�����豸
		TCHAR szDevPath[MAX_PATH + 1] = NULL, //�豸·��
		int iIndex = 0); //��N���豸 ���Զ����ͬ���豸�������֣�


private:
	// ��������ת����
	SIZE mstPanel;
	//��д���豸��������ڶ�ȡ��������
	HANDLE mhReadHandle;
	//���ݶ�ȡ���֪ͨ�¼�
	HANDLE mhEventObject;
	//�ȴ���ȡ����߳�
	HANDLE mhReadThread;
	//��ȡ�ļ��ṹ��
	OVERLAPPED mdHIDOverlapped;
	wchar_t mszDevPath[MAX_PATH + 1];
	//��ȡ�����ڴ�
	//PVOID mInputReport;
	BYTE mInputReport[USBHID_BUFFER_SIZE];

	//��ʼ��ȡ����
	void BeginRead();
	//��ȡ�����߳�
	static DWORD __stdcall ReadFileThread(LPVOID npParam);
	

};

