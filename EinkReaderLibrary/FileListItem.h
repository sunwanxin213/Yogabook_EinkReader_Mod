/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	�ļ��б���
*/

DECLARE_BUILTIN_NAME(FileListItem)

class CFileListItem:
	public CXuiElement<CFileListItem,GET_BUILTIN_NAME(FileListItem)>
{
	friend CXuiElement<CFileListItem,GET_BUILTIN_NAME(FileListItem)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	//����·��
	void SetPath(wchar_t* npszPath, wchar_t* npszDisplayName);
protected:
	CFileListItem(void);
	~CFileListItem(void);

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//��ʱ��
	virtual void OnTimer(
		PSTEMS_TIMER npStatus
		);
	//��Ϣ������
	//virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	//��ť�����¼�
	virtual ERESULT OnCtlButtonClick(IEinkuiIterator* npSender);
	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize);
	//֪ͨԪ�ء���ʾ/���ء������ı�
	virtual ERESULT OnElementShow(bool nbIsShow);


	//���ļ��Ĵ���
	void ProcFile(wchar_t* npszPath);
	//���ļ��еĴ���
	void ProcFolder(wchar_t* npszPath, wchar_t* npszDisplayName);
	//��ȡĿ¼��ָ���ļ���Ŀ¼����
	DWORD GetFolderCount(wchar_t* npszPath, wchar_t* npszName = NULL);

private:
	wchar_t* mpszFilePath;

	IEinkuiIterator* mpIterFolderIcon;
	IEinkuiIterator* mpIterPdfIcon;
	IEinkuiIterator* mpIterName;
	IEinkuiIterator* mpIterAttrib;
	IEinkuiIterator* mpIterBt;

	DWORD mdwClickTicount;

	wchar_t mpszItem[100];
};

#define FLI_BT 10 //���