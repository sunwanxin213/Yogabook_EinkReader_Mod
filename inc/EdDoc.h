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
__interface IEdPage;	// ҳ
__interface IEdBitmap;	// λͼ
__interface IEdAnnotManager;	// ע�͹�������ÿҳ�����Լ���ע�͹�����
__interface IEdAnnot;				// ��ע�����ǻ����ӿڣ�ͨ���ڲ��Ľӿ���ת����ʵ�����͵Ľӿ�
__interface IEdInkAnnot;			// ī��ע��
__interface IEdStextAnnot;		// ����/ɾ��/�»���ע��
__interface IEdTextAnnot;			// ���ֱ�ǩע��
__interface IEdStructuredTextPage;	// �ṹ�ı�ҳ�����ڴ���structured text��Ϣ 		
__interface IEdStextQuadList;		// �ṹ�ı��Ľṹ��Ϣ
__interface IEdAnnotList;			// Annot�б����

typedef IEdModule* IEdModule_ptr;
typedef IEdDocument* IEdDocument_ptr;
typedef IEdPage* IEdPage_ptr;
typedef IEdAnnotManager* IEdAnnotManager_ptr;
typedef IEdAnnot* IEdAnnot_ptr;
typedef IEdInkAnnot* IEdInkAnnot_ptr;
typedef IEdStextAnnot* IEdStextAnnot_ptr;
typedef IEdTextAnnot* IEdTextAnnot_ptr;
typedef IEdBitmap* IEdBitmap_ptr;
typedef IEdStextQuadList* IEdStextQuadList_ptr;
typedef IEdStructuredTextPage* IEdStructuredTextPage_ptr;
typedef IEdAnnotList* IEdAnnotList_ptr;


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

typedef void(__stdcall *PEDDOC_THUMBNAILS_CALLBACK)(uint32Eink pageNum,void_ptr npContext);
// pageNum:
//		the page number that it's thumbnail have rendered.
// this routine will be called as many as the page count of the current document;


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
#define EDERR_3RDPART_FAILED	EDERR_MAKE_ERR(130)




#define		EDPDF_ANNOT_NONE	0
#define		EDPDF_ANNOT_INK		1
#define		EDPDF_ANNOT_UNDERL		2
#define		EDPDF_ANNOT_DELETE		3
#define		EDPDF_ANNOT_HIGHLIGHT	4
#define		EDPDF_ANNOT_TEXT		5



// ���ú�
#define BOOL_TRUE(_X) (_X!=0)
#define BOOL_FALSE(_X) (_X==0)

#define ERR_SUCCESS(_X) (_X<=0x7fffffff)
#define ERR_FAILED(_X) (_X>0x7fffffff)


#define ANN_SIGN_TYPE(_X)	(_X>>(64-8))
#define ANN_SIGN_PAGE(_X)	((_X>>(64-8-16))&0xFFFF)
#define ANN_SIGN_ALT(_X)	((_X>>(64-8-16-16))&0xFFFF)
#define ANN_SIGN_CHECKSUM(_X)	(_X&0xFFFFFF)
#define ANN_MAKE(_T,_P,_A,_C) (((ULONGLONG)_T<<(64-8))|((ULONGLONG)_P<<(64-8-16))|((ULONGLONG)_A<<(64-8-16-16))|(_C&0xFFFFFF))

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

typedef _ED_SIZE ED_POINT,*ED_POINT_PTR;
typedef _ED_SIZEF ED_POINTF,* ED_POINTF_PTR;

typedef struct _ED_LINE {
	int32Eink x1;
	int32Eink x2;
	int32Eink y1;
	int32Eink y2;
}ED_LINE, *ED_LINE_PTR;
typedef struct _ED_LINEF {
	float32 x1;
	float32 x2;
	float32 y1;
	float32 y2;
}ED_LINEF, *ED_LINEF_PTR;


typedef struct _ED_COLOR {
	int32Eink r;
	int32Eink g;
	int32Eink b;
	int32Eink a;
}ED_COLOR,* PED_COLOR;

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
		OUT IEdDocument_ptr* documentPtrPtr
	) = NULL;

};


