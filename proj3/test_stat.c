
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/param.h>
#include <sys/vnode.h>
#include <sys/fs/ufs_fs.h>
#include <sys/fs/ufs_inode.h>
#include <time.h>
#include <errno.h>
extern int errno;

// Call this program with a file name as the argument
main(argc, argv)
int argc; 
char *argv[];
{
  char cmd[80];
  long ltime=1;
  struct stat buf;
  int fd;
  mode_t s_m;
  time_t tm;
  time_t cm;
  unsigned long ftype;

	if (argc != 2) {printf ("usage: File_Attr filename \n"); exit(0);}

/* "ls -i file" prints out the inode of a file */
	sprintf(cmd, "ls -i %s", argv[1]);
	printf ("cmd=  %s\n", cmd);
	system (cmd);  

/* Print current time */
	time(&ltime);//time in seconds since Jan. 1, 1970
	printf("current time: %22.20s",       ctime(&ltime));
	printf("current time in seconds since epoch= %ld\n",ltime);

/* Open the file to test if the file indeed exists before
reading its "stat" structure. */
/* To see what are the fields in the inode, see the file 
/usr/include/sys/fs/ufs_inode.h */

/* using lstat() funtcion; for lstat() to be able find the file,
argv[1] should be either complete pathname of a file, or it must
be present in the stated directory. For example, if you want to
use file "X" in the parent directory, argv[1] = "../X". */
	lstat(argv[1], &buf);
	ftype = buf.st_mode & 0xF000;
	printf("ftype=%x\n", ftype);
	if (ftype == S_IFLNK) printf ("link file\n");

/* using fstat() */
	if ((fd = open (argv[1], O_RDWR)) == -1)
	{
	perror("open problem");
	printf ("error=%d \n", errno);
	exit (-1);
	}
	if (fstat(fd, &buf) < 0) 
	   {perror ("fstat problem"); exit (-1);}

	tm = buf.st_atime; //time of last access
	printf("time in seconds since last access = %ld\n", (ltime - tm));

	ftype = buf.st_mode & 0xF000;
	if (ftype == S_IFREG) printf ("reg file\n");
	//if (S_ISREG(buf.st_mode)) - another way to test 
	else printf ("not reg file\n");
	printf("ftype=%x\n", ftype);

	printf (" inode num=%d \n", buf.st_ino);

	/* Testing permission bits */
	s_m = buf.st_mode & S_IEXEC;
	if (s_m) printf ("exec permission is granted to the owner \n");
	else printf("execute perminssion not granted \n");

	close(fd);

}
