#include "stdafx.h"
#define	M_CMPCTEST	1

#if M_CMPCTEST
/*#include "stdafx.h"   */
#define	inline		/**/
#include <stdio.h>
#include <stdlib.h>
#ifndef  SUN
//#include <io.h>
#endif
#include <fcntl.h>
#include <string.h>
#include "compress.h"

//#ifdef	SUN
#define		min(a,b)	((a)<=(b)?(a):(b))
//#endif

/*----------------------------------------------------------------
//  CMP1 - Real-time compresion routine - method 1
//  Copyright 1994, By NEO Solutions Inc.  All Rights Reserved
//----------------------------------------------------------------

			  Compression Codin
			  ------------------
Val   Action	  Bits	   Code        Val  Action	 Bits	    Code
---------------------------------      -----------------------------------
 0  3, 2-256	  4 +8	    (0) 	 7  Copy 6	 4 +Dist     (7)
 1  4, 2-256	  4 +8	    (1) 	 8  Copy 7-8	 4 +1 +Dist  (8)
 2  5, 1-256	  4 +8	    (2) 	 9  Copy 9-12	 4 +2 +Dist  (9)
 3  3, 257-512	  4 +8	    (3) 	10  Move 1	 4	    (10)
 4  3, 513-4096   4 +4 +8   (4) 	11  Move 2	 4	    (11)
    3, Dist 1	  4 +4	    (4) 0	12  Move 3-4	 4 +1	    (12)
    4, Dist 1	  4 +4	    (4) 1	13  Move 5-8	 4 +2	    (13)
 5  4, 257-8192   4 +Dist-1 (5) 	14  Move 9-24	 4 +4	    (14)
 6  5, 257-8192   4 +Dist-1 (6) 	15  Copy 13-29	 4 +1+4+Dist(15) 0
					    Copy 29-Max  4 +2+8+Dist(15) 11
					    Move 25-Max  4 +2+8     (15) 10

	Distance Coding
	---------------
  Distance	   Bits
------------------------------------
   1 -	256    (9) 1 + 8
 257 - 4096   (13) 1 + 4 + 8
4097 - 8192   (17) 1 + 4 + 4 + 8
**/

#define LONGMOVENIBBLE 15
#define LONGCOPYNIBBLE 15
#define LONGCOPYBITS	2
#define LONGCOPYCODE	3
#define LONGMOVEBITS	2
#define LONGMOVECODE	1

UCHAR *OutPtr;
UINT  *BitBuf, *NibbleBuf;
UINT  BitsLeft, NibblePos;

#define THRESHOLD    3

#define MAXSHORTMOVE  24
#define MAXSHORTCOPY  28
#define MAXREP	     (MAXSHORTCOPY+254)
#define MAXREPMOVE   (MAXSHORTMOVE+255)

UCHAR MoveNibble[] = {
  10,11,12,12,13,13,13,13,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14 };
UCHAR MoveBits[]  = {  0,  0,  1,  1,  2,  2,  2,  2 };
UINT  MoveCodes[] = {  0,  0,  0,  1,  0,  2,  1,  3 };

UCHAR LenNibble[] = { 7, 8, 8, 9, 9, 9, 9 };
UCHAR LenBits[]   = { 0, 1, 1, 2, 2, 2, 2 };
UCHAR LenCodes[]  = { 0, 0, 1, 0, 2, 1, 3 };

/*-------------------------------------
// OutBitCodes
//-------------------------------------
//					*/
void OutBitCodes(UINT Bits, UINT Code)
{
	if (Bits == 0)
		return;
	
	if (BitsLeft)
#ifndef SUN
		*BitBuf |= Code << (32 - BitsLeft);
#else
		ut_32toL ( (int*)BitBuf , ut_Lto32 ( (int*)BitBuf )|
						Code << (32 - BitsLeft)  ) ;
#endif
	if (Bits > BitsLeft)
	{
		BitBuf = (UINT *)OutPtr;
		OutPtr += 4;
		Bits -= BitsLeft;
#ifndef SUN
		*BitBuf = Code >> BitsLeft;
#else
		ut_32toL ( (int*)BitBuf , Code >> BitsLeft ) ;
#endif
		BitsLeft = 32;
	}
	BitsLeft -= Bits;
}

