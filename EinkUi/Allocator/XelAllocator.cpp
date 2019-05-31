/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */


// ObjectMgr.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CommonHeader.h"

#include "XelAllocator.h"


DEFINE_BUILTIN_NAME(CXelAllocator)
CXelAllocator::CXelAllocator()
{
	mlTotalBmp = 0;

}


CXelAllocator::~CXelAllocator()
{
	if (mopDataMgr != NULL)
		mopDataMgr->Release();
	
	if (mopFactoryMgr != NULL)
		mopFactoryMgr->Release();

}


ULONG CXelAllocator::InitOnCreate(const wchar_t* nswRegPath)
{
	mopDataMgr = NULL;
	mopFactoryMgr = NULL;

	// ��ȡ���ù���������	
	mopDataMgr = CXelDataMgr::SingleInstance(nswRegPath);
	if (mopDataMgr == NULL)
		return 0x80000000;
	
	mopFactoryMgr = CXelFactoryMgr::CreateInstance();
	if (mopFactoryMgr == NULL)
		return 0x80000000;	

	return 0;

}

// �ṩ��XUI��ģ��ķ��������ڻ�ȡָ��DLL����Ĺ�����ӿ�
IElementFactory* CXelAllocator::LoadFactory(
	/*const wchar_t* nswRelativePathName*/
	)
{
	IElementFactory* lpElementFact = NULL;
	CFilePathName loPath;
	const wchar_t*	lswWidgetPath = NULL;

	lswWidgetPath = EinkuiGetSystem()->GetCurrentWidget()->GetWidgetDefaultPath();
	if (lswWidgetPath == NULL)
		return NULL;

	loPath = lswWidgetPath;
	loPath.AssurePath();
	loPath.Transform(EinkuiGetSystem()->GetCurrentWidget()->GetModuleName());


	lpElementFact = mopDataMgr->HasFactoryBeenLoaded(loPath.GetPathName());
	if (lpElementFact == NULL)
	{
		SetDllDirectory(lswWidgetPath);

		lpElementFact = mopFactoryMgr->GetFactInstance(loPath.GetPathName());

		SetDllDirectory(NULL);
	}

	return lpElementFact;
}


IElementFactory* CXelAllocator::PrepareAllocator(
	const wchar_t* nswClsName
	)
{

	IElementFactory* lpELementFact = NULL;
	wchar_t lwszDllPath[MAX_PATH] = {0};
	REGSTATUS lStateEnum;

	// �鿴�Ƿ�ע�� 
	lStateEnum = mopDataMgr->IsRegisted(nswClsName, lwszDllPath, &lpELementFact);

	// û��ע�ᣬ����NULL
	if (lStateEnum == Invalid)
		return NULL;
	
	// LOAD����£��᷵��һ����Ч��IElementFactory�ӿ�ָ��
	if (lStateEnum == LOAD)
		return lpELementFact;

	// UNLOAD,û�м��أ�����ù��������෽�����ȡ������ӿ�
	return mopFactoryMgr->GetFactInstance(lwszDllPath);


}


// ����һ��ͼƬ�ļ�
IEinkuiBitmap* __stdcall CXelAllocator::LoadImageFile( 
	IN wchar_t* nswRelativePathName,		//��ֵΪ���·��ʱ�����·��Ϊ��ģ��Dll����Ŀ¼
	IN bool nbIsFullPath					//�õ����ʾnswRelativePathNameΪȫ·��������Ϊ���·��
	)
{
	// ��ȡ��ǰwidget���ڵ�·������ӻ�ȡImageFile��FullPath
	const wchar_t*	lswWidgetPath = NULL;
	lswWidgetPath = EinkuiGetSystem()->GetCurrentWidget()->GetWidgetDefaultPath();
	if (lswWidgetPath == NULL)
		return NULL;

	CFilePathName	lopFilePath(lswWidgetPath);
	lopFilePath.AssurePath();
	lopFilePath.Transform(nswRelativePathName, false);

	// ����ImageFile,���ؽӿ�ָ��
	IEinkuiBitmap* lpElBitmap = CXD2dBitmap::CreateInstance(nbIsFullPath==false?lopFilePath.GetPathName():nswRelativePathName);
	if (lpElBitmap == NULL)
		return NULL;
	
	InterlockedIncrement(&mlTotalBmp);
	//CEinkuiSystem::gpXuiSystem->GetBitmapList().RegisteBitmap(lpElBitmap); //��λͼ������ע�� ax nov.28,17
	
	return lpElBitmap;

}


// ��ָ����PE�ļ��У���ȡλͼ��Դ������һ��λͼ����
IEinkuiBitmap* __stdcall CXelAllocator::LoadImageFromPE( 
	IN wchar_t *npPeFileName,
	IN int niDummy,
	IN int niXSize,
	IN int niYSize
	)
{
	if (NULL == npPeFileName)
		return NULL;

	IEinkuiBitmap* lpElBitmap = CXD2dBitmap::CreateInstance(npPeFileName,niDummy, niXSize, niYSize);
	if (lpElBitmap == NULL)
		return NULL;

	InterlockedIncrement(&mlTotalBmp);
	//CEinkuiSystem::gpXuiSystem->GetBitmapList().RegisteBitmap(lpElBitmap); //��λͼ������ע�� ax nov.28,17

	return lpElBitmap;

}


