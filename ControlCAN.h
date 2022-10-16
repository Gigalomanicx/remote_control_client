#ifndef CONTROLCAN_H
#define CONTROLCAN_H

//???????????
#define VCI_PCI5121		1
#define VCI_PCI9810		2
#define VCI_USBCAN1		3
#define VCI_USBCAN2		4
#define VCI_USBCAN2A	4
#define VCI_PCI9820		5
#define VCI_CAN232		6
#define VCI_PCI5110		7
#define VCI_CANLITE		8
#define VCI_ISA9620		9
#define VCI_ISA5420		10
#define VCI_PC104CAN	11
#define VCI_CANETUDP	12
#define VCI_CANETE		12
#define VCI_DNP9810		13
#define VCI_PCI9840		14
#define VCI_PC104CAN2	15
#define VCI_PCI9820I	16
#define VCI_CANETTCP	17
#define VCI_PEC9920		18
#define VCI_PCIE_9220	18
#define VCI_PCI5010U	19
#define VCI_USBCAN_E_U	20
#define VCI_USBCAN_2E_U 21
#define VCI_PCI5020U	22
#define VCI_EG20T_CAN	23
#define VCI_PCIE9221	24
#define VCI_WIFICAN_TCP  25
#define VCI_WIFICAN_UDP 26
#define VCI_PCIe9120 27
#define VCI_PCIe9110 28
#define VCI_PCIe9140 29


//CAN??????
#define	ERR_CAN_OVERFLOW			0x0001	//CAN?????????FIFO????
#define	ERR_CAN_ERRALARM			0x0002	//CAN????????????
#define	ERR_CAN_PASSIVE				0x0004	//CAN??????????????
#define	ERR_CAN_LOSE				0x0008	//CAN???????????
#define	ERR_CAN_BUSERR				0x0010	//CAN?????????????
#define ERR_CAN_BUSOFF				0x0020 //?????????
//????????
#define	ERR_DEVICEOPENED			0x0100	//?õô???????
#define	ERR_DEVICEOPEN				0x0200	//?????õô????
#define	ERR_DEVICENOTOPEN			0x0400	//?õô??§Õ???
#define	ERR_BUFFEROVERFLOW			0x0800	//??????????
#define	ERR_DEVICENOTEXIST			0x1000	//???õô??????
#define	ERR_LOADKERNELDLL			0x2000	//??????????
#define ERR_CMDFAILED				0x4000	//???????????????
#define	ERR_BUFFERCREATE			0x8000	//??ÖÎ??


//???????¡Â??????
#define	STATUS_OK					1
#define STATUS_ERR					0

#define CMD_DESIP			0
#define CMD_DESPORT			1
#define CMD_CHGDESIPANDPORT		2
#define CMD_SRCPORT			2
#define CMD_TCP_TYPE		4					//tcp ???????????????:1 ????????:0
#define TCP_CLIENT			0
#define TCP_SERVER			1
//?????????????§¹
#define CMD_CLIENT_COUNT    5					//??????????????
#define CMD_CLIENT			6					//???????????
#define CMD_DISCONN_CLINET  7					//??????????
#define CMD_SET_RECONNECT_TIME 8			//??????????

#include "windef.h"

typedef struct tagRemoteClient{
    int iIndex;
    DWORD port;
    HANDLE hClient;
    char szip[32];
}REMOTE_CLIENT;


typedef struct _tagChgDesIPAndPort
{
    char szpwd[10];
    char szdesip[20];
    int desport;
    BYTE blistenonly;
}CHGDESIPANDPORT;

//1.ZLGCAN??§ß?????????????????
typedef  struct  _VCI_BOARD_INFO{
    USHORT	hw_Version;
    USHORT	fw_Version;
    USHORT	dr_Version;
    USHORT	in_Version;
    USHORT	irq_Num;
    BYTE	can_Num;
    CHAR	str_Serial_Num[20];
    CHAR	str_hw_Type[40];
    USHORT	Reserved[4];
} VCI_BOARD_INFO,*PVCI_BOARD_INFO;

