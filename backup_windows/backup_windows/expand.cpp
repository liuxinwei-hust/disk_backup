
//----------------------------------------------------------------
//  EXP32.CPP - Extractor for real-time compressor
//  Copyright 1994-1996, By NEO Solutions Inc.	All Rights Reserved
//----------------------------------------------------------------
#include "stdafx.h"
#include <stdio.h>
//#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define WORD  unsigned short int
#define UINT  unsigned int
#define UCHAR unsigned char
#define ULONG unsigned long

#define FALSE	       0
#define TRUE	       1
#define EXIT_OK        0
#define EXIT_FAILED    1

#define CLUSTER_SIZE	 (32768)

//#define FILL_CLUSTER		  // Defined when extractor must zero
// remaining cluster data

//#define USE_C_CODE		  // Define to use C code rather than ASM
//#define DEBUG_MOVE
//#define DEBUG_COPY

struct HeaderRecord
{
	unsigned int format;
	unsigned int size;
	unsigned int CDsize;
	char Description[128];
	char Pad[372];
};

//FILE *InFile, *OutFile;

//UCHAR InBuf[CLUSTER_SIZE+300], OutBuf[CLUSTER_SIZE+300];
//WORD  Offset[CLUSTER_SIZE+2];

//-------------------------------------
// Function: GetNibble
//-------------------------------------

unsigned int GetNibble1(UCHAR **InPtr, UINT *NibData, int *NibCnt)
{
	unsigned int i;

	if (--(*NibCnt) <= 0)
	{
		*NibData = *(UINT *)*InPtr;
		*InPtr += 4;
		*NibCnt = 8;
	}
	i = *NibData & 0xF;
	*NibData >>= 4;

	return i;
}

//-------------------------------------
// Function: GetBit
//-------------------------------------

unsigned int GetBit1(UCHAR **InPtr, UINT  *BitData, int *BitCnt)
{
	unsigned int i;

	if (--(*BitCnt) <= 0)
	{
		*BitData = *(UINT *)*InPtr;
		*InPtr += 4;
		*BitCnt = 32;
	}
	i = *BitData & 1;
	*BitData >>= 1;

	return i;
}

//-------------------------------------
// Function: GetDist
//-------------------------------------

unsigned int GetDist1(UINT More256, UCHAR **InPtr, UINT *NibData, int *NibCnt)
{
	UINT Dist;

	if (!More256)
		return *(*InPtr)++;

	if ((Dist = GetNibble1(InPtr, NibData, NibCnt)) != 0)
		return (Dist << 8) | *(*InPtr)++;

	Dist = 4096 | (GetNibble1(InPtr, NibData, NibCnt) << 8);
	return Dist | *(*InPtr)++;
}

//-------------------------------------
// Function: Expand
//-------------------------------------

