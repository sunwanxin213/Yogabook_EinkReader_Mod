﻿/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */
#ifndef _ERESULT_H_
#define _ERESULT_H_
/* 
	本文定义Eiui的系统返回码
	Definitions for the return value from Eiui funcitons
*/



#define  ERESULT ULONG

#define ERESULT_SUCCEEDED(_X) ((_X&0x80000000)==0)
#define ERESULT_FAILED(_X) ((_X&0x80000000)!=0)


//////////////////////////////////////////////////////////////////////////
// 返回码定义规则
//////////////////////////////////////////////////////////////////////////
// 返回码分为3个域，其中最高位用于确定返回码的性质，如果为1表示返回的结果代表错误发生此时称作错误码；如果为0表示返回的是非错误的返回码
// 返回码的第二个域是错误码涉及的系统或者元素类型，同XUI.H文件中定义的消息主体一致，如LMSG_TP_SYSTEM是系统错误码，通常Xui默认都是用系统类型
// 返回码的第三个域是返回码子编码，取值不大于32767

// 返回码定义用宏
#define ERESULT_NORMAL_DEFINE(_TYPE,_NUMBER) ((((_TYPE)&0xFFFF)<<15)|((_NUMBER)&0x7FFF))

// 错误码定义用宏
#define ERESULT_ERROR_DEFINE(_TYPE,_NUMBER) ((((_TYPE)&0xFFFF)<<15)|((_NUMBER)&0x7FFF)|0x80000000)






//////////////////////////////////////////////////////////////////////////
// 非错误系统返回码
//////////////////////////////////////////////////////////////////////////
#define ERESULT_SUCCESS				ERESULT_NORMAL_DEFINE(LMSG_TP_SYSTEM,0)
#define ERESULT_DEFAULT				ERESULT_NORMAL_DEFINE(LMSG_TP_SYSTEM,1)

#define ERESULT_NOT_POINTOWNER		ERESULT_NORMAL_DEFINE(LMSG_TP_SYSTEM,10)	// 探测屏幕上的一个点是否属于某个ELEMENT时，返回不是，否则，返回success表示该点属于该ELEMENT

#define ERESULT_DRGDRP_REFUSE		ERESULT_NORMAL_DEFINE(LMSG_TP_SYSTEM,20)	// 拒绝Drag&Drop操作，用于父窗口对控件的Drag&Drop事件处理

#define ERESULT_NO_MESSAGE			ERESULT_NORMAL_DEFINE(LMSG_TP_SYSTEM,30)	// 当前消息队列空

#define ERESULT_NOT_PAINT			ERESULT_NORMAL_DEFINE(LMSG_TP_SYSTEM,35)	// 没有绘制，可能是窗口隐藏或者被遮挡

#define ERESULT_QUIT_OPLOOP			ERESULT_NORMAL_DEFINE(LMSG_TP_SYSTEM,50)	// 退出Operation Thread Message Loop
#define ERESULT_BLOCK				ERESULT_NORMAL_DEFINE(LMSG_TP_SYSTEM,52)	// 仍然阻塞，没有完成
#define ERESULT_QUIT_XUI			ERESULT_NORMAL_DEFINE(LMSG_TP_SYSTEM,60)	// 退出XUI系统



#define ERESULT_UNEXPECTED_MESSAGE	ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,1)	// 通常由消息处理类函数返回，表示该条消息未被处理
#define ERESULT_MSG_SENDTO_NEXT	ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,5)	// 将接收到的Hook消息传递给下一个Hook或者发送到原始接收者
#define ERESULT_WINMSG_SENDTO_NEXT		ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,5)	// 将Windows消息传递给下一个请求者或者交还系统处理
#define ERESULT_ENUM_CHILD			ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,10)	// 继续枚举所有子元素
#define ERESULT_STOP_ENUM_CHILD		ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,11)	// 停止枚举子元素
#define ERESULT_EXIT_ENUM			ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,12)	// 停止全部枚举行为
#define ERESULT_REDO_ENUM			ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,13)	// 对当前节点及其子节点重新枚举一次
#define ERESULT_DISCARD				ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,14)	// 通用返回值，告诉消息发送者放弃某种行为或数据
#define ERESULT_ACCEPT				ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,15)	// 通用返回值，告诉消息发送者接受某种行为或数据

