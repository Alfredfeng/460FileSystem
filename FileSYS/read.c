/********read.c file***********/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

#include "type.h"

extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC   proc[NPROC], *running;
extern OFT    oft[NFD];

extern char gpath[256];
extern char *name[64]; // assume at most 64 components in pathnames
extern int  n;
extern int  fd, dev;
extern int  nblocks, ninodes, bmap, imap, inode_start;
extern char pathname[256], parameter[256];

int read_file()
{
	int fd; int nbytes;
	char buf[1024];//buf for read
	printf("Enter the opened fd>");
	scanf("%d",&fd);
	printf("Enter the number of bytes to read>");
	scanf("%d",&nbytes);
	myread(fd,buf,nbytes);
	return 0;
}

int myread(int fd, char *buf, int nbytes)
{
	int count = 0;	int startByte;
	int lbk;// logical block number
	int blk;//physical block number
	int avil;
	int offset;
	char *cq = buf; //cq points at buf[]
	int ibuf[256];//ibuf for indirect block
	int dbuf[256];//dbuf for double indirect block
	char readbuf[1024];

	OFT *oftp = running->fd[fd];
	MINODE *mip = oftp->mptr;
	INODE *ip = &mip->INODE; //get the inode address
	offset = oftp->offset;//update offset
	avil = ip->i_size - offset;//set avil
	int dev = mip->dev;//dev number
	//2.
	while( nbytes > 0 && avil > 0)
	{
		//compute logical block number lbk and start byte in that block from offse
		lbk = oftp->offset /BLKSIZE; //initialize lbk number
		startByte = oftp->offset % BLKSIZE;

		if(lbk < 12)
		{
			blk = mip->INODE.i_block[lbk];//map LOGICAL lbk to Physical blk
		}
		else if (lbk >= 12 && lbk < 256 + 12)//indirect blocks
		{
			get_block(dev,ip->i_block[12],ibuf);//get ibuf content
			blk = ibuf[lbk-12];//get the blk number
		}
		else//double indirect blocks
		{
			lbk -= (12+256);
			int dblk = dbuf[lbk/256];
			get_block(dev,dblk,dbuf);
			blk = dbuf[lbk%256];
		}
		//get the data block into readbuf[BLKSIZE]
		get_block(dev, blk, readbuf);
		//copy from  startByte to buf[], at most remain bytes in this block
		char *cp = readbuf + startByte;//number of bytes remaining in readbuf[]
		int remain = BLKSIZE - startByte;
		while(remain > 0)
		{
			*cq ++ = *cp ++;//copy byte from readbuf[] into buf[]
			oftp->offset++;
			count++;
			avil--; nbytes--; remain--;
			if(nbytes <= 0 || avil <=0)
				break;
		}//end inner while

		//??: if one data block is not enough, loop back to outer while for more

	}//end outer while
	// printf("myread: read %d char from file descriptor %d\n", count ,fd);
	//printf("readbuf=%s\n",readbuf);
	return count;
}

int cat_file()
{
	printf("inside cat_file()\n");
	char mybuf[1024], dummy = 0; // a null char at the end of mybuf[]
	int n;
	int fd; // = open file name for READ
	printf("Enter fd to cat>");
	scanf("%d",&fd);
	printf("fd=%d\n",fd);
	while( n = myread(fd, mybuf, 1000))
	{
		 mybuf[n] = 0; // as a null-terminated string
		 printf("%s",mybuf);//not good
		//spit out chars from mybuf[] but handle '\n' properly
	}
	//close(fd);//close the file descriptor
}