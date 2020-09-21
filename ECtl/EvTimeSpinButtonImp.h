#ifndef		__EV_TIME_SPIN_BUTTON_IMP_H__
#define		__EV_TIME_SPIN_BUTTON_IMP_H__

#include "EvEditImp.h"
#include "EvImageButtonImp.h"
#include "EvLabelImp.h"
#include <string>

DECLARE_BUILTIN_NAME(TimeSpinButton)
class CEvTimeSpinButton:
	public CXuiElement<CEvTimeSpinButton ,GET_BUILTIN_NAME(TimeSpinButton)>
{
	friend CXuiElement<CEvTimeSpinButton ,GET_BUILTIN_NAME(TimeSpinButton)>;

public:

	ULONG InitOnCreate(
		IN IXuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		) ;

protected:
	// ֻ���ڱ������ó�ʼֵ����ָ����ΪNULL�����п���ʧ�ܵ������֮������㶼Ӧ����InitOnCreate�н���
	CEvTimeSpinButton();
	// �����ͷų�Ա����
	virtual ~CEvTimeSpinButton();
	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IXuiIterator* npIterator);
	//����
	virtual ERESULT OnPaint(IXuiPaintBoard* npPaintBoard);
	// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
	// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
	virtual ERESULT ParseMessage(IXuiMessage* npMsg);

	//���û�����
	virtual ERESULT OnElementEnable(bool nbIsEnable);

private:

	BOOL SetChildCtrlPara();
	void UpdateView();
	bool SetTimeSpinButtonEnable(bool nbIsEnable);

public:

	bool SetCurrentTime(XuiTimeFormat time);
	XuiTimeFormat GetCurrentTime() const;

private:

	CEvEditImp*			mpEditMinutes;			//	�༭��
	CEvEditImp*			mpEditSeconds;			//	�༭��
	CEvEditImp*			mpEditMilliseconds;		//	�༭����

	CEvImageButton*		mpBtnUpArrow;			//	���ϼ�ͷ
	CEvImageButton*		mpBtnDownArrow;			//	���¼�ͷ

	CEvLabelImp*		mpLabelColonOne;		//	ð��1
	CEvLabelImp*		mpLabelColonTwo;		//	ð��2

	XuiTimeFormat		mCurrentTime;			//	��ǰʱ��

	int					mnCurrentEditID;		//	��¼��ǰ��ȡ����ı༭��

	bool				mbSendModifyMsgWhenEditModify;		//���õ��༭���޸�ʱ�Ƿ��� EEVT_TIMESPINBUTTON_TIME_MODIFIED
};


#endif		//__EV_TIME_SPIN_BUTTON_IMP_H__