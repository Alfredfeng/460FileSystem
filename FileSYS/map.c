u32 map(MINODE *mip,int lbk)
{
	//algorithm to convert lbk to blk that is the physical block number
	int dev = mip->dev;
	char ibuf[256]; char dbuf[256];
	int blk;
	if(lbk < 12)
	{
		blk = inode->i_block[lbk];
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