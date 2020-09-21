#pragma once

#include "StdAfx.h"
#include <string>
#include "ElementImp.h"
#include "cmmstruct.h"

using std::wstring;

DECLARE_BUILTIN_NAME(AskConvertDlg)

enum class PromptDialogAskResult : int
{
	Yes = 0,
	No = 1,
};

class CAskConvertDlg: 
	public CXuiElement<CAskConvertDlg, GET_BUILTIN_NAME(AskConvertDlg)>
{
public:
	CAskConvertDlg() = default;
	~CAskConvertDlg() = default;

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
	PromptDialogAskResult DoModal();
	void ExitModal(PromptDialogAskResult result);

protected:
	//��Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg) override;
	//��ť�����¼�
	virtual ERESULT OnCtlButtonClick(IEinkuiIterator* npSender) override;
	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize) override;

private:

	void NotAskConvert(bool nbIsShow);
	IEinkuiIterator* mpIteratoraskswitch = nullptr;	
	PromptDialogAskResult m_result;
	bool mbIsAsk = false;
};

#define PDF_COV_CONTINUE  1
#define PDF_COV_CANCEL    2
#define PDF_COV_ASKSWITCH 6