/*-------------------------------------
// OutNibble
//-------------------------------------
//					*/
inline void OutNibble(UINT Code)
{
	if (NibblePos == 0)
	{
		NibbleBuf = (UINT *)OutPtr;
#ifndef SUN
		*NibbleBuf = 0;
#else
		ut_32toL ( (int*)NibbleBuf , 0 ) ;
#endif
		OutPtr += 4;
	}
#ifndef SUN
	*NibbleBuf |= Code << NibblePos;
#else
	ut_32toL ( (int*)NibbleBuf , ut_Lto32 ( (int*)NibbleBuf )|
						Code << NibblePos  ) ;
#endif
	if ((NibblePos += 4) == 32)
		NibblePos = 0;
}

/*-------------------------------------
// SendMoves
//-------------------------------------
// Output move data encoding		*/

void SendMoves(UCHAR *InPtr, UINT MoveCnt)
{
#ifdef STATS
	++MovCnts[MoveCnt];
	++TotalMoves;
#endif
	
	if (MoveCnt <= MAXSHORTMOVE)
	{
		OutNibble(MoveNibble[MoveCnt-1]);
		if (MoveCnt < 9)
			OutBitCodes(MoveBits[MoveCnt-1], MoveCodes[MoveCnt-1]);
		else
			OutNibble(MoveCnt - 9);
	}
	else
	{
		OutNibble(LONGMOVENIBBLE);
		OutBitCodes(LONGMOVEBITS, LONGMOVECODE);
		*OutPtr++ = (UCHAR)(MoveCnt - MAXSHORTMOVE - 1);
	}
	
#ifdef DEBUG_MOVE
	printf("Move-%d\n",MoveCnt);
#endif
	
	InPtr -= MoveCnt;
	do
	{
		*OutPtr++ = *InPtr++;
	} while (--MoveCnt);
}

/*-------------------------------------
// SendCopy
//-------------------------------------
// Output copy data encoding			*/

void SendCopy(UINT Len, UINT Dist)
{
#ifdef STATS
	++TotalCopies;
	if (Dist < 16)
		++LengthCnts[Len<16 ? Len-3 : (Len-3)/16+13][0];
	else if (Dist < 1024)
		++LengthCnts[Len<16 ? Len-3 : (Len-3)/16+13][Dist/256 + 1];
	else if (Dist < 4096)
		++LengthCnts[Len<16 ? Len-3 : (Len-3)/16+13][Dist/1024+4];
	else if (Dist < 8192)
		++LengthCnts[Len<16 ? Len-3 : (Len-3)/16+13][8];
	else
		++LengthCnts[Len<16 ? Len-3 : (Len-3)/16+13][9];
#endif
	
	if (Len <= 5)
	{
		Len -= 3;
		if (Dist < 256)
		{
			if (Dist==0 && Len <= 1)
			{
				OutNibble(4);
				OutNibble(Len);
				return;
			}
			OutNibble(Len);
		}
		else if (Len == 0 && Dist < 512)
		{
			OutNibble(3);
			*OutPtr++ = (UCHAR)((Dist-256) % 256);
			return;
		}
		else
			OutNibble(Len + 4);
	}
	else if (Len < 13)
	{
		OutNibble(LenNibble[Len - 6]);
		OutBitCodes(LenBits[Len - 6], LenCodes[Len - 6]);
		OutBitCodes(1, Dist >= 256);
	}
	else
	{
		OutNibble(LONGCOPYNIBBLE);
		if (Len < 29)
		{
			OutBitCodes(1, 0);
			OutNibble(Len - 13);
		}
		else
		{
			OutBitCodes(LONGCOPYBITS, LONGCOPYCODE);
			*OutPtr++ = (UCHAR)(Len - 29);
		}
		OutBitCodes(1, Dist >= 256);
	}
	
	if (Dist >= 256)
	{
		if (Dist < 4096)
			OutNibble(Dist / 256);
		else
		{
			OutNibble(0);
			OutNibble((Dist - 4096) / 256);
		}
	}
	*OutPtr++ = (UCHAR)(Dist % 256);
}

/*-------------------------------------
// Compress
//-------------------------------------
// Main compress routine
//    Parameters - OutBuf  = Output buffer to hold compressed data, must be
//			     Cluster size + 300 bytes for overflow.
//		   InBuf   = Input buffer to hold data being compressed, must
//			     be at least cluster size.
//		   WorkBuf = Work buffer - must be
//			     (HASHSIZE * HASHDEPTH * 2) + (CLUSTER_SIZE * 2)
//		   InLen   = Length of input data, should be set to 0 for sector
//			     based compressor, where input size is always =
//			     cluster size.			*/

