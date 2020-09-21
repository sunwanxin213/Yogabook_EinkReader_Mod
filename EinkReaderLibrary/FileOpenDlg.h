/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once
#include <vector>
#include <string>
#include "FileListItem.h"
#include "cmmstruct.h"

//���ļ��Ի���

DECLARE_BUILTIN_NAME(FileOpenDlg)

using std::vector;
using std::wstring;


class COfficeConvertDlg;
class CFileOpenDlg:
	public CXuiElement<CFileOpenDlg,GET_BUILTIN_NAME(FileOpenDlg)>
{
public:
	enum eFileType
	{
		e_File_UnKnown = 0,
		e_File_Pdf,
		e_File_Epub,
		e_File_Mobi,
		e_File_Txt,
		e_File_Word,
		e_File_Excel,
		e_File_PowerPoint,
		e_File_Visio
	};
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
	wstring DoModal(bool* npbIsSuccess);
	//������ʷ��¼
	void SetHistoryList(wchar_t* npHistroyPath);

	void ExitModal();

	const wstring& SelectedFile();

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

	vector<wstring> m_FolderPathList; //���ĳ���ļ��������������ļ���

	//cmmVector<wchar_t*>* mpdHistroyPath; //��ʷ�ļ�
	cmmVector<ULONG> mdFolderLevel;//���ڴ���ϴν���Ŀ¼ʱ��ҳ��

	bool* mpbIsSucess;
	COfficeConvertDlg *m_pDemoDlg = nullptr;

private:
	//��ʼ��list,Ĭ����ʾ���������ļ��м��̷�
	void InitList(void);
	//list����
	void ListClick(wchar_t* npszFilePath);
	//�����ļ���
	void EnterFolder(wchar_t* npszPath,bool nbIsBack = false);
	//��ȡĿ¼��ָ���ļ���Ŀ¼
	DWORD FillSubDirAndRelatedFiles(wchar_t* npszPath, const vector<const wchar_t*>& relatedExtList);

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

	eFileType GetFileType(wstring strFilePath);
	bool m_bCanConvertToPDF = true; // true��pdfת���߼���false�ر�pdfת���߼�

	wstring m_selectedFile;
};

#define FP_LIST_MAX 8	//LISTһҳ��༸������

#define FP_ID_BT_OPEN 1
#define FP_ID_BT_CLOSE 7
#define FP_ID_BT_PRE 3
#define FP_ID_BT_NEXT 4
#define FP_ID_BT_Back 11


#define FP_TIMER_ID_SHOW 1

