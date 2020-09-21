/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */

#include "windows.h"
#include "pdfDocument.h"
#include "pdfModule.h"
#include "cmmString.h"


#pragma comment(lib,"libmupdf.lib")
#pragma comment(lib,"libthirdparty.lib")
#pragma comment(lib,"..\\mupdf\\platform\\win32\\x64\\Release\\libresources.lib")

DEFINE_BUILTIN_NAME(CEdpdfModule)
CEdpdfModule* CEdpdfModule::glModule = NULL;



// ��ȡģ�����
IEdModule_ptr __stdcall EdGetModule(void)
{
	CEdpdfModule* uniqueObj = CEdpdfModule::GetUniqueObject();

	return uniqueObj;
}



CEdpdfModule::CEdpdfModule()
{
	fzContext = NULL;
	colorSpace = NULL;
}

CEdpdfModule::~CEdpdfModule()
{
	if(colorSpace!= NULL)
		fz_drop_colorspace(fzContext,colorSpace);
	if(fzContext != NULL)
		fz_drop_context(fzContext);
}

// ���Ψһ����
CEdpdfModule* CEdpdfModule::GetUniqueObject(void)
{
	if (glModule == NULL)
		CEdpdfModule::CreateInstance();

	CMMASSERT(glModule != NULL);

	return glModule;
}

void lock_mutex(void *user, int lock)
{
	CExclusiveAccess* mutex = (CExclusiveAccess *)user;

	mutex->Enter();
}

void unlock_mutex(void *user, int lock)
{
	CExclusiveAccess* mutex = (CExclusiveAccess *)user;

	mutex->Leave();
}


ULONG CEdpdfModule::InitOnCreate(void) {
	if (glModule != NULL)
		return EDERR_OBJECT_EXISTED;

	glModule = this;

	fz_locks_context fzLocks;
	fzLocks.user = mFzCtxLock;
	fzLocks.lock = lock_mutex;
	fzLocks.unlock = unlock_mutex;

	fzContext = fz_new_context(NULL,&fzLocks, FZ_STORE_UNLIMITED);// FZ_STORE_DEFAULT);
	if (fzContext == NULL)
		return EDERR_FAILED_FZCONTEXT;

	transition.duration = 0.25f;
	transition.type = FZ_TRANSITION_FADE;
	colorSpace = fz_device_bgr(fzContext);

	return EDERR_SUCCESS;
}

// ��ȡ֧�ֵ��ļ���ʽ����
int32 CEdpdfModule::GetTypeCount(void)
{
	return 1;
}

// ��ȡ֧�ֵ��ļ���ʽ����չ��
const char16_ptr CEdpdfModule::GetTypeName(int32 index)
{
	if(index == 0)
		return L"PDF";

	return L"";
}


// ���ĵ�
ED_ERR CEdpdfModule::OpenDocument(
	IN char16* pathName,
	OUT IEdDocument_ptr* documentPtrPtr
)
{
	if (pathName == NULL || pathName[0] == 0 || documentPtrPtr == NULL)
		return EDERR_WRONG_PARAMETERS;

	cmmStringW fileName = wcsrchr(pathName, L'.');
	if (fileName.compare(L".pdf",false) != 0)
		return EDERR_UNSUCCESSFUL;

	auto docObj = CpdfDocument::CreateInstance(pathName);
	if(docObj == NULL)
		return EDERR_UNSUCCESSFUL;

	*documentPtrPtr = docObj;
	return EDERR_SUCCESS;
}
