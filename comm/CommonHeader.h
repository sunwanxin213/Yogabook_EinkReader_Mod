/* Copyright 2019 - present Lenovo */
/* License: COPYING.GPLv3 */



// ����ͳһ�����������������ͷ�ļ�
#include "gdiplus.h"
using namespace Gdiplus;

#include <d2d1.h>
#include <d3d10_1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <ShellAPI.h>

#include <d3d10_1.h>
//#include <d3dx10math.h>
using namespace D2D1;


#include "resource.h"

// ϵͳ���
#include "Shlobj.h"

// ״̬����
#include "EResult.h"

// ͨ�û���
#include "CfgIface.h"
#include "cmmBaseObj.h"

// ͨ�����ݽӿ�
#include "BpTree.h"
#include "cmmstruct.h"
#include "cmmPath.h"
#include "graphy/XgBitArray.h"

// �ۼ�׷��
//#include "cmmTrace.h"

// �ӿ�ͷ�ļ�
#include "Einkui.h"


#include "XsWgtContext.h"
#include "XsWidgetImp.h"
#include "XsBmpList.h"
#include "XsBrushList.h"
#include "einkuiimp.h"
#include "ElementImp.h"
#include "XleIteratorImp.h"
#include "XsMsgImp.h"
#include "XsMsgQueue.h"
#include "XleMgrImp.h"

#include "./Graphy/XgD2DBitmap.h"
#include "./Graphy/XgD2DBrush.h"
#include "./Graphy/XgD2DEngine4Eink.h"

#include "graphy/XgTextBitmapImp.h"

#include "./Allocator/XelFactoryMgr.h"
#include "./Allocator/XelDataMgr.h"
#include "./Allocator/XelAllocator.h"



// 
#include "ThreadAbort.h"
#include "Xuix.h"
