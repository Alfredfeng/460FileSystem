//implementation of open
char mode[10];

int open_file()
{
	int i = 0;
	int ino;
	int mode = atoi(parameter);
	MINODE *mip;//minode pointer
	//char _pathname[256];
	//1. ask for a pathname and mode to open:
	printf("pathname=%s\n",pathname);
	//strcpy(_pathname,pathname);
	//printf("addr of _pathname=%x\n",_pathname);
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
	getchar();

	ino = getino(pathname);//seg fault here
	printf("ino=%d\n",ino);
	//3. get its Minode pointer
	mip = iget(dev,ino);
	//4. check mip->INODE.i_mode
	int result = mip->INODE.i_mode & 0xFFFF;
	printf("result=%x\n",result);

	//for now we suppose that the permission is ok because I don't know how to check permission bits based on the last 9 bits in i_mode
	if(result == 0x8000)
	{
		//check if it is in a incompatible mode
	}

	//5. allocate a FREE OpenFileTable and fill in values:
	return 1;
	//return i as the file descriptor
	
}

int close_file( int fd)
{
	printf("Closing fd[%d]\n",fd);
	//1. verify fd is within range

	//2. verify running->fd[fd] is pointing at an OFT entry

	//3. The following code segments should be obvious:
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


