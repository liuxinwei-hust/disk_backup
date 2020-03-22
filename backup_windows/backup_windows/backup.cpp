#include "stdafx.h"
#include "BackupProgress.h"
#include "backupinfo.h"
#include "winioctl.h"
#include <winternl.h>

#define RESSECSIZE (2*1024*1024)
#define BITMAPBYTES						4096
#define BITMAPSIZE						(BITMAPBYTES+2*sizeof(ULONGLONG))
#define MAX_READWRITE_SECTOR	8
#define MAX_BIMAPBUF_SIZE		(64*1024ULL)
#define MAX_COPY_SIZE		4096  //sector num
#define WM_MYUPDATE (WM_USER + 101)

extern BOOL ReadSectors(BYTE Disk, UINT64 StartLBA , UINT64 ReadSectorNum,BYTE *Buffer, DWORD dwSectorSize );
extern Backup_Mission BackupPartinfo;
extern BOOL g_bCancel;
extern unsigned Compress(UCHAR *OutBuf, UCHAR *InBuf, UCHAR *WorkBuf, UINT InLen);

static UINT64	CurBackupStartClusterNo;	    //calculate cluster when change disk
static UINT64	CurBackupSectorsNum;			//calculate sector when change disk
static int packet_size;	// real size of packethead
int g_iSequenceID=0;
FILE *pfile=NULL;

int hdzFileCount=1;
UINT64 hdzPacketCount=0;
BYTE pCompress_Buf1[136312];
BYTE pCompress_Buf2[136312];
unsigned char pCompress_DiffBuf[120*1024];
static unsigned long crc32_table[256];
CString hdzfilename;

HANDLE g_hMyDriver;

//��ȡλͼ����ʱ��һЩ�ṹ��

typedef struct {
	ULONGLONG			StartLcn;
	ULONGLONG			ClustersToEndOfVol;
	BYTE				Map[1];
} BITMAP_DESCRIPTOR, *PBITMAP_DESCRIPTOR; 





typedef NTSTATUS (__stdcall *PNtFsControlFile)( 
	HANDLE FileHandle,
	HANDLE Event,					// optional
	PIO_APC_ROUTINE ApcRoutine,		// optional
	PVOID ApcContext,				// optional
	PIO_STATUS_BLOCK IoStatusBlock,	
	ULONG FsControlCode,
	PVOID InputBuffer,				// optional
	ULONG InputBufferLength,
	PVOID OutputBuffer,				// optional
	ULONG OutputBufferLength
	);

PNtFsControlFile   NtFsControlFile;





/************************************************************
Function:		GetMBRSectors
Description:	get amount of sectors engrossed by MBR
                eg:63/2048
Input:			diskNum	// symbol of disk eg:0x80
Output:
Return:
**************************************************************/
DWORD GetMBRSectors(BYTE diskNum)
{
    BYTE buf[514];
    DWORD Pos[4]={0};
    int ValidPosNum=0;
    DWORD ValidPos[4]={0};
    DWORD minPos;
	memcpy(buf,BackupPartinfo.SectorBuffer,514);
    Pos[0]=*(DWORD*)(buf+454);//�Ĵ��������ķ�����ʼ���������
    Pos[1]=*(DWORD*)(buf+454+16);
    Pos[2]=*(DWORD*)(buf+454+16*2);
    Pos[3]=*(DWORD*)(buf+454+16*3);
    for(int i=0;i<4;i++)
    {
        if(Pos[i] != 0)
        {
            ValidPos[ValidPosNum] = Pos[i];
            ValidPosNum++;
        }
    }
    minPos=ValidPos[0];
    for(int j=0;j<ValidPosNum;j++)
    {
        minPos=min(minPos,ValidPos[j]);
    }
    if(minPos<=2048)
    {
        return minPos;
    }
    BYTE XPFlag[] = {0x33,0xC0,0x8E,0xD0,0xBC,0x0,0x7C,0xFB};
    BYTE VistaFlag[] = {0x33,0xC0,0x8E,0xD0,0xBC,0x0,0x7C,0x8E};
    if(memcmp(buf,XPFlag,8)==0)
    {
        return 63;
    }
    if(memcmp(buf,VistaFlag,8)==0)
    {
        return 2048;
    }
    BYTE flagbufV6[] = {0xEB , 0x3C, 0x90};
    BYTE flagbufV5[] = {0xFA,0x33,0xC0,0x8E,0xD0,0xBC,0x00,0x7C,0x8B,0xF4,0x50,0x07,0x50,0x1F,0xFB,0xB8};
    if((memcmp(buf+3,"ARKUSB  ",8)==0)
        ||(memcmp(buf+3,"ARKDVD  ",8)==0)
        ||(memcmp(buf+3,"ARKHDZ  ",8)==0)
        ||(memcmp(buf, flagbufV6, 3) == 0)
        ||(memcmp(buf, flagbufV5, 16) == 0))
    {
        ReadSectors(diskNum,62,1,buf,512);
        if(memcmp(buf,XPFlag,8)==0)
        {
            return 63;
        }
        if(memcmp(buf,VistaFlag,8)==0)
        {
            return 2048;
        }
    }
    return 2048;
}