// �ĵ�����
__interface IEdDocument :public IBaseObject
{
	virtual bool32 LoadAllPage(PEDDOC_CALLBACK callBackFun, void_ptr contextPtr, PPAGE_PDF_CONTEXT initPage)=NULL;
	virtual bool32 LoadAllThumbnails(PEDDOC_THUMBNAILS_CALLBACK callBack, void_ptr contextPtr,const wchar_t* nphumbnailPathName =NULL) = NULL;
	virtual bool32 GetThumbanilsPath(wchar_t* npszPathBuffer, int niLen) = NULL;
	virtual int32Eink GetMetaData(char* keyName,char* dataBuf, int32Eink bufSize)=NULL;
	virtual int32Eink GetDocType(void) = NULL;
	virtual int32Eink GetPageCount(void) = NULL;
	virtual IEdPage_ptr GetPage(int32Eink pageIndex) = NULL;
	virtual IEdPage_ptr GetPage(PPAGE_PDF_CONTEXT contextPtr) = NULL;	// contextPtr==NULL for the first page of this doc
	virtual IEdPage_ptr GetPage(IEdPage_ptr currentPage, int32Eink pagesOff) = NULL;
	virtual bool32 GetThumbnailPathName(int32Eink pageIndex, char16 pathName[256],bool* hasAnnot=NULL)=NULL;
	virtual int32Eink GetAnnotPageCount(void) = NULL;

	virtual bool32 SaveAllChanges(const char16_ptr pathName = NULL) = NULL;

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

	virtual IEdAnnotManager_ptr GetAnnotManager(void) = NULL;	// ��õ�����������ͷţ����Զ�����page����һͬ����

	virtual bool32 GetSelectedText(
		IN ED_RECTF_PTR selBox,
		OUT char16_ptr textBuf,
		IN int32Eink bufSize
	) = NULL;

	virtual IEdStructuredTextPage_ptr GetStructuredTextPage(void) = NULL;	// ���صĶ�����Ҫ����release�ͷ�
};

// λͼ����
__interface IEdBitmap :public IBaseObject
{
	virtual bin_ptr GetBuffer() = NULL;
	virtual int32Eink GetWidth() = NULL;
	virtual int32Eink GetHeight() = NULL;
};


__interface IEdAnnotManager
{
	// �Ӵ浵����װ��һ��Annotation
	virtual IEdAnnot_ptr LoadAnnotFromArchive(buf_ptr buf, uint32Eink bufSize) = NULL;

	// �½��ı��ʼ�
	virtual IEdAnnot_ptr AddTextAnnot(
		IN ED_POINT position,
		IN char16_ptr text
	) = NULL;

	// �½�īˮ�ʼǣ����ض�����Ҫ�ͷ�
	virtual IEdAnnot_ptr AddInkAnnot(
		IN ED_POINTF* stroke,
		IN int32Eink pointCount,
		IN ED_COLOR* color=NULL,
		IN float32 border=1.0f
	) = NULL;

	// �½�Highlight�ʼǣ����ض�����Ҫ�ͷ�
	virtual IEdAnnot_ptr AddHighLightAnnot(
		IN IEdStextQuadList_ptr stext,
		IN ED_COLOR* color = NULL,
		IN int32Eink clrN = 3
	);

	// �½�ɾ���ֱʼǣ����ض�����Ҫ�ͷ�
	virtual IEdAnnot_ptr AddDeleteLineAnnot(
		IN IEdStextQuadList_ptr stext,
		IN ED_COLOR* color = NULL,
		IN int32Eink clrN = 3
	);

	// �½��»��߱ʼǣ����ض�����Ҫ�ͷ�
	virtual IEdAnnot_ptr AddUnderLineAnnot(
		IN IEdStextQuadList_ptr stext,
		IN ED_COLOR* color = NULL,
		IN int32Eink clrN = 3
	);

	// ͨ��ǩ������һ��Annot�����ض�����Ҫ�ͷ�
	virtual IEdAnnot_ptr GetAnnotBySignature(ULONGLONG signature) = NULL;
	virtual ULONGLONG GetSignature(IEdAnnot_ptr annot) = NULL;

	// ��ȡ��һ���ʼǣ����ض�����Ҫ�ͷ�
	virtual IEdAnnot_ptr GetFirstAnnot(void) = NULL;

	// ��ȡ��һ���ʼǣ����ض�����Ҫ�ͷ�
	virtual IEdAnnot_ptr GetNextAnnot(IEdAnnot_ptr crt) = NULL;

	// ��ȡ���ж�����б���annotList==NULLʱ��������Ҫ��List��Ԫ��,������������ʱ����-1���ɹ����ػ�õ�IEdAnnot_ptr������
	virtual int32Eink GetAllAnnot(IEdAnnot_ptr* annotList,int bufSize) = NULL;

	// ���һ�ʻ��Ӵ�����һϵ��Ink�ʼǶ����ཻ������annotList==NULLʱ��������Ҫ�Ļ���������,������������ʱ����-1
	virtual int32Eink DetectInkAnnot(
		IN ED_POINTF* stroke,
		IN int32Eink pointCount,
		OUT	IEdAnnot_ptr* annotList,	// ���ڷ��������ཻ����Ļ�����������һ���Է����Ļ����������� IEdAnnot_ptr buf[256];���ض�����Ҫ�ͷ�
		IN int32Eink bufSize					// �����������ĵ�Ԫ���������ֽ���
	) = NULL;

	// ɾ��һ��Annotation�����ô˺�����IEdAnnot_ptr annot����Ȼ��Ҫ�ͷ�
	virtual void RemoveAnnot(
		IEdAnnot_ptr annot
	) = NULL;
};

