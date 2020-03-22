
#define UINT  unsigned int
#define WORD  unsigned short int
#define UCHAR unsigned char
#define ULONG unsigned long

/* Define SEC_ALLOCATE if min. allocation size is a sector (usually 512
// bytes).	The compressor will stop attempting to compress data when
// less than 1 sector can be saved.					*/


#define CLUSTER_SIZE   32768
#define SEC_SIZE	 512
#define HASHSIZE	8192
#define HASHDEPTH	   2

#define WORKBUF_SIZE ((HASHSIZE * HASHDEPTH * 2) + (CLUSTER_SIZE * 2))

#define EXIT_OK		   0
#define EXIT_FAILED	   1

unsigned Compress(UCHAR *OutBuf, UCHAR *InBuf, UCHAR *WorkBuf, UINT InLen);
int Expand(unsigned char *OutBuf, unsigned char *InBuf, unsigned short int *Offset);
