#ifndef _FACTORYIMP_H_
#define _FACTORYIMP_H_
__interface IEinkuiSystem;



#define CONFIG_FILE_NAME_MAX_LEN 20 //�����ļ����ֳ���

DECLARE_BUILTIN_NAME(CFactoryImp)
class CFactoryImp : public cmmBaseObject<CFactoryImp, IElementFactory, GET_BUILTIN_NAME(CFactoryImp)>
{
protected:
	CFactoryImp();
	virtual ~CFactoryImp();


	// ��ʼ������������ʵ�ָ��ֲ�ͬ�����ĳ�ʼ��������ע�⣬����������InitOnCreate������һ��Ҫ���û����InitOnCreate����
	// ����0��ʾ�ɹ�������ֵ���λΪ1��ʾ�������ش���Ӧ����ֹ��ʼ�����̣����صľ��Ǵ����룻��������ֵ��ʾ�����Ǵ��󷵻���
	ULONG InitOnCreate(void);

	DEFINE_CUMSTOMIZE_CREATE(CFactoryImp, (), ())
public:

	// �������ļ��д�������
	virtual IEinkuiIterator* __stdcall CreateElement(
		IN IEinkuiIterator* npParent,		// ������ָ��
		IN ICfKey* npTemplete,			// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	);

	// ͨ����������������
	virtual IEinkuiIterator* __stdcall CreateElement(
		IN IEinkuiIterator* npParent,		// ������ָ��
		IN const wchar_t*		nswClassName,	// ����
		IN ULONG nuEID = MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	);

	// ������Module���׵�Profile�ļ��ӿڣ����صĽӿڵ�����ʹ��ʱ����ҪRelease
	virtual IConfigFile* __stdcall GetTempleteFile(void);

	// ��ñ�DLLΨһ�Ĺ�������
	static CFactoryImp* GetUniqueObject(void);

protected:
	// Ψһʵ��
	static CFactoryImp* gpFactoryInstance;
	IConfigFile* mpConfig;

	CMapList<AFX_MAPCALL, CHashFront>	moMapList;

};




























#endif//_FACTORYIMP_H_