//2.????CAN???????????????
typedef  struct  _VCI_CAN_OBJ{
    UINT	ID;
    UINT	TimeStamp;
    BYTE	TimeFlag;
    BYTE	SendType;
    BYTE	RemoteFlag;//?????????
    BYTE	ExternFlag;//?????????
    BYTE	DataLen;
    BYTE	Data[8];
    BYTE	Reserved[3];
}VCI_CAN_OBJ,*PVCI_CAN_OBJ;

//3.????CAN???????????????????
typedef struct _VCI_CAN_STATUS{
    UCHAR	ErrInterrupt;
    UCHAR	regMode;
    UCHAR	regStatus;
    UCHAR	regALCapture;
    UCHAR	regECCapture;
    UCHAR	regEWLimit;
    UCHAR	regRECounter;
    UCHAR	regTECounter;
    DWORD	Reserved;
}VCI_CAN_STATUS,*PVCI_CAN_STATUS;

//4.??????????????????????
typedef struct _VCI_ERR_INFO{
    UINT	ErrCode;
    BYTE	Passive_ErrData[3];
    BYTE	ArLost_ErrData;
} VCI_ERR_INFO,*PVCI_ERR_INFO;

//5.?????????CAN??????????
typedef struct _VCI_INIT_CONFIG{
    DWORD	AccCode;
    DWORD	AccMask;
    DWORD	Reserved;
    UCHAR	Filter;
    UCHAR	Timing0;
    UCHAR	Timing1;
    UCHAR	Mode;
}VCI_INIT_CONFIG,*PVCI_INIT_CONFIG;


///////// new add struct for filter /////////
typedef struct _VCI_FILTER_RECORD{
    DWORD ExtFrame;	//????????
    DWORD Start;
    DWORD End;
}VCI_FILTER_RECORD,*PVCI_FILTER_RECORD;

//?????????????
typedef struct _VCI_AUTO_SEND_OBJ{
    BYTE Enable;//??????????.  0??????   1?????
    BYTE Index;  //???????.   ???????32??????
    DWORD Interval;//??????????1ms???¦Ë
    VCI_CAN_OBJ obj;//????
}VCI_AUTO_SEND_OBJ,*PVCI_AUTO_SEND_OBJ;

#define EXTERNC		extern "C"

EXTERNC DWORD __stdcall VCI_OpenDevice(DWORD DeviceType,DWORD DeviceInd,DWORD Reserved);
EXTERNC DWORD __stdcall VCI_CloseDevice(DWORD DeviceType,DWORD DeviceInd);
EXTERNC DWORD __stdcall VCI_InitCAN(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_INIT_CONFIG pInitConfig);

EXTERNC DWORD __stdcall VCI_ReadBoardInfo(DWORD DeviceType,DWORD DeviceInd,PVCI_BOARD_INFO pInfo);
EXTERNC DWORD __stdcall VCI_ReadErrInfo(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_ERR_INFO pErrInfo);
EXTERNC DWORD __stdcall VCI_ReadCANStatus(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_STATUS pCANStatus);

EXTERNC DWORD __stdcall VCI_GetReference(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,DWORD RefType,PVOID pData);
EXTERNC DWORD __stdcall VCI_SetReference(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,DWORD RefType,PVOID pData);

EXTERNC ULONG __stdcall VCI_GetReceiveNum(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);
EXTERNC DWORD __stdcall VCI_ClearBuffer(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);

EXTERNC DWORD __stdcall VCI_StartCAN(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);
EXTERNC DWORD __stdcall VCI_ResetCAN(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);

EXTERNC ULONG __stdcall VCI_Transmit(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_OBJ pSend,ULONG Len);
EXTERNC ULONG __stdcall VCI_Receive(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_OBJ pReceive,ULONG Len,INT WaitTime=-1);


#endif