/***************************************
*���ܣ��õ�λͼ����
* ���룺����id
* ���أ�λͼ
****************************************/





UINT64 getBitmap(char driver,BYTE *pBitmap)
{
	DWORD						status;
	PBITMAP_DESCRIPTOR			bitMappings;
	ULONGLONG					cluster;	
	ULONGLONG					nextLcn;
	IO_STATUS_BLOCK				ioStatus;	
	ULONG                       BitmapSize;

	BYTE*		BitMap = NULL;
	BYTE		*buffer;	
	if( !(NtFsControlFile = (PNtFsControlFile) GetProcAddress( GetModuleHandleA("ntdll.dll"),
		"NtFsControlFile" )) ) 
	{
		return 0;
	}
	BitMap = new BYTE[BITMAPSIZE];
	if(BitMap == NULL)
	{
		return 0;
	}
	bitMappings = (PBITMAP_DESCRIPTOR) BitMap;
	cluster = 0;
	nextLcn = 0;
	status = NtFsControlFile( g_hMyDriver, NULL, NULL, 0, &ioStatus,
		FSCTL_GET_VOLUME_BITMAP,
		&nextLcn, sizeof( cluster ),
		bitMappings, BITMAPSIZE );

	BitmapSize = (ULONG)(bitMappings->ClustersToEndOfVol/8+20);
	if(BitMap)
	{
		delete[]BitMap;
	}
	buffer = new BYTE[BitmapSize]; 

	bitMappings = (PBITMAP_DESCRIPTOR) buffer;
	cluster = 0;
	nextLcn = 0;
	status = NtFsControlFile( g_hMyDriver, NULL, NULL, 0, &ioStatus,
		FSCTL_GET_VOLUME_BITMAP,
		&nextLcn, sizeof( cluster ),
		bitMappings, BitmapSize );
	if(status != 0)
	{
		delete buffer;
		return 0;
	}

	// 2004.10.04 stesirog modify
	memcpy(pBitmap,buffer+16,BitmapSize-16);
	BitmapSize = (ULONG)(bitMappings->ClustersToEndOfVol/8+20);

	delete buffer;

	// 2003.06.23 stesirog modify
	return BitmapSize;
}

/***************************************
*���ܣ��õ�λͼ����
* ���룺����id
* ���أ�λͼ
****************************************/
UINT64  getBipmapLength(char driver)
{
	DWORD						status;
	PBITMAP_DESCRIPTOR			bitMappings;
	ULONGLONG					cluster;		
	ULONGLONG					nextLcn;	
	IO_STATUS_BLOCK				ioStatus;	
	BYTE* BitMap = NULL;
	if( !(NtFsControlFile = (PNtFsControlFile) GetProcAddress( GetModuleHandleA("ntdll.dll"),
		"NtFsControlFile" )) ) 
	{ 
		return 0;
	}
	cluster = 0;
	nextLcn = 0; 
	BitMap = new BYTE[BITMAPSIZE];
	if(BitMap == NULL)
	{
		return 0;
	}
	bitMappings = (PBITMAP_DESCRIPTOR) BitMap;

	status = NtFsControlFile( g_hMyDriver, NULL, NULL, 0, &ioStatus,
		FSCTL_GET_VOLUME_BITMAP,
		&nextLcn, sizeof( cluster ),
		bitMappings, BITMAPSIZE );

	UINT64 ullRet = (bitMappings->ClustersToEndOfVol/8+20);
	delete[]BitMap;

	return ullRet;
}





