/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once
#include "FileListItem.h"
#include "cmmstruct.h"

//���ļ��Ի���

DECLARE_BUILTIN_NAME(FileOpenDlg)

class CFileOpenDlg:
	public CXuiElement<CFileOpenDlg,GET_BUILTIN_NAME(FileOpenDlg)>
{
public:
	// ��������캯���趨Ϊprotected������Ҫ����仰; ���򣬲���Ҫ�������
	friend CXuiElement<CFileOpenDlg,GET_BUILTIN_NAME(FileOpenDlg)>;

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	// �������༰����������ʱ�����ر�ע��!!! һ��Ҫ���ȵ��û���ķ���
	// �����������ڽ�����Ԫ�ض��󡢳�ʼ���������ݣ�λ�á���С������ͼ�ȣ�
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent,	// ������ָ��
		IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32		// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��EUIϵͳ�Զ�����
		);

	//��ʱ��
	virtual void OnTimer(
		PSTEMS_TIMER npStatus
		);

public:
	CFileOpenDlg();
	~CFileOpenDlg(void);

	// ģ̬��ʾ�öԻ���
	void DoModal(bool nbIsEnableCancel = true);
	//������ʷ��¼
	void SetHistoryList(cmmVector<wchar_t*>* npdHistroyPath);

	void ExitModal();

protected:
	

	//��Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	//��ť�����¼�
	virtual ERESULT OnCtlButtonClick(IEinkuiIterator* npSender);
	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize);
	

private:
	IEinkuiIterator* mpIteratorClose;
	IEinkuiIterator* mpIteratorPre;
	IEinkuiIterator* mpIteratorNext;
	IEinkuiIterator* mpIteratorPage;
	IEinkuiIterator* mpIteratorPath; //��ʾ��ǰ·��
	IEinkuiIterator* mpIteratorImBack; //�����ϼ���ͼ��
	IEinkuiIterator* mpIteratorBtBack; //������һ��

	cmmVector<CFileListItem*> mdList; //���list����
	ULONG mulCurrentPage; //��ǰ��ʾҳ��
	ULONG mulMaxPage;	//���ҳ��
	wchar_t mszCurrentPath[MAX_PATH]; //��ǰ·��
	wchar_t mszDisplayPath[MAX_PATH]; //�������Ϸ���ʾ·��

	cmmVector<wchar_t*> mdFolderPathList; //���ĳ���ļ��������������ļ���

	cmmVector<wchar_t*>* mpdHistroyPath; //��ʷ�ļ�
	cmmVector<ULONG> mdFolderLevel;//���ڴ���ϴν���Ŀ¼ʱ��ҳ��

	//��ʼ��list,Ĭ����ʾ���������ļ��м��̷�
	void InitList(void);
	//list����
	void ListClick(wchar_t* npszFilePath);
	//�����ļ���
	void EnterFolder(wchar_t* npszPath,bool nbIsBack = false);
	//��ȡĿ¼��ָ���ļ���Ŀ¼
	DWORD GetFolderPath(wchar_t* npszPath, wchar_t* npszName);

	//��ת��ָ��ҳ��
	void SetPage(ULONG nulPage);
	//��һҳ����һҳ
	void NextPage(bool nbIsNext);
	//������һ��Ŀ¼
	void BackFolder(void);
	//����ԭ����·������
	void ClearFilePath(void);
	//��ȡ����Ŀ¼�Ķ������ַ���
	bool GetDisplayName(GUID niCSIDL, OUT wchar_t* npszName, IN int niLen);
};

#define FP_LIST_MAX 4	//LISTһҳ��༸������

#define FP_ID_BT_OPEN 1
#define FP_ID_BT_CLOSE 1
#define FP_ID_BT_PRE 3
#define FP_ID_BT_NEXT 4
#define FP_ID_BT_Back 11

#define FP_ID_GROUP_FILE 1  //�ļ�
#define FP_ID_GROUP_HISTORY 2 //��ʷ��¼

#define FP_TIMER_ID_SHOW 1

