int read_file()
{
	printf("inside read_file()\n");
	return 1;
}

int myread(int fd, char *buf, int nbytes)
{
	int count = 0;
	int blk;
	int avil;//avil = fileSize - OFT's offset
	char *cq = buf; //cq points at buf[]
	char readbuf[1024];//readbuf

	int lbk; int startByte;
	OFT *oftp = running->fd[fd];
	MINODE *mip = oftp->minodePtr;

	//2.
	while( nbytes && avil)
	{
		//compute logical block number lbk and start byte in that block from offse
		lbk = oftp->offset /BLKSIZE;
		startByte = oftp->offset % BLKSIZE;

		if(lbk < 12)
		{
			//lbk is a direct block
			blk = mip->INODE.i_block[lbk];//map LOGICAL lbk to Physical blk
		}
		else if (lbk >= 12 && lbk < 256 + 12)
		{
			//indirect blocks
		}
		else
		{
			//double indirect blocks
		}
		//get the data block into readbuf[BLKSIZE]
		get_block(mip->dev, blk, readbuf);
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

		//if one data block is not enough, loop back to outer while for mor

	}//end outer while
	printf("myread: read %d char from file descriptor %d\n", count ,fd);
	return count;
}

int cat_file()
{
	printf("inside cat_file()\n");
	char mybuf[1024], dummy = 0; // a null char at the end of mybuf[]
	int n;
	int fd; // = open file name for READ
	while( n = read(fd, mybuf[1024], 1024))
	{
		mybuf[n] = 0; // as a null-terminated string
		// printf("%s",mybuf);//not good
		//spit out chars from mybuf[] but handle '\n' properly
	}
	close(fd);//close the file descriptor
}