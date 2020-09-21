#pragma once

#include "StdAfx.h"
#include <tuple>
#include <string>
#include <chrono>
#include "ElementImp.h"
#include "cmmstruct.h"
#include "PdfConvert.h"

using std::wstring;
using std::tuple;

DECLARE_BUILTIN_NAME(ConvertProgressDlg)


class CConvertProgressDlg :
	public CXuiElement<CConvertProgressDlg, GET_BUILTIN_NAME(ConvertProgressDlg)>
{
public:
	CConvertProgressDlg() = default;
	~CConvertProgressDlg() = default;

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator) override;
	// �������༰����������ʱ�����ر�ע��!!! һ��Ҫ���ȵ��û���ķ���
	// �����������ڽ�����Ԫ�ض��󡢳�ʼ���������ݣ�λ�á���С������ͼ�ȣ�
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent,	// ������ָ��
		IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32		// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��EUIϵͳ�Զ�����
		);

	tuple<PDFConvert::ConvertResult, wstring> DoModal(const wstring& sourceFileName);
	void ExitModal(PDFConvert::ConvertResult result);
	void OnTimer(PSTEMS_TIMER npStatus) override;

protected:
	//��Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg) override;
	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize) override;

private:
	IEinkuiIterator* mpIteratorProgressIcon = nullptr;
	PDFConvert::ConvertResult m_result;
	wstring m_sourceFileName;
	wstring m_targetFileName;
	int m_currentIconPosition = 50;
	std::chrono::system_clock::time_point m_startTime;
};