//--->modify 2009/10/20 because external projects use function "Expand" 
// from
// int Expand1(UCHAR *OutBuf, UCHAR *InBuf, WORD *Offset)
// to
int Expand(UCHAR *OutBuf, UCHAR *InBuf, WORD *Offset)
	//<---
{
	int Len, Dist, OffPos;
	WORD i;
	UCHAR *Ptr, *OutLim, *InLim;

	//	Use to be global... Moved local to make re-enterent
	UINT		BitData, NibData;
	int		BitCnt, NibCnt;
	UCHAR	*InPtr, *OutPtr;

	InPtr = InBuf;
	OutPtr = OutBuf;
	NibCnt = BitCnt = 0;
	OffPos = 0;
	OutLim = OutBuf + CLUSTER_SIZE;
	InLim =  InBuf + CLUSTER_SIZE;

	do
	{
		Len = 0;
		Dist = -1;
		switch (GetNibble1(&InPtr, &NibData, &NibCnt))
		{
		case 2:   // Copy 5, Dist 1 - 256
			++Len;
		case 1:   // Copy 4, Dist 2 - 256
			++Len;
		case 0:   // Copy 3, Dist 2 - 256
			Len += 3;
			Dist = *InPtr++;
			break;

		case 3:   // Copy 3, Dist 257 - 512
			Len = 3;
			Dist = 256 + *InPtr++;
			break;

		case 4:   // Copy 3, Dist 513 - 4096, Copy 3 or 4 Dist 1
			Len = 3;
			if ((Dist = GetNibble1(&InPtr, &NibData, &NibCnt)) < 2)
			{
				Len += Dist;
				Dist = 0;
			}
			else
				Dist = (Dist << 8) | *InPtr++;
			break;

		case 6:   // Copy 5, Dist 257 - 65536
			++Len;
		case 5:   // Copy 4, Dist 257 - 65536
			Len += 4;
			Dist = GetDist1(1, &InPtr, &NibData, &NibCnt);
			break;

		case 7:   // Copy 6
			Len = 6;
			Dist = GetDist1(GetBit1(&InPtr, &BitData, &BitCnt), &InPtr, &NibData, &NibCnt);
			break;
		case 8:   // Copy 7-8
			Len = 7 + GetBit1(&InPtr, &BitData, &BitCnt);
			Dist = GetDist1(GetBit1(&InPtr, &BitData, &BitCnt), &InPtr, &NibData, &NibCnt);
			break;
		case 9:   // Copy 9-12
			Len = 9 + (GetBit1(&InPtr, &BitData, &BitCnt) << 1);
			Len += GetBit1(&InPtr, &BitData, &BitCnt);
			Dist = GetDist1(GetBit1(&InPtr, &BitData, &BitCnt), &InPtr, &NibData, &NibCnt);
			break;

		case 11:  // Move 2
			++Len;
		case 10:  // Move 1
			++Len;
			break;
		case 12:  // Move 3-4
			Len = 3 + GetBit1(&InPtr, &BitData, &BitCnt);
			break;
		case 13:  // Move 5-8
			Len = 5 + (GetBit1(&InPtr, &BitData, &BitCnt) << 1);
			Len += GetBit1(&InPtr, &BitData, &BitCnt);
			break;
		case 14:  // Move 9-28
			Len = 9 + GetNibble1(&InPtr, &NibData, &NibCnt);
			break;

		case 15:  // Misc.
			if (GetBit1(&InPtr, &BitData, &BitCnt) == 0)	       // Copy 13 - 28
			{
				Len = 13 + GetNibble1(&InPtr, &NibData, &NibCnt);
				Dist = GetDist1(GetBit1(&InPtr, &BitData, &BitCnt), &InPtr, &NibData, &NibCnt);
				break;
			}
			else if (GetBit1(&InPtr, &BitData, &BitCnt) == 0)    // Move 25 - Max
				Len = 25 + *InPtr++;
			else		       // Copy 29 - Max
			{
				Len = 29 + *InPtr++;
				if (Len == 29 + 255)    // End of compressor
				{
					Len = (UINT)(OutPtr - OutBuf);
#ifdef FILL_CLUSTER
					while (Len++ < CLUSTER_SIZE)
						*OutPtr++ = '\0';
#endif
					return Len;
				}
				Dist = GetDist1(GetBit1(&InPtr, &BitData, &BitCnt), &InPtr, &NibData, &NibCnt);
			}
			break;
		}

		if (Dist < 0)	     // Do a move
		{
#ifdef DEBUG_MOVE
			printf("Move-%d\n",Len);
#endif
			memcpy(OutPtr, InPtr, Len);
			i = (WORD)(OutPtr - OutBuf);
			OutPtr += Len;
			InPtr += Len;
			do
			{
				Offset[OffPos++] = i++;
			} while (--Len);
		}
		else		     // Do a copy
		{
#ifdef DEBUG_COPY
			printf("Copy-%d %d\n",Len,Dist);
#endif
			if ((Ptr = &OutBuf[Offset[OffPos - Dist - 1]]) > OutPtr)
				return -2;			// Extract error
			Offset[OffPos++] = (WORD)(OutPtr - OutBuf);
			do
			{
				*OutPtr++ = *Ptr++;
			} while (--Len);
		}

		if (OutPtr > OutLim || InPtr > InLim)
			return -1;		       // Extract error
	} while (TRUE);
}
