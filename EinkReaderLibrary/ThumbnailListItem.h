/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#pragma once

/*
	����ͼ�б���
*/


DECLARE_BUILTIN_NAME(ThumbnailListItem)

class CThumbnailListItem:
	public CXuiElement<CThumbnailListItem,GET_BUILTIN_NAME(ThumbnailListItem)>
{
	friend CXuiElement<CThumbnailListItem,GET_BUILTIN_NAME(ThumbnailListItem)>;

public:
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	//��������
	void SetData(wchar_t* npszFilePath, ULONG nulPageNumber,bool nbIsCurrent);

protected:
	CThumbnailListItem(void);
	~CThumbnailListItem(void);

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
	//������Ϣ
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);
	// ����Ԫ�أ�����֪ͨһ��Ԫ�����٣�ע�⣺��Ԫ�������յ�����Ϣ��Ӧ�ü�ʱ����Ԫ�ع�������UnregisterElement������
	// �Ӷ�����Ԫ�ع�������������һ��Ԫ�ط���������Ϣ�������ٽ��Լ���Ԫ�ع�����ע���������ͷ��������
	virtual ERESULT OnElementDestroy();
	//�ȱ����ţ�ndSizeDestԭʼͼƬ��С  ndSizeSrcҪ���Ƶ�����
	//���صȱ����ŵĴ�С
	D2D1_RECT_F GetProportionalScaling(D2D1_SIZE_F ndSizeSrc, D2D1_SIZE_F ndSizeDest);

	//����ͼƬ��С��λ��
	void RelocationItem(void);
private:

	IEinkuiIterator* mpIterPicture;	//���Թ�Ͷ�¼�
	IEinkuiIterator* mpIterPageNumber;		//�ļ�����
	IEinkuiIterator* mpIterBtClick;		

	DWORD mdwClickTicount;
	ULONG mulPageNumber;
	D2D1_SIZE_F mdPicSize;
	bool mbIsCurrent;

	IEinkuiBrush* mpLineBrush;	// �豸���
};

#define TL_BT_CLICK 10