/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */
#ifndef _EDDOC_H_
#define _EDDOC_H_
#include "cmmBaseObj.h"

// Ĭ�϶���
#ifndef IN
#define IN
#endif//IN
#ifndef OUT
#define OUT
#endif//OUT


//////////////////////////////////////////////////////////////////////////
// ǰ�������������ע
//////////////////////////////////////////////////////////////////////////

__interface IEdModule; // ģ��
__interface IEdDocument; // �ĵ�
__interface IEdPage;		// ҳ
__interface IEdComment;  // ע��
__interface IEdBitmap;	// λͼ

typedef IEdModule* IEdModule_ptr;
typedef IEdDocument* IEdDocument_ptr;
typedef IEdPage* IEdPage_ptr;
typedef IEdComment* IEdComment_ptr;
typedef IEdBitmap* IEdBitmap_ptr;


//////////////////////////////////////////////////////////////////////////
// ǰ����������
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// �������Ͷ���
//////////////////////////////////////////////////////////////////////////
typedef unsigned char bin;
typedef unsigned char* bin_ptr;
typedef unsigned long bool32;
typedef unsigned long uint32Eink;
typedef long int32Eink;
#ifndef _INTTYPE_CONFLICT	//$ax$ �˴���Sumatra���ͻ�����������ˣ�ע�⣬���EdDoc.h�ļ���EinkReader����ͬ���ļ������������
#define uint32 uint32Eink
#define int32 int32Eink
#endif
typedef char* buf_ptr;
typedef wchar_t char16;
typedef wchar_t* char16_ptr;
typedef float float32;
typedef double float64;
typedef void* void_ptr;



#define false32 (int32Eink)		0
#define success32(int32Eink)	1
//////////////////////////////////////////////////////////////////////////
// �������Ͷ���
//////////////////////////////////////////////////////////////////////////

typedef void(__stdcall *PEDDOC_CALLBACK)(uint32Eink loadingStep, uint32Eink pageCount, void_ptr npContext);
// loadingStep: 
//		0			start of loading
//		MAXULONG	end of loading
//		otherwise	loading

//typedef ERESULT(__stdcall IBaseObject::*PXUI_CALLBACK)(ULONG nuFlag, LPVOID npContext);
//typedef ERESULT(__stdcall *PXUI_CUSTOM_DRAW_CALLBACK)(unsigned char* npPixels, ULONG nuWidth, ULONG nuHeight, bool nbRefresh);//nbRefresh���ã�������ύ����


//////////////////////////////////////////////////////////////////////////
// ��������
//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
// ����ֵ
//////////////////////////////////////////////////////////////////////////
typedef uint32Eink ED_ERR;

#define EDERR_SUCCESS 0
#define EDERR_UNSUCCESSFUL MAXDWORD

#define EDERR_MAKE_ERR(_X) (_X|0x80000000)
#define EDERR_OBJECT_EXISTED EDERR_MAKE_ERR(1)
#define EDERR_WRONG_PARAMETERS EDERR_MAKE_ERR(2)

#define EDERR_FAILED_FZCONTEXT	EDERR_MAKE_ERR(100)
#define EDERR_FIALED_OPENFILE	EDERR_MAKE_ERR(105)
#define EDERR_HAS_PASSWORD		EDERR_MAKE_ERR(106)
#define EDERR_UNKOWN_DOCTYPE	EDERR_MAKE_ERR(107)
#define EDERR_EMPTY_CONTENT		EDERR_MAKE_ERR(110)
#define EDERR_FIALED_LOAD_PAGE	EDERR_MAKE_ERR(111)
#define EDERR_THREAD_ERROR		EDERR_MAKE_ERR(115)
#define EDERR_NOTENOUGH_MEMORY	EDERR_MAKE_ERR(120)






// ���ú�
#define BOOL_TRUE(_X) (_X!=0)
#define BOOL_FALSE(_X) (_X==0)

#define ERR_SUCCESS(_X) (_X<=0x7fffffff)
#define ERR_FAILED(_X) (_X>0x7fffffff)

//////////////////////////////////////////////////////////////////////////
// �ṹ�嶨�壬�˴�����Ľṹ�����ڿ�ģ�齻������
//////////////////////////////////////////////////////////////////////////

#pragma pack(4)

//typedef struct _STEMS_TIMER {
//	void* Context;		//
//}STEMS_TIMER, *PSTEMS_TIMER;

typedef struct _ED_RECT {
	int32Eink left;
	int32Eink right;
	int32Eink top;
	int32Eink bottom;
}ED_RECT,* ED_RECT_PTR;
typedef struct _ED_RECTF {
	float32 left;
	float32 right;
	float32 top;
	float32 bottom;
}ED_RECTF,* ED_RECTF_PTR;

#define ED_RECT_WIDTH(_X) (_X.right - _X.left)
#define ED_RECT_HEIGHT(_X) (_X.bottom - _X.top)

typedef struct _ED_SIZE {
	int32Eink x;
	int32Eink y;
}ED_SIZE,* ED_SIZE_PTR;

typedef struct _ED_SIZEF {
	float32 x;
	float32 y;
}ED_SIZEF,* ED_SIZEF_PTR;