/************************************************************
Function:		�й�crc�ĺ���
Description:    ����һ��hdz�ļ�
Input:
Output:
Return:			ture/false
**************************************************************/

ULONG Reflect(ULONG ref, char ch)
{
	ULONG value=0;

	for(int i = 1; i < (ch + 1); i++)
	{
		if(ref & 1)
			value |= 1 << (ch - i);
		ref >>= 1;
	}
	return value;
}

void Init_CRC32_Table()
{
	ULONG ulPolynomial = 0x04c11db7;

	for(int i = 0; i <= 0xFF; i++)
	{
		crc32_table[i]=Reflect(i, 8) << 24;
		for (int j = 0; j < 8; j++)
			crc32_table[i] = (crc32_table[i] << 1) ^ (crc32_table[i] & (1 << 31) ? ulPolynomial : 0);
		crc32_table[i] = Reflect(crc32_table[i], 32);
	}
}


int Get_CRC(BYTE*  buffer,DWORD dwSize)
{
	ULONG  crc;
	int len;

	crc=0xffffffff;	
	len = dwSize;
	while(len--)
		crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ *buffer++];

	return crc^0xffffffff;
}
/************************************************************
Function:		OpenNextHdz
Description:    ����һ��hdz�ļ�
Input:
Output:
Return:			ture/false
**************************************************************/
bool OpenNextHdz()
{
	HDBTag  tag;
	tag.packetnum=hdzPacketCount;
	tag.LastFileLength=_ftelli64(pfile);
	tag.SequenceID=hdzFileCount-1;
	int ret = fwrite((void *)&tag,sizeof(tag),1,pfile);
	if(!ret)
	{
		return false;
	}
	if(fclose(pfile)!=0)//�رճɹ�����0
	{
		return false;
	}
	CString pathtemp;
	pathtemp.Format("%s%s_%d.hdz",BackupPartinfo.strDestDrive,BackupPartinfo.strHdzFileName,hdzFileCount);
	hdzfilename=pathtemp;
	char destpath[MAX_PATH];
	sprintf_s(destpath,"%s",pathtemp);
	if(fopen_s(&pfile,destpath,"wb"))
	{
		MessageBox(NULL,"���ļ�ʧ��","��ʾ",MB_OK);
		return false;
	}
	if(pfile==NULL)
	{
		return false;
	}
	hdzPacketCount=0;
	hdzFileCount++;
    return true;
}

/************************************************************
Function:		WritePacket
Description:	write a packet to hdz file
Input:			packet
                *buf
                *pfile
Output:
Return:			0/1/2
**************************************************************/
int WritePacket(DataPacketHead packet,unsigned char * buf)
{
    int ret=0;
    CString format,msg;

	if( BackupPartinfo.Split!=0  &&
		_ftelli64(pfile) + packet.DataSize +sizeof(packet)> BackupPartinfo.Split )      //tccdel over one hdz
    {
        if(OpenNextHdz()==false)
        {
            printf("writePacket  opennexthdz err\n");
            return 0;
        }

    }



	if(BackupPartinfo.bCrc)
    {
        packet.CRCValue=Get_CRC(buf,packet.DataSize);
        packet_size=sizeof(packet);
    }
    else
    {
        packet_size=sizeof(packet)-sizeof(packet.CRCValue);
    }


	if(!BackupPartinfo.bCompress)
    {
        packet.CompressType = 0;
        fwrite((void *)&packet,packet_size,1,pfile);
        ret = fwrite(buf,packet.DataSize,1,pfile);
        if(!ret) printf("Fwrite err 13\n");
    }
    else
    {
        //int result=1;
        UINT len = packet.DataSize;

        memset(pCompress_DiffBuf,0,120*1024);
        len = Compress(pCompress_Buf1,buf,pCompress_Buf2,len);

        if(len >= packet.DataSize)
        {
            packet.CompressType = 0;
            fwrite((void *)&packet,packet_size,1,pfile);
            ret = fwrite(buf,packet.DataSize,1,pfile);
            if(!ret) printf("Fwrite err 12\n");
        }
        else
        {
            packet.DataSize = len;
            packet.CompressType = 1;
            fwrite((void *)&packet,packet_size,1,pfile);
            ret = fwrite(pCompress_Buf1,packet.DataSize,1,pfile);
            if(!ret) printf("Fwrite err 11\n");
        }

    }
    if(ret) return 1;
    return 0;
}








