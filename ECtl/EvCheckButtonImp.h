#ifndef _EVCHECKBUTTONIMP_H_
#define _EVCHECKBUTTONIMP_H_



// Element�Ļ���ʵ�֣���������Elementʱ�����Դ�Ϊ������Ҫ���ԴӴ����ֽ��������࣬��Ϊ��������������ʵ�������Ľӿڣ�ֱ�����������CEvButton���������ṩIXsElement�ӿ�
// ���ʵ�ֵ�����ͬ�ӿڵ���𣬾Ϳ���ֱ�Ӵ�ĳ��ʵ�������������ࡣ


DECLARE_BUILTIN_NAME(CheckButton)
class CEvCheckButton : public CEvButton
{
public:

	// �������༰����������ʱ�����ر�ע��!!! һ��Ҫ���ȵ��û���ķ���
	// �����������ڽ�����Ԫ�ض��󡢳�ʼ���������ݣ�λ�á���С������ͼ�ȣ�
	ULONG InitOnCreate(
		IN IEinkuiIterator* npParent,	// ������ָ��
		IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32		// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	);

	// ����ʵ�����ӿ�
	DEFINE_CUMSTOMIZE_CREATE(CEvCheckButton, (IEinkuiIterator* npParent, ICfKey* npTemplete, ULONG nuEID = MAXULONG32), (npParent, npTemplete, nuEID))

		//////////////////////////////////////////////////////////////////////////
		// һ��Ҫ������һ�У�������������ʶ��
		DEFINE_DERIVED_TYPECAST(CheckButton, CEvButton)

protected:
	// ֻ���ڱ������ó�ʼֵ����ָ����ΪNULL�����п���ʧ�ܵ������֮������㶼Ӧ����InitOnCreate�н���
	CEvCheckButton();
	// �����ͷų�Ա����
	virtual ~CEvCheckButton();
	//��ʼ��������һ��Ԫ�ر�����ʱ���ã�ע�⣺��Ԫ�ػ����ڸ�Ԫ���յ�������Ϣ���Ӷ�ȷ����Ԫ����һ������Ԫ�س�ʼ��֮�����ȫ����ʼ���Ļ���
	virtual ERESULT OnElementCreate(IEinkuiIterator* npIterator);
	//����
	virtual ERESULT OnPaint(IEinkuiPaintBoard* npPaintBoard);
	// ��������
	virtual ERESULT OnMouseOwnerTest(const D2D1_POINT_2F& rPoint);
	//��λ����ͼƬ��ʾλ��
	virtual void RelocateText(void);
private:

};

#endif//_EVCHECKBUTTONIMP_H_

#define TF_ID_CHECK_BT_CHECKED L"Checked"			//����������ֵ��˵������ʱ��ѡ��