#define ED_SIZE_CLEAN(_X) (_X.x=0,_X.y = 0)
#define ED_SIZEF_CLEAN(_X) (_X.x=0.0f,_X.y = 0.0f)

typedef _ED_SIZEF ED_POINT;
typedef _ED_SIZEF ED_POINTF;

typedef struct _PAGE_PDF_CONTEXT {
	ULONG pageIndex;	
	ULONG pageContext;	
	ULONG pageContext2;
}PAGE_PDF_CONTEXT, *PPAGE_PDF_CONTEXT;

#pragma pack()
//////////////////////////////////////////////////////////////////////////
// �ṹ�嶨�����
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// �ӿڶ���
//////////////////////////////////////////////////////////////////////////

// ģ��ӿڶ���
__interface IEdModule :public IBaseObject
{
	// ��ȡ֧�ֵ��ļ���ʽ����
	virtual int32Eink GetTypeCount(void) = NULL;
	// ��ȡ֧�ֵ��ļ���ʽ����չ��
	virtual const char16_ptr GetTypeName(int32Eink index=0) = NULL;

	// ���ĵ�
	virtual ED_ERR OpenDocument(
		IN char16_ptr pathName,
		OUT IEdDocument_ptr* documentPtrPtr,
		IN int32Eink asType=-1	// -1 not indicated
	) = NULL;

};


// �ĵ�����
__interface IEdDocument :public IBaseObject
{
	virtual bool32 LoadAllPage(PEDDOC_CALLBACK callBackFun, void_ptr contextPtr)=NULL;
	virtual int32Eink GetMetaData(char* keyName,char* dataBuf, int32Eink bufSize)=NULL;
	virtual int32Eink GetDocType(void) = NULL;
	virtual int32Eink GetPageCount(void) = NULL;
	virtual IEdPage_ptr GetPage(int32Eink pageIndex) = NULL;
	virtual IEdPage_ptr GetPage(PPAGE_PDF_CONTEXT contextPtr) = NULL;	// contextPtr==NULL for the first page of this doc
	virtual IEdPage_ptr GetPage(IEdPage_ptr currentPage, int32Eink pagesOff) = NULL;

	//////////////////////////////////////////////////////////////////////////
	// following functions just work on a txt document.

	// This method to update the layout of all pages.
	IEdPage_ptr Rearrange(PPAGE_PDF_CONTEXT contextPtr)=NULL; 

	// This method to update the layout of all pages.
	IEdPage_ptr Rearrange(IEdPage_ptr currentPage)=NULL;

	// This method to change the font used to render this document. 
	virtual void SetFont(const char16_ptr fontName, int32Eink fontSize)=NULL;

	// this method to set the view port to render txt document. 
	virtual void SetViewPort(int32Eink viewWidth, int32Eink viewHeight) = NULL;

};
// keyName
//key: Which meta data key to retrieve...
//	"format"
//	"encryption"
//	"info:Author"
//	"info:Title"
// keyName
//key: Which meta data key to retrieve...

//Basic information:
//	'format'	-- Document format and version.
//	'encryption'	-- Description of the encryption used.

//From the document information dictionary:
//	'info:Title'
//	'info:Author'
//	'info:Subject'
//	'info:Keywords'
//	'info:Creator'
//	'info:Producer'
//	'info:CreationDate'
//	'info:ModDate'


//From the document information dictionary:
//	'info:Title'
//	'info:Author'
//	'info:Subject'
//	'info:Keywords'
//	'info:Creator'
//	'info:Producer'
//	'info:CreationDate'
//	'info:ModDate'


// ҳ����
__interface IEdPage :public IBaseObject
{
	virtual bool32 GetMediaBox(
		OUT ED_RECTF_PTR mediaBox
	) = NULL;
	virtual bool32 GetCropBox(
		OUT ED_RECTF_PTR cropBox
	) = NULL;
	virtual bool32 GetBleedBox(
		OUT ED_RECTF_PTR bleedBox
	) = NULL;
	virtual IEdBitmap_ptr Render(
		IN float32 scalRatio,
		IN bool32 cleanUp
	) = NULL;
	virtual int32Eink GetPageIndex(void) = NULL;

	virtual bool32 GetPageContext(
		PPAGE_PDF_CONTEXT contextPtr
	) = NULL;

};

// λͼ����
__interface IEdBitmap :public IBaseObject
{
	virtual bin_ptr GetBuffer() = NULL;
	virtual int32Eink GetWidth() = NULL;
	virtual int32Eink GetHeight() = NULL;
};


//////////////////////////////////////////////////////////////////////////
// �ӿڶ������
//////////////////////////////////////////////////////////////////////////








////////////////////////////////////////////////////////////////////////////
//// �����ں�������
////////////////////////////////////////////////////////////////////////////
//#ifdef __cplusplus
//extern "C" {
//#endif
//
//	// ��ȡģ�����
//	IEdModule_ptr __stdcall EdGetModule(void);
//
//#ifdef __cplusplus
//}
//#endif
typedef IEdModule_ptr(__stdcall *GetModule_Proc)(void);


//////////////////////////////////////////////////////////////////////////
// �ļ�����
//////////////////////////////////////////////////////////////////////////
#endif//_XUI_H_