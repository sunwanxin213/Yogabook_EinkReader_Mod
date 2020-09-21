#pragma once

#include "StdAfx.h"
#include <string>
#include "ElementImp.h"
#include "cmmstruct.h"

//���ļ�ʧ�ܵ���ʾ

DECLARE_BUILTIN_NAME(FileOpenFailDlg)

using std::wstring;

class CFileOpenFailDlg:
	public CXuiElement<CFileOpenFailDlg, GET_BUILTIN_NAME(FileOpenFailDlg)>
{
public:
	CFileOpenFailDlg() = default;
	~CFileOpenFailDlg() = default;

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator) override;
	// �������༰����������ʱ�����ر�ע��!!! һ��Ҫ���ȵ��û���ķ���
	// �����������ڽ�����Ԫ�ض��󡢳�ʼ���������ݣ�λ�á���С������ͼ�ȣ�
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent,	// ������ָ��
		IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32		// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��EUIϵͳ�Զ�����
		);

	// ģ̬��ʾ�öԻ���
	void DoModal(bool* npbIsSuccess);
	void ExitModal();
	void SetText(const wstring& text);

protected:
	//��Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg) override;
	//��ť�����¼�
	virtual ERESULT OnCtlButtonClick(IEinkuiIterator* npSender) override;
	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize) override;

private:
	IEinkuiIterator* mpIteratorOk = nullptr;
	IEinkuiIterator* mpIteratorErrorText = nullptr;
	bool* mpbIsSucess = false;
	wstring m_text;
};

