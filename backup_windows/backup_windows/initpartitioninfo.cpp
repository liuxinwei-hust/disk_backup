#include "stdafx.h"
#include "initpartitioninfo.h"


extern BOOL ReadSectors(BYTE Disk, UINT64 StartLBA , UINT64 ReadSectorNum,BYTE *Buffer, DWORD dwSectorSize );
int diskmaxnum=0;
Int13ParaEx diskinfo[PhysicalDriverMAX];


/************************************************************
Function:		Is_Floppy_CDROM
Description:	chech whether logic driver is CDROM or removable device
Input:			Driver_buffer	// symbol of logic driver
Output:
Return:			true
				false
**************************************************************/
BOOL Is_Floppy_CDROM(char Driver_buffer)
{
	char	lcDriveRoot[MAX_PATH];
	memset(lcDriveRoot,'\0',MAX_PATH);
	lcDriveRoot[0] = Driver_buffer;
	lcDriveRoot[1] = ':';
	lcDriveRoot[2] = '\\';
	CString lcDrivestring;
	BOOL	Bret = false;

	UINT lcDrivetype = GetDriveTypeA(lcDriveRoot);

	switch (lcDrivetype)
	{
	case DRIVE_REMOVABLE:
		Bret = true;
		break;
	case DRIVE_CDROM:
		Bret = true;
		break;
	case DRIVE_RAMDISK:
		break;
	default:
		;
	}
	return Bret;
}


/************************************************************
Function:		CanOpenPartition       
Description:    check whether partition exists
and check whether partition can be opened
if partition exists
Input:			DriverName	// symbol of logic driver
Output:				
Return:         true/false	
**************************************************************/
BOOL CanOpenPartition(char DriverName)
{
	CString DiskSymbol="";
	DiskSymbol.Format(_T("\\\\.\\%c:"),DriverName);

	HANDLE hPart = CreateFile(DiskSymbol, 
		GENERIC_READ | GENERIC_WRITE,       
		FILE_SHARE_READ | FILE_SHARE_WRITE,  
		0,OPEN_EXISTING,0,0); 
	if (hPart == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}	
	CloseHandle(hPart);
	return TRUE;

}

/*
char getsymbol(int num,UINT64 StartSec)
{
	DWORD sectorsize=diskinfo[num].SectorSize;
    HKEY	hKey			;
	HKEY	Driver_hKey		;
	CString strMuontDriver	;
	strMuontDriver = "SYSTEM\\MountedDevices";
	long	retValue		;
	DWORD	index	 = 0	;
	DWORD	cchValue = MAX_PATH	;
	char	achValue[MAX_PATH]	; 
	memset(achValue,MAX_PATH,'\0');
	CString Driver;
	long	retstatus;
	DWORD   MyKeyType  = REG_BINARY;
	int		i = 0;
	UINT64 LStartSector = 0;

	long ret = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
		strMuontDriver,0, KEY_READ, &hKey );

	if (ret != ERROR_SUCCESS) 
	{
		MessageBox(NULL,"打开注册表失败","提示",MB_OK);
		return '0';
	}
	for(index = 0;;index++)//retValue != ERROR_SUCCESS
	{
		cchValue = MAX_PATH;
		achValue[0] = '\0'; 
		retValue = RegEnumValueA(hKey, 
			index, 
			achValue, 
			&cchValue, 
			NULL, 
			NULL,    // &dwType, 
			NULL,    // &bData, 
			NULL);   // &bcData 
		if(retValue!= ERROR_SUCCESS)
		{
			break;
		}
		if (strlen(achValue) == 14 && achValue[12] != 'A' && !Is_Floppy_CDROM(achValue[12]))
		{
			Driver = achValue;
			retstatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
						strMuontDriver,
						0,
						KEY_ALL_ACCESS,
						&Driver_hKey);
			if (retstatus == ERROR_SUCCESS)
			{
				DWORD   MyKeySize = 24;
				BYTE	tmp[25];
				memset(tmp,'\0',sizeof(tmp));
				retstatus = RegQueryValueEx(Driver_hKey, Driver, 0, &MyKeyType, tmp, &MyKeySize);
                memcpy( (void *)&LStartSector, &tmp[4], 8 );
				if ((tmp[0] ==diskinfo[num].SectorBuffer[440])&&(tmp[1] == diskinfo[num].SectorBuffer[441])
					&&(tmp[2] == diskinfo[num].SectorBuffer[442])&&(tmp[3] == diskinfo[num].SectorBuffer[443]))
				{
					if (StartSec == LStartSector/diskinfo[num].SectorSize && CanOpenPartition(achValue[12]))
					{
						RegCloseKey( Driver_hKey );
						RegCloseKey( hKey );
						return achValue[12] ;	
				    }
				}
				RegCloseKey( Driver_hKey );
			}

		}
	}
	RegCloseKey( hKey );
	return '0';
}
*/
char getsymbol2(int num,UINT64 StartSec)
{
	
	

	for (char letter='A';letter<='Z';letter++)
 {
	HANDLE hDevice;               // handle to the drive to be examined
    BOOL result;                 // results flag
    DWORD readed;                   // discard results
    STORAGE_DEVICE_NUMBER number;   //use this to get disk numbers
 
	CString path;
	path.Format("\\\\.\\%c:", letter);
    hDevice = CreateFile(path, // drive to open
                         GENERIC_READ | GENERIC_WRITE,    // access to the drive
                         FILE_SHARE_READ | FILE_SHARE_WRITE,    //share mode
                         NULL,             // default security attributes
                         OPEN_EXISTING,    // disposition
                         0,                // file attributes
                         NULL);            // do not copy file attribute
    if (hDevice == INVALID_HANDLE_VALUE) // cannot open the drive
    {
        fprintf(stderr, "CreateFile() Error: %ld\n", GetLastError());
		continue;
    }
 
    result = DeviceIoControl(
                hDevice,                // handle to device
                IOCTL_STORAGE_GET_DEVICE_NUMBER, // dwIoControlCode
                NULL,                            // lpInBuffer
                0,                               // nInBufferSize
                &number,           // output buffer
                sizeof(number),         // size of output buffer
                &readed,       // number of bytes returned
                NULL      // OVERLAPPED structure
            );
       if (!result) // fail
       {
        fprintf(stderr, "IOCTL_STORAGE_GET_DEVICE_NUMBER Error: %ld\n", GetLastError());
        (void)CloseHandle(hDevice);
		continue;
       }

	   // 读取分区大小和偏移量  
       PARTITION_INFORMATION partInfo;  
       result = DeviceIoControl(hDevice, IOCTL_DISK_GET_PARTITION_INFO,  
       NULL, 0, &partInfo, sizeof(PARTITION_INFORMATION), &readed, NULL);  
   
	   UINT64 lOffset =(UINT64) partInfo.StartingOffset.QuadPart;             // 偏移  
  
       // StartingOffset 除以每扇区字节数就是开始扇区；  
  
		UINT64  PartBegin = lOffset / diskinfo[num].SectorSize;
		if(PartBegin==StartSec && diskinfo[num].Flags==number.DeviceNumber)
		{
			return letter;
		}
       (void)CloseHandle(hDevice);
	}
	return '0';
}



