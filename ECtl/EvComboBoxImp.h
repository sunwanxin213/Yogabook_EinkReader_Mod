/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef		__EVCOMBOBOXIMP_H__
#define		__EVCOMBOBOXIMP_H__

#include "EvPictureFrameImp.h"
#include "EvEditImp.h"
#include "EvButtonImp.h"
#include "EvListImp.h"
#include "EvImageButtonImp.h"


//���峣��
#define BUF_SIZE	256


DECLARE_BUILTIN_NAME(ComboBox)
class CEvComboBox:
	public CXuiElement<CEvComboBox ,GET_BUILTIN_NAME(ComboBox)>
{
	friend CXuiElement<CEvComboBox ,GET_BUILTIN_NAME(ComboBox)>;

public:

	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		) ;

protected:
	// ֻ���ڱ������ó�ʼֵ����ָ����ΪNULL�����п���ʧ�ܵ������֮������㶼Ӧ����InitOnCreate�н���
	CEvComboBox();
	// �����ͷų�Ա����
	virtual ~CEvComboBox();
	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//����
	//virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);
	// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
	// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);

	virtual ERESULT OnCtlButtonClick(IEinkuiIterator* npSender);

	//���û�����
	virtual ERESULT OnElementEnable(bool nbIsEnable);

	//װ��������Դ
	//ERESULT LoadResource();

public:
	BOOL InsertItem(wchar_t* lswName);

	// ɾ��ָ���С��0��ʾɾ��ȫ���� ��������Add by colin
	bool DeleteItem(IN int niIndexToDel);

private:
	BOOL SetChildCtrlPara();

	BOOL SetDefaultValueList();

	//	��ȡUnificSetting���ݣ�������Ϊ
	bool SetValue();

	bool SetComboBoxEnable(bool lbEnalbe);

	bool SetItemSelected(int nID);

	bool SetItemEnable(int nID);
	bool SetItemDisable(int nID);

	

private:

	//IUnificSetting* mpUnificSetting;

public:

	struct ComboMenuItem 
	{
		int			mnID;
		wchar_t	mpText[BUF_SIZE];

		ComboMenuItem() : mnID(0) {}
	};

private:

	CEvPictureFrame*	mpUpperPicture;				//�༭����ı������ϱ�����
	CEvEditImp*			mpCurrentItemEdit;			//��Ͽ��е�ǰ��༭ģʽ��
	CEvButton*			mpCurrentItemButton;		//��Ͽ��е�ǰ��Ǳ༭ģʽ��
	CEvImageButton*		mpDropDownButton;			//��Ͽ��е�������ť
	
	//wchar_t*			mpCurItem;					//��ǰѡ������ı�

	//ULONG				mpNumItem;					//��ĸ���

	ICfKey*				mpComboBoxKey;				//Ĭ��ComboBox��ֵ

	int					mnStyle;					//�ؼ���� 0���ɱ༭ģʽ 1�����ɱ༭ģʽ

	IEinkuiIterator*		mpIterPopMenu;				//�����˵�
	IEinkuiIterator*		mpIterList;

	cmmVector<ComboMenuItem>	mpVecComboMenuItem;

	COMBOBOX_MSG			mMsgInfo;	

	//�ؼ���Ϣ
	TOOLBAR_MSG			mToolbarMsgInfo;

	bool				mbOnlyAcceptNum;			//	�༭״̬���ǹ�ֻ������������
	int					mnMaxInputNum;
	int					mnMinInputNum;

};


#endif		//__EVCOMBOBOXIMP_H__