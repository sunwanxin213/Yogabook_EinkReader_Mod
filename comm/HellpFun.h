#pragma once
class CHellpFun
{
public:
	CHellpFun();
	~CHellpFun();

	//������״̬
	static void CheckService(void);
	//�����̼߳�ط���״̬���������û�ˣ�����������
	static void ProcService(void);
	//ͨ���������ƻ�ȡ����PID
	static DWORD GetPIDByName(wchar_t* npszName);
	//�ػ��߳�
	static bool ProtectThread(LPVOID npData);
	//ɱ��ָ������
	static BOOL KillProc(DWORD ProcessID);
	//ɱ��ĳЩ����
	static void KillProcByName(wchar_t* npszName);
};

