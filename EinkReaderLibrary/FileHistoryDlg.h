/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once
#include <tuple>
#include "FileHistoryListItem.h"
#include "cmmstruct.h"
#include "FileOpenDlg.h"
#include "DeleteHistory.h"

//��ʷ�ļ��Ի���

DECLARE_BUILTIN_NAME(FileHistoryDlg)

class CFileHistoryDlg:
	public CXuiElement<CFileHistoryDlg,GET_BUILTIN_NAME(FileHistoryDlg)>
{
public:
	// ��������캯���趨Ϊprotected������Ҫ����仰; ���򣬲���Ҫ�������
	friend CXuiElement<CFileHistoryDlg,GET_BUILTIN_NAME(FileHistoryDlg)>;

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
	CFileHistoryDlg();
	~CFileHistoryDlg(void);

	// ģ̬��ʾ�öԻ���
	std::tuple<HISTORY_FILE_ATTRIB*, wstring> DoModal(bool nbIsEnableCancel = true);
	//������ʷ��¼
	void SetHistoryList(cmmVector<HISTORY_FILE_ATTRIB*>* npdHistroyPath);

	void ExitModal();

protected:
	

	//��Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	//��ť�����¼�
	virtual ERESULT OnCtlButtonClick(IEinkuiIterator* npSender);
	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize);
	//������Ϣ
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);

private:
	IEinkuiIterator* mpIteratorClose;
	IEinkuiIterator* mpIteratorOpen;
	IEinkuiIterator* mpIteratorMore;
	IEinkuiIterator* mpIteratorSelected;

	cmmVector<CFileHistoryListItem*> mdList; //���list����

	cmmVector<HISTORY_FILE_ATTRIB*>* mpdHistroyPath; //��ʷ�ļ�
	CFileOpenDlg* mpFileOpenDlg;
	CDeleteHistory* mpDeleteHistory;

	HISTORY_FILE_ATTRIB* m_resultHistoryFile = nullptr;
	wstring m_resultFileName;


	//��ʼ��list,Ĭ����ʾ���������ļ��м��̷�
	void InitList(void);
	//list����
	void ListClick(HISTORY_FILE_ATTRIB* npFileAttrib);
	//���ô��ļ����ڵ�λ��
	void SetOpenFilePos(void);
	//���¶�λԪ��
	void Relocation(void);
};

#define FHD_LIST_MAX 12	//�����ʾ������ʷ��¼

#define FHD_BT_CLOSE 3
#define FHD_BT_OPEN 4
#define FHD_BT_MORE 6


#define FHD_TIMER_ID_SHOW 1

