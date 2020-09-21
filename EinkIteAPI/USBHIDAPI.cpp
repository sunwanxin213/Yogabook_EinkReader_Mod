/* License: COPYING.GPLv3 */
/* Copyright 2019 - present Lenovo */
#include "stdafx.h"
#include "USBHIDAPI.h"
#include <setupapi.h>  
#include "EinkIteAPI.h"

extern "C" {
#include <hidsdi.h>  
}

USBHIDAPI::USBHIDAPI(SIZE& nrPanel)
{
	mhEventObject = NULL;
	mhReadThread = NULL;
	mhReadHandle = NULL;
	mhEventObject = CreateEvent(NULL, TRUE, TRUE, L"");
	ZeroMemory(&mdHIDOverlapped, sizeof(mdHIDOverlapped));
	mdHIDOverlapped.hEvent = mhEventObject;
	mdHIDOverlapped.Offset = 0;
	mdHIDOverlapped.OffsetHigh = 0;
	mstPanel.cx = nrPanel.cx;
	mstPanel.cy = nrPanel.cy;

	TCHAR szDevPath[MAX_PATH + 1] = { 0 };
	EnumHIDDevice(0x048d, 0x8951, TRUE, szDevPath);
}


USBHIDAPI::~USBHIDAPI()
{
	if (mhEventObject != NULL)
		CloseHandle(mhEventObject);
	if (mhReadHandle != NULL)
		CloseHandle(mhReadHandle);
	if (mhReadThread != NULL)
		CloseHandle(mhReadThread);
}

void SendTouchMessage(PEI_TOUCHINPUT npTouchInput);

//��ȡ�����߳�
DWORD __stdcall USBHIDAPI::ReadFileThread(LPVOID npParam)
{
	USBHIDAPI *lpThis = static_cast<USBHIDAPI *>(npParam);
	PEI_TOUCHINPUT lpTouchInput = (PEI_TOUCHINPUT)HeapAlloc(GetProcessHeap(), NULL, sizeof(EI_TOUCHINPUT) + sizeof(EI_TOUCHINPUT_POINT) * 100);
	if (lpTouchInput == NULL)
		return 0;

	do
	{
		WaitForSingleObject(lpThis->mhEventObject, INFINITE);

		//��������
		//OVERLAPPED ldHIDOverlapped = lpThis->mdHIDOverlapped;
		//DWORD NumberOfBytesRead = 0;
		//GetOverlappedResult(lpThis->mhReadHandle, &lpThis->mdHIDOverlapped, &NumberOfBytesRead, TRUE);
		//OVERLAPPED ldHIDOverlapped
		int liDataLen = 11;
		BYTE* lpBuffer = lpThis->mInputReport;

		lpTouchInput->PointCount = 0;
		lpTouchInput->NewAction = 0;
		int lj = 0;
		for (lj = 0; lj < lpThis->mdHIDOverlapped.InternalHigh / liDataLen; lj++)
		{
			int unfirst = 0;
			int liBufferindex = 0;
			lpBuffer += lj*liDataLen;
			if (lpBuffer[liBufferindex++] == USBHID_TP_REPORT_ID)
			{
				int touch_number = lpBuffer[liBufferindex++] & 0xF;
				for (int i = 0; i < touch_number; i++)
				{
					if (lpBuffer[liBufferindex++] == 0x01)
					{
						//tempstring2 += String.Format("finger");
					}
					//tempstring2 += String.Format("{0:D2}:", lpBuffer[liBufferindex++]);
					if (lpBuffer[liBufferindex] == 0x01)
					{
						//tempstring2 += String.Format("touch_mode\r\n");
					}
					else if (lpBuffer[liBufferindex] == 0x02)
					{
						//tempstring2 += String.Format("leave_mode\r\n");
					}
					liBufferindex++;
					liBufferindex++;

					if (lpBuffer[4] == 0)
					{
						//if (lpTouchInput->PointCount > 0)
						//{	// �����µ�һ�δ�����Ϊ�����ܰ�����ǰһ��������Ϊ����һ��������
						//	SendTouchMessage(lpTouchInput);
						//	lpTouchInput->PointCount = 0;
						//}
						lpTouchInput->NewAction = 1;
					}

					int x, y, z;
					x = lpBuffer[liBufferindex++];
					x += (int)(lpBuffer[liBufferindex++]) << 8;
					//tempstring2 += String.Format("X:{0:D}\r\n", x);
					y = lpBuffer[liBufferindex++];
					y += (int)(lpBuffer[liBufferindex++]) << 8;
					//tempstring2 += String.Format("Y:{0:D}\r\n", y);
					z = lpBuffer[liBufferindex++];
					z += (int)(lpBuffer[liBufferindex++]) << 8;

					lpTouchInput->Point[lpTouchInput->PointCount].x = lpThis->mstPanel.cx - y;	// ת������
					lpTouchInput->Point[lpTouchInput->PointCount].y = x;
					lpTouchInput->Point[lpTouchInput->PointCount].z = z;
					lpTouchInput->Point[lpTouchInput->PointCount].r = 0;
					lpTouchInput->PointCount++;
					wchar_t* lpszOut = new wchar_t[MAX_PATH];
					_swprintf(lpszOut, L"x=%d  y=%d  z=%d\r\n", lpThis->mstPanel.cx - y, x, z);
					OutputDebugString(lpszOut);

					unfirst = lpBuffer[4];
					if (unfirst == 0)
						OutputDebugString(L"Release");

					if (lpTouchInput->PointCount >= 100)
					{	// ���¼�������ֵ
						SendTouchMessage(lpTouchInput);
						lpTouchInput->PointCount = 0;
						lpTouchInput->NewAction = 0;
					}

					//unfirst=0 ��ʾ�õ��Ǳ�д�����,����ʱ��=1
					//unfirst = lpBuffer[4];
					//OutputDebugString(L"end");

				}
			}
		}

		// ��������㣬�������һ��		
		if(lj > 0)
			SendTouchMessage(lpTouchInput);

		//�ٴο�����ȡ
		lpThis->BeginRead();

	} while (true);
}