unsigned Compress(UCHAR *OutBuf, UCHAR *InBuf, UCHAR *WorkBuf, UINT InLen)
{
	UCHAR *InPtr, *Ptr, *mPtr, *Lim, *InLim, *OutLim;
	WORD  *Hash, *pHash, *Offset, OffPos, Match1, Match2;
	UINT  Len, Dist, Dist1, MoveCnt;
	
	Hash = (WORD *)WorkBuf;
	Offset = (WORD *)(&WorkBuf[HASHSIZE * HASHDEPTH * sizeof(WORD)]);
	
	if (InLen == 0)		       /* Do sector based compression */
	{
		OutLim = &OutBuf[CLUSTER_SIZE - SEC_SIZE];
		InLim = &InBuf[CLUSTER_SIZE-1];
		while (*InLim == '\0')           /* Skip 0's at End of Cluster */
		{
			if (--InLim == InBuf)
			{
				OutNibble(LONGCOPYNIBBLE);
				OutBitCodes(LONGCOPYBITS, LONGCOPYCODE);
				*OutPtr++ = 255;		    /* Send End of Data code */
				return (UINT)(OutPtr - OutBuf);
			}
		}
	}
	else
	{
		OutLim = &OutBuf[InLen-1];
		InLim = &InBuf[InLen];	       /* Max pos for InBuf */
	}
	
	memset(Hash, 0xFF, HASHSIZE * HASHDEPTH * sizeof(WORD));
	
	InPtr = InBuf;
	OutPtr = OutBuf;
	NibblePos = 0;
	BitsLeft = 0;		       /* Num of Unused bits in Buffer */
	MoveCnt = 0;
	OffPos = 0;
	
	do
	{
		Offset[(UINT)(InPtr-InBuf)] = OffPos++;
		
		pHash = &Hash[((*InPtr + (*(InPtr+1) << 4) + (*(InPtr+2)<<8)) &
			(HASHSIZE - 1)) * HASHDEPTH];
		
		Match1 = *pHash;
		*pHash = (WORD)(InPtr - InBuf);
		Match2 = pHash[1];
		pHash[1] = Match1;
		
		Len = 0;
		if (Match1 != 0xFFFF && (Dist = OffPos - Offset[Match1] - 2) < 8192)
		{
			mPtr = &InBuf[Match1];
			Lim = min(InPtr+MAXREP, InLim);
			for (Ptr=InPtr; Ptr<Lim && *Ptr == *mPtr; Ptr++, mPtr++)
				;
//--->modify 2009/11/08 to avoid warning of 64 bit verison 
// from
//			Len = Ptr - InPtr;
// to
			Len = (unsigned int)(Ptr - InPtr);
//<---			
			if (Match2 != 0xFFFF)
			{
				mPtr = &InBuf[Match2];
				if (*(mPtr+Len)==*(InPtr+Len) &&
					(Dist1 = OffPos - Offset[Match2] - 2) < 8192)
				{
					for (Ptr=InPtr; Ptr<Lim && *Ptr==*mPtr; Ptr++, mPtr++)
						;
					if ((UINT)(Ptr - InPtr) > Len)
					{
//--->modify 2009/11/08 to avoid warning of 64 bit version
// from
//						Len = Ptr - InPtr;
// to
						Len = (unsigned int)(Ptr - InPtr);
//<---
						Dist = Dist1;
					}
				}
			}
		}
		
		if (Len < THRESHOLD || (Len == THRESHOLD && Dist >= 4096))
		{
			InPtr++;
			if (++MoveCnt >= MAXREPMOVE)
			{
				SendMoves(InPtr, MoveCnt);
				MoveCnt = 0;
			}
		}
		else
		{
			if (MoveCnt != 0)
			{
				SendMoves(InPtr, MoveCnt);
				MoveCnt = 0;
			}
			SendCopy(Len, Dist);
			InPtr += Len;
#ifdef DEBUG_COPY
			printf("Copy-%d %d\n",Len,Dist);
#endif
		}
		
		if (OutPtr > OutLim)
		{
			if (InLen == 0)
				return CLUSTER_SIZE;
			else
				return InLen;
		}
	} while (InPtr < InLim);
	
	if (MoveCnt != 0)
		SendMoves(InPtr, MoveCnt);
	
	OutNibble(LONGCOPYNIBBLE);
	OutBitCodes(LONGCOPYBITS, LONGCOPYCODE);
	*OutPtr++ = 255;		   /* Send End of Data code */
	
	if (OutPtr > OutLim)
	{
		if (InLen == 0)
			return CLUSTER_SIZE;
		else
			return InLen;
	}
	
	return (UINT)(OutPtr - OutBuf);
}

