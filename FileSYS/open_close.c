//implementation of open
char mode[10];

int open_helper( char *filename, int flags);

/*
int truncate(MINODE *mip)
{
	printf("Inside truncate()\n");
	
	// 1. release mip->INODE's data blocks;
	// a file may have 12 direct blocks, 256 indirect blocks and 256*256
	// double indirect data blocks. release them all.
	// 2. update INODE's time field

	// 3. set INODE's size to 0 and mark Minode[ ] dirty

}
*/


int open_file()
{
	int i;
	char filename[256];

	printf("Enter the filename to open>");
	fgets(filename,256,stdin);
	//kill the last '\n'
	for(int i = 0 ; i < 256 ; i ++ )
	{
		if(filename[i] = '\n');
		filename[i] = 0;
	}
	printf("Enter the mode to open>");
	scanf("%d",&i);
	open_helper(filename,i);
}

/// helper function for open_file: filename is the name of the file to open; flag is R/W/APPEND/RW
int open_helper( char *filename, int flags)
{
	int i = 0;
	int ino;
	int mode = flags;
	OFT *oftp;

	char _pathname[64];
	strcpy(_pathname,filename);
	MINODE *mip;//minode pointer
	//1. ask for a pathname and mode to open:
	printf("pathname=%s\n",filename);

	printf("The mode to open=%d\n",mode);
	
	//2. get pathname's inumber:
	if(pathname[0] == '/')
	{
		dev = root->dev; //root INODE's dev
		printf("root->dev=%d\n",dev);
	}
	else
	{
		dev = running->cwd->dev;
		printf("running->cwd->dev=%d\n",dev);
	}

	ino = getino(_pathname);//seg fault here
	printf("ino=%d\n",ino);
	//3. get its Minode pointer
	mip = iget(dev,ino);
	//4. check mip->INODE.i_mode
	int result = mip->INODE.i_mode & 0XF000;
	printf("result=%x\n",result);

	//for now we ignore permission bits and compatibility of the mode
	if(result != 0x8000)
	{
		printf("Not a regular file");
		return -1;
	}

	//5. allocate a FREE OpenFileTable and fill in values:
	for(i = 0 ; i < NOFT ; i ++)
	{
		if(oft[i].refCount == 0)
		{
			printf("Found a free OpenFileTable: oft[%d]\n",i);
			running->fd[i] = &oft[i];//set the address to the fd pointer
			running->fd[i]->mode = mode;//set mode
			running->fd[i]->refCount = 1;//set refCount
			running->fd[i]->minodePtr = mip;//set minnode pointer
			//set oftp pointer
			oftp = running->fd[i];
			break;
		}
	}
	//6.Depending on the open mode 0|1|2|3, set the OFT's offset accordingly:
	switch(mode)
	{
		case 0 : 
				oftp->offset = 0;     // R: offset = 0
				break;
		case 1 :
				truncate(mip);        // W: truncate file to 0 size
				oftp->offset = 0;
				break;
		case 2 :
				oftp->offset = 0;     // RW: do NOT truncate file
				break;
		case 3 :
				oftp->offset =  mip->INODE.i_size;  // APPEND mode
				break;
		default:
				printf("invalid mode\n");
				return(-1);
	}
	//8. 
	running->fd[i]->minodePtr->dirty = 1;//mark minode as dirty
	/*
	update INODE's time field
         for R: touch atime. 
         for W|RW|APPEND mode : touch atime and mtime
      mark Minode[ ] dirty
    */
	//9. return i as the file descriptor
	return i;
}

int close_file( int fd)
{
	MINODE *mip;
	printf("Closing fd[%d]\n",fd);
	//1. verify fd is within range
	if(fd < 0 || fd > NOFT)
	{
		printf("Error: fd is out of range.\n");
		return -1;
	}

	//2. verify running->fd[fd] is pointing at an OFT entry
	if(!running->fd[fd])
	{
		printf("Error: running->fd[fd] is NULL\n");
		return -1;
	}

	//3. The following code segments should be obvious:
	OFT *oftp = running->fd[fd];
	running->fd[fd] = 0;

	oftp->refCount--;
	if (oftp->refCount > 0) 
		return 0;

	// last user of this OFT entry ==> dispose of the Minode[]
     mip = oftp->minodePtr;
     iput(mip);

     return 0; 
}

int pfd()
{
	//display file discriptors
	int dev; int ino;

	printf(" fd    mode    offset    [dev,ino]\n");
	//how to comppute the current number of opened fd's?
	for(int i = 0 ; i < 10 ; i ++ )
	{
		//dev = running->fd[i]->minodePtr->dev;
		//ino = running->fd[i]->minodePtr->ino;
		//setMode(i);
		//printf("%d    %s    0    [%d,%d]\n",i,mode[i],dev,ino);
	}

}

void setMode(int i)
{
	
	switch(i)
	{
		case 0: strcpy(mode,"READ"); break;
		case 1: strcpy(mode,"WRITE"); break;
		case 2: strcpy(mode, "RDWR"); break;
		case 3: strcpy(mode, "APPEND"); break;
	}
}

int lseek_file()
{
	printf("lseeking\n");
}

int lseek_helper(int fd, int position)
{
	/*
		From fd, find the OFT entry. 

		change OFT entry's offset to position but make sure NOT to over run either end
		of the file.

		return originalPosition
	*/
}

int dup(int fd)
{
	printf("inside dup()\n");
  // verify fd is an opened descriptor;
  // duplicates (copy) fd[fd] into FIRST empty fd[ ] slot;
  // increment OFT's refCount by 1;
}

int dup2(int fd, int gd)
{
	printf("inside dup2()\n");
  // CLOSE gd fisrt if it's already opened;
  // duplicates fd[fd] into fd[gd]; 
}


