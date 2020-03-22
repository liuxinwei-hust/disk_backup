//#include "winioctl.h"
#include "stdafx.h"
//备份信息
typedef struct _BACKUP_MISSION
{
//备份源信息
	int                    SourceDisk;      //备份源物理磁盘号
	char                   strSourceDrive;  //备份源盘符号
	int                    partID;          //第几分区

	BYTE		           SystemFlag;			   // flag of file system of partition eg:0x07分区文件系统
	UINT64		           StartLogicSecNo;	       // 分区起始扇区
	UINT64		           SectorNum;			   // 分区总扇区
	UINT64		           PartitionSec;		   // 扩展分区起始扇区
	BYTE	               InExtend;			   // 标志此分区是否在扩展分区
    DWORD	               SectorSize	;		   // 每个扇区大小
	BYTE	               SectorBuffer[514];	   // store MBR
	ULARGE_INTEGER         SourceTotal;            //总空间
	ULARGE_INTEGER         SourceLeft;             //剩余空间

	BYTE                   AcvtiveFlag;           //备份源为活动分区
	BYTE                   SysFlag;               //备份源为系统分区
	char                   ActPartSymbol;         //  活动分区盘符
	UINT64		           ActStartLogicSecNo;	  //  活动分区起始扇区
	UINT64		           ActSectorNum;	      //  活动分区总扇区
//备份目的地
	CString                strDestDrive;         //备份目的地
	CString                strHdzFileName;       //备份文件名
	CString                strPassword;          //密码
	int                    SplitType;            //分割级别
	UINT64                 Split;                //每个备份文件的大小
	BOOL                   bpasswd;              //是否有密码
	BOOL                   bCompress;            //压缩
	BOOL                   bCrc;	             //CRC校验
	BOOL                   bEndCheck;            //结束后检查
	BOOL                   bEndShutdown;         //结束后自动关机
	ULARGE_INTEGER         DestTotal;            //总空间
	ULARGE_INTEGER         DestLeft;             //剩余空间

}Backup_Mission,*PBackup_Mission;

//还原信息
typedef struct _RESTORE_MISSION
{
//文件信息
	CString FilePath;//备份文件路径
//分区信息
	int                    SourceDisk;      //物理磁盘号
	char                   strSourceDrive;  //盘符号
	int                    partID;          //第几分区

	BYTE		           SystemFlag;			   // flag of file system of partition eg:0x07分区文件系统
	UINT64		           StartLogicSecNo;	       // start LBA of partition 分区起始扇区
	UINT64		           SectorNum;			   // total sectors of partition分区总扇区
	UINT64		           PartitionSec;		   // start LBA of extend partition扩展分区起始扇区
	BYTE	               InExtend;			   // flag which indicates the partition is in extend partition标志此分区是否在扩展分区
    DWORD	               SectorSize	;		   // 每个扇区大小
	BYTE	               SectorBuffer[514];	   // store MBR
	ULARGE_INTEGER         SourceTotal;            //总空间
	ULARGE_INTEGER         SourceLeft;             //剩余空间

	BYTE                   AcvtiveFlag;           //备份源为活动分区
	BYTE                   SysFlag;               //备份源为系统分区
	char                   ActPartSymbol;         //  活动分区盘符
	UINT64		           ActStartLogicSecNo;	  //  活动分区起始扇区
	UINT64		           ActSectorNum;	      //  活动分区总扇区
//功能信息
	BOOL                   bEndShutdown;         //结束后自动关机
	BOOL                   SetActive;            //设为活动分区,系统可引导
	BOOL                   RestCheck;               //还原前检查 
	BOOL                   SetWindSys;            //设为系统分区
}Restore_Mission;
//扩展头
typedef struct
{
    DWORD ResSecSize;					// reserved bytes which store MBR	eg:2*1024*1024 用来存储MBR
    BYTE  exist63sec;					// flag which indicates whether MBR has been backup eg:0x01 标志MBR是否存储
    DWORD Sec63Offset;					// offset of system's MBR in xx.hdz eg:0x00005800 系统MBR在文件中存储的位置
   
    DWORD mbrSectors;					// sectors of MBR eg:0x0000003F/0x00000800 MBR的扇区
    BYTE    bIncludeSystemReserved;     // 1 byte  是否存储隐藏扇区
    UINT64  SysResTotalSectorNum;       // 8 bytes
    UINT64  SysResStartPos;				// 8 bytes
}HeaderExtend,*pHeaderExtend;
//第一个文件的头
typedef struct
{
    char	szComment[50];	            // comment for the data file(50 bytes)备注
    char	szFileName[256];	        // full path of backup file (256 bytes)备份文件的完整路径

    UINT64	ToatalSize;				    // 原始驱动器的总扇区
	UINT64  PartToSec;
    UINT64	TotalUsedSectors;			// 已使用扇区
    UINT64	SplitSize;					// 分割大小
    WORD	wDataFileCount;				// 备份文件的数量
	DWORD	SectorSize	;		       // 每个扇区大小

	UINT64  packetnum;                  // 包的数量
	UINT64  LasPackStar;                //最后一个包的起始扇区
	WORD    LastSector;                 //最后一个包的扇区数

    BYTE FileSystem;			        // 文件系统

    DWORD   DiskID;						// 磁盘id
    UINT64	readErrNum;					// 读取错误次数

    DWORD	dwIndexLength;				// 簇的数量
    DWORD	dwReserveSectors;			// 保留扇区数
    BYTE	SectorsPerPacket;			// 每簇的扇区数量

	BYTE    bcompress;                   //是否压缩
	BYTE    bpassword;                   //存在密码
    BYTE    passwd[8];	                 // password(8 bytes) 密码
    BYTE	BootFromHDZ;				// 分区可引导
	BYTE    CRCInfo;					// crc校验是否存在
    BYTE    bHidePart;					// 是否是隐藏分区
    BYTE    bExternPart;				// 是否是扩展分区
    BYTE    bActivePart;				// 是否是活动分区
    UINT64  PartStartPos;				// 备份起始地址

	UINT64 dig_sign;
}	Header;								// 879bytes
//数据包
typedef struct
{
    UINT64	StartSector;				// start sector of data数据的起始扇区
    WORD	Sectors;					// amount of sectors which is packed 打包扇区数量
    WORD	CompressType;				// type of compress 压缩方式
    DWORD	DataSize;					// size of data which has been packed 被打包的数据大小
    DWORD	CRCValue;					// CRC of data before compresing 压缩前的数据的CRC
}	DataPacketHead;
//文件尾
typedef struct
{
	UINT64  packetnum;
    WORD	SequenceID;					// sequence of current backup file (eg:0100-->xx.001)当前备份文件的顺序
    UINT64	LastFileLength;				// size of last hdz file(bytes)最后一个hdz文件的大小
}	HDBTag;