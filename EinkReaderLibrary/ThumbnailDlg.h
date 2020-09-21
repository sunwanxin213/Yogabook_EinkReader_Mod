/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	����ͼ����
*/

#include "PageProgress.h"
#include "ThumbnailListItem.h"
#include "cmmstruct.h"
#include "PdfPicture.h"
#include "MenuThumbnail.h"

DECLARE_BUILTIN_NAME(ThumbnailDlg)

#define TD_ITEM_MAX 16 //�������

class CThumbnailDlg:
	public CXuiElement<CThumbnailDlg,GET_BUILTIN_NAME(ThumbnailDlg)>
{
	friend CXuiElement<CThumbnailDlg,GET_BUILTIN_NAME(ThumbnailDlg)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	//��ʾ������Ԫ��
	void ShowItem(bool nbIsShow);
	//���õ�ǰ�򿪵��ļ�����
	void SetDoctype(int niType);
	//�����ĵ�����ָ��
	void SetPdfPicture(CPdfPicture* npPdfPicture);
	// ģ̬��ʾ�öԻ���
	void DoModal();
	void ExitModal();


protected:
	CThumbnailDlg(void);
	~CThumbnailDlg(void);

	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//��ʱ��
	virtual void OnTimer(
		PSTEMS_TIMER npStatus
		);
	//��Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);
	//��ť�����¼�
	virtual ERESULT OnCtlButtonClick(IEinkuiIterator* npSender);
	//Ԫ�زο��ߴ緢���仯
	virtual ERESULT OnElementResized(D2D1_SIZE_F nNewSize);
	//֪ͨԪ�ء���ʾ/���ء������ı�
	virtual ERESULT OnElementShow(bool nbIsShow);
	//������Ϣ
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);

	//���¶�λԪ��
	void RelocationItem(void);

	//��ת��ָ��ҳ��
	void EnterPage(ULONG nulPage);
	//��һҳ����һҳ
	void NextPage(bool nbIsNext);
	//��ȡ�����ַ���
	void GetTranslateString(ULONG nulPageCount,ULONG nulPageAnnot);
	//����ȫ����ɸѡ
	void SelectAll(bool nbIsAll);
private:

	IEinkuiIterator* mpIterBtPageNumber;	//ҳ��
	IEinkuiIterator* mpIteratorPre;	//��һҳ
	IEinkuiIterator* mpIteratorNext;	//��һҳ
	IEinkuiIterator* mpIteratorSelect;	//ɸѡ��
	CPageProgress* mpPageProgress; //ҳ����ת

	int miDocType;
	IEdDocument_ptr mpDocument;
	CMenuThumbnail* mpMenuThumbnail;
	wchar_t mszPageAll[MAX_PATH];
	wchar_t mszPageAnnot[MAX_PATH];
	
	ULONG mulCurrentPage; //��ǰ��ʾҳ��
	ULONG mulMaxPage;	//����ͼ���ҳ��
	ULONG mulDocMaxPage; //�ĵ����ҳ��
	ULONG mulAnnotPage; //�б�ע��ҳ������
	ULONG mulSelectType; 
	ULONG mulCurrentDoc; //��ǰ�û����ڿ���������ҳ
	cmmVector<ULONG> mdAnnotPageNumber; //�ѱ�עҳ��ҳ��

	cmmVector<CThumbnailListItem*> mdList; //���list����
	
};

#define TD_BT_CLOSE 1
#define TD_BT_PRE 2
#define TD_BT_NEXT 3
#define TD_BT_NUMBER 5
#define TD_BT_SELECT 6

