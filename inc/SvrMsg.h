#pragma once
#include "EiMsgQueue.h"
#include "EinkIteAPI.h"

// ����SDK��Sevice�������Ҫִ������App������DLL֮���ͨѶ���ݺͶ���

//////////////////////////////////////////////////////////////////////////
// APP���͸�Service����Ϣ����
#define EMHOSTID_REG_APP 1
// MsgBuf:struct REG_APP_INFO

//Homepage����ָ��APP
#define EMHOSTID_RUN_APP 2
// MsgBuf:EI_MSG_RUN_APP
typedef struct  _EI_MSG_RUN_APP {
	wchar_t FilePath[MAX_PATH];	//��ִ���ļ�·��
	wchar_t CommandLine[MAX_PATH];	//��Ҫ���Ĳ���
	int SessionID;		//�û�ID��0��ʾsystem�û�

}EI_MSG_RUN_APP, *PEI_MSG_RUN_APP;

//����
#define EMHOSTID_DRAW 3
// MsgBuf: REG_APP_DRAW
typedef struct  _EI_MSG_APP_DRAW {
	EI_RECT Area;
	ULONG BufferOffset;		// ���ƻ����ƫ����

}EI_MSG_APP_DRAW, *PEI_MSG_APP_DRAW;

//APP����������Ļ��ʾ����
#define EMHOSTID_SET_DISPLAY_ORIENTATION 4
// MsgBuf:DWORD

//APP����Einkģʽ��0,1,2
#define EMHOSTID_SET_SCENARIO 5
// MsgBuf:DWORD

// APP�����ػ�
#define EMHOSTID_INVALID_PANEL 6
// MsgBuf:EI_RECT

// APPע���Լ�
#define EMHOSTID_UNREG_APP 7
// MsgBuf:EI_RECT

// ����FWģʽ
#define EMHOSTID_SET_FW_MODE 8
// MsgBuf:DWORD

//APP��ȡ��ǰEINK�豸��Ϣ
#define EMHOSTID_GET_EINK_INFO 10
// MsgBuf: none

//APP��ȡ��ǰ��ʾ�趨
#define EMHOSTID_GET_APP_CONTEXT 11
// MsgBuf: none

//Switcher����
#define EMHOSTID_SWITCHER_DRAW 15
// MsgBuf: REG_APP_DRAW

//����Switcher��λ��
#define EMHOSTID_SWITCHER_LOCATION 16
// MsgBuf: EI_RECT

//��Ļ�������仯֪ͨ
#define EMHOSTID_ORIENTATION_CHANGED 17
// MsgBuf: DWORD

//������̬�����仯֪ͨ
#define EMHOSTID_LAPTOP_MODE_CHANGED 18
// MsgBuf: ULONG


//ϵͳ��Դ�仯״̬�仯֪ͨ
#define EMHOSTID_POWER_CHANGE	19
// MsgBuf: DWORD 0��ʾ����Connected standby 1��ʾConnected standby���� 2��ʾϵͳ�ػ�


//������Ļ
#define EMHOSTID_CLEANUP_SCREEN	20
// MsgBuf:  unsigned char��background color

//���ùػ�����ͼƬ��·��
#define EMHOSTID_SET_SHUTDOWN_COVER 21
// MsgBuf: wchat_t*

//���ü���ͼƬ��·��
#define EMHOSTID_SET_KEYBOARD_PATH 22
// MsgBuf: wchat_t*

//������ʾ״̬Ҫ�����仯
#define EMHOSTID_SET_SHOW_STATUS 23
// MsgBuf: ULONG 0������ʾAPP 1��ʾ���� 2��ʾ����A 3��ʾ����B 4����ӦAPP��������
#define SHOW_STATUS_NORMAL 0
#define SHOW_STATUS_COVER 1
#define SHOW_STATUS_KEYBOARD_A 2	//˫���ɻ���
#define SHOW_STATUS_KEYBOARD_B 3	//���̲�����
#define SHOW_STATUS_NOAPP 4

//�ػ��߳�֪ͨ��������ָ��Ӧ��
#define EMHOSTID_PROTECT_RUN_APP 24
// MsgBuf: ULONG 0Homebar 1Settings
#define PROTECT_RUN_APP_HOMEBAR 0
#define PROTECT_RUN_APP_SETTINGS 1
#define PROTECT_RUN_APP_KEYBOARD 2
#define PROTECT_RUN_APP_PROTECT 3

//��ȡITE�����Ƿ�ready
#define EMHOSTID_GET_DRAW_READY 25
// MsgBuf: none

//��ȡITE�����Ƿ�ready
#define EMHOSTID_SET_HANDWRITING_RECT 26
// MsgBuf: EI_RECT

//����FW Partial����
#define EMHOSTID_SET_PARTIAL_ENABLE 27
// MsgBuf: BOOL

//����FW Partial����
#define EMHOSTID_GET_SCENARIO_MODE 28
// MsgBuf: none

