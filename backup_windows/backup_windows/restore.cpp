#include "stdafx.h"
#include "RestoreProgress.h"
#include "backupinfo.h"
#include "winioctl.h"

extern BOOL WriteSectors(HANDLE hDevice, UINT64 StartLBA, UINT64 WriteSectorNum,BYTE *Buffer);
extern Restore_Mission RestorePart;
extern Header head;
extern BOOL g_bCancel;
extern int Expand(unsigned char *OutBuf, unsigned char *InBuf, unsigned short int *Offset);
#define WRITE_BUFFER_LEN 4096
#define PartTablePOS 0x1be
#define WM_RESTUPDATE (WM_USER + 102)





/************************************************************
Function:		ReadSectors       
Description:    read secors according to LBA in disk根据地址读取磁盘
Input:			Disk			// symbol of disk eg:0x80哪个盘
				StartLBA		// LBA in disk开始地址
				ReadSectorNum	// amount of sectors reading读取数目
Output:			*Buffer			// buffer storing data读到的内容	
Return:         true/false	
**************************************************************/
BOOL NTReadSectors(HANDLE hDevice, UINT64 StartLBA , UINT64 ReadSectorNum,BYTE *Buffer, DWORD dwSectorSize )
{
	
	BOOL bResult;                 // results flag
	DWORD dwBytesRead,dwPos;
	LARGE_INTEGER HiddenSectors;
	HiddenSectors.QuadPart= StartLBA * dwSectorSize;//LowPart ,HighPart 

	dwPos = SetFilePointer(hDevice, HiddenSectors.LowPart,&HiddenSectors.HighPart, FILE_BEGIN); 
	bResult = ReadFile(hDevice, Buffer, (unsigned long)(ReadSectorNum*dwSectorSize) , &dwBytesRead , NULL);

	if (!bResult) // if IOCTL failed...
	{
		//  AfxMessageBoxEx(IDS_READ_DISK_FAILED);
		return (FALSE);
	}


	return (bResult);
}
/************************************************************
Function:       NTLockDriver锁定卷
Description:    lock driver by handle	
Input:          hDevice // handle of driver
Output:			
Return:			ture	// success
				false	// invalid handle
						// IOCTL failed
**************************************************************/
BOOL NTLockDriver(HANDLE hDevice)
{
	DWORD junk; 

	if( hDevice == INVALID_HANDLE_VALUE || hDevice == NULL )
	{
		return (FALSE);
	}

	BOOL bResult = DeviceIoControl(hDevice,  // the device we are querying
		FSCTL_DISMOUNT_VOLUME ,  // operation to perform
		NULL, 0, // no input buffer, so we pass zero
		NULL, 0, // no input buffer, so we pass zero
		&junk, // discard the count of bytes returned
		(LPOVERLAPPED)NULL);  // synchronous I/O

	if (!bResult)                // if IOCTL failed...
	{
		return (FALSE);
	}

	return (TRUE) ;
}

/************************************************************
Function:       NTUnlockDriver解锁卷
Description:    unlock driver by handle	
Input:          hDevice // handle of driver
Output:			
Return:			ture	// success
				false	// invalid handle
						// IOCTL failed
**************************************************************/
BOOL NTUnlockDriver(HANDLE hDevice)
{
	DWORD junk; 

	if( hDevice == INVALID_HANDLE_VALUE || hDevice == NULL )
	{
		return (FALSE);
	}

	BOOL bResult = DeviceIoControl(hDevice,  // the device we are querying
		FSCTL_UNLOCK_VOLUME ,  // operation to perform
		NULL, 0, // no input buffer, so we pass zero
		NULL, 0, // no input buffer, so we pass zero
		&junk, // discard the count of bytes returned
		(LPOVERLAPPED)NULL);  // synchronous I/O

	if (!bResult)                // if IOCTL failed...
	{
		return (FALSE);
	}

	return (TRUE) ;
}