#define ERESULT_MOUSE_OWNERSHIP		ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,20)	// 承认为鼠标落点的拥有者，即鼠标处于自身的作用范围

#define ERESULT_EDRAGDROP_BEGIN		ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,30)	// 请求进入XUI Drag&Drop过程，仅当处理EMSG_DRAG_BEGIN消息时有效
#define ERESULT_EDRAGDROP_ACCEPT	ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,32)	// XUI Drag&Drop过程中，接受体响应EMSG_XUI_DROP_TEST，表示可以接受Drop

#define ERESULT_WDRAGDROP_BEGIN		ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMGR,40)	// 请求进入Windows Drag&Drop过程，仅当处理EMSG_DRAG_BEGIN消息时有效




#define ERESULT_IDOK				ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMENT,1)	// 对话框返回的ok结果
#define ERESULT_IDCANCLE			ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMENT,2)	// 对话框返回的Cancel结果
#define ERESULT_IDYES				ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMENT,3)	// 对话框返回的Yes结果
#define ERESULT_IDNO				ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMENT,4)	// 对话框返回的No结果
#define ERESULT_IDABOUT				ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMENT,5)	// 对话框返回的About结果
#define ERESULT_IDRETRY				ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMENT,6)	// 对话框返回的Retry结果
#define ERESULT_IDIGNORE			ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMENT,7)	// 对话框返回的Ignore结果
#define ERESULT_IDCONTINUE			ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMENT,8)	// 对话框返回的Continue结果

#define ERESULT_REFUSE_MOVING		ERESULT_NORMAL_DEFINE(LMSG_TP_ELEMENT,10)	// ELEMENT不能移动
#define ERESULT_KEY_UNEXPECTED		ERESULT_DISCARD // 专门用于键盘消息的处理，当某个控件接受全部键盘消息，但只处理自己识别的按键，自身不处理的按键消息一定要返回这个结果
#define ERESULT_KEY_ACCEPTED		ERESULT_ACCEPT // 专门用于键盘消息的处理，当某个控件接受全部键盘消息，但只处理自己识别的按键，收到自身处理的按键消息后一定要返回这个结果

#define ERESULT_SKIP_RENDER_CONTENT	ERESULT_NORMAL_DEFINE(LMSG_TP_RENDER,5)	// 跳过绘制目标及其子元素，不给它们发送EMSG_PAINT消息
#define ERESULT_REDO_RENDER_CONTENT	ERESULT_NORMAL_DEFINE(LMSG_TP_RENDER,6)	// 再次绘制目标及其子元素，包括增效器本身也将再次执行




//////////////////////////////////////////////////////////////////////////
// 系统错误码
//////////////////////////////////////////////////////////////////////////
#define ERESULT_UNSUCCESSFUL		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,1)
#define ERESULT_ERROR_WINAPI		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,2)	// Windows API错误
#define ERESULT_WRONG_PARAMETERS	ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,3)
#define ERESULT_WRONG_PARAMETERS1	ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,4)	// 空指针错误
#define ERESULT_MESSAGE_EXCEPTION	ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,7)	// 消息处理过程发生异常
#define ERESULT_UNEXPECTED_CALL		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,8)	// 未预料的调用，即不该发生的调用
#define ERESULT_NOT_INITIALIZED		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,9)	// 未初始化
#define ERESULT_OBJECT_LOCKED		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,10)	// 对象被锁定
#define ERESULT_ELEMENT_INVALID		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,11)	// UI元素未定义
#define ERESULT_UNKNOWN_ERROR		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,12)	// 未知错误
#define ERESULT_ID_CONFLICT			ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,13)	// ID冲突
#define ERESULT_OBJECT_EXISTED		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,14)	// 对象已经存在，不能重复建立
#define ERESULT_ITERATOR_INVALID	ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,15)	// 无效的迭代器对象
#define ERESULT_ACCESS_CONFLICT		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,16)	// 访问冲突
#define ERESULT_SENDER_INVALID		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,17)	// 消息发送者无效
#define ERESULT_DRAGGING_UNSUPPORT		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,18)	// 不支持拖动
#define ERESULT_NOT_ENOUGH_BUFFER	ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,19)	// 缓冲区不足
#define ERESULT_OBJECT_NOT_FOUND	ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,20)	// 对象不存在
#define ERETULT_WRONG_STYLE			ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,21)	// 风格错误