int initpartitioninfo()
{
	for(int i=0;i<diskmaxnum;i++)
	{
		int partnum=0;
		diskinfo[i].partitionnum=0;
		for(int j=0;j<4;j++)
		{
			int offset;
			BYTE FileSystemFlag;
			BYTE ActiveFlag;
			UINT64 ExtendFirstStartSec=0;
			UINT64 ExtendStartSec=0;
			offset=16*j;
			FileSystemFlag=diskinfo[i].SectorBuffer[offset+PartTablePOS+4];
			ActiveFlag=diskinfo[i].SectorBuffer[offset+PartTablePOS];
			if(FileSystemFlag==0)
			{
				continue;
			}
			if(*(DWORD*)(diskinfo[i].SectorBuffer+offset+PartTablePOS+8)==0)
			{
				continue;
			}
			if(*(DWORD*)(diskinfo[i].SectorBuffer+offset+PartTablePOS+12)<=63)
			{
				continue;
			}
			if(FileSystemFlag!=0x05 && FileSystemFlag!=0x0f)//不是扩展分区
			{
				diskinfo[i].partition[partnum].ActiveFlag=ActiveFlag;
				diskinfo[i].partition[partnum].InExtend=0;
				diskinfo[i].partition[partnum].PartitionSec=0;
				diskinfo[i].partition[partnum].SystemFlag=FileSystemFlag;
				diskinfo[i].partition[partnum].StartLogicSecNo=*(DWORD*)(diskinfo[i].SectorBuffer+PartTablePOS+offset+8);
				diskinfo[i].partition[partnum].SectorNum=*(DWORD*)(diskinfo[i].SectorBuffer+PartTablePOS+offset+12);
				diskinfo[i].partition[partnum].LogicDriveSymbol=getsymbol2(i,diskinfo[i].partition[partnum].StartLogicSecNo);
				partnum++;
				diskinfo[i].partitionnum=partnum;
			}
			else
			{
				ExtendFirstStartSec=*(DWORD*)(diskinfo[i].SectorBuffer+PartTablePOS+offset+8);
				ExtendStartSec=ExtendFirstStartSec;
				while(ExtendStartSec!=0)
				{
					DWORD sectorsize=diskinfo[i].SectorSize;
	                BYTE *mbrtemp=new BYTE[sectorsize];
	                WORD driver;
					driver=0x80+diskinfo[i].Flags;
	                memset(mbrtemp,0,sectorsize);
					if(!ReadSectors((BYTE)driver,ExtendStartSec,1,mbrtemp,sectorsize))
	                {
		               MessageBox(NULL,"读取MBR出错","提示",MB_OK);
					   delete []mbrtemp;
					   return 0;
                   	}
					diskinfo[i].partition[partnum].ActiveFlag=mbrtemp[PartTablePOS];
				    diskinfo[i].partition[partnum].InExtend=1;
					diskinfo[i].partition[partnum].PartitionSec=ExtendStartSec;
				    diskinfo[i].partition[partnum].SystemFlag=mbrtemp[PartTablePOS+4];
					diskinfo[i].partition[partnum].StartLogicSecNo=*(DWORD*)(mbrtemp+PartTablePOS+8)+ExtendStartSec;
					diskinfo[i].partition[partnum].SectorNum=*(DWORD*)(mbrtemp+PartTablePOS+12);
					diskinfo[i].partition[partnum].LogicDriveSymbol=getsymbol2(i,diskinfo[i].partition[partnum].StartLogicSecNo);
				    partnum++;
				    diskinfo[i].partitionnum=partnum;
					ExtendStartSec=*(DWORD*)(mbrtemp+PartTablePOS+16+8)+ExtendFirstStartSec;
					if(mbrtemp[PartTablePOS+16+4]==0)
			        {
				         ExtendStartSec=0;
						 delete[]mbrtemp;
						 break;
			        }
			        if(*(DWORD*)(mbrtemp+PartTablePOS+16+8)==0)
			        {
				         ExtendStartSec=0;
						 delete[]mbrtemp;
						 break;
			        }
			        if(*(DWORD*)(mbrtemp+PartTablePOS+16+12)<=63)
			        {
				         ExtendStartSec=0;
						 delete[]mbrtemp;
						 break;
			        }
					delete []mbrtemp;
				}
			}
		}
	}
	return 1;
}