int   Compress_buff_size (int c_inbuff_size , int* rc_outbuff_size ,
					      int* rc_wkbuff_size ) 
{
    c_inbuff_size = c_inbuff_size;
	/* c_inbuff_size = CLUSTER_SIZE		*/
	*rc_outbuff_size = CLUSTER_SIZE + 512 ;
	*rc_wkbuff_size =  (HASHSIZE * HASHDEPTH * 2) + (CLUSTER_SIZE * 2)  ;
//--->add 2009/11/08 to avoid warning
	return 0;
//<---
}


#if M_CMPCTEST

/* #include	"expand.c" */

#else

//----------------------------------------------------------------
//  EXP32.CPP - Extractor for real-time compressor
//  Copyright 1994-1996, By NEO Solutions Inc.	All Rights Reserved
//----------------------------------------------------------------

#if 000
    #include "vmm.h"
#define _WINDOWS    1		/* VxD changes in REDIR.H and CDEXT.H */
    #include <stdtype.h>
#else
//  #include "vmm.h"
#define _WINDOWS    1		/* VxD changes in REDIR.H and CDEXT.H */
//  #include <stdtype.h>
#define		CDECL
#define		BYTE	char
#define		TRUE	1
#endif

#pragma data_seg( "_LDATA", "CODE" )

//#define WORD	unsigned int
#define UINT  unsigned long
#define UCHAR unsigned char
#define ULONG unsigned long
#define SetC(X) 		    (X) |=  0x01
#define ClrC(X) 		    (X) &= ~0x01

#define WORK_BUF_SIZE			32768+300
#define OFFSET_BUF_SIZE 		(32768+2)
#define COMPRESSION_INDEX_ARRAY_SIZE	21121
#define COMPRESS_OPTIONS_BLK		512L
#define COMPRESS_NONE			0
#define COMPRESS_PKWARE 		1
#define STRING_BUF_SIZE 		64
#define BYTES_PER_SECTOR		2048
#define SECTORS_PER_TRACK		1
#define TRACKS_PER_CYLINDER		15
#define VTOC_LOCATION			0x8000
#define CONTAINERENTRYSZ		2048
#define CLUSTER_SIZE			(32768)
#define MAXIMUM_CDROM_SIZE		804

//#define FILL_CLUSTER		  // Defined when extractor must zero
				  // remaining cluster data

//#define USE_C_CODE		  // Define to use C code rather than ASM
//#define DEBUG_MOVE
//#define DEBUG_COPY


UINT  BitData, NibData;
int   BitCnt, NibCnt;
UCHAR *InPtr;
UCHAR *OutPtr;

long CDECL Expand(UCHAR *, UCHAR *, WORD *);
void CDECL str_ncpy(BYTE *, BYTE *, WORD);

ULONG CDECL Conv_1Block ( char* p_in  , char* p_out )
{
	char	c_offsetbuff[OFFSET_BUF_SIZE*sizeof(WORD)]	;
	
	if ((*(WORD *)(p_in)) & 0x8000) {
	    Expand(p_out, p_in + 2 , c_offsetbuff ) ;
	  } else {
	    str_ncpy(p_out, p_in + 2 , *(WORD *)p_in + 1);
	}
}

int /* ULONG CDECL */ Conv_1Block_Leng ( char* p_in  )
{
		int	c_header	;
		
#ifndef	SUN
		c_header = *(WORD *)(p_in) & 0xffff ;
#else
		c_header = ( *(unsigned char*)(p_in+0)      ) +
			   ( *(unsigned char*)(p_in+1) << 8 ) ;
#endif
	if (c_header/*(*(WORD *)(p_in))*/ & 0x8000) {
		    c_header &= 0x7fff ;
		  } else {
		    c_header += 1 ;
	}
	return ( c_header ) ;
}

//-------------------------------------
// Function: GetNibble
//-------------------------------------

