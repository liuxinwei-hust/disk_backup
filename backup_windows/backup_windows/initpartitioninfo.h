#include "winioctl.h"

#define PhysicalDriverMAX 16
#define LogicDriverMax 64
#define PartTablePOS 0x1be

typedef struct _LogicDRIVE 
{

	char        LogicDriveSymbol;    // C; D; ...盘符
	BYTE		ActiveFlag;			 // active flag eg:0x80活动分区还是非活动分区
	BYTE		SystemFlag;			 // flag of file system of partition eg:0x07分区文件系统
	UINT64		StartLogicSecNo;	 // start LBA of partition 分区起始扇区
	UINT64		SectorNum;			 // total sectors of partition分区总扇区
	UINT64		PartitionSec;		// start LBA of extend partition扩展分区起始扇区
	BYTE	    InExtend;			// flag which indicates the partition is in extend partition标志此分区是否在扩展分区
}LogicDRIVE;

typedef struct
{
	BYTE	bChanged	;
	WORD	ErrorCode	;		// flag indicates whether disk has error	
	int	Flags		;		// symbol of disk eg:0x80
	MEDIA_TYPE mediatype;        //设备类型
	DWORD	Cylinders	;		// 柱面数
	DWORD	Heads		;		// 磁头数
	DWORD	SecPerTrack	;		// 每条磁道扇区数
	UINT64	Sectors	;			// 扇区总数    
	DWORD	SectorSize	;		// 每个扇区大小
	BYTE	SectorBuffer[514];	// store MBR
	//BYTE	bReadOnly	;
	//BYTE	bShipBadCheck;
	//BYTE	bModified	;
	//WORD	bScsi		;		//deal with scsi hd disk driver name
	LogicDRIVE partition[LogicDriverMax];//此盘上的所有分区
	int partitionnum;             //此盘上的分区数量
}  Int13ParaEx;