//���ü��̰���������
#define EMHOSTID_SET_KEYBOARD_DOWN 29
// MsgBuf: bool

//���ü���̧��������
#define EMHOSTID_SET_KEYBOARD_UP 30
// MsgBuf: bool

//Ӧ������homebar״̬
#define EMHOSTID_SET_HOMEBAR_STATUS 31
// MsgBuf: ULONG

//OOBE����
#define EMHOSTID_OOBE_COMPLETE 32
// MsgBuf: none

//OOBE��ʼ
#define EMHOSTID_OOBE_START 33
// MsgBuf: none

// ��ȡϵͳ������������С
#define EMHOSTID_SET_KEYBOARD_VOLUME 34
// MsgBuf: none

// ����ϵͳ������������С
#define EMHOSTID_GET_KEYBOARD_VOLUME 35
// MsgBuf: LONG (0-100)


// ����FW��д�߿�
#define EMHOSTID_SET_HANDWRITING_LINE_WIDTH 36
// MsgBuf: BYTE

// B/C˫���¼�
#define EMHOSTID_BC_DBOULE_CLICK 37
// MsgBuf: bool true for C cover

// ��ȡ��ǰ������̬
#define EMHOSTID_GET_TABLET_MODE 38
// MsgBuf: none

// homebar֪ͨ�����Լ�״̬�����仯
#define EMHOSTID_HOMEBAR_CHANGE 39
// MsgBuf: ULONG

// ֪ͨ����BCover
#define EMHOSTID_SET_BCOVER_PATH 40
// MsgBuf: wchat_t*

// ֪ͨ����B�������˳�˫��ģʽ
#define EMHOSTID_B_ENTER_DBTAP 41
// MsgBuf: bool true Enter

//����Switcher�Ŀ�������
#define EMHOSTID_SWITCHER_SHOW_AREA 42
// MsgBuf: EI_RECT

//���ü�����ʽ�ļ�
#define EMHOSTID_SET_KEYBOARD_STYLE 43
// MsgBuf: EI_KEYBOARD_STYLE

//���ü���������
#define EMHOSTID_SET_KEYBOARD_SENSITIVITY 44
// MsgBuf: LONG

//B������
#define EMHOSTID_SET_CLOSE_B_COVER 45
// MsgBuf: BOOL

//���ż���������Ϊ������
#define EMHOSTID_PLAY_KEYBOARD_SOUND 46
// MsgBuf: none

//��ϵͳ�л����ʼǱ���ƽ��ģʽ
#define EMHOSTID_CHANGE_TABLET_MODE 47
// MsgBuf: bool //Ϊ���ʾ�л���ƽ�壬�����л����ʼǱ�

//��ʼ��8951
#define EMHOSTID_INIT_8951 48
// MsgBuf: none

//���¼�����Ƶ�豸
#define EMHOSTID_INIT_VIDEO 49
// MsgBuf: none


//Set TP Area
#define EMHOSTID_SET_TP_AREA 50
// MsgBuf: SET_TP_AREA

//Check App Status
#define EMHOSTID_CHECK_APP_STATUS 51
// MsgBuf: none

//����/�ر�knock knock����
#define EMHOSTID_OPEN_KNOCK_KNOCK 52
// MsgBuf: bool //Ϊ���ʾ��

//֪ͨ��ǰӦ�ã��������� tp area
#define EMHOSTID_RESET_TP_AREA 53
// MsgBuf: none

// ������˽Э�鿪��״̬
#define EMHOSTID_SET_PRIVACY_STATUS 54
// MsgBuf: DWORD

// ��ȡ��˽Э�鿪��״̬
#define EMHOSTID_GET_PRIVACY_STATUS 55
// MsgBuf: DWORD

//8951�������������ˣ�����Ҳ����openһ���豸
#define EMHOSTID_REOPEN_8951 56
// MsgBuf: none

//����ָ��ָ������ǰ�û���
#define EMHOSTID_RUN_EXE_FOR_CURRENT_USER 57
// MsgBuf:EI_MSG_RUN_APP

//����C����������������
//�������£��ڱʼǱ�ģʽ��C�淽���B�汣��һ�¡���tent/tableʹ��C���Լ�������
#define EMHOSTID_SET_CCOVER_ORI 58
// MsgBuf:ULONG

// Touch ��HID��ȡת����SeviceCenter
#define EMHOSTID_TOUCH_INPUT 59
// MsgBuf:EI_TOUCHINPUT_POINT

// ѯ���Ƿ���Ҫ��ʾoobe,ͬһ�û�ֻ��ʾһ��
#define EMHOSTID_GET_IS_SHOW_OOBE 60
// MsgBuf:

// ��ȡ�û���ʾ����ID
#define EMHOSTID_GET_USER_LAGID 61

// 8951 connect or remove
#define EMHOSTID_8951_CONNECT_OR_REMOVE 62
// MsgBuf:bool true for connect

// ��ȡ�Լ��Ƿ���ǰ̨����
#define EMHOSTID_GET_IS_FOREGROUND_WINDOW 63

// ѯ��8951�Ƿ���sleep״̬
#define EMHOSTID_GET_8951_IS_SLEEP 64
// MsgBuf:

// ֪ͨhomebar�л�Ӧ��
#define EMHOSTID_CHANGE_APP 65
// MsgBuf: DWORD

// �ر�B���豸���
#define EMHOSTID_CLOSE_B_COVER_HANDLE 66
// MsgBuf: none

// 5182 connect or remove
#define EMHOSTID_5182_CONNECT_OR_REMOVE 67
// MsgBuf:bool true for connect

// ��ȡ��ǰ�û������б�
#define EMHOSTID_GET_USER_DIST_LIST 68
// MsgBuf:none

// MsgBuf:
//////////////////////////////////////////////////////////////////////////
// Sevice���͸�App����Ϣ����
#define EMAPPID_RESULT  1000
// MsgBuf: none

// ������Ϣ
// touch input message
#define EMAPPID_FINGER_MOVE  1001
// MsgBuf: EI_TOUCHINPUT

// ֪ͨAPP�ػ�
#define EMAPPID_RE_DRAW  1002
// MsgBuf: EI_RECT

// ��ԾӦ���л�
#define EMAPPID_ACTIVATE  1003
// MsgBuf: ULONG �����ʾ��Ӧ�ñ���ʾ��ǰ̨��Ϊ���ʾ��Ӧ�ñ�ת����̨
// MsgBuf: ULONG zero indicate this app is show at front , otherwise this app is switch to background .

// ֪ͨAPP��Ļ��ʾ����仯
#define EMAPPID_ORIENTATION_CHANGED 1004
// MsgBuf: ULONG DMDO_DEFAULT/DMDO_90/DMDO_180/DMDO_270

// ֪ͨAPP������̬����仯
#define EMAPPID_LATTOP_CHANGED 1005
// MsgBuf: ULONG GIR_MODE_LAPTOP/GIR_MODE_TENT/GIR_MODE_TABLET

//����֪ͨӦ���¼�
#define EMAPPID_EVENT 1006
// MsgBuf: ULONG 

// ֪ͨAPP homebar״̬�����仯
#define EMAPPID_HOMEBAR_CHANGED 1007
// MsgBuf: ULONG

// ֪ͨAPP �����л����
#define EMAPPID_KEYBOARD_CHANGED 1008
// MsgBuf: BOOL

// ֪ͨAPP ��������TP area
#define EMAPPID_RESET_TP_AREA 1009
// MsgBuf: NA

// ��˽����״̬�����仯
#define EMAPPID_PRIVACY_STATUS_CHANGED 1010
// MsgBuf: NA


#pragma pack(4)

//�ڴ�ӳ���ļ���С
#define EAC_FILE_SIZE 1024*1024*10
//����Ϣ����3M�ռ�
#define EAC_MSG_BUFFER_SIZE 1024*1024*3

//ע��APPʱ��Ҫ���ݵ�����
#define APP_NAME_MAX 100	//�¼�������󳤶�
#define APP_FILE_SIZE_MAX	1024*1024*10 //�ڴ�ӳ���ļ���С
typedef struct _REG_APP_INFO
{
	wchar_t mszAppMutex[APP_NAME_MAX];
	wchar_t mszAppSemaphore[APP_NAME_MAX];
	wchar_t mszAppFilePath[MAX_PATH];
}REG_APP_INFO,*PREG_APP_INFO;

// ���������
class CEiSvrMsgItem {
public:
	union {
		struct {
			ULONG MsgId;
			ULONG AppId;
			HANDLE WaitHandle;
			CEiSvrMsgItem* OrgItem;
			ULONG Result;	// ֻ�е���SendMessageʱ��������
			ULONGLONG TickSent;	// ��Ϣ����ʱ��TickCount
			ULONG BufSize;
			char  MsgBuf[1];
		}Item;
		char All[2048];
	}Data;
	CEiSvrMsgItem() {
		Data.Item.WaitHandle = NULL;
		Data.Item.OrgItem = NULL;
		Data.Item.BufSize = 0;
	}
	~CEiSvrMsgItem() {}

	CEiSvrMsgItem& operator =(const CEiSvrMsgItem& src) {
		RtlCopyMemory(Data.All, src.Data.All, sizeof(Data.Item) + src.Data.Item.BufSize - 1);
		return *this;
	}

	bool IsTypeOf(const CEiSvrMsgItem& nrRefTo) {
		return Data.Item.MsgId == nrRefTo.Data.Item.MsgId;
	}

	void Recall() {
		Data.Item.MsgId = 0;
	}

	void SaveTickCount(void) {
		Data.Item.TickSent = GetTickCount64();
	}

	ULONGLONG GetElapsedTick(void)	{
		return GetTickCount64() - Data.Item.TickSent;
	}

};

#define SVR_MSGITEM_BUF_MAX (2048-sizeof(CEiSvrMsgItem::Data.Item))

#pragma pack()




#pragma once
