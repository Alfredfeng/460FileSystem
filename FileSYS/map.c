/********map.c file***********/
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

u32 map(MINODE *mip,int lbk)
{
	//algorithm to convert lbk to blk that is the physical block number
	int dev = mip->dev;
	int dblk;
	char ibuf[256]; char dbuf[256];
	int blk;
	if(lbk < 12)
	{
		blk = mip->INODE.i_block[lbk];
	}
	else if ( lbk >= 12 && lbk < 12+256)
	{
		//indirect blocks
		//read inode->i_block[12] into u32 ibuf[256];
		get_block(dev,mip->INODE.i_block[12],ibuf);
		blk = ibuf[lbk-12];
	}
	else
	{
		//read INODE.i_block[13] into u32 dbuf[256];
		get_block(dev,mip->INODE.i_block[13],dbuf);
		lbk -= 12+256;
		dblk = dbuf[lbk/256];
		get_block(dev,dblk,dbuf);
		blk = dbuf[lbk %256];
	}
	return blk;
}