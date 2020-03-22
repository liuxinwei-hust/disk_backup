#include "winioctl.h"

#define PhysicalDriverMAX 16
#define LogicDriverMax 64
#define PartTablePOS 0x1be

typedef struct _LogicDRIVE 
{

	char        LogicDriveSymbol;    // C; D; ...�̷�
	BYTE		ActiveFlag;			 // active flag eg:0x80��������Ƿǻ����
	BYTE		SystemFlag;			 // flag of file system of partition eg:0x07�����ļ�ϵͳ
	UINT64		StartLogicSecNo;	 // start LBA of partition ������ʼ����
	UINT64		SectorNum;			 // total sectors of partition����������
	UINT64		PartitionSec;		// start LBA of extend partition��չ������ʼ����
	BYTE	    InExtend;			// flag which indicates the partition is in extend partition��־�˷����Ƿ�����չ����
}LogicDRIVE;

typedef struct
{
	BYTE	bChanged	;
	WORD	ErrorCode	;		// flag indicates whether disk has error	
	int	Flags		;		// symbol of disk eg:0x80
	MEDIA_TYPE mediatype;        //�豸����
	DWORD	Cylinders	;		// ������
	DWORD	Heads		;		// ��ͷ��
	DWORD	SecPerTrack	;		// ÿ���ŵ�������
	UINT64	Sectors	;			// ��������    
	DWORD	SectorSize	;		// ÿ��������С
	BYTE	SectorBuffer[514];	// store MBR
	//BYTE	bReadOnly	;
	//BYTE	bShipBadCheck;
	//BYTE	bModified	;
	//WORD	bScsi		;		//deal with scsi hd disk driver name
	LogicDRIVE partition[LogicDriverMax];//�����ϵ����з���
	int partitionnum;             //�����ϵķ�������
}  Int13ParaEx;