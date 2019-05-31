/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef		__EVSPINBUTTONIMP_H__
#define		__EVSPINBUTTONIMP_H__

#include "EvEditImp.h"
#include "EvImageButtonImp.h"
#include "EvPictureFrameImp.h"

#define SPINBUTTON_BUF_SIZE		256
#define SPINBUTTON_MAX_VALUE	10000
#define SPINBUTTON_MIN_VALUE	-10000


DECLARE_BUILTIN_NAME(SpinButton)
class CEvSpinButton:
	public CXuiElement<CEvSpinButton ,GET_BUILTIN_NAME(SpinButton)>
{
	friend CXuiElement<CEvSpinButton ,GET_BUILTIN_NAME(SpinButton)>;

public:

	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		) ;

protected:
	// ֻ���ڱ������ó�ʼֵ����ָ����ΪNULL�����п���ʧ�ܵ������֮������㶼Ӧ����InitOnCreate�н���
	CEvSpinButton();
	// �����ͷų�Ա����
	virtual ~CEvSpinButton();
	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//����
	//virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);
	// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
	// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
	virtual ERESULT ParseMessage(IEinkuiMessage* npMsg);

	//���û�����
	virtual ERESULT OnElementEnable(bool nbIsEnable);

	//virtual ERESULT OnCtlButtonClick(IEinkuiIterator* npSender);

public:
	BOOL	SetCurrentValue(const int nValue);
	int		GetCurrentValue();
	
private:
	BOOL UpdateEditView();
	BOOL SetCurrentValueByDisplay();

private:
	BOOL SetChildCtrlPara();

public:


private:

	CEvEditImp*			mpEdit;				//�༭��
	CEvImageButton*		mpBtnUpArrow;		//���ϼ�ͷ
	CEvImageButton*		mpBtnDownArrow;		//���¼�ͷ
	CEvPictureFrame*	mpBkg;				//����ͼ

	int					mnCurrentValue;		//

	int					miMinValue;			//  ��Сֵ
	int					miMaxValue;			// ���ֵ

};


#endif		//__EVSPINBUTTONIMP_H__