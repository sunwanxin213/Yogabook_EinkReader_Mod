#pragma once


DECLARE_BUILTIN_NAME(PingpongButton)

class CEvPingpongButton : public CEvButton
{

public:
	CEvPingpongButton();
	~CEvPingpongButton();

	// ���س�ʼ������
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent,		// ������ָ��
		IN ICfKey* npTemplete,			// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32		// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	);

	// ����ʵ�����ӿ�
	DEFINE_CUMSTOMIZE_CREATE(CEvPingpongButton, (IEinkuiIterator* npParent, ICfKey* npTemplete, ULONG nuEID = MAXULONG32), (npParent, npTemplete, nuEID))

		//////////////////////////////////////////////////////////////////////////
		// һ��Ҫ������һ�У�������������ʶ��
		DEFINE_DERIVED_TYPECAST(PingpongButton, CEvButton)

protected:

	//Ԫ����ק
	virtual ERESULT OnDragging(const STMS_DRAGGING_ELE* npInfo);

	//��ק��ʼ
	virtual ERESULT OnDragBegin(const STMS_DRAGGING_ELE* npInfo);

	//��ק����
	virtual ERESULT OnDragEnd(const STMS_DRAGGING_ELE* npInfo);

private:

	int	 miDirection;	// ��ť�����ʶ��00����ʶ��Ч��1��ʶˮƽ��2��ʶ��ֱ

};


