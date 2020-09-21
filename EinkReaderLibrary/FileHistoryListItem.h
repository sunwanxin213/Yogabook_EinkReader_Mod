/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	�ļ�����ʷ�б���
*/


// �ļ����Խṹ��
typedef struct _HISTORY_FILE_ATTRIB {
	wchar_t FilePath[MAX_PATH]; //�ļ�·��
	wchar_t ThumbanilFilePath[MAX_PATH]; //����ͼ�ļ�·��
	DWORD ReadProgress;			//�Ķ�����
	DWORD IsModify;				//�Ƿ��޸Ĺ�
	ULONG TimeStamp;			//����ʱ��
	ULONG PageContext;			//ҳ��򿪱��
	ULONG PageContext2;			//ҳ��򿪱��
	ULONG PageNumber;			//ҳ��

// [zhuhl5@20200116:ZoomRecovery]
	int	autoZoom{ 0 };
	float	scaling{ 1.0f };
	int		scalingLevel{ 0 };
	float	scalingPosX{ 0.0f };
	float	scalingPosY{ 0.0f };
}HISTORY_FILE_ATTRIB, *PHISTORY_FILE_ATTRIB;

DECLARE_BUILTIN_NAME(FileHistoryListItem)

class CFileHistoryListItem:
	public CXuiElement<CFileHistoryListItem,GET_BUILTIN_NAME(FileHistoryListItem)>
{
	friend CXuiElement<CFileHistoryListItem,GET_BUILTIN_NAME(FileHistoryListItem)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	//�����ļ�����
	void SetData(HISTORY_FILE_ATTRIB* npFileAttrib);
	//�Ƿ����õ��
	void SetEnable(bool nbIsEnable);
protected:
	CFileHistoryListItem(void);
	~CFileHistoryListItem(void);

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
	void ProcFile(HISTORY_FILE_ATTRIB* npFileAttrib);


private:
	HISTORY_FILE_ATTRIB* mpFileAttrib;

	IEinkuiIterator* mpIterFolderIcon;	//�ļ�����ͼ��
	IEinkuiIterator* mpIterModifyIcon;	//�޸ı�־
	IEinkuiIterator* mpIterName;		//�ļ�����
	IEinkuiIterator* mpIterAttrib;		//��������
	IEinkuiIterator* mpIterBt;			//�����ť

	DWORD mdwClickTicount;

	wchar_t mpszItem[100];
};

#define FLI_BT 10 //���