int initdiskinfo()
{
	diskmaxnum=0;
	for(int i=0;i<PhysicalDriverMAX;i++)
	{
		DISK_GEOMETRY_EX  pdg;        // disk drive geometry structure
	    HANDLE hDevice;               // handle to the drive to be examined 
	    BOOL bResult;                 // results flag
	    DWORD junk;                   // discard results
	    CString DiskSymbol;


	DiskSymbol.Format(_T("\\\\.\\PhysicalDrive%d"),i);
	hDevice = CreateFile(DiskSymbol, 
		GENERIC_READ | GENERIC_WRITE,       
		FILE_SHARE_READ | FILE_SHARE_WRITE,  
		0,OPEN_EXISTING,0,0); 

	if (hDevice == INVALID_HANDLE_VALUE)         // if we can't open the drive...
	{
		continue;
	}

	bResult = DeviceIoControl(hDevice,  // the device we are querying
		IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,  // operation to perform
		NULL, 0, // no input buffer, so we pass zero
		&pdg, sizeof(pdg),  // the output buffer
		&junk, // discard the count of bytes returned
		(LPOVERLAPPED) NULL);  // synchronous I/O

	if (!bResult)                // if IOCTL failed...
	{
		CloseHandle(hDevice);
		continue;
	}

/*	if( pdg.Geometry.MediaType == RemovableMedia )
	{
		CloseHandle(hDevice);
		return 0;
	}*/
	diskinfo[diskmaxnum].Flags =i;
	diskinfo[diskmaxnum].Heads = (pdg.Geometry.TracksPerCylinder);
	diskinfo[diskmaxnum].SecPerTrack = (pdg.Geometry.SectorsPerTrack);
	diskinfo[diskmaxnum].Cylinders = (pdg.Geometry.Cylinders.LowPart);
	diskinfo[diskmaxnum].SectorSize = (pdg.Geometry.BytesPerSector);
	diskinfo[diskmaxnum].Sectors = pdg.DiskSize.QuadPart / pdg.Geometry.BytesPerSector;
	diskinfo[diskmaxnum].mediatype=pdg.Geometry.MediaType;

//存储MBR
	DWORD sectorsize=diskinfo[diskmaxnum].SectorSize;
	BYTE *mbrtemp=new BYTE[sectorsize];
	WORD driver;
	driver=0x80+i;
	memset(mbrtemp,0,sectorsize);
	if(!ReadSectors((BYTE)driver,0,1,mbrtemp,sectorsize))
	{
		MessageBox(NULL,"读取MBR出错","提示",MB_OK);
	}
	memcpy(diskinfo[i].SectorBuffer,mbrtemp,512);
	delete mbrtemp;
	diskmaxnum++;
	CloseHandle(hDevice);         // we're done with the handle

	}
	initpartitioninfo();
	return 1;

}