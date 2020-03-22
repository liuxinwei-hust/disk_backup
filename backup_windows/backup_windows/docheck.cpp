#include "stdafx.h"
#include "CheckProgress.h"
#include "backupinfo.h"
#include "winioctl.h"

extern CString checkfilepath;
extern void Init_CRC32_Table();
extern int Expand(unsigned char *OutBuf, unsigned char *InBuf, unsigned short int *Offset);
extern BOOL g_bchCancel;
extern int Get_CRC(BYTE*  buffer,DWORD dwSize);


#define WM_CHECKUPDATE (WM_USER + 103)

BOOL ReadPacket2(DataPacketHead packet,unsigned char *buf, FILE * pfile)
{
        if(packet.CompressType==0)
        {
                if(fread(buf,packet.DataSize,1,pfile)==0)
                {
                      return FALSE;
                }
				if(packet.CRCValue!=(DWORD)Get_CRC(buf,packet.DataSize))
				{
					 MessageBox(NULL,"CRC检验出错，建议重新备份","提示",MB_OK);
					 return FALSE;
				}
                return TRUE;
        }
        else if(packet.CompressType == 1)
        {
			    BYTE pCompress_Buf1[136312];
				BYTE pCompress_Buf2[136312];
                if(fread(pCompress_Buf1,packet.DataSize,1,pfile)==0)
                {
                    return FALSE;                     
                }
                Expand(buf,pCompress_Buf1,(WORD *)pCompress_Buf2);
				if(packet.CRCValue!=(DWORD)Get_CRC(buf,packet.Sectors*512))
				{
					MessageBox(NULL,"CRC检验出错，建议重新备份","提示",MB_OK);
					return FALSE;
				}
				return TRUE;
        }

        return FALSE;
}





int do_check(CCheckProgress *progressdlg)
{
	FILE *pfile;
	Header headtem;
	CString temp;
	UINT64 totalsector;
	UINT64 cursector=0;//当前检查扇区
	temp.Format("%s.hdz",checkfilepath);
	if(fopen_s(&pfile,temp,"rb+"))
	{
		MessageBox(NULL,"打开文件失败","提示",MB_OK);
		goto err;
	}
    fread(&headtem,sizeof(Header),1,pfile);
    fclose(pfile);
	Init_CRC32_Table();
	int filecount=headtem.wDataFileCount;
	totalsector=headtem.TotalUsedSectors;//需还原的扇区总数
	//包大小
	DataPacketHead  packet;
	int packetsize;
    packetsize=sizeof(packet);
 //起始时间
	time_t start_time,end_time;
	double timediff1=0.0;
	time(&start_time);
	//检查开始
	for(int i=0;i<filecount;i++)
	{
         HDBTag  tag;//文件末尾信息
		if(i==0)
		{
			long offset;
			temp.Format("%s.hdz",checkfilepath);
			if(fopen_s(&pfile,temp,"rb+"))
			{
				MessageBox(NULL,"打开文件失败","提示",MB_OK);
				goto err;
			}
			if(filecount==1)
			{
				tag.packetnum=headtem.packetnum;
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
			temp.Format("%s_%d.hdz",checkfilepath,i);
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
			if(ReadPacket2(packet,buf,pfile)==FALSE)
			{
				free(buf);
				goto err;
			}	
			if(g_bchCancel)
			{
				free(buf);
				goto cancel;
			}
			cursector+=packet.Sectors;
			
			percentage=(int)(cursector*100/totalsector);
			progressdlg->m_percentage.Format("%d%%",percentage);
			progressdlg->m_progress.SetPos(percentage);
			progressdlg->m_filepath=temp;

			time(&end_time);
			timdiff=difftime(end_time,start_time);
			if(timdiff-timediff1>=1)//每一秒更新一次数据
			{
				timediff1=timdiff;
				timdiff=timdiff*(totalsector-cursector)/cursector;				    
				leftmin=(int)timdiff/60;
				lefthour=leftmin/60;
				leftmin=leftmin % 60;
				sec= (int)timdiff % 60;
				progressdlg->m_lefttime.Format("%d小时%d分%d秒",lefthour,leftmin,sec);
			}
			progressdlg->SendMessage(WM_CHECKUPDATE,0);	
		}
		free(buf);
		fclose(pfile);
		pfile=NULL;
	}
	if(pfile)fclose(pfile);
	return 0;
err:
	if(pfile)fclose(pfile);
	return 1;
cancel:
	if(pfile)fclose(pfile);
	return 2;
}