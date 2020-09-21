/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	����ʱ�Ĺ�����
*/

#include "MsgDefine.h"
#include "MenuTxt.h"
#include "EditToolbar.h"
#include "MenuPdf.h"
#include "MenuEpubMobi.h"

DECLARE_BUILTIN_NAME(ToolbarH)

class CToolbarH:
	public CXuiElement<CToolbarH,GET_BUILTIN_NAME(ToolbarH)>
{
	friend CXuiElement<CToolbarH,GET_BUILTIN_NAME(ToolbarH)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	//���ñ�״̬��ťѡ��
	void SetPenStatusSelect(ULONG nulID);
	//����˫ҳ��ʾ��ť״̬
	void SetDuopageButton(bool nbSingle);
	//������Ļ����ť״̬
	void SetScreenOriButton(ULONG nulScreenOri);
	//��ȡ��ǰ˫ҳ��ʾ״̬
	bool GetDuopageStatus(void);
	//���õ�ǰ�򿪵��ļ�����
	void SetDoctype(int niType);
	//����txt�ֺ�
	void SetTxtFontSizeIndex(DWORD ndwIndex);
	// ���ñʼ���ʼ������
	void SetPenData(DWORD ndwPenWidthIndex, DWORD ndwPenColorIndex);
	//����redo,undo��ť״̬
	void SetUndoRedoStatus(bool nbUndoEnable, bool nbRedoEnable);
	//����ҳ��״̬
	void UpdatePageStatus(PAGE_STATUS ndStatus);
	//���õ�ǰҳ���ע����
	void SetCurrentPageInkCount(int niCount);
	//�����ļ�����
	void SetFileAttrib(DWORD ndwAttrib);
	//��ȡ�ļ�����
	DWORD GetFileAttrib(void);
	//�ĵ��������
	void DocmentLoadComplete(void);
	//����ͼ�������
	void ThumbnailsLoadComplete(void);

	//bool GetBCoverState();//��ȡB��״̬�Ƿ�رգ� true ������false �ر�
	//void SetBCoverState();//����ע����ֵ����B��״̬�Ƿ�رգ� true ������false �ر�
	//void UpdateBCoverState();//ͬ��ע����е�ֵ�����ֵһ�£� true ������false �ر�
	//���ظ���˵�
	void HideMoreMenu();

protected:
	CToolbarH(void);
	~CToolbarH(void);

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
	// ��������
	virtual ERESULT OnMouseOwnerTest(const D2D1_POINT_2F& rPoint);
private:
	IEinkuiIterator* mpIterBtFileOpen;	//���ļ��Ի���
	IEinkuiIterator* mpIterBtMore; //���ఴť
	IEinkuiIterator* mpIterBackground;	//����ͼ
	IEinkuiIterator* mpIterBtThumbnail; //����ͼ

	CMenuTxt* mpMenuTxt; //txt�ļ�ʱ�ĸ���˵�
	CMenuPdf* mpMenuPdf; //PDF�ļ�ʱ�ĸ���˵�
	CEditToolbar* mpEditToolbar; //�༭������
	CMenuEpubMobi* mpMenuEpubMobi; //epub/mobi����˵�

	int miCurrentPageInkCount; //��ǰҳ���ע����
	bool mbIsTwoScreen;//true��ʾ˫��
	//bool mbIsLCDClose;//true��ʾ������false��ʾ����
	int miDocType;
	DWORD mdwTxtFontSizeIndex;
	bool mbIsHScreen; //true��ʾ�Ǻ���
	DWORD mdwAttrib;
	ULONG mulScreenOritent; //��Ļ����

	//txt�ĸ���˵�
	void ShowTxtMoreMenu();
	//pdf�ĸ���˵�
	void ShowPDFMoreMenu();
	//epub/mobi�ĸ���˵�
	void ShowEpubMobiMoreMenu();
	//���¶�λԪ��λ��
	void Relocation(void);
};

#define TBH_BT_OPEN_FILE 8
#define TBH_BT_THUMBNAIL 9
#define TBH_BT_MORE 2
