#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "flash_ioctl.h"

int main(int argc, char** argv)
{
	int fd;

	fd = open("/dev/norflsh0", O_RDWR);
	if(fd < 0)
	{
		fprintf(stderr,"Error openning the flash device.\n");
		return -1;
	}

	ioctl(fd, ERASE_FLASH);

	close(fd);

	return 0;
}