#define ERESULT_INSUFFICIENT_RESOURCES	ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,30)	// 内存或者其他资源不足
#define ERESULT_FILE_NOT_FOUND		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,31)	// 没有找到文件
#define ERESULT_STRING_NOT_FOUND	ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,32)	// 配置文件中的信息没有找到
#define ERESULT_BUSY				ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,33)	// 系统忙碌
#define ERESULT_TIMEOUT				ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,34)	// 系统超时
#define ERESULT_WRONG_THREAD		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,35)	// 错误的线程
#define ERESULT_STALLED_THREAD		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,36)	// 线程阻塞

#define ERESULT_DIRECTX_ERROR		ERESULT_ERROR_DEFINE(LMSG_TP_SYSTEM,50)	// Direct2D or Direct3D发生错误，通常用于直接操作D3dD2d的模块，向系统返回Paint结果，阻止系统的当前绘制过程



#define ERESULT_NOT_SET				ERESULT_ERROR_DEFINE(LMSG_TP_OTHERS,1)	// 通常用于程序处理流程，表示一个返回值尚未被设置

//////////////////////////////////////////////////////////////////////////
// 位图相关错误
//////////////////////////////////////////////////////////////////////////
#define ERESULT_BMP_CREATEDECODER	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 1)	// 从文件创建解码器接口失败
#define ERESULT_BMP_GETFRAME		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 2)	// 从解码器获取帧接口失败
#define ERESULT_BMP_CREATEWICBMP	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 3)	// 转换WIC BITMAP接口失败
#define ERESULT_BMP_WICBMPNOTREADY	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 4)	// WIC BITMAP接口为NULL
#define ERESULT_BMP_WICLOCKBMP		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 5)	// WIC BITMAP接口Lock失败
#define ERESULT_BMP_ERRGETSIZE		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 6)	// 获取位图宽高信息失败
#define ERESULT_BMP_IVLDFORMAT		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 7)	// 无效的位图格式
#define ERESULT_BMP_GETBMPBUF		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 8)	// 获取位图数据缓冲区失败
#define ERESULT_BMP_CREATECVTER		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 8)	// 创建WIC格式转换器接口失败
#define ERESULT_BMP_INITCVTER		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 9)	// 初始化WIC格式转换器接口失败
#define ERESULT_BMP_CREATED2DBMP	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 10)	// 创建D2D位图接口失败
#define ERESULT_BMP_CREATESCALER	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 11)	// 创建WIC缩放接口失败
#define ERESULT_BMP_INITSCALER		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 12)	// 初始化WIC缩放接口失败
#define ERESULT_BMP_INVALIDXY		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 13)	// 输入的坐标值无效
#define ERESULT_BMP_CREATEWICBMPRT	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 14)	// CreateWicBitmapRenderTarget失败
#define ERESULT_BMP_CREATEDWRITEFAC	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 15)	// 创建DWrite Factory失败
#define ERESULT_BMP_CREATETEXTFMT	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 16)	// 创建DWrite ITextFormat失败
#define ERESULT_BMP_CREATETEXTLAYOUT	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 17)	// 创建DWrite ITextLayout失败
#define ERESULT_BMP_GETMETRICS		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 18)	// 调用DWrite GetMetrics失败
#define ERESULT_BMP_EXSTRACTICON	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 19)	// 从EXE中释放icon失败
#define ERESULT_BMP_COPYFROMRT		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 20)	// 从Rt复制位图失败
#define ERESULT_BMP_CREATEBMPBRUSH	ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 21)	// 创建位图Brush失败
#define ERESULT_BMP_DXGIDESC		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 22)	// 获取DXGISurface描述失败
#define ERESULT_BMP_INVALIDBUF		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 23)	// 输入缓冲区无效
#define ERESULT_BMP_DRAWRT			ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 24)	// 输出到RenderTarget时出错
#define ERESULT_BMP_CREATEWICRT		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 25)	// 创建WICBitmapRenderTarget失败
#define ERESULT_BMP_COPYFROMBUF		ERESULT_ERROR_DEFINE(LMSG_TP_BITMAP, 26)	// CopyFromBuffer失败




































#endif//_ERESULT_H_