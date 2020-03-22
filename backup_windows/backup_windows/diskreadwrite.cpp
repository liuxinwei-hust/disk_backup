#include "stdafx.h"

/************************************************************
Function:		ReadSectors       
Description:    read secors according to LBA in disk根据地址读取磁盘
Input:			Disk			// symbol of disk eg:0x80哪个盘
				StartLBA		// LBA in disk开始地址
				ReadSectorNum	// amount of sectors reading读取数目
Output:			*Buffer			// buffer storing data读到的内容	
Return:         true/false	
**************************************************************/
BOOL ReadSectors(BYTE Disk, UINT64 StartLBA , UINT64 ReadSectorNum,BYTE *Buffer, DWORD dwSectorSize )
{
	HANDLE hDevice;               // handle to the drive to be examined 
	BOOL bResult;                 // results flag
	CString DiskSymbol;

	int DiskNum=Disk-0x80;
//	WORD BytePerSector=512;		//DiskParameters[DiskNum].SectorSize;

	DiskSymbol.Format(_T("\\\\.\\PhysicalDrive%d"),DiskNum);

	hDevice = CreateFile(DiskSymbol, 
		GENERIC_READ | GENERIC_WRITE,       
		FILE_SHARE_READ | FILE_SHARE_WRITE,  
		0,OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN,0);

	if (hDevice == INVALID_HANDLE_VALUE)         // if we can't open the drive...
	{
		//AfxMessageBoxEx(IDS_READ_DISK_FAILED);
		return (FALSE);
	}
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

	CloseHandle(hDevice); // we're done with the handle

	return (bResult);
}

BOOL FastReadSectors(HANDLE hDevice, UINT64 StartLBA, UINT64 ReadSectorNum, BYTE* Buffer, DWORD dwSectorSize)
{
	BOOL bResult;                 // results flag
	CString DiskSymbol;

	if (hDevice == INVALID_HANDLE_VALUE)         // if we can't open the drive...
	{
		//AfxMessageBoxEx(IDS_READ_DISK_FAILED);
		return (FALSE);
	}
	DWORD dwBytesRead, dwPos;
	LARGE_INTEGER HiddenSectors;
	HiddenSectors.QuadPart = StartLBA * dwSectorSize;//LowPart ,HighPart 

	dwPos = SetFilePointer(hDevice, HiddenSectors.LowPart, &HiddenSectors.HighPart, FILE_BEGIN);
	bResult = ReadFile(hDevice, Buffer, (unsigned long)(ReadSectorNum * dwSectorSize), &dwBytesRead, NULL);

	if (!bResult) // if IOCTL failed...
	{
		//  AfxMessageBoxEx(IDS_READ_DISK_FAILED);
		return (FALSE);
	}

	return (bResult);
}

/************************************************************
Function:		WriteSectors       
Description:    write data to disk according to LBA
Input:			Disk			// symbol of disk eg:0x80
				StartLBA		// LBA in disk
				ReadSectorNum	// amount of sectors write
				*Buffer			// buffer storing data
Output:				
Return:         true/false	
**************************************************************/
BOOL WriteSectors(HANDLE hDevice, UINT64 StartLBA, UINT64 WriteSectorNum,BYTE *Buffer)
{

               // handle to the drive to be examined 
	BOOL bResult;                 // results flag



	WORD BytePerSector=512;



	DWORD dwBytesWrite,dwPos;
	LARGE_INTEGER HiddenSectors;
	HiddenSectors.QuadPart=StartLBA * BytePerSector;//LowPart ,HighPart 

	dwPos = SetFilePointer(hDevice, HiddenSectors.LowPart,&HiddenSectors.HighPart, FILE_BEGIN); 
	bResult = WriteFile(hDevice, Buffer,(unsigned long)(WriteSectorNum*BytePerSector) , &dwBytesWrite , NULL);

	if (!bResult) // if IOCTL failed...
	{
		return (FALSE);
	}

//	CloseHandle(hDevice); // we're done with the handle

	return (bResult);
}