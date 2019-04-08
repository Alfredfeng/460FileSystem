int write_file()
{
	printf("Inside write_file()\n");
	return 1;
}

int mywrite(int fd, char buf[ ], int nbytes )
{
	//compute lbk
	OFT *oftp = running->fd[fd];
	MINODE *mip = oftp->minodePtr;
	int num_bytes = nbytes;
	int startByte;
	int remain;
	char wbuf[1024];
	char *cp;
	char *cq = buf;//cq points at buf[]

	//start the outer while
	while(num_bytes > 0)
	{
		int lbk = oftp->offset/BLKSIZE;
		int startByte = oftp->offset % BLKSIZE;
		int blk = map(mip,lbk);//compute physical block number blk

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