UINT CDECL GetNibble(void)
{
   UINT i;

   if (--NibCnt <= 0)
   {
      NibData = *(UINT *)InPtr;
      InPtr += 4;
      NibCnt = 8;
   }
   i = NibData & 0xF;
   NibData >>= 4;

   return i;
}

//-------------------------------------
// Function: GetBit
//-------------------------------------

UINT CDECL GetBit(void)
{
   UINT i;

   if (--BitCnt <= 0)
   {
      BitData = *(UINT *)InPtr;
      InPtr += 4;
      BitCnt = 32;
   }
   i = BitData & 1;
   BitData >>= 1;

   return i;
}

//-------------------------------------
// Function: GetDist
//-------------------------------------

UINT CDECL GetDist(UINT More256)
{
   UINT Dist;

   if (!More256)
      return *InPtr++;

   if ((Dist = GetNibble()) != 0)
      return (Dist << 8) | *InPtr++;

   Dist = 4096 | (GetNibble() << 8);
   return Dist | *InPtr++;
}

//-------------------------------------
// Function: Expand
//-------------------------------------

long CDECL Expand(UCHAR *OutBuf, UCHAR *InBuf, WORD *Offset)
{
   long Len, Dist, OffPos;
   WORD i;
   UCHAR *Ptr;
   UCHAR *OutLim;
   UCHAR *InLim;

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
      switch (GetNibble())
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
	    if ((Dist = GetNibble()) < 2)
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
	    Dist = GetDist(1L);
	    break;

	 case 7:   // Copy 6
	    Len = 6;
	    Dist = GetDist(GetBit());
	    break;
	 case 8:   // Copy 7-8
	    Len = 7 + GetBit();
	    Dist = GetDist(GetBit());
	    break;
	 case 9:   // Copy 9-12
	    Len = 9 + (GetBit() << 1);
	    Len += GetBit();
	    Dist = GetDist(GetBit());
	    break;

	 case 11:  // Move 2
	    ++Len;
	 case 10:  // Move 1
	    ++Len;
	    break;
	 case 12:  // Move 3-4
	    Len = 3 + GetBit();
	    break;
	 case 13:  // Move 5-8
	    Len = 5 + (GetBit() << 1);
	    Len += GetBit();
	    break;
	 case 14:  // Move 9-28
	    Len = 9 + GetNibble();
	    break;

	 case 15:  // Misc.
	    if (GetBit() == 0)	       // Copy 13 - 28
	    {
	       Len = 13 + GetNibble();
	       Dist = GetDist(GetBit());
	       break;
	    }
	    else if (GetBit() == 0)    // Move 25 - Max
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
	       Dist = GetDist(GetBit());
	    }
	    break;
      }

      if (Dist < 0)	     // Do a move
      {
	 #ifdef DEBUG_MOVE
	    printf("Move-%d\n",Len);
	 #endif
	 str_ncpy(OutPtr, InPtr, Len);
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

/*-----------------------------------------------------------------------
    Copy up to n characters from one string to another

    CALL BY:	void str_ncpy(s1, s2, n)
		byte	 *s1;
		byte	 *s2;
		word n;

    ON EXIT:	nothing
 -----------------------------------------------------------------------*/
void CDECL str_ncpy(s1, s2, n)
    BYTE    *s1;
    BYTE    *s2;
    WORD    n;
{
    for (; n > 0; *s1++ = *s2++, n--);
}

#endif



#else

#include "compress.h"
unsigned Compress(UCHAR *OutBuf, UCHAR *InBuf, UCHAR *WorkBuf, UINT InLen)
{
	int	c_outlen	;
	c_outlen = ( InLen *3 ) / 5  ;
	memcpy ( OutBuf , InBuf , c_outlen ) ;
	return ( c_outlen ) ;
}

int   Compress_buff_size (int c_inbuff_size , int* rc_outbuff_size ,
					      int* rc_wkbuff_size ) 
{
	*rc_outbuff_size = c_inbuff_size ;
	*rc_wkbuff_size =             4  ;
}

#if 1111

#include	"expand.c"
#else
Conv_1Block ( char* p_in /* 32K + 2 */ , char* p_out /* 32K */)
{
	int	c_leng = *(unsigned short*)p_in & 0x7fff ;
	memcpy ( p_out , p_in , c_leng ) ;
}

#endif

#endif
