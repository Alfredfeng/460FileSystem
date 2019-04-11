/********cp_mv.c file***********/
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

int cp_file()
{
	char name[25];
	char name2[25];
	char cp_buf[1024];
	int n;
	int fd = 0;//fd for read
	int gd = 1;//fd for write
	printf("Enter the file name to copy from>");
	gets(name);
	open_helper(name,fd);
	printf("Enter the file name to copy to>");
	gets(name2);
	open_helper(name2,gd);//open for write
	pfd();


	while( n = myread(fd,cp_buf,1024))
	{
		mywrite(gd,cp_buf,n);
	}
	printf("copy successful\n");
	getchar();
	close_file(fd);
	close_file(gd);
	return 0;
}

int mv_file()
{
	printf("inside mv_file()\n");
	return 1;
}