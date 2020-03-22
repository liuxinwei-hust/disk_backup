//#include "winioctl.h"
#include "stdafx.h"
//������Ϣ
typedef struct _BACKUP_MISSION
{
//����Դ��Ϣ
	int                    SourceDisk;      //����Դ������̺�
	char                   strSourceDrive;  //����Դ�̷���
	int                    partID;          //�ڼ�����

	BYTE		           SystemFlag;			   // flag of file system of partition eg:0x07�����ļ�ϵͳ
	UINT64		           StartLogicSecNo;	       // ������ʼ����
	UINT64		           SectorNum;			   // ����������
	UINT64		           PartitionSec;		   // ��չ������ʼ����
	BYTE	               InExtend;			   // ��־�˷����Ƿ�����չ����
    DWORD	               SectorSize	;		   // ÿ��������С
	BYTE	               SectorBuffer[514];	   // store MBR
	ULARGE_INTEGER         SourceTotal;            //�ܿռ�
	ULARGE_INTEGER         SourceLeft;             //ʣ��ռ�

	BYTE                   AcvtiveFlag;           //����ԴΪ�����
	BYTE                   SysFlag;               //����ԴΪϵͳ����
	char                   ActPartSymbol;         //  ������̷�
	UINT64		           ActStartLogicSecNo;	  //  �������ʼ����
	UINT64		           ActSectorNum;	      //  �����������
//����Ŀ�ĵ�
	CString                strDestDrive;         //����Ŀ�ĵ�
	CString                strHdzFileName;       //�����ļ���
	CString                strPassword;          //����
	int                    SplitType;            //�ָ��
	UINT64                 Split;                //ÿ�������ļ��Ĵ�С
	BOOL                   bpasswd;              //�Ƿ�������
	BOOL                   bCompress;            //ѹ��
	BOOL                   bCrc;	             //CRCУ��
	BOOL                   bEndCheck;            //��������
	BOOL                   bEndShutdown;         //�������Զ��ػ�
	ULARGE_INTEGER         DestTotal;            //�ܿռ�
	ULARGE_INTEGER         DestLeft;             //ʣ��ռ�

}Backup_Mission,*PBackup_Mission;

//��ԭ��Ϣ
typedef struct _RESTORE_MISSION
{
//�ļ���Ϣ
	CString FilePath;//�����ļ�·��
//������Ϣ
	int                    SourceDisk;      //������̺�
	char                   strSourceDrive;  //�̷���
	int                    partID;          //�ڼ�����

	BYTE		           SystemFlag;			   // flag of file system of partition eg:0x07�����ļ�ϵͳ
	UINT64		           StartLogicSecNo;	       // start LBA of partition ������ʼ����
	UINT64		           SectorNum;			   // total sectors of partition����������
	UINT64		           PartitionSec;		   // start LBA of extend partition��չ������ʼ����
	BYTE	               InExtend;			   // flag which indicates the partition is in extend partition��־�˷����Ƿ�����չ����
    DWORD	               SectorSize	;		   // ÿ��������С
	BYTE	               SectorBuffer[514];	   // store MBR
	ULARGE_INTEGER         SourceTotal;            //�ܿռ�
	ULARGE_INTEGER         SourceLeft;             //ʣ��ռ�

	BYTE                   AcvtiveFlag;           //����ԴΪ�����
	BYTE                   SysFlag;               //����ԴΪϵͳ����
	char                   ActPartSymbol;         //  ������̷�
	UINT64		           ActStartLogicSecNo;	  //  �������ʼ����
	UINT64		           ActSectorNum;	      //  �����������
//������Ϣ
	BOOL                   bEndShutdown;         //�������Զ��ػ�
	BOOL                   SetActive;            //��Ϊ�����,ϵͳ������
	BOOL                   RestCheck;               //��ԭǰ��� 
	BOOL                   SetWindSys;            //��Ϊϵͳ����
}Restore_Mission;
//��չͷ
typedef struct
{
    DWORD ResSecSize;					// reserved bytes which store MBR	eg:2*1024*1024 �����洢MBR
    BYTE  exist63sec;					// flag which indicates whether MBR has been backup eg:0x01 ��־MBR�Ƿ�洢
    DWORD Sec63Offset;					// offset of system's MBR in xx.hdz eg:0x00005800 ϵͳMBR���ļ��д洢��λ��
   
    DWORD mbrSectors;					// sectors of MBR eg:0x0000003F/0x00000800 MBR������
    BYTE    bIncludeSystemReserved;     // 1 byte  �Ƿ�洢��������
    UINT64  SysResTotalSectorNum;       // 8 bytes
    UINT64  SysResStartPos;				// 8 bytes
}HeaderExtend,*pHeaderExtend;
//��һ���ļ���ͷ
typedef struct
{
    char	szComment[50];	            // comment for the data file(50 bytes)��ע
    char	szFileName[256];	        // full path of backup file (256 bytes)�����ļ�������·��

    UINT64	ToatalSize;				    // ԭʼ��������������
	UINT64  PartToSec;
    UINT64	TotalUsedSectors;			// ��ʹ������
    UINT64	SplitSize;					// �ָ��С
    WORD	wDataFileCount;				// �����ļ�������
	DWORD	SectorSize	;		       // ÿ��������С

	UINT64  packetnum;                  // ��������
	UINT64  LasPackStar;                //���һ��������ʼ����
	WORD    LastSector;                 //���һ������������

    BYTE FileSystem;			        // �ļ�ϵͳ

    DWORD   DiskID;						// ����id
    UINT64	readErrNum;					// ��ȡ�������

    DWORD	dwIndexLength;				// �ص�����
    DWORD	dwReserveSectors;			// ����������
    BYTE	SectorsPerPacket;			// ÿ�ص���������

	BYTE    bcompress;                   //�Ƿ�ѹ��
	BYTE    bpassword;                   //��������
    BYTE    passwd[8];	                 // password(8 bytes) ����
    BYTE	BootFromHDZ;				// ����������
	BYTE    CRCInfo;					// crcУ���Ƿ����
    BYTE    bHidePart;					// �Ƿ������ط���
    BYTE    bExternPart;				// �Ƿ�����չ����
    BYTE    bActivePart;				// �Ƿ��ǻ����
    UINT64  PartStartPos;				// ������ʼ��ַ

	UINT64 dig_sign;
}	Header;								// 879bytes
//���ݰ�
typedef struct
{
    UINT64	StartSector;				// start sector of data���ݵ���ʼ����
    WORD	Sectors;					// amount of sectors which is packed �����������
    WORD	CompressType;				// type of compress ѹ����ʽ
    DWORD	DataSize;					// size of data which has been packed ����������ݴ�С
    DWORD	CRCValue;					// CRC of data before compresing ѹ��ǰ�����ݵ�CRC
}	DataPacketHead;
//�ļ�β
typedef struct
{
	UINT64  packetnum;
    WORD	SequenceID;					// sequence of current backup file (eg:0100-->xx.001)��ǰ�����ļ���˳��
    UINT64	LastFileLength;				// size of last hdz file(bytes)���һ��hdz�ļ��Ĵ�С
}	HDBTag;