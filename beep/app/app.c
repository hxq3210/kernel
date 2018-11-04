#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "ioctl.h"

/*
   ./a.out sethz 300
   ./a.out stop
 */
int main(int argc, char **argv)
{
	int fd;
	int ret;
	int hz;

	if ((argc != 2) && (argc != 3)) {
		fprintf(stderr, "Usage: %s sethz  hz\n", argv[0]);
		fprintf(stderr, "Usage: %s stop\n", argv[0]);
		exit(1);
	}

	fd = open("/dev/beep", O_RDWR | O_NDELAY);
	assert(fd > 0);

	if (argc == 2) {
		if (!strncmp("stop", argv[1], 4)) {
			ret = ioctl(fd, BEEP_STOP);
			assert(ret == 0);
		} else {

			fprintf(stderr, "Usage: %s sethz  hz\n", argv[0]);
			fprintf(stderr, "Usage: %s stop\n", argv[0]);
			exit(1);

		}
	} else {
		if (!strncmp("sethz", argv[1], 5)) {
			hz = atoi(argv[2]);
			ret = ioctl(fd, BEEP_SET_HZ, hz);
			assert(ret == 0);
		} else {

			fprintf(stderr, "Usage: %s sethz  hz\n", argv[0]);
			fprintf(stderr, "Usage: %s stop\n", argv[0]);
			exit(1);
		}
	}

	close(fd);

	return 0;
}