BOOL ReadPacket(DataPacketHead packet,unsigned char *buf, FILE * pfile)
{
        if(packet.CompressType==0)
        {
                if(fread(buf,packet.DataSize,1,pfile)==0)
                {
                        return FALSE;
                }
                return TRUE;
        }
        else if(packet.CompressType == 1)
        {
			    BYTE pCompress_Buf1[136312];
				BYTE pCompress_Buf2[136312];
                if(fread(pCompress_Buf1,packet.DataSize,1,pfile))
                {
                        Expand(buf,pCompress_Buf1,(WORD *)pCompress_Buf2);
                        return TRUE;
                }
                else
                {
                        return FALSE;
                }
        }

        return FALSE;
}


int restore(CRestoreProgress *progressdlg)
{
	UINT64 totalsector;
	UINT64 cursector=0;//当前还原扇区
	int filecount;
	CString filepath;
	unsigned   char *Resvbuf;
	Resvbuf = (unsigned char *)malloc(514);
	memset(Resvbuf,0,514);

	UINT64 BufFirstSec=0;
	UINT64 BufCurPos=0;
	BYTE *pWriteBuf;
	pWriteBuf=(BYTE *)malloc(512*WRITE_BUFFER_LEN);//写扇区缓冲区
	memset(pWriteBuf,0,512*WRITE_BUFFER_LEN);

	filecount=head.wDataFileCount;    //备份文件数量
	totalsector=head.TotalUsedSectors;//需还原的扇区总数

	CString DiskSymbol=_T("");
	if(head.bHidePart)
	{
		DiskSymbol.Format("\\\\.\\HarddiskVolume%d",RestorePart.partID);
	}
	else
	{
		DiskSymbol.Format(_T("\\\\.\\%c:"),RestorePart.strSourceDrive);
	}	
	HANDLE	hMyDriver = CreateFile(DiskSymbol, 
		GENERIC_READ | GENERIC_WRITE,       
		FILE_SHARE_READ | FILE_SHARE_WRITE,  
		0,OPEN_EXISTING,0,0);
	NTLockDriver(hMyDriver);



	DiskSymbol.Format(_T("\\\\.\\PhysicalDrive%d"),RestorePart.SourceDisk);

	HANDLE	hDevice = CreateFile(DiskSymbol, 
		GENERIC_READ | GENERIC_WRITE,       
		FILE_SHARE_READ | FILE_SHARE_WRITE,  
		0,OPEN_EXISTING,0,0); 
	if (hDevice == INVALID_HANDLE_VALUE)         // if we can't open the drive...
		return (FALSE);


//	CloseHandle(hMyDriver);
//起始时间
	time_t start_time,end_time;
	double timediff1=0.0;
	time(&start_time);
//获取备份文件路径
	filepath=RestorePart.FilePath;
    int k=filepath.GetLength()-4;
    filepath.GetBuffer()[k]='\0';

//包大小
	DataPacketHead  packet;
	int packetsize;
	if(head.CRCInfo)
	{
		packetsize=sizeof(packet);
	}
	else
	{
		packetsize=sizeof(packet)-sizeof(packet.CRCValue);
	}
//修改MBR
	 memcpy(Resvbuf,RestorePart.SectorBuffer,514);
		for(int j=0;j<4;j++)
		{
			int offset;
			BYTE FileSystemFlag;
			BYTE ActiveFlag;
			UINT64 ExtendFirstStartSec=0;
			UINT64 ExtendStartSec=0;
			offset=16*j;
			FileSystemFlag=Resvbuf[offset+PartTablePOS+4];
			ActiveFlag=Resvbuf[offset+PartTablePOS];
			if(FileSystemFlag==0)
			{
				continue;
			}
			if(*(DWORD*)(Resvbuf+offset+PartTablePOS+8)==0)
			{
				continue;
			}
			if(*(DWORD*)(Resvbuf+offset+PartTablePOS+12)<=63)
			{
				continue;
			}
			if(FileSystemFlag!=0x05 && FileSystemFlag!=0x0f)//不是扩展分区
			{

				if(RestorePart.StartLogicSecNo==*(DWORD*)(Resvbuf+PartTablePOS+offset+8))
				{
					*(DWORD*)(Resvbuf+PartTablePOS+offset+12)=(DWORD)head.PartToSec;
					if(RestorePart.SetWindSys)
					{
						*(DWORD*)(Resvbuf+PartTablePOS+offset+8)=(DWORD)head.PartStartPos;
						RestorePart.StartLogicSecNo=head.PartStartPos;
					}
					if(RestorePart.SetActive)
					{
						*(BYTE*)(Resvbuf+PartTablePOS+offset)=0x80;
					}
					if(WriteSectors(hDevice,0,1,Resvbuf)==FALSE)
			        {
				         MessageBox(NULL,"写扇区失败","提示",MB_OK);
				         goto err;
			        }
					break;
				}

			}
			else
			{
				ExtendFirstStartSec=*(DWORD*)(Resvbuf+PartTablePOS+offset+8);
				ExtendStartSec=ExtendFirstStartSec;
				while(ExtendStartSec!=0)
				{
					DWORD sectorsize=RestorePart.SectorSize;
	                BYTE *mbrtemp=new BYTE[sectorsize];
	                WORD driver;
					driver=0x80+RestorePart.SourceDisk;
	                memset(mbrtemp,0,sectorsize);
					if(!NTReadSectors(hDevice,ExtendStartSec,1,mbrtemp,sectorsize))
	                {
		               MessageBox(NULL,"读取MBR出错","提示",MB_OK);
					   delete mbrtemp;
					   return 0;
                   	}
					if(RestorePart.StartLogicSecNo==(*(DWORD*)(mbrtemp+PartTablePOS+8)+ExtendStartSec))
					{
						*(DWORD*)(mbrtemp+PartTablePOS+12)=(DWORD)head.PartToSec;
					    if(RestorePart.SetWindSys)
					    {
						   *(DWORD*)(mbrtemp+PartTablePOS+8)=(DWORD)head.PartStartPos;
						   RestorePart.StartLogicSecNo=head.PartStartPos;
					    }
					    if(WriteSectors(hDevice,ExtendStartSec,1,mbrtemp)==FALSE)
			            {
				         MessageBox(NULL,"写扇区失败","提示",MB_OK);
						 delete mbrtemp;
				         goto err;
			            }
					}
					

					ExtendStartSec=*(DWORD*)(mbrtemp+PartTablePOS+16+8)+ExtendFirstStartSec;
					if(mbrtemp[PartTablePOS+16+4]==0)
			        {
				         ExtendStartSec=0;
						 delete mbrtemp;
						 break;
			        }
			        if(*(DWORD*)(mbrtemp+PartTablePOS+16+8)==0)
			        {
				         ExtendStartSec=0;
						 delete mbrtemp;
						 break;
			        }
			        if(*(DWORD*)(mbrtemp+PartTablePOS+16+12)<=63)
			        {
				         ExtendStartSec=0;
						 delete mbrtemp;
						 break;
			        }
					delete mbrtemp;
				}
			}
		}
	free(Resvbuf);
//还原开始
	for(int i=0;i<filecount;i++)
	{
		 FILE *pfile;
         HDBTag  tag;//文件末尾信息
		 CString temp;
		if(i==0)
		{
			
			long offset;
			temp.Format("%s.hdz",filepath);
			if(fopen_s(&pfile,temp,"rb+"))
			{
				MessageBox(NULL,"打开文件失败","提示",MB_OK);
				goto err;
			}
			if(filecount==1)
			{
				tag.packetnum=head.packetnum;
			}
			else
			{
			   offset=sizeof(tag);
			   offset=0-offset;
			   _fseeki64(pfile,offset,SEEK_END);
			   fread(&tag,sizeof(tag),1,pfile);
			}
			offset=2*1024*1024+22*1024;
			_fseeki64(pfile,offset,SEEK_SET);
		}
		else
		{
			
			long offset;
			temp.Format("%s_%d.hdz",filepath,i);
			if(fopen_s(&pfile,temp,"rb+"))
			{
				MessageBox(NULL,"打开文件失败","提示",MB_OK);
				goto err;
			}
			offset=sizeof(tag);
			offset=0-offset;
			_fseeki64(pfile,offset,SEEK_END);
			fread(&tag,sizeof(tag),1,pfile);
			_fseeki64(pfile,0,SEEK_SET);
		}
		BYTE *buf;
		buf=(BYTE*)malloc(4096*512);
		int percentage;
		double timdiff=0.0;
		int leftmin=0;
		int lefthour=0;
		int sec=0;
		for(UINT64 j=0;j<tag.packetnum;j++)
		{

			fread(&packet,packetsize,1,pfile);
			if(ReadPacket(packet,buf,pfile)==FALSE)
			{
				free(buf);
				goto err;
			}

/*			if(BufCurPos==0)
			{
				BufFirstSec=packet.StartSector;
				memcpy(pWriteBuf,buf,packet.Sectors*512);
				BufCurPos=packet.Sectors;
			}*/
		    if((BufCurPos+packet.Sectors<=WRITE_BUFFER_LEN)&&(BufFirstSec+BufCurPos==packet.StartSector))
			{
				memcpy(pWriteBuf+BufCurPos*512,buf,packet.Sectors*512);
				BufCurPos+=packet.Sectors;
			}
			else
			{
				if(WriteSectors(hDevice,RestorePart.StartLogicSecNo+BufFirstSec,BufCurPos,pWriteBuf)==FALSE)
			    {
				    MessageBox(NULL,"写扇区失败","提示",MB_OK);
					free(buf);
			        goto err;
			    }
				BufFirstSec=packet.StartSector;
				memcpy(pWriteBuf,buf,packet.Sectors*512);
				BufCurPos=packet.Sectors;
			}

		
			if(g_bCancel)
			{
				free(buf);
				goto cancel;
			}
			cursector+=packet.Sectors;
			
			time(&end_time);
			timdiff=difftime(end_time,start_time);
			if(timdiff-timediff1>=1)//每一秒更新一次数据
			{
				percentage = (int)(cursector * 100 / totalsector);
				progressdlg->m_percentage.Format("%d%%", percentage);
				progressdlg->m_prog.SetPos(percentage);
				progressdlg->m_filename = temp;
				timediff1=timdiff;
				timdiff=timdiff*(totalsector-cursector)/cursector;				    
				leftmin=(int)timdiff/60;
				lefthour=leftmin/60;
				leftmin=leftmin % 60;
				sec= (int)timdiff % 60;
				progressdlg->m_lefttime.Format("%d小时%d分%d秒",lefthour,leftmin,sec);
				progressdlg->SendMessage(WM_RESTUPDATE, 0);
			}
			
		}
		free(buf);
		fclose(pfile);
	}
	if(BufCurPos!=0)
	{
		if(WriteSectors(hDevice,RestorePart.StartLogicSecNo+BufFirstSec,BufCurPos,pWriteBuf)==FALSE)
	    {
			MessageBox(NULL,"写扇区失败","提示",MB_OK);
			goto err;
	    }
	}
	free(pWriteBuf);

	  
	CloseHandle(hDevice);
	NTUnlockDriver(hMyDriver);
	CloseHandle(hMyDriver);
	return 0;
err:
	if(Resvbuf)free(Resvbuf);
	if(pWriteBuf)free(pWriteBuf);
	CloseHandle(hDevice);
	NTUnlockDriver(hMyDriver);
	CloseHandle(hMyDriver);
	return 1;
cancel:
	if(Resvbuf)free(Resvbuf);
	if(pWriteBuf)free(pWriteBuf);
	CloseHandle(hDevice);
	NTUnlockDriver(hMyDriver);
	CloseHandle(hMyDriver);
	return 2;
}