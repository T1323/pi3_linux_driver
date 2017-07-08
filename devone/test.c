
/*
 *
 * 
 * test program for devone
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


#define DEVFILE "/dev/devone0"

int main(void)
{
	int fd, ret;
	unsigned char buf[4];
	
	fd = open(DEVFILE, O_RDWR);
	
	if (fd == -1) {
		perror("open");
		return -1;
	}
	
	ret = read(fd, buf, 1);
	printf("read result %d len:%d\n", buf[0], ret);
	
	buf[0] = 2;
	ret = write(fd, buf, 1);
	printf("write 2%d\n", ret);
	
	sleep(1);
	buf[0] = 1;
	ret = write(fd, buf, 1);
	printf("write 1  %d\n", ret);
	
	sleep(1);
	buf[0] = 3;
	ret = write(fd, buf, 1);
	printf("write 3%d\n", ret);
	
	sleep(1);
	ret = read(fd, buf, 1);
	printf("read result %d len:%d\n", buf[0], ret);
	
	close(fd);
	
	return 0;
}

