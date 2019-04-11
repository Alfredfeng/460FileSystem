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
	char buf[1024];//the buf
	int nbytes;
	printf("Enter the fd>");
	scanf("%d",&fd);
	if(!running->fd[fd] || running->fd[fd]->mode != 1 || running->fd[fd]->mode != 3)
	{
		printf("Error: the file is not opened for write\n");
		return -1;
	}
	//the file is opened for write
	oftp = running->fd[fd];
	printf("Enter contents to write>");
	gets(buf);
	buf[strlen(buf)] = 0;//kill the last '\n';
	nbytes = strlen(buf);
	mywrite(fd,buf,nbytes);

	return 1;
}

//write_file() in the textbook
int mywrite(int fd, char buf[ ], int nbytes )
{
	//compute lbk
	OFT *oftp = running->fd[fd];
	MINODE *mip = oftp->mptr;
	int num_bytes = nbytes;
	int startByte;
	int remain;
	char wbuf[1024]; //what is this wbuf used for?
	strcpy(wbuf,buf);
	char *cp;
	char *cq = wbuf;//cq points at buf[]
	int blk;//physical block number

	//start the outer while
	while(num_bytes > 0)
	{
		//compute logical block number and the startByte in that lbk

		int lbk = oftp->offset/BLKSIZE;
		int startByte = oftp->offset % BLKSIZE;

		//int blk = map(mip,lbk);//compute physical block number blk
		if(lbk < 12)
		{
			if(mip->INODE.i_block[lbk] == 0)
			{
				//if no data block yet
				mip->INODE.i_block[lbk] = balloc(mip->dev);//allocate a new data block
			}
		}
		else if( lbk >= 12 && lbk < 256 + 12)
		{
			//indirect blocks
			if(mip->INODE.i_block[12] == 0)
			{
				mip->INODE.i_block[12] = balloc(mip->dev); //allocate a block for it;
				// how to zero out the blocks on disk??
			}
			char ibuf[256];
			//how to get i_block[12]  into an int ibuf[256]??
		}
		else
		{
			//double indirect blocks
		}

		get_block(mip->dev, blk, wbuf); // write wbuf[] to disk
		cp = wbuf + startByte;
		remain = BLKSIZE - startByte; //num of bytes remaining in this block

		while(remain > 0)
		{
			*cp++ = *cq++;
			num_bytes--; remain--;
			oftp->offset++;
			if(oftp->offset > mip->INODE.i_size)
				mip->INODE.i_size++;
			if(nbytes <= 0)
				break;
		}
		put_block(mip->dev, blk, wbuf);
		//loop back to outer while to write more ... until nbytes are written
	}

	mip->dirty = 1; //mark mip dirty for iput()
	printf("wrote %d char into file descriptor fd=%d\n",num_bytes, fd);
	return num_bytes;
}