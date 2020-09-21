#pragma once

class CProcessStarter
{
public:
	CProcessStarter();
	~CProcessStarter();

	// ���û���ҵ�"�Ѿ������SessionId", ˵����û�н�������
	static BOOL FindActiveSessionId(OUT DWORD& dwSessionId, BOOL bNeedLog);
	//���nbIsSystem=true��ֱ��ʹ��system�û�����
	//npszCommndLine������ǰ��һ��Ҫ��һ���ո񣬷����޷��ɹ�����
	static DWORD Run(const wchar_t* pcProcessPathName, HANDLE& rProHandle, int niSessionID = -1, wchar_t* npszCommndLine = NULL);
	//��ȡ��ǰ�û�ID��0��ʾû�м����û�
	static DWORD GetSessionID(void);
	//��ȡ��ǰ�û�SDK,���ص��ַ����ڲ��õ�ʱ�����LocalFree�ͷ�
	static wchar_t* GetUserSID(int niSessionID = -1);
	//��ȡ��ǰ�û�Token
	static HANDLE GetCurrentUserToken();
private:



};