//����������ͼƬ
IEinkuiBitmap* __stdcall CXelAllocator::CreateImageByText(STETXT_BMP_INIT& rdBmpInit)
{
	IEinkuiBitmap* lpElBitmap = CXuiTextBitmap::CreateInstance(rdBmpInit);
	if (lpElBitmap == NULL)
		return NULL;

	InterlockedIncrement(&mlTotalBmp);
	//CEinkuiSystem::gpXuiSystem->GetBitmapList().RegisteBitmap(lpElBitmap); //��λͼ������ע�� ax nov.28,17

	return lpElBitmap;
}


// ָ����С���Լ�λͼ���ݣ�����һ��λͼ���󣬳��򷵻غ�npRawData�����е������ͷ�
IEinkuiBitmap* __stdcall CXelAllocator::CreateBitmapFromMemory(
	LONG nuWidth,					// λͼ���
	LONG nuHeight,					// λͼ�߶�
	LONG PixelSize,					// ���ص�λ��3 or 4
	LONG Stride,
	void* npRawData				// λͼԭʼ����
	)
{
	IEinkuiBitmap* lpBitmap = NULL;
	do 
	{
		BREAK_ON_NULL(npRawData);

		lpBitmap = CXD2dBitmap::CreateInstance(nuWidth, nuHeight,PixelSize,Stride,npRawData);
		BREAK_ON_NULL(lpBitmap);

		InterlockedIncrement(&mlTotalBmp);
		//CEinkuiSystem::gpXuiSystem->GetBitmapList().RegisteBitmap(lpBitmap); //��λͼ������ע�� ax nov.28,17

	} while (false);

	return lpBitmap;

}

// �������ļ��д�������
IEinkuiIterator* __stdcall CXelAllocator::CreateElement( 
	IN IEinkuiIterator* npParent, 
	IN ICfKey* npTemplete,
	IN ULONG nuEID	// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	)
{
	wchar_t lswClsName[MAX_PATH] ={0};
	IElementFactory*	lpClsFactory = NULL;
	IEinkuiIterator*			lpIterator = NULL;
	int liStatus;

	// ��ȡҪ�����Ķ�������
	liStatus = npTemplete->GetValue(lswClsName, MAX_PATH*sizeof(wchar_t));
	if (liStatus <= 0)
		return NULL;
	
	// ���ȳ��Ե��õ�ǰwidget�Ĺ�����ӿڴ�������
	lpClsFactory = EinkuiGetSystem()->GetCurrentWidget()->GetDefaultFactory();
	if (lpClsFactory != NULL)
	{
		lpIterator = lpClsFactory->CreateElement(npParent, npTemplete,nuEID);
		if (lpIterator != NULL)
			return lpIterator;
	}

	lpClsFactory = PrepareAllocator(lswClsName);
	if (lpClsFactory == NULL)
		return NULL;

	// ��������
	return lpClsFactory->CreateElement(npParent, npTemplete,nuEID);


}


// ͨ����������������
IEinkuiIterator* __stdcall CXelAllocator::CreateElement(
	IN IEinkuiIterator* npParent,		// ������ָ��
	IN const wchar_t* nswClassName,		// ����
	IN ULONG nuEID					// �����Ϊ0��MAXULONG32����ָ����Ԫ�ص�EID; ����ȡ��һ��������ģ�������õ�ֵ��ΪEID�����ģ��Ҳû������EID����ʹ��XUIϵͳ�Զ�����
	)
{
	IElementFactory*	lpClsFactory = NULL;
	IEinkuiIterator*			lpIterator = NULL;
	ERESULT luResult = ERESULT_UNSUCCESSFUL;


	do 
	{
		BREAK_ON_NULL(nswClassName);

		// ���ȳ��Ե��õ�ǰwidget�Ĺ�����ӿڴ�������
		lpClsFactory = EinkuiGetSystem()->GetCurrentWidget()->GetDefaultFactory();
		BREAK_ON_NULL(lpClsFactory);

		lpIterator = lpClsFactory->CreateElement(npParent, nswClassName,nuEID);
		BREAK_ON_NULL(lpIterator);

		luResult = ERESULT_SUCCESS;

	} while (false);



	do 
	{	// ��⣬���Ĭ�Ͻӿڴ������ɹ��������ע����࣬���¼��ض�Ӧ�Ĺ����࣬���д���
		if (luResult == ERESULT_SUCCESS)
			break;
		
		lpClsFactory = PrepareAllocator(nswClassName);
		BREAK_ON_NULL(lpClsFactory);
			
		// ��������
		lpIterator = lpClsFactory->CreateElement(npParent, nswClassName,nuEID);

	} while (false);

	return lpIterator;
}