__interface IEdAnnot :public IBaseObject
{
	virtual uint32Eink GetType() = NULL;	// EDPDF_ANNOT_INK ,EDPDF_ANNOT_UNDERL, EDPDF_ANNOT_DELETE,EDPDF_ANNOT_HIGHLIGHT, EDPDF_ANNOT_TEXT
	virtual char* GetTypeName() = NULL;		// "ink" "text" "highlight" "underline" "deleteline" or "Identity"

	virtual IEdTextAnnot_ptr ConvertToTextAnnot(void) = NULL;	// ����ֵ�����ͷ�
	virtual IEdInkAnnot_ptr ConvertToInkAnnot(void) = NULL;		// ����ֵ�����ͷ�
	virtual IEdStextAnnot_ptr ConvertToStextAnnot(void) = NULL;	// ����ֵ�����ͷ�

	// ��buf==NULLʱ��������Ҫ�Ļ������ֽ��������������㷵��-1
	virtual uint32Eink SaveToAchive(buf_ptr buf, uint32Eink bufSize) = NULL;	// ���˶��󱣴浽�浵���������Ӵ浵�ָ��浽��Page(ͨ������IEdPage�ķ��� LoadAnnotFromArchive)
};

__interface IEdTextAnnot :public IEdAnnot
{
	virtual const char16_ptr GetText(void) = NULL;

	virtual ED_POINT GetPosition(void) = NULL;

	virtual ED_RECT GetRect(void) = NULL;
};

__interface IEdInkAnnot :public IEdAnnot
{
	virtual void SetColor(ED_COLOR clr) = NULL;		// ����������ɫ
	virtual ED_COLOR GetColor(void) = NULL;

	virtual void SetBorder(int32Eink border) = NULL;	// ����������� 1 ~ N
	virtual int32Eink GetBorder(void) = NULL;
};

__interface IEdStextAnnot :public IEdAnnot
{
	virtual void SetColor(ED_COLOR clr) = NULL;		// ����������ɫ
	virtual ED_COLOR GetColor(void) = NULL;

	virtual IEdStextQuadList_ptr GetQuadsList(void) = NULL; // ��÷ֿ��б�
};

__interface IEdStructuredTextPage :public IBaseObject	// ѡ�����ڴ���structured text��Ϣ 
{
	// ̽���ı�ѡ�����
	virtual bool DetectSelectedText(
		IN const ED_POINTF* aPoint,
		IN const ED_POINTF* bPoint,
		OUT IEdStextQuadList_ptr* stext,	// ���ؽṹ���ı�������Ҫ�ͷ�
		OUT IEdAnnotList_ptr* impactedAnnot=NULL,// ������ײ������Annot�����б���Ҫ�ͷ�
		IN bool32 includeImpacted = false	// ����ײ��annot����Ҳ����ѡ��
	) = NULL;	// ���صĶ�����Ҫ�ͷ�

	// ����Ŀ�������ı�������ֵ�Ǹ��Ƶ��ַ�������������β0
	virtual int32Eink CopyText(
		IN ED_RECTF_PTR selBox,
		OUT char16_ptr textBuf,	// ����NULL����������ʵ�ʵ��ַ�����������β0
		IN int32Eink bufSize
	) = NULL;

};

__interface IEdStextQuadList :public IBaseObject
{
	// ���A�㣨��ֹ�㣩
	virtual void GetAPoint(
		OUT ED_POINTF* pt
		) = NULL;

	// ���B�㣨��ֹ�㣩
	virtual void GetBPoint(
		OUT ED_POINTF* pt
		) = NULL;

	// ��÷ֿ�����
	virtual int32Eink GetQuadCount(void) = NULL;

	// ���һ���ֿ��λ��
	virtual void GetQuadBound(
		IN int32Eink index,
		OUT ED_RECTF_PTR quad
		) = NULL;

	virtual const ED_RECTF* GetQuad(IN int32Eink index)=NULL;
};

__interface IEdAnnotList :public IBaseObject
{
	virtual int32Eink GetCount(void) = NULL;
	virtual IEdAnnot_ptr GetAnnot(int32Eink index) = NULL;	// ���صĶ�����Ҫ�ͷ�
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