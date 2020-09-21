#ifndef		__EVTOOLPANEIMP_H__
#define		__EVTOOLPANEIMP_H__


DECLARE_BUILTIN_NAME(ToolPane)
class CEvToolPane:
	public CXuiElement<CEvToolPane ,GET_BUILTIN_NAME(ToolPane)>
{
	friend CXuiElement<CEvToolPane ,GET_BUILTIN_NAME(ToolPane)>;

public:

	ULONG InitOnCreate(
		IN IXuiIterator* npParent = NULL,	// ������ָ��
		IN ICfKey* npTemplete = NULL,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		) ;

protected:
	// ֻ���ڱ������ó�ʼֵ����ָ����ΪNULL�����п���ʧ�ܵ������֮������㶼Ӧ����InitOnCreate�н���
	CEvToolPane();
	// �����ͷų�Ա����
	virtual ~CEvToolPane();
	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IXuiIterator* npIterator);
	//����
	//virtual ERESULT OnPaint(IXuiPaintBoard* npPaintBoard);
	// �ֽ���Ϣ���ṩ��Ϣ�ֽ����Ϣ��Ӧ�Ĺ��ܣ������ʵ���ǽ���Ϣ�ֽ�Ϊ��ͬ������󣬵�����Ӧ�Ĵ����麯�������ڲ���ʶ����Ϣ��һ�ɷ���ERESULT_UNEXPECTED_MESSAGE
	// �������ķ���ֵ���Զ�ͬ�����õ�npMsgָ�����Ϣ������
	virtual ERESULT ParseMessage(IXuiMessage* npMsg);

	virtual ERESULT OnCtlButtonClick(IXuiIterator* npSender);

private:

	bool SetPaneItemValue(int lID);

	//ERESULT LoadResource();			//װ��������Դ

public:

	IUnificSetting* mpUnificSetting;

private:
	

private:


};

#endif		// __EVTOOLPANEIMP_H__