/***************************************
*���ܣ���������������
* ���룺�������Ի���ָ��
* ���أ�0��ʾʧ�ܣ�1��ʾ�ɹ���2��ʾȡ��
****************************************/
int backup(CBackupProgress *progressdlg)
{
	unsigned char *bitmap = NULL;
	BYTE *buf;//128*512
	buf=(BYTE*)malloc(4096*512);
	BYTE *BitMapBuf;
	BitMapBuf=(BYTE*)malloc(64*1024);
    unsigned   char *Resvbuf;
    UINT64 BitMapLen;
    UINT64 i,j;
	UINT64 AllPacketNum=0;
    int BackupSectorNum;
    int SectorsPerCluster;
    Header   header;
    pHeaderExtend pHeaderExt;
    DataPacketHead  packet;
    UINT64 TotalSectors=0;
    UINT64 TotalUsedSectors=0;
    UINT64 CurBackupSectors=0;
    int WriteReturn = 0;
    DWORD dwStartTime = 0;
	BOOL backupReserved=FALSE;
    int nRedraw = 0;

	int g_ullErrNum = 0;
    int filesystem = 0x70;



    BYTE  sDiskNO = 0x80;
    DWORD dDiskNO = 1;

    UUID  driverGUID = GUID_NULL;
    bool  bExistGUID = false;
    bool  bIsSysDrive = false;
    CString strInfo = "";
	pHeaderExt = (HeaderExtend *)malloc(sizeof(HeaderExtend));
    memset(pHeaderExt,0,sizeof(HeaderExtend));
    dDiskNO += 0x80;

//BEGIN:
		

	hdzfilename.Format("%s%s.hdz",BackupPartinfo.strDestDrive,BackupPartinfo.strHdzFileName);

    memset((void *)&header,0,sizeof(header));
//���ݿ�ʼʱ��
	CString backupStarttime;
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	backupStarttime.Format("%4d-%02d-%02d %02d:%02d:%02d.%03d",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds);
	sprintf_s(header.szComment,"LXW:%s",backupStarttime);
    //sprintf_s(header.szComment,"LXW");

    pHeaderExt->exist63sec=1;//��־MBR�洢
    pHeaderExt->Sec63Offset=18432+4096;//ϵͳMBR��hdz�ļ��е�λ��
    pHeaderExt->ResSecSize=2*1024*1024;//�����洢MBR�Ŀռ�
//�������ط���
    if(backupReserved)
    {
        pHeaderExt->bIncludeSystemReserved = 1;
		pHeaderExt->SysResTotalSectorNum = BackupPartinfo.ActSectorNum;//������������
		pHeaderExt->SysResStartPos = BackupPartinfo.ActStartLogicSecNo;//������ʼ��ַ
        pHeaderExt->ResSecSize=(ULONG)(RESSECSIZE+BackupPartinfo.ActSectorNum*512);
    }


	//��������
	if (BackupPartinfo.bpasswd)                                     //tccdel   has password
    {
		header.bpassword=1;
		char StrPassWord[9];
		sprintf_s(StrPassWord,"%s",BackupPartinfo.strPassword);
		for(i=0;i<BackupPartinfo.strPassword.GetLength();i++)
        {
            StrPassWord[i]+=(char)0x80;
        }
        memset(header.passwd,0,8);
        memcpy(header.passwd,StrPassWord,BackupPartinfo.strPassword.GetLength());
    }
	else
	{
		header.bpassword=0;
	}

    Resvbuf = (unsigned char *)malloc(RESSECSIZE);
    memset(Resvbuf,0,RESSECSIZE);
    DWORD MBRSectors=GetMBRSectors((BYTE)BackupPartinfo.SourceDisk+0x80);
	ReadSectors(BackupPartinfo.SourceDisk+0x80,0,MBRSectors,Resvbuf,512);//��ȡMBR
    printf("MBRSectors = %ul\n",MBRSectors);
    pHeaderExt->mbrSectors=MBRSectors;



	memcpy(header.szFileName,BackupPartinfo.strDestDrive,BackupPartinfo.strDestDrive.GetLength());//����Ŀ�ĵ�
    header.wDataFileCount = 1 ;//���ݳ�hdz������
	header.SplitSize =  BackupPartinfo.Split;//ÿ��hdz�ļ��Ĵ�С
	header.CRCInfo  =BackupPartinfo.bCrc;//�Ƿ�У��
	header.DiskID  = BackupPartinfo.SourceDisk;//����id
	header.BootFromHDZ =BackupPartinfo.SysFlag;
	header.SectorSize=BackupPartinfo.SectorSize;
	header.bcompress=BackupPartinfo.bCompress;
	header.dig_sign=0x55AA55AA55AA55AA;//����ǩ��
	if(BackupPartinfo.bCrc)//��ʼ��CRCУ���
	{
		Init_CRC32_Table();
	}


	header.FileSystem=BackupPartinfo.SystemFlag;
/*	if(BackupPartinfo.strSourceDrive=='0')
	{
		header.bHidePart=TRUE;
		CString partname;
		partname.Format("\\Device\\HarddiskVolume%d",BackupPartinfo.partID);
		int bRet = DefineDosDevice ( 
			DDD_RAW_TARGET_PATH, "]:", partname);//�����ɾ������̷���
		BackupPartinfo.strSourceDrive=']';
	}
	else
	{
		header.bHidePart = FALSE;//�������ط���
	} */
	header.bExternPart = BackupPartinfo.InExtend;
	header.bActivePart = BackupPartinfo.AcvtiveFlag;
	header.PartStartPos=BackupPartinfo.StartLogicSecNo;//������ʼ��ַ
    header.PartToSec=BackupPartinfo.SectorNum;
    //lij modify.2004.08.25���������ļ�hdz
	CString pathtemp;
	pathtemp=BackupPartinfo.strDestDrive+BackupPartinfo.strHdzFileName+".hdz";
	char destpath[MAX_PATH];
	sprintf_s(destpath,"%s",pathtemp);
	if(fopen_s(&pfile,destpath,"wb"))
	{
		MessageBox(NULL,"���ļ�ʧ��","��ʾ",MB_OK);
		goto ERR;
	}
	if(pfile==NULL)
	{
		return 0;
	}

    //for ntfs
    if(filesystem == 0x70)
    {
		//��ȡ�������
		CString DiskSymbol=_T("");
		if(BackupPartinfo.strSourceDrive=='0')
	    {
		       header.bHidePart=TRUE;
		       DiskSymbol.Format("\\\\.\\HarddiskVolume%d",BackupPartinfo.partID);
	    }
	    else
	    {
	  	      header.bHidePart = FALSE;//�������ط���
			  DiskSymbol.Format(_T("\\\\.\\%c:"),BackupPartinfo.strSourceDrive);
	    }		
		g_hMyDriver = CreateFile(DiskSymbol, 
		GENERIC_READ | GENERIC_WRITE,       
		FILE_SHARE_READ | FILE_SHARE_WRITE,  
		0,OPEN_EXISTING,0,0); 
	    if (g_hMyDriver == INVALID_HANDLE_VALUE)
	    {
		   return 0;
	    }
        memset((void *)buf,0,1024);
		ReadSectors(BackupPartinfo.SourceDisk+0x80,BackupPartinfo.StartLogicSecNo,1,buf,512);                   //tccmodify  source partition��ȡ��ntfs��BPB��������
        TotalSectors =  (*(UINT64*)(buf + 0x28));//��������
        SectorsPerCluster = (*(BYTE*)(buf + 0xd));//ÿ�ص�������

		BitMapLen=getBipmapLength(BackupPartinfo.SourceDisk);
		bitmap=(unsigned char*)malloc((int)BitMapLen);
		memset(bitmap,0,(UINT)BitMapLen);
		getBitmap(BackupPartinfo.SourceDisk,bitmap);//��ȡλͼ����




        header.dwReserveSectors = 0;
        if(SectorsPerCluster > MAX_READWRITE_SECTOR)//û��Ҫntfsÿ�ش�С���Ϊ4kBһ��һ����
        {
            header.SectorsPerPacket = MAX_READWRITE_SECTOR;
        }
        else
        {
            header.SectorsPerPacket = SectorsPerCluster;//ȷ��ÿ������������
        }
		//ȷ����������
            if(TotalSectors%(header.SectorsPerPacket) == 0)
            {
                header.dwIndexLength = (DWORD)(TotalSectors/(header.SectorsPerPacket));
            }
            else
            {
                header.dwIndexLength = (DWORD)(TotalSectors/(header.SectorsPerPacket)+1);
            }


//ͳ��ʹ�õĴ�
        TotalUsedSectors = 0;
        for(i=0;;)
        {
            if(i>=BitMapLen*8)
            {
                break;
            }
            if(i*SectorsPerCluster>TotalSectors-SectorsPerCluster)
            {
                bitmap[i/8] &= ~(((BYTE)1)<<(i%8));
            }
            if((bitmap[i/8]&(((unsigned char)1)<<(i%8))))
            {
                TotalUsedSectors ++;
            }
            i++;
        }
        TotalUsedSectors*=SectorsPerCluster;
        header.TotalUsedSectors = TotalUsedSectors;//ʹ��������
        header.ToatalSize = TotalSectors;//��������
		

		fwrite((void *)&header,sizeof(header),1,pfile);//��һ����hdzд��header
        ///////////////////////////////////
        memset((void *)buf,0,1024-sizeof(header));
		fwrite(buf,1024-sizeof(header),1,pfile);//д0���ļ�����1KB
 		ReadSectors(BackupPartinfo.SourceDisk+0x80,BackupPartinfo.StartLogicSecNo,1,buf,512); //����������
		fwrite(buf,512,1,pfile);//��������BPB����
        TotalSectors =  (*(UINT64*)(buf + 0x28));
        SectorsPerCluster = (*(BYTE*)(buf + 0xd));//ÿ��������
        memset((void *)buf,0,512);
		fwrite(buf,512,1,pfile);//д512�ֽ�0����1kb,��2kb
        //reserved
        memset((void *)buf,0,8192);
		fwrite(buf,8192,1,pfile);//8kb0��10kb
		fwrite(buf,8192,1,pfile);//8kb0��18kb
		fwrite((void *)pHeaderExt,sizeof(HeaderExtend),1,pfile);//����HeaderExtend
        memset((void *)buf,0,4096-sizeof(HeaderExtend));
		fwrite(buf,4096-sizeof(HeaderExtend),1,pfile);//����4kb,��22kb
		fwrite(Resvbuf,RESSECSIZE,1,pfile);//����MBR��СΪ2M


        //--->add 2009.10.15
        if(backupReserved)                     //tcc getexist reserved  partition�����Ӻ�����ȫ����ϵͳ��,�ؼ���LogicDriverList[0]������ʲô
        {

 
        }

        CurBackupSectors  = 0;
        if( BitMapLen < MAX_BIMAPBUF_SIZE )//����λͼ��Ϣ,���λͼ���ȳ���MAX_BIMAPBUF_SIZE64KB,��ֻ��64KB
        {
            memcpy( BitMapBuf, bitmap, (size_t)BitMapLen );
        }
        else
        {
            memcpy(BitMapBuf,bitmap,MAX_BIMAPBUF_SIZE);
        }

        j = 0;

		time_t start_time,end_time;
		double timediff1=0.0;
		time(&start_time);
        for(i=0;;)
        {
            if((j*MAX_BIMAPBUF_SIZE*8+i)>=BitMapLen*8)
            {
                break;
            }
            if((BitMapBuf[i/8]&(((BYTE)1)<<(i%8))))//ȡ����i/8�ֽڵĵ�i%8λ,����Ϊһ��֤��λͼ��λ�ö�Ӧ���߼���(4KB)��ռ��,�ʱ���֮
            {
                UINT64  dwStartSector= (j*MAX_BIMAPBUF_SIZE*8+i)*SectorsPerCluster;//����λͼ��ȡ��ʼ����
                BackupSectorNum= 0;
                while((BitMapBuf[i/8]&(((BYTE)1)<<(i%8))))//���λͼĳһ�ε�ֵȫ��һ���ǿ��԰���ͳ�Ƴ���Ȼ��һ���Զ�ȡ����
                {
                    BackupSectorNum+=SectorsPerCluster;
                    i++;
                    if(i==MAX_BIMAPBUF_SIZE*8)
                        break;
                    if(BackupSectorNum>=MAX_COPY_SIZE)
                        break;
                }

				if(!ReadSectors(BackupPartinfo.SourceDisk+0x80,BackupPartinfo.StartLogicSecNo+dwStartSector,BackupSectorNum,buf,512))
                 {

                     {
                            printf("read sectors err1!!!\n");
                            goto ERR;
                        }
                        memset(buf,0,512);
                        g_ullErrNum++;
                 }

                for(int k=0;k<BackupSectorNum;)//����ȡ��BackupSectorNum���������ݴ��,һ��һ����,����İ�һ�ش��
                {
                    int num=min(MAX_READWRITE_SECTOR,SectorsPerCluster);//���Ϊ4kb,8������

                    if(num>BackupSectorNum-k)
                    {
                        num = BackupSectorNum-k;
                    }

                    packet.StartSector   = dwStartSector+k;
                    packet.Sectors       = num;//BackupSectorNum;
                    packet.DataSize      = 512*num;
                    CurBackupStartClusterNo = j*MAX_BIMAPBUF_SIZE*8+i;
                    CurBackupSectorsNum =     CurBackupSectors;
					WriteReturn = WritePacket(packet,buf+k*512);
					if(g_bCancel == TRUE)
                    {
                        goto CANCEL;
                    }

                    if(WriteReturn == 0)
                    {
                        goto ERR;
                    }
                    else if(WriteReturn == 2)
                    {
                        goto CANCEL;
                    }
					hdzPacketCount++;
					AllPacketNum++;
                    CurBackupSectors += num;
					
					time(&end_time);
					double timdiff=difftime(end_time,start_time);
					if(timdiff-timediff1>=1)//ÿһ�����һ������
					{
                        int percentage;
                        percentage = (int)(CurBackupSectors * 100 / TotalUsedSectors);
                        progressdlg->m_percentage.Format("%d%%", percentage);
                        progressdlg->m_progress.SetPos(percentage);
                        progressdlg->m_backupdest.Format("%s", hdzfilename);
						timediff1=timdiff;
						timdiff=timdiff*(TotalUsedSectors-CurBackupSectors)/CurBackupSectors;				    
						int leftmin=(int)timdiff/60;
					    int lefthour=leftmin/60;
					    leftmin=leftmin % 60;
					    int sec= (int)timdiff % 60;
					    progressdlg->m_lefttime.Format("%dСʱ%d��%d��",lefthour,leftmin,sec);
                        progressdlg->SendMessage(WM_MYUPDATE, 0);
					}
					
                    k+=num;
              //Ԥ��ʱ��



                }
            }
            else
            {
                i++;
            }

            if(i==MAX_BIMAPBUF_SIZE*8)//������һ��64KB��λͼ��BitMapBuf,����64KB����ʣ�µ�
            {
                i=0;
                j++;
                if((BitMapLen-j*MAX_BIMAPBUF_SIZE)>=MAX_BIMAPBUF_SIZE)
                    memcpy(BitMapBuf,bitmap+(j*MAX_BIMAPBUF_SIZE),MAX_BIMAPBUF_SIZE);
                else
                {
                    memset(BitMapBuf,0,MAX_BIMAPBUF_SIZE);
                    memcpy(BitMapBuf,bitmap+(j*MAX_BIMAPBUF_SIZE),(size_t)(BitMapLen-j*MAX_BIMAPBUF_SIZE));
                }

            }


        }


    }
 

/*    // 2003.10.24 stesirog modify
    packet.StartSector   = (UINT64)0xffffffffffffffff;
    packet.Sectors       = 0;
    packet.CompressType  = 0;
    packet.DataSize      = (DWORD)g_ullErrNum;

	if(fwrite((void *)&packet,packet_size,1,pfile)==false)//д��ͳ�ƵĴ����
    {
        goto ERR;
    }

  */
	HDBTag  tag;
	tag.packetnum=hdzPacketCount;
	tag.LastFileLength=_ftelli64(pfile);
	tag.SequenceID=hdzFileCount-1;
	fwrite((void *)&tag,sizeof(tag),1,pfile);

	header.LasPackStar=packet.StartSector;
	header.LastSector=packet.Sectors;
    header.ToatalSize = TotalSectors;
	header.wDataFileCount = hdzFileCount;
    header.TotalUsedSectors = TotalUsedSectors;
	header.packetnum=AllPacketNum;
    header.readErrNum = g_ullErrNum;

/*	fwrite((void *)&header,sizeof(header),1,pfile);//д��header

    memset((void *)buf,0,1024-sizeof(header));
	fwrite(buf,1024-sizeof(header),1,pfile);//����1kb
	*/
	if(fclose(pfile)!=0)
    {
            goto ERR;
     }
	pfile=NULL;
	//���µ�һ���ļ���header
	FILE *pfile2;
	if(fopen_s(&pfile2,destpath,"rb+"))
	{
		MessageBox(NULL,"���ļ�ʧ��","��ʾ",MB_OK);
		goto ERR;
	}
	if(pfile2==NULL)
    {

            goto ERR;

    }
	 fseek(pfile2,0,SEEK_SET);
	 fwrite((void *)&header,sizeof(header),1,pfile2);
	 if(fclose(pfile2)!=0)
    {
        goto ERR;
    }
	 pfile2=NULL;



	 if(buf!=NULL)
	 {
	    free(buf);
        buf = NULL;
	 }
	 if(BitMapBuf !=NULL)
	 {
	    free(BitMapBuf);
        BitMapBuf = NULL;
	 }
    if (pHeaderExt != NULL)
    {
        free(pHeaderExt);
        pHeaderExt = NULL;
    }

    if( Resvbuf != NULL )
    {

        free(Resvbuf);
        Resvbuf = NULL;
    }

    if( bitmap != NULL )
    {

        free(bitmap);
        bitmap=NULL;
    }
    return 1;

ERR:
	fclose(pfile);
    if(buf!=NULL)
	 {
	    free(buf);
        buf = NULL;
	 }
	 if(BitMapBuf !=NULL)
	 {
	    free(BitMapBuf);
        BitMapBuf = NULL;
	 }
    if (pHeaderExt != NULL)
    {
        free(pHeaderExt);
        pHeaderExt = NULL;
    }

    if( Resvbuf != NULL )
    {

        free(Resvbuf);
        Resvbuf = NULL;
    }

    if( bitmap != NULL )
    {

        free(bitmap);
        bitmap=NULL;
    }

    return 0;

CANCEL:


	fclose(pfile);

	for(i=0;i<hdzFileCount;i++)
    {
        if(i==0)
        {
            _unlink(destpath);
        }
        else
        {
			CString DelFileName;
			DelFileName=BackupPartinfo.strDestDrive+BackupPartinfo.strHdzFileName;
            char destpath2[MAX_PATH];
			sprintf_s(destpath2,"%s_%d.hdz",DelFileName,i);
            _unlink(destpath2);
        }
    }

    if (pHeaderExt != NULL)
    {
        free((BYTE*)pHeaderExt);
        pHeaderExt = NULL;
    }

    if( Resvbuf != NULL )
    {
        free( Resvbuf );
        Resvbuf = NULL;
    }

    if(bitmap)
    {
        free(bitmap);
        bitmap=NULL;
    }
	 if(buf!=NULL)
	 {
	    free(buf);
        buf = NULL;
	 }
	 if(BitMapBuf !=NULL)
	 {
	    free(BitMapBuf);
        BitMapBuf = NULL;
	 }
    return 2;
}