/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


#ifndef _EOB_OBJECT_MGR_
#define _EOB_OBJECT_MGR_

class CXelDataMgr;
class CXelFactoryMgr;

DECLARE_BUILTIN_NAME(CXelAllocator)
class CXelAllocator : public cmmBaseObject<CXelAllocator, IXelAllocator, GET_BUILTIN_NAME(CXelAllocator)>
{
public:
	CXelAllocator();
	virtual ~CXelAllocator();
	
	// ��ʼ��
	ULONG InitOnCreate(const wchar_t* nswRegPath);

	DEFINE_CUMSTOMIZE_CREATE(CXelAllocator,(const wchar_t* nswRegPath),(nswRegPath))

public:
	// �������ļ��д�������
	virtual IEinkuiIterator* __stdcall CreateElement(
		IN IEinkuiIterator* npParent,	// ������ָ��
		IN ICfKey* npTemplete,		// npTemplete��Key ID����EID��ֵ��������EType
		IN ULONG nuEID=MAXULONG32		// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);

	// ͨ����������������,�������Ķ�����Я������
	virtual IEinkuiIterator* __stdcall CreateElement(
		IN IEinkuiIterator* npParent,		// ������ָ��
		IN const wchar_t* nswClassName,		// ����
		IN ULONG nuEID=MAXULONG32	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
		);



	// ����һ��ͼƬ�ļ�
	virtual IEinkuiBitmap* __stdcall LoadImageFile(
		IN wchar_t* nswRelativePathName,		//��ֵΪ���·��ʱ�����·��Ϊ��ģ��Dll����Ŀ¼
		IN bool nbIsFullPath = false			//�õ����ʾnswRelativePathNameΪȫ·��������Ϊ���·��
		);

	// ��ָ����PE�ļ��У���ȡλͼ��Դ������һ��λͼ����
	virtual IEinkuiBitmap* __stdcall LoadImageFromPE(
		IN wchar_t *npPeFileName,				// ָ��PE�ļ�ȫ·��
		IN int dwDummy,						// ��Դ����
		IN int niXSize = 64,					// ָ��Ŀ��ͼ��Ŀ��
		IN int niYSize = 64						// ָ��Ŀ��ͼ��ĸ߶�
		);

	//����������ͼƬ
	virtual IEinkuiBitmap* __stdcall CreateImageByText(STETXT_BMP_INIT& rdBmpInit);

	// ָ����С���Լ�λͼ���ݣ�����һ��λͼ���󣬳��򷵻غ�npRawData�����е������ͷ�
	virtual IEinkuiBitmap* __stdcall CreateBitmapFromMemory(
		LONG nuWidth,					// λͼ���
		LONG nuHeight,					// λͼ�߶�
		LONG PixelSize,					// ���ص�λ��3 or 4
		LONG Stride,
		void* npRawData				// λͼԭʼ����
		);

public:
	// �ṩ��XUI��ģ��ķ��������ڻ�ȡָ��DLL����Ĺ�����ӿ�
	IElementFactory* LoadFactory(/*const wchar_t* nswRelativePathName*/);

protected:
	IElementFactory* PrepareAllocator(const wchar_t* nswClsName);

	CXelDataMgr*	mopDataMgr;
	CXelFactoryMgr*	mopFactoryMgr;
	
	LONG mlTotalBmp;

};


#endif