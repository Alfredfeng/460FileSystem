/********write.c file***********/
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

//system call write for regular files
int write_file()
{
	int fd;
	OFT *oftp;
	char write_buf[1024];//the buf
	int nbytes;
	fd = atoi(pathname);
	oftp = running->fd[fd];
	printf("fd=%d\n",fd);

	if(!oftp || ( oftp->mode != 1 && oftp->mode != 3) )
	{
		printf("Error: the file is not opened for write\n");
		return -1;
	}
	//the file is opened for write

	printf("Enter contents to write>");
	gets(write_buf);
	//buf[strlen(buf)] = 0;//kill the last '\n';
	printf("write_buf=%s\n",write_buf);
	nbytes = strlen(write_buf);
	mywrite(fd,write_buf,nbytes);

	return 1;
}

//write_file() in the textbook
int mywrite(int fd, char buf[ ], int nbytes )
{
	//compute lbk
	OFT *oftp = running->fd[fd];
	MINODE *mip = oftp->mptr;
	INODE *ip = &mip->INODE;

	int dev = mip->dev;
	int startByte;
	int remain;
	int num_bytes = nbytes;
	int buf12[1024];//block for b12
	char direct_buf[256];
	char ibuf[256];//buffer for indirect block
	char dbuf[256];//buffer for double indirect block

	char wbuf[1024];//wbuf

	char *cp;
	char *cq = buf;//cq points at buf[]
	int lbk;
	int blk;//physical block number
	int count = 0;

	//start the outer while
	while(num_bytes > 0)
	{
		//compute logical block number and the startByte in that lbk

		lbk = oftp->offset/BLKSIZE;
		startByte = oftp->offset % BLKSIZE;

		//int blk = map(mip,lbk);//compute physical block number blk
		if(lbk < 12)
		{
			if(ip->i_block[lbk] == 0)
			{
				//if no data block yet
				printf("ip->i_block[%d}==0\n",lbk);
				ip->i_block[lbk] = balloc(mip->dev);//allocate a new data block
			}

			blk = ip->i_block[lbk]; //blk should be a disk block now
		}
		else if( lbk >= 12 && lbk < 256 + 12) //indirect blocks
		{
			if(mip->INODE.i_block[12] == 0)
			{
				int b12;
				b12 = mip->INODE.i_block[12] = balloc(mip->dev); //allocate a block for it;
				if(b12 == 0)
				{
					printf("no more data blocks\n");
					return 0;
				}
				get_block(mip->INODE.i_block[12],buf12);//get the content into buf12
				int *up = (int*)buf12;
				//zero out the blocks
				for( int j = 0 ; j < 256 ;j ++)
				{
					up[j] = 0;
				}
				put_block(mip->INODE.i_block[12],b12);//put the content back
			}
			//how to get i_block[12]  into an int ibuf[256]??
		}
		else
		{
			//double indirect blocks
			lbk -= (12+256);
			int dblk = dbuf[lbk/256];
			get_block(dev,dblk,dbuf);
			blk = dbuf[lbk%256];
		}

		get_block(dev, blk, wbuf); // write wbuf[] to disk
		cp = wbuf + startByte;
		remain = BLKSIZE - startByte; //num of bytes remaining in this block

		while(remain > 0)
		{
			*cp++ = *cq++;
			num_bytes--; remain--;
			count ++;
			oftp->offset++;
			if(oftp->offset > mip->INODE.i_size)
				mip->INODE.i_size++;
			if(num_bytes <= 0)
				break;
		}
		put_block(mip->dev, blk, wbuf);
		//loop back to outer while to write more ... until nbytes are written
	}

	mip->dirty = 1; //mark mip dirty for iput()
	// printf("wrote %d char into file descriptor fd=%d\n",count, fd);
	return nbytes;
}