VOID WINAPI CompletedReadRoutine(DWORD dwErr, DWORD cbBytesRead,LPOVERLAPPED lpOverLap)
{
	OutputDebugString(L"succ");
}



//��ʼ��ȡ����
void USBHIDAPI::BeginRead()
{
	DWORD NumberOfBytesRead = 0;
	//����ڴ�
	ZeroMemory(mInputReport, USBHID_BUFFER_SIZE);

	BOOL lResult = ReadFile(mhReadHandle, mInputReport, USBHID_BUFFER_SIZE,&NumberOfBytesRead, &mdHIDOverlapped);
}

BOOL USBHIDAPI::EnumHIDDevice(WORD nuVID, WORD nuPID, //USB VID PID
	BOOL nbPresentFlag, //�豸������ڱ�־ 0����Ҫ�����豸
	TCHAR nszDevPath[MAX_PATH + 1], //�豸·��
	int niIndex) //��N���豸 ���Զ����ͬ���豸�������֣�
{
	BOOL lbRet = FALSE;

	int liFoundCount = 0; //���ҵ�ƥ���豸����

	GUID lHidGuid;
	HidD_GetHidGuid(&lHidGuid);

	DWORD ldwFlage = DIGCF_DEVICEINTERFACE;
	if (nbPresentFlag)
		ldwFlage |= DIGCF_PRESENT;
	HDEVINFO hdev = SetupDiGetClassDevs(&lHidGuid,
		NULL,
		NULL,
		ldwFlage);

	if (INVALID_HANDLE_VALUE != hdev)
	{
		int idev = 0;
		while (!lbRet)
		{
			SP_DEVICE_INTERFACE_DATA did = { 0 };
			did.cbSize = sizeof(did);

			if (SetupDiEnumDeviceInterfaces(hdev,
				0,
				&lHidGuid,
				idev,
				&did))
			{
				DWORD cbRequired = 0;

				SetupDiGetDeviceInterfaceDetail(hdev,
					&did,
					0,
					0,
					&cbRequired,
					0);
				if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
				{
					PSP_DEVICE_INTERFACE_DETAIL_DATA pdidd =
						(PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, cbRequired);
					if (pdidd)
					{
						pdidd->cbSize = sizeof(*pdidd);
						if (SetupDiGetDeviceInterfaceDetail(hdev,
							&did,
							pdidd,
							cbRequired,
							&cbRequired,
							0))
						{
							//TRACE(_T("\n%s\n"), pdidd->DevicePath);
							// Enumerated a battery.  Ask it for information.
							HANDLE hDevHandle =
								CreateFile(pdidd->DevicePath,
									GENERIC_READ | GENERIC_WRITE,
									FILE_SHARE_READ | FILE_SHARE_WRITE,
									NULL,
									OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL,
									NULL);
							if (INVALID_HANDLE_VALUE != hDevHandle)
							{
								// Ask the battery for its tag.
								HIDD_ATTRIBUTES hidAttributes = { 0 };
								hidAttributes.Size = sizeof(hidAttributes);

								if (HidD_GetAttributes(hDevHandle, &hidAttributes))
								{
									if (hidAttributes.VendorID == nuVID
										&& hidAttributes.ProductID == nuPID)
									{
										WCHAR szManufacturer[MAX_PATH + 1] = { 0 };
										WCHAR szProduct[MAX_PATH + 1] = { 0 };
										WCHAR szSerialNumber[MAX_PATH + 1] = { 0 };
										HidD_GetManufacturerString(hDevHandle, szManufacturer, MAX_PATH);
										HidD_GetProductString(hDevHandle, szProduct, MAX_PATH);
										HidD_GetSerialNumberString(hDevHandle, szSerialNumber, MAX_PATH);

										//TRACE(_T("Manufacturer string=%s\n"), (LPCTSTR)CString(szManufacturer));
										//TRACE(_T("Product string=%s\n"), (LPCTSTR)CString(szProduct));
										//TRACE(_T("SerialNumber=%s\n"), (LPCTSTR)CString(szSerialNumber));

										if (liFoundCount == niIndex)
										{
											wcscpy_s(mszDevPath, MAX_PATH, pdidd->DevicePath);

											//���豸
											mhReadHandle = CreateFile(
												mszDevPath,
												GENERIC_READ,
												FILE_SHARE_READ | FILE_SHARE_WRITE,
												NULL,
												OPEN_EXISTING,
												FILE_FLAG_OVERLAPPED,
												NULL);

											//BeginRead();
											//���������߳�
											mhReadThread = CreateThread(NULL, 0, ReadFileThread, (LPVOID)this, 0, NULL);
										
											lbRet = TRUE;
										}
										liFoundCount++;

									}
								}
								CloseHandle(hDevHandle);
							}
							else
							{
								DWORD dwErr = GetLastError();
								//TRACE(_T("CreateFile failed with code %lu\n"), dwErr);
							}
						}
						else
						{
							DWORD dwErr = GetLastError();
							//TRACE(_T("SetupDiGetDeviceInterfaceDetail failed with code %lu\n"), dwErr);
						}
						LocalFree(pdidd);
					}
				}
			}
			else  if (ERROR_NO_MORE_ITEMS == GetLastError())
			{
				break;  // Enumeration failed - perhaps there are no more items
			}

			idev++; //for next device
		}
		SetupDiDestroyDeviceInfoList(hdev);
	}

	return